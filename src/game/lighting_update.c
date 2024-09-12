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



struct GlobalFog sNightFog = {
    .r = 5,
    .g = 5,
    .b = 30,
    .a = 255,
    .low = 960,
    .high = 995,
};

struct GlobalFog sDawnFog = {
    .r = 255 / AMBIENT_LIGHT_MODIFIER,
    .g = 148 / AMBIENT_LIGHT_MODIFIER,
    .b = 58 / AMBIENT_LIGHT_MODIFIER,
    .a = 255,
    .low = 960,
    .high = 995,
};

struct GlobalFog sDayFog = {
    .r = 200 / AMBIENT_LIGHT_MODIFIER,
    .g = 130 / AMBIENT_LIGHT_MODIFIER,
    .b = 100 / AMBIENT_LIGHT_MODIFIER,
    .a = 255,
    .low = 960,
    .high = 995,
};

struct GlobalFog sDuskFog = {
    .r = 220 / AMBIENT_LIGHT_MODIFIER,
    .g = 92 / AMBIENT_LIGHT_MODIFIER,
    .b = 81 / AMBIENT_LIGHT_MODIFIER,
    .a = 255,
    .low = 960,
    .high = 995,
};


struct DayConfig gDayConfigs[] = {
    // 00:00
    {
        .timeStart = 0,
        .fog = &sNightFog,
        .dirR = 30,
        .dirG = 30,
        .dirB = 30,
    },
    // Dawn. DAY_START to 30 minutes later. Still pre-sunrise lighting.
    {
        .timeStart = DAY_START,
        .fog = &sNightFog,
        .dirR = 30,
        .dirG = 30,
        .dirB = 30,        
    },
    // Sunrise. DAY_START+30min to DAY_START+one hour. Epic sunrise lighting.
    {
        .timeStart = DAY_START + (MINUTE * 30),
        .fog = &sDawnFog,
        .dirR = 160,
        .dirG = 160,
        .dirB = 30,
    },
    // Day. DAY_START+one hour until DAY_END. Day lighting.
    {
        .timeStart = DAY_START + (HOUR * 1),
        .fog = &sDayFog,
        .dirR = 230,
        .dirG = 230,
        .dirB = 180,
    },
    // Dusk start. DAY_END. Still day lighting.
    {
        .timeStart = DAY_END - (HOUR * 1),
        .fog = &sDayFog,
        .dirR = 230,
        .dirG = 230,
        .dirB = 180,
    },
    // Peak dusk. DAY_END+30min. Peak dusk lighting.
    {
        .timeStart = DAY_END - (MINUTE * 30),
        .fog = &sDuskFog,
        .dirR = 160,
        .dirG = 160,
        .dirB = 30,
    },
    // Night start. DAY_END+1 hour. Night lighting.
    {
        .timeStart = DAY_END + (MINUTE * 15),
        .fog = &sNightFog,
        .dirR = 30,
        .dirG = 30,
        .dirB = 30,
    },
    // 24:00
    {
        .timeStart = HOUR * 24,
        .fog = &sNightFog,
        .dirR = 30,
        .dirG = 30,
        .dirB = 30,
    }

};

void set_light_direction(void) {
    s32 angle;
    if ((gUnpausedTimer < DAY_START) || (gUnpausedTimer > DAY_END)) {
        s32 tempTime = gUnpausedTimer;
        if (gUnpausedTimer > (HOUR * 18)) {
            tempTime = (0 - ((HOUR * 24) - tempTime));
        }
        angle = remap(tempTime, HOUR * -6, HOUR * 6, 0, 0x8000);
    } else {
        angle = remap(gUnpausedTimer, DAY_START, DAY_END, 0, 0x8000);
    }
    

    gLightDirection[0] = 0.0f;
    gLightDirection[1] = sins(angle);
    gLightDirection[2] = coss(angle);
}

s32 get_next_day_index(s32 curDay) {
    if ((curDay + 1) > ARRAY_COUNT(gDayConfigs)) {
        return 0;
    } else {
        return curDay + 1;
    }
}

s32 find_day_config(void) {
    int numConfigs = ARRAY_COUNT(gDayConfigs);
    for (int i = 0; i < numConfigs; i++) {
        if ((gUnpausedTimer >= gDayConfigs[i].timeStart && (gUnpausedTimer < gDayConfigs[get_next_day_index(i)].timeStart))) {
            return i;
            break;
        }
    }
    return 69420;
}

