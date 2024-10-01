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
#include "mtwister.h"
#include "behavior_data.h"
#include "types.h"
#include "include/n64/PR/os_libc.h"
#include "actors/sign_idiot/geo_header.h"

struct DesertSpawnCoords LeftSide = {
    .x = -3000,
    .y = 0,
    .z = -18472,
};

struct DesertSpawnCoords RightSide = {
    .x = 3000,
    .y = 0,
    .z = -18472,
};

struct DesertSpawnCoords Road = {
    .x = 0,
    .y = 0,
    .z = -18472,
};

struct DesertSpawnCoords decide_left_or_right(MTRand *rand) {
    f32 randValue = genRand(rand);
    if (randValue < 0.5f) {
        return LeftSide;
    } else {
        return RightSide;
    }
}

void spawn_electrical_poles(void) {
    spawn_object_desert(gCurrentObject, 0, MODEL_ELECTRICAL_POLE, bhvElectricalPole, LeftSide.x,LeftSide.y,LeftSide.z,0,0,0);
    spawn_object_desert(gCurrentObject, 0, MODEL_ELECTRICAL_POLE, bhvElectricalPole, RightSide.x,RightSide.y,RightSide.z,0,0,0);
}

#define TIER_2_THRESHOLD 20
#define TIER_3_THRESHOLD 30
#define TIER_4_THRESHOLD 40

u16 decide_billboard_model_id(MTRand *rand) {
    u16 maxTier;
    if (gInstantWarpCounter > TIER_3_THRESHOLD) {
        maxTier = MODEL_TIER_4_START - 1;
    } else if (gInstantWarpCounter > TIER_2_THRESHOLD){
        maxTier = MODEL_TIER_3_START - 1;
    } else {
        maxTier = MODEL_TIER_2_START - 1;
    }
    u16 randValue = random_in_range(rand, (maxTier - MODEL_BILLBOARD_START) + 1);
    randValue = randValue + MODEL_BILLBOARD_START;
    return randValue;
}

void spawn_billboard(MTRand *rand) {
    u8 isRight;
    u16 modelID;
    u16 rot;
    struct DesertSpawnCoords spawnCoords = decide_left_or_right(rand);
    isRight = (spawnCoords.x == RightSide.x);

    if (isRight) {
        rot = DEGREES(-45);
    } else {
        rot = DEGREES(45);
    }

    modelID = MODEL_SIGN_IDIOT;

    if (modelID ==  MODEL_SIGN_IDIOT && gUsernameSuccess != 0) {
        bcopy(gAvatarTexture, segmented_to_virtual(sign_idiot_mario_rgba16), 2048);
    }

    //while (modelID == gLastBillboard) {
        //modelID = decide_billboard_model_id(rand);
    //}

    //gLastBillboard = modelID;

    spawn_object_desert(gCurrentObject, 0, modelID, bhvDesertSign, spawnCoords.x,spawnCoords.y,spawnCoords.z,0,rot,0);
}

void spawn_bushes(MTRand *rand) {
    u32 bushesAmount = random_in_range(rand, 5) + 3;
    for (u32 i = 0; i < bushesAmount; i++) {
        u32 bushX = (s32)random_in_range(rand, 501) - 250;
        u32 bushZ = (s32)random_in_range(rand, 501) - 250;
        u32 bushX2 = (s32)random_in_range(rand, 501) - 250;
        u32 bushZ2 = (s32)random_in_range(rand, 501) - 250;
        spawn_object_desert(gCurrentObject, 0, MODEL_BUSH, bhvDesertDecor, LeftSide.x + bushX,LeftSide.y,LeftSide.z + bushZ,0,0,0);
        spawn_object_desert(gCurrentObject, 0, MODEL_BUSH, bhvDesertDecor, RightSide.x + bushX2,RightSide.y,RightSide.z + bushZ2,0,0,0);
    }
}

void spawn_gas_station(void) {
    spawn_object_desert(gCurrentObject, 0, MODEL_GAS_STATION, bhvGasStation, LeftSide.x,LeftSide.y,LeftSide.z,0,0,0);
}


#define GOOMBA_CHANCE 0.25f
#define POKEY_CHANCE 0.25f
#define KLEPTO_CHANCE 0.10f
#define SUN_CHANCE 0.10f

#define ELECTRICAL_POLE_CHANCE 0.25f
#define BUSH_CHANCE 0.25f
#define BILLBOARD_CHANCE 0.25f

void spawn_big_decoration(MTRand *rand) {
    f32 chanceStorage = genRand(rand);
    //print_text_fmt_int(20,20, "Chance: %d", (s32)(chanceStorage * 100));
    chanceStorage -= ELECTRICAL_POLE_CHANCE;
    if (chanceStorage < 0) {
        spawn_electrical_poles();
        return;
    }
    chanceStorage -= BILLBOARD_CHANCE;
    if (chanceStorage < 0) {
        spawn_billboard(rand);
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
    struct Object *obj = spawn_object_desert(gCurrentObject, 0, MODEL_GOOMBA_CUSTOM_MESH, bhvGoomba, Road.x,Road.y,Road.z,0,0,0);
    if (random < 0.1f) {
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
        spawn_object_desert(gCurrentObject, 0, MODEL_NONE, bhvPokey, Road.x,Road.y,Road.z,0,0,0);
        return;
    }
    chanceStorage -= KLEPTO_CHANCE;
    if (chanceStorage < 0) {
        spawn_object_desert(gCurrentObject, 0, MODEL_KLEPTO, bhvKlepto, Road.x,Road.y,Road.z,0,0,0);
        return;
    }
}

void bhv_desert_spawner_loop(void) {
    MTRand newSeed = seedRand(gInstantWarpCounter);
     
    if (gInstantWarpDisplacement) {
        if (gInstantWarpType == INSTANT_WARP_FORWARDS) {
            gVisitedAreas = (gVisitedAreas << 1) & 0b111111111;
        } else if (gInstantWarpType == INSTANT_WARP_BACKWARDS) {
            gVisitedAreas = (gVisitedAreas >> 1) & 0b111111111;
        } else {
            print_text(20,20,"TEST");
        }
        if (!(gVisitedAreas & 0b000010000)) {
            u32 numSmall = random_in_range(&newSeed, 5);
            if (gInstantWarpCounter % 20 == 0) {
                spawn_gas_station();
            } else {
                for (u32 i = 0; i < numSmall; i++) {
                    spawn_small_decoration(&newSeed);
                }
                spawn_big_decoration(&newSeed);
                spawn_enemy(&newSeed);
            }
            gVisitedAreas |= 0b000010000;
        }
    }
}



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

    gMarioState->inRangeOfWaterSeller = bhv_koopa_water_seller_update_range();
    if (gMarioCurrentRoom == 2) {
        cur_obj_unhide();
    } else {
        cur_obj_hide();
    }
    
}