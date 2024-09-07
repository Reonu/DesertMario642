#include "src/game/envfx_snow.h"

const GeoLayout moon_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_ALPHA, 0, 0, 0, moon_moon_mesh_layer_4),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