void set_light_color(void) {
    s32 curDayConfig;
    s32 nextDayConfig;
    f32 remapTime;
    u8 dirR;
    u8 dirG;
    u8 dirB;
    static u16 fogHigh;
    static u16 fogLow;
    static u8 fogOverride;

    curDayConfig = find_day_config();

    nextDayConfig = get_next_day_index(curDayConfig);

    remapTime = remap(gUnpausedTimer, gDayConfigs[curDayConfig].timeStart,gDayConfigs[nextDayConfig].timeStart,0,1);

    gAmbientR = lerpf(gDayConfigs[curDayConfig].fog->r, gDayConfigs[nextDayConfig].fog->r,remapTime);
    gAmbientG = lerpf(gDayConfigs[curDayConfig].fog->g, gDayConfigs[nextDayConfig].fog->g,remapTime);
    gAmbientB = lerpf(gDayConfigs[curDayConfig].fog->b, gDayConfigs[nextDayConfig].fog->b,remapTime);
    gLow = lerpf(gDayConfigs[curDayConfig].fog->low, gDayConfigs[nextDayConfig].fog->low,remapTime);
    gHigh = lerpf(gDayConfigs[curDayConfig].fog->high, gDayConfigs[nextDayConfig].fog->high,remapTime);

    dirR = lerpf(gDayConfigs[curDayConfig].dirR, gDayConfigs[nextDayConfig].dirR,remapTime);
    dirG = lerpf(gDayConfigs[curDayConfig].dirG, gDayConfigs[nextDayConfig].dirG,remapTime);
    dirB = lerpf(gDayConfigs[curDayConfig].dirB, gDayConfigs[nextDayConfig].dirB,remapTime);

    set_ambient_light(gAmbientR, gAmbientG, gAmbientB);
    set_directional_light(gLightDirection,dirR,dirG,dirB);
    

    //print_text_fmt_int(20,40,"GLOW %d", gLow);
    //print_text_fmt_int(20,60,"GHIGH %d", gHigh);

    //print_text_fmt_int(20, 40, "DAYCONFIG %d",curDayConfig);
    //print_text_fmt_int(20, 60, "NEXTDAY %d",nextDayConfig);

    if (gPlayer1Controller->buttonPressed & L_TRIG) {
        fogOverride ^= 1;
    }

    if (fogOverride) {
        if (gPlayer1Controller->buttonDown & L_JPAD) {
            fogHigh--;
        } else if (gPlayer1Controller->buttonDown & R_JPAD) {
            fogHigh++;
        } else if (gPlayer1Controller->buttonDown & D_JPAD) {
            fogLow--;
        } else if (gPlayer1Controller->buttonDown & U_JPAD) {
            fogLow++;
        }

        update_global_fog_override(gAmbientR, gAmbientG, gAmbientB, 255, fogLow, fogHigh);

        //print_text_fmt_int(20,20,"LOW %d",fogLow);
        //print_text_fmt_int(20,40,"HIGH %d",fogHigh);


    } else {
        update_global_fog_override(gAmbientR, gAmbientG, gAmbientB, 255, gLow, gHigh);
    }

}

void update_lighting(void) {
    int twentyfourhr = (gUnpausedTimer / HOUR);
    set_light_direction();
    set_light_color();
    //print_text_fmt_int(20,20, "TIME %d", twentyfourhr);  
    
    //set_directional_light(gLightDirection, gDayTime, gDayTime, gDayTime);
    //set_ambient_light(gDayTime/3, gDayTime/3, gDayTime/3);
      
}

void bhv_sun_loop(void) {

    if (!BPARAM1) {
        if (gUnpausedTimer > DAY_END || gUnpausedTimer < DAY_START) {
            cur_obj_hide();
        } else {
            cur_obj_unhide();
        }
    } else {
         if (gUnpausedTimer > DAY_END || gUnpausedTimer < DAY_START) {
            cur_obj_unhide();
        } else {
            cur_obj_hide();
        }       
    }

    o->oPosX = gCamera->pos[0] + (gLightDirection[0] * 20000);
    o->oPosY = gCamera->pos[1] + (gLightDirection[1] * 20000);
    o->oPosZ = gCamera->pos[2] + (gLightDirection[2] * 20000);

    // Avoid the sun or moon suddenly snapping when mario gets instant warped
    if (gInstantWarpDisplacement) {
        o->oPosZ += gInstantWarpDisplacement;
    }
}
