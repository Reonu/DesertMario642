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

struct DesertSpawnCoords LeftSide = {
    .x = -3500,
    .y = 0,
    .z = -18472,
};

struct DesertSpawnCoords RightSide = {
    .x = 3500,
    .y = 0,
    .z = -18472,
};

struct DesertSpawnCoords Road = {
    .x = 0,
    .y = 0,
    .z = -18472,
};

void spawn_electrical_poles(void) {
    spawn_object_desert(gCurrentObject, 0, MODEL_ELECTRICAL_POLE, bhvElectricalPole, LeftSide.x,LeftSide.y,LeftSide.z,0,0,0);
    spawn_object_desert(gCurrentObject, 0, MODEL_ELECTRICAL_POLE, bhvElectricalPole, RightSide.x,RightSide.y,RightSide.z,0,0,0);
}

void spawn_bushes(MTRand *rand) {
    u32 bushX = (s32)random_in_range(rand, 501) - 250;
    u32 bushZ = (s32)random_in_range(rand, 501) - 250;
}

f32 decorationChance;
f32 enemyChance;

f32 electricalPoleChance = 0.3f;
f32 goombaChance = 0.3f;
f32 bushChance = 0.3f;

#define GOOMBA_CHANCE 0.25f
#define ELECTRICAL_POLE_CHANCE 0.25f
#define BUSH_CHANCE 0.25f

void bhv_desert_spawner_loop(void) {
    MTRand newSeed = seedRand(gInstantWarpCounter);
    f32 chanceStorage = genRand(&newSeed);
    if (gInstantWarpDisplacement) {
        chanceStorage -= GOOMBA_CHANCE;
        if (chanceStorage < 0) {
            spawn_object_desert(gCurrentObject, 0, MODEL_GOOMBA, bhvGoomba, Road.x,Road.y,Road.z,0,0,0);
        }
        chanceStorage -= ELECTRICAL_POLE_CHANCE;
        if (chanceStorage < 0) {
            spawn_electrical_poles();
        }
    }
}

void bhv_desert_decor_loop(void) {
    warp_desert_object(o);
}