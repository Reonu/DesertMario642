#include "src/game/envfx_snow.h"

const GeoLayout sun_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_TRANSPARENT, 0, 0, 0, sun_sun_mesh_layer_5),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
