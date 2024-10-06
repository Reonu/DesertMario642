#include "src/game/envfx_snow.h"

const GeoLayout sign_normal_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_ASM(LAYER_OPAQUE, geo_billboard_image_scene),
		GEO_ASM(LAYER_ALPHA, geo_billboard_image_scene),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
