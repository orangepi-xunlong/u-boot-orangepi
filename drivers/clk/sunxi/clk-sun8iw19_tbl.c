/*
 * Copyright (C) 2016 Allwinnertech
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Adjustable factor-based clock implementation
 */

#include "clk-sun8iw19.h"

/*
 * freq table from hardware, need follow rules
 * 1)   each table  named as
 *      factor_pll1_tbl
 *      factor_pll2_tbl
 *      ...
 * 2) for each table line
 *      a) follow the format PLLx(n, k, m, p, d1, d2, freq), and keep the
 *         factors order
 *      b) if any factor not used, skip it
 *      c) the factor is the value to write registers, not means factor + 1
 *
 *      example
 *      PLL1(9, 0, 0, 2, 60000000) means PLL1(n, k, m, p, freq)
 *      PLLVIDEO0(3, 0, 96000000) means PLLVIDEO0(n, m, freq)
 *
 */

/* PLLCPU(n, m, p, freq)	F_N8X8_M0X2_P16x2 */
struct sunxi_clk_factor_freq factor_pllcpu_tbl[] = {
PLLCPU(41,     0,     0,     1008000000U),
};

/* PLLDDR0(n, d1, d2, freq)	F_N8X8_D1V1X1_D2V0X1 */
/* ISP VE DE CSI ISE device also use this table */
struct sunxi_clk_factor_freq factor_pllddr0_tbl[] = {
PLLDDR0(27,     0,     0,     672000000U),
};

/* PLLPERIPH0(n, d1, d2, freq)	F_N8X8_D1V1X1_D2V0X1 */
/* periph0 periph1 use this table */
struct sunxi_clk_factor_freq factor_pllperiph0_tbl[] = {
PLLPERIPH0(49,     0,     0,     600000000U),
};

