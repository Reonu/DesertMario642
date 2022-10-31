#include <PR/ultratypes.h>
#include "sm64.h"

#include "area.h"
#include "rendering_graph_node.h"
#include "engine/math_util.h"
#include "object_list_processor.h"
#include "level_update.h"
#include "camera.h"
#include "render_fog.h"

struct GlobalFog sWaterFog = {
    .r    = 0x10,
    .g    = 0x2B,
    .b    = 0x29,
    .a    = 0xFF,
    .low  = 920,
    .high = 1000
};

struct GlobalFog sOverrideFog = {
    .r    = 0xB3,
    .g    = 0xBC,
    .b    = 0xDD,
    .a    = 0xFF,
    .low  = 950,
    .high = 1005
};

struct GlobalFog sSnowheadFogArea1 = {
    .r    = 0xB3,
    .g    = 0xBC,
    .b    = 0xDD,
    .a    = 0xFF,
    .low  = 950,
    .high = 1000
};

extern struct GlobalFog gGlobalFog;

s32 sOverride = FALSE;

void update_global_fog_override(
    u8 r,
    u8 g,
    u8 b,
    u8 a,
    s16 low,
    s16 high
) {
    sOverrideFog.r = r;
    sOverrideFog.g = g;
    sOverrideFog.b = b;
    sOverrideFog.a = a;
    sOverrideFog.low = low;
    sOverrideFog.high = high;
    sOverride = TRUE;
}

void disable_fog_override(void) {
    sOverride = FALSE;
}

#define GLOBAL_FOG_UPDATE_RATE_DIVISOR 10

void update_global_fog(void) {
    struct GlobalFog *goalFog;

    if (gCameraIsUnderwater) {
        goalFog = &sWaterFog;
    }
    else if (sOverride) {
        goalFog = &sOverrideFog;
    }
    else {
        switch (gCurrAreaIndex)
        {
        case 1:
        default:
            goalFog = &sSnowheadFogArea1;
            break;
        }
    }

    gGlobalFog.r    = approach_s16_asymptotic(gGlobalFog.r,    goalFog->r,    GLOBAL_FOG_UPDATE_RATE_DIVISOR);
    gGlobalFog.g    = approach_s16_asymptotic(gGlobalFog.g,    goalFog->g,    GLOBAL_FOG_UPDATE_RATE_DIVISOR);
    gGlobalFog.b    = approach_s16_asymptotic(gGlobalFog.b,    goalFog->b,    GLOBAL_FOG_UPDATE_RATE_DIVISOR);
    gGlobalFog.a    = approach_s16_asymptotic(gGlobalFog.a,    goalFog->a,    GLOBAL_FOG_UPDATE_RATE_DIVISOR);
    gGlobalFog.low  = approach_s16_asymptotic(gGlobalFog.low,  goalFog->low,  GLOBAL_FOG_UPDATE_RATE_DIVISOR);
    gGlobalFog.high = approach_s16_asymptotic(gGlobalFog.high, goalFog->high, GLOBAL_FOG_UPDATE_RATE_DIVISOR);
    if (gGlobalFog.high < gGlobalFog.low + 5) gGlobalFog.high = gGlobalFog.low + 5;
}
