#include "src/game/envfx_snow.h"

const GeoLayout water_bottle_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, water_bottle_water_bottle_mesh_layer_1),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT, water_bottle_water_bottle_mesh_layer_5),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
