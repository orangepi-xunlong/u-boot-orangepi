/*
 * (C) Copyright 2014-2019
 * allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * caiyongheng <caiyongheng@allwinnertech.com>
 *
 * some simple description for this code
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 */

#include <common.h>
#include <errno.h>
#include <asm/io.h>
#include <mapmem.h>
#include <fdtdec.h>
#include <fdt_support.h>
#include <sys_config.h>
#include <asm/global_data.h>
#include <asm/gpio.h>
#include <asm/arch/dma.h>

#include "sun50iw10-codec.h"
#include "sunxi_rw_func.h"

DECLARE_GLOBAL_DATA_PTR;

struct sunxi_codec {
	void __iomem *addr_clkbase;
	void __iomem *addr_dbase;
	void __iomem *addr_abase;
	user_gpio_set_t spk_cfg;
	u32 hp_vol;
	u32 spk_gpio;
	u32 pa_ctl_level;
};

static struct sunxi_codec g_codec;

#if 0
static int sunxi_codec_read(struct sunxi_codec *codec, u32 reg)
{
	if (reg >= SUNXI_PR_CFG) {
		/* Analog part */
		reg = reg - SUNXI_PR_CFG;
		return read_prcm_wvalue(reg, codec->addr_abase);
	} else {
		return codec_rdreg(codec->addr_dbase + reg);
	}
	return 0;
}
#endif

static int sunxi_codec_write(struct sunxi_codec *codec, u32 reg, u32 val)
{
	codec_wrreg(codec->addr_dbase + reg, val);
	return 0;
}

static int sunxi_codec_update_bits(struct sunxi_codec *codec, u32 reg, u32 mask,
				   u32 val)
{
	codec_wrreg_bits(codec->addr_dbase + reg, mask, val);
	return 0;
}

static const struct sample_rate sample_rate_conv[] = {
	{44100, 0},
	{48000, 0},
	{8000, 5},
	{32000, 1},
	{22050, 2},
	{24000, 2},
	{16000, 3},
	{11025, 4},
	{12000, 4},
	{192000, 6},
	{96000, 7},
};

/*
 * sample bits
 * sample rate
 * channels
 *
 */

int sunxi_codec_hw_params(u32 sb, u32 sr, u32 ch)
{
	int i;
	struct sunxi_codec *codec = &g_codec;

	/*
	 * Audio codec
	 * SUNXI_DAC_FIFOC		0x10
	 * SUNXI_DAC_DPC		0x00
	 *
	 */
	/* set playback sample resolution, only little endian*/
	switch (sb) {
	case 16:
		sunxi_codec_update_bits(codec, SUNXI_DAC_FIFOC,
					(3 << FIFO_MODE), (3 << FIFO_MODE));
		sunxi_codec_update_bits(codec, SUNXI_DAC_FIFOC,
					(1 << TX_SAMPLE_BITS), (0 << TX_SAMPLE_BITS));
		break;
	case 24:
	case 32:
		sunxi_codec_update_bits(codec, SUNXI_DAC_FIFOC,
					(3 << FIFO_MODE), (0 << FIFO_MODE));
		sunxi_codec_update_bits(codec, SUNXI_DAC_FIFOC,
					(1 << TX_SAMPLE_BITS), (1 << TX_SAMPLE_BITS));
		break;
	default:
		return -1;
	}
	/* set playback sample rate */
	for (i = 0; i < ARRAY_SIZE(sample_rate_conv); i++) {
		if (sample_rate_conv[i].samplerate == sr) {
			sunxi_codec_update_bits(codec, SUNXI_DAC_FIFOC,
						(0x7 << DAC_FS),
						(sample_rate_conv[i].rate_bit << DAC_FS));
		}
	}
	/* set playback channels */
	if (ch == 1) {
		sunxi_codec_update_bits(codec, SUNXI_DAC_FIFOC,
					(1 << DAC_MONO_EN), (1 << DAC_MONO_EN));
	} else if (ch == 2) {
		sunxi_codec_update_bits(codec, SUNXI_DAC_FIFOC,
					(1 << DAC_MONO_EN), (0 << DAC_MONO_EN));
	} else {
		printf("channel:%u is not support!\n", ch);
	}
	return 0;
}

