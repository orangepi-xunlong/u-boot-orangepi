// SPDX-License-Identifier: GPL-2.0+
/*
 *  drivers/dsp/dsp.c
 *
 * Copyright (c) 2020 Allwinner.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program;
 */

#include <asm/arch-sunxi/cpu_ncat.h>
#include <asm/io.h>
#include <common.h>
#include "elf.h"
#include "fdt_support.h"
#include <sys_config.h>
#include <sunxi_image_verifier.h>

#include "dsp_i.h"

#ifdef CONFIG_SUNXI_IMAGE_HEADER
#include <sunxi_image_header.h>
#endif

#include <asm/arch-sunxi/efuse.h>

#define ROUND_DOWN(a, b) ((a) & ~((b)-1))
#define ROUND_UP(a,   b) (((a) + (b)-1) & ~((b)-1))


#define ROUND_DOWN_CACHE(a) ROUND_DOWN(a, CONFIG_SYS_CACHELINE_SIZE)
#define ROUND_UP_CACHE(a)   ROUND_UP(a, CONFIG_SYS_CACHELINE_SIZE)


#define DSP_AHBS_CLK_CONFIG 0x07010000
#define DSP_FREQ_CONFIG_REG DSP_AHBS_CLK_CONFIG

#define AHBS_CLK_SRC_OFFSET          24
#define AHBS_CLK_SRC_DCXO24M         (0 << AHBS_CLK_SRC_OFFSET)
#define AHBS_CLK_SRC_RTC_32K         (1 << AHBS_CLK_SRC_OFFSET)
#define AHBS_CLK_SRC_RC16M           (2 << AHBS_CLK_SRC_OFFSET)
#define AHBS_CLK_SRC_PLL_PERI2X      (3 << AHBS_CLK_SRC_OFFSET)
#define AHBS_CLK_SRC_PLL_AUDIO0_DIV2 (4 << AHBS_CLK_SRC_OFFSET)

#define AHBS_CLK_DIV_RATIO_N_MASK  0x300
#define AHBS_CLK_FACTOR_M_MASK     0x1f

#define AHBS_CLK_DIV_RATIO_OFFSET  8
#define AHBS_CLK_DIV_RATIO_N_1     (0 << AHBS_CLK_DIV_RATIO_OFFSET)
#define AHBS_CLK_DIV_RATIO_N_2     (1 << AHBS_CLK_DIV_RATIO_OFFSET)
#define AHBS_CLK_DIV_RATIO_N_4     (2 << AHBS_CLK_DIV_RATIO_OFFSET)
#define AHBS_CLK_DIV_RATIO_N_8     (3 << AHBS_CLK_DIV_RATIO_OFFSET)

/* x must be 1 - 32 */
#define AHBS_CLK_FACTOR_M(x)      (((x) - 1) << 0)


/*
 * dsp need to remap addresses for some addr.
 */
struct vaddr_range_t {
	unsigned long vstart;
	unsigned long vend;
	unsigned long offset;
};

static struct vaddr_range_t addr_mapping[] = {
	{ 0x18000000, 0x1fffffff, 0x8000000 },
	{ 0x38000000, 0x3fffffff, 0x8000000 },
};

unsigned long set_img_va_to_pa(unsigned long vaddr,
								struct vaddr_range_t *map,
								int size)
{
	unsigned long paddr = vaddr;
	int i;

	for (i = 0; i < size; i++) {
			if (vaddr >= map[i].vstart
					&& vaddr <= map[i].vend) {
					paddr += map[i].offset;
					break;
			}
	}

	return paddr;
}

static int dts_get_dsp_memory(ulong *start, u32 *size, u32 id)
{
	struct fdt_header *dtb_base = working_fdt;
	int nodeoffset;
	int ret;
	u32 reg_data[8];

	if (id == 0) {
		nodeoffset = fdt_path_offset(dtb_base, "/reserved-memory/dsp0");
		if (nodeoffset < 0) {
			pr_err("%s: no /reserved-memory/dsp0 in fdt\n", __func__);
			return -1;
		}
	} else {
		nodeoffset = fdt_path_offset(dtb_base, "/reserved-memory/dsp1");
		if (nodeoffset < 0) {
			pr_err("%s: no /reserved-memory/dsp1 in fdt\n", __func__);
			return -1;
		}
	}

	memset(reg_data, 0, sizeof(reg_data));
	ret = fdt_getprop_u32(dtb_base, nodeoffset, "reg", reg_data);
	if (ret < 0) {
		pr_err("%s: error fdt get reg\n", __func__);
		return -2;
	}

	*start = reg_data[1];
	*size = reg_data[3];
	pr_msg("start = 0x%x size =0x%x\n", (u32)*start, *size);
	return 0;
}

static void sunxi_dsp_set_runstall(u32 dsp_id, u32 value)
{
	u32 reg_val;

	if (dsp_id == 0) { /* DSP0 */
		reg_val = readl(DSP0_CFG_BASE + DSP_CTRL_REG0);
		reg_val &= ~(1 << BIT_RUN_STALL);
		reg_val |= (value << BIT_RUN_STALL);
		writel(reg_val, DSP0_CFG_BASE + DSP_CTRL_REG0);
	} else { /* DSP1 */
		reg_val = readl(DSP1_CFG_BASE + DSP_CTRL_REG0);
		reg_val &= ~(1 << BIT_RUN_STALL);
		reg_val |= (value << BIT_RUN_STALL);
		writel(reg_val, DSP1_CFG_BASE + DSP_CTRL_REG0);
	}
}

