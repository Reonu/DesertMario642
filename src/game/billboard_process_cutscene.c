#include <PR/ultratypes.h>
#include <PR/gbi.h>

#include "sm64.h"
#include "actors/sign_normal/geo_header.h"
#include "actors/sign_video/geo_header.h"
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

#define VIDEO_TEXTURE_WIDTH  192
#define VIDEO_TEXTURE_HEIGHT 96

// NOTE: This has acceptable alignment, but should otherwise be taken into consideration with DMA usage.
#define VIDEO_TEXTURE_SIZE (VIDEO_TEXTURE_WIDTH * VIDEO_TEXTURE_HEIGHT * sizeof(RGBA16))
#define ALIGNED_VIDEO_BUFFER_SIZE ALIGN16(VIDEO_TEXTURE_SIZE)

#define IMAGE_TEXTURE_WIDTH  256
#define IMAGE_TEXTURE_HEIGHT 128

// NOTE: This has acceptable alignment, but should otherwise be taken into consideration with DMA usage.
#define IMAGE_TEXTURE_SIZE (IMAGE_TEXTURE_WIDTH * IMAGE_TEXTURE_HEIGHT * sizeof(RGBA16))
#define ALIGNED_IMAGE_BUFFER_SIZE ALIGN16(IMAGE_TEXTURE_SIZE)

#define IMAGE_BUFFERS 4

#define MAX_DMA_COUNT (((s32) ((VIDEO_TEXTURE_SIZE - 1) / 0x1000) + 1) + (IMAGE_BUFFERS * ((s32) ((IMAGE_TEXTURE_SIZE - 1) / 0x1000) + 1)))

#define SOUND_EFFECT_DELAY_FRAMES ((gEmulator & INSTANT_INPUT_BLACKLIST) ? 2 : 0)

enum BillboardType {
    BB_TYPE_IMAGE,
    BB_TYPE_VIDEO,
};

enum ImageDMAStatus {
    BB_IMAGE_READY,
    BB_IMAGE_ACTIVE_DMA,
};

enum VideoSafetyStates {
    VIDEO_SAFETY_UNALLOCATED,
    VIDEO_SAFETY_UNSAFE,
    VIDEO_SAFETY_ACTIVE_DMA,
    VIDEO_SAFETY_SAFE,
};

struct DMAVideoProperties {
    const struct DMAImageProps *addr;
    s32 billboardId;
    u32 relativeLoopStart;
    u32 frameTotal;
    u32 framerate;
    u32 startFrame;
    u32 sound;
};

enum BillboardIDs billboardList[BB_BILLBOARD_END] = {
    // TIER 1
    [BB_SIGN_HACKERN64]          = (u32) BB_IMAGE_HACKERN64,
    [BB_SIGN_MALTINGIUS]         = (u32) BB_IMAGE_MALTINGIUS,
    [BB_SIGN_COZIES]             = (u32) BB_IMAGE_COZIES,
    [BB_SIGN_APPY]               = (u32) BB_IMAGE_APPY,
    [BB_SIGN_MOTH]               = (u32) BB_IMAGE_MOTH,
    [BB_SIGN_SPK]                = (u32) BB_IMAGE_SPK,
    [BB_SIGN_OATMEAL]            = (u32) BB_IMAGE_OATMEAL,
    [BB_SIGN_BETTERCALLSHY]      = (u32) BB_IMAGE_BETTERCALLSHY,
    [BB_SIGN_CROSS]              = (u32) BB_IMAGE_CROSS,
    [BB_SIGN_AMOGUS]             = (u32) BB_IMAGE_AMOGUS,
    [BB_SIGN_CHEEZEPIN]          = (u32) BB_IMAGE_CHEEZEPIN,
    [BB_SIGN_FARM]               = (u32) BB_IMAGE_FARM,
    [BB_SIGN_ROVERT]             = (u32) BB_IMAGE_ROVERT,
    [BB_SIGN_COMIT]              = (u32) BB_IMAGE_COMIT,
    [BB_SIGN_MUSHROOM]           = (u32) BB_IMAGE_MUSHROOM,
    [BB_SIGN_PATCHMAKER]         = (u32) BB_IMAGE_PATCHMAKER,
    [BB_SIGN_SYNERGY]            = (u32) BB_IMAGE_SYNERGY,
    [BB_SIGN_MOTIVATE]           = (u32) BB_IMAGE_MOTIVATE,
    [BB_SIGN_PREDATOR]           = (u32) BB_IMAGE_PREDATOR,
    [BB_SIGN_NINTENDO_EMPLOYEES] = (u32) BB_IMAGE_NINTENDO_EMPLOYEES,
    [BB_SIGN_BART]               = (u32) BB_IMAGE_BART,
    [BB_SIGN_YUGAMINEENA]        = (u32) BB_IMAGE_YUGAMINEENA,
    [BB_SIGN_GAMER_TYPE]         = (u32) BB_IMAGE_GAMER_TYPE,
    [BB_SIGN_ROSALU]             = (u32) BB_IMAGE_ROSALU,
    [BB_SIGN_BLOCKINGTON]        = (u32) BB_IMAGE_BLOCKINGTON,

