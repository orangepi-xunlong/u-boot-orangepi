/*
 * (C) Copyright 2007-2015
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.     See the
 * GNU General Public License for more details.
 *
 * SPDX-License-Identifier:    GPL-2.0
 */

#include <common.h>
#include <sunxi_mbr.h>
#include <boot_type.h>
#include <sys_partition.h>
#include <sys_config.h>
#include <mmc.h>
#include <power/sunxi/axp.h>
#include <asm/io.h>
#include <power/sunxi/pmu.h>

DECLARE_GLOBAL_DATA_PTR;


/* add board specific code here */
int board_init(void)
{

#ifdef CONFIG_MACH_TYPE
    gd->bd->bi_arch_number_useless = CONFIG_MACH_TYPE; /* board id for Linux */
#endif
    return 0;
}


void dram_init_banksize(void)
{
    gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
    gd->bd->bi_dram[0].size  = gd->ram_size;

    return ;
}



int dram_init(void)
{
    uint *addr = NULL;
    addr = (uint *)uboot_spare_head.boot_data.dram_para;

    if(addr[4])
    {
        gd->ram_size = (addr[1] & 0xffff) * 1024 * 1024;
    }
    else
    {
        gd->ram_size = get_ram_size((long *)PHYS_SDRAM_1, PHYS_SDRAM_1_SIZE);
    }
    puts("DRAM:");
    print_size(gd->ram_size, "");
    putc('\n');
    return 0;
}

#ifdef CONFIG_GENERIC_MMC

extern int sunxi_mmc_init(int sdc_no);

int board_mmc_init(bd_t *bis)
{
    sunxi_mmc_init(bis->bi_card_num);

    return 0;
}

void board_mmc_pre_init(int card_num)
{
    bd_t *bd;

    bd = gd->bd;
    gd->bd->bi_card_num = card_num;
    mmc_initialize(bd);

    return ;
}

int board_mmc_get_num(void)
{
    return gd->boot_card_num;
}


void board_mmc_set_num(int num)
{
    gd->boot_card_num = num;

    return ;
}

#endif



#ifdef CONFIG_DISPLAY_BOARDINFO
int checkboard(void)
{
    printf("Board: SUN3I\n");
    return 0;
}
#endif

int cpu0_set_detected_paras(void)
{
    return 0;
}

ulong get_spare_head_size(void)
{
    return (ulong)sizeof(struct spare_boot_head_t);
}

extern int axp15_probe(void);
extern int axp20_probe(void);


/**
 * platform_axp_probe -detect the pmu on  board
 * @sunxi_axp_dev_pt: pointer to the axp array
 * @max_dev: the size of the array where sunxi_axp_dev_pt point to
 * returns:
 *    the num of pmu
 */

int platform_axp_probe(sunxi_axp_dev_t  *sunxi_axp_dev_pt[], int max_dev)
{
    sunxi_axp_dev_pt[0] = &sunxi_axp_null;
    return 0;

}

char* board_hardware_info(void)
{
    static char * hardware_info  = "sun3iw1p1";
    return hardware_info;
}