static int sun50iw11_dram_set(void *head_addr)
{
	struct fdt_header *dtb_base = working_fdt;
	int nodeoffset;
	u32 *dst_addr = ((void *)head_addr + 0x30);

	nodeoffset = fdt_path_offset(dtb_base, "/dram");
	if (nodeoffset < 0) {
		pr_err("dam error get parameter\n");
		return -1;
	}

	fdt_getprop_u32(dtb_base, nodeoffset, "dram_clk", dst_addr++);
	fdt_getprop_u32(dtb_base, nodeoffset, "dram_type", dst_addr++);
	fdt_getprop_u32(dtb_base, nodeoffset, "dram_zq", dst_addr++);
	fdt_getprop_u32(dtb_base, nodeoffset, "dram_odt_en", dst_addr++);
	fdt_getprop_u32(dtb_base, nodeoffset, "dram_para1", dst_addr++);
	fdt_getprop_u32(dtb_base, nodeoffset, "dram_para2", dst_addr++);
	fdt_getprop_u32(dtb_base, nodeoffset, "dram_mr0", dst_addr++);
	fdt_getprop_u32(dtb_base, nodeoffset, "dram_mr1", dst_addr++);
	fdt_getprop_u32(dtb_base, nodeoffset, "dram_mr2", dst_addr++);
	fdt_getprop_u32(dtb_base, nodeoffset, "dram_mr3", dst_addr++);
	fdt_getprop_u32(dtb_base, nodeoffset, "dram_tpr0", dst_addr++);
	fdt_getprop_u32(dtb_base, nodeoffset, "dram_tpr1", dst_addr++);
	fdt_getprop_u32(dtb_base, nodeoffset, "dram_tpr2", dst_addr++);
	fdt_getprop_u32(dtb_base, nodeoffset, "dram_tpr3", dst_addr++);
	fdt_getprop_u32(dtb_base, nodeoffset, "dram_tpr4", dst_addr++);
	fdt_getprop_u32(dtb_base, nodeoffset, "dram_tpr5", dst_addr++);
	fdt_getprop_u32(dtb_base, nodeoffset, "dram_tpr6", dst_addr++);
	fdt_getprop_u32(dtb_base, nodeoffset, "dram_tpr7", dst_addr++);
	fdt_getprop_u32(dtb_base, nodeoffset, "dram_tpr8", dst_addr++);
	fdt_getprop_u32(dtb_base, nodeoffset, "dram_tpr9", dst_addr++);
	fdt_getprop_u32(dtb_base, nodeoffset, "dram_tpr10", dst_addr++);
	fdt_getprop_u32(dtb_base, nodeoffset, "dram_tpr11", dst_addr++);
	fdt_getprop_u32(dtb_base, nodeoffset, "dram_tpr12", dst_addr++);
	fdt_getprop_u32(dtb_base, nodeoffset, "dram_tpr13", dst_addr++);

	return 0;
}

static int sun50iw11_codec_param_set(struct fdt_header *dtb_base, int nodeoffset,
						struct codec_param *codec_param)
{
	unsigned int temp_val = 0;
	char *string_val = NULL;
	user_gpio_set_t gpio_cfg;

	fdt_getprop_u32(dtb_base, nodeoffset, "mic1gain", &temp_val);
	codec_param->mic_gain[0] = temp_val;
	fdt_getprop_u32(dtb_base, nodeoffset, "mic2gain", &temp_val);
	codec_param->mic_gain[1] = temp_val;
	fdt_getprop_u32(dtb_base, nodeoffset, "mic3gain", &temp_val);
	codec_param->mic_gain[2] = temp_val;
	fdt_getprop_u32(dtb_base, nodeoffset, "mic4gain", &temp_val);
	codec_param->mic_gain[3] = temp_val;
	fdt_getprop_u32(dtb_base, nodeoffset, "mic5gain", &temp_val);
	codec_param->mic_gain[4] = temp_val;
	if (sunxi_efuse_get_soc_ver() == SUN50IW11P1_VERSION_C) {
		fdt_getprop_u32(dtb_base, nodeoffset, "mic6gain", &temp_val);
		codec_param->mic_gain[5] = temp_val;
	}

	fdt_getprop_u32(dtb_base, nodeoffset, "adcdrc_cfg", &temp_val);
	codec_param->adcdrc_cfg = temp_val;
	fdt_getprop_u32(dtb_base, nodeoffset, "adchpf_cfg", &temp_val);
	codec_param->adchpf_cfg = temp_val;
	fdt_getprop_u32(dtb_base, nodeoffset, "dacdrc_cfg", &temp_val);
	codec_param->dacdrc_cfg = temp_val;
	fdt_getprop_u32(dtb_base, nodeoffset, "dachpf_cfg", &temp_val);
	codec_param->dachpf_cfg = temp_val;

