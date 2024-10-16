#include <PR/ultratypes.h>

#include "sm64.h"
#include "seq_ids.h"
#include "dialog_ids.h"
#include "audio/external.h"
#include "audio/synthesis.h"
#include "level_update.h"
#include "camera.h"
#include "game_init.h"
#include "level_update.h"
#include "main.h"
#include "engine/math_util.h"
#include "engine/graph_node.h"
#include "engine/surface_load.h"
#include "area.h"
#include "hud.h"
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
#include "levels/desert_intro/header.h"
#include "game/emutest.h"
#include "game/obj_behaviors_2.h"

extern void obj_die_if_health_non_positive(void);

u8 sRockSpawned = 0;
u8 sCactusSpawned = 0;

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

void spawn_butterfly_bombs(void) {
    if (gMarioStationaryTimer >= 0 && gMarioState->health < 0x100) {
        gShouldResetStationaryTimer = TRUE;
    }

    if (gCurrLevelNum == LEVEL_DESERT && gMarioStationaryTimer < 0 && gMarioState->health >= 0x100) {
        if (find_first_object_with_behavior_and_bparams(bhvTripletButterflyDesert, 0, 0) == NULL) {
            gInstantWarpType = INSTANT_WARP_FORWARDS;
            s32 bombAngle = random_u16();
            Vec3f pos;
            vec3f_copy(pos, gMarioState->pos);
            f32 bombDist = 300.0f + (random_float() * 300.0f);

            pos[0] += bombDist * sins(bombAngle);
            pos[2] += bombDist * coss(bombAngle);

            MTRand rand = seedRand(0x12345678);
            struct Object *obj = spawn_object_desert(gCurrentObject, 0, MODEL_BUTTERFLY, bhvTripletButterflyDesert, pos[0], pos[1], pos[2], 0, 0, 0, &rand);
            if (obj) {
                obj->oAction = TRIPLET_BUTTERFLY_ACT_ACTIVATE;
                if (gMarioObject) {
                    obj->oAngleToMario = obj_angle_to_object(obj, gMarioObject);
                }
            }
        }
    }
}

#define TIER_2_THRESHOLD ((s32)(INSTANT_WARPS_GOAL * 0.25f))
#define TIER_3_THRESHOLD ((s32)(INSTANT_WARPS_GOAL * 0.40f))
#define TIER_4_THRESHOLD ((s32)(INSTANT_WARPS_GOAL * 0.55f))
#define FORCE_CREDIT_BILLBOARD 254
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
    u16 rot;
    ModelID32 model = MODEL_BILLBOARD_IMAGE;
    struct DesertSpawnCoords spawnCoords = decide_left_or_right(rand);
    isRight = (spawnCoords.x == RightSide.x);

    if (isRight) {
        rot = DEGREES(-45);
    } else {
        rot = DEGREES(45);
    }

    if (gInstantWarpSpawnIndex == TILES_IN_FRONT_OR_BEHIND + 1) {
        spawn_object_desert(gCurrentObject, 0, MODEL_BILLBOARD_CREDIT, bhvDesertSign, RightSide.x,RightSide.y,RightSide.z,0,rot,0,rand);
        spawn_object_desert(gCurrentObject, 0, MODEL_BILLBOARD_INSPIRED, bhvDesertSign, LeftSide.x,LeftSide.y,LeftSide.z,0,rot,0,rand);

        return;
    } else if (ABS(gInstantWarpSpawnIndex) < (TILES_IN_FRONT_OR_BEHIND + 1) + (TILES_IN_FRONT_OR_BEHIND + 1)) {
        return;
    }
    
    u8 bparam2 = decide_billboard_model_id(rand);

    if (get_desert_sign_video_id(bparam2) >= 0) {
        model = MODEL_BILLBOARD_VIDEO;
    }

    struct Object *obj;
    obj = spawn_object_desert(gCurrentObject, 0, model, bhvDesertSign, spawnCoords.x,spawnCoords.y,spawnCoords.z,0,rot,0,rand);

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

u16 decide_rock_modeL_id(MTRand *rand) {
    //Randomly decide between MODEL_ROCK_A, MODEL_ROCK_B and MODEL_ROCK_C
    u16 rockModel;
    f32 randValue = genRand(rand);
    if (randValue < 0.33f) {
        rockModel = MODEL_ROCK_A;
    } else if (randValue < 0.66f) {
        rockModel = MODEL_ROCK_B;
    } else {
        rockModel = MODEL_ROCK_C;
    }
    return rockModel;
}

#define NORMAL_ROTATION 1
#define RANDOM_ROTATION 2

void spawn_decor_and_rotate(MTRand *rand, u16 modelID, const BehaviorScript *behavior, u8 rotationMode, u16 zPosOffset) {
    u8 isRight;
    struct DesertSpawnCoords spawnCoords = decide_left_or_right(rand);

    isRight = (spawnCoords.x == RightSide.x);
    
    if (rotationMode == NORMAL_ROTATION) {
        if (isRight) {
            spawn_object_desert(gCurrentObject, 0, modelID, behavior, spawnCoords.x,spawnCoords.y,spawnCoords.z,0,DEGREES(0),0,rand);
        } else {
            spawn_object_desert(gCurrentObject, 0, modelID, behavior, spawnCoords.x,spawnCoords.y,spawnCoords.z,0,DEGREES(180),0,rand);
        }
    } else {
        spawn_object_desert(gCurrentObject, 0, modelID, behavior, spawnCoords.x,spawnCoords.y,spawnCoords.z + zPosOffset,0,DEGREES((random_u16() % 360)),0,rand);
    }

}


#define GOOMBA_CHANCE 0.10f
#define POKEY_CHANCE 0.07f
#define KLEPTO_CHANCE 0.03f
#define SUN_CHANCE 0.02f

#define ELECTRICAL_POLE_CHANCE 0.05f

#define BILLBOARD_CHANCE 0.05f
#define UFO_CHANCE 0.01f
#define HOUSE_CHANCE 0.01f

