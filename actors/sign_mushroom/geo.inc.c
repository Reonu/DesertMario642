#include "src/game/envfx_snow.h"

const GeoLayout sign_mushroom_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, sign_mushroom_sign_mushroom_mesh_layer_1),
		GEO_DISPLAY_LIST(LAYER_ALPHA, sign_mushroom_sign_mushroom_mesh_layer_4),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
