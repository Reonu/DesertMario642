#include "src/game/envfx_snow.h"

const GeoLayout sign_spk_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, sign_spk_sign_spk_mesh_layer_1),
		GEO_DISPLAY_LIST(LAYER_ALPHA, sign_spk_sign_spk_mesh_layer_4),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