    // TIER 2
    [BB_SIGN_YOUTUBE]            = (u32) BB_IMAGE_YOUTUBE,
    [BB_SIGN_SPOON]              = (u32) BB_IMAGE_SPOON,
    [BB_SIGN_JOEL]               = (u32) BB_IMAGE_JOEL,
    [BB_SIGN_TCS]                = (u32) BB_IMAGE_TCS,
    [BB_SIGN_FREE_PSP]           = (u32) BB_VIDEO_FREE_PSP,
    [BB_SIGN_FELIZ_JEUVES]       = (u32) BB_VIDEO_FELIZ_JEUVES,

    // TIER 3
    [BB_SIGN_SIMPLEFLIPS]        = (u32) BB_IMAGE_SIMPLEFLIPS,
    [BB_SIGN_ASS_IMPACT]         = (u32) BB_IMAGE_ASS_IMPACT,
    [BB_SIGN_MVC]                = (u32) BB_IMAGE_MVC,
    [BB_SIGN_MVH]                = (u32) BB_IMAGE_MVH,
    [BB_SIGN_IDIOT]              = (u32) BB_IMAGE_IDIOT,
    [BB_SIGN_DESERT_MARIO]       = (u32) BB_IMAGE_DESERT_MARIO,
    [BB_SIGN_GBJ_PAINTING]       = (u32) BB_VIDEO_GBJ_PAINTING,

    // TIER 4
    [BB_SIGN_COMIT_STANS]        = (u32) BB_IMAGE_COMIT_STANS,
    [BB_SIGN_COMIT_ANTIS]        = (u32) BB_IMAGE_COMIT_ANTIS,
    [BB_SIGN_MAKE_CLEAN]         = (u32) BB_IMAGE_MAKE_CLEAN,
    [BB_SIGN_XENOBLADE]          = (u32) BB_IMAGE_XENOBLADE,
    [BB_SIGN_XENOGEARS]          = (u32) BB_IMAGE_XENOGEARS,
    [BB_SIGN_LEON]               = (u32) BB_VIDEO_LEON,
};

struct DMAVideoProperties videoDMAProps[BB_VIDEO_COUNT] = {
    [BB_VIDEO_IMAGES]       = {.addr = dma_image_data,          .billboardId = -1,                   .relativeLoopStart = 0, .frameTotal = ARRAY_COUNT(dma_image_data),          .startFrame = 0, .framerate = 1,  .sound = NO_SOUND     },
    [BB_VIDEO_FREE_PSP]     = {.addr = free_psp_video_data,     .billboardId = BB_SIGN_FREE_PSP,     .relativeLoopStart = 0, .frameTotal = ARRAY_COUNT(free_psp_video_data),     .startFrame = 0, .framerate = 10, .sound = NO_SOUND     },
    [BB_VIDEO_GBJ_PAINTING] = {.addr = gbj_painting_video_data, .billboardId = BB_SIGN_GBJ_PAINTING, .relativeLoopStart = 0, .frameTotal = ARRAY_COUNT(gbj_painting_video_data), .startFrame = 0, .framerate = 30, .sound = NO_SOUND     },
    [BB_VIDEO_LEON]         = {.addr = leon_video_data,         .billboardId = BB_SIGN_LEON,         .relativeLoopStart = 0, .frameTotal = ARRAY_COUNT(leon_video_data),         .startFrame = 0, .framerate = 24, .sound = SOUND_BB_LEON},
    [BB_VIDEO_FELIZ_JEUVES] = {.addr = feliz_jeuves_video_data, .billboardId = BB_SIGN_FELIZ_JEUVES, .relativeLoopStart = 0, .frameTotal = ARRAY_COUNT(feliz_jeuves_video_data), .startFrame = 0, .framerate = 20, .sound = NO_SOUND     },
};

