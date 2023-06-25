/*
 * Copyright (C) 2013 Allwinnertech, huafenghuang <huafenghuang@allwinnertech.com>
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

#include "clk-sun8iw21.h"
#include "clk-sun8iw21_tbl.c"
#include "clk_factor.h"
#include "clk_periph.h"
#include <linux/compat.h>
#include <fdt_support.h>
#include <div64.h>

#define FACTOR_SIZEOF(name) (sizeof(factor_pll##name##_tbl)/ \
			     sizeof(struct sunxi_clk_factor_freq))

#define FACTOR_SEARCH(name) (sunxi_clk_com_ftr_sr( \
		&sunxi_clk_factor_pll_##name, factor, \
		factor_pll##name##_tbl, index, \
		FACTOR_SIZEOF(name)))

#ifndef CONFIG_EVB_PLATFORM
	#define LOCKBIT(x) 31
#else
	#define LOCKBIT(x) x
#endif

DEFINE_SPINLOCK(clk_lock);
void __iomem *sunxi_clk_base;
void __iomem *sunxi_clk_cpus_base;
void __iomem *sunxi_clk_rtc_base;
int sunxi_clk_maxreg = SUNXI_CLK_MAX_REG;
int cpus_clk_maxreg = CPUS_CLK_MAX_REG;

/*					ns  nw  ks  kw  ms  mw  ps  pw  d1s d1w d2s d2w {frac   out mode}   en-s    sdmss   sdmsw   sdmpat          sdmval*/
SUNXI_CLK_FACTORS(pll_cpu,		8,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,    0,    0,  0,      31,     0,      0,      0,              0);
SUNXI_CLK_FACTORS(pll_ddr0,		8,  8,  0,  0,  0,  0,  0,  0,  1,  1,  0,  1,    0,    0,  0,      31,     24,     1,      PLL_DDR0PAT,    0xd1303333);
SUNXI_CLK_FACTORS(pll_periph0x2,	8,  8,  0,  0,  1,  1,  0,  0,  16, 3,  0,  0,    0,    0,  0,      31,     24,     1,      PLL_PERI0PAT0,  0xd1303333);
SUNXI_CLK_FACTORS(pll_periph0800m,	8,  8,  0,  0,  1,  1,  0,  0,  20, 3,  0,  0,    0,    0,  0,      31,     24,     1,      PLL_PERI0PAT0,  0xd1303333);
SUNXI_CLK_FACTORS(pll_periph0480m,	8,  8,  0,  0,  1,  1,  0,  0,  2,  3,  0,  0,    0,    0,  0,      31,     24,     1,      PLL_PERI0PAT0,  0xd1303333);
SUNXI_CLK_FACTORS(pll_video0x4,		8,  8,  0,  0,  0,  0,  0,  0,  1,  1,  0,  0,    0,    0,  0,      31,     24,     1,      PLL_VIDEO0PAT0, 0xd1303333);
SUNXI_CLK_FACTORS(pll_csix4,		8,  8,  0,  0,  0,  0,  0,  0,  1,  1,  0,  0,    0,    0,  0,      31,     24,     1,      PLL_CSIPAT0,    0xd1303333);
SUNXI_CLK_FACTORS(pll_audio_div2,	8,  8,  0,  0,  0,  0,  0,  0,  1,  1,  16, 3,    0,    0,  0,      31,     24,     1,      PLL_AUDIOPAT0,  0xd1303333);
SUNXI_CLK_FACTORS(pll_audio_div5,	8,  8,  0,  0,  0,  0,  0,  0,  1,  1,  20, 3,    0,    0,  0,      31,     24,     1,      PLL_AUDIOPAT0,  0xd1303333);
SUNXI_CLK_FACTORS(pll_npux4,		8,  8,  0,  0,  1,  1,  0,  0,  0,  0,  0,  0,    0,    0,  0,      31,     24,     1,      PLL_NPUPAT0,    0xd1303333);

static int get_factors_pll_cpu(u32 rate, u32 parent_rate,
		struct clk_factors_value *factor)
{
	int index;
	u64 tmp_rate;

	if (!factor)
		return -1;
	tmp_rate = rate > pllcpu_max ? pllcpu_max : rate;
	do_div(tmp_rate, 1000000);
	index = tmp_rate;

	if (FACTOR_SEARCH(cpu))
		return -1;

	return 0;
}

static int get_factors_pll_ddr0(u32 rate, u32 parent_rate,
		struct clk_factors_value *factor)
{
	int index;
	u64 tmp_rate;

	if (!factor)
		return -1;

	tmp_rate = rate > pllddr0_max ? pllddr0_max : rate;
	do_div(tmp_rate, 1000000);
	index = tmp_rate;

	if (FACTOR_SEARCH(ddr0))
		return -1;

	return 0;
}

static int get_factors_pll_periph0x2(u32 rate, u32 parent_rate,
		struct clk_factors_value *factor)
{
	int index;
	u64 tmp_rate;

	if (!factor)
		return -1;

	tmp_rate = rate > pllperiph0x2_max ? pllperiph0x2_max : rate;
	do_div(tmp_rate, 1000000);
	index = tmp_rate;

	if (FACTOR_SEARCH(periph0x2))
		return -1;

	return 0;
}

static int get_factors_pll_periph0800m(u32 rate, u32 parent_rate,
		struct clk_factors_value *factor)
{
	int index;
	u64 tmp_rate;

	if (!factor)
		return -1;

	tmp_rate = rate > pllperiph0800m_max ? pllperiph0800m_max : rate;
	do_div(tmp_rate, 1000000);
	index = tmp_rate;

	if (FACTOR_SEARCH(periph0800m))
		return -1;

	return 0;
}

static int get_factors_pll_periph0480m(u32 rate, u32 parent_rate,
		struct clk_factors_value *factor)
{
	int index;
	u64 tmp_rate;

	if (!factor)
		return -1;

	tmp_rate = rate > pllperiph0480m_max ? pllperiph0480m_max : rate;
	do_div(tmp_rate, 1000000);
	index = tmp_rate;

	if (FACTOR_SEARCH(periph0480m))
		return -1;

	return 0;
}
static int get_factors_pll_video0x4(u32 rate, u32 parent_rate,
		struct clk_factors_value *factor)
{
	u64 tmp_rate;
	int index;

	if (!factor)
		return -1;

	tmp_rate = rate > pllvideo0x4_max ? pllvideo0x4_max : rate;
	do_div(tmp_rate, 1000000);
	index = tmp_rate;

	if (FACTOR_SEARCH(video0x4))
		return -1;

	return 0;
}

static int get_factors_pll_csix4(u32 rate, u32 parent_rate,
		struct clk_factors_value *factor)
{
	u64 tmp_rate;
	int index;

	if (!factor)
		return -1;

	tmp_rate = rate > pllcsix4_max ? pllcsix4_max : rate;
	do_div(tmp_rate, 1000000);
	index = tmp_rate;

	if (FACTOR_SEARCH(csix4))
		return -1;

	return 0;
}

static int get_factors_pll_audio_div2(u32 rate, u32 parent_rate,
		struct clk_factors_value *factor)
{
	if (rate == 294912000) {
		factor->factorn = 21;
		factor->factord1 = 0;
		factor->factord2 = 1;
		sunxi_clk_factor_pll_audio_div2.sdmval = 0xC001288D;
	} else if (rate == 491520000) {
		factor->factorn = 39;
		factor->factord1 = 0;
		factor->factord2 = 1;
		sunxi_clk_factor_pll_audio_div2.sdmval = 0xC001EB85;
	} else {
		return -1;
	}