static int sunxi_codec_init(struct sunxi_codec *codec)
{
	void __iomem *cfg_reg;
	volatile u32 tmp_val = 0;

	/*
	 * CCMU register
	 * BASE				0x0300 1000
	 * PLL_AUDIO_CTRL_REG		0x78
	 * PLL_AUDIO_PAT0_CTRL_REG	0x178
	 * PLL_AUDIO_PAT1_CTRL_REG	0x17C
	 * PLL_AUDIO_BIAS_REG		0x378
	 * AUDIO_CODEC_1X_CLK_REG	0xA50
	 * AUDIO_CODEC_BGR_REG		0xA5C
	 */
	cfg_reg = codec->addr_clkbase + 0x78;
	tmp_val = 0x89042701;
	writel(tmp_val, cfg_reg);
	/*printf("reg:0x%08x, before: 0x%x, after:0x%x\n", cfg_reg, val, tmp_val);*/
	cfg_reg = codec->addr_clkbase + 0x178;
	tmp_val = 0xc001eb85;
	writel(tmp_val, cfg_reg);

	cfg_reg = codec->addr_clkbase + 0x17C;
	tmp_val = 0x0;
	writel(tmp_val, cfg_reg);

	cfg_reg = codec->addr_clkbase + 0x378;
	tmp_val = 0x00030000;
	writel(tmp_val, cfg_reg);

	cfg_reg = codec->addr_clkbase + 0xA5C;
	tmp_val = 0x00010000;
	writel(tmp_val, cfg_reg);
	tmp_val = 0x00010001;
	writel(tmp_val, cfg_reg);

	cfg_reg = codec->addr_clkbase + 0xA50;
	tmp_val = 0x80000003;
	writel(tmp_val, cfg_reg);

	/*
	 * Audio codec
	 * SUNXI_DAC_DAP_CTL		0xF0
	 * SUNXI_DAC_DPC		0x00
	 *
	 */

	/* DAC_VOL_SEL default disabled */
	sunxi_codec_update_bits(codec, SUNXI_DAC_VOL_CTRL,
			(0x1 << DAC_VOL_SEL), (0x1 << DAC_VOL_SEL));

	/* set digital volume 0x0, 0*-1.16 = 0dB */
	sunxi_codec_update_bits(codec, SUNXI_DAC_DPC, (0x3f << DVOL), (0x0 << DVOL));
	/* set MIC2,3 Boost AMP disable */
	/*sunxi_codec_write(codec, SUNXI_MIC2_MIC3_CTL, 0x44);*/
	/* set LADC Mixer mute */
	/*sunxi_codec_write(codec, SUNXI_LADCMIX_SRC, 0x0);*/
	/* set LINEOUT volume, such as 0x19, -(31-0x19)*1.5 = -9dB */
	sunxi_codec_update_bits(codec, SUNXI_DAC_REG, (0x7 << HEADPHONE_GAIN),
				(codec->hp_vol << HEADPHONE_GAIN));
	/* set Right Lineout source */
	//sunxi_codec_update_bits(codec, SUNXI_LINEOUT_CTL0, (0x1 << LINEOUTR_SRC),
	//			(0x1 << LINEOUTR_SRC));

	/* For improve performance of THD+N about HP */
	sunxi_codec_update_bits(codec, SUNXI_HEADPHONE_REG,
			(0x3 << CP_CLKS), (0x3 << CP_CLKS));

	/* Swap the L/R channel to make it output right */
	sunxi_codec_update_bits(codec, SUNXI_DAC_DG,
			(0x1<<DAC_SWP), (0x1<<DAC_SWP));

	/* To fix some pop noise */
	sunxi_codec_update_bits(codec, SUNXI_HEADPHONE_REG,
			(0x1 << HPCALIFIRST), (0x1 << HPCALIFIRST));

	sunxi_codec_update_bits(codec, SUNXI_HEADPHONE_REG,
			(0x3 << HPPA_DEL), (0x3 << HPPA_DEL));

	sunxi_codec_update_bits(codec, SUNXI_DAC_REG,
			(0x3 << CPLDO_VOLTAGE), (0x1 << CPLDO_VOLTAGE));


	return 0;
}

