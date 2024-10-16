#include "src/game/envfx_snow.h"

const GeoLayout Rock_B_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, Rock_B_Rock_B_mesh_layer_1),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
