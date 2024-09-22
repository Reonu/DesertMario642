const Collision gas_station_collision[] = {
	COL_INIT(),
	COL_VERTEX_INIT(76),
	COL_VERTEX(-3648, 0, -380),
	COL_VERTEX(-3648, 771, -380),
	COL_VERTEX(-910, 771, -380),
	COL_VERTEX(-910, 0, -380),
	COL_VERTEX(38, 45, 335),
	COL_VERTEX(-486, 45, 335),
	COL_VERTEX(-486, 45, 1851),
	COL_VERTEX(38, 45, 1851),
	COL_VERTEX(-910, 497, 948),
	COL_VERTEX(-910, 771, 948),
	COL_VERTEX(-910, 771, 1203),
	COL_VERTEX(-910, 497, 1203),
	COL_VERTEX(-486, 0, 1851),
	COL_VERTEX(-486, 0, 335),
	COL_VERTEX(38, 0, 335),
	COL_VERTEX(38, 0, 1851),
	COL_VERTEX(38, 255, 335),
	COL_VERTEX(-486, 255, 335),
	COL_VERTEX(-486, 255, 397),
	COL_VERTEX(38, 255, 397),
	COL_VERTEX(38, 45, 397),
	COL_VERTEX(-486, 45, 397),
	COL_VERTEX(-160, 283, 577),
	COL_VERTEX(-297, 283, 577),
	COL_VERTEX(-297, 283, 736),
	COL_VERTEX(-160, 283, 736),
	COL_VERTEX(-160, 45, 736),
	COL_VERTEX(-297, 45, 736),
	COL_VERTEX(-160, 45, 577),
	COL_VERTEX(-297, 45, 577),
	COL_VERTEX(-910, 0, 2532),
	COL_VERTEX(-910, 771, 2532),
	COL_VERTEX(-3648, 771, 2532),
	COL_VERTEX(-3648, 0, 2532),
	COL_VERTEX(-1138, 497, 1203),
	COL_VERTEX(-1138, 497, 948),
	COL_VERTEX(-910, 0, 948),
	COL_VERTEX(-1138, 0, 948),
	COL_VERTEX(-910, 0, 1203),
	COL_VERTEX(-1138, 0, 1203),
	COL_VERTEX(-3648, 0, 948),
	COL_VERTEX(-3648, 0, 1203),
	COL_VERTEX(-486, 255, 1851),
	COL_VERTEX(-486, 255, 1789),
	COL_VERTEX(-486, 45, 1789),
	COL_VERTEX(38, 255, 1789),
	COL_VERTEX(38, 45, 1789),
	COL_VERTEX(38, 255, 1851),
	COL_VERTEX(-297, 45, 1587),
	COL_VERTEX(-297, 283, 1587),
	COL_VERTEX(-297, 283, 1427),
	COL_VERTEX(-297, 45, 1427),
	COL_VERTEX(-160, 283, 1427),
	COL_VERTEX(-160, 45, 1427),
	COL_VERTEX(-160, 283, 1587),
	COL_VERTEX(-160, 45, 1587),
	COL_VERTEX(-910, 771, 2585),
	COL_VERTEX(360, 771, 2585),
	COL_VERTEX(360, 2065, 2585),
	COL_VERTEX(-910, 2065, 2585),
	COL_VERTEX(-910, 771, -450),
	COL_VERTEX(-3648, 771, -433),
	COL_VERTEX(-3648, 2065, -433),
	COL_VERTEX(-910, 2065, -450),
	COL_VERTEX(-3648, 771, 2585),
	COL_VERTEX(-3648, 2065, 2585),
	COL_VERTEX(-914, 771, -450),
	COL_VERTEX(360, 771, -450),
	COL_VERTEX(-914, 771, 2585),
	COL_VERTEX(360, 2065, -450),
	COL_VERTEX(-910, 771, -379),
	COL_VERTEX(-3648, 771, -379),
	COL_VERTEX(-3648, 771, 2520),
	COL_VERTEX(-910, 771, 2520),
	COL_VERTEX(360, 0, 2532),
	COL_VERTEX(360, 0, -380),
	COL_TRI_INIT(SURFACE_HARD, 72),
	COL_TRI(0, 1, 2),
	COL_TRI(0, 2, 3),
	COL_TRI(4, 5, 6),
	COL_TRI(4, 6, 7),
	COL_TRI(8, 9, 10),
	COL_TRI(8, 10, 11),
	COL_TRI(12, 6, 5),
	COL_TRI(12, 5, 13),
	COL_TRI(14, 4, 7),
	COL_TRI(14, 7, 15),
	COL_TRI(13, 5, 4),
	COL_TRI(13, 4, 14),
	COL_TRI(15, 7, 6),
	COL_TRI(15, 6, 12),
	COL_TRI(16, 17, 18),
	COL_TRI(16, 18, 19),
	COL_TRI(20, 19, 18),
	COL_TRI(20, 18, 21),
	COL_TRI(4, 16, 19),
	COL_TRI(4, 19, 20),
	COL_TRI(5, 17, 16),
	COL_TRI(5, 16, 4),
	COL_TRI(21, 18, 17),
	COL_TRI(21, 17, 5),
	COL_TRI(22, 23, 24),
	COL_TRI(22, 24, 25),
	COL_TRI(26, 25, 24),
	COL_TRI(26, 24, 27),
	COL_TRI(28, 22, 25),
	COL_TRI(28, 25, 26),
	COL_TRI(29, 23, 22),
	COL_TRI(29, 22, 28),
	COL_TRI(27, 24, 23),
	COL_TRI(27, 23, 29),
	COL_TRI(30, 31, 32),
	COL_TRI(30, 32, 33),
	COL_TRI(8, 11, 34),
	COL_TRI(8, 34, 35),
	COL_TRI(36, 8, 35),
	COL_TRI(36, 35, 37),
	COL_TRI(30, 38, 11),
	COL_TRI(11, 10, 31),
	COL_TRI(11, 31, 30),
	COL_TRI(11, 38, 39),
	COL_TRI(11, 39, 34),
	COL_TRI(33, 32, 1),
	COL_TRI(1, 0, 40),
	COL_TRI(41, 33, 1),
	COL_TRI(1, 40, 41),
	COL_TRI(36, 3, 2),
	COL_TRI(2, 9, 8),
	COL_TRI(2, 8, 36),
	COL_TRI(6, 42, 43),
	COL_TRI(6, 43, 44),
	COL_TRI(44, 43, 45),
	COL_TRI(44, 45, 46),
	COL_TRI(46, 45, 47),
	COL_TRI(46, 47, 7),
	COL_TRI(7, 47, 42),
	COL_TRI(7, 42, 6),
	COL_TRI(45, 43, 42),
	COL_TRI(45, 42, 47),
	COL_TRI(48, 49, 50),
	COL_TRI(48, 50, 51),
	COL_TRI(51, 50, 52),
	COL_TRI(51, 52, 53),
	COL_TRI(53, 52, 54),
	COL_TRI(53, 54, 55),
	COL_TRI(55, 54, 49),
	COL_TRI(55, 49, 48),
	COL_TRI(52, 50, 49),
	COL_TRI(52, 49, 54),
	COL_TRI_INIT(SURFACE_INSTANT_WARP_1D, 2),
	COL_TRI(38, 36, 37),
	COL_TRI(38, 37, 39),
	COL_TRI_INIT(SURFACE_DEFAULT, 22),
	COL_TRI(37, 35, 34),
	COL_TRI(37, 34, 39),
	COL_TRI(56, 57, 58),
	COL_TRI(56, 58, 59),
	COL_TRI(60, 61, 62),
	COL_TRI(60, 62, 63),
	COL_TRI(64, 65, 62),
	COL_TRI(64, 62, 61),
	COL_TRI(66, 67, 57),
	COL_TRI(66, 57, 68),
	COL_TRI(58, 57, 67),
	COL_TRI(58, 67, 69),
	COL_TRI(61, 66, 70),
	COL_TRI(61, 70, 71),
	COL_TRI(67, 60, 63),
	COL_TRI(67, 63, 69),
	COL_TRI(64, 56, 59),
	COL_TRI(64, 59, 65),
	COL_TRI(68, 64, 72),
	COL_TRI(68, 72, 73),
	COL_TRI(74, 75, 3),
	COL_TRI(74, 3, 30),
	COL_TRI_STOP(),
	COL_END()
};