OSIoMesg videoImageDMAIoMesg[MAX_DMA_COUNT];
OSMesg videoImageDMAReceivedMesg[MAX_DMA_COUNT];
OSMesgQueue videoImageDMAQueue;

u8 gSafeToLoadVideo = VIDEO_SAFETY_UNALLOCATED;
u8 sTripleBufferIndex = 0;
u8 safeBufferIndex = 0;
u8 hasInitializedMessageQueue = FALSE;
s32 sImageDMACount = 0;

Texture *dmaImageTextureAddrs[IMAGE_BUFFERS];
Texture *dmaImageYAY0Addrs[IMAGE_BUFFERS];
enum ImageDMAStatus dmaImageStatus[IMAGE_BUFFERS];

Texture *dmaVideoTextureAddrs[3];
Texture *dmaVideoYAY0Addrs[3];
struct DMAImageProps *videoDataProps[BB_VIDEO_COUNT];

u32 imageGameFrame = 0;
u32 imageVideoFrame = 0;
s32 gVideoIndex = -1;
u32 replaySoundEffect = -1;

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

    assert(sImageDMACount <= MAX_DMA_COUNT, "sImageDMACount too large!");
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

static void dma_video_frame(struct DMAImageProps *dmaProps) {
    Texture *relativeAddr = (Texture *) ((u32) dmaProps->address);
    Texture *dest = dmaVideoYAY0Addrs[sTripleBufferIndex];
    sTripleBufferIndex = (sTripleBufferIndex + 1) % ARRAY_COUNT(dmaVideoYAY0Addrs);

    assert(dmaProps->compressedSize < ALIGNED_VIDEO_BUFFER_SIZE, "Compressed YAY0 larger than source!");

    dma_read_image_noblock(dest, relativeAddr, (u8 *) ((size_t) relativeAddr + dmaProps->compressedSize));
}

static void add_menu_frame(void) {
    if (gVideoIndex >= 0 && gVideoIndex < BB_VIDEO_COUNT) {
        imageGameFrame++;
        imageVideoFrame = imageGameFrame * videoDMAProps[gVideoIndex].framerate / GAME_FRAMERATE;
        if (replaySoundEffect > 0) {
            replaySoundEffect--;
        }

        if (imageVideoFrame >= videoDMAProps[gVideoIndex].frameTotal) {
            imageGameFrame = videoDMAProps[gVideoIndex].relativeLoopStart * GAME_FRAMERATE / videoDMAProps[gVideoIndex].framerate;
            imageVideoFrame = imageGameFrame * videoDMAProps[gVideoIndex].framerate / GAME_FRAMERATE;
            if (videoDMAProps[gVideoIndex].sound != NO_SOUND) {
                replaySoundEffect = SOUND_EFFECT_DELAY_FRAMES;
            }
        }
    }
}

