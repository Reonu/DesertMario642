#include <PR/ultratypes.h>

#include "sm64.h"
#include "seq_ids.h"
#include "dialog_ids.h"
#include "audio/external.h"
#include "audio/synthesis.h"
#include "level_update.h"
#include "game_init.h"
#include "level_update.h"
#include "main.h"
#include "engine/math_util.h"
#include "engine/graph_node.h"
#include "area.h"
#include "save_file.h"
#include "sound_init.h"
#include "mario.h"
#include "camera.h"
#include "object_list_processor.h"
#include "ingame_menu.h"
#include "obj_behaviors.h"
#include "save_file.h"
#ifdef MULTILANG
#include "memory.h"
#include "segment_symbols.h"
#endif
#include "level_table.h"
#include "course_table.h"
#include "rumble_init.h"
#include "puppycam2.h"
#include "puppyprint.h"
#include "level_commands.h"
#include "debug.h"
#include "render_fog.h"
#include "print.h"
#include "desert_spawn.h"
#include "mtwister.h"
#include "behavior_data.h"
#include "types.h"
#include "include/n64/PR/os_libc.h"
#include "actors/sign_normal/geo_header.h"
#include "game/emutest.h"

struct DesertSpawnCoords LeftSide = {
    .x = -3000,
    .y = 0,
    .z = -24628,
};

struct DesertSpawnCoords RightSide = {
    .x = 3000,
    .y = 0,
    .z = -24628,
};

struct DesertSpawnCoords Road = {
    .x = 0,
    .y = 0,
    .z = -24628,
};

struct DesertSpawnCoords decide_left_or_right(MTRand *rand) {
    f32 randValue = genRand(rand);
    if (randValue < 0.5f) {
        return LeftSide;
    } else {
        return RightSide;
    }
}

void spawn_electrical_poles(MTRand *rand) {
    spawn_object_desert(gCurrentObject, 0, MODEL_ELECTRICAL_POLE, bhvElectricalPole, LeftSide.x,LeftSide.y,LeftSide.z,0,0,0,rand);
    spawn_object_desert(gCurrentObject, 0, MODEL_ELECTRICAL_POLE, bhvElectricalPole, RightSide.x,RightSide.y,RightSide.z,0,0,0,rand);
}

#define TIER_2_THRESHOLD 20
#define TIER_3_THRESHOLD 30
#define TIER_4_THRESHOLD 40

u16 decide_billboard_model_id(MTRand *rand) {
    u16 maxTier;
    if (gInstantWarpSpawnIndex >= TIER_4_THRESHOLD) {
        maxTier = BB_BILLBOARD_END_TIER4;
    } else if (gInstantWarpSpawnIndex >= TIER_3_THRESHOLD) {
        maxTier = BB_BILLBOARD_END_TIER3;
    } else if (gInstantWarpSpawnIndex >= TIER_2_THRESHOLD){
        maxTier = BB_BILLBOARD_END_TIER2;
    } else {
        maxTier = BB_BILLBOARD_END_TIER1;
    }

    return generate_weighted_billboard(rand, maxTier);
}

void spawn_billboard(MTRand *rand) {
    u8 isRight;
    u8 bparam2;
    u16 rot;
    ModelID32 model = MODEL_BILLBOARD_IMAGE;
    struct DesertSpawnCoords spawnCoords = decide_left_or_right(rand);
    isRight = (spawnCoords.x == RightSide.x);

    if (isRight) {
        rot = DEGREES(-45);
    } else {
        rot = DEGREES(45);
    }

    bparam2 = decide_billboard_model_id(rand);

    if (get_desert_sign_video_id(bparam2) >= 0) {
        model = MODEL_BILLBOARD_VIDEO;
    }

    struct Object *obj = spawn_object_desert(gCurrentObject, 0, model, bhvDesertSign, spawnCoords.x,spawnCoords.y,spawnCoords.z,0,rot,0,rand);
    if (obj) {
        obj->oBehParams2ndByte = bparam2;
        SET_BPARAM2(obj->oBehParams, bparam2);
    }
}

