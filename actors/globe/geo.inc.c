#include "src/game/envfx_snow.h"

const GeoLayout globe_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_ANIMATED_PART(LAYER_OPAQUE, 5, 116, -1, NULL),
		GEO_OPEN_NODE(),
			GEO_ANIMATED_PART(LAYER_OPAQUE, -44, 0, 0, globe_base_mesh_layer_1),
			GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 31, 103, 0, 0, 0, 28),
			GEO_OPEN_NODE(),
				GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, globe_globe_mesh_layer_1),
			GEO_CLOSE_NODE(),
		GEO_CLOSE_NODE(),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