	return 0;
}

static int get_factors_pll_audio_div5(u32 rate, u32 parent_rate,
		struct clk_factors_value *factor)
{
	if (rate == 67737600) {
		factor->factorn = 21;
		factor->factord1 = 0;
		factor->factord2 = 7;
		sunxi_clk_factor_pll_audio_div5.sdmval = 0xC001288D;
	} else if (rate == 196608000) {
		factor->factorn = 39;
		factor->factord1 = 0;
		factor->factord2 = 4;
		sunxi_clk_factor_pll_audio_div5.sdmval = 0xC001EB85;
	} else
		return -1;

	return 0;
}

static int get_factors_pll_npux4(u32 rate, u32 parent_rate,
		struct clk_factors_value *factor)
{
	int index;
	u64 tmp_rate;

	if (!factor)
		return -1;

	tmp_rate = rate > pllnpux4_max ? pllnpux4_max : rate;
	do_div(tmp_rate, 1000000);
	index = tmp_rate;

	if (FACTOR_SEARCH(npux4))
		return -1;

	return 0;
}

static unsigned long calc_rate_pll(u32 parent_rate,
		struct clk_factors_value *factor)
{
	u64 tmp_rate = (parent_rate ? parent_rate : 24000000);

	factor->factorn = factor->factorn >= 0xff ? 0 : factor->factorn;
	factor->factorm = factor->factorm >= 0xff ? 0 : factor->factorm;
	factor->factork = factor->factork >= 0xff ? 0 : factor->factork;
	factor->factorp = factor->factorp >= 0xff ? 0 : factor->factorp;
	factor->factord1 = factor->factord1 >= 0xff ? 0 : factor->factord1;
	factor->factord2 = factor->factord2 >= 0xff ? 0 : factor->factord2;
	tmp_rate = tmp_rate * (factor->factorn + 1);
	do_div(tmp_rate,
	       (factor->factorm + 1) *
	       (factor->factord1 + 1) *
	       (factor->factord2 + 1)
	      );
	return (unsigned long)tmp_rate;
}
/*
 *    pll_audio: 24*N/D1/D2/P
 *
 *    NOTE: pll_audiox4 = 24*N/D1/2
 *          pll_audiox2 = 24*N/D1/4
 *
 *    pll_audiox4=2*pll_audiox2=4*pll_audio only when D2*P=8
 */
static unsigned long calc_rate_audio(u32 parent_rate,
		struct clk_factors_value *factor)
{
	u64 tmp_rate = (parent_rate ? parent_rate : 24000000);
	if ((factor->factorn == 39) &&
			(factor->factord1 == 0) &&
			(factor->factord2 == 1))
		return 491520000;
	else if ((factor->factorn == 39) &&
			(factor->factord1 == 0) &&
			(factor->factord2 == 4))
		return 196608000;
	else if ((factor->factorn == 21) &&
			(factor->factord1 == 0) &&
			(factor->factord2 == 7))
		return 67737600;
	else if ((factor->factorn == 21) &&
			(factor->factord1 == 0) &&
			(factor->factord2 == 1))
		return 294912000;
	else {
		tmp_rate = tmp_rate * (factor->factorn + 1);
		do_div(tmp_rate, ((factor->factorp + 1) *
				(factor->factord1 + 1) *
				(factor->factord2 + 1)));
		return (unsigned long)tmp_rate;
	}
}

static const char *hosc_parents[] = {"hosc"};
struct factor_init_data sunxi_factos[] = {
	/* name             parent           parent_num,           flags                                reg          lock_reg     lock_bit     pll_lock_ctrl_reg lock_en_bit lock_mode           config                         get_factors               calc_rate              priv_ops*/
	{"pll_cpu",         hosc_parents,    1,                    CLK_GET_RATE_NOCACHE|CLK_NO_DISABLE, PLL_CPU,     PLL_CPU,     LOCKBIT(28), PLL_CPU,          29,         PLL_LOCK_NEW_MODE,  &sunxi_clk_factor_pll_cpu,     &get_factors_pll_cpu,     &calc_rate_pll,        (struct clk_ops *)NULL},
	{"pll_ddr0",        hosc_parents,    1,                    CLK_GET_RATE_NOCACHE|CLK_NO_DISABLE, PLL_DDR0,    PLL_DDR0,    LOCKBIT(28), PLL_DDR0,         29,         PLL_LOCK_NEW_MODE,  &sunxi_clk_factor_pll_ddr0,    &get_factors_pll_ddr0,    &calc_rate_pll,        (struct clk_ops *)NULL},
	{"pll_periph0x2",   hosc_parents,    1,	                   CLK_NO_DISABLE,                      PLL_PERIPH0, PLL_PERIPH0, LOCKBIT(28), PLL_PERIPH0,      29,         PLL_LOCK_NEW_MODE,  &sunxi_clk_factor_pll_periph0x2, &get_factors_pll_periph0x2, &calc_rate_pll,        (struct clk_ops *)NULL},
	{"pll_periph0800m", hosc_parents,    1,                    CLK_NO_DISABLE,                      PLL_PERIPH0, PLL_PERIPH0, LOCKBIT(28), PLL_PERIPH0,      29,         PLL_LOCK_NEW_MODE,  &sunxi_clk_factor_pll_periph0800m, &get_factors_pll_periph0800m, &calc_rate_pll,        (struct clk_ops *)NULL},
	{"pll_periph0480m", hosc_parents,    1,                    CLK_NO_DISABLE,                      PLL_PERIPH0, PLL_PERIPH0, LOCKBIT(28), PLL_PERIPH0,      29,         PLL_LOCK_NEW_MODE,  &sunxi_clk_factor_pll_periph0480m, &get_factors_pll_periph0480m, &calc_rate_pll,        (struct clk_ops *)NULL},
	{"pll_video0x4",    hosc_parents,    1,                    CLK_NO_DISABLE,                      PLL_VIDEO0,  PLL_VIDEO0,  LOCKBIT(28), PLL_VIDEO0,       29,         PLL_LOCK_NEW_MODE,  &sunxi_clk_factor_pll_video0x4,  &get_factors_pll_video0x4,  &calc_rate_pll,        (struct clk_ops *)NULL},
	{"pll_csix4",       hosc_parents,    1,                    CLK_NO_DISABLE,                      PLL_CSI,     PLL_CSI,     LOCKBIT(28), PLL_CSI,          29,         PLL_LOCK_NEW_MODE,  &sunxi_clk_factor_pll_csix4,     &get_factors_pll_csix4,     &calc_rate_pll,        (struct clk_ops *)NULL},
	{"pll_audio_div2",  hosc_parents,    1,                    CLK_NO_DISABLE,                      PLL_AUDIO,   PLL_AUDIO,   LOCKBIT(28), PLL_AUDIO,        29,         PLL_LOCK_NEW_MODE,  &sunxi_clk_factor_pll_audio_div2,   &get_factors_pll_audio_div2,   &calc_rate_audio,      (struct clk_ops *)NULL},
	{"pll_audio_div5",  hosc_parents,    1,                    CLK_NO_DISABLE,                      PLL_AUDIO,   PLL_AUDIO,   LOCKBIT(28), PLL_AUDIO,        29,         PLL_LOCK_NEW_MODE,  &sunxi_clk_factor_pll_audio_div5,   &get_factors_pll_audio_div5,   &calc_rate_audio,      (struct clk_ops *)NULL},
	{"pll_npux4",       hosc_parents,    1,                    CLK_NO_DISABLE,                      PLL_NPU,     PLL_NPU,     LOCKBIT(28), PLL_NPU,          29,         PLL_LOCK_NEW_MODE,  &sunxi_clk_factor_pll_npux4,   &get_factors_pll_npux4,   &calc_rate_pll,        (struct clk_ops *)NULL},
};

