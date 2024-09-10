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
 



void bhv_desert_spawner_loop (void) {
MTRand newSeed = seedRand(gInstantWarpCounter);

if (gInstantWarpDisplacement) {
    if (genRand(&newSeed) < 0.8f) {
        spawn_object_relative(0,0,250,0,gCurrentObject,MODEL_GOOMBA,bhvGoomba);
        print_text(20,140,"TEST2");
    }
    print_text(20,120,"TEST");
}

}