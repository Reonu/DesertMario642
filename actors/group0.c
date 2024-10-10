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
#include "gas_station/collision.inc.c"
#include "gas_station/model.inc.c"
#include "koopa_with_shell_custom/model.inc.c"

#include "koopa/anims/data.inc.c"
#include "koopa/anims/table.inc.c"
#include "mario/anims/anim_drinking_water.inc.c"
#include "mario/anims/anim_no_water_left.inc.c"
#include "mario/anims/table.inc.c"

#include "mario/anim_header.h"

#include "pokey_body_custom/model.inc.c"
#include "pokey_head_custom_mesh/model.inc.c"
#include "goomba_custom_mesh/model.inc.c"

#include "klepto/model.inc.c"
#include "klepto/anims/data.inc.c"
#include "klepto/anims/table.inc.c"
#include "ufo/collision.inc.c"
#include "ufo_visual/model.inc.c"
#include "desert_house/model.inc.c"
#include "desert_house_rgb/model.inc.c"
#include "sign_normal/model.inc.c"
#include "sign_video/model.inc.c"
#include "angry_sun/model.inc.c"
#include "jukebox/model.inc.c"
#include "jukebox/collision.inc.c"
#include "exclamation_mark/model.inc.c"
#include "klepto_with_water_bottle/model.inc.c"
#include "water_bottle/model.inc.c"
#include "bus/model.inc.c"
#include "cactus/model.inc.c"