#include "src/game/envfx_snow.h"

const GeoLayout desert_house_rgb_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_ASM(LAYER_OPAQUE, geo_set_prim_color),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, desert_house_rgb_desert_house_rgb_mesh_layer_1),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
