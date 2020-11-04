/*
 * drivers/clk/sunxi/clk-sun3iw1_tbl.c
 *
 * Copyright (c) 2007-2019 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <zhengxiaobin@allwinnertech.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include "clk-sun3iw1.h"
/*
 * freq table from hardware, need follow rules
 * 1)   each table  named as
 *      factor_pll1_tbl
 *      factor_pll2_tbl
 *      ...
 * 2) for each table line
 *      a) follow the format PLLx(n,k,m,p,d1,d2,freq), and keep the factors order
 *      b) if any factor not used, skip it
 *      c) the factor is the value to write registers, not means factor+1
 *
 *      example
 *      PLL1(9, 0, 0, 2, 60000000) means PLL1(n,k,m,p,freq)
 *      PLLVIDEO0(3, 0, 96000000)       means PLLVIDEO0(n,m,freq)
 *
 */

//PLLCPU(n , k , m , p , freq)  F_N8X5_K4X2_M0X2_P16x2
struct sunxi_clk_factor_freq factor_pllcpu_tbl[] = {
PLLCPU( 24 , 0 , 2 , 0 , 200000000U),
PLLCPU( 16 , 0 , 0 , 0 , 408000000U),
PLLCPU( 21 , 0 , 0 , 0 , 528000000U),
PLLCPU( 24 , 3 , 2 , 0 , 800000000U),
};

//PLLAUDIO(n,m,freq)    F_N8X7_M0X4
struct sunxi_clk_factor_freq factor_pllaudio_tbl[] = {
PLLAUDIO( 4  , 11 , 20000000U ) ,
PLLAUDIO( 49 , 5  , 200000000U) ,

};


//PLLVIDEO(n,m,freq)    F_N8X7_M0X4
struct sunxi_clk_factor_freq factor_pllvideo_tbl[] = {
PLLVIDEO(    4  ,    3  ,     30000000U ),
PLLVIDEO(    24 ,    0  ,     600000000U),

};

//PLLVE(n,m,freq)   F_N8X7_M0X4
struct sunxi_clk_factor_freq factor_pllve_tbl[] = {
PLLVE(    4  ,    3  ,     30000000U ),
PLLVE(    24 ,    0  ,     600000000U),

};

//PLLDDR(n,k,m,freq)    F_N8X5_K4X2_M0X2
struct sunxi_clk_factor_freq factor_pllddr_tbl[] = {
PLLDDR(    9 ,      1   ,    1  ,   240000000U),
PLLDDR(    12,      1   ,    1  ,   312000000U),
PLLDDR(    14,      1   ,    1  ,   360000000U),

};

//PLLPERIPH(n,k,freq)   F_N8X5_K4X2
struct sunxi_clk_factor_freq factor_pllperiph_tbl[] = {
PLLPERIPH(    8  ,    0  ,      216000000U ),
PLLPERIPH(    24 ,    2  ,      1800000000U),

};


static unsigned int pllcpu_max,pllaudio_max,pllvideo_max,
                pllve_max,pllddr_max ,pllperiph_max;

#define PLL_MAX_ASSIGN(name)    pll##name##_max=factor_pll##name##_tbl[ARRAY_SIZE(factor_pll##name##_tbl)-1].freq

void sunxi_clk_factor_initlimits(void)
{
    PLL_MAX_ASSIGN(cpu);PLL_MAX_ASSIGN(audio);PLL_MAX_ASSIGN(video);
    PLL_MAX_ASSIGN(ve);PLL_MAX_ASSIGN(ddr);PLL_MAX_ASSIGN(periph);
}
