// SPDX-License-Identifier: GPL-2.0+
/*
 * drivers/riscv/sun8iw21/riscv.c
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
#include "riscv_reg.h"
#include "../common/riscv_fdt.h"
#include "../common/riscv_img.h"
#include "../common/riscv_ic.h"

#define readl_riscv(addr)	readl((const volatile void*)(addr))
#define writel_riscv(val, addr)	writel((u32)(val), (volatile void*)(addr))

#define ROUND_DOWN(a, b)	((a) & ~((b)-1))
#define ROUND_UP(a, b)		(((a) + (b)-1) & ~((b)-1))

#define ROUND_DOWN_CACHE(a) ROUND_DOWN(a, CONFIG_SYS_CACHELINE_SIZE)
#define ROUND_UP_CACHE(a)   ROUND_UP(a, CONFIG_SYS_CACHELINE_SIZE)

/*
 * riscv need to remap addresses for some addr.
 */
static struct vaddr_range_t addr_mapping[] = {
	{ 0x10000000, 0x1fffffff, 0x30000000 },
	{ 0x30000000, 0x3fffffff, 0x10000000 },
};

static int update_reset_vec(u32 img_addr, u32 *run_addr)
{
	Elf32_Ehdr *ehdr; /* Elf header structure pointer */

	ehdr = (Elf32_Ehdr *)(ADDR_TPYE)img_addr;
	if (!*run_addr)
		*run_addr = ehdr->e_entry;

	return 0;
}

static int load_image(u32 img_addr, u32 riscv_id)
{
	Elf32_Ehdr *ehdr; /* Elf header structure pointer */
	Elf32_Phdr *phdr; /* Program header structure pointer */
	void *dst = NULL;
	void *src = NULL;
	int i = 0;
	int size = sizeof(addr_mapping) / sizeof(struct vaddr_range_t);
	ehdr = (Elf32_Ehdr *)(ADDR_TPYE)img_addr;
	phdr = (Elf32_Phdr *)(ADDR_TPYE)(img_addr + ehdr->e_phoff);

	/* Load each program header */
	for (i = 0; i < ehdr->e_phnum; ++i) {

		//remap addresses
		dst = (void *)(ADDR_TPYE)set_img_va_to_pa((unsigned long)phdr->p_paddr, \
					addr_mapping, \
					size);

		src = (void *)(ADDR_TPYE)img_addr + phdr->p_offset;
		RISCV_DEBUG("Loading phdr %i from 0x%p to 0x%p (%i bytes)\n",
		      i, src, dst, phdr->p_filesz);

		if (phdr->p_filesz)
			memcpy(dst, src, phdr->p_filesz);
		if (phdr->p_filesz != phdr->p_memsz)
			memset(dst + phdr->p_filesz, 0x00,
			       phdr->p_memsz - phdr->p_filesz);
		if (i == 0)
			show_img_version((char *)dst + 896, riscv_id);

		flush_cache(ROUND_DOWN_CACHE((unsigned long)dst),
			    ROUND_UP_CACHE(phdr->p_filesz));
		++phdr;
	}

	return 0;
}

int sunxi_riscv_init(u32 img_addr, u32 run_ddr, u32 riscv_id)
{

	u32 reg_val;
	u32 image_len = 0;

#ifdef CONFIG_SUNXI_VERIFY_RISCV
	if (sunxi_verify_riscv(img_addr, image_len, riscv_id) < 0) {
		return -1;
	}
#endif
	/* update run addr */
	update_reset_vec(img_addr, &run_ddr);
	/* load image to ram */
	load_image(img_addr, riscv_id);
	if (riscv_id == 0) { /* RISCV0 */
		printf("[bsp]: %s: %s(): +%d\n", __FILE__, __func__, __LINE__);
		/* clock gating */
		reg_val = readl_riscv(SUNXI_CCM_BASE + RISCV_GATING_RST_REG);
		reg_val |= RISCV_CLK_GATING;
		reg_val |= RISCV_GATING_RST_FIELD;
		writel_riscv(reg_val, SUNXI_CCM_BASE + RISCV_GATING_RST_REG);

		/* reset */
		reg_val = readl_riscv(SUNXI_CCM_BASE + RISCV_CFG_BGR_REG);
		reg_val |= RISCV_CFG_RST;
		reg_val |= RISCV_CFG_GATING;
		writel_riscv(reg_val, SUNXI_CCM_BASE + RISCV_CFG_BGR_REG);

		/* set start addr */
		reg_val = readl_riscv(RISCV_CFG_BASE + RISCV_STA_ADD_REG);
		reg_val |= run_ddr;
		writel_riscv(reg_val, RISCV_CFG_BASE + RISCV_STA_ADD_REG);
		/* clock gating */
		reg_val = readl_riscv(SUNXI_CCM_BASE + RISCV_GATING_RST_REG);
		reg_val |= RISCV_CLK_GATING;
		reg_val |= RISCV_GATING_RST_FIELD;
		writel_riscv(reg_val, SUNXI_CCM_BASE + RISCV_GATING_RST_REG);

		/* set e907 clock to peri_600M */
		reg_val = readl_riscv(SUNXI_CCM_BASE + CCMU_RISCV_CLK_REG);
		reg_val |= RISCV_CLK_PERI_600M;
		writel_riscv(reg_val, SUNXI_CCM_BASE + CCMU_RISCV_CLK_REG);

		/* clock gating reset*/
		reg_val = readl_riscv(SUNXI_CCM_BASE + RISCV_GATING_RST_REG);
		reg_val &= ~(0xffff << 16);
		reg_val |= (0x3 << 1 | 0x16aa << 16);
		writel_riscv(reg_val, SUNXI_CCM_BASE + RISCV_GATING_RST_REG);

		RISCV_DEBUG("cfg bgr reg(0x%08x):0x%08x\n", SUNXI_CCM_BASE + RISCV_CFG_BGR_REG,
						readl_riscv(SUNXI_CCM_BASE + RISCV_CFG_BGR_REG));
		RISCV_DEBUG("start addr reg(0x%08x):0x%08x\n", RISCV_CFG_BASE + RISCV_STA_ADD_REG,
						readl_riscv(RISCV_CFG_BASE + RISCV_STA_ADD_REG));
		RISCV_DEBUG("clock gating reg(0x%08x):0x%08x\n", SUNXI_CCM_BASE + RISCV_GATING_RST_REG,
						readl_riscv(SUNXI_CCM_BASE + RISCV_GATING_RST_REG));
	}
	RISCV_DEBUG("RISCV%d start ok, img length %d, booting from 0x%x\n",
			riscv_id, image_len, run_ddr);
	return 0;
}