	fdt_getprop_u32(dtb_base, nodeoffset, "mic_num", &temp_val);
	codec_param->mic_num = temp_val;

	fdt_getprop_u32(dtb_base, nodeoffset, "pa_msleep_time", &temp_val);
	codec_param->pa_msleep_time = temp_val;
	fdt_getprop_u32(dtb_base, nodeoffset, "capture_cma", &temp_val);
	codec_param->capture_cma = temp_val;
	fdt_getprop_u32(dtb_base, nodeoffset, "playback_cma", &temp_val);
	codec_param->playback_cma = temp_val;
	fdt_getprop_u32(dtb_base, nodeoffset, "spk_vol", &temp_val);
	codec_param->spk_vol = temp_val;

	if (fdt_get_one_gpio_by_offset(nodeoffset,
				"gpio-boost-en", &gpio_cfg) >= 0) {
		codec_param->pa_gpio[0] =
				(gpio_cfg.port - 1) * 32 + gpio_cfg.port_num;
	}

	if (fdt_get_one_gpio_by_offset(nodeoffset, "gpio-spk", &gpio_cfg) >= 0) {
		codec_param->pa_gpio[1] =
				(gpio_cfg.port - 1) * 32 + gpio_cfg.port_num;
	}

	if (fdt_get_one_gpio_by_offset(nodeoffset,
				"gpio-pa-power", &gpio_cfg) >= 0) {
		codec_param->pa_gpio[2] =
				(gpio_cfg.port - 1) * 32 + gpio_cfg.port_num;
	}

	fdt_getprop_u32(dtb_base, nodeoffset, "rx_sync_en", &temp_val);
	codec_param->rx_sync_en = temp_val;

	fdt_getprop_string(dtb_base, nodeoffset, "status", &string_val);
	if (!strcmp("okay", string_val)) {
		codec_param->used = 1;
	} else {
		codec_param->used = 0;
	}

	return 0;
}

static int daudio_param_set(struct fdt_header *dtb_base, int nodeoffset,
						struct daudio_param *daudio_param)
{
	unsigned int temp_val = 0;
	char *string_val = NULL;

	fdt_getprop_u32(dtb_base, nodeoffset, "pcm_lrck_period", &temp_val);
	daudio_param->pcm_lrck_period = temp_val;

	fdt_getprop_u32(dtb_base, nodeoffset, "daudio_master", &temp_val);
	daudio_param->daudio_master = temp_val;
	fdt_getprop_u32(dtb_base, nodeoffset, "audio_format", &temp_val);
	daudio_param->audio_format = temp_val;
	fdt_getprop_u32(dtb_base, nodeoffset, "signal_inversion", &temp_val);
	daudio_param->signal_inversion = temp_val;

	fdt_getprop_u32(dtb_base, nodeoffset, "slot_width_select", &temp_val);
	daudio_param->slot_width_select = temp_val;

	fdt_getprop_u32(dtb_base, nodeoffset, "mclk_div", &temp_val);
	daudio_param->mclk_div = temp_val;
	fdt_getprop_u32(dtb_base, nodeoffset, "sign_extend", &temp_val);
	daudio_param->sign_extend = temp_val;
	fdt_getprop_u32(dtb_base, nodeoffset, "tx_data_mode", &temp_val);
	daudio_param->tx_data_mode = temp_val;
	fdt_getprop_u32(dtb_base, nodeoffset, "rx_data_mode", &temp_val);
	daudio_param->rx_data_mode = temp_val;
	fdt_getprop_u32(dtb_base, nodeoffset, "msb_lsb_first", &temp_val);
	daudio_param->msb_lsb_first = temp_val;
	fdt_getprop_u32(dtb_base, nodeoffset, "tdm_config", &temp_val);
	daudio_param->tdm_config = temp_val;
	fdt_getprop_u32(dtb_base, nodeoffset, "tdm_num", &temp_val);
	daudio_param->tdm_num = temp_val;
	fdt_getprop_u32(dtb_base, nodeoffset, "frame_type", &temp_val);
	daudio_param->frame_type = temp_val;
	fdt_getprop_u32(dtb_base, nodeoffset, "daudio_type", &temp_val);
	daudio_param->daudio_type = temp_val;
	fdt_getprop_u32(dtb_base, nodeoffset, "tx_num", &temp_val);
	daudio_param->tx_num = temp_val;
	fdt_getprop_u32(dtb_base, nodeoffset, "tx_chmap0", &temp_val);
	daudio_param->tx_chmap[0] = temp_val;
	fdt_getprop_u32(dtb_base, nodeoffset, "tx_chmap1", &temp_val);
	daudio_param->tx_chmap[1] = temp_val;
	fdt_getprop_u32(dtb_base, nodeoffset, "rx_num", &temp_val);
	daudio_param->rx_num = temp_val;
	fdt_getprop_u32(dtb_base, nodeoffset, "rx_chmap0", &temp_val);
	daudio_param->rx_chmap[0] = temp_val;
	fdt_getprop_u32(dtb_base, nodeoffset, "rx_chmap1", &temp_val);
	daudio_param->rx_chmap[1] = temp_val;
	fdt_getprop_u32(dtb_base, nodeoffset, "rx_chmap2", &temp_val);
	daudio_param->rx_chmap[2] = temp_val;
	fdt_getprop_u32(dtb_base, nodeoffset, "rx_chmap3", &temp_val);
	daudio_param->rx_chmap[3] = temp_val;
	fdt_getprop_u32(dtb_base, nodeoffset, "capture_cma", &temp_val);
	daudio_param->capture_cma = temp_val;
	fdt_getprop_u32(dtb_base, nodeoffset, "playback_cma", &temp_val);
	daudio_param->playback_cma = temp_val;
	fdt_getprop_u32(dtb_base, nodeoffset, "rx_sync_en", &temp_val);
	daudio_param->rx_sync_en = temp_val;

