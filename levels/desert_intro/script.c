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
#include "levels/desert_intro/header.h"

/* Fast64 begin persistent block [scripts] */
/* Fast64 end persistent block [scripts] */

const LevelScript level_desert_intro_entry[] = {
	INIT_LEVEL(),
	LOAD_YAY0(0x7, _desert_intro_segment_7SegmentRomStart, _desert_intro_segment_7SegmentRomEnd), 
	ALLOC_LEVEL_POOL(),
	MARIO(MODEL_MARIO, 0x00000001, bhvMario),
	/* Fast64 begin persistent block [level commands] */
	/* Fast64 end persistent block [level commands] */

	AREA(1, desert_intro_area_1),
		WARP_NODE(0x0A, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0xF0, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0xF1, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		INSTANT_WARP(0x00, 0x01, 0, 0, 12314),
		INSTANT_WARP(0x01, 0x01, 0, 0, -12314),
		INSTANT_WARP(0x02, 0x01, -15080, 0, -1062),
		INSTANT_WARP(0x03, 0x01, 15080, 0, 1062),
		OBJECT(MODEL_JUKEBOX, -19339, 0, 1158, 0, 0, 0, 0x00000000, bhvJukebox),
		OBJECT(MODEL_KOOPA_WITH_SHELL_CUSTOM, -20936, 0, 916, 0, -180, 0, 0x00000000, bhvKoopaCustom),
		OBJECT(MODEL_KOOPA_WITH_SHELL_CUSTOM, -20135, 0, 916, 0, -180, 0, (1), bhvKoopaCustom),
		OBJECT(MODEL_LAKITU_CUSTOM, 253, 0, 993, 0, 0, 0, 0x00000000, bhvLakituNuhUh),
		MARIO_POS(0x01, 0, 0, 0, 0),
		OBJECT(MODEL_NONE, 0, 0, 0, 0, 0, 0, 0x000A0000, bhvSpinAirborneWarp),
		OBJECT(MODEL_NONE, -75, 0, 2030, 0, 0, 0, 0x00000000, bhvDesertSpawner),
		OBJECT(MODEL_MOON, 0, 0, 0, 0, 0, 0, (1 << 24), bhvSun),
		OBJECT(MODEL_SUN, 0, 0, 0, 0, 0, 0, 0x00000000, bhvSun),
		TERRAIN(desert_intro_area_1_collision),
		ROOMS(desert_intro_area_1_collision_rooms),
		MACRO_OBJECTS(desert_intro_area_1_macro_objs),
		SET_BACKGROUND_MUSIC(0x00, SEQ_KALIMARI_DESERT),
		TERRAIN_TYPE(TERRAIN_SAND),
		/* Fast64 begin persistent block [area commands] */
		/* Fast64 end persistent block [area commands] */
	END_AREA(),
	FREE_LEVEL_POOL(),
	MARIO_POS(0x01, 0, 0, 0, 0),
	CALL(0, lvl_init_or_update),
	CALL_LOOP(1, lvl_init_or_update),
	CLEAR_LEVEL(),
	SLEEP_BEFORE_EXIT(1),
	EXIT(),
};