#define BUSH_CHANCE 0.25f
#define CACTUS_CHANCE 0.07f
#define ROCK_CHANCE 0.08f

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
        spawn_decor_and_rotate(rand, MODEL_DESERT_HOUSE, bhvDesertDecor, NORMAL_ROTATION, 0);
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
    chanceStorage -= CACTUS_CHANCE;
    if (chanceStorage < 0 && !sCactusSpawned) {
        spawn_decor_and_rotate(rand, MODEL_CACTUS, bhvDesertDecorWithHitbox, RANDOM_ROTATION, -1800);
        sCactusSpawned = 1;
        return;
    }
    chanceStorage -= ROCK_CHANCE;
    if (chanceStorage < 0 && !sRockSpawned) {
        u16 model = decide_rock_modeL_id(rand);
        spawn_decor_and_rotate(rand, model, bhvDesertDecorWithBigHitbox, RANDOM_ROTATION, -1800);
        sRockSpawned = 1;
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

u8 sBusAlreadySpawned = 0;
void bhv_desert_spawner_init(void) {
    gUnpausedTimer = DAY_START;
    gInstantWarpDisplacement = 0;
    gInstantWarpCounter = 0;
    gInstantWarpType = INSTANT_WARP_FORWARDS;
    gInstantWarpSpawnIndex = TILES_IN_FRONT_OR_BEHIND;
    sBusAlreadySpawned = 0;
}

#define GAS_STATION_SPAWN_INTERVAL 25
#define RGB_HOUSE_WARPS ((s32) (INSTANT_WARPS_GOAL * 0.5f))
#define FUNNY_BUS_WARPS  ((s32) (INSTANT_WARPS_GOAL * 0.667f))
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
        }

        if (gInstantWarpSpawnIndex >= -TILES_IN_FRONT_OR_BEHIND && gInstantWarpSpawnIndex <= TILES_IN_FRONT_OR_BEHIND) {
            return; // Do not generate anything at the start of the run, as it doesn't when you're initially spawned either
        }

        MTRand firstRand = seedRand(gInstantWarpSpawnIndex + 0x80000000);
        u32 actualSeed = genRandLong(&firstRand);
        MTRand newSeed = seedRand(actualSeed);
        u32 numSmall = random_in_range(&newSeed, 5);
        
        if (gInstantWarpSpawnIndex >= INSTANT_WARPS_GOAL) {
            // Only spawn small decorations past the goal
            for (u32 i = 0; i < numSmall; i++) {
                spawn_small_decoration(&newSeed);
                sRockSpawned = 0;
                sCactusSpawned = 0;
            }
            return;
        }

        if (gInstantWarpSpawnIndex == RGB_HOUSE_WARPS - (RGB_HOUSE_WARPS % GAS_STATION_SPAWN_INTERVAL) + (s32) (GAS_STATION_SPAWN_INTERVAL / 2)) {
            spawn_decor_and_rotate(&newSeed, MODEL_DESERT_HOUSE_RGB, bhvDesertDecor, NORMAL_ROTATION, 0); 
        } else if (gInstantWarpSpawnIndex % GAS_STATION_SPAWN_INTERVAL == 0) {
            spawn_gas_station(&newSeed);
        } else {
            for (u32 i = 0; i < numSmall; i++) {
                spawn_small_decoration(&newSeed);
                sRockSpawned = 0;
                sCactusSpawned = 0;
            }
            if (gInstantWarpSpawnIndex == TILES_IN_FRONT_OR_BEHIND + 1) {
                spawn_billboard(&newSeed);
            } else {
                spawn_big_decoration(&newSeed);
            }
            spawn_enemy(&newSeed);
        }

        if (gInstantWarpSpawnIndex >= FUNNY_BUS_WARPS && sBusAlreadySpawned < 2 && (gInstantWarpCounter % GAS_STATION_SPAWN_INTERVAL != 0) && (gMarioCurrentRoom != 2) // No gas station
                    && (gUnpausedTimer > (DAY_END + (HOUR * 2)) || gUnpausedTimer < (DAY_START - (HOUR * 2)))) { // Force nighttime spawn
            
            MTRand newSeed2 = newSeed;
            if (sBusAlreadySpawned == 1) {
                if (find_first_object_with_behavior_and_bparams(bhvBus, 0, 0) == NULL) {
                    sBusAlreadySpawned = 0;
                }
            } else if (spawn_object_desert(gCurrentObject, 0, MODEL_BUS, bhvBus, Road.x,Road.y,Road.z,0,0,0,&newSeed2)) {
                sBusAlreadySpawned = 1;
            }
        }
    }

    spawn_butterfly_bombs();
}

void bhv_desert_spawn_intro_init(void) {
    gUnpausedTimer = DAY_START + (HOUR * 2);
    gInstantWarpDisplacement = 12314;
    gInstantWarpType = INSTANT_WARP_FORWARDS;
    for (gInstantWarpCounter = (-TILES_IN_FRONT_OR_BEHIND * 2); gInstantWarpCounter <= 0; gInstantWarpCounter++) {
        gInstantWarpSpawnIndex = gInstantWarpCounter + TILES_IN_FRONT_OR_BEHIND;
        warp_desert_objects();

        MTRand firstRand = seedRand((u32) gInstantWarpSpawnIndex + 0x80000000);
        u32 actualSeed = genRandLong(&firstRand);
        MTRand newSeed = seedRand(actualSeed);
        u32 numSmall = random_in_range(&newSeed, 16);

        for (u32 i = 0; i < numSmall; i++) {
            spawn_small_decoration(&newSeed);
            sRockSpawned = 0;
            sCactusSpawned = 0;
        }

        if (gInstantWarpCounter == -TILES_IN_FRONT_OR_BEHIND - 1) {
            spawn_decor_and_rotate(&newSeed, MODEL_DESERT_HOUSE, bhvDesertDecor, NORMAL_ROTATION, 0);
        }
    }

    gInstantWarpCounter = 0;
    gInstantWarpSpawnIndex = gInstantWarpCounter + TILES_IN_FRONT_OR_BEHIND;
    gInstantWarpDisplacement = 0;
}


void bhv_desert_spawn_intro_loop(void) {

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
    } else if (o->behavior == segmented_to_virtual(bhvGasStation)) {
        if (gMarioState->action != ACT_SPECIAL_KB_BUS) {
		    load_object_collision_model();
        }
        Vec3f pos = {o->oPosX + 200, o->oPosY + 500, o->oPosZ};
        Vec3f pos2 = {o->oPosX + 200, o->oPosY + 500, o->oPosZ + 2000};
        // Emit a light with the first, second and third byte of oPrimRGB as the RGB values
        emit_light(pos, (o->oPrimRGB >> 16) & 0xff, (o->oPrimRGB >> 8) & 0xff, o->oPrimRGB & 0xff, 3, 45, 8, 0);
        emit_light(pos2, (o->oPrimRGB >> 16) & 0xff, (o->oPrimRGB >> 8) & 0xff, o->oPrimRGB & 0xff, 3, 45, 8, 0);
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
        
        if (o->oDistanceToMario < 3000.f) {
            gMarioState->caramelldansen = TRUE;
        } else {
            gMarioState->caramelldansen = FALSE;
        }
        
        if (gCurrentObject->header.gfx.node.flags & GRAPH_RENDER_ACTIVE) {
            play_sound(SOUND_BG1_CARAMELLDANSEN, gCurrentObject->header.gfx.cameraToObject);
        }
    }
}

// Koopa water/battery seller

enum KoopaWaterSellerAction {
    KOOPA_WATER_SELLER_IDLE,
    KOOPA_WATER_SELLER_OFFER_WATER,
    KOOPA_WATER_SELLER_THANK_YOU,
    KOOPA_WATER_SELLER_WATER_FULL,
    KOOPA_BATTERY_SELLER_OFFER_BATTERY,
    KOOPA_BATTERY_SELLER_BATTERY_FULL,
    KOOPA_SELLER_NOT_ENOUGH_COINS,
};



#define SELLS_WATER 0 
#define SELLS_BATTERIES 1

#define SELLER_MAX_DISTANCE 400.f

#define WATER_PRICE 10
#define BATTERY_PRICE 10

void bhv_koopa_water_seller_set_exclamation_mark(void) {
    o->oAction = KOOPA_WATER_SELLER_IDLE;
    if (o->behavior != (segmented_to_virtual(bhvGasStation)) || (!gEnteredGasStationOnce)) {
        o->oExclamationMarkObject = spawn_object(o, MODEL_EXCLAMATION_MARK, bhvExclamationMark);
    }
    
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
        gMarioState->inRangeOfWaterSeller = TRUE;
        print_small_text_at_slot(WATER_TEXT_X_POS, 1, "Press <COL_1FFF1F-->B<COL_--------> to buy water for <COL_FFFF00-->10<COL_--------> coins.", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
        lock_remaining_text_slots();
        if (gPlayer1Controller->buttonPressed & B_BUTTON) {
            gShouldResetStationaryTimer = TRUE;
            if (gMarioState->numCoins >= WATER_PRICE) {
                if (o->oExclamationMarkObject != NULL) {
                    mark_obj_for_deletion(o->oExclamationMarkObject);
                }
                gMarioState->numCoins -= WATER_PRICE;
                gMarioState->waterLeft = MAX_WATER;
                o->oAction = KOOPA_WATER_SELLER_THANK_YOU;
            } else {
                o->oAction = KOOPA_SELLER_NOT_ENOUGH_COINS;
            }

        }
    } else {
        o->oAction = KOOPA_WATER_SELLER_IDLE;
    }
}

