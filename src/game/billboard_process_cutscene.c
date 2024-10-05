#include <PR/ultratypes.h>
#include <PR/gbi.h>

#include "sm64.h"
#include "actors/sign_videodata/geo_header.h"
#include "dma_data/dma_data.h"
#include "behavior_data.h"
#include "audio/external.h"
#include "engine/graph_node.h"
#include "area.h"
#include "debug.h"
#include "game_init.h"
#include "ingame_menu.h"
#include "level_update.h"
#include "mtwister.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "print.h"
#include "rendering_graph_node.h"
#include "save_file.h"
#include "segment2.h"
#include "segment7.h"
#include "spawn_object.h"
#include "boot/slidec.h"
#ifdef PUPPYPRINT
#include "puppyprint.h"
#endif

#define GAME_FRAMERATE 30

#define TEXTURE_WIDTH  256
#define TEXTURE_HEIGHT 128

// NOTE: This has acceptable alignment, but should otherwise be taken into consideration with DMA usage.
#define TEXTURE_SIZE (TEXTURE_WIDTH * TEXTURE_HEIGHT * sizeof(RGBA16))
#define ALIGNED_BUFFER_SIZE ALIGN16(TEXTURE_SIZE)

#define TEXTURE_COUNT ((s32) ((TEXTURE_SIZE - 1) / 0x1000) + 1)

enum BillboardType {
    BB_TYPE_IMAGE,
    BB_TYPE_VIDEO,
};

enum VideoDMAIDs {
    BB_VIDEO_COURSE1,

    BB_VIDEO_COUNT,
};

enum VideoSafetyStates {
    VIDEO_SAFETY_UNALLOCATED,
    VIDEO_SAFETY_UNSAFE,
    VIDEO_SAFETY_ACTIVE_DMA,
    VIDEO_SAFETY_SAFE,
};

struct DMAImageProperties {
    const struct DMAVideoProps *addr;
    u32 modelId;
    u32 relativeLoopStart;
    u32 frameTotal;
    u32 framerate;
    u32 startFrame;
};

struct DMAImageProperties dmaProps[BB_VIDEO_COUNT] = {
    [BB_VIDEO_COURSE1] = {.addr = course1_video_data, .modelId = MODEL_SIGN_BSM_COURSE_1, .relativeLoopStart = 0, .frameTotal = ARRAY_COUNT(course1_video_data), .startFrame = 0, .framerate = 30},
};

OSIoMesg videoImageDMAIoMesg[TEXTURE_COUNT];
OSMesg videoImageDMAReceivedMesg[TEXTURE_COUNT];
OSMesgQueue videoImageDMAQueue;

u8 gSafeToLoadVideo = VIDEO_SAFETY_UNALLOCATED;
u8 sTripleBufferIndex = 0;
u8 safeBufferIndex = 0;
u8 hasInitializedMessageQueue = FALSE;
s32 sImageDMACount = 0;

Texture *dmaTextureAddrs[3];
Texture *dmaYAY0Addrs[3];
struct DMAVideoProps *videoDataProps[BB_VIDEO_COUNT];

u32 imageGameFrame = 0;
u32 imageVideoFrame = 0;
s32 gVideoIndex = -1;

static void dma_read_image_noblock(u8 *dest, u8 *srcStart, u8 *srcEnd) {
    u32 size = ALIGN16(srcEnd - srcStart);

    osInvalDCache(dest, size);
    while (size != 0) {
        u32 copySize = (size >= 0x1000) ? 0x1000 : size;

        osPiStartDma(&videoImageDMAIoMesg[sImageDMACount++], OS_MESG_PRI_NORMAL, OS_READ, (uintptr_t) srcStart, dest, copySize,
                     &videoImageDMAQueue);

        dest += copySize;
        srcStart += copySize;
        size -= copySize;
    }

    assert(sImageDMACount <= TEXTURE_COUNT, "sImageDMACount too large!");
}

static void dma_read_block(u8 *dest, u8 *srcStart, u8 *srcEnd) {
    u32 size = ALIGN16(srcEnd - srcStart);

    osInvalDCache(dest, size);
    while (size != 0) {
        u32 copySize = (size >= 0x1000) ? 0x1000 : size;

        osPiStartDma(&videoImageDMAIoMesg[sImageDMACount], OS_MESG_PRI_NORMAL, OS_READ, (uintptr_t) srcStart, dest, copySize,
                     &videoImageDMAQueue);
        osRecvMesg(&videoImageDMAQueue, &videoImageDMAReceivedMesg[sImageDMACount], OS_MESG_BLOCK);

        dest += copySize;
        srcStart += copySize;
        size -= copySize;
    }
}