#if 1
fdt_addr_t new_fdtdec_get_addr_size_fixed(const void *blob, int node,
		const char *prop_name, int index, int na,
		int ns, fdt_size_t *sizep,
		bool translate)
{
	const fdt32_t *prop, *prop_end;
	const fdt32_t *prop_addr, *prop_size, *prop_after_size;
	int len;
	fdt_addr_t addr;

	prop = fdt_getprop(blob, node, prop_name, &len);
	if (!prop) {
		debug("(not found)\n");
		return FDT_ADDR_T_NONE;
	}
	prop_end = prop + (len / sizeof(*prop));

	prop_addr = prop + (index * (na + ns));
	prop_size = prop_addr + na;
	prop_after_size = prop_size + ns;
	if (prop_after_size > prop_end) {
		debug("(not enough data: expected >= %d cells, got %d cells)\n",
		      (u32)(prop_after_size - prop), ((u32)(prop_end - prop)));
		return FDT_ADDR_T_NONE;
	}

#if CONFIG_IS_ENABLED(OF_TRANSLATE)
	if (translate)
		addr = fdt_translate_address(blob, node, prop_addr);
	else
#endif
		addr = fdtdec_get_number(prop_addr, na);

	if (sizep) {
		*sizep = fdtdec_get_number(prop_size, ns);
		debug("addr=%08llx, size=%llx\n", (unsigned long long)addr,
		      (unsigned long long)*sizep);
	} else {
		debug("addr=%08llx\n", (unsigned long long)addr);
	}

	return addr;
}
#else
#define new_fdtdec_get_addr_size_fixed fdtdec_get_addr_size_fixed
#endif

static int get_sunxi_codec_values(struct sunxi_codec *codec, const void *blob)
{
	int node, ret;
	fdt_addr_t addr = 0;
	fdt_size_t size = 0;

	node = fdt_node_offset_by_compatible(blob, 0, "allwinner,clk-init");
	if (node < 0) {
		printf("unable to find allwinner,clk-init node in device tree.\n");
		return node;
	}
	addr = new_fdtdec_get_addr_size_fixed(blob, node, "reg", 0, 2, 2, &size, false);
	codec->addr_clkbase = map_sysmem(addr, size);

	node = fdt_node_offset_by_compatible(blob, 0, "allwinner,sunxi-internal-codec");
	if (node < 0) {
		printf("unable to find allwinner,sunxi-internal-codec node in device tree.\n");
		return node;
	}
	if (!fdtdec_get_is_enabled(blob, node)) {
		printf("sunxi-internal-codec disabled in device tree.\n");
		return -1;
	}

	/*regs = fdtdec_get_addr();*/
	addr = new_fdtdec_get_addr_size_fixed(blob, node, "reg", 0, 2, 2, &size, false);
	codec->addr_dbase = map_sysmem(addr, size);

	addr = new_fdtdec_get_addr_size_fixed(blob, node, "reg", 1, 2, 2, &size, false);
	codec->addr_abase = map_sysmem(addr, size);

	printf("dbase:0x%x, abase:0x%x\n", codec->addr_dbase, codec->addr_abase);
	ret = fdt_getprop_u32(blob, node, "hp_vol", &codec->hp_vol);
	if (ret < 0) {
		printf("lineout_vol not set. default 0x1a\n");
		codec->hp_vol = 0x3;
	}
	ret = fdt_getprop_u32(blob, node, "pa_ctl_level", &codec->pa_ctl_level);
	if (ret < 0) {
		printf("pa_ctl_level not set. default 1\n");
		codec->pa_ctl_level = 1;
	}

	ret = fdt_get_one_gpio_by_offset(node, "gpio-spk", &codec->spk_cfg);
	if (ret < 0 || codec->spk_cfg.port == 0) {
		printf("parser gpio-spk failed, ret:%d, port:%u\n", ret, codec->spk_cfg.port);
		return -1;
	}
	codec->spk_gpio = (codec->spk_cfg.port - 1) * 32 + codec->spk_cfg.port_num;

	gpio_request(codec->spk_gpio, NULL);
#if 0
	printf("pa_ctl_level:%u, port:%u, num:%u, gpio:%u\n",
	       codec->pa_ctl_level, codec->spk_cfg.port,
	       codec->spk_cfg.port_num, codec->spk_gpio);
#endif

	return 0;
}