void bhv_koopa_water_seller_offer_battery(void) {
    if (bhv_koopa_water_seller_update_range() == TRUE) {
        gMarioState->inRangeOfWaterSeller = TRUE;
        print_small_text_at_slot(WATER_TEXT_X_POS, 1, "Press <COL_1FFF1F-->B<COL_--------> to buy batteries for <COL_FFFF00-->10<COL_--------> coins.", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
        lock_remaining_text_slots();
        if (gPlayer1Controller->buttonPressed & B_BUTTON) {
            gShouldResetStationaryTimer = TRUE;
            if (gMarioState->numCoins >= BATTERY_PRICE) {
                if (o->oExclamationMarkObject != NULL) {
                    mark_obj_for_deletion(o->oExclamationMarkObject);
                }
                gMarioState->numCoins -= BATTERY_PRICE;
                gMarioState->batteryMeter = MAX_BATTERIES;
                o->oAction = KOOPA_WATER_SELLER_THANK_YOU;
            } else {
                o->oAction = KOOPA_SELLER_NOT_ENOUGH_COINS;
            }
        }
    } else {
        o->oAction = KOOPA_WATER_SELLER_IDLE;
    }
}

void bhv_koopa_water_seller_thank_you(void) {
    if (o->oDistanceToMario < 500.f) {
        print_small_text_at_slot(WATER_TEXT_X_POS, 1, "Thank you for your business!", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
        lock_remaining_text_slots();
    } else {
        o->oAction = KOOPA_WATER_SELLER_IDLE;
    }
}

void bhv_koopa_water_seller_water_full(void) {
    if (o->oDistanceToMario < 500.f) {
        print_small_text_at_slot(WATER_TEXT_X_POS, 1, "You can't carry any more water!", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
        lock_remaining_text_slots();
    } else {
        o->oAction = KOOPA_WATER_SELLER_IDLE;
    }
}

void bhv_koopa_water_seller_battery_full(void) {
    if (o->oDistanceToMario < 500.f) {
        print_small_text_at_slot(WATER_TEXT_X_POS, 1, "Your batteries are maxed out!", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
        lock_remaining_text_slots();
    } else {
        o->oAction = KOOPA_WATER_SELLER_IDLE;
    }
}

void bhv_koopa_water_seller_not_enough_coins(void) {
    if ((o->oDistanceToMario < 500.f) || (o->oTimer < 60)) {
        print_small_text_at_slot(WATER_TEXT_X_POS, 1, "You don't have enough coins!", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
        lock_remaining_text_slots();
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
        case KOOPA_SELLER_NOT_ENOUGH_COINS:
            bhv_koopa_water_seller_not_enough_coins();
            break;
    }

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

    o->oNumLootCoins = NUM_SUN_CYCLES;
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
                o->oHealth = 0;
                obj_die_if_health_non_positive();
            }
    }

    if (o->oDesertTimer > 7) {
        cur_obj_become_tangible();
        if (cur_obj_check_interacted()) {
            o->oNumLootCoins--;
        }
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

struct DesertSeqs {
    const u16 seqId;
    const u8 isPort;
    u8 hasRandomized;
    const char *seqAuthor;
    const char *seqName;
};

struct DesertSeqs seqsToRandomize[] = {
    {.seqId = SEQ_KALIMARI_DESERT,                  .isPort = TRUE,  .hasRandomized = TRUE,  .seqAuthor = "ShrooboidBrat",       .seqName = "Mario Kart 64: Kalimari Desert"}, // NOTE: Kalimari Desert must be first here!
    {.seqId = SEQ_LEVEL_HOT,                        .isPort = FALSE, .hasRandomized = TRUE,  .seqAuthor = "Nintendo",            .seqName = "Super Mario 64: Shifting Sand Land"},
    {.seqId = SEQ_CROSSING_THOSE_HILLS,             .isPort = TRUE,  .hasRandomized = FALSE, .seqAuthor = "scutte",              .seqName = "Final Fantasy IX: Crossing Those Hills"},
    {.seqId = SEQ_ROUTE_203,                        .isPort = TRUE,  .hasRandomized = FALSE, .seqAuthor = "scutte",              .seqName = "Pokemon Legends Arceus: Route 203"},
    {.seqId = SEQ_SMO_SAND_KINGDOM,                 .isPort = TRUE,  .hasRandomized = FALSE, .seqAuthor = "ArcticJaguar725",     .seqName = "Super Mario Odyssey: Sand Kingdom"},
    {.seqId = SEQ_REDC_MANSION_BASEMENT,            .isPort = FALSE, .hasRandomized = FALSE, .seqAuthor = "Capcom",              .seqName = "Resident Evil (DC): Mansion Basement"},
    {.seqId = SEQ_CASCADE_CAPERS_WATERFALL,         .isPort = TRUE,  .hasRandomized = FALSE, .seqAuthor = "scutte",              .seqName = "DKC3: Cascade Capers Waterfall"},
    {.seqId = SEQ_SKY_HIGH_GRAND_NUAGE,             .isPort = TRUE,  .hasRandomized = FALSE, .seqAuthor = "scutte",              .seqName = "MMZX: Sky High - Grand Nuage"},
    {.seqId = SEQ_BEAT_THE_HEAT,                    .isPort = TRUE,  .hasRandomized = FALSE, .seqAuthor = "scutte",              .seqName = "A Hat in Time: Beat the Heat"},
    {.seqId = SEQ_LIVING_WATERWAY,                  .isPort = TRUE,  .hasRandomized = FALSE, .seqAuthor = "scutte",              .seqName = "Cave Story: Living Waterway"},
    {.seqId = SEQ_COSTA_DEL_SOL,                    .isPort = TRUE,  .hasRandomized = FALSE, .seqAuthor = "scutte",              .seqName = "FFVII: Costa del Sol"},
    {.seqId = SEQ_CONFRONTING_MYSELF,               .isPort = TRUE,  .hasRandomized = FALSE, .seqAuthor = "scutte",              .seqName = "Celeste: Confronting Myself"},
    {.seqId = SEQ_DRACULAS_TEARS,                   .isPort = TRUE,  .hasRandomized = FALSE, .seqAuthor = "scutte",              .seqName = "Castlevania DoS: Dracula's Tears"},
    {.seqId = SEQ_EGYPTIAN_DUEL,                    .isPort = TRUE,  .hasRandomized = FALSE, .seqAuthor = "scutte",              .seqName = "Yu-Gi-Oh! FM: Egyptian Duel"},
    {.seqId = SEQ_FIRE_ISLAND_VOLCANO,              .isPort = TRUE,  .hasRandomized = FALSE, .seqAuthor = "scutte",              .seqName = "Pokemon SMD: Fire Island Volcano"},
    {.seqId = SEQ_FIELD,                            .isPort = TRUE,  .hasRandomized = FALSE, .seqAuthor = "scutte",              .seqName = "YSIV: Field"},
    {.seqId = SEQ_RAY_OF_HOPE,                      .isPort = TRUE,  .hasRandomized = FALSE, .seqAuthor = "scutte",              .seqName = "Corpse Party: Ray of Hope"},
    {.seqId = SEQ_SAVANNAH_DESERT,                  .isPort = TRUE,  .hasRandomized = FALSE, .seqAuthor = "scutte",              .seqName = "Sonic Unleashed: Savannah Desert"},
    {.seqId = SEQ_ARID_SANDS,                       .isPort = TRUE,  .hasRandomized = FALSE, .seqAuthor = "scutte",              .seqName = "Sonic Unleashed: Arid Sands"},
    {.seqId = SEQ_RAIN_FORMERLY_KNOWN_AS_PURPLE,    .isPort = FALSE, .hasRandomized = FALSE, .seqAuthor = "Chris Christodoulou", .seqName = "RoR2: The Rain Formerly Known as Purple"},
    {.seqId = SEQ_PIZZA_TIME,                       .isPort = TRUE,  .hasRandomized = FALSE, .seqAuthor = "Teraok",              .seqName = "Pizza Tower: Pizza Time"},
    {.seqId = SEQ_SEA_ME_NOW,                       .isPort = TRUE,  .hasRandomized = FALSE, .seqAuthor = "Teraok",              .seqName = "Splatoon 3: Sea me now"},
    {.seqId = SEQ_PEPPERMAN_STRIKES,                .isPort = TRUE,  .hasRandomized = FALSE, .seqAuthor = "sm64pie",             .seqName = "Pizza Tower: Pepperman Strikes"},
    {.seqId = SEQ_JELLYFISH_FIELDS,                 .isPort = TRUE,  .hasRandomized = FALSE, .seqAuthor = "JoshTheBosh",         .seqName = "BfBB: Jellyfish Fields"},
    {.seqId = SEQ_CHINATOWN,                        .isPort = TRUE,  .hasRandomized = FALSE, .seqAuthor = "Teraok",              .seqName = "Katana Zero: Chinatown"},
    {.seqId = SEQ_FROM_RUSSIA_WITH_LOVE,            .isPort = TRUE,  .hasRandomized = FALSE, .seqAuthor = "Teraok",              .seqName = "From Russia With Love"},
    {.seqId = SEQ_NSANITY_ISLAND,                   .isPort = TRUE,  .hasRandomized = FALSE, .seqAuthor = "Teraok",              .seqName = "Crash Twinsanity: N. Sanity Island"},
    {.seqId = SEQ_GRASS_N_SEAS,                     .isPort = FALSE, .hasRandomized = FALSE, .seqAuthor = "Luminator",           .seqName = "Grass N Seas"},
    {.seqId = SEQ_POP_N_BUBBLE_INC,                 .isPort = FALSE, .hasRandomized = FALSE, .seqAuthor = "Luminator",           .seqName = "Pop N Bubble Inc."},
    {.seqId = SEQ_SELECT_YOUR_LEVEL,                .isPort = FALSE, .hasRandomized = FALSE, .seqAuthor = "SpK",                 .seqName = "Select Your Level"},
    {.seqId = SEQ_SHOP_THEME,                       .isPort = FALSE, .hasRandomized = FALSE, .seqAuthor = "Luminator",           .seqName = "Shop Theme"},
    {.seqId = SEQ_THE_MEAT_GRINDER,                 .isPort = FALSE, .hasRandomized = FALSE, .seqAuthor = "SpK",                 .seqName = "The Meat Grinder"},
    {.seqId = SEQ_XBC_MECHONIS_FIELD,               .isPort = FALSE, .hasRandomized = FALSE, .seqAuthor = "Monolith Soft",       .seqName = "Xenoblade Chronicles (DE): Mechonis Field"},
    {.seqId = SEQ_PKMN_RSE_MT_CHIMNEY,              .isPort = TRUE,  .hasRandomized = FALSE, .seqAuthor = "ArcticJaguar725",     .seqName = "Pokemon Ruby & Sapphire: Mt. Chimney"},
    {.seqId = SEQ_MGTT_FOREST_STAR,                 .isPort = TRUE,  .hasRandomized = FALSE, .seqAuthor = "ArcticJaguar725",     .seqName = "Mario Golf (TT): Lakitu Valley 2"},
};
u8 lastSeqRandomizedIndex = 0;

static u32 jukebox_generate_randomized_track(void) {
    u32 randomizedIndex;
    u32 numSeqsToRandomize = 0;
    for (s32 i = 0; i < ARRAY_COUNT(seqsToRandomize); i++) {
        if (seqsToRandomize[i].hasRandomized == FALSE) {
            numSeqsToRandomize++;
        }
    }

    if (numSeqsToRandomize == 0) {
        // Out of new sequences, reset pool
        for (u32 i = 0; i < ARRAY_COUNT(seqsToRandomize); i++) {
            seqsToRandomize[i].hasRandomized = FALSE;
        }

        randomizedIndex = (u32) (random_float() * (ARRAY_COUNT(seqsToRandomize) - 1));

        // Do not repeat the last sequence!
        if (randomizedIndex >= lastSeqRandomizedIndex) {
            randomizedIndex++;
        }

        seqsToRandomize[randomizedIndex].hasRandomized = TRUE;
        lastSeqRandomizedIndex = randomizedIndex;
        return randomizedIndex;
    }

    randomizedIndex = (u32) (random_float() * numSeqsToRandomize);
    for (u32 i = 0; i < ARRAY_COUNT(seqsToRandomize); i++) {
        if (seqsToRandomize[i].hasRandomized == FALSE) {
            if (randomizedIndex == 0) {
                // Found match, return here!
                seqsToRandomize[i].hasRandomized = TRUE;
                lastSeqRandomizedIndex = i;
                return i;
            }

            randomizedIndex--;
        }
    }

    assert(FALSE, "Theoretically impossible randomized sequence!");
    return 0;
}

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

#define TRIGGER_DIST 250.f
extern u8 sCurrentBackgroundMusicSeqId;
void bhv_jukebox_loop(void) {
    static char str[128];
    s32 seqIndex;

#ifdef DESERT_DEBUG
    static s32 debugPlayerIndex = 0;
    if (gPlayer1Controller->buttonPressed & L_JPAD) {
        debugPlayerIndex--;
        if (debugPlayerIndex < 0) {
            debugPlayerIndex = ARRAY_COUNT(seqsToRandomize) - 1;
        }
        set_background_music(0, seqsToRandomize[debugPlayerIndex].seqId, 0);
        o->oAction = JUKEBOX_ACT_SUCCESS;
        o->oPrevAction = JUKEBOX_ACT_SUCCESS;
        o->oSubAction = 1;
        o->oTimer = 0;
    } else if (gPlayer1Controller->buttonPressed & R_JPAD) {
        debugPlayerIndex++;
        if (debugPlayerIndex >= ARRAY_COUNT(seqsToRandomize)) {
            debugPlayerIndex = 0;
        }
        set_background_music(0, seqsToRandomize[debugPlayerIndex].seqId, 0);
        o->oAction = JUKEBOX_ACT_SUCCESS;
        o->oPrevAction = JUKEBOX_ACT_SUCCESS;
        o->oSubAction = 1;
        o->oTimer = 0;
    }
#endif

    switch (o->oAction) {
        case JUKEBOX_ACT_IDLE:
            if (o->oDistanceToMario < TRIGGER_DIST) {
                o->oAction = JUKEBOX_ACT_SHOW_PROMPT;
            }
            break;
        case JUKEBOX_ACT_SHOW_PROMPT:
            gMarioState->inRangeOfWaterSeller = TRUE;
            print_small_text_at_slot(20, 0, "Press <COL_1FFF1F-->B<COL_--------> to play a random song for <COL_FFFF00-->25<COL_--------> coins.", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
            lock_remaining_text_slots();
            if (gPlayer1Controller->buttonPressed & B_BUTTON) {
                gShouldResetStationaryTimer = TRUE;
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
            print_small_text_at_slot(20, 0, "You don't have enough coins!", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
            lock_remaining_text_slots();
            if ((o->oDistanceToMario > TRIGGER_DIST) || (o->oTimer > 45)) {
                o->oAction = JUKEBOX_ACT_IDLE;
            }
            break;
        case JUKEBOX_ACT_CHANGE_SONG:
            o->oDesertSequenceIndex = jukebox_generate_randomized_track();
            set_background_music(0, seqsToRandomize[o->oDesertSequenceIndex].seqId, 0);
            gBGMusicActive = TRUE;
            o->oAction = JUKEBOX_ACT_SUCCESS;
            break;
        case JUKEBOX_ACT_SUCCESS:
            if ((o->oTimer >= 120)) {
                o->oAction = JUKEBOX_ACT_IDLE;
            }

            seqIndex = o->oDesertSequenceIndex;
#ifdef DESERT_DEBUG
            if (o->oSubAction == 1) {
                seqIndex = debugPlayerIndex;
            }
#endif
            print_small_text_at_slot(20, 3, "Song changed!", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
            sprintf(str, "<COL_3FFF3F-->%s<COL_-------->", seqsToRandomize[seqIndex].seqName);
            print_small_text_at_slot(20, 1, str, TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
            if (seqsToRandomize[seqIndex].seqAuthor) {
                if (seqsToRandomize[seqIndex].isPort) {
                    sprintf(str, "Music port by: <COL_FF1F6F-->%s<COL_-------->", seqsToRandomize[seqIndex].seqAuthor);
                } else {
                    sprintf(str, "<COL_FF1F6F-->%s<COL_-------->", seqsToRandomize[seqIndex].seqAuthor);
                }
                print_small_text_at_slot(20, 0, str, TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
            }
            lock_remaining_text_slots();
            break;
    }

#ifdef DESERT_DEBUG
    if (gMarioCurrentRoom == 2 || o->oAction == JUKEBOX_ACT_SUCCESS) {
#else
    if (gMarioCurrentRoom == 2) {
#endif
        cur_obj_unhide();
    } else {
        o->oAction = JUKEBOX_ACT_IDLE;
        cur_obj_hide();
    }
}

// Custom exclamation point

void bhv_exclamation_mark_init(void) {
    o->oPosY += 400;
    if ((o->parentObj != NULL) && (o->parentObj->behavior == segmented_to_virtual(bhvGasStation))) {
        o->oPosX = -3600;
        o->oPosZ = o->parentObj->oPosZ + 1100;
        o->oDesiredRoom = 1;
    } else {
        o->oDesiredRoom = 2;
    }
}

void bhv_exclamation_mark_loop(void) {
    o->oFaceAngleYaw += 0x1000;
    o->oPrimRGB += 1300;
    o->oPosY += sins(o->oPrimRGB) * 1.1f;
    o->oFaceAngleYaw += 100;

    Vec3f pos = {o->oPosX, o->oPosY, o->oPosZ + 200};
    if (gMarioCurrentRoom == o->oDesiredRoom) {
        cur_obj_unhide();
        emit_light(pos, 255, 0, 0, 4, 80, 4, 0);
    } else {
        cur_obj_hide();
    }

    if (o->oDesiredRoom == 1 && gEnteredGasStationOnce) {
        mark_obj_for_deletion(o);
    }
}

void bhv_water_bottle_init(void) {

}

void bhv_water_bottle_loop(void) {
    o->oFaceAngleYaw += 0x1000;
    o->oPrimRGB += 1300;
    o->oPosY += sins(o->oPrimRGB) * 1.1f;
    o->oFaceAngleYaw += 100;    

    if (o->oDistanceToMario < 200.f) {
        if (gMarioState->waterLeft < MAX_WATER) {
            gMarioState->waterLeft = o->oKleptoStoleWaterBottle;
            play_sound(SOUND_GENERAL_COLLECT_1UP, gGlobalSoundSource);
        }
        mark_obj_for_deletion(o);
    }
}

// Cringe tutorial code that I didn't want to write

u8 sCurrentTutorial;
u32 uTutorialTimer;

enum TutorialSteps {
    TUTORIAL_WATER_START,
    TUTORIAL_WATER_END,
    TUTORIAL_MUSIC,
    TUTORIAL_CAMERA_1,
    TUTORIAL_CAMERA_2,
    TUTORIAL_STATIONARY_START,
    TUTORIAL_STATIONARY_END,
    TUTORIAL_FLASHLIGHT_START,
    TUTORIAL_FLASHLIGHT_END,
    TUTORIAL_DONE,
};

void choose_tutorial(void) {
    if (!gWaterTutorialProgress) {
        sCurrentTutorial = TUTORIAL_WATER_START;
    } else if (gWaterTutorialProgress == 1) {
        sCurrentTutorial = TUTORIAL_WATER_END;
    } else if (gWaterTutorialProgress == 2) {
        sCurrentTutorial = TUTORIAL_CAMERA_1;
    } else if (gWaterTutorialProgress == 3) {
        sCurrentTutorial = TUTORIAL_CAMERA_2;
    } else if (gWaterTutorialProgress == 4) {
        sCurrentTutorial = TUTORIAL_MUSIC;
    } else if (gStationaryFirstTime == 1) {
        sCurrentTutorial = TUTORIAL_STATIONARY_START;
    } else if (gStationaryFirstTime == 2) {
        sCurrentTutorial = TUTORIAL_STATIONARY_END;
    } else if (gNightFirstTime == 1) {
        sCurrentTutorial = TUTORIAL_FLASHLIGHT_START;
    } else if (gNightFirstTime == 2) {
        sCurrentTutorial = TUTORIAL_FLASHLIGHT_END;
    } else {
        sCurrentTutorial = TUTORIAL_DONE;
    }

    if (gUnpausedTimer >= DAY_END && !gNightFirstTime) {
        gNightFirstTime = 1;
    }
}

void run_tutorial(void) {
    static u8 alpha;
    switch (sCurrentTutorial) {
        case TUTORIAL_WATER_START:
            alpha = 255;
            bzero(gCurrEnvCol, sizeof(gCurrEnvCol));
            print_set_envcolour(255, 255, 255, alpha);
            print_small_text_at_slot(WATER_TEXT_X_POS, 1, "You're dehydrated, so you run slowly.", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
            print_small_text_at_slot(WATER_TEXT_X_POS, 0, "Press <COL_FF1F1F-->R<COL_--------> to drink water.", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
            lock_remaining_text_slots();
            if (gMarioState->action == ACT_DRINKING_WATER) {
                gWaterTutorialProgress++;
                uTutorialTimer = 0;
            }
            break;
        case TUTORIAL_WATER_END:
            bzero(gCurrEnvCol, sizeof(gCurrEnvCol));
            print_set_envcolour(255, 255, 255, alpha);
            print_small_text_at_slot(WATER_TEXT_X_POS, 1, "If you run out of water, you can", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
            print_small_text_at_slot(WATER_TEXT_X_POS, 0, "buy more at a gas station.", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
            lock_remaining_text_slots();
            if (uTutorialTimer++ >= 210) {
                gWaterTutorialProgress++;
                uTutorialTimer = 0;
            } else {
                alpha = 255;
            }
            break;
        case TUTORIAL_CAMERA_1:
            bzero(gCurrEnvCol, sizeof(gCurrEnvCol));
            print_set_envcolour(255, 255, 255, alpha);
            print_small_text_at_slot(WATER_TEXT_X_POS, 1, "The game's camera may be maneuvered", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
            print_small_text_at_slot(WATER_TEXT_X_POS, 0, "and adjusted using the <COL_3FFFFF-->D-Pad<COL_-------->.", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
            lock_remaining_text_slots();
            if (uTutorialTimer++ >= 210) {
                gWaterTutorialProgress++;
                uTutorialTimer = 0;
            } else {
                alpha = 255;
            }
            break;
        case TUTORIAL_CAMERA_2:
            bzero(gCurrEnvCol, sizeof(gCurrEnvCol));
            print_set_envcolour(255, 255, 255, alpha);
            print_small_text_at_slot(WATER_TEXT_X_POS, 1, "<COL_3FFFFF-->D-Pad Up/Down<COL_--------> adjusts the sensitivity.", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
            print_small_text_at_slot(WATER_TEXT_X_POS, 0, "<COL_3FFFFF-->D-Pad Left/Right<COL_--------> rotates by 90 degrees.", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
            lock_remaining_text_slots();
            if (uTutorialTimer++ >= 210) {
                gWaterTutorialProgress++;
                uTutorialTimer = 0;
            } else {
                alpha = 255;
            }
            break;
        case TUTORIAL_MUSIC:
            bzero(gCurrEnvCol, sizeof(gCurrEnvCol));
            print_set_envcolour(255, 255, 255, alpha);
            print_small_text_at_slot(WATER_TEXT_X_POS, 2, "If you ever find yourself tired of the", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
            print_small_text_at_slot(WATER_TEXT_X_POS, 1, "game's background music, it may be muted", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
            print_small_text_at_slot(WATER_TEXT_X_POS, 0, "by pressing the <COL_BFBF00-->START<COL_--------> button.", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
            lock_remaining_text_slots();
            if (uTutorialTimer++ >= 150) {
                alpha = remap(uTutorialTimer, 150, 210, 255, 0);
                if (uTutorialTimer >= 210) {
                    gWaterTutorialProgress++;
                    uTutorialTimer = 0;
                }
            } else {
                alpha = 255;
            }
            break;
        case TUTORIAL_STATIONARY_START:
            alpha = 255;
            
            if (!gMarioState->isDead) {
                bzero(gCurrEnvCol, sizeof(gCurrEnvCol));
                print_set_envcolour(255, 191, 191, alpha);
                print_small_text_at_slot(WATER_TEXT_X_POS, 2, "Uh oh!  It looks like Mario isn't trying", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
                print_small_text_at_slot(WATER_TEXT_X_POS, 1, "hard enough.  Get moving, before Mario", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
                print_small_text_at_slot(WATER_TEXT_X_POS, 0, "starts taking damage!", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
                lock_remaining_text_slots();
            }

            uTutorialTimer++;
            if (uTutorialTimer > 120 && gMarioStationaryTimer > STATIONARY_FREAKOUT_TIME) {
                gStationaryFirstTime++;
                uTutorialTimer = 0;
            }           
            break;
        case TUTORIAL_STATIONARY_END:
            if (!gMarioState->isDead) {
                bzero(gCurrEnvCol, sizeof(gCurrEnvCol));
                print_set_envcolour(255, 255, 255, alpha);
                print_small_text_at_slot(WATER_TEXT_X_POS, 1 + (sPowerMeterHUD.animation == POWER_METER_HIDDEN ? 0 : 2), "You will perish if you're caught lounging", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
                print_small_text_at_slot(WATER_TEXT_X_POS, 0 + (sPowerMeterHUD.animation == POWER_METER_HIDDEN ? 0 : 2), "around.  Do not lose sight of the end goal!", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
                lock_remaining_text_slots();
            }
            if (uTutorialTimer++ >= 150) {
                alpha = remap(uTutorialTimer, 150, 210, 255, 0);
                if (uTutorialTimer >= 210) {
                    gStationaryFirstTime++;
                    uTutorialTimer = 0;
                }
            } else {
                alpha = 255;
            }
            break;
        case TUTORIAL_FLASHLIGHT_START:
            alpha = 255;
            bzero(gCurrEnvCol, sizeof(gCurrEnvCol));
            print_set_envcolour(255, 255, 255, alpha);
            print_small_text_at_slot(WATER_TEXT_X_POS, 1, "It's getting dark.", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
            print_small_text_at_slot(WATER_TEXT_X_POS, 0, "Press <COL_7F9FFF-->L<COL_--------> to turn on your flashlight.", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT); 
            lock_remaining_text_slots();
            if (gMarioState->flashlightOn) {
                gNightFirstTime++;
                uTutorialTimer = 0;
            }           
            break;
        case TUTORIAL_FLASHLIGHT_END:
            bzero(gCurrEnvCol, sizeof(gCurrEnvCol));
            print_set_envcolour(255, 255, 255, alpha);
            print_small_text_at_slot(WATER_TEXT_X_POS, 1, "Your flashlight needs batteries.", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
            print_small_text_at_slot(WATER_TEXT_X_POS, 0, "You can buy batteries at a gas station.", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
            lock_remaining_text_slots();
            if (uTutorialTimer++ >= 150) {
                alpha = remap(uTutorialTimer, 150, 210, 255, 0);
                if (uTutorialTimer >= 210) {
                    gNightFirstTime++;
                    uTutorialTimer = 0;
                }
            } else {
                alpha = 255;
            }
            break;
        case TUTORIAL_DONE:
        default:
            break;
    }
    choose_tutorial();
}

// Epic bus

#define BASE_BUS_SPEED 330.f
enum BusActions {
    BUS_ACT_EARLY_SPAWN,
    BUS_ACT_BEFORE_HITTING_MARIO,
    BUS_ACT_AFTER_HITTING_MARIO,
};

void bhv_bus_init(void) {
    o->oPosY += 380;
    o->oPosX = gMarioState->pos[0];
    o->oFloat100 = 1.f;
    o->oFloat104 = 0.0f; // Pitch multiplier
    o->oFloat108 = ABS(o->oPosZ); // Last distance to Mario
}

void bhv_bus_before_hitting_mario(void) {
    cur_obj_unhide();
    if (o->oDistanceToMario > 1000.f) {
        o->oPosX = approach_f32_asymptotic(o->oPosX, gMarioState->pos[0], 0.071f);
    }
    o->oFloat100 = 1.f;
    if (gMarioState->action == ACT_SPECIAL_KB_BUS) {
        o->oAction = BUS_ACT_AFTER_HITTING_MARIO;
    }

    o->oPosZ += BASE_BUS_SPEED * o->oFloat100;
}

void bhv_bus_after_hitting_mario(void) {
    cur_obj_unhide();
    o->oFloat100 += 0.009f;

    o->oPosZ += BASE_BUS_SPEED * o->oFloat100;

}

void bhv_bus_loop(void) {
    switch (o->oAction) {
        case BUS_ACT_EARLY_SPAWN:
            if (o->oTimer > 150) {
                o->oAction++;
            } else if (o->oTimer < 25 || (o->oTimer >= 75 && o->oTimer < 110)) {
                play_sound(SOUND_FG1_VENGABEEP, gCurrentObject->header.gfx.cameraToObject);
            }
            cur_obj_hide();
            o->oFloat108 += BASE_BUS_SPEED;
            break;
        case BUS_ACT_BEFORE_HITTING_MARIO:
            if (o->oTimer >= 25 && o->oTimer < 100) {
                play_sound(SOUND_FG1_VENGABEEP, gCurrentObject->header.gfx.cameraToObject);
            }
            sBusAlreadySpawned = 2;
            bhv_bus_before_hitting_mario();
            break;
        case BUS_ACT_AFTER_HITTING_MARIO:
            if (o->oTimer < 30) {
                play_sound(SOUND_FG1_VENGABEEP, gCurrentObject->header.gfx.cameraToObject);
            }
            sBusAlreadySpawned = 2;
            bhv_bus_after_hitting_mario();
            break;
    }

    delete_if_mario_in_gas_station(o);
    if (o->activeFlags == ACTIVE_FLAG_DEACTIVATED) {
        if (o->oAction == BUS_ACT_EARLY_SPAWN) {
            sBusAlreadySpawned = 0;
        }
        return;
    }

    if (o->oAction != BUS_ACT_EARLY_SPAWN){
        Vec3f pos = {o->oPosX, o->oPosY + 800, o->oPosZ + 1500};
        emit_light(pos, 255, 255, 255, 1, 10, 8, 0);
    }

    o->oDistanceToMario = dist_between_objects(o, gMarioObject); // Update with new distance
    o->oFloat104 = (o->oFloat108 - o->oDistanceToMario) / 10000.0f;
    play_sound(SOUND_BG1_VENGAMUSIC, gCurrentObject->header.gfx.cameraToObject);
    play_sound(SOUND_BG2_VENGAENGINE, gCurrentObject->header.gfx.cameraToObject);

    o->oFloat108 = o->oDistanceToMario;
}

// Lakitu

enum LakituActions {
    LAKITU_ACT_INVISIBLE,
    LAKITU_ACT_APPEAR,
    LAKITU_ACT_STAY,
    LAKITU_ACT_LEAVE,
};

#define LAKITU_APPEAR_THRESHOLD 120 
#define LAKITU_DISAPPEAR_THRESHOLD 5
#define LAKITU_ANIMATION_SPEED 0.1f // Speed at which Lakitu moves up and down

void bhv_lakitu_act_invisible(void) {
    cur_obj_hide();

    // Stay very high up, outside the screen
    o->oPosY = 2000.0f;

    if (gMarioState->vel[2] > 0 && gMarioState->action != ACT_SPECIAL_KB_BUS) {
        if (o->oLakituStatusTimer == 0) {
            o->oLakituLastZPos = gMarioState->pos[2];
            o->oLakituStatusTimer++;
        } else {
            if (gInstantWarpDisplacement) {
                o->oLakituLastZPos += gInstantWarpDisplacement;
            }

            if (ABS(gMarioState->pos[2] - o->oLakituLastZPos) >= 2000.0f) {
                o->oAction = LAKITU_ACT_APPEAR;
                o->oLakituStatusTimer = 0;
                o->oLakituLastZPos = 0.0f;
            }
        }
    } else {
        o->oLakituStatusTimer = 0;
    }
}

void bhv_lakitu_act_stay(void) {
    // Bop up and down
    o->oPosY = o->oHomeY - sins(o->oTimer * 0x800) * 50.0f;
    if (gMarioState->vel[2] > 0 || o->oTimer == 0) {
        o->oLakituLastZPos = gMarioState->pos[2];
    } else {
        if (gInstantWarpDisplacement) {
            o->oLakituLastZPos += gInstantWarpDisplacement;
        } 
        
        if (ABS(gMarioState->pos[2] - o->oLakituLastZPos) >= 200.0f) {
            o->oAction = LAKITU_ACT_LEAVE;
            o->oLakituStatusTimer++;
        }
    }

    if (o->oLakituStatusTimer > LAKITU_DISAPPEAR_THRESHOLD) {
        o->oAction = LAKITU_ACT_LEAVE;
        o->oLakituStatusTimer = 0;
        o->oLakituLastZPos = 0.0f;
    }
}

void bhv_lakitu_act_appear(void) {
    // Swoop down from the top of the screen
    o->oPosY = approach_f32_asymptotic(o->oPosY, o->oHomeY - 150.0f, LAKITU_ANIMATION_SPEED);
    if (o->oPosY < o->oHomeY) {
        o->oAction = LAKITU_ACT_STAY;
        o->oPrevAction = LAKITU_ACT_STAY;
        o->oTimer = 0;
        bhv_lakitu_act_stay(); // Update on the same frame
    }
}

void bhv_lakitu_act_disappear(void) {
    if (o->oTimer == 0) {
        o->oVelY = 0;
    }

    if (o->oTimer < 3) {
        o->oVelY -= 3;
    } else {
        o->oVelY += 5;
    }

    o->oPosY += o->oVelY;
    if (o->oPosY >= 2000.0f) {
        o->oPosY = 2000.0f;
        o->oVelY = 0;
        o->oAction = LAKITU_ACT_INVISIBLE;
    }
}

void bhv_lakitu_nuh_uh_loop(void) {    
    cur_obj_unhide();

    switch (o->oAction) {
        case LAKITU_ACT_INVISIBLE:
            bhv_lakitu_act_invisible();
            break;
        case LAKITU_ACT_APPEAR:
            bhv_lakitu_act_appear();
            break;
        case LAKITU_ACT_STAY:
            bhv_lakitu_act_stay();
            break;
        case LAKITU_ACT_LEAVE:
            bhv_lakitu_act_disappear();
            break;
    }

    s16 angle = atan2s(gLakituState.curPos[2] - o->oPosZ, gLakituState.curPos[0] - o->oPosX) + 0x8000;
    o->oFaceAngleYaw = approach_s16_asymptotic(o->oFaceAngleYaw, angle, 0.125f);
    o->oPosX = approach_f32_asymptotic(o->oPosX, gLakituState.curPos[0] - (2500.0f * sins(gLakituState.yaw)), 0.125f);
    o->oPosZ = approach_f32_asymptotic(o->oPosZ, gLakituState.curPos[2] - (2500.0f * coss(gLakituState.yaw)), 0.125f);

    // stay in front of mario
    if (o->oAction != LAKITU_ACT_INVISIBLE) {
        f32 intensity = 1.0f - ((o->oPosY - o->oHomeY) / (2000.0f - o->oHomeY));
        intensity = MIN(intensity, 1.0f);
        intensity = MAX(intensity, 0.0f);
        Vec3f pos = {o->oPosX - sins(o->oFaceAngleYaw) * 350, o->oPosY + 800, o->oPosZ - coss(o->oFaceAngleYaw) * 350};
        emit_light(pos, (255.0f * intensity) + 0.5f, 0, 0, 4, 50, 8, 0);
    }

    if (gMarioCurrentRoom == 2 || gMarioState->action == ACT_SPECIAL_KB_BUS) {
        o->oAction = LAKITU_ACT_INVISIBLE;
        o->oLakituStatusTimer = 0;
    }
}

#define WAIT_FRAMES 15
#define FADE_IN_FRAMES (WAIT_FRAMES + 20)
#define FADE_OUT_FRAMES (FADE_IN_FRAMES + 45)
#define ANIM_CYCLE_FRAMES 180
#define PRESS_A_FRAMES_START 120
#define CREDIT_FRAMES_START 140
void render_title_logo(void) {
    static s32 timer = 0;
    static u8 currentText;

    if (gCurrLevelNum != LEVEL_DESERT_INTRO) {
        timer = 0;
        return;
    }

    if (timer != __INT32_MAX__) {
        timer++;
    }

    if (timer < WAIT_FRAMES) {
        return;
    }

    f32 translateVal = (5.0f * sins(0x2C00 + 0x10000 * (timer - FADE_IN_FRAMES) / ANIM_CYCLE_FRAMES));
    f32 rotVal = 1.0f * sins(0xE000 + 0x10000 * (timer - FADE_IN_FRAMES) / ANIM_CYCLE_FRAMES);

    if (timer < (2.0f * ANIM_CYCLE_FRAMES) + FADE_IN_FRAMES) {
        if (timer < FADE_IN_FRAMES) {
            translateVal = 0.0f;
            rotVal = 0.0f;
        } else {
            translateVal *= (f32) (timer - FADE_IN_FRAMES) / (2.0f * ANIM_CYCLE_FRAMES);
            rotVal *= (f32) (timer - FADE_IN_FRAMES) / (2.0f * ANIM_CYCLE_FRAMES);
        }
    }

    gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    create_dl_ortho_matrix();
    create_dl_translation_matrix(MENU_MTX_NOPUSH, SCREEN_CENTER_X, (SCREEN_HEIGHT * 0.667f) + translateVal, 0.0f);
    create_dl_rotation_matrix(MENU_MTX_NOPUSH, rotVal, 0.0f, 0.0f, 1.0f);
	gDPSetCombineLERP(gDisplayListHead++, TEXEL0, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0);

    if (timer < FADE_IN_FRAMES) {
        f32 amount = 1.0f - coss(0x4000 * ((f32) (timer - WAIT_FRAMES) / (FADE_IN_FRAMES - WAIT_FRAMES)));
        amount = sqr(amount);

        create_dl_scale_matrix(MENU_MTX_NOPUSH, amount, amount, 1.0f);
        gDPSetPrimColor(gDisplayListHead++, 0, 0, 255, 255, 255, (u8) ((255 * amount) + 0.5f));
        gSPDisplayList(gDisplayListHead++, logoMesh_logoMesh_mesh_layer_5);
	    gDPSetCombineLERP(gDisplayListHead++, 0, 0, 0, 1, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, 1, TEXEL0, 0, PRIMITIVE, 0);
        gDPSetPrimColor(gDisplayListHead++, 0, 0, 255, 255, 255, (u8) ((255 * sqr(amount)) + 0.5f));
        gSPDisplayList(gDisplayListHead++, logoMesh_logoMesh_mesh_layer_5);
    } else if (timer < FADE_OUT_FRAMES) {
        f32 amount = 1.0f - ((f32) (timer - FADE_IN_FRAMES) / (FADE_OUT_FRAMES - FADE_IN_FRAMES));
        f32 scale = ((f32) (timer - FADE_IN_FRAMES) / (FADE_OUT_FRAMES - FADE_IN_FRAMES)) * 0.15f;

        gDPSetPrimColor(gDisplayListHead++, 0, 0, 255, 255, 255, 255);
        gSPDisplayList(gDisplayListHead++, logoMesh_logoMesh_mesh_layer_5);
        create_dl_scale_matrix(MENU_MTX_NOPUSH, 1.0f + scale, 1.0f + scale, 1.0f);
        gDPSetPrimColor(gDisplayListHead++, 0, 0, 255, 255, 255, (u8) ((255 * amount) + 0.5f));
	    gDPSetCombineLERP(gDisplayListHead++, 0, 0, 0, 1, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, 1, TEXEL0, 0, PRIMITIVE, 0);
        gSPDisplayList(gDisplayListHead++, logoMesh_logoMesh_mesh_layer_5);
    } else {
        gDPSetPrimColor(gDisplayListHead++, 0, 0, 255, 255, 255, 255);
        gSPDisplayList(gDisplayListHead++, logoMesh_logoMesh_mesh_layer_5);
    }

    gDPSetRenderMode(gDisplayListHead++, G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2);

    if (o->oTimer > PRESS_A_FRAMES_START) {
        if (gPlayer1Controller->buttonPressed & (A_BUTTON | B_BUTTON | START_BUTTON)) {
            level_trigger_warp(gMarioState, WARP_OP_DESERT_SPAWN);
        }

        bzero(gCurrEnvCol, sizeof(gCurrEnvCol));
        if (timer < PRESS_A_FRAMES_START + 45) {
            print_set_envcolour(255, 255, 255, remap(uTutorialTimer, (timer - PRESS_A_FRAMES_START), (timer - PRESS_A_FRAMES_START) + 45, 255, 0));
        } else {
            print_set_envcolour(255, 255, 255, 255);
        }
        print_small_text(SCREEN_CENTER_X, SCREEN_HEIGHT * 3 / 4 - 42, "Press <COL_BFBF00-->START<COL_--------> To Begin!", TEXT_ALIGN_CENTER, PRINT_ALL, FONT_DEFAULT);
        static s32 textFrame;
        textFrame = (timer - PRESS_A_FRAMES_START) % 120;
        f32 textAlphaPrev = sins((textFrame * 0x8000) / 120);
        s32 textAlpha = (s32) (textAlphaPrev * 255);
        currentText = ((timer - PRESS_A_FRAMES_START) / 120) % 3;
        switch (currentText) {
            case 0x00:
                print_set_envcolour(191, 191, 191, textAlpha);
                print_small_text(SCREEN_CENTER_X, SCREEN_HEIGHT * 3 / 4 - 14, "Hack by", TEXT_ALIGN_CENTER, PRINT_ALL, FONT_DEFAULT);
                print_set_envcolour(255, 48, 127, textAlpha);
                print_small_text(SCREEN_CENTER_X, SCREEN_HEIGHT * 3 / 4 +  2, "Reonu", TEXT_ALIGN_CENTER, PRINT_ALL, FONT_DEFAULT);
                break;
            case 0x01:
                print_set_envcolour(191, 191, 191, textAlpha);
                print_small_text(SCREEN_CENTER_X, SCREEN_HEIGHT * 3 / 4 - 14, "Logo by", TEXT_ALIGN_CENTER, PRINT_ALL, FONT_DEFAULT);
                print_set_envcolour(255, 48, 127, textAlpha);
                print_small_text(SCREEN_CENTER_X, SCREEN_HEIGHT * 3 / 4 +  2, "Shiru", TEXT_ALIGN_CENTER, PRINT_ALL, FONT_DEFAULT);
                break;
            case 0x02:
                print_set_envcolour(191, 191, 191, textAlpha);
                print_small_text(SCREEN_CENTER_X, SCREEN_HEIGHT * 3 / 4 - 14, "Special thanks to:", TEXT_ALIGN_CENTER, PRINT_ALL, FONT_DEFAULT);
                print_set_envcolour(255, 48, 127, textAlpha);
                print_small_text(SCREEN_CENTER_X - (SCREEN_CENTER_X * 3 / 8), SCREEN_HEIGHT * 3 / 4 +  2, "ArcticJaguar725", TEXT_ALIGN_CENTER, PRINT_ALL, FONT_DEFAULT);
                print_set_envcolour(255, 48, 127, textAlpha);
                print_small_text(SCREEN_CENTER_X - (SCREEN_CENTER_X * 3 / 8), SCREEN_HEIGHT * 3 / 4 + 20, "thecozies", TEXT_ALIGN_CENTER, PRINT_ALL, FONT_DEFAULT);
                print_set_envcolour(255, 48, 127, textAlpha);
                print_small_text(SCREEN_CENTER_X + (SCREEN_CENTER_X * 3 / 8), SCREEN_HEIGHT * 3 / 4 +  2, "Wiseguy", TEXT_ALIGN_CENTER, PRINT_ALL, FONT_DEFAULT);
                print_set_envcolour(255, 48, 127, textAlpha);
                print_small_text(SCREEN_CENTER_X + (SCREEN_CENTER_X * 3 / 8), SCREEN_HEIGHT * 3 / 4 + 20, "Simpleflips", TEXT_ALIGN_CENTER, PRINT_ALL, FONT_DEFAULT);
                break;
        }

    }
}

void bhv_light_emitter_loop(void) {
    Vec3f pos = {o->oPosX, o->oPosY, o->oPosZ};

    u16 r = BPARAM1;
    u16 g = BPARAM2;
    u16 b = BPARAM3;

    // Apply a random modifier between -10 and +10 (capped at 255 and 0) to the color values to have a flickering effect
    r = CLAMP((random_u16() % 20 - 10 + r), 0, 255);
    g = CLAMP((random_u16() % 20 - 10 + g), 0, 255);
    b = CLAMP((random_u16() % 20 - 10 + b), 0, 255);


    emit_light(pos, (u8)r, (u8)b, (u8)b, 1, 10, 8, 0);

}
enum GlobeAnims {
    GLOBE_ANIM_IDLE,
    GLOBE_ANIM_SPIN,
};

enum GlobeActions {
    GLOBE_ACT_IDLE,
    GLOBE_ACT_SHOW_PROMPT,
    GLOBE_ACT_SPIN,
    GLOBE_ACT_NOT_ENOUGH_COINS,
    GLOBE_ACT_SUCCESS,
};

void bhv_globe_init(void) {
    o->oAction = GLOBE_ACT_IDLE;
    o->oGlobePrice = 10;
}

void bhv_globe_set_exclamation_mark(void) {
    o->oExclamationMarkObject = spawn_object(o, MODEL_EXCLAMATION_MARK, bhvExclamationMark);
}

void bhv_globe_idle(void) {
    if (o->oDistanceToMario < TRIGGER_DIST) {
        o->oAction = GLOBE_ACT_SHOW_PROMPT;
    }
    cur_obj_init_animation(GLOBE_ANIM_IDLE);
}

void bhv_globe_show_prompt(void) {
    gMarioState->inRangeOfWaterSeller = TRUE;
    char priceText[48];
    sprintf(priceText, "for <COL_FFFF00-->%u<COL_--------> coins.", (u16) o->oGlobePrice);
    print_small_text_at_slot(WATER_TEXT_X_POS, 1, "Press <COL_1FFF1F-->B<COL_--------> to find out where you are", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
    print_small_text_at_slot(WATER_TEXT_X_POS, 0, priceText, TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
    lock_remaining_text_slots();
    cur_obj_init_animation(GLOBE_ANIM_IDLE);
    if (gPlayer1Controller->buttonPressed & B_BUTTON) {
        gShouldResetStationaryTimer = TRUE;
        if (gMarioState->numCoins >= (u16) o->oGlobePrice) {
            gMarioState->numCoins -= (u16) o->oGlobePrice;
            o->oGlobePrice = (u16) (o->oGlobePrice * 1.5f);
            o->oAction = GLOBE_ACT_SPIN;
        } else {
            o->oAction = GLOBE_ACT_NOT_ENOUGH_COINS;
        }
    }
    if (o->oDistanceToMario > TRIGGER_DIST) {
        o->oAction = GLOBE_ACT_IDLE;
    }
}

void bhv_globe_spin(void) {
    cur_obj_init_animation(GLOBE_ANIM_SPIN);
    if (cur_obj_check_if_at_animation_end()) {
        o->oAction = GLOBE_ACT_SUCCESS;
    }
    if (o->oExclamationMarkObject != NULL) {
        mark_obj_for_deletion(o->oExclamationMarkObject);
    }
    gShouldResetStationaryTimer = TRUE;
}

void bhv_globe_success(void) {
    char text[64];
    u16 progress = (u16) ((gInstantWarpCounter / (f32) INSTANT_WARPS_GOAL) * 100);
    sprintf(text, "You are <COL_BF7FDF-->%d%%<COL_--------> of the way there!", progress);
    print_small_text_at_slot(20, 0, text, TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
    cur_obj_init_animation(GLOBE_ANIM_IDLE);
    if (o->oTimer >= 120) {
        o->oAction = GLOBE_ACT_IDLE;
    }
}

void bhv_globe_not_enough_coins(void) {
    print_small_text_at_slot(20, 0, "You don't have enough coins!", TEXT_ALIGN_LEFT, PRINT_ALL, FONT_DEFAULT);
    lock_remaining_text_slots();
    cur_obj_init_animation(GLOBE_ANIM_IDLE);
    if ((o->oDistanceToMario > TRIGGER_DIST) || (o->oTimer > 45)) {
        o->oAction = GLOBE_ACT_IDLE;
    }
}

void bhv_globe_loop(void) {
    switch (o->oAction) {
        case GLOBE_ACT_IDLE:
            bhv_globe_idle();
            break;
        case GLOBE_ACT_SHOW_PROMPT:
            bhv_globe_show_prompt();
            break;
        case GLOBE_ACT_SPIN:
            bhv_globe_spin();
            break;
        case GLOBE_ACT_SUCCESS:
            bhv_globe_success();
            break;
        case GLOBE_ACT_NOT_ENOUGH_COINS:
            bhv_globe_not_enough_coins();
            break;
    }
    if (gMarioCurrentRoom == 2) {
        cur_obj_unhide();
    } else {
        cur_obj_hide();
    }
}