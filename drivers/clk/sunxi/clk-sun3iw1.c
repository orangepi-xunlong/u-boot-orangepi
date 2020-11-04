/*
 * Copyright (C) 2013 Allwinnertech, kevin.z.m <kevin@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include"clk-sun3iw1.h"
#include<clk/clk_plat.h>
#include"clk_factor.h"
#include"clk_periph.h"
#include"clk-sun3iw1_tbl.c"
#include<div64.h>

#ifndef CONFIG_EVB_PLATFORM
    #define LOCKBIT(x) 31
#else
    #define LOCKBIT(x) x
#endif

/*                           ns  nw  ks  kw  ms  mw  ps  pw  d1s d1w d2s d2w {frac   out mode}   en-s    sdmss   sdmsw   sdmpat         sdmval*/
SUNXI_CLK_FACTORS( pll_video, 8 , 7 , 0 , 0 , 0 , 4 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 25 , 24 , 31 , 20 , 0 , PLL_VIDEOPAT , 0xd1303333);
SUNXI_CLK_FACTORS( pll_ve   , 8 , 7 , 0 , 0 , 0 , 4 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 25 , 24 , 31 , 20 , 0 , 0            , 0);


static int get_factors_pll_video(u32 rate, u32 parent_rate, struct clk_factors_value *factor)
{
    u64 tmp_rate;
    int index;

    if(!factor)
        return -1;

    tmp_rate = rate>pllvideo_max ? pllvideo_max : rate;
    do_div(tmp_rate, 1000000);
    index = tmp_rate;
    if(sunxi_clk_com_ftr_sr(&sunxi_clk_factor_pll_video,factor, factor_pllvideo_tbl,index,sizeof(factor_pllvideo_tbl)/sizeof(struct sunxi_clk_factor_freq)))
        return -1;

    if(rate == 297000000) {
        factor->frac_mode = 0;
        factor->frac_freq = 1;
        factor->factorm = 0;
    }
    else if(rate == 270000000) {
        factor->frac_mode = 0;
        factor->frac_freq = 0;
        factor->factorm = 0;
    } else {
        factor->frac_mode = 1;
        factor->frac_freq = 0;
    }

    return 0;
}

static int get_factors_pll_ve(u32 rate, u32 parent_rate, struct clk_factors_value *factor)
{
    int index;
    u64 tmp_rate;

    if (!factor)
        return -1;

    tmp_rate = rate > pllve_max ? pllve_max : rate;
    do_div(tmp_rate, 1000000);
    index = tmp_rate;
    if (sunxi_clk_com_ftr_sr(&sunxi_clk_factor_pll_ve, factor,
                     factor_pllve_tbl, index,
                     sizeof(factor_pllve_tbl)/sizeof(struct sunxi_clk_factor_freq)))
        return -1;

    return 0;
}

/*    pll_video0:24*N/M    */
static unsigned long calc_rate_video(u32 parent_rate, struct clk_factors_value *factor)
{
    u64 tmp_rate = (parent_rate?parent_rate:24000000);
    if(factor->frac_mode == 0)
    {
        if(factor->frac_freq == 1)
            return 297000000;
        else
            return 270000000;
    }
    else
    {
        tmp_rate = tmp_rate * (factor->factorn+1);
        do_div(tmp_rate, factor->factorm+1);
        return (unsigned long)tmp_rate;
    }
}

/*    pll_ddr:24*N/M    */
static unsigned long calc_rate_pll_ve(u32 parent_rate, struct clk_factors_value *factor)
{
    u64 tmp_rate = (parent_rate?parent_rate:24000000);
    tmp_rate = tmp_rate * (factor->factorn+1);
    do_div(tmp_rate, factor->factorm+1);
    return (unsigned long)tmp_rate;
}


static const char *hosc_parents[] = {"hosc"};

