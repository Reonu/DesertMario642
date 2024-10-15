#include "src/game/envfx_snow.h"

const GeoLayout billboard_inspired_by_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, billboard_inspired_by_sign_billboard_credit_001_mesh_layer_1),
		GEO_DISPLAY_LIST(LAYER_ALPHA, billboard_inspired_by_sign_billboard_credit_001_mesh_layer_4),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
