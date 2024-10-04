#include "src/game/envfx_snow.h"

const GeoLayout desert_house_rgb_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_ASM(LAYER_OPAQUE, geo_set_prim_color),
		GEO_TRANSLATE_NODE_WITH_DL(LAYER_OPAQUE, -1750, 0, 0, desert_house_rgb_desert_house_rgb_mesh_layer_1),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