void init_menu_video_buffers(void) {
    gSafeToLoadVideo = VIDEO_SAFETY_UNALLOCATED;
    imageGameFrame = 0;
    imageVideoFrame = 0;
    gVideoIndex = -1;
    replaySoundEffect = -1;
    sTripleBufferIndex = 0;
    safeBufferIndex = 0;

    u8 *imagememaddr = main_pool_alloc(ALIGNED_IMAGE_BUFFER_SIZE * ARRAY_COUNT(dmaImageTextureAddrs), MEMORY_POOL_LEFT);
    if (imagememaddr == NULL) {
        assert(FALSE, "Out of memory! :(");
        return;
    }

    u8 *imageyay0memaddr = main_pool_alloc(ALIGNED_IMAGE_BUFFER_SIZE * ARRAY_COUNT(dmaImageYAY0Addrs), MEMORY_POOL_LEFT);
    if (imageyay0memaddr == NULL) {
        assert(FALSE, "Out of memory! :(");
        return;
    }

    u8 *texturememaddr = main_pool_alloc(ALIGNED_VIDEO_BUFFER_SIZE * ARRAY_COUNT(dmaVideoTextureAddrs), MEMORY_POOL_LEFT);
    if (texturememaddr == NULL) {
        assert(FALSE, "Out of memory! :(");
        return;
    }

    u8 *yay0memaddr = main_pool_alloc(ALIGNED_VIDEO_BUFFER_SIZE * ARRAY_COUNT(dmaVideoYAY0Addrs), MEMORY_POOL_LEFT);
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
        uint32_t dataSize = (videoDMAProps[i].frameTotal * sizeof(struct DMAImageProps));
        u8 *propData = main_pool_alloc(ALIGN16(dataSize), MEMORY_POOL_LEFT);
        if (propData == NULL) {
            assert(FALSE, "Out of memory! :(");
            return;
        }

        videoDataProps[i] = (struct DMAImageProps*) propData;

        dma_read_block(propData, (u8 *) videoDMAProps[i].addr, (u8 *) videoDMAProps[i].addr + (size_t) ALIGN16(dataSize));
    }

    gSafeToLoadVideo = VIDEO_SAFETY_UNSAFE;

    for (s32 i = 0; i < ARRAY_COUNT(dmaImageTextureAddrs); i++) {
        dmaImageTextureAddrs[i] = &imagememaddr[ALIGNED_IMAGE_BUFFER_SIZE * i];
        dmaImageYAY0Addrs[i] = &imageyay0memaddr[ALIGNED_IMAGE_BUFFER_SIZE * i];
    }

    for (s32 i = 0; i < ARRAY_COUNT(dmaVideoTextureAddrs); i++) {
        dmaVideoTextureAddrs[i] = &texturememaddr[ALIGNED_VIDEO_BUFFER_SIZE * i];
        dmaVideoYAY0Addrs[i] = &yay0memaddr[ALIGNED_VIDEO_BUFFER_SIZE * i];
    }
}

void update_menu_video_buffers(void) {
    if (gSafeToLoadVideo == VIDEO_SAFETY_UNALLOCATED) {
        return;
    }

    u8 dmaComplete = check_image_dma_complete(FALSE);

    if (gVideoIndex < 0 || gVideoIndex >= BB_VIDEO_COUNT || find_first_object_with_behavior_and_bparams(bhvDesertSign, BB_TYPE_VIDEO, 0xFF) == NULL) {
        gSafeToLoadVideo = VIDEO_SAFETY_UNSAFE;
        imageGameFrame = 0;
        imageVideoFrame = 0;
        gVideoIndex = -1;
        replaySoundEffect = -1;
        return;
    }

    if (!dmaComplete) {
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
            if (videoDMAProps[gVideoIndex].sound != NO_SOUND) {
                replaySoundEffect = SOUND_EFFECT_DELAY_FRAMES;
            }
            imageGameFrame = videoDMAProps[gVideoIndex].startFrame * GAME_FRAMERATE / videoDMAProps[gVideoIndex].framerate;
            imageVideoFrame = imageGameFrame * videoDMAProps[gVideoIndex].framerate / GAME_FRAMERATE;
            safeBufferIndex = sTripleBufferIndex;
            gSafeToLoadVideo = VIDEO_SAFETY_ACTIVE_DMA;
        }
    } else {
        s32 lastBufferIndex = (sTripleBufferIndex + (ARRAY_COUNT(dmaVideoTextureAddrs) - 1)) % ARRAY_COUNT(dmaVideoTextureAddrs);
        slidstart(dmaVideoYAY0Addrs[lastBufferIndex], dmaVideoTextureAddrs[sTripleBufferIndex]);
        add_menu_frame();
    }

    dma_video_frame(&videoDataProps[gVideoIndex][imageVideoFrame]);
}

s32 check_image_dma_complete(u32 shouldUninitialize) {
    while (osRecvMesg(&videoImageDMAQueue, NULL, OS_MESG_NOBLOCK) == 0) {
        sImageDMACount--;
        assert(sImageDMACount >= 0, "Negative sImageDMACount detected!");
        if (sImageDMACount < 0) {
            sImageDMACount = 0;
        }
    }

    if (sImageDMACount == 0) {
        for (s32 i = 0; i < ARRAY_COUNT(dmaImageStatus); i++) {
            if (dmaImageStatus[i] == BB_IMAGE_ACTIVE_DMA) {
                dmaImageStatus[i] = BB_IMAGE_READY;
                slidstart(dmaImageYAY0Addrs[i], dmaImageTextureAddrs[i]);
            }
        }

        if (shouldUninitialize) {
            gSafeToLoadVideo = VIDEO_SAFETY_UNALLOCATED;
            imageGameFrame = 0;
            imageVideoFrame = 0;
            gVideoIndex = -1;
            replaySoundEffect = -1;
            sTripleBufferIndex = 0;
            safeBufferIndex = 0;
        }

        return TRUE;
    }

    return FALSE;
}