	fdt_getprop_string(dtb_base, nodeoffset, "status", &string_val);
	if (!strcmp("okay", string_val)) {
		daudio_param->used = 1;
	} else {
		daudio_param->used = 0;
		temp_val = daudio_param->used;
		fdt_getprop_u32(dtb_base, nodeoffset, "daudio_used", &temp_val);
		daudio_param->used = temp_val;
	}

	return 0;
}

static int dmic_param_set(struct fdt_header *dtb_base, int nodeoffset,
						struct dmic_param *dmic_param)
{
	unsigned int temp_val = 0;
	char *string_val = NULL;

	fdt_getprop_u32(dtb_base, nodeoffset, "rx_chmap", &temp_val);
	dmic_param->rx_chmap = temp_val;

	fdt_getprop_u32(dtb_base, nodeoffset, "data_vol", &temp_val);
	dmic_param->data_vol = temp_val;

	fdt_getprop_u32(dtb_base, nodeoffset, "capture_cma", &temp_val);
	dmic_param->capture_cma = temp_val;

	fdt_getprop_u32(dtb_base, nodeoffset, "rx_sync_en", &temp_val);
	dmic_param->rx_sync_en = temp_val;

	fdt_getprop_string(dtb_base, nodeoffset, "status", &string_val);
	if (!strcmp("okay", string_val)) {
		dmic_param->used = 1;
	} else {
		dmic_param->used = 0;
	}

	return 0;
}

static int mad_param_set(struct fdt_header *dtb_base, int nodeoffset,
						struct mad_param *mad_param)
{
	unsigned int temp_val = 0;

	fdt_getprop_u32(dtb_base, nodeoffset, "lpsd_th", &temp_val);
	mad_param->lpsd_th = temp_val;

	fdt_getprop_u32(dtb_base, nodeoffset, "lpsd_rrun", &temp_val);
	mad_param->lpsd_rrun = temp_val;

	fdt_getprop_u32(dtb_base, nodeoffset, "lpsd_rstop", &temp_val);
	mad_param->lpsd_rstop = temp_val;

	fdt_getprop_u32(dtb_base, nodeoffset, "lpsd_ecnt", &temp_val);
	mad_param->lpsd_ecnt = temp_val;

	return 0;
}

static int sun50iw11_audio_set(void *head_addr)
{
	struct fdt_header *dtb_base = working_fdt;
	int nodeoffset;
	struct audio_param *audio_param = ((void *)head_addr + 0x30 + sizeof(struct dram_para_32));

	/* set for audiocodec */
	nodeoffset = fdt_path_offset(dtb_base, "/soc/codec");
	if (nodeoffset < 0) {
		pr_err("r_codec error get parameter\n");
	} else {
		sun50iw11_codec_param_set(dtb_base, nodeoffset,
							&audio_param->codec_param);
	}

	/* set for snddaudio0 */
	nodeoffset = fdt_path_offset(dtb_base, "/soc/daudio@0x07033000");
	if (nodeoffset < 0) {
		pr_err("r_daudio0 error get parameter\n");
	} else {
		daudio_param_set(dtb_base, nodeoffset, &audio_param->daudio_param[0]);
	}

	/* set for snddaudio1 */
	nodeoffset = fdt_path_offset(dtb_base, "/soc/daudio@0x07034000");
	if (nodeoffset < 0) {
		pr_err("r_daudio1 error get parameter\n");
	} else {
		daudio_param_set(dtb_base, nodeoffset, &audio_param->daudio_param[1]);
	}

	/* set for snddmic */
	nodeoffset = fdt_path_offset(dtb_base, "/soc/dmic-controller");
	if (nodeoffset < 0) {
		pr_err("r_dmic error get parameter\n");
	} else
		dmic_param_set(dtb_base, nodeoffset, &audio_param->dmic_param);

	/* set for mad */
	nodeoffset = fdt_path_offset(dtb_base, "/soc/mad@0x07097000");
	if (nodeoffset < 0) {
		pr_err("mad error get parameter\n");
	} else
		mad_param_set(dtb_base, nodeoffset, &audio_param->mad_param);

	return 0;
}

#define fdt_standby_phase(_name) do {\
	err = fdt_getprop_u32(dtb_base, nodeoffset, _name, &temp_val); \
	if (err < 0) { \
		pr_err("phase dts node(%s) failed. ret: %d, val:0x%08x\n", \
		_name, err, temp_val); \
	} else { \
		pr_info("phase %s: 0x%08x\n", _name, temp_val); \
	} } while (0);