static void dma_video_frame(struct DMAVideoProps *dmaProps) {
    Texture *relativeAddr = (Texture *) ((u32) dmaProps->address);
    Texture *dest = dmaYAY0Addrs[sTripleBufferIndex];
    sTripleBufferIndex = (sTripleBufferIndex + 1) % ARRAY_COUNT(dmaYAY0Addrs);

    assert(dmaProps->compressedSize < ALIGNED_BUFFER_SIZE, "Compressed YAY0 larger than source!");

    dma_read_image_noblock(dest, relativeAddr, (u8 *) ((size_t) relativeAddr + dmaProps->compressedSize));
}

static void add_menu_frame(void) {
    if (gVideoIndex >= 0 && gVideoIndex < BB_VIDEO_COUNT) {
        imageGameFrame++;
        imageVideoFrame = imageGameFrame * dmaProps[gVideoIndex].framerate / GAME_FRAMERATE;
        if (imageVideoFrame >= dmaProps[gVideoIndex].frameTotal) {
            imageGameFrame = dmaProps[gVideoIndex].relativeLoopStart * GAME_FRAMERATE / dmaProps[gVideoIndex].framerate;
            imageVideoFrame = imageGameFrame * dmaProps[gVideoIndex].framerate / GAME_FRAMERATE;
        }
    }
}

void init_menu_video_buffers(void) {
    gSafeToLoadVideo = VIDEO_SAFETY_UNALLOCATED;
    imageGameFrame = 0;
    imageVideoFrame = 0;
    gVideoIndex = -1;
    sTripleBufferIndex = 0;
    safeBufferIndex = 0;

    u8 *texturememaddr = main_pool_alloc(ALIGNED_BUFFER_SIZE * ARRAY_COUNT(dmaTextureAddrs), MEMORY_POOL_LEFT);
    if (texturememaddr == NULL) {
        assert(FALSE, "Out of memory! :(");
        return;
    }

    u8 *yay0memaddr = main_pool_alloc(ALIGNED_BUFFER_SIZE * ARRAY_COUNT(dmaYAY0Addrs), MEMORY_POOL_LEFT);
    if (yay0memaddr == NULL) {
        assert(FALSE, "Out of memory! :(");
        return;
    }

    if (!hasInitializedMessageQueue) {
        hasInitializedMessageQueue = TRUE;
        osCreateMesgQueue(&videoImageDMAQueue, videoImageDMAReceivedMesg,
                        ARRAY_COUNT(videoImageDMAReceivedMesg));
    }

    for (s32 i = 0; i < ARRAY_COUNT(videoDataProps); i++) {
        uint32_t dataSize = (dmaProps[i].frameTotal * sizeof(struct DMAVideoProps));
        u8 *propData = main_pool_alloc(ALIGN16(dataSize), MEMORY_POOL_LEFT);
        if (propData == NULL) {
            assert(FALSE, "Out of memory! :(");
            return;
        }

        videoDataProps[i] = (struct DMAVideoProps*) propData;

        dma_read_block(propData, (u8 *) dmaProps[i].addr, (u8 *) dmaProps[i].addr + (size_t) ALIGN16(dataSize));
    }

    gSafeToLoadVideo = VIDEO_SAFETY_UNSAFE;

    for (s32 i = 0; i < ARRAY_COUNT(dmaTextureAddrs); i++) {
        dmaTextureAddrs[i] = &texturememaddr[ALIGNED_BUFFER_SIZE * i];
        dmaYAY0Addrs[i] = &yay0memaddr[ALIGNED_BUFFER_SIZE * i];
    }
}