// return -1 if no video sign is found
s32 get_desert_sign_video_id(ModelID32 billboardId) {
    for (s32 i = 0; i < ARRAY_COUNT(videoDMAProps); i++) {
        if (billboardId == (u32) videoDMAProps[i].billboardId) {
            return i;
        }
    }

    return -1;
}

// return -1 if no image sign is found
static s32 get_desert_sign_image_buffer(ModelID32 billboardId) {
    uintptr_t *behaviorAddr = segmented_to_virtual(bhvDesertSign);
    struct ObjectNode *listHead = &gObjectLists[get_object_list_from_behavior(behaviorAddr)];
    struct ObjectNode *objNode = listHead->next;

    if (!gFBEEnabled) {
        return -1;
    }

    while (listHead != objNode) {
        struct Object *obj = ((struct Object *) objNode);
        if (obj_has_behavior(obj, bhvDesertSign) && obj->activeFlags != ACTIVE_FLAG_DEACTIVATED
                    && obj->oBehParams2ndByte == (s32) billboardId && GET_BPARAM4(obj->oBehParams) != BB_TYPE_VIDEO) {
            return GET_BPARAM3(obj->oBehParams);
        }

        objNode = objNode->next;
    }

    return -1;
}

void framebuffer_copy(RGBA16 *fbAddr) {
    if (!gFBEEnabled || gSafeToLoadVideo == VIDEO_SAFETY_UNALLOCATED) {
        return;
    }

    s32 bufferIndex = get_desert_sign_image_buffer(BB_SIGN_DESERT_MARIO);
    if (bufferIndex < 0 || dmaImageStatus[bufferIndex] == BB_IMAGE_ACTIVE_DMA) {
        return;
    }

    Texture *bufferStart = dmaImageTextureAddrs[bufferIndex];

    #define x1 (SCREEN_CENTER_X - (IMAGE_TEXTURE_WIDTH / 2))
    #define y1 (SCREEN_CENTER_Y - (IMAGE_TEXTURE_HEIGHT / 2))

    for (s32 i = 0; i < IMAGE_TEXTURE_HEIGHT; i++) {
        bcopy(&fbAddr[(SCREEN_WIDTH * (i + y1)) + x1], &bufferStart[i * (IMAGE_TEXTURE_WIDTH * sizeof(RGBA16))], (IMAGE_TEXTURE_WIDTH * sizeof(RGBA16)));
    }

    #undef x1
    #undef y1
}

