#include "game/level_update.h"

#define STN_NUM_COUNT 6
#define STN_NUM_TIME_EACH ((s32) (STATIONARY_FREAKOUT_TIME / 6))
#define STN_NUM_START_TIME (STN_NUM_COUNT * STN_NUM_TIME_EACH)
#define STN_NUM_FLICKER_FRAMES 3
#define STN_NUM_FLICKER_COUNT ((STN_NUM_FLICKER_FRAMES * 2) * 3)

void bhv_stationary_number_loop(void) {
    s32 timeRemaining;
    f32 timePercentagePassed;

    if (gMarioStationaryTimer > STATIONARY_FREAKOUT_TIME) {
        obj_mark_for_deletion(o);
        return;
    }

    if (o != o->parentObj) {
        vec3f_copy(&o->oPosVec, &o->parentObj->oPosVec);
        o->oPosY += 240.0f;
    }

    o->oAnimState = MAX((s32) (gMarioStationaryTimer / STN_NUM_TIME_EACH), ORANGE_NUMBER_0);

    cur_obj_unhide();
    if (gMarioStationaryTimer >= STN_NUM_START_TIME) {
        cur_obj_hide();
    }

    if (gMarioStationaryTimer < 0) {
        return;
    }

    timeRemaining = gMarioStationaryTimer % STN_NUM_TIME_EACH;
    timePercentagePassed = 1.0f - ((f32) timeRemaining / STN_NUM_TIME_EACH);

    if (timePercentagePassed < 0.33f) {
        o->oPosY -= 100.0f * (1.0f - sins(0x4000 * (timePercentagePassed / 0.33f)));
    }

    if (timeRemaining < STN_NUM_FLICKER_COUNT) {
        if (((timeRemaining / STN_NUM_FLICKER_FRAMES) % 2) != 0) {
            cur_obj_hide();
        }
    }
}
