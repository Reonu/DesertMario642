#include "src/game/envfx_snow.h"

const GeoLayout sign_bettercallshy_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, sign_bettercallshy_sign_bettercallshy_mesh_layer_1),
		GEO_DISPLAY_LIST(LAYER_ALPHA, sign_bettercallshy_sign_bettercallshy_mesh_layer_4),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
