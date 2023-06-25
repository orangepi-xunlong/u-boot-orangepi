// SPDX-License-Identifier: GPL-2.0+
/*
 * drivers/dsp/common/dsp_img.c
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
#include "elf.h"
#include "imgdts.h"
#include "dsp_img.h"
#include "dsp_ic.h"
#include "dsp_fdt.h"

int show_img_version(char *head_addr, u32 dsp_id)
{

	printf("dsp%d version is %s\n", dsp_id, head_addr);

	return 0;
}

int find_img_section(u32 img_addr,
			const char *section_name,
			unsigned long *section_addr)
{
	int i = 0;
	unsigned char *strtab = NULL;
	int ret = -1;

	Elf32_Ehdr *ehdr = NULL;
	Elf32_Shdr *shdr = NULL;

	ehdr = (Elf32_Ehdr *)(ADDR_TPYE)img_addr;
	shdr = (Elf32_Shdr *)(ADDR_TPYE)(img_addr + ehdr->e_shoff
			+ (ehdr->e_shstrndx * sizeof(Elf32_Shdr)));

	if (shdr->sh_type == SHT_STRTAB)
		strtab = (unsigned char *)(ADDR_TPYE)(img_addr + shdr->sh_offset);

	for (i = 0; i < ehdr->e_shnum; ++i) {
		shdr = (Elf32_Shdr *)(ADDR_TPYE)(img_addr + ehdr->e_shoff
				+ (i * sizeof(Elf32_Shdr)));

		if (!(shdr->sh_flags & SHF_ALLOC)
			|| (shdr->sh_addr == 0)
			|| (shdr->sh_size == 0)) {

			continue;
		}

		if (strtab) {

			char *pstr = (char *)(&strtab[shdr->sh_name]);

			if (strcmp(pstr, section_name) == 0) {
				DSP_DEBUG("find dsp section: %s ,addr = 0x%lx\n",
					pstr, (unsigned long)shdr->sh_addr);
				*section_addr = shdr->sh_addr;
				ret = 0;
				break;
			}
		}
	}

	return ret;
}


int get_img_len(u32 img_addr,
		unsigned long section_addr,
		u32 *img_len)
{
	int ret = -1;
	int i = 0;
	struct spare_rtos_head_t *prtos = NULL;
	Elf32_Ehdr *ehdr = NULL;
	Elf32_Phdr *phdr = NULL;

	ehdr = (Elf32_Ehdr *)(ADDR_TPYE)img_addr;
	phdr = (Elf32_Phdr *)(ADDR_TPYE)(img_addr + ehdr->e_phoff);


	for (i = 0; i < ehdr->e_phnum; ++i) {
		if (section_addr == (unsigned long)phdr->p_paddr) {
			prtos = (struct spare_rtos_head_t *)(ADDR_TPYE)(img_addr
					+ phdr->p_offset);
			*img_len = prtos->rtos_img_hdr.image_size;
			ret = 0;
			break;
		}
		++phdr;
	}
	return ret;
}


int set_msg_dts(u32 img_addr,
		unsigned long section_addr,
		struct dts_msg_t *dts_msg)
{
	int ret = -1;
	int i = 0;
	struct spare_rtos_head_t *prtos = NULL;
	Elf32_Ehdr *ehdr = NULL;
	Elf32_Phdr *phdr = NULL;

	ehdr = (Elf32_Ehdr *)(ADDR_TPYE)img_addr;
	phdr = (Elf32_Phdr *)(ADDR_TPYE)(img_addr + ehdr->e_phoff);

	for (i = 0; i < ehdr->e_phnum; ++i) {
		if (section_addr == (unsigned long)phdr->p_paddr) {
			prtos = (struct spare_rtos_head_t *)(ADDR_TPYE)(img_addr
					+ phdr->p_offset);
			memcpy((void *)&prtos->rtos_img_hdr.dts_msg,
					(void *)dts_msg,
					sizeof(struct dts_msg_t));
			ret = 0;
			break;
		}
		++phdr;
	}
	return ret;
}



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

