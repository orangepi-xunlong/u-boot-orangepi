// SPDX-License-Identifier: GPL-2.0+
/*
 * drivers/dsp/sun8iw20/dsp.c
 *
 * Copyright (c) 2007-2025 Allwinnertech Co., Ltd.
 * Author: wujiayi <wujiayi@allwinnertech.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details
 *
 */

#include <asm/arch-sunxi/cpu_ncat_v2.h>
#include <asm/io.h>
#include <common.h>
#include <sys_config.h>
#include <sunxi_image_verifier.h>

#include "platform.h"
#include "elf.h"
#include "fdt_support.h"
#include "dsp_reg.h"
#include "../common/dsp_fdt.h"
#include "../common/dsp_img.h"
#include "../common/dsp_ic.h"

#define readl_dsp(addr)		readl((const volatile void*)(addr))
#define writel_dsp(val, addr)	writel((u32)(val), (volatile void*)(addr))

#define ROUND_DOWN(a, b) ((a) & ~((b)-1))
#define ROUND_UP(a,   b) (((a) + (b)-1) & ~((b)-1))

#define ROUND_DOWN_CACHE(a) ROUND_DOWN(a, CONFIG_SYS_CACHELINE_SIZE)
#define ROUND_UP_CACHE(a)   ROUND_UP(a, CONFIG_SYS_CACHELINE_SIZE)

/*
 * dsp need to remap addresses for some addr.
 */
static struct vaddr_range_t addr_mapping[] = {
	{ 0x10000000, 0x1fffffff, 0x30000000 },
	{ 0x30000000, 0x3fffffff, 0x10000000 },
};

static void sunxi_dsp_set_runstall(u32 dsp_id, u32 value)
{
	u32 reg_val;

	if (dsp_id == 0) { /* DSP0 */
		reg_val = readl_dsp(DSP0_CFG_BASE + DSP_CTRL_REG0);
		reg_val &= ~(1 << BIT_RUN_STALL);
		reg_val |= (value << BIT_RUN_STALL);
		writel_dsp(reg_val, (DSP0_CFG_BASE + DSP_CTRL_REG0));
	}
}

static int update_reset_vec(u32 img_addr, u32 *run_addr)
{
	Elf32_Ehdr *ehdr; /* Elf header structure pointer */

	ehdr = (Elf32_Ehdr *)(ADDR_TPYE)img_addr;
	if (!*run_addr)
		*run_addr = ehdr->e_entry;

	return 0;
}