void update_menu_video_buffers(void) {
    if (gSafeToLoadVideo == VIDEO_SAFETY_UNALLOCATED) {
        return;
    }

    if (gVideoIndex < 0 || gVideoIndex >= BB_VIDEO_COUNT || find_first_object_with_behavior_and_bparams(bhvDesertSign, BB_TYPE_VIDEO << 16, 0xFF << 16) == NULL) {
        gSafeToLoadVideo = VIDEO_SAFETY_UNSAFE;
        imageGameFrame = 0;
        imageVideoFrame = 0;
        gVideoIndex = -1;
        return;
    }

    if (!check_image_dma_complete()) {
        if (gSafeToLoadVideo != VIDEO_SAFETY_UNSAFE) {
            add_menu_frame();
        }
        return;
    }

    if (gSafeToLoadVideo == VIDEO_SAFETY_ACTIVE_DMA && safeBufferIndex == sTripleBufferIndex) {
        gSafeToLoadVideo = VIDEO_SAFETY_SAFE;
    }

    if (gSafeToLoadVideo == VIDEO_SAFETY_UNSAFE) {
        if (gVideoIndex >= 0 && gVideoIndex < BB_VIDEO_COUNT) {
            imageGameFrame = dmaProps[gVideoIndex].startFrame * GAME_FRAMERATE / dmaProps[gVideoIndex].framerate;
            imageVideoFrame = imageGameFrame * dmaProps[gVideoIndex].framerate / GAME_FRAMERATE;
            safeBufferIndex = sTripleBufferIndex;
            gSafeToLoadVideo = VIDEO_SAFETY_ACTIVE_DMA;
        }
    } else {
        s32 lastBufferIndex = (sTripleBufferIndex + (ARRAY_COUNT(dmaTextureAddrs) - 1)) % ARRAY_COUNT(dmaTextureAddrs);
        slidstart(dmaYAY0Addrs[lastBufferIndex], dmaTextureAddrs[sTripleBufferIndex]);
        add_menu_frame();
    }

    dma_video_frame(&videoDataProps[gVideoIndex][imageVideoFrame]);
}

s32 check_image_dma_complete(void) {
    while (osRecvMesg(&videoImageDMAQueue, NULL, OS_MESG_NOBLOCK) == 0) {
        sImageDMACount--;
        assert(sImageDMACount >= 0, "Negative sImageDMACount detected!");
        if (sImageDMACount < 0) {
            sImageDMACount = 0;
        }
    }

    return (sImageDMACount == 0);
}

// return -1 if no video sign is found
static s32 get_desert_sign_video_id(ModelID32 modelId) {
    for (s32 i = 0; i < ARRAY_COUNT(dmaProps); i++) {
        if (modelId == dmaProps[i].modelId) {
            return i;
        }
    }

    return -1;
}

struct BBHistory {
    s32 instantWarpId;
    s32 billboardId;
};
#define BB_COUNT (MODEL_BILLBOARD_END - MODEL_BILLBOARD_START)
#define BB_MAX_ATTEMPTS 10
f32 gBillboardWeights[BB_COUNT] = {[0 ... BB_COUNT - 1] = 0.5f - (1.0f / (f32) BB_COUNT)}; // Weighting array for billboards so that repeated billboards are less likely to immediately show up
f32 gBillboardWeightsBackwards[BB_COUNT] = {[0 ... BB_COUNT - 1] = 0.5f - (1.0f / (f32) BB_COUNT)}; // Same as above but for going backwards (as to not interfere with forwards generation)
struct BBHistory gBillboardHistory[INSTANT_WARPS_GOAL] = {[0 ... INSTANT_WARPS_GOAL - 1] = {.instantWarpId = 0, .billboardId = -1}}; // History needed to preserve accuracy of billboards between playthroughs in all (expectable) situations
s32 generate_weighted_billboard(MTRand *rand, s32 lastBillboard) {
    s32 index;
    s32 count = lastBillboard - MODEL_BILLBOARD_START;
    f32 weightIncrease = 1.0f / (f32) count;
    f32 *weights = gBillboardWeights;
    s32 attempts = 0;
    s32 instantWarpIndex = ((gInstantWarpSpawnIndex % INSTANT_WARPS_GOAL) + INSTANT_WARPS_GOAL) % INSTANT_WARPS_GOAL; // handles negative numbers
    struct BBHistory *hist = &gBillboardHistory[instantWarpIndex];

    if (gInstantWarpSpawnIndex <= 0) {
        weights = gBillboardWeightsBackwards;
    }

    if (hist->billboardId >= 0) {
        if (!(gInstantWarpSpawnIndex > 0 && gInstantWarpSpawnIndex > hist->instantWarpId) // Going forwards from start
                    && !(hist->instantWarpId <= 0 && gInstantWarpSpawnIndex <= 0 && gInstantWarpSpawnIndex < hist->instantWarpId)) { // Going backwards from start
            return hist->billboardId + MODEL_BILLBOARD_START; // Billboard has already been generated
        }
    }

    for (attempts = 0; attempts < BB_MAX_ATTEMPTS; attempts++) {
        f32 weightTotal = 0.0f;
        f32 currentWeight = 0.0f;
        f32 generatedWeight = genRand(rand);

        for (index = 0; index < count; index++) {
            // Increase the probability selection window for each billboards; offers future benefit against more recently selected indexes
            weights[index] += weightIncrease;

            // If weight is below 0, effectively exclude it from the possible selection pool
            if (weights[index] > 0.0f)
                weightTotal += weights[index];
        }

        generatedWeight *= weightTotal;

        for (index = 0; index < count - 1; index++) { // count - 1 not an accident
            // If weight is below 0, skip the index. This in theory should never favor the last unprocessed index if that falls below 0.
            if (weights[index] <= 0.0f)
                continue;

            currentWeight += weights[index];
            if (currentWeight >= generatedWeight)
                break;
        }

        // Only one video can be loaded at a time! If a video is already loaded, try again.
        if (gVideoIndex < 0 || get_desert_sign_video_id(index + MODEL_BILLBOARD_START) < 0) {
            break;
        }
    }

    if (attempts == BB_MAX_ATTEMPTS) {
        index = 0; // NOTE: MODEL_BILLBOARD_START should not be a video!
    }

    // Index should be guaranteed to not show up again until at least 30% of the other billboards have been processed (at static quantity)
    weights[index] = -0.30f;

    // Update history entry, as we have just generated a new one
    hist->billboardId = index;
    hist->instantWarpId = gInstantWarpSpawnIndex;
    return index + MODEL_BILLBOARD_START;
}

