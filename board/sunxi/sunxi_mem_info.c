/* SPDX-License-Identifier: GPL-2.0+
   * (C) Copyright 2021
   * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
   * Some init for sunxi platform.
   */

#include <linux/compiler.h>
#include <linux/err.h>
#include <console.h>
#include <asm/io.h>
#include <linux/stddef.h>
#include <sunxi_board.h>
#include <common.h>
#include <image.h>
#include <android_image.h>
#include <asm/global_data.h>
#include <sunxi_board.h>
DECLARE_GLOBAL_DATA_PTR;
#define SUNXI_MEM_MAX_COUNT 20
#define SUNXI_NAME_LEN 32
typedef struct memory_map_new {
	char name[SUNXI_NAME_LEN];
	unsigned long startaddr;
	unsigned long endaddr;
} memory_map_new_t;
void sunxi_mem_info_dump(memory_map_new_t *memory_map);
int sunxi_mem_info(char pname[], void *addr, unsigned long len);

static __attribute__((section(".data")))
memory_map_new_t g_memory_info[SUNXI_MEM_MAX_COUNT];

int sunxi_mem_info(char pname[], void *addr, unsigned long len)
{
	memory_map_new_t *memory_map = g_memory_info;
	unsigned long startaddr      = (unsigned long)addr;
	int i			     = 0;
	if ((pname[0] == 0) || (addr == NULL)) {
		printf("memory name or addr is NULL");
		return -1;
	}
	if ((startaddr + len) > (CONFIG_SYS_SDRAM_BASE + gd->ram_size)) {
		printf("input addr exceed dram scope\n");
	}
	for (i = 0; (i < SUNXI_MEM_MAX_COUNT) && (memory_map->name[0] != 0);
	     memory_map++, i++) {
		if ((startaddr >= memory_map->startaddr) &&
		    (startaddr < memory_map->endaddr)) {
			printf("%s  addr 0x%lx 0x%lx exceed %s scope\n", pname,
			       startaddr, (startaddr + len), memory_map->name);
			printf("%s  addr 0x%lx 0x%lx\n", memory_map->name,
			       memory_map->startaddr, memory_map->endaddr);
		} else if ((startaddr < memory_map->startaddr) &&
			   (startaddr + len >= memory_map->startaddr)) {
			printf("%s  addr 0x%lx 0x%lx exceed %s scope\n", pname,
			       startaddr, (startaddr + len), memory_map->name);
			printf("%s  addr 0x%lx 0x%lx\n", memory_map->name,
			       memory_map->startaddr, memory_map->endaddr);
		} else if ((startaddr >= memory_map->startaddr) &&
			   ((startaddr + len) <= memory_map->endaddr)) {
			printf("%s  addr 0x%lx 0x%lx exceed %s scope\n", pname,
			       startaddr, (startaddr + len), memory_map->name);
			printf("%s  addr 0x%lx 0x%lx\n", memory_map->name,
			       memory_map->startaddr, memory_map->endaddr);
		} else if ((startaddr <= memory_map->startaddr) &&
			   ((startaddr + len) >= memory_map->endaddr)) {
			printf("%s  addr 0x%lx 0x%lx exceed %s scope\n", pname,
			       startaddr, (startaddr + len), memory_map->name);
			printf("%s  addr 0x%lx 0x%lx\n", memory_map->name,
			       memory_map->startaddr, memory_map->endaddr);
		}
	}
	if (i < SUNXI_MEM_MAX_COUNT) {
		memcpy(memory_map->name, pname, SUNXI_NAME_LEN);
		memory_map->startaddr = startaddr;
		memory_map->endaddr   = startaddr + len;
	} else {
		printf("memory info exceed  max count:%d\n",
		       SUNXI_MEM_MAX_COUNT);
		return -1;
	}
	return 0;
}

void sunxi_mem_info_dump(memory_map_new_t *memory_map)
{
	int i = 0;
	for (i = 0; i < SUNXI_MEM_MAX_COUNT; i++) {
		if (0 == (memory_map[i].name[0])) {
			break;
		}
		printf("memory info: %s  0x%lx 0x%lx\n", memory_map[i].name,
		       memory_map[i].startaddr, memory_map[i].endaddr);
	}
}

int do_sunxi_mem_info_dump(cmd_tbl_t *cmdtp, int flag, int argc,
			   char *const argv[])
{
	sunxi_mem_info_dump(g_memory_info);
	return 0;
}

U_BOOT_CMD(sunxi_mem_info_dump, 6, 1, do_sunxi_mem_info_dump,
	   "check if not exceed scope", "get memory scope for kernel fdt ..");
