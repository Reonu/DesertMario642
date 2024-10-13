#include <ultra64.h>
#include "sm64.h"
#include "behavior_data.h"
#include "model_ids.h"
#include "seq_ids.h"
#include "dialog_ids.h"
#include "segment_symbols.h"
#include "level_commands.h"

#include "game/level_update.h"

#include "levels/scripts.h"

#include "make_const_nonconst.h"
#include "levels/vegas_ending/header.h"

/* Fast64 begin persistent block [scripts] */
/* Fast64 end persistent block [scripts] */

const LevelScript level_vegas_ending_entry[] = {
	INIT_LEVEL(),
	LOAD_YAY0(0x7, _vegas_ending_segment_7SegmentRomStart, _vegas_ending_segment_7SegmentRomEnd), 
	LOAD_YAY0(0xa, _water_skybox_yay0SegmentRomStart, _water_skybox_yay0SegmentRomEnd), 
	ALLOC_LEVEL_POOL(),
	MARIO(MODEL_MARIO, 0x00000001, bhvMario), 
	/* Fast64 begin persistent block [level commands] */
	/* Fast64 end persistent block [level commands] */

	AREA(1, vegas_ending_area_1),
		WARP_NODE(0x0A, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		OBJECT(MODEL_NONE, 4283, -7012, 13455, 0, 0, 0, (255 << 24) | (255 << 16) | (254 << 8), bhvLightEmitter),
		OBJECT(MODEL_NONE, 4283, -7012, 3680, 0, 0, 0, (255 << 24) | (255 << 16) | (254 << 8), bhvLightEmitter),
		OBJECT(MODEL_NONE, 4283, -7012, -9716, 0, 0, 0, (255 << 24) | (255 << 16) | (254 << 8), bhvLightEmitter),
		OBJECT(MODEL_NONE, -14869, -5828, -11620, 0, 0, 0, (255 << 24) | (255 << 16) | (254 << 8), bhvLightEmitter),
		MARIO_POS(0x01, 0, 1708, -9629, 27438),
		OBJECT(MODEL_NONE, 1362, -9629, 27368, 0, 0, 0, (0x0A << 16), bhvInstantActiveWarp),
		TERRAIN(vegas_ending_area_1_collision),
		MACRO_OBJECTS(vegas_ending_area_1_macro_objs),
		STOP_MUSIC(0),
		TERRAIN_TYPE(TERRAIN_GRASS),
		/* Fast64 begin persistent block [area commands] */
		/* Fast64 end persistent block [area commands] */
	END_AREA(),
	FREE_LEVEL_POOL(),
	MARIO_POS(0x01, 0, 1708, -9629, 27438),
	CALL(0, lvl_init_or_update),
	CALL_LOOP(1, lvl_init_or_update),
	CLEAR_LEVEL(),
	SLEEP_BEFORE_EXIT(1),
	EXIT(),
};