static int sun50iw11_standby_set(void *head_addr)
{
	struct fdt_header *dtb_base = working_fdt;
	int nodeoffset, err = -1;
	struct standby_param *standby_param = ((void *)head_addr + 0x30 + \
			sizeof(struct dram_para_32) + sizeof(struct audio_param));

	unsigned int temp_val = 0;

	/* find standby node */
	nodeoffset = fdt_path_offset(dtb_base, "/soc/standby_param");
	if (nodeoffset < 0) {
		pr_err("standby_param error get parameter\n");
	}

	/* set power for standby */
	fdt_standby_phase("vdd-cpu");
	standby_param->power_param.vdd_cpu = temp_val;
	fdt_standby_phase("vdd-sys");
	standby_param->power_param.vdd_sys = temp_val;
	fdt_standby_phase("vcc-pll");
	standby_param->power_param.vcc_pll = temp_val;

	/* set power for standby */
	fdt_standby_phase("osc24m-on");
	standby_param->clk_param.osc24m_on = !!temp_val;
	fdt_standby_phase("pllcpu-off");
	standby_param->clk_param.pllcpu_off = !!temp_val;
	fdt_standby_phase("pllperiph0-off");
	standby_param->clk_param.pllperiph0_off = !!temp_val;
	fdt_standby_phase("pllaudio0-off");
	standby_param->clk_param.pllaudio0_off = !!temp_val;
	fdt_standby_phase("pllaudio1-off");
	standby_param->clk_param.pllaudio1_off = !!temp_val;
	fdt_standby_phase("ahb1ahb2-to-32k");
	standby_param->clk_param.ahb1ahb2_to_32k = !!temp_val;
	fdt_standby_phase("apb1-to-32k");
	standby_param->clk_param.apb1_to_32k = !!temp_val;
	fdt_standby_phase("apb2-to-32k");
	standby_param->clk_param.apb2_to_32k = !!temp_val;
	fdt_standby_phase("axi-to-32k");
	standby_param->clk_param.axi_to_32k = !!temp_val;
	fdt_standby_phase("apbs0-to-32k");
	standby_param->clk_param.apbs0_to_32k = !!temp_val;
	fdt_standby_phase("apbs1-to-32k");
	standby_param->clk_param.apbs1_to_32k = !!temp_val;

	/* set func for standby */
	fdt_standby_phase("uart-off");
	standby_param->func_param.uart_off = !!temp_val;

	/* set standby */
	fdt_standby_phase("nmi-wakeup");
	standby_param->nmi_wakeup = !!temp_val;
	fdt_standby_phase("sleep-freq");
	standby_param->sleep_freq = temp_val;

	standby_param->flag_isupdate = 0;

	return 0;
}


static int sun50iw11_head_set(u32 *head_addr)
{
	sun50iw11_dram_set(head_addr);
	sun50iw11_audio_set(head_addr);
	sun50iw11_standby_set(head_addr);

	return 0;
}

static int sun50iw11_print_version(void *head_addr)
{
	/* we promise 896 offset is verson string */
	char *pstr = head_addr + 896;

	/* max string is 100 */
	pstr[100] = 0;
	printf("DSP VERSION IS %s\n", pstr);

	return 0;
}

static int load_image_old(u32 img_addr, u32 *run_addr)
{
	Elf32_Ehdr *ehdr; /* Elf header structure pointer */
	Elf32_Phdr *phdr; /* Program header structure pointer */
	int i;

	ehdr = (Elf32_Ehdr *)img_addr;
	phdr = (Elf32_Phdr *)(img_addr + ehdr->e_phoff);

	/* Load each program header */
	for (i = 0; i < ehdr->e_phnum; ++i) {
		void *dst = (void *)(uintptr_t)phdr->p_paddr;
		void *src = (void *)img_addr + phdr->p_offset;

		debug("Loading phdr %i to 0x%p (%i bytes)\n",
		      i, dst, phdr->p_filesz);
		if (phdr->p_filesz)
			memcpy(dst, src, phdr->p_filesz);
		if (phdr->p_filesz != phdr->p_memsz)
			memset(dst + phdr->p_filesz, 0x00,
			       phdr->p_memsz - phdr->p_filesz);
#ifdef CONFIG_MACH_SUN50IW11
		/* 50iw11 have header,we set it and simple check it */
		if (phdr->p_memsz <= 0x400 && i == 0) {
			sun50iw11_head_set(dst);
			sun50iw11_print_version(dst);
		}
#endif /* CONFIG_MACH_SUN50IW11 */
		flush_cache(ROUND_DOWN_CACHE((unsigned long)dst),
			    ROUND_UP_CACHE(phdr->p_filesz));
		++phdr;
	}
	if (!*run_addr)
		*run_addr = ehdr->e_entry;

	return 0;
}

