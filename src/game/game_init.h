#ifndef GAME_INIT_H
#define GAME_INIT_H

#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include <PR/os_cont.h>
#include <PR/os_message.h>

#include "types.h"
#include "memory.h"
#include "config.h"
#include "emutest.h"

#define MARIO_ANIMS_POOL_SIZE 0x4000
#define DEMO_INPUTS_POOL_SIZE 0x800

// NOTE: For some reason, checking something with index 13 and FBE_CHECK being set to 12 fails on some versions of GlideN64 (pain)
// So apparently this value actually matters...???
#define FBE_PIXEL_OFFSET   13
#define FBE_CHECK          0xFF01

#define INSTANT_INPUT_BLACKLIST (EMU_CONSOLE | EMU_WIIVC | EMU_ARES | EMU_SIMPLE64 | EMU_CEN64)

struct GfxPool {
    Gfx buffer[GFX_POOL_SIZE];
    struct SPTask spTask;
};

struct DemoInput {
    u8 timer; // time until next input. if this value is 0, it means the demo is over
    s8 rawStickX;
    s8 rawStickY;
    u8 buttonMask;
};

enum ZBmodes {
    KEEP_ZBUFFER = 0,
    CLEAR_ZBUFFER = 1,
};

extern struct Controller gControllers[MAXCONTROLLERS];
extern OSContStatus gControllerStatuses[MAXCONTROLLERS];
extern OSContPadEx gControllerPads[MAXCONTROLLERS];
extern OSMesgQueue gGameVblankQueue;
extern OSMesgQueue gGfxVblankQueue;
extern OSMesg gGameMesgBuf[1];
extern OSMesg gGfxMesgBuf[1];
extern struct VblankHandler gGameVblankHandler;
extern uintptr_t gPhysicalFramebuffers[3];
extern uintptr_t gPhysicalZBuffer;
extern void *gMarioAnimsMemAlloc;
extern void *gDemoInputsMemAlloc;
extern struct SPTask *gGfxSPTask;
extern Gfx *gDisplayListHead;
extern u8 *gGfxPoolEnd;
extern struct GfxPool *gGfxPool;
extern u8 gControllerBits;
extern u8 gBorderHeight;
#ifdef VANILLA_STYLE_CUSTOM_DEBUG
extern u8 gCustomDebugMode;
#endif
extern u8 *gAreaSkyboxStart[AREA_COUNT];
extern u8 *gAreaSkyboxEnd[AREA_COUNT];
extern u8 gAmbientR;
extern u8 gAmbientG;
extern u8 gAmbientB;
extern u8 gInstantWarpType;
extern u8 gGoingBackwards;
extern u8 gJustExitedGasStation;
extern u8 gAngrySunPresent;
extern u8 gWaterBottleStolen;
extern u8 gWaterTutorialProgress;
extern u8 gFlashlightTutorialProgress;
extern u8 gNightFirstTime;
extern u8 gStationaryFirstTime;
extern u8 gEnteredGasStationOnce;
#ifdef SIMPLEFLIPS_FALLBACK
extern u8 gSimpleflipsFallbackHappening;
#endif
extern u8 gInflation;
extern u16 gVisitedAreas;
extern s32 gInstantWarpDisplacement;
extern u16 gLow;
extern u16 gHigh;
extern s32 gInstantWarpCounter;
extern s32 gInstantWarpSpawnIndex;
#ifdef EEP
extern s8 gEepromProbe;
#endif
#ifdef SRAM
extern s8 gSramProbe;
#endif

extern void (*gGoddardVblankCallback)(void);
extern struct Controller* const gPlayer1Controller;
extern struct Controller* const gPlayer2Controller;
extern struct Controller* const gPlayer3Controller;
extern struct Controller* const gPlayer4Controller;
extern struct DemoInput *gCurrDemoInput;
extern u16 gDemoInputListID;
extern struct DemoInput gRecordedDemoInput;

// this area is the demo input + the header. when the demo is loaded in, there is a header the size
// of a single word next to the input list. this word is the current ID count.
extern struct DmaHandlerList gMarioAnimsBuf;
extern struct DmaHandlerList gDemoInputsBuf;

extern u8 gMarioAnims[];
extern u8 gDemoInputs[];

extern u16 sRenderingFramebuffer;
extern u32 gGlobalTimer;
extern s32 gUnpausedTimer;
extern u32 gDayTime;
extern Vec3f gLightDirection;

extern u8 gFBEEnabled;
extern u8 rhdcPFPRGBA32[4096];
extern char rhdcUsername[33];

s32 check_fbe(s16 arg0, s32 arg1);
void setup_game_memory(void);
void thread5_game_loop(UNUSED void *arg);
void clear_framebuffer(s32 color);
void clear_viewport(Vp *viewport, s32 color);
void make_viewport_clip_rect(Vp *viewport);
void init_rcp(s32 resetZB);
void end_master_display_list(void);
void render_init(void);
void select_gfx_pool(void);
void display_and_vsync(void);

#endif // GAME_INIT_H