static const char *cpu_parents[] = {"hosc", "losc", "iosc", "pll_cpu", "pll_periph0600m", "pll_periph0800m"};
static const char *cpu_apb_axi_parents[] = {"cpu"};
static const char *ahb_apb_parents[] = {"hosc", "losc", "iosc", "pll_periph0600m"};
static const char *mbus_parents[] = {"sdramd4"};
static const char *de_parents[] = {"pll_periph0300m", "pll_video0"};
static const char *ce_parents[] = {"hosc", "pll_periph0400m", "pll_periph0300m"};
static const char *ve_parents[] = {"pll_periph0300m", "pll_periph0400m", "pll_periph0480m", "pll_npux4", "pll_video0x4", "pll_csix4"};
static const char *npu_parents[] = {"pll_periph0480m", "pll_periph0600m", "pll_periph0800m", "pll_npux4"};

static const char *sdram_parents[] = {"pll_ddr0", "pll_periph0x2", "pll_periph0800m"};
static const char *smhc01_parents[] = {"hosc", "pll_periph0400m", "pll_periph0300m"};
static const char *smhc2_parents[] = {"hosc", "pll_periph0600m", "pll_periph0400m"};
static const char *spi_parents[] = {"hosc", "pll_periph0300m", "pll_periph0200m"};
static const char *spif_parents[] = {"hosc", "pll_periph0400m", "pll_periph0300m"};
static const char *gmac_25m_parents[] = {"pll_periph0150m_div6"};
static const char *pll_audiox4_parents[] = {"pll_audio_div2"};
static const char *pll_audio_parents[] = {"pll_audio_div5"};
static const char *audio_parents[] = {"pll_audio", "pll_audiox4"};
static const char *usbohci_12m_parents[] = {"osc48md4", "hoscd2", "losc"};
static const char *dsi_parents[] = {"hosc", "pll_periph0200m", "pll_periph0150m"};
static const char *tcon_lcd_parents[] = {"pll_video0x4", "pll_periph0x2", "pll_csix4"};
static const char *csi_top_parents[] = {"pll_periph0300m", "pll_periph0400m", "pll_video0x4", "pll_csix4"};
static const char *csi_master_parents[] = {"hosc", "pll_csix4", "pll_video0x4", "pll_periph0x2"};
static const char *isp_parents[] = {"hosc" };
static const char *e907_parents[] = {"hosc", "losc", "iosc", "pll_periph0600m", "pll_periph0480m", "pll_cpu"};
static const char *e907_axi_parents[] = {"e907"};
static const char *fanout25m_parents[] = {"pll_periph0150m_div6"};
static const char *fanout16m_parents[] = {"pll_periph0160m_div10"};
static const char *fanout12m_parents[] = {"hoscd2"};
static const char *fanout24m_parents[] = {"hosc"};
static const char *fanout27m_parents[] = {"pll_audio", "pll_csi", "pll_periph0300m"};
static const char *fanout_pclk_parents[] = {"apb0"};
static const char *fanout012_parents[] = {"losc_out", "fanout_clk12m", "fanout_clk16m", "fanout_clk24m", "fanout_clk25m", "fanout_clk27m", "fanout_pclk"};

static const char *ahbmod_parents[] = {"ahb"};
static const char *apb0mod_parents[] = {"apb0"};
static const char *apb1mod_parents[] = {"apb1"};

static const char *cpurahbs_apbs_parents[] = {"hosc", "losc", "iosc", "pll_periph0div3"};
static const char *cpurtwd_parents[] = {"cpurapbs0"};
static const char *cpurrtc_parents[] = {"cpurahbs"};

static const char *hosc32k_parents[] = {"hoscdiv32k"};
static const char *losc_parents[] = {"iosc32k", "ext_losc"};
static const char *losc_out_parents[] = {"losc", "ext_losc", "hosc32k"};

struct sunxi_clk_comgate com_gates[] = {
{"csi",     0, 0x1ff, BUS_GATE_SHARE | RST_GATE_SHARE | MBUS_GATE_SHARE, 0},
{"codec",   0, 0x3, BUS_GATE_SHARE | RST_GATE_SHARE, 0},
{"e907",    0, 0x3, BUS_GATE_SHARE | RST_GATE_SHARE, 0},
};