/* PLLVIDEO0(n, d1, freq)	F_N8X8_D1V1X1 */
struct sunxi_clk_factor_freq factor_pllvideo0_tbl[] = {
PLLVIDEO0(23,     1,     72000000U),
PLLVIDEO0(24,     1,     75000000U),
PLLVIDEO0(12,     0,     78000000U),
PLLVIDEO0(26,     1,     81000000U),
PLLVIDEO0(13,     0,     84000000U),
PLLVIDEO0(28,     1,     87000000U),
PLLVIDEO0(14,     0,     90000000U),
PLLVIDEO0(30,     1,     93000000U),
PLLVIDEO0(15,     0,     96000000U),
PLLVIDEO0(32,     1,     99000000U),
PLLVIDEO0(16,     0,     102000000U),
PLLVIDEO0(34,     1,     105000000U),
PLLVIDEO0(17,     0,     108000000U),
PLLVIDEO0(36,     1,     111000000U),
PLLVIDEO0(18,     0,     114000000U),
PLLVIDEO0(38,     1,     117000000U),
PLLVIDEO0(19,     0,     120000000U),
PLLVIDEO0(40,     1,     123000000U),
PLLVIDEO0(20,     0,     126000000U),
PLLVIDEO0(42,     1,     129000000U),
PLLVIDEO0(21,     0,     132000000U),
PLLVIDEO0(44,     1,     135000000U),
PLLVIDEO0(22,     0,     138000000U),
PLLVIDEO0(46,     1,     141000000U),
PLLVIDEO0(23,     0,     144000000U),
PLLVIDEO0(48,     1,     147000000U),
PLLVIDEO0(24,     0,     150000000U),
PLLVIDEO0(50,     1,     153000000U),
PLLVIDEO0(25,     0,     156000000U),
PLLVIDEO0(52,     1,     159000000U),
PLLVIDEO0(26,     0,     162000000U),
PLLVIDEO0(54,     1,     165000000U),
PLLVIDEO0(27,     0,     168000000U),
PLLVIDEO0(56,     1,     171000000U),
PLLVIDEO0(28,     0,     174000000U),
PLLVIDEO0(58,     1,     177000000U),
PLLVIDEO0(29,     0,     180000000U),
PLLVIDEO0(60,     1,     183000000U),
PLLVIDEO0(30,     0,     186000000U),
PLLVIDEO0(62,     1,     189000000U),
PLLVIDEO0(31,     0,     192000000U),
PLLVIDEO0(64,     1,     195000000U),
PLLVIDEO0(32,     0,     198000000U),
PLLVIDEO0(66,     1,     201000000U),
PLLVIDEO0(33,     0,     204000000U),
PLLVIDEO0(68,     1,     207000000U),
PLLVIDEO0(34,     0,     210000000U),
PLLVIDEO0(70,     1,     213000000U),
PLLVIDEO0(35,     0,     216000000U),
PLLVIDEO0(72,     1,     219000000U),
PLLVIDEO0(36,     0,     222000000U),
PLLVIDEO0(74,     1,     225000000U),
PLLVIDEO0(37,     0,     228000000U),
PLLVIDEO0(76,     1,     231000000U),
PLLVIDEO0(38,     0,     234000000U),
PLLVIDEO0(78,     1,     237000000U),
PLLVIDEO0(39,     0,     240000000U),
PLLVIDEO0(80,     1,     243000000U),
PLLVIDEO0(40,     0,     246000000U),
PLLVIDEO0(82,     1,     249000000U),
PLLVIDEO0(41,     0,     252000000U),
PLLVIDEO0(84,     1,     255000000U),
PLLVIDEO0(42,     0,     258000000U),
PLLVIDEO0(86,     1,     261000000U),
PLLVIDEO0(43,     0,     264000000U),
PLLVIDEO0(88,     1,     267000000U),
PLLVIDEO0(44,     0,     270000000U),
PLLVIDEO0(90,     1,     273000000U),
PLLVIDEO0(45,     0,     276000000U),
PLLVIDEO0(92,     1,     279000000U),
PLLVIDEO0(46,     0,     282000000U),
PLLVIDEO0(94,     1,     285000000U),
PLLVIDEO0(47,     0,     288000000U),
PLLVIDEO0(96,     1,     291000000U),
PLLVIDEO0(48,     0,     294000000U),
PLLVIDEO0(98,     1,     297000000U),
PLLVIDEO0(49,     0,     300000000U),
PLLVIDEO0(100,     1,     303000000U),
PLLVIDEO0(50,     0,     306000000U),
PLLVIDEO0(102,     1,     309000000U),
PLLVIDEO0(51,     0,     312000000U),
PLLVIDEO0(104,     1,     315000000U),
PLLVIDEO0(52,     0,     318000000U),
PLLVIDEO0(106,     1,     321000000U),
PLLVIDEO0(53,     0,     324000000U),
PLLVIDEO0(108,     1,     327000000U),
PLLVIDEO0(54,     0,     330000000U),
PLLVIDEO0(110,     1,     333000000U),
PLLVIDEO0(55,     0,     336000000U),
PLLVIDEO0(112,     1,     339000000U),
PLLVIDEO0(56,     0,     342000000U),
PLLVIDEO0(114,     1,     345000000U),
PLLVIDEO0(57,     0,     348000000U),
PLLVIDEO0(116,     1,     351000000U),
PLLVIDEO0(58,     0,     354000000U),
PLLVIDEO0(118,     1,     357000000U),
PLLVIDEO0(59,     0,     360000000U),
PLLVIDEO0(120,     1,     363000000U),
PLLVIDEO0(60,     0,     366000000U),
PLLVIDEO0(122,     1,     369000000U),
PLLVIDEO0(61,     0,     372000000U),
PLLVIDEO0(124,     1,     375000000U),
PLLVIDEO0(62,     0,     378000000U),
PLLVIDEO0(126,     1,     381000000U),
PLLVIDEO0(63,     0,     384000000U),
PLLVIDEO0(128,     1,     387000000U),
PLLVIDEO0(64,     0,     390000000U),
PLLVIDEO0(130,     1,     393000000U),
PLLVIDEO0(65,     0,     396000000U),
PLLVIDEO0(132,     1,     399000000U),
PLLVIDEO0(66,     0,     402000000U),
PLLVIDEO0(134,     1,     405000000U),
PLLVIDEO0(67,     0,     408000000U),
PLLVIDEO0(136,     1,     411000000U),
PLLVIDEO0(68,     0,     414000000U),
PLLVIDEO0(138,     1,     417000000U),
PLLVIDEO0(69,     0,     420000000U),
PLLVIDEO0(140,     1,     423000000U),
PLLVIDEO0(70,     0,     426000000U),
PLLVIDEO0(142,     1,     429000000U),
PLLVIDEO0(71,     0,     432000000U),
PLLVIDEO0(144,     1,     435000000U),
PLLVIDEO0(72,     0,     438000000U),
PLLVIDEO0(146,     1,     441000000U),
PLLVIDEO0(73,     0,     444000000U),
PLLVIDEO0(148,     1,     447000000U),
PLLVIDEO0(74,     0,     450000000U),
PLLVIDEO0(150,     1,     453000000U),
PLLVIDEO0(75,     0,     456000000U),
PLLVIDEO0(152,     1,     459000000U),
PLLVIDEO0(76,     0,     462000000U),
PLLVIDEO0(154,     1,     465000000U),
PLLVIDEO0(77,     0,     468000000U),
PLLVIDEO0(156,     1,     471000000U),
PLLVIDEO0(78,     0,     474000000U),
PLLVIDEO0(158,     1,     477000000U),
PLLVIDEO0(79,     0,     480000000U),
PLLVIDEO0(160,     1,     483000000U),
PLLVIDEO0(80,     0,     486000000U),
PLLVIDEO0(162,     1,     489000000U),
PLLVIDEO0(81,     0,     492000000U),
PLLVIDEO0(164,     1,     495000000U),
PLLVIDEO0(82,     0,     498000000U),
PLLVIDEO0(166,     1,     501000000U),
PLLVIDEO0(83,     0,     504000000U),
PLLVIDEO0(168,     1,     507000000U),
PLLVIDEO0(84,     0,     510000000U),
PLLVIDEO0(170,     1,     513000000U),
PLLVIDEO0(85,     0,     516000000U),
PLLVIDEO0(172,     1,     519000000U),
PLLVIDEO0(86,     0,     522000000U),
PLLVIDEO0(174,     1,     525000000U),
PLLVIDEO0(87,     0,     528000000U),
PLLVIDEO0(176,     1,     531000000U),
PLLVIDEO0(88,     0,     534000000U),
PLLVIDEO0(178,     1,     537000000U),
PLLVIDEO0(89,     0,     540000000U),
PLLVIDEO0(180,     1,     543000000U),
PLLVIDEO0(90,     0,     546000000U),
PLLVIDEO0(182,     1,     549000000U),
PLLVIDEO0(91,     0,     552000000U),
PLLVIDEO0(184,     1,     555000000U),
PLLVIDEO0(92,     0,     558000000U),
PLLVIDEO0(186,     1,     561000000U),
PLLVIDEO0(93,     0,     564000000U),
PLLVIDEO0(188,     1,     567000000U),
PLLVIDEO0(94,     0,     570000000U),
PLLVIDEO0(190,     1,     573000000U),
PLLVIDEO0(95,     0,     576000000U),
PLLVIDEO0(192,     1,     579000000U),
PLLVIDEO0(96,     0,     582000000U),
PLLVIDEO0(194,     1,     585000000U),
PLLVIDEO0(97,     0,     588000000U),
PLLVIDEO0(196,     1,     591000000U),
PLLVIDEO0(98,     0,     594000000U),
PLLVIDEO0(198,     1,     597000000U),
PLLVIDEO0(99,     0,     600000000U),
PLLVIDEO0(200,     1,     603000000U),
PLLVIDEO0(100,     0,     606000000U),
PLLVIDEO0(202,     1,     609000000U),
PLLVIDEO0(101,     0,     612000000U),
PLLVIDEO0(204,     1,     615000000U),
PLLVIDEO0(102,     0,     618000000U),
PLLVIDEO0(206,     1,     621000000U),
PLLVIDEO0(103,     0,     624000000U),
PLLVIDEO0(208,     1,     627000000U),
PLLVIDEO0(104,     0,     630000000U),
};

/* PLLAUDIO(n, p, d1, d2, freq)	F_N8X8_P16X6_D1V1X1_D2V0X1 */
struct sunxi_clk_factor_freq factor_pllaudio_tbl[] = {
PLLAUDIO(11,    0,    0,    0,    288000000U),
};

static unsigned int pllcpu_max, pllddr0_max, pllperiph0_max,
		 pllvideo0_max, pllaudio_max;

#define PLL_MAX_ASSIGN(name) (pll##name##_max = \
	factor_pll##name##_tbl[ARRAY_SIZE(factor_pll##name##_tbl)-1].freq)

void sunxi_clk_factor_initlimits(void)
{
	PLL_MAX_ASSIGN(cpu);
	PLL_MAX_ASSIGN(ddr0);
	PLL_MAX_ASSIGN(periph0);
	PLL_MAX_ASSIGN(video0);
	PLL_MAX_ASSIGN(audio);
}