static int load_image(u32 img_addr, u32 dsp_id)
{
	Elf32_Ehdr *ehdr; /* Elf header structure pointer */
	Elf32_Phdr *phdr; /* Program header structure pointer */
	int i;
	void *dst = NULL;
	void *src = NULL;
	int size = sizeof(addr_mapping) / sizeof(struct vaddr_range_t);
	ulong mem_start = 0;
	u32 mem_size = 0;

	ehdr = (Elf32_Ehdr *)img_addr;
	phdr = (Elf32_Phdr *)(img_addr + ehdr->e_phoff);

	/* Load each program header */
	for (i = 0; i < ehdr->e_phnum; ++i) {

		//remap addresses
		dst = (void *)set_img_va_to_pa((uintptr_t)phdr->p_paddr, \
					addr_mapping, \
					size);

		src = (void *)img_addr + phdr->p_offset;

		debug("Loading phdr %i to 0x%p (%i bytes)\n",
		      i, dst, phdr->p_filesz);
		if (phdr->p_filesz)
			memcpy(dst, src, phdr->p_filesz);
		if (phdr->p_filesz != phdr->p_memsz)
			memset(dst + phdr->p_filesz, 0x00,
			       phdr->p_memsz - phdr->p_filesz);

		/* 50iw11 have header,we set it and simple check it */
		if (phdr->p_memsz <= 0x400 && i == 0) {
			sun50iw11_head_set(dst);
			sun50iw11_print_version(dst);
		}

		//flush_cache(ROUND_DOWN_CACHE((unsigned long)dst),
		//	    ROUND_UP_CACHE(phdr->p_filesz));
		++phdr;
	}

	dts_get_dsp_memory(&mem_start, &mem_size, dsp_id);
	if (!mem_start || !mem_size) {
		pr_err("dts_get_dsp_memory fail\n");
	} else {
		flush_cache(ROUND_DOWN_CACHE(mem_start),
		ROUND_UP_CACHE(mem_size));
	}

	return 0;
}

static int get_image_len(u32 img_addr)
{
	Elf32_Ehdr *ehdr = NULL; /* Elf header structure pointer */
	Elf32_Phdr *phdr = NULL; /* Program header structure pointer */
	Elf32_Shdr *shdr = NULL;

	ehdr = (Elf32_Ehdr *)img_addr;
	phdr = (Elf32_Phdr *)(img_addr + ehdr->e_phoff);
	shdr =  (Elf32_Shdr *)(img_addr + ehdr->e_shoff
			+ (ehdr->e_shstrndx * sizeof(Elf32_Shdr)));

	int i = 0;
	unsigned long change_addr = 0;
	unsigned char *strtab = NULL;

	if (shdr->sh_type == SHT_STRTAB)
		strtab = (unsigned char *)(img_addr + shdr->sh_offset);

	for (i = 0; i < ehdr->e_shnum; ++i) {
		shdr = (Elf32_Shdr *)(img_addr + ehdr->e_shoff
				+ (i * sizeof(Elf32_Shdr)));

		if (!(shdr->sh_flags & SHF_ALLOC)
			|| (shdr->sh_addr == 0)
			|| (shdr->sh_size == 0)) {

			continue;
		}

		if (strtab) {

			char *pstr = (char *)(&strtab[shdr->sh_name]);

			if (strcmp(pstr, ".oemhead.text") == 0) {
				printf("find dsp section: %s ,addr = 0x%lx \n",
					pstr, (unsigned long)shdr->sh_addr);
				change_addr = shdr->sh_addr;
				break;
			}
		}
	}

	int img_len = 0;
	struct spare_rtos_head_t *prtos = NULL;

	for (i = 0; i < ehdr->e_phnum; ++i) {
		if (change_addr == (unsigned long)phdr->p_paddr) {
			prtos = (struct spare_rtos_head_t *)(img_addr
					+ phdr->p_offset);
			img_len = prtos->rtos_img_hdr.image_size;
			break;
		}
		++phdr;
	}

	return img_len;
}

static void dsp_freq_default_set(void)
{
	u32 reg = DSP_FREQ_CONFIG_REG;
	u32 val = 0;

	val = AHBS_CLK_SRC_PLL_PERI2X | AHBS_CLK_DIV_RATIO_N_1 |
	      AHBS_CLK_FACTOR_M(3);
	writel(val, reg);

}

static void sram_remap_set(int value)
{
	u32 val = 0;

	val = readl(SUNXI_PRCM_BASE + PRCM_DSP0_LOCALRAM_REMAP_REG);
	val &= ~(1 << BIT_DSP0_LOCALRAM);
	val |= (value << BIT_DSP0_LOCALRAM);
	writel(val, SUNXI_PRCM_BASE + PRCM_DSP0_LOCALRAM_REMAP_REG);
}

static void ahbs_clk_set(void)
{
	u32 reg = DSP_FREQ_CONFIG_REG;
	u32 val = 0;

	val = AHBS_CLK_SRC_PLL_PERI2X | AHBS_CLK_DIV_RATIO_N_1 |
	      AHBS_CLK_FACTOR_M(6);
	writel(val, reg);

}

static int update_reset_vec(u32 img_addr, u32 *run_addr)
{
	Elf32_Ehdr *ehdr; /* Elf header structure pointer */

	ehdr = (Elf32_Ehdr *)img_addr;
	if (!*run_addr)
		*run_addr = ehdr->e_entry;

	return 0;
}