struct factor_init_data sunxi_factos[] = {
    {
        .name                = "pll_video",
        .parent_names        = hosc_parents,
        .num_parents         = 1,
        .flags               = CLK_IGNORE_DISABLE,
        .reg                 = PLL_VIDEO,
        .lock_reg            = PLL_VIDEO,
        .lock_bit            = LOCKBIT(28),
        .pll_lock_ctrl_reg   = PLL_CLK_CTRL,
        .lock_en_bit         = 2,
        .lock_mode           = PLL_LOCK_NONE_MODE,
        .config              = &sunxi_clk_factor_pll_video,
        .get_factors         = &get_factors_pll_video,
        .calc_rate           = &calc_rate_video,
        .priv_ops            = (struct clk_ops *)NULL,
    },
    {
        .name                = "pll_ve",
        .parent_names        = hosc_parents,
        .num_parents         = 1,
        .flags               = CLK_IGNORE_DISABLE,
        .reg                 = PLL_VE,
        .lock_reg            = PLL_VE,
        .lock_bit            = LOCKBIT(28),
        .pll_lock_ctrl_reg   = PLL_CLK_CTRL,
        .lock_en_bit         = 3,
        .lock_mode           = PLL_LOCK_NONE_MODE,
        .config              = &sunxi_clk_factor_pll_ve,
        .get_factors         = &get_factors_pll_ve,
        .calc_rate           = &calc_rate_pll_ve,
        .priv_ops            = (struct clk_ops *)NULL,
    }
};



static const char *de_parents[]          =  {"pll_video" , ""              , "pll_periph"  , "" , "" , "" , "" , ""};
static const char *tcon_parents[]        =  {"pll_video" , ""              , "pll_videox2" , "" , "" , "" , "" , ""};
static const char *deinterlace_parents[] =  {"pll_video" , ""              , "pll_videox2" , "" , "" , "" , "" , ""};
static const char *tve_clk2_parents[]    =  {"pll_video" , ""              , "pll_videox2" , "" , "" , "" , "" , ""};
static const char *tve_clk1_parents[]    =  {"tve_clk2"  , "tve_clk2_div2"                                         };
static const char *tvd_parents[]         =  {"pll_video" , "hosc"          , "pll_videox2" , "" , "" , "" , "" , ""};
//static const char *ve_parents[]        =  {"pll_ve"                                                              };
static const char *codec_parents[]       =  {"pll_audio"                                                           };


struct sunxi_clk_comgate com_gates[]={
{"csi",       0,  0x7,    BUS_GATE_SHARE|RST_GATE_SHARE|MBUS_GATE_SHARE, 0},
};

static int clk_lock = 0;