/*
SUNXI_CLK_PERIPH(name,           mux_reg,         mux_sft, mux_wid,      div_reg,            div_msft,  div_mwid,   div_nsft,   div_nwid,   gate_flag,  en_reg,          rst_reg,         bus_gate_reg,  drm_gate_reg,  en_sft,     rst_sft,    bus_gate_sft,   dram_gate_sft, lock,      com_gate,         com_gate_off)
*/
SUNXI_CLK_PERIPH(cpu,            CPU_CFG,         24,      3,            0,                  0,         0,          0,          0,          0,          0,               0,               0,             0,             0,          0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(axi,            0,               0,       0,            CPU_CFG,            0,         2,          0,          0,          0,          0,               0,               0,             0,             0,          0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(cpuapb,         0,               0,       0,            CPU_CFG,            8,         2,          0,          0,          0,          0,               0,               0,             0,             0,          0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(ahb,            AHB_CFG,         24,      2,            AHB_CFG,            0,         5,          8,          2,          0,          0,               0,               0,             0,             0,          0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(apb0,           APB0_CFG,        24,      2,            APB0_CFG,           0,         5,          8,          2,          0,          0,               0,               0,             0,             0,          0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(apb1,           APB1_CFG,        24,      2,            APB1_CFG,           0,         5,          8,          2,          0,          0,               0,               0,             0,             0,          0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(mbus,           0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               MBUS_CFG,        0,             0,             0,          30,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(de,             DE_CFG,          24,      1,            DE_CFG,             0,         5,          0,          0,          0,          DE_CFG,          DE_GATE,         DE_GATE,       0,             31,         16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(g2d,            G2D_CFG,         24,      1,            G2D_CFG,            0,         5,          0,          0,          0,          G2D_CFG,         G2D_GATE,        G2D_GATE,      MBUS_GATE,     31,         16,         0,              10,            &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(ce,             CE_CFG,          24,      2,            CE_CFG,             0,         4,          0,          0,          0,          CE_CFG,          CE_GATE,         CE_GATE,       MBUS_GATE,     31,         16,         0,              2,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(ve,             VE_CFG,          24,      3,            VE_CFG,             0,         5,          0,          0,          0,          VE_CFG,          VE_GATE,         VE_GATE,       MBUS_GATE,     31,         16,         0,              1,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(npu,            NPU_CFG,         24,      3,            NPU_CFG,            0,         5,          0,          0,          0,          NPU_CFG,         NPU_GATE,        NPU_GATE,      0,             21,         16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(dma,            0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               DMA_GATE,        DMA_GATE,      MBUS_GATE,     0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(msgbox0,        0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               MSGBOX_GATE,     MSGBOX_GATE,    0,             0,         16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(msgbox1,        0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               MSGBOX_GATE,     MSGBOX_GATE,    0,             0,         17,         1,              0,            &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(spinlock,       0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,                SPINLOCK_GATE,  SPINLOCK_GATE, 0,             0,         16,         0,              0,            &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(hstimer,        0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               HSTIMER_GATE,    HSTIMER_GATE,  0,          0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(avs,            0,               0,       0,            0,                  0,         0,          0,          0,          0,          AVS_CFG,         0,               0,             0,             31,         0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(dbgsys,         0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               DBGSYS_GATE,     DBGSYS_GATE,   0,             0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(pwm,            0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               PWM_GATE,        PWM_GATE,      0,             0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(iommu,          0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               0,               IOMMU_GATE,    0,             0,          0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(sdram,          DRAM_CFG,        24,      3,            DRAM_CFG,           0,         5,          8,          2,          0,          DRAM_CFG,        DRAM_GATE,       DRAM_GATE,     0,             31,         16,         0,             0,             &clk_lock, NULL,             0);

SUNXI_CLK_PERIPH(sdmmc0_mod,     SMHC0_CFG,       24,      3,            SMHC0_CFG,          0,         4,          8,          2,          0,          SMHC0_CFG,       0,              0,                 0,          31,         0,          0,             0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(sdmmc0_rst,     0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               SMHC_GATE,      0,                 0,          0,          16,         0,             0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(sdmmc0_bus,     0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               0,              SMHC_GATE,         0,          0,          0,         0,             0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(sdmmc1_mod,     SMHC1_CFG,       24,      3,            SMHC1_CFG,          0,         4,          8,          2,          0,          SMHC1_CFG,       0,              0,                 0,          31,         0,          0,             0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(sdmmc1_rst,     0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               SMHC_GATE,      0,                 0,          0,          17,         0,             0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(sdmmc1_bus,     0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               0,              SMHC_GATE,         0,          0,          0,          1,             0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(sdmmc2_mod,     SMHC2_CFG,       24,      3,            SMHC2_CFG,          0,         4,          8,          2,          0,          SMHC2_CFG,       0,              0,                 0,          31,         0,          0,             0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(sdmmc2_rst,     0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               SMHC_GATE,      0,                 0,          0,          18,         0,             0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(sdmmc2_bus,     0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               0,              SMHC_GATE,         0,          0,          0,          2,             0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(uart0,          0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               UART_GATE,      UART_GATE,      0,             0,          16,         0,             0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(uart1,          0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               UART_GATE,      UART_GATE,      0,             0,          17,         1,             0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(uart2,          0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               UART_GATE,      UART_GATE,      0,             0,          18,         2,             0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(uart3,          0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               UART_GATE,      UART_GATE,      0,             0,          19,         3,             0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(twi0,           0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               TWI_GATE,       TWI_GATE,           0,         0,          16,         0,             0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(twi1,           0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               TWI_GATE,       TWI_GATE,           0,         0,          17,         1,             0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(twi2,           0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               TWI_GATE,       TWI_GATE,           0,         0,          18,         2,             0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(twi3,           0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               TWI_GATE,       TWI_GATE,           0,         0,          19,         3,             0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(twi4,           0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               TWI_GATE,       TWI_GATE,           0,         0,          20,         4,             0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(spi0,           SPI0_CFG,        24,      3,            SPI0_CFG,           0,         4,          8,          2,          0,          SPI0_CFG,        SPI_GATE,       SPI_GATE,          0,          31,         16,         0,             0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(spi1,           SPI1_CFG,        24,      3,            SPI1_CFG,           0,         4,          8,          2,          0,          SPI1_CFG,        SPI_GATE,       SPI_GATE,          0,          31,         17,         1,             0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(spi2,           SPI2_CFG,        24,      3,            SPI2_CFG,           0,         4,          8,          2,          0,          SPI2_CFG,        SPI_GATE,       SPI_GATE,          0,          31,         18,         2,             0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(spi3,           SPI3_CFG,        24,      3,            SPI3_CFG,           0,         4,          8,          2,          0,          SPI3_CFG,        SPI_GATE,       SPI_GATE,          0,          31,         19,         3,             0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(spif,           SPIF_CFG,        24,      3,            SPIF_CFG,           0,         4,          8,          2,          0,          SPI3_CFG,        SPI_GATE,       SPI_GATE,          0,          31,         20,         4,             0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(gmac_25m,       0,               0,       0,            0,                  0,         0,          0,          0,          0,          GMAC_25M_CFG,    0,              GMAC_25M_CFG,      0,          31,         0,          30,            0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(gmac,           0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               GMAC_GATE,      GMAC_GATE,         0,          0,          16,         0,             0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(gpadc,          0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               GPADC_GATE,     GPADC_GATE,        0,          0,          16,         0,             0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(ths,            0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               THS_GATE,        THS_GATE,         0,          0,          16,         0,             0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(pll_audiox4,     0,               0,       0,            PLL_PRE_CFG,        5,         5,          0,          0,          0,          0,               0,               0,             0,             0,          0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(pll_audio,       0,               0,       0,            PLL_PRE_CFG,        0,         5,          0,          0,          0,          0,               0,               0,             0,             0,          0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(i2s0,           I2S0_CFG,        24,      1,            I2S0_CFG,           0,         4,          0,          0,          0,          I2S0_CFG,        I2S_GATE,        I2S_GATE,         0,          31,         16,         0,             0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(i2s1,           I2S1_CFG,        24,      1,            I2S1_CFG,           0,         4,          0,          0,          0,          I2S1_CFG,        I2S_GATE,        I2S_GATE,         0,          31,         17,         1,             0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(dmic,           DMIC_CFG,        24,      1,            DMIC_CFG,           0,         4,          0,          0,          0,          DMIC_CFG,        DMIC_GATE,       DMIC_GATE,        0,          31,         16,         0,             0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(codec_dac,      CODEC_DAC_CFG,   24,      1,            CODEC_DAC_CFG,      0,         4,          0,          0,          0,          CODEC_DAC_CFG,   CODEC_GATE,     CODEC_GATE,       0,          31,          16,         0,              0,             &clk_lock, &com_gates[1],    0);
SUNXI_CLK_PERIPH(codec_adc,      CODEC_ADC_CFG,   24,      1,            CODEC_ADC_CFG,      0,         4,          0,          0,          0,          CODEC_ADC_CFG,   CODEC_GATE,     CODEC_GATE,       0,          31,          16,         0,              0,             &clk_lock, &com_gates[1],    1);
SUNXI_CLK_PERIPH(usbphy0,        0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               USB0_CFG,       0,                0,          0,           30,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(usbohci0,       0,               0,       0,            0,                  0,         0,          0,          0,          0,          USB0_CFG,        USB_GATE,        USB_GATE,         0,          31,         16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(usbohci0_12m,   USB0_CFG,        24,      2,            0,                  0,         0,          0,          0,          0,          0,               0,               0,                0,          0,          0,          0,             0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(usbehci0,       0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               USB_GATE,        USB_GATE,         0,          0,          20,         4,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(usbotg,         0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               USB_GATE,        USB_GATE,         0,          0,          24,         8,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(dpss_top,       0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               DPSS_TOP_GATE,   DPSS_TOP_GATE, 0,             0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(mipi_dsi,       DSI_CFG,         24,      3,            DSI_CFG,            0,         4,          0,          0,          0,          DSI_CFG,         DSI_GATE,        DSI_GATE,      0,             31,         16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(tcon_lcd,       TCON_LCD_CFG,    24,      3,            TCON_LCD_CFG,       0,         4,          8,          2,          0,          TCON_LCD_CFG,    TCON_LCD_GATE,   TCON_LCD_GATE, 0,             31,         16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(csi_top,        CSI_TOP_CFG,     24,      3,            CSI_TOP_CFG,        0,         5,          0,          0,          0,          CSI_TOP_CFG,     CSI_GATE,        CSI_GATE,      MBUS_GATE,     31,         16,         0,              8,             &clk_lock, &com_gates[0],    0);
SUNXI_CLK_PERIPH(csi_master0,    CSI_MASTER0_CFG, 24,      3,            CSI_MASTER0_CFG,    0,         5,          8,          2,          0,          CSI_MASTER0_CFG,	0,		0,		0,      31,          0,         0,              0,             &clk_lock,  NULL,             0);
SUNXI_CLK_PERIPH(csi_master1,    CSI_MASTER1_CFG, 24,      3,            CSI_MASTER1_CFG,    0,         5,          8,          2,          0,          CSI_MASTER1_CFG,	0,		0,		0,      31,          0,         0,              0,             &clk_lock,  NULL,             0);
SUNXI_CLK_PERIPH(csi_master2,    CSI_MASTER2_CFG, 24,      3,            CSI_MASTER2_CFG,    0,         5,          8,          2,          0,          CSI_MASTER2_CFG,	0,		0,		0,      31,          0,         0,              0,             &clk_lock,  NULL,             0);
SUNXI_CLK_PERIPH(isp,            0,               0,       0,            0,                  0,         0,          0,          0,          0,          MBUS_GATE,       CSI_GATE,        CSI_GATE,      MBUS_GATE,     9,          16,         0,              8,             &clk_lock, &com_gates[0],    0);
SUNXI_CLK_PERIPH(wiegand,        0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               WIEGAND_GATE,    WIEGAND_GATE,  0,             0,          16,         0,              0,             &clk_lock, NULL,             0);
/* TODO the E907 modele reset and gate were not config */
SUNXI_CLK_PERIPH(e907,           E907_CFG,        24,      3,            E907_CFG,           0,         5,          0,          0,          0,          0,               RISCV_GATE,      RISCV_GATE,    0,             0,          16,         0,              0,             &clk_lock, &com_gates[2],    0);
SUNXI_CLK_PERIPH(e907_axi,       E907_CFG,        0,       0,            E907_CFG,           8,         2,          0,          0,          0,          0,               RISCV_GATE,     RISCV_GATE,     0,             0,          16,         0,              0,             &clk_lock, &com_gates[2],    1);
SUNXI_CLK_PERIPH(fanout_25m,     0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               0,               FANOUT_GATE,   0,             0,          0,          3,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(fanout_16m,     0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               0,               FANOUT_GATE,   0,             0,          0,          2,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(fanout_12m,     0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               0,               FANOUT_GATE,   0,             0,          0,          1,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(fanout_24m,     0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               0,               FANOUT_GATE,   0,             0,          0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(fanout_27m,     FANOUT27M_CFG,   24,      2,            FANOUT27M_CFG,     0,         5,           8,          2,          0,          0,               0,               0,             0,             31,         0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(fanout_pclk,    0,               0,       0,            FANOUTPCLK_CFG,     0,         5,          0,          0,          0,          FANOUTPCLK_CFG,  0,               0,             0,             31,         0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(fanout0,        CCMU_FANOUT_CFG, 0,       3,            0,                  0,         0,          0,          0,          0,          CCMU_FANOUT_CFG, 0,               0,             0,             21,         0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(fanout1,        CCMU_FANOUT_CFG, 3,       3,            0,                  0,         0,          0,          0,          0,          CCMU_FANOUT_CFG, 0,               0,             0,             22,         0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(fanout2,        CCMU_FANOUT_CFG, 6,       3,            0,                  0,         0,          0,          0,          0,          CCMU_FANOUT_CFG, 0,               0,             0,             23,         0,          0,              0,             &clk_lock, NULL,             0);

SUNXI_CLK_PERIPH(cpurahbs,       CPUS_AHBS_CFG,   24,      3,            CPUS_AHBS_CFG,      0,         5,          8,          2,          0,          0,               0,               0,             0,             0,          0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(cpurapbs0,      CPUS_APBS0_CFG,  24,      3,            CPUS_APBS0_CFG,     0,         5,          8,          2,          0,          0,               0,               0,             0,             0,          0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(cpurtwd,        0,               0,       0,            0,	             0,         0,          0,          0,          0,          0,               0,               CPUS_TWD_GATE, 0,             0,          0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(cpurppu,        0,               0,       0,            0,	             0,         0,          0,          0,          0,          0,               CPUS_PPU_GATE,   CPUS_PPU_GATE, 0,             0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(cpurrtc,        0,               0,       0,            0,	             0,         0,          0,          0,          0,          0,               CPUS_RTC_GATE,   CPUS_RTC_GATE, 0,             0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(cpurcpus,        0,               0,       0,            0,	             0,         0,          0,          0,          0,          0,               CPUS_CPU_GATE,   CPUS_CPU_GATE, 0,             0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(losc,           LOSC_CTL_CFG,    0,       1,            0,                  0,         0,          0,          0,          0,          0,               0,                0,                0,             0,         0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(losc_out,       LOSC_OUT_GATE,   1,       2,            0,                  0,         0,          0,          0,          0,          LOSC_OUT_GATE,   0,                0,                0,             0,         0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(rtc_spi,        0,               0,       0,            RTC_SPI_CFG,        0,         5,          0,          0,          0,          RTC_SPI_CFG,     0,                0,                0,            31,         0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(hosc32k,        0,               0,       0,            0,                  0,         0,          0,          0,          0,          LOSC_OUT_GATE,   0,                0,                0,            16,         0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(dcxo_out,       0,               0,       0,            0,                  0,         0,          0,          0,          0,          DCXO_OUT_CFG,    0,               0,             0,             31,         0,          0,             0,             &clk_lock, NULL,             0);

struct periph_init_data sunxi_periphs_init[] = {
	{"cpu",            CLK_GET_RATE_NOCACHE, cpu_parents,            ARRAY_SIZE(cpu_parents),            &sunxi_clk_periph_cpu              },
	{"axi",            0,                    cpu_apb_axi_parents,    ARRAY_SIZE(cpu_apb_axi_parents),    &sunxi_clk_periph_axi              },
	{"cpuapb",         0,                    cpu_apb_axi_parents,    ARRAY_SIZE(cpu_apb_axi_parents),    &sunxi_clk_periph_cpuapb           },
	{"ahb",            0,                    ahb_apb_parents,        ARRAY_SIZE(ahb_apb_parents),        &sunxi_clk_periph_ahb              },
	{"apb0",           0,                    ahb_apb_parents,        ARRAY_SIZE(ahb_apb_parents),        &sunxi_clk_periph_apb0             },
	{"apb1",           0,                    ahb_apb_parents,        ARRAY_SIZE(ahb_apb_parents),        &sunxi_clk_periph_apb1             },
	{"mbus",           0,                    mbus_parents,           ARRAY_SIZE(mbus_parents),           &sunxi_clk_periph_mbus             },
	{"de",             0,                    de_parents,             ARRAY_SIZE(de_parents),             &sunxi_clk_periph_de               },
	{"g2d",            0,                    de_parents,             ARRAY_SIZE(de_parents),             &sunxi_clk_periph_g2d              },
	{"ce",             0,                    ce_parents,             ARRAY_SIZE(ce_parents),             &sunxi_clk_periph_ce               },
	{"ve",             0,                    ve_parents,             ARRAY_SIZE(ve_parents),             &sunxi_clk_periph_ve               },
	{"npu",            0,                    npu_parents,            ARRAY_SIZE(npu_parents),            &sunxi_clk_periph_npu              },
	{"dma",            0,                    ahbmod_parents,         ARRAY_SIZE(ahbmod_parents),         &sunxi_clk_periph_dma              },
	{"msgbox0",        0,                    ahbmod_parents,         ARRAY_SIZE(ahbmod_parents),         &sunxi_clk_periph_msgbox0          },
	{"msgbox1",        0,                    ahbmod_parents,         ARRAY_SIZE(ahbmod_parents),         &sunxi_clk_periph_msgbox1          },
	{"spinlock",       0,                    ahbmod_parents,         ARRAY_SIZE(ahbmod_parents),         &sunxi_clk_periph_spinlock         },
	{"hstimer",        0,                    ahbmod_parents,         ARRAY_SIZE(ahbmod_parents),         &sunxi_clk_periph_hstimer          },
	{"avs",            0,                    hosc_parents,           ARRAY_SIZE(hosc_parents),           &sunxi_clk_periph_avs              },
	{"dbgsys",         0,                    hosc_parents,           ARRAY_SIZE(hosc_parents),           &sunxi_clk_periph_dbgsys           },
	{"pwm",            0,                    apb0mod_parents,        ARRAY_SIZE(apb0mod_parents),        &sunxi_clk_periph_pwm              },
	{"iommu",          0,                    ahbmod_parents,         ARRAY_SIZE(ahbmod_parents),         &sunxi_clk_periph_iommu            },
	{"sdram",          0,                    sdram_parents,          ARRAY_SIZE(sdram_parents),          &sunxi_clk_periph_sdram            },
	{"sdmmc0_mod",     0,                    smhc01_parents,         ARRAY_SIZE(smhc01_parents),         &sunxi_clk_periph_sdmmc0_mod       },
	{"sdmmc0_rst",     0,                    smhc01_parents,         ARRAY_SIZE(smhc01_parents),         &sunxi_clk_periph_sdmmc0_rst       },
	{"sdmmc0_bus",     0,                    smhc01_parents,         ARRAY_SIZE(smhc01_parents),         &sunxi_clk_periph_sdmmc0_bus       },
	{"sdmmc1_mod",     0,                    smhc01_parents,         ARRAY_SIZE(smhc01_parents),         &sunxi_clk_periph_sdmmc1_mod       },
	{"sdmmc1_rst",     0,                    smhc01_parents,         ARRAY_SIZE(smhc01_parents),         &sunxi_clk_periph_sdmmc1_rst       },
	{"sdmmc1_bus",     0,                    smhc01_parents,         ARRAY_SIZE(smhc01_parents),         &sunxi_clk_periph_sdmmc1_bus       },
	{"sdmmc2_mod",     0,                    smhc2_parents,          ARRAY_SIZE(smhc2_parents),          &sunxi_clk_periph_sdmmc2_mod       },
	{"sdmmc2_rst",     0,                    smhc2_parents,          ARRAY_SIZE(smhc2_parents),          &sunxi_clk_periph_sdmmc2_rst       },
	{"sdmmc2_bus",     0,                    smhc2_parents,          ARRAY_SIZE(smhc2_parents),          &sunxi_clk_periph_sdmmc2_bus       },
	{"uart0",          0,                    apb1mod_parents,        ARRAY_SIZE(apb1mod_parents),        &sunxi_clk_periph_uart0            },
	{"uart1",          0,                    apb1mod_parents,        ARRAY_SIZE(apb1mod_parents),        &sunxi_clk_periph_uart1            },
	{"uart2",          0,                    apb1mod_parents,        ARRAY_SIZE(apb1mod_parents),        &sunxi_clk_periph_uart2            },
	{"uart3",          0,                    apb1mod_parents,        ARRAY_SIZE(apb1mod_parents),        &sunxi_clk_periph_uart3            },
	{"twi0",           0,                    apb1mod_parents,        ARRAY_SIZE(apb1mod_parents),        &sunxi_clk_periph_twi0             },
	{"twi1",           0,                    apb1mod_parents,        ARRAY_SIZE(apb1mod_parents),        &sunxi_clk_periph_twi1             },
	{"twi2",           0,                    apb1mod_parents,        ARRAY_SIZE(apb1mod_parents),        &sunxi_clk_periph_twi2             },
	{"twi3",           0,                    apb1mod_parents,        ARRAY_SIZE(apb1mod_parents),        &sunxi_clk_periph_twi3             },
	{"twi4",           0,                    apb1mod_parents,        ARRAY_SIZE(apb1mod_parents),        &sunxi_clk_periph_twi4             },
	{"spi0",           0,                    spi_parents,            ARRAY_SIZE(spi_parents),            &sunxi_clk_periph_spi0             },
	{"spi1",           0,                    spi_parents,            ARRAY_SIZE(spi_parents),            &sunxi_clk_periph_spi1             },
	{"spi2",           0,                    spi_parents,            ARRAY_SIZE(spi_parents),            &sunxi_clk_periph_spi2             },
	{"spi3",           0,                    spi_parents,            ARRAY_SIZE(spi_parents),            &sunxi_clk_periph_spi3             },
	{"spif",           0,                    spif_parents,           ARRAY_SIZE(spif_parents),           &sunxi_clk_periph_spif             },
	{"gmac_25m",       0,                    gmac_25m_parents,       ARRAY_SIZE(gmac_25m_parents),       &sunxi_clk_periph_gmac_25m         },
	{"gmac",           0,                    ahbmod_parents,         ARRAY_SIZE(ahbmod_parents),         &sunxi_clk_periph_gmac             },
	{"gpadc",          0,                    apb0mod_parents,        ARRAY_SIZE(apb0mod_parents),        &sunxi_clk_periph_gpadc            },
	{"ths",            0,                    apb0mod_parents,        ARRAY_SIZE(apb0mod_parents),        &sunxi_clk_periph_ths              },
	/*
	 * Strictly speaking, the pll_audiox4 and pll_audio are not pll clocks, although they are called pll
	 */
	{"pll_audiox4",    0,                    pll_audiox4_parents,    ARRAY_SIZE(pll_audiox4_parents),    &sunxi_clk_periph_pll_audiox4      },
	{"pll_audio",      CLK_SET_RATE_PARENT,  pll_audio_parents,      ARRAY_SIZE(pll_audio_parents),      &sunxi_clk_periph_pll_audio        },
	{"i2s0",           0,                    audio_parents,          ARRAY_SIZE(audio_parents),          &sunxi_clk_periph_i2s0             },
	{"i2s1",           0,                    audio_parents,          ARRAY_SIZE(audio_parents),          &sunxi_clk_periph_i2s1             },
	{"dmic",           0,                    audio_parents,          ARRAY_SIZE(audio_parents),          &sunxi_clk_periph_dmic             },
	{"codec_dac",      0,                    audio_parents,          ARRAY_SIZE(audio_parents),          &sunxi_clk_periph_codec_dac        },
	{"codec_adc",      0,                    audio_parents,          ARRAY_SIZE(audio_parents),          &sunxi_clk_periph_codec_adc        },
	{"usbphy0",        0,                    hosc_parents,           ARRAY_SIZE(hosc_parents),           &sunxi_clk_periph_usbphy0          },
	{"usbohci0",       0,                    ahbmod_parents,         ARRAY_SIZE(ahbmod_parents),         &sunxi_clk_periph_usbohci0         },
	{"usbohci0_12m",   0,                    usbohci_12m_parents,    ARRAY_SIZE(usbohci_12m_parents),    &sunxi_clk_periph_usbohci0_12m     },
	{"usbehci0",       0,                    ahbmod_parents,         ARRAY_SIZE(ahbmod_parents),         &sunxi_clk_periph_usbehci0         },
	{"usbotg",         0,                    ahbmod_parents,         ARRAY_SIZE(ahbmod_parents),         &sunxi_clk_periph_usbotg           },
	{"dpss_top",       0,                    ahbmod_parents,         ARRAY_SIZE(ahbmod_parents),         &sunxi_clk_periph_dpss_top         },
	{"mipi_dsi",       0,                    dsi_parents,            ARRAY_SIZE(dsi_parents),            &sunxi_clk_periph_mipi_dsi         },
	{"tcon_lcd",       0,                    tcon_lcd_parents,       ARRAY_SIZE(tcon_lcd_parents),       &sunxi_clk_periph_tcon_lcd         },
	{"csi_top",        0,                    csi_top_parents,        ARRAY_SIZE(csi_top_parents),        &sunxi_clk_periph_csi_top          },
	{"csi_master0",    0,                    csi_master_parents,     ARRAY_SIZE(csi_master_parents),     &sunxi_clk_periph_csi_master0      },
	{"csi_master1",    0,                    csi_master_parents,     ARRAY_SIZE(csi_master_parents),     &sunxi_clk_periph_csi_master1      },
	{"csi_master2",    0,                    csi_master_parents,     ARRAY_SIZE(csi_master_parents),     &sunxi_clk_periph_csi_master2      },
	{"isp",            0,                    isp_parents,            ARRAY_SIZE(isp_parents),            &sunxi_clk_periph_isp              },
	{"wiegand",        0,                    apb0mod_parents,        ARRAY_SIZE(apb0mod_parents),        &sunxi_clk_periph_wiegand          },
	{"e907",           0,                    e907_parents,           ARRAY_SIZE(e907_parents),           &sunxi_clk_periph_e907             },
	{"e907_axi",       0,                    e907_axi_parents,       ARRAY_SIZE(e907_axi_parents),       &sunxi_clk_periph_e907_axi         },
	{"fanout_25m",     0,                    fanout25m_parents,      ARRAY_SIZE(fanout25m_parents),      &sunxi_clk_periph_fanout_25m       },
	{"fanout_16m",     0,                    fanout16m_parents,      ARRAY_SIZE(fanout16m_parents),      &sunxi_clk_periph_fanout_16m       },
	{"fanout_12m",     0,                    fanout12m_parents,      ARRAY_SIZE(fanout12m_parents),      &sunxi_clk_periph_fanout_12m       },
	{"fanout_24m",     0,                    fanout24m_parents,      ARRAY_SIZE(fanout24m_parents),      &sunxi_clk_periph_fanout_24m       },
	{"fanout_27m",     0,                    fanout27m_parents,      ARRAY_SIZE(fanout27m_parents),      &sunxi_clk_periph_fanout_27m       },
	{"fanout_pclk",    0,                    fanout_pclk_parents,    ARRAY_SIZE(fanout_pclk_parents),    &sunxi_clk_periph_fanout_pclk      },
	{"fanout0",        0,                    fanout012_parents,      ARRAY_SIZE(fanout012_parents),      &sunxi_clk_periph_fanout0          },
	{"fanout1",        0,                    fanout012_parents,      ARRAY_SIZE(fanout012_parents),      &sunxi_clk_periph_fanout1          },
	{"fanout2",        0,                    fanout012_parents,      ARRAY_SIZE(fanout012_parents),      &sunxi_clk_periph_fanout2          },
};

struct periph_init_data sunxi_periphs_cpus_init[] = {
	{"cpurahbs",        CLK_GET_RATE_NOCACHE|CLK_READONLY,  cpurahbs_apbs_parents,  ARRAY_SIZE(cpurahbs_apbs_parents),  &sunxi_clk_periph_cpurahbs      },
	{"cpurapbs0",       CLK_GET_RATE_NOCACHE|CLK_READONLY,  cpurahbs_apbs_parents,  ARRAY_SIZE(cpurahbs_apbs_parents),  &sunxi_clk_periph_cpurapbs0     },
	{"cpurtwd",         CLK_GET_RATE_NOCACHE|CLK_READONLY,  cpurtwd_parents,        ARRAY_SIZE(cpurtwd_parents),        &sunxi_clk_periph_cpurtwd       },
	{"cpurppu",         CLK_GET_RATE_NOCACHE|CLK_READONLY,  cpurtwd_parents,        ARRAY_SIZE(cpurtwd_parents),        &sunxi_clk_periph_cpurppu       },
	{"cpurrtc",         CLK_GET_RATE_NOCACHE|CLK_READONLY,  cpurrtc_parents,        ARRAY_SIZE(cpurrtc_parents),        &sunxi_clk_periph_cpurrtc       },
	{"cpurcpus",        CLK_GET_RATE_NOCACHE|CLK_READONLY,  cpurtwd_parents,        ARRAY_SIZE(cpurtwd_parents),        &sunxi_clk_periph_cpurcpus      },
	{"pll_audio",        CLK_GET_RATE_NOCACHE|CLK_READONLY|CLK_SET_RATE_PARENT,  pll_audio_parents,        ARRAY_SIZE(pll_audio_parents),        &sunxi_clk_periph_pll_audio      },

};

struct periph_init_data sunxi_periphs_rtc_init[] = {
	{"losc",            CLK_READONLY,                       losc_parents,           ARRAY_SIZE(losc_parents),           &sunxi_clk_periph_losc          },
	{"losc_out",        0,                                  losc_out_parents,       ARRAY_SIZE(losc_out_parents),       &sunxi_clk_periph_losc_out      },
	{"rtc_spi",         0,                                  cpurrtc_parents,        ARRAY_SIZE(cpurrtc_parents),        &sunxi_clk_periph_rtc_spi       },
	{"hosc32k",         0,                                  hosc32k_parents,        ARRAY_SIZE(hosc32k_parents),        &sunxi_clk_periph_hosc32k       },
};

/* specify parent of fix_factor clocks, not in dts */
struct fix_factor_init_data sunxi_fix_factos[] = {
	{"pll_video0",		"pll_video0x4"		},
	{"pll_periph0600m",	"pll_periph0x2"		},
	{"pll_periph0400m",	"pll_periph0x2"		},
	{"pll_periph0200m",	"pll_periph0400m"	},
	{"pll_periph0300m",	"pll_periph0600m"	},
	{"pll_periph0150m",	"pll_periph0300m"	},
};

int dcxo_out_priv_enable(struct clk_hw *hw)
{
	unsigned long reg;

	if (sunxi_clk_periph_dcxo_out.gate.enable) {
		reg = readl(sunxi_clk_periph_dcxo_out.gate.enable);
		reg = SET_BITS(31, 1, reg, 0);
		writel(reg, sunxi_clk_periph_dcxo_out.gate.enable);
	}

	return 0;
}

void dcxo_out_priv_disable(struct clk_hw *hw)
{
	unsigned long reg;

	if (sunxi_clk_periph_dcxo_out.gate.enable) {
		reg = readl(sunxi_clk_periph_dcxo_out.gate.enable);
		reg = SET_BITS(31, 1, reg, 1);
		writel(reg, sunxi_clk_periph_dcxo_out.gate.enable);
	}
}

int dcxo_out_priv_is_enabled(struct clk_hw *hw)
{
	unsigned long reg;

	if (sunxi_clk_periph_dcxo_out.gate.enable) {
		reg = readl(sunxi_clk_periph_dcxo_out.gate.enable);
		if (GET_BITS(sunxi_clk_periph_dcxo_out.gate.enb_shift, 1, reg))
			return 0;
		else
			return 1;
	}

	return 0;
}

void set_dcxo_out_priv_ops(struct clk_ops *priv_ops)
{
	priv_ops->enable = dcxo_out_priv_enable;
	priv_ops->disable = dcxo_out_priv_disable;
	priv_ops->is_enabled = dcxo_out_priv_is_enabled;
}

struct clk_ops dcxo_out_priv_ops;
void sunxi_set_clk_priv_ops(char *clk_name, struct clk_ops  *clk_priv_ops,
	void (*set_priv_ops)(struct clk_ops *priv_ops))
{
	int i = 0;
	sunxi_clk_get_periph_ops(clk_priv_ops);
	set_priv_ops(clk_priv_ops);
	for (i = 0; i < (ARRAY_SIZE(sunxi_periphs_cpus_init)); i++) {
		if (!strcmp(sunxi_periphs_cpus_init[i].name, clk_name))
			sunxi_periphs_cpus_init[i].periph->priv_clkops =
								clk_priv_ops;
	}
}

/*
 * sunxi_clk_get_factor_by_name() - Get factor clk init config
 */
struct factor_init_data *sunxi_clk_get_factor_by_name(const char *name)
{
	struct factor_init_data *factor;
	int i;

	/* get pll clk init config */
	for (i = 0; i < ARRAY_SIZE(sunxi_factos); i++) {
		factor = &sunxi_factos[i];
		if (strcmp(name, factor->name))
			continue;
		return factor;
	}

	return NULL;
}

struct periph_init_data *sunxi_clk_get_periph_rtc_by_name(const char *name)
{
	return NULL;
}
/*
 * sunxi_clk_get_periph_by_name() - Get periph clk init config
 */
struct periph_init_data *sunxi_clk_get_periph_by_name(const char *name)
{
	struct periph_init_data *perpih;
	int i;

	for (i = 0; i < ARRAY_SIZE(sunxi_periphs_init); i++) {
		perpih = &sunxi_periphs_init[i];
		if (strcmp(name, perpih->name))
			continue;
		return perpih;
	}

	return NULL;
}

/*
 * sunxi_clk_get_periph_cpus_by_name() - Get periph clk init config
 */
struct periph_init_data *sunxi_clk_get_periph_cpus_by_name(const char *name)
{
	struct periph_init_data *perpih;
	int i;

	for (i = 0; i < ARRAY_SIZE(sunxi_periphs_cpus_init); i++) {
		perpih = &sunxi_periphs_cpus_init[i];
		if (strcmp(name, perpih->name))
			continue;
		return perpih;
	}

	return NULL;
}
struct periph_init_data *sunxi_cpus_clk_get_periph_by_name(const char *name)
{
	struct periph_init_data *perpih;
	int i;

	for (i = 0; i < ARRAY_SIZE(sunxi_periphs_cpus_init); i++) {
		perpih = &sunxi_periphs_cpus_init[i];
		if (strcmp(name, perpih->name))
			continue;
		return perpih;
	}

	return NULL;
}

void init_clocks(void)
{
	int i;
	int node;
	struct clk *clk;
	u32 div, mult;
	char node_name[80];
	struct factor_init_data *factor;
	struct fix_factor_init_data *fix_factor;
	struct periph_init_data *periph;
	tick_printf("line:%d %s\n", __LINE__, __func__);
	/* get clk register base address */
	sunxi_clk_base = (void *)SUNXI_CCM_BASE; // fixed base address.
	sunxi_clk_cpus_base = (void *)SUNXI_PRCM_BASE;

	sunxi_clk_periph_losc_out.gate.bus =
	    (void *)(0x07090000 + LOSC_OUT_GATE);
	sunxi_clk_periph_dcxo_out.gate.enable =
	    (void *)(0x07090000 + DCXO_OUT_CFG);
	sunxi_set_clk_priv_ops("dcxo_out", &dcxo_out_priv_ops,
			       set_dcxo_out_priv_ops);
	sunxi_clk_factor_initlimits();

	clk_register_fixed_rate(NULL, "hosc", NULL, CLK_IS_ROOT, 24000000);

	/* register normal factors, based on sunxi factor framework */
	for (i = 0; i < ARRAY_SIZE(sunxi_factos); i++) {
		factor = &sunxi_factos[i];
		factor->priv_regops = NULL;
		sunxi_clk_register_factors(NULL, (void *)sunxi_clk_base,
					   (struct factor_init_data *)factor);
	}

	/* register fix_factor clock */
	for (i = 0; i < ARRAY_SIZE(sunxi_fix_factos); i++) {
		fix_factor = &sunxi_fix_factos[i];
		sprintf(node_name, "/clocks/%s", fix_factor->name);
		node = fdt_path_offset(working_fdt, node_name);
		if (node < 0) {
			printf("not find the node:%s\n", fix_factor->name);
			return;
		}

		if (fdt_getprop_u32(working_fdt, node, "clock-div", &div) < 0) {
			printf("get clock-div err\n");
			return;
		}

		if (fdt_getprop_u32(working_fdt, node, "clock-mult", &mult) < 0) {
			printf("get clock-mult err\n");
			return;
		}

		clk = clk_register_fixed_factor(NULL, fix_factor->name,
				fix_factor->parent_name, 0, mult, div);
		if (!clk) {
			printf("register fix_factor clk error\n");
			return;
		}

	}

	/* register periph clock */
	for (i = 0; i < ARRAY_SIZE(sunxi_periphs_init); i++) {
		periph = &sunxi_periphs_init[i];
		periph->periph->priv_regops = NULL;
		sunxi_clk_register_periph(periph, sunxi_clk_base);
	}
}
