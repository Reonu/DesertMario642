#include "src/game/envfx_snow.h"

const GeoLayout sign_hackern64_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, sign_hackern64_sign_hackern64_mesh_layer_1),
		GEO_DISPLAY_LIST(LAYER_ALPHA, sign_hackern64_sign_hackern64_mesh_layer_4),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
