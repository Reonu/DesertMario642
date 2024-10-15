static const s16 globe_anim_globe_rotate_values[] = {
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 
	0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 
	0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0034, 
	0x0130, 0x0387, 0x07C9, 0x0E6E, 0x1772, 0x22A1, 0x2FAD, 0x3E05, 0x4CFA, 
	0x5CE8, 0x6F9D, 0x875C, 0x9F29, 0xB240, 0xC0F7, 0xD65D, 0xFD2E, 0x252A, 
	0x3BC4, 0x4847, 0x5429, 0x6146, 0x6F9E, 0x7D7E, 0x89FB, 0x9516, 0x9F3C, 
	0xA8F7, 0xB26B, 0xBB93, 0xC455, 0xCC8F, 0xD442, 0xDB89, 0xE256, 0xE89E, 
	0xEE50, 0xF35A, 0xF7A9, 0xFB2B, 0xFDC9, 0xFF6D, 0x0000, 0x0000, 0x0000, 
	0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 
	0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 
	0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 
};

static const u16 globe_anim_globe_rotate_indices[] = {
	0x0001, 0x0000, 0x0001, 0x0001, 0x0001, 0x0002, 0x0001, 0x0003, 0x0001, 
	0x0004, 0x0001, 0x0005, 0x0001, 0x0006, 0x0001, 0x0007, 0x0001, 0x0008, 
	0x002C, 0x0009, 0x002C, 0x0035, 0x002C, 0x0061, 
};

static const struct Animation globe_anim_globe_rotate = {
	1,
	0,
	0,
	1,
	44,
	ANIMINDEX_NUMPARTS(globe_anim_globe_rotate_indices),
	globe_anim_globe_rotate_values,
	globe_anim_globe_rotate_indices,
	0,
};