void spawn_ufo(MTRand *rand) {
    u8 isRight;
    struct DesertSpawnCoords spawnCoords = decide_left_or_right(rand);

    isRight = (spawnCoords.x == RightSide.x);

    if (isRight) {
        spawn_object_desert(gCurrentObject, 0, MODEL_UFO_VISUAL, bhvUfo, spawnCoords.x,spawnCoords.y,spawnCoords.z,0,0,0,rand);
    } else {
        spawn_object_desert(gCurrentObject, 0, MODEL_UFO_VISUAL, bhvUfo, spawnCoords.x,spawnCoords.y,spawnCoords.z,0,0,0,rand);
    }
}

void spawn_bushes(MTRand *rand) {
    u32 bushesAmount = random_in_range(rand, 5) + 3;
    for (u32 i = 0; i < bushesAmount; i++) {
        u32 bushX = (s32)random_in_range(rand, 501) - 250;
        u32 bushZ = (s32)random_in_range(rand, 501) - 250;
        u32 bushX2 = (s32)random_in_range(rand, 501) - 250;
        u32 bushZ2 = (s32)random_in_range(rand, 501) - 250;
        spawn_object_desert(gCurrentObject, 0, MODEL_BUSH, bhvDesertDecor, LeftSide.x + bushX,LeftSide.y,LeftSide.z + bushZ,0,0,0,rand);
        spawn_object_desert(gCurrentObject, 0, MODEL_BUSH, bhvDesertDecor, RightSide.x + bushX2,RightSide.y,RightSide.z + bushZ2,0,0,0,rand);
    }
}

void spawn_gas_station(MTRand *rand) {
    spawn_object_desert(gCurrentObject, 0, MODEL_GAS_STATION, bhvGasStation, LeftSide.x,LeftSide.y,LeftSide.z,0,0,0,rand);
}

void spawn_decor_and_rotate(MTRand *rand, u16 modelID) {
    u8 isRight;
    struct DesertSpawnCoords spawnCoords = decide_left_or_right(rand);

    isRight = (spawnCoords.x == RightSide.x);

    if (isRight) {
        spawn_object_desert(gCurrentObject, 0, modelID, bhvDesertDecor, spawnCoords.x,spawnCoords.y,spawnCoords.z,0,DEGREES(0),0,rand);
    } else {
        spawn_object_desert(gCurrentObject, 0, modelID, bhvDesertDecor, spawnCoords.x,spawnCoords.y,spawnCoords.z,0,DEGREES(180),0,rand);
    }
}


#define GOOMBA_CHANCE 0.10f
#define POKEY_CHANCE 0.07f
#define KLEPTO_CHANCE 0.05f
#define SUN_CHANCE 0.40f

#define ELECTRICAL_POLE_CHANCE 0.05f
#define BUSH_CHANCE 0.25f
#define BILLBOARD_CHANCE 0.50f
#define UFO_CHANCE 0.01f
#define HOUSE_CHANCE 0.01f

f32 chancePrint;
void spawn_big_decoration(MTRand *rand) {
    f32 chanceStorage = genRand(rand);
    chancePrint = chanceStorage;
    chanceStorage -= ELECTRICAL_POLE_CHANCE;
    if (chanceStorage < 0) {
        spawn_electrical_poles(rand);
        return;
    }
    chanceStorage -= BILLBOARD_CHANCE;
    if (chanceStorage < 0) {
        spawn_billboard(rand);
        return;
    }
    chanceStorage -= HOUSE_CHANCE;
    if (chanceStorage < 0) {
        spawn_decor_and_rotate(rand, MODEL_DESERT_HOUSE);
        return;
    }
    chanceStorage -= UFO_CHANCE;
    if (chanceStorage < 0) {
        spawn_ufo(rand);
        return;
    }

}

void spawn_small_decoration(MTRand *rand) {
    f32 chanceStorage = genRand(rand);
    chanceStorage -= BUSH_CHANCE;
    if (chanceStorage < 0) {
        spawn_bushes(rand);
        return;
    }    
}