struct BBHistory {
    s32 instantWarpId;
    s32 billboardId;
};
#define BB_MAX_ATTEMPTS 10
f32 gBillboardWeights[BB_BILLBOARD_END] = {[0 ... BB_BILLBOARD_END - 1] = 0.5f - (1.0f / (f32) BB_BILLBOARD_END)}; // Weighting array for billboards so that repeated billboards are less likely to immediately show up
f32 gBillboardWeightsBackwards[BB_BILLBOARD_END] = {[0 ... BB_BILLBOARD_END - 1] = 0.5f - (1.0f / (f32) BB_BILLBOARD_END)}; // Same as above but for going backwards (as to not interfere with forwards generation)
struct BBHistory gBillboardHistory[INSTANT_WARPS_GOAL] = {[0 ... INSTANT_WARPS_GOAL - 1] = {.instantWarpId = 0, .billboardId = -1}}; // History needed to preserve accuracy of billboards between playthroughs in all (expectable) situations
s32 generate_weighted_billboard(MTRand *rand, s32 lastBillboard) {
    s32 index;
    f32 weightIncrease = 1.0f / (f32) lastBillboard;
    f32 *weights = gBillboardWeights;
    s32 attempts = 0;
    s32 instantWarpIndex = ((gInstantWarpSpawnIndex % INSTANT_WARPS_GOAL) + INSTANT_WARPS_GOAL) % INSTANT_WARPS_GOAL; // handles negative numbers
    struct BBHistory *hist = &gBillboardHistory[instantWarpIndex];
    u32 newSeed = genRandLong(rand);

    if (gInstantWarpSpawnIndex <= 0) {
        weights = gBillboardWeightsBackwards;
    }

    if (hist->billboardId >= 0) {
        if (!(gInstantWarpSpawnIndex > 0 && gInstantWarpSpawnIndex > hist->instantWarpId) // Going forwards from start
                    && !(hist->instantWarpId <= 0 && gInstantWarpSpawnIndex <= 0 && gInstantWarpSpawnIndex < hist->instantWarpId)) { // Going backwards from start
            return hist->billboardId; // Billboard has already been generated
        }
    }

    MTRand newRand = seedRand(newSeed);
    for (attempts = 0; attempts < BB_MAX_ATTEMPTS; attempts++) {
        f32 weightTotal = 0.0f;
        f32 currentWeight = 0.0f;
        f32 generatedWeight = genRand(&newRand);

        for (index = 0; index < lastBillboard; index++) {
            // Increase the probability selection window for each billboards; offers future benefit against more recently selected indexes
            weights[index] += weightIncrease;

            // If weight is below 0, effectively exclude it from the possible selection pool
            if (weights[index] > 0.0f)
                weightTotal += weights[index];
        }

        generatedWeight *= weightTotal;

        for (index = 0; index < lastBillboard - 1; index++) { // lastBillboard - 1 not an accident
            // If weight is below 0, skip the index. This in theory should never favor the last unprocessed index if that falls below 0.
            if (weights[index] <= 0.0f)
                continue;

            currentWeight += weights[index];
            if (currentWeight >= generatedWeight)
                break;
        }

        // Only one video can be loaded at a time! If a video is already loaded, try again.
        if (gVideoIndex < 0 || get_desert_sign_video_id(index) < 0) {
            break;
        }
    }

    if (attempts == BB_MAX_ATTEMPTS) {
        index = BB_BILLBOARD_START; // NOTE: BB_BILLBOARD_START should not be a video!
    }

    // Index should be guaranteed to not show up again until at least 45% of the other billboards have been processed (at static quantity)
    weights[index] = -0.45f;

    // Update history entry, as we have just generated a new one
    hist->billboardId = index;
    hist->instantWarpId = gInstantWarpSpawnIndex;
    return index;
}

void bhv_desert_sign_init(void) {
    for (s32 i = 0; i < ARRAY_COUNT(videoDMAProps); i++) {
        if (o->oBehParams2ndByte == videoDMAProps[i].billboardId) {
            // bparam4 lets us know video data is active
            SET_BPARAM4(o->oBehParams, BB_TYPE_VIDEO);
            
            if (gSafeToLoadVideo != VIDEO_SAFETY_UNALLOCATED) {
                gSafeToLoadVideo = VIDEO_SAFETY_UNSAFE;
                gVideoIndex = i;
                imageGameFrame = 0;
                imageVideoFrame = 0;
                replaySoundEffect = -1;
            }
        }
    }

    u8 imagesUsed[IMAGE_BUFFERS] = {[0 ... IMAGE_BUFFERS - 1] = FALSE};
    uintptr_t *behaviorAddr = segmented_to_virtual(bhvDesertSign);
    struct ObjectNode *listHead = &gObjectLists[get_object_list_from_behavior(behaviorAddr)];
    struct ObjectNode *objNode = listHead->next;

    while (listHead != objNode) {
        struct Object *obj = ((struct Object *) objNode);
        if (obj_has_behavior(obj, bhvDesertSign) && obj->activeFlags != ACTIVE_FLAG_DEACTIVATED && 
                    GET_BPARAM4(obj->oBehParams) != BB_TYPE_VIDEO) {
            imagesUsed[GET_BPARAM3(obj->oBehParams)] = TRUE;
        }

        objNode = objNode->next;
    }

    u8 openIndex = 0;
    for (; openIndex < ARRAY_COUNT(imagesUsed); openIndex++) {
        if (!imagesUsed[openIndex]) {
            break;
        }
    }

    if (openIndex == ARRAY_COUNT(imagesUsed)) {
        obj_mark_for_deletion(o); // No open image DMA slots...
        return;
    }

    SET_BPARAM3(o->oBehParams, openIndex);
    struct DMAImageProps *dmaProps = &videoDataProps[BB_VIDEO_IMAGES][billboardList[o->oBehParams2ndByte]];
    
    assert(dmaProps->compressedSize < ALIGNED_IMAGE_BUFFER_SIZE, "Compressed YAY0 larger than source!");
    Texture *relativeAddr = (Texture *) ((u32) dmaProps->address);
    Texture *dest = dmaImageYAY0Addrs[openIndex];
    dma_read_image_noblock(dest, relativeAddr, (u8 *) ((size_t) relativeAddr + dmaProps->compressedSize));
    dmaImageStatus[openIndex] = BB_IMAGE_ACTIVE_DMA;
}