static int load_image(u32 img_addr, u32 dsp_id)
{
	Elf32_Ehdr *ehdr; /* Elf header structure pointer */
	Elf32_Phdr *phdr; /* Program header structure pointer */
	void *dst = NULL;
	void *src = NULL;
	int i = 0;
	int size = sizeof(addr_mapping) / sizeof(struct vaddr_range_t);
	ulong mem_start = 0;
	u32 mem_size = 0;

	ehdr = (Elf32_Ehdr *)(ADDR_TPYE)img_addr;
	phdr = (Elf32_Phdr *)(ADDR_TPYE)(img_addr + ehdr->e_phoff);

	/* Load each program header */
	for (i = 0; i < ehdr->e_phnum; ++i) {

		//remap addresses
		dst = (void *)(ADDR_TPYE)set_img_va_to_pa((unsigned long)phdr->p_paddr, \
					addr_mapping, \
					size);

		src = (void *)(ADDR_TPYE)img_addr + phdr->p_offset;
		DSP_DEBUG("Loading phdr %i from 0x%x to 0x%p (%i bytes)\n",
		      i, phdr->p_paddr, dst, phdr->p_filesz);

		if (phdr->p_filesz)
			memcpy(dst, src, phdr->p_filesz);
		if (phdr->p_filesz != phdr->p_memsz)
			memset(dst + phdr->p_filesz, 0x00,
			       phdr->p_memsz - phdr->p_filesz);
		if (i == 0)
			show_img_version((char *)dst + 896, dsp_id);

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


static void dsp_freq_default_set(void)
{
	u32 val = 0;

	val = DSP_CLK_SRC_PERI2X | DSP_CLK_FACTOR_M(2)
		| (1 << BIT_DSP_SCLK_GATING);
	writel_dsp(val, (SUNXI_CCM_BASE + CCMU_DSP_CLK_REG));
}

static void sram_remap_set(int value)
{
	u32 val = 0;

	val = readl_dsp(SUNXI_SRAMC_BASE + SRAMC_SRAM_REMAP_REG);
	val &= ~(1 << BIT_SRAM_REMAP_ENABLE);
	val |= (value << BIT_SRAM_REMAP_ENABLE);
	writel_dsp(val, SUNXI_SRAMC_BASE + SRAMC_SRAM_REMAP_REG);
}

int sunxi_dsp_init(u32 img_addr, u32 run_ddr, u32 dsp_id)
{
	u32 reg_val;
	u32 image_len = 0;
	struct dts_msg_t dts_msg;
	unsigned long section_addr = 0;
	char *str = ".oemhead.text";
	int ret = 0;

#ifdef CONFIG_SUNXI_VERIFY_DSP
	if (sunxi_verify_dsp(img_addr, image_len, dsp_id) < 0) {
		return -1;
	}
#endif

	/* clear dts msg data */
	memset((void *)&dts_msg, 0, sizeof(struct dts_msg_t));

	/* get dsp status */
	ret = dts_dsp_status(&dts_msg, dsp_id);
	if (ret < 0) {
		printf("dsp%d:dsp close in dts\n", dsp_id);
		return 0;
	}

	/* get dts msg about dsp */
	ret = dts_uart_msg(&dts_msg, dsp_id);
	if (ret < 0)
		printf("dsp%d:uart config fail\n", dsp_id);

	/* get gpio interrput about dsp */
	ret = dts_gpio_int_msg(&dts_msg, dsp_id);
	if (ret < 0)
		printf("dsp%d:gpio init config fail\n", dsp_id);

	ret = dts_sharespace_msg(&dts_msg, dsp_id);
	if (ret < 0)
		printf("dsp%d:sharespace config fail\n", dsp_id);

	/* set uboot use local ram */
	sram_remap_set(1);

	/* update run addr */
	update_reset_vec(img_addr, &run_ddr);

	/* find section */
	ret = find_img_section(img_addr, str, &section_addr);
	if (ret < 0) {
		printf("dsp%d:find section err\n", dsp_id);
		return -1;
	}

	/* get img len */
	ret = get_img_len(img_addr, section_addr, &image_len);
	if (ret < 0) {
		printf("dsp%d:get img len err\n", dsp_id);
		return -1;
	}

	/* set img dts */
	ret = set_msg_dts(img_addr, section_addr, &dts_msg);
	if (ret < 0) {
		printf("dsp%d:set img dts err\n", dsp_id);
		return -1;
	}

	dsp_freq_default_set();

	if (dsp_id == 0) { /* DSP0 */
		/* clock gating */
		reg_val = readl_dsp(SUNXI_CCM_BASE + CCMU_DSP_BGR_REG);
		reg_val |= (1 << BIT_DSP0_CFG_GATING);
		writel_dsp(reg_val, SUNXI_CCM_BASE + CCMU_DSP_BGR_REG);

		/* reset */
		reg_val = readl_dsp(SUNXI_CCM_BASE + CCMU_DSP_BGR_REG);
		reg_val |= (1 << BIT_DSP0_CFG_RST);
		reg_val |= (1 << BIT_DSP0_DBG_RST);
		writel_dsp(reg_val, SUNXI_CCM_BASE + CCMU_DSP_BGR_REG);

		/* set external Reset Vector if needed */
		if (run_ddr != DSP_DEFAULT_RST_VEC) {
			writel_dsp(run_ddr, DSP0_CFG_BASE +
					DSP_ALT_RESET_VEC_REG);

			reg_val = readl_dsp(DSP0_CFG_BASE + DSP_CTRL_REG0);
			reg_val |= (1 << BIT_START_VEC_SEL);
			writel_dsp(reg_val, DSP0_CFG_BASE + DSP_CTRL_REG0);
		}

		/* set runstall */
		sunxi_dsp_set_runstall(dsp_id, 1);

		/* set dsp clken */
		reg_val = readl_dsp(DSP0_CFG_BASE + DSP_CTRL_REG0);
		reg_val |= (1 << BIT_DSP_CLKEN);
		writel_dsp(reg_val, DSP0_CFG_BASE + DSP_CTRL_REG0);

		/* de-assert dsp0 */
		reg_val = readl_dsp(SUNXI_CCM_BASE + CCMU_DSP_BGR_REG);
		reg_val |= (1 << BIT_DSP0_RST);
		writel_dsp(reg_val, SUNXI_CCM_BASE + CCMU_DSP_BGR_REG);

		/* load image to ram */
		load_image(img_addr, dsp_id);

		/* set dsp use local ram */
		sram_remap_set(0);

		/* clear runstall */
		sunxi_dsp_set_runstall(dsp_id, 0);
	}
	printf("DSP%d start ok, img length %d, booting from 0x%x\n",
			dsp_id, image_len, run_ddr);
	return 0;
}
