#include "src/game/envfx_snow.h"

const GeoLayout gas_station_geo[] = {
	GEO_CULLING_RADIUS(600000),
	GEO_OPEN_NODE(),
		GEO_ASM(LAYER_OPAQUE, geo_flicker_env_color),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, gas_station_gas_station_mesh_layer_1),
		GEO_DISPLAY_LIST(LAYER_OPAQUE_DECAL, gas_station_gas_station_mesh_layer_2),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT_DECAL, gas_station_gas_station_mesh_layer_6),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