int sunxi_dsp_init_old(u32 img_addr, u32 run_ddr, u32 dsp_id, u32 image_len)
{
	u32 reg_val;

	load_image_old(img_addr, &run_ddr);

#ifdef CONFIG_MACH_SUN50IW11
	dsp_freq_default_set();
#endif /* CONFIG_MACH_SUN50IW11 */

	printf("DSP%d booting from 0x%x...\n", dsp_id, run_ddr);

	if (dsp_id == 0) { /* DSP0 */
		/* clock gating */
		reg_val = readl(SUNXI_PRCM_BASE + PRCM_DSP_BGR_REG);
		reg_val |= (1 << BIT_DSP0_GATING);
		reg_val |= (1 << BIT_DSP0_CFG_GATING);
		writel(reg_val, SUNXI_PRCM_BASE + PRCM_DSP_BGR_REG);

		/* reset */
		reg_val = readl(SUNXI_PRCM_BASE + PRCM_DSP_BGR_REG);
		reg_val |= (1 << BIT_DSP0_CFG_RST);
		reg_val |= (1 << BIT_DSP0_DBG_RST);
		writel(reg_val, SUNXI_PRCM_BASE + PRCM_DSP_BGR_REG);

		/* set external Reset Vector if needed */
		if (run_ddr != DSP_DEFAULT_RST_VEC) {
			writel(run_ddr, DSP0_CFG_BASE + DSP_ALT_RESET_VEC_REG);

			reg_val = readl(DSP0_CFG_BASE + DSP_CTRL_REG0);
			reg_val |= (1 << BIT_START_VEC_SEL);
			writel(reg_val, DSP0_CFG_BASE + DSP_CTRL_REG0);
		}

		/* set runstall */
		sunxi_dsp_set_runstall(dsp_id, 1);

		/* de-assert dsp0 */
		reg_val = readl(SUNXI_PRCM_BASE + PRCM_DSP_BGR_REG);
		reg_val |= (1 << BIT_DSP0_RST);
		writel(reg_val, SUNXI_PRCM_BASE + PRCM_DSP_BGR_REG);

		/* clear runstall */
		sunxi_dsp_set_runstall(dsp_id, 0);
	} else { /* DSP1 */
		/* clock gating */
		reg_val = readl(SUNXI_PRCM_BASE + PRCM_DSP_BGR_REG);
		reg_val |= (1 << BIT_DSP1_GATING);
		reg_val |= (1 << BIT_DSP1_CFG_GATING);
		writel(reg_val, SUNXI_PRCM_BASE + PRCM_DSP_BGR_REG);

		/* reset */
		reg_val = readl(SUNXI_PRCM_BASE + PRCM_DSP_BGR_REG);
		reg_val |= (1 << BIT_DSP1_CFG_RST);
		reg_val |= (1 << BIT_DSP1_DBG_RST);
		writel(reg_val, SUNXI_PRCM_BASE + PRCM_DSP_BGR_REG);

		/* set external Reset Vector if needed */
		if (run_ddr != DSP_DEFAULT_RST_VEC) {
			writel(run_ddr, DSP1_CFG_BASE + DSP_ALT_RESET_VEC_REG);

			reg_val = readl(DSP1_CFG_BASE + DSP_CTRL_REG0);
			reg_val |= (1 << BIT_START_VEC_SEL);
			writel(reg_val, DSP1_CFG_BASE + DSP_CTRL_REG0);
		}

		/* set runstall */
		sunxi_dsp_set_runstall(dsp_id, 1);

		/* de-assert dsp1 */
		reg_val = readl(SUNXI_PRCM_BASE + PRCM_DSP_BGR_REG);
		reg_val |= (1 << BIT_DSP1_RST);
		writel(reg_val, SUNXI_PRCM_BASE + PRCM_DSP_BGR_REG);

		/* clear runstall */
		sunxi_dsp_set_runstall(dsp_id, 0);
	}

	return 0;
}