void bhv_desert_sign_loop(void) {
    if (BPARAM4 != BB_TYPE_VIDEO || gSafeToLoadVideo != VIDEO_SAFETY_SAFE) {
        return;
    }

    if (replaySoundEffect == 0 && (gVideoIndex >= 0 && gVideoIndex < BB_VIDEO_COUNT) && videoDMAProps[gVideoIndex].sound != NO_SOUND) {
        replaySoundEffect = -1;
        play_sound(videoDMAProps[gVideoIndex].sound, o->header.gfx.cameraToObject);
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
            renderIndex = ARRAY_COUNT(dmaVideoTextureAddrs) - 1;
        }

        SET_GRAPH_NODE_LAYER(currentGraphNode->fnNode.node.flags, currentGraphNode->parameter);

        Gfx *dlHead;
        if (currentGraphNode->parameter == LAYER_OPAQUE) {
            dlStart = alloc_display_list(4 * sizeof(Gfx));
            dlHead = dlStart;

            gSPDisplayList(dlHead++, sign_video_sign_video_mesh_layer_1_begin);
            gDPSetTextureImage(dlHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, VIDEO_TEXTURE_WIDTH, dmaVideoTextureAddrs[renderIndex]);
            gSPDisplayList(dlHead++, sign_video_sign_video_mesh_layer_1_end);
            gSPEndDisplayList(dlHead);
        } else if (currentGraphNode->parameter == LAYER_ALPHA) {
            dlStart = alloc_display_list(2 * sizeof(Gfx));
            dlHead = dlStart;

            gSPDisplayList(dlHead++, sign_video_sign_video_mesh_layer_4);
            gSPEndDisplayList(dlHead);
        }
    }

    return dlStart;
};

// Geo to display the funny image
Gfx *geo_billboard_image_scene(s32 callContext, struct GraphNode *node, UNUSED void *context) {
    Gfx *dlStart = NULL;

    if (callContext == GEO_CONTEXT_RENDER) {
        struct GraphNodeGenerated *currentGraphNode = (struct GraphNodeGenerated *) node;
        struct Object *obj = (struct Object *) gCurGraphNodeObject;

        if (dmaImageStatus[GET_BPARAM3(obj->oBehParams)] == BB_IMAGE_ACTIVE_DMA) {
            return NULL;
        }

        SET_GRAPH_NODE_LAYER(currentGraphNode->fnNode.node.flags, currentGraphNode->parameter);

        Gfx *dlHead;
        if (currentGraphNode->parameter == LAYER_OPAQUE) {
            dlStart = alloc_display_list(5 * sizeof(Gfx));
            dlHead = dlStart;

            gSPDisplayList(dlHead++, sign_normal_sign_normal_mesh_layer_1_first);
            gDPSetTextureImage(dlHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, IMAGE_TEXTURE_WIDTH, dmaImageTextureAddrs[GET_BPARAM3(obj->oBehParams)]);
            gSPDisplayList(dlHead++, sign_normal_sign_normal_mesh_layer_1_second);
            gSPDisplayList(dlHead++, sign_normal_sign_normal_mesh_layer_1_end);
            gSPEndDisplayList(dlHead);
        } else if (currentGraphNode->parameter == LAYER_ALPHA) {
            dlStart = alloc_display_list(2 * sizeof(Gfx));
            dlHead = dlStart;

            gSPDisplayList(dlHead++, sign_normal_sign_normal_mesh_layer_4);
            gSPEndDisplayList(dlHead);
        } else if (currentGraphNode->parameter == LAYER_TRANSPARENT_DECAL && obj->oBehParams2ndByte == BB_SIGN_IDIOT) {
            dlStart = alloc_display_list(2 * sizeof(Gfx));
            dlHead = dlStart;

            gSPDisplayList(dlHead++, sign_normal_sign_idiot_mesh_layer_1_idiot_mario);
            gSPEndDisplayList(dlHead);
        }
    }

    return dlStart;
};

STATIC_ASSERT((BB_IMAGE_COUNT + (BB_VIDEO_COUNT - 1)) == BB_BILLBOARD_END, "Data mismatch for tracked images/videos and billboard count");