void spawn_goomba(MTRand *rand) {
    f32 random = genRand(rand);
    struct Object *obj = spawn_object_desert(gCurrentObject, 0, MODEL_GOOMBA_CUSTOM_MESH, bhvGoomba, Road.x,Road.y,Road.z,0,0,0,rand);
    if (obj && random < 0.1f) {
        obj->oIsFloomba = TRUE;
    }
}

void spawn_enemy(MTRand *rand) {
    f32 chanceStorage = genRand(rand);
    chanceStorage -= GOOMBA_CHANCE;
    if (chanceStorage < 0) {
        spawn_goomba(rand);
        return;
    }
    chanceStorage -= POKEY_CHANCE;
    if (chanceStorage < 0) {
        spawn_object_desert(gCurrentObject, 0, MODEL_NONE, bhvPokey, Road.x,Road.y,Road.z,0,0,0,rand);
        return;
    }
    chanceStorage -= KLEPTO_CHANCE;
    if (chanceStorage < 0) {
        spawn_object_desert(gCurrentObject, 0, MODEL_KLEPTO, bhvKlepto, Road.x,Road.y,Road.z,0,0,0,rand);
        return;
    }
    chanceStorage -= SUN_CHANCE;
    if (chanceStorage < 0) {
        if (!gAngrySunPresent) {
            spawn_object_desert(gCurrentObject, 0, MODEL_ANGRY_SUN, bhvAngrySun, Road.x,Road.y,Road.z,0,0,0,rand);
        }
        return;
    }
}
#define RGB_HOUSE_WARPS (INSTANT_WARPS_GOAL / 2)
void bhv_desert_spawner_loop(void) {
    //print_text_fmt_int(20,20, "Chance: %.2f", chancePrint);
    if (gInstantWarpDisplacement) {
        gInstantWarpSpawnIndex = gInstantWarpCounter;

        // Offset the warp counter by 2 based on the direction.
        // This is because objects are spawned 2 tiles away from the center of the map.
        if (gInstantWarpType == INSTANT_WARP_FORWARDS) {
            gInstantWarpSpawnIndex += TILES_IN_FRONT_OR_BEHIND;
        } else if (gInstantWarpType == INSTANT_WARP_BACKWARDS) {
            gInstantWarpSpawnIndex -= TILES_IN_FRONT_OR_BEHIND;
        } else {
            print_text(20,20,"TEST");
        }

        if (gInstantWarpSpawnIndex >= -TILES_IN_FRONT_OR_BEHIND && gInstantWarpSpawnIndex <= TILES_IN_FRONT_OR_BEHIND) {
            return; // Do not generate anything at the start of the run, as it doesn't when you're initially spawned either
        }

        MTRand firstRand = seedRand(gInstantWarpSpawnIndex);
        u32 actualSeed = genRandLong(&firstRand);
        MTRand newSeed = seedRand(actualSeed);
        u32 numSmall = random_in_range(&newSeed, 5);

        if (gInstantWarpSpawnIndex % 20 == 0) {
            spawn_gas_station(&newSeed);
        } else if (gInstantWarpSpawnIndex == RGB_HOUSE_WARPS) {
            spawn_decor_and_rotate(&newSeed, MODEL_DESERT_HOUSE_RGB); 
        } else {
            for (u32 i = 0; i < numSmall; i++) {
                spawn_small_decoration(&newSeed);
            }
            spawn_big_decoration(&newSeed);
            spawn_enemy(&newSeed);
        }
    }
}

void modulate_rgb_color(u32 *color) {
    static float hue = 0.0f;
    float r, g, b;

    // Convert hue to RGB
    int i = (int)(hue * 6);
    float f = hue * 6 - i;
    float q = 1 - f;

    switch (i % 6) {
        case 0: default: r = 1, g = f, b = 0; break;
        case 1:          r = q, g = 1, b = 0; break;
        case 2:          r = 0, g = 1, b = f; break;
        case 3:          r = 0, g = q, b = 1; break;
        case 4:          r = f, g = 0, b = 1; break;
        case 5:          r = 1, g = 0, b = q; break;
    }

    // Scale RGB values to 0-255
    *color = ((int)(r * 255) << 16) | ((int)(g * 255) << 8) | (int)(b * 255);

    // Increment hue
    hue += 0.01f;
    if (hue >= 1.0f) {
        hue -= 1.0f;
    }
}