int sunxi_dsp_init(u32 img_addr, u32 run_ddr, u32 dsp_id)
{
	u32 reg_val = 0;
	int image_len = 0;

#ifdef CONFIG_SUNXI_IMAGE_HEADER
	sunxi_image_header_t *ih = (sunxi_image_header_t *)img_addr;

	image_len = get_image_len((u32)((uint8_t *)ih + ih->ih_hsize));
#else
	image_len = get_image_len(img_addr);
#endif


#if defined(CONFIG_SUNXI_VERIFY_DSP) && defined(CONFIG_SUNXI_IMAGE_VERIFIER)
	// for IMAGE_HEADER, copy payload to img_addr
	if (sunxi_verify_dsp(img_addr, image_len, dsp_id) < 0) {
		return -1;
	}
#else
	pr_msg("no verify dsp\n");
#endif

	if (sunxi_efuse_get_soc_ver() != SUN50IW11P1_VERSION_C) {
		if (sunxi_dsp_init_old(img_addr, run_ddr, dsp_id, image_len) < 0) {
			return -1;
		}

		return 0;
	}

	/* update run addr */
	update_reset_vec(img_addr, &run_ddr);

	/* set ahbs clk */
	ahbs_clk_set();

	if (dsp_id == 0) { /* DSP0 */

		/* set uboot use local ram */
		sram_remap_set(1);

		/* set dsp clk value 600M
		reg_val = readl(SUNXI_PRCM_BASE + PRCM_DSP0_CLK_CFG_REG);
		reg_val |= (1 << BIT_DSP0_SCLK_GATING);
		reg_val |= (3 << BIT_DSP0_CLK_SRC_SEL);
		reg_val |= (0 << BIT_DSP0_CLK_DIV_RATIO_N);
		reg_val |= (1 << BIT_DSP0_FACTOR_M);
		writel(reg_val, SUNXI_PRCM_BASE + PRCM_DSP0_CLK_CFG_REG);
		*/

		/* set dsp clk value 516M */
		reg_val = readl(SUNXI_PRCM_BASE + PRCM_DSP0_CLK_CFG_REG);
		reg_val |= (1 << BIT_DSP0_SCLK_GATING);
		reg_val |= (4 << BIT_DSP0_CLK_SRC_SEL);
		reg_val |= (0 << BIT_DSP0_CLK_DIV_RATIO_N);
		reg_val |= (0 << BIT_DSP0_FACTOR_M);
		writel(reg_val, SUNXI_PRCM_BASE + PRCM_DSP0_CLK_CFG_REG);


		/* clock gating */
		reg_val = readl(SUNXI_PRCM_BASE + PRCM_DSP_BGR_REG);
		reg_val |= (1 << BIT_DSP0_GATING);
		reg_val |= (1 << BIT_DSP0_CFG_GATING);
		writel(reg_val, SUNXI_PRCM_BASE + PRCM_DSP_BGR_REG);

		/* reset */
		reg_val = readl(SUNXI_PRCM_BASE + PRCM_DSP_BGR_REG);
		reg_val |= (1 << BIT_DSP0_CFG_RST);
		reg_val |= (1 << BIT_DSP0_DBG_RST);
		writel(reg_val, SUNXI_PRCM_BASE + PRCM_DSP_BGR_REG);

		/* load image*/
		load_image(img_addr, dsp_id);

		/* set dsp vector*/
		writel(run_ddr, DSP0_CFG_BASE + DSP_ALT_RESET_VEC_REG);
		reg_val = readl(DSP0_CFG_BASE + DSP_CTRL_REG0);
		reg_val |= (1 << BIT_START_VEC_SEL);
		writel(reg_val, DSP0_CFG_BASE + DSP_CTRL_REG0);

		/* set runstall */
		sunxi_dsp_set_runstall(dsp_id, 1);

		/* set dsp use local ram */
		sram_remap_set(0);

		/* de-assert dsp */
		reg_val = readl(SUNXI_PRCM_BASE + PRCM_DSP_BGR_REG);
		reg_val |= (1 << BIT_DSP0_RST);
		writel(reg_val, SUNXI_PRCM_BASE + PRCM_DSP_BGR_REG);

		/* clear runstall */
		sunxi_dsp_set_runstall(dsp_id, 0);


	} else { /* DSP1 */

		/* set dsp clk value 400M*/
		reg_val = readl(SUNXI_PRCM_BASE + PRCM_DSP1_CLK_CFG_REG);
		reg_val |= (1 << BIT_DSP1_SCLK_GATING);
		reg_val |= (3 << BIT_DSP1_CLK_SRC_SEL);
		reg_val |= (0 << BIT_DSP1_CLK_DIV_RATIO_N);
		reg_val |= (2 << BIT_DSP1_FACTOR_M);
		writel(reg_val, SUNXI_PRCM_BASE + PRCM_DSP1_CLK_CFG_REG);

		/* clock gating */
		reg_val = readl(SUNXI_PRCM_BASE + PRCM_DSP_BGR_REG);
		reg_val |= (1 << BIT_DSP1_GATING);
		reg_val |= (1 << BIT_DSP1_CFG_GATING);
		writel(reg_val, SUNXI_PRCM_BASE + PRCM_DSP_BGR_REG);

		/* reset */
		reg_val = readl(SUNXI_PRCM_BASE + PRCM_DSP_BGR_REG);
		reg_val |= (1 << BIT_DSP1_CFG_RST);
		reg_val |= (1 << BIT_DSP1_DBG_RST);
		writel(reg_val, SUNXI_PRCM_BASE + PRCM_DSP_BGR_REG);

		/* load image*/
		load_image(img_addr, dsp_id);

		/* set dsp vector*/
		writel(run_ddr, DSP1_CFG_BASE + DSP_ALT_RESET_VEC_REG);
		reg_val = readl(DSP1_CFG_BASE + DSP_CTRL_REG0);
		reg_val |= (1 << BIT_START_VEC_SEL);
		writel(reg_val, DSP1_CFG_BASE + DSP_CTRL_REG0);

		/* set runstall */
		sunxi_dsp_set_runstall(dsp_id, 1);

		/* de-assert dsp */
		reg_val = readl(SUNXI_PRCM_BASE + PRCM_DSP_BGR_REG);
		reg_val |= (1 << BIT_DSP1_RST);
		writel(reg_val, SUNXI_PRCM_BASE + PRCM_DSP_BGR_REG);

		/* clear runstall */
		sunxi_dsp_set_runstall(dsp_id, 0);

	}

	printf("DSP%d start ok, img length %d, booting from 0x%x\n",
		dsp_id, image_len, run_ddr);

	return 0;
}