/*
SUNXI_CLK_PERIPH(name , mux_reg , mux_sft , mux_wid , div_reg , div_msft , div_mwid , div_nsft , div_nwid , gate_flag , en_reg , rst_reg , bus_gate_reg , drm_gate_reg , en_sft , rst_sft , bus_gate_sft , dram_gate_sft , lock , com_gate , com_gate_off)
*/
SUNXI_CLK_PERIPH(debe        , BE_CFG          , 24 , 3 , BE_CFG          , 0 , 4 , 0 , 0 , 0 , BE_CFG          , BUS_RST1 , BUS_GATE1 , DRAM_GATE , 31 , 12 , 12 , 26 , &clk_lock , NULL , 0);
SUNXI_CLK_PERIPH(defe        , FE_CFG          , 24 , 3 , FE_CFG          , 0 , 4 , 0 , 0 , 0 , FE_CFG          , BUS_RST1 , BUS_GATE1 , DRAM_GATE , 31 , 14 , 14 , 24 , &clk_lock , NULL , 0);
SUNXI_CLK_PERIPH(tcon        , TCON_CFG        , 24 , 3 , 0               , 0 , 0 , 0 , 0 , 0 , TCON_CFG        , BUS_RST1 , BUS_GATE1 , 0         , 31 , 4  , 4  , 0  , &clk_lock , NULL , 0);
SUNXI_CLK_PERIPH(deinterlace , DEINTERLACE_CFG , 24 , 3 , DEINTERLACE_CFG , 0 , 4 , 0 , 0 , 0 , DEINTERLACE_CFG , BUS_RST1 , BUS_GATE1 , DRAM_GATE , 31 , 5  , 5  , 2  , &clk_lock , NULL , 0);
SUNXI_CLK_PERIPH(tve_clk2    , TVE_CFG         , 24 , 3 , TVE_CFG         , 0 , 4 , 0 , 0 , 0 , TVE_CFG         , BUS_RST1 , BUS_GATE1 , 0         , 31 , 10 , 10 , 0  , &clk_lock , NULL , 0);
SUNXI_CLK_PERIPH(tve_clk1    , TVE_CFG         , 8  , 1 , 0               , 0 , 0 , 0 , 0 , 0 , TVE_CFG         , BUS_RST1 , BUS_GATE1 , 0         , 15 , 10 , 10 , 0  , &clk_lock , NULL , 0);
SUNXI_CLK_PERIPH(tvd         , TVD_CFG         , 24 , 3 , TVD_CFG         , 0 , 4 , 0 , 0 , 0 , TVD_CFG         , BUS_RST1 , BUS_GATE1 , DRAM_GATE , 31 , 9  , 9  , 3  , &clk_lock , NULL , 0);
SUNXI_CLK_PERIPH(ve          , VE_CFG          , 0  , 0 , 0               , 0 , 0 , 0 , 0 , 0 , VE_CFG          , BUS_RST1 , BUS_GATE1 , DRAM_GATE , 31 , 0  , 0  , 0  , &clk_lock , NULL , 0);

struct periph_init_data sunxi_periphs_init[] = {
    {"debe"        , 0 , de_parents          , ARRAY_SIZE(de_parents)          , &sunxi_clk_periph_debe            },
    {"defe"        , 0 , de_parents          , ARRAY_SIZE(de_parents)          , &sunxi_clk_periph_defe            },
    {"tcon"        , 0 , tcon_parents        , ARRAY_SIZE(tcon_parents)        , &sunxi_clk_periph_tcon            },
    {"deinterlace" , 0 , deinterlace_parents , ARRAY_SIZE(deinterlace_parents) , &sunxi_clk_periph_deinterlace     },
    {"tve_clk2"    , 0 , tve_clk2_parents    , ARRAY_SIZE(tve_clk2_parents)    , &sunxi_clk_periph_tve_clk2        },
    {"tve_clk1"    , 0 , tve_clk1_parents    , ARRAY_SIZE(tve_clk1_parents)    , &sunxi_clk_periph_tve_clk1        },
    {"tvd"         , 0 , tvd_parents         , ARRAY_SIZE(tvd_parents)         , &sunxi_clk_periph_tvd             },
    {"ve"          , 0 , codec_parents       , ARRAY_SIZE(codec_parents)       , &sunxi_clk_periph_ve              },
};



void  sunxi_init_clocks(void)
{
    return ;
}

static void  *sunxi_clk_base = NULL;

void  init_clocks(void)
{
    int     i;
    //struct clk *clk;
    struct factor_init_data *factor;
    struct periph_init_data *periph;

    /* get clk register base address */
    sunxi_clk_base = (void*)0x01c20000; //fixed base address.
    sunxi_clk_factor_initlimits();
    clk_register_fixed_rate(NULL, "hosc", NULL, CLK_IS_ROOT, 24000000);

    /* register normal factors, based on sunxi factor framework */
    for(i=0; i<ARRAY_SIZE(sunxi_factos); i++) {
        factor = &sunxi_factos[i];
        factor->priv_regops = NULL;
        sunxi_clk_register_factors(NULL, (void*)sunxi_clk_base, (struct factor_init_data*)factor);
    }

    /* register periph clock */
    for(i=0; i<ARRAY_SIZE(sunxi_periphs_init); i++) {
        periph = &sunxi_periphs_init[i];
        periph->periph->priv_regops = NULL;
        sunxi_clk_register_periph(periph,sunxi_clk_base);
    }
}