#define RGB_HOUSE_WARPS_START_MUSIC (RGB_HOUSE_WARPS + 1)
#define RGB_HOUSE_WARPS_STOP_MUSIC (RGB_HOUSE_WARPS + 3)
void bhv_desert_decor_loop(void) {
    s16 offset;
    u16 isRight;

    if (o->oPosX > 0) {
        isRight = 1;
    } else {
        isRight = 0;
    }

    if (isRight) {
        offset = DEGREES(-45);
    } else {
        offset = DEGREES(45);
    }

    warp_desert_object(o);

    if (o->behavior == segmented_to_virtual(bhvDesertSign)) {
        MTRand newSeed = seedRand(gUnpausedTimer);
        u8 intensity = random_in_range(&newSeed, 16) + 240;
        Vec3f pos = {0,0,0};
        if (o->os16F4) {
            pos[0] = o->oPosX - 150;
            pos[1] = o->oPosY + 800;
            if (bhv_flip_desert_object(o, offset) == TRUE) {
                pos[2] = o->oPosZ - 400;
            } else {
                pos[2] = o->oPosZ + 400;
            }
            
        } else {
            pos[0] = o->oPosX + 150;
            pos[1] = o->oPosY + 800;
            if (bhv_flip_desert_object(o, offset) == TRUE) {
                pos[2] = o->oPosZ - 400;
            } else {
                pos[2] = o->oPosZ + 400;
            }
        }
        emit_light(pos, intensity, intensity, intensity, 4, 50, 8, 0);
    } else if (obj_has_model(o, MODEL_DESERT_HOUSE_RGB)) {
        Vec3f pos = {0,0,0};
        pos[1] = o->oPosY + 500;
        pos[2] = o->oPosZ;

        // NOTE: 1750 values represented by model geo translation, needed for music to not sound like it's coming from the wrong spot
        if (isRight) {
            pos[0] = o->oPosX - 200 - 1750;
        } else {
            pos[0] = o->oPosX + 200 + 1750;
        }
        
        modulate_rgb_color(&o->oPrimRGB);
        emit_light(pos, (o->oPrimRGB >> 16) & 0xff, (o->oPrimRGB >> 8) & 0xff, o->oPrimRGB & 0xff, 4, 50, 8, 0);
        print_text_fmt_int(20, 20, "warp %d", o->oInstantWarpSpawn);
        
        if (gCurrentObject->header.gfx.node.flags & GRAPH_RENDER_ACTIVE) {
            play_sound(SOUND_BG1_CARAMELLDANSEN, gCurrentObject->header.gfx.cameraToObject);
        }
    }
}


void bhv_point_light_preview_loop(void) {
    warp_desert_object(o);
}

enum KoopaWaterSellerAction {
    KOOPA_WATER_SELLER_IDLE,
    KOOPA_WATER_SELLER_OFFER_WATER,
    KOOPA_WATER_SELLER_THANK_YOU,
    KOOPA_WATER_SELLER_WATER_FULL,
    KOOPA_BATTERY_SELLER_OFFER_BATTERY,
    KOOPA_BATTERY_SELLER_BATTERY_FULL,
    KOOPA_SELLER_THANK_YOU
};

#define WATER_TEXT_X_POS 20
#define WATER_TEXT_Y_POS 180

#define SELLS_WATER 0 
#define SELLS_BATTERIES 1

#define SELLER_MAX_DISTANCE 400.f

void bhv_koopa_water_seller_set_exclamation_mark(void) {
    o->oAction = KOOPA_WATER_SELLER_IDLE;
    o->oExclamationMarkObject = spawn_object(o, MODEL_EXCLAMATION_MARK, bhvExclamationMark);
}

u8 bhv_koopa_water_seller_update_range(void) {
    if (gMarioCurrentRoom == 2) {
        if (o->oDistanceToMario < SELLER_MAX_DISTANCE) {
            return TRUE;
        } else {
            return FALSE;
        }
    } else {
        return FALSE;
    }

}   

