#include <ultra64.h>
#include "sm64.h"
#include "surface_terrains.h"
#include "geo_commands.h"

#include "make_const_nonconst.h"

// Note: This bin does not use bin IDs, unlike the other segmented bins.
#include "mario/model.inc.c"

#include "bubble/model.inc.c"

#include "walk_smoke/model.inc.c"

#include "burn_smoke/model.inc.c"

#include "small_water_splash/model.inc.c"

#include "water_wave/model.inc.c"

#include "sparkle/model.inc.c"

#include "water_splash/model.inc.c"

#include "white_particle_small/model.inc.c"

#include "sparkle_animation/model.inc.c"

#ifdef S2DEX_TEXT_ENGINE
#include "src/s2d_engine/s2d_config.h"
#include FONT_C_FILE
#endif

#include "sun/model.inc.c"
#include "moon/model.inc.c"
#include "electrical_pole/collision.inc.c"
#include "electrical_pole/model.inc.c"
#include "bush/model.inc.c"
#include "pokey/model.inc.c"
#include "sign/collision.inc.c"
#include "sign_amogus/model.inc.c"
#include "sign_cheezepin/model.inc.c"
#include "sign_farm/model.inc.c"
#include "sign_spoon/model.inc.c"
#include "sign_joel/model.inc.c"
#include "sign_simpleflips/model.inc.c"
#include "sign_ass_impact/model.inc.c"