void bhv_desert_sign_init(void) {
    for (s32 i = 0; i < ARRAY_COUNT(dmaProps); i++) {
        if (cur_obj_has_model(dmaProps[i].modelId)) {
            // bparam2 lets us know video data is active
            o->oBehParams2ndByte = BB_TYPE_VIDEO;
            SET_BPARAM2(o->oBehParams, BB_TYPE_VIDEO);
            
            if (gSafeToLoadVideo != VIDEO_SAFETY_UNALLOCATED) {
                gSafeToLoadVideo = VIDEO_SAFETY_UNSAFE;
                gVideoIndex = i;
                imageGameFrame = 0;
                imageVideoFrame = 0;
            }
        }
    }
}

// Geo to display the funny video
Gfx *geo_billboard_video_scene(s32 callContext, struct GraphNode *node, UNUSED void *context) {
    Gfx *dlStart = NULL;

    if (callContext == GEO_CONTEXT_RENDER) {
        struct GraphNodeGenerated *currentGraphNode = (struct GraphNodeGenerated *) node;

        if (gSafeToLoadVideo != VIDEO_SAFETY_SAFE) {
            return NULL;
        }

        s32 renderIndex = sTripleBufferIndex - 1;
        if (renderIndex < 0) {
            renderIndex = ARRAY_COUNT(dmaTextureAddrs) - 1;
        }

        SET_GRAPH_NODE_LAYER(currentGraphNode->fnNode.node.flags, currentGraphNode->parameter);

        Gfx *dlHead;
        if (currentGraphNode->parameter == LAYER_OPAQUE) {
            dlStart = alloc_display_list(4 * sizeof(Gfx));
            dlHead = dlStart;

            gSPDisplayList(dlHead++, sign_video_data_sign_video_data_mesh_layer_1_begin);
            gDPSetTextureImage(dlHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, TEXTURE_WIDTH, dmaTextureAddrs[renderIndex]);
            gSPDisplayList(dlHead++, sign_video_data_sign_video_data_mesh_layer_1_end);
            gSPEndDisplayList(dlHead);
        } else if (currentGraphNode->parameter == LAYER_ALPHA) {
            dlStart = alloc_display_list(2 * sizeof(Gfx));
            dlHead = dlStart;

            gSPDisplayList(dlHead++, sign_video_data_sign_video_data_mesh_layer_4);
            gSPEndDisplayList(dlHead);
        }
    }

    return dlStart;
};