void bhv_koopa_water_seller_idle(void) {
    if (o->oDistanceToMario < SELLER_MAX_DISTANCE) {
        if (BPARAM4 == SELLS_WATER) {
            if (gMarioState->waterLeft < MAX_WATER) {
                o->oAction = KOOPA_WATER_SELLER_OFFER_WATER;
            } else {
                o->oAction = KOOPA_WATER_SELLER_WATER_FULL;
            }
        } else {
            if (gMarioState->batteryMeter < MAX_BATTERIES) {
                o->oAction = KOOPA_BATTERY_SELLER_OFFER_BATTERY;
            } else {
                o->oAction = KOOPA_BATTERY_SELLER_BATTERY_FULL;
            }
        }
    } 
}

void bhv_koopa_water_seller_offer_water(void) {
    if (bhv_koopa_water_seller_update_range() == TRUE) {
        print_small_text_buffered(WATER_TEXT_X_POS, WATER_TEXT_Y_POS, "Press B to buy water for 10 coins", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
        if (gPlayer1Controller->buttonPressed & B_BUTTON) {
            if (o->oExclamationMarkObject != NULL) {
                mark_obj_for_deletion(o->oExclamationMarkObject);
            }
            gMarioState->waterLeft = MAX_WATER;
            o->oAction = KOOPA_WATER_SELLER_THANK_YOU;
        }
    } else {
        o->oAction = KOOPA_WATER_SELLER_IDLE;
    }
}

void bhv_koopa_water_seller_offer_battery(void) {
    if (bhv_koopa_water_seller_update_range() == TRUE) {
        print_small_text_buffered(WATER_TEXT_X_POS, WATER_TEXT_Y_POS, "Press B to buy batteries for 10 coins", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
        if (gPlayer1Controller->buttonPressed & B_BUTTON) {
            if (o->oExclamationMarkObject != NULL) {
                mark_obj_for_deletion(o->oExclamationMarkObject);
            }
            gMarioState->batteryMeter = MAX_BATTERIES;
            o->oAction = KOOPA_WATER_SELLER_THANK_YOU;
        }
    } else {
        o->oAction = KOOPA_WATER_SELLER_IDLE;
    }
}

void bhv_koopa_water_seller_thank_you(void) {
    if (o->oDistanceToMario < 500.f) {
        print_small_text_buffered(WATER_TEXT_X_POS, WATER_TEXT_Y_POS, "Thank you for your business!", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
    } else {
        o->oAction = KOOPA_WATER_SELLER_IDLE;
    }
}

void bhv_koopa_water_seller_water_full(void) {
    if (o->oDistanceToMario < 500.f) {
        print_small_text_buffered(WATER_TEXT_X_POS, WATER_TEXT_Y_POS, "You can't carry any more water!", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
    } else {
        o->oAction = KOOPA_WATER_SELLER_IDLE;
    }
}

void bhv_koopa_water_seller_battery_full(void) {
    if (o->oDistanceToMario < 500.f) {
        print_small_text_buffered(WATER_TEXT_X_POS, WATER_TEXT_Y_POS, "Your batteries are maxed out!", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
    } else {
        o->oAction = KOOPA_WATER_SELLER_IDLE;
    }
}

void bhv_koopa_water_seller_loop(void) {

    switch (o->oAction) {
        case KOOPA_WATER_SELLER_IDLE:
            bhv_koopa_water_seller_idle();
            break;
        case KOOPA_WATER_SELLER_OFFER_WATER:
            bhv_koopa_water_seller_offer_water();
            break;
        case KOOPA_WATER_SELLER_THANK_YOU:
            bhv_koopa_water_seller_thank_you();
            break;
        case KOOPA_WATER_SELLER_WATER_FULL:
            bhv_koopa_water_seller_water_full();
            break;
        case KOOPA_BATTERY_SELLER_OFFER_BATTERY:
            bhv_koopa_water_seller_offer_battery();
            break;
        case KOOPA_BATTERY_SELLER_BATTERY_FULL:
            bhv_koopa_water_seller_battery_full();
            break;
    }

    //gMarioState->inRangeOfWaterSeller = bhv_koopa_water_seller_update_range();
    if (gMarioCurrentRoom == 2) {
        cur_obj_unhide();
    } else {
        cur_obj_hide();
    }
    
}

// Angry sun

enum AngrySunActions {
    ANGRY_SUN_ACT_IDLE,
    ANGRY_SUN_ACT_CIRCLE,
    ANGRY_SUN_ACT_SWOOP,
    ANGRY_SUN_ACT_LEAVE
};

#define SUN_OFFSET_HORIZONTAL   700
#define SUN_OFFSET_VERTICAL     650
#define SUN_CIRCLE_SIZE     300
#define SUN_CIRCLE_SPEED     0x999
#define SUN_SWOOP_SPEED     0x300
#define NUM_SUN_CYCLES     4

void bhv_angry_sun_init(void) {
    o->oF4 = 0;
    o->oF8 = 1;
    gAngrySunPresent = 1;
    o->oHomeY = SUN_OFFSET_VERTICAL;
    s16 sunOffsetSideHorizontalInit = SUN_OFFSET_HORIZONTAL * o->oF8;
    
    o->oHomeX = gMarioState->pos[0] + sunOffsetSideHorizontalInit;
    o->oHomeZ = gMarioState->pos[2];

    o->oPosY = o->oHomeY;
    o->oPosX = o->oHomeX;
    
    o->oPosZ = gMarioState->pos[2];
}

void bhv_angry_sun_loop(void) {

    s16 sunOffsetSideHorizontal = SUN_OFFSET_HORIZONTAL * o->oF8;

    o->oHomeY = SUN_OFFSET_VERTICAL;
    
    o->oHomeX = gMarioState->pos[0] + sunOffsetSideHorizontal;
    o->oHomeZ = gMarioState->pos[2];

    o->oPosZ = gMarioState->pos[2];

    switch (o->oAction) {
        case ANGRY_SUN_ACT_IDLE:
            o->oPosY = approach_f32_asymptotic(o->oPosY, o->oHomeY, 0.10f);
            o->oPosX = approach_f32_asymptotic(o->oPosX, o->oHomeX, 0.10f);

            if (o->oTimer == 30) {
                o->oAction = ANGRY_SUN_ACT_CIRCLE;
            }
        break;
        case ANGRY_SUN_ACT_CIRCLE:
            o->oPosY = approach_f32_asymptotic(o->oPosY, o->oHomeY + (SUN_CIRCLE_SIZE * coss(o->oTimer * SUN_CIRCLE_SPEED)), 0.08f);
            o->oPosX = approach_f32_asymptotic(o->oPosX, o->oHomeX + (SUN_CIRCLE_SIZE * sins(o->oTimer * SUN_CIRCLE_SPEED)), 0.08f);

            if (o->oTimer == 60) {
                o->oAction = ANGRY_SUN_ACT_SWOOP;
            }
        break;
        case ANGRY_SUN_ACT_SWOOP:
            o->oPosY = approach_f32_asymptotic(o->oPosY, o->oHomeY + (-SUN_OFFSET_VERTICAL * sins(o->oTimer * SUN_SWOOP_SPEED)), 0.16f);
            o->oPosX = approach_f32_asymptotic(o->oPosX, gMarioState->pos[0] + (sunOffsetSideHorizontal * coss(o->oTimer * SUN_SWOOP_SPEED)), 0.16f);

            if (o->oTimer == 40) {
                o->oF4++;
                if (o->oF4 >= NUM_SUN_CYCLES) {
                    o->oAction = ANGRY_SUN_ACT_LEAVE;
                }
                else {
                    o->oAction = ANGRY_SUN_ACT_IDLE;
                    o->oF8 *= -1;
                }
            }
        break;
        case ANGRY_SUN_ACT_LEAVE:
            o->oPosY = approach_f32_asymptotic(o->oPosY, 1600, 0.10f);

            if (o->oTimer > 30) {
                gAngrySunPresent = 0;
                mark_obj_for_deletion(o);
            }
    }

    if (o->oDesertTimer > 7) {
        cur_obj_become_tangible();
        cur_obj_check_interacted();
    } else {
        cur_obj_become_intangible();
    }
    
    Vec3f pos = {o->oPosX, o->oPosY + 600, o->oPosZ};
    emit_light(pos, 255, 220, 210, 4, 10, 4, 0);

    if (gMarioCurrentRoom == 2) {
        gAngrySunPresent = 0;
        mark_obj_for_deletion(o);
    }


    
}

// Jukebox

enum JukeboxActions {
    JUKEBOX_ACT_IDLE,
    JUKEBOX_ACT_SHOW_PROMPT,
    JUKEBOX_ACT_NOT_ENOUGH_COINS,
    JUKEBOX_ACT_CHANGE_SONG,
    JUKEBOX_ACT_SUCCESS,
};

void bhv_jukebox_init(void) {
    o->oExclamationMarkObject = spawn_object(o, MODEL_EXCLAMATION_MARK, bhvExclamationMark);
    o->oAction = JUKEBOX_ACT_IDLE;
}

#define CUSTOM_SONGS_START SEQ_KALIMARI_DESERT
#define CUSTOM_SONGS_END SEQ_CROSSING_THOSE_HILLS
#define TRIGGER_DIST 250.f
extern u8 sCurrentBackgroundMusicSeqId;
void bhv_jukebox_loop(void) {
    switch (o->oAction) {
        case JUKEBOX_ACT_IDLE:
            if (o->oDistanceToMario < TRIGGER_DIST) {
                o->oAction = JUKEBOX_ACT_SHOW_PROMPT;
            }
            break;
        case JUKEBOX_ACT_SHOW_PROMPT:
            print_small_text_buffered(20, 180, "Press B to play a song for 25 coins", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
            if (gPlayer1Controller->buttonPressed & B_BUTTON) {
                if (gMarioState->numCoins >= 25) {
                    if (o->oExclamationMarkObject != NULL) {
                        mark_obj_for_deletion(o->oExclamationMarkObject);
                    }
                    gMarioState->numCoins -= 25;
                    o->oAction = JUKEBOX_ACT_CHANGE_SONG;
                } else {
                    o->oAction = JUKEBOX_ACT_NOT_ENOUGH_COINS;
                }
            }
            if (o->oDistanceToMario > TRIGGER_DIST) {
                o->oAction = JUKEBOX_ACT_IDLE;
            }
            break;
        case JUKEBOX_ACT_NOT_ENOUGH_COINS:
            print_small_text_buffered(20, 180, "You don't have enough coins!", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
            if ((o->oDistanceToMario > TRIGGER_DIST) || (o->oTimer > 45)) {
                o->oAction = JUKEBOX_ACT_IDLE;
            }
            break;
        case JUKEBOX_ACT_CHANGE_SONG:
            u8 song = random_u16() % (SEQ_COUNT - CUSTOM_SONGS_START) + CUSTOM_SONGS_START;
            stop_background_music(sCurrentBackgroundMusicSeqId);
            play_music(SEQ_PLAYER_LEVEL, SEQUENCE_ARGS(4, song), 30);
            o->oAction = JUKEBOX_ACT_SUCCESS;
            break;
        case JUKEBOX_ACT_SUCCESS:
            if ((o->oDistanceToMario > TRIGGER_DIST) || (o->oTimer > 45)) {
                o->oAction = JUKEBOX_ACT_IDLE;
            }
            print_small_text_buffered(20, 180, "Song changed!", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
            break;
    }
}

void bhv_exclamation_mark_init(void) {
    o->oPosY += 400;
}

void bhv_exclamation_mark_loop(void) {
    o->oFaceAngleYaw += 0x1000;
    o->oPrimRGB += 1300;
    o->oPosY += sins(o->oPrimRGB) * 1.1f;
    o->oFaceAngleYaw += 100;

    Vec3f pos = {o->oPosX, o->oPosY, o->oPosZ - 200};
    emit_light(pos, 255, 0, 0, 4, 80, 4, 0);
}