int sunxi_codec_playback_prepare(void)
{
	struct sunxi_codec *codec = &g_codec;

	/* FIFO flush, clear pending, clear sample counter */
	sunxi_codec_update_bits(codec, SUNXI_DAC_FIFOC,
				(1 << FIFO_FLUSH), (1 << FIFO_FLUSH));
	sunxi_codec_write(codec, SUNXI_DAC_FIFOS,
			  (1 << DAC_TXE_INT | 1 << DAC_TXU_INT | 1 << DAC_TXO_INT));
	sunxi_codec_write(codec, SUNXI_DAC_CNT, 0);

	/* enable FIFO empty DRQ */
	sunxi_codec_update_bits(codec, SUNXI_DAC_FIFOC,
				(1 << DAC_DRQ_EN), (1 << DAC_DRQ_EN));

	/* Enable DAC analog left channel */
	sunxi_codec_update_bits(codec, SUNXI_DAC_REG, (0x1 << DACLEN),
				(0x1 << DACLEN));
	/* Enable DAC analog right channel */
	sunxi_codec_update_bits(codec, SUNXI_DAC_REG, (0x1 << DACREN),
				(0x1 << DACREN));
	/* enable DAC digital part */
	sunxi_codec_update_bits(codec, SUNXI_DAC_DPC,
				(0x1 << EN_DAC), (0x1 << EN_DAC));
	/* enable left and right HPOUT */
	sunxi_codec_update_bits(codec, SUNXI_HEADPHONE_REG,
				(0x1 << HPPA_EN), (0x1 << HPPA_EN));
	sunxi_codec_update_bits(codec, SUNXI_HEADPHONE_REG,
				(0x1 << HPOUTPUTEN), (0x1 << HPOUTPUTEN));
	sunxi_codec_update_bits(codec, SUNXI_HEADPHONE_REG,
				(0x1 << HPINPUTEN), (0x1 << HPINPUTEN));
	/* increase delay time, if Pop occured */
	mdelay(10);
	gpio_direction_output(codec->spk_gpio, codec->pa_ctl_level);

	return 0;
}

int sunxi_codec_probe(void)
{

	int ret;
	struct sunxi_codec *codec = &g_codec;
	const void *blob = gd->fdt_blob;

	if (get_sunxi_codec_values(codec, blob) < 0) {
		debug("FDT Codec values failed\n");
		return -1;
	}

	ret = sunxi_codec_init(codec);

	return ret;
}

void sunxi_codec_fill_txfifo(u32 *data)
{
	struct sunxi_codec *codec = &g_codec;

	sunxi_codec_write(codec, SUNXI_DAC_TXDATA, *data);
}

int sunxi_codec_playback_start(ulong handle, u32 *srcBuf, u32 cnt)
{
	int ret = 0;
	struct sunxi_codec *codec = &g_codec;

	flush_cache((unsigned long)srcBuf, cnt);

#if 0
	printf("start dma: form 0x%x to 0x%x  total 0x%x(%u)byte\n",
	       srcBuf, codec->addr_dbase + SUNXI_DAC_TXDATA, cnt, cnt);
#endif
	/*ret = sunxi_dma_start(handle, (uint)srcBuf, codec->addr_dbase + SUNXI_DAC_TXDATA);*/
	ret = sunxi_dma_start(handle, (uint)srcBuf,
			      (uint)(codec->addr_dbase + SUNXI_DAC_TXDATA), cnt);

	return ret;
}
