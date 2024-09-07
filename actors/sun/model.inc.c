Vtx sun_sun_mesh_layer_1_vtx_cull[8] = {
	{{ {-1062, -2448, 0}, 0, {0, 0}, {0, 0, 0, 0} }},
	{{ {-1062, -333, 0}, 0, {0, 0}, {0, 0, 0, 0} }},
	{{ {-1062, -333, 0}, 0, {0, 0}, {0, 0, 0, 0} }},
	{{ {-1062, -2448, 0}, 0, {0, 0}, {0, 0, 0, 0} }},
	{{ {1062, -2448, 0}, 0, {0, 0}, {0, 0, 0, 0} }},
	{{ {1062, -333, 0}, 0, {0, 0}, {0, 0, 0, 0} }},
	{{ {1062, -333, 0}, 0, {0, 0}, {0, 0, 0, 0} }},
	{{ {1062, -2448, 0}, 0, {0, 0}, {0, 0, 0, 0} }},
};

Vtx sun_sun_mesh_layer_1_vtx_0[18] = {
	{{ {0, -1399, 0}, 0, {496, 496}, {0, 0, 127, 255} }},
	{{ {0, -333, 0}, 0, {496, -16}, {0, 0, 127, 255} }},
	{{ {-385, -405, 0}, 0, {311, 19}, {0, 0, 127, 255} }},
	{{ {385, -405, 0}, 0, {681, 19}, {0, 0, 127, 255} }},
	{{ {718, -611, 0}, 0, {841, 118}, {0, 0, 127, 255} }},
	{{ {955, -924, 0}, 0, {954, 268}, {0, 0, 127, 255} }},
	{{ {1062, -1301, 0}, 0, {1006, 449}, {0, 0, 127, 255} }},
	{{ {1026, -1691, 0}, 0, {988, 636}, {0, 0, 127, 255} }},
	{{ {851, -2042, 0}, 0, {905, 805}, {0, 0, 127, 255} }},
	{{ {561, -2306, 0}, 0, {766, 931}, {0, 0, 127, 255} }},
	{{ {196, -2448, 0}, 0, {590, 999}, {0, 0, 127, 255} }},
	{{ {-196, -2448, 0}, 0, {402, 999}, {0, 0, 127, 255} }},
	{{ {-561, -2306, 0}, 0, {226, 931}, {0, 0, 127, 255} }},
	{{ {-851, -2042, 0}, 0, {87, 805}, {0, 0, 127, 255} }},
	{{ {-1026, -1691, 0}, 0, {4, 636}, {0, 0, 127, 255} }},
	{{ {-1062, -1301, 0}, 0, {-14, 449}, {0, 0, 127, 255} }},
	{{ {-955, -924, 0}, 0, {38, 268}, {0, 0, 127, 255} }},
	{{ {-718, -611, 0}, 0, {151, 118}, {0, 0, 127, 255} }},
};

Gfx sun_sun_mesh_layer_1_tri_0[] = {
	gsSPVertex(sun_sun_mesh_layer_1_vtx_0 + 0, 18, 0),
	gsSP2Triangles(0, 1, 2, 0, 0, 3, 1, 0),
	gsSP2Triangles(0, 4, 3, 0, 0, 5, 4, 0),
	gsSP2Triangles(0, 6, 5, 0, 0, 7, 6, 0),
	gsSP2Triangles(0, 8, 7, 0, 0, 9, 8, 0),
	gsSP2Triangles(0, 10, 9, 0, 0, 11, 10, 0),
	gsSP2Triangles(0, 12, 11, 0, 0, 13, 12, 0),
	gsSP2Triangles(0, 14, 13, 0, 0, 15, 14, 0),
	gsSP2Triangles(0, 16, 15, 0, 0, 17, 16, 0),
	gsSP1Triangle(0, 2, 17, 0),
	gsSPEndDisplayList(),
};


Gfx mat_sun_sun[] = {
	gsDPPipeSync(),
	gsDPSetCombineLERP(0, 0, 0, PRIMITIVE, 0, 0, 0, 1, 0, 0, 0, PRIMITIVE, 0, 0, 0, 1),
	gsDPSetAlphaDither(G_AD_NOISE),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsDPSetPrimColor(0, 0, 255, 234, 198, 255),
	gsSPEndDisplayList(),
};

Gfx mat_revert_sun_sun[] = {
	gsDPPipeSync(),
	gsDPSetAlphaDither(G_AD_DISABLE),
	gsSPEndDisplayList(),
};

Gfx sun_sun_mesh_layer_1[] = {
	gsSPClearGeometryMode(G_LIGHTING),
	gsSPVertex(sun_sun_mesh_layer_1_vtx_cull + 0, 8, 0),
	gsSPSetGeometryMode(G_LIGHTING),
	gsSPCullDisplayList(0, 7),
	gsSPDisplayList(mat_sun_sun),
	gsSPDisplayList(sun_sun_mesh_layer_1_tri_0),
	gsSPDisplayList(mat_revert_sun_sun),
	gsDPPipeSync(),
	gsSPSetGeometryMode(G_LIGHTING),
	gsSPClearGeometryMode(G_TEXTURE_GEN),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 0),
	gsDPSetEnvColor(255, 255, 255, 255),
	gsDPSetAlphaCompare(G_AC_NONE),
	gsSPEndDisplayList(),
};

