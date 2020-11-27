/*
 * (C) Copyright 2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * zhouhuacai <zhouhuacai@allwinnertech.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * SPDX-License-Identifier:	GPL-2.0
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
#include <asm/arch/ccmu.h>
#include <i2c.h>
#include <sunxi_i2c.h>

DECLARE_GLOBAL_DATA_PTR;


int enable_smp(void)
{
   //SMP status is controlled by bit 6 of the CP15 Aux Ctrl Reg
   asm volatile("MRC     p15, 0, r0, c1, c0, 1");  // Read ACTLR
   asm volatile("ORR     r0, r0, #0x040");         // Set bit 6
   asm volatile("MCR     p15, 0, r0, c1, c0, 1");  // Write ACTLR
    return 0;
}

int board_init(void)
{
	//asm volatile("b .");
    u32 reg_val;
    int cpu_status = 0;
    cpu_status = readl(SUNXI_C0_CPUX_CFG_BASE + 0x80);
    cpu_status &= (0x7<<24);

    //note:
    //sbrom will enable smp bit when jmp to non-secure fel on AW1718.
    //but normal brom not do this operation.
    //so should enable smp when run uboot by normal fel mode.
    if(!cpu_status)
        enable_smp();

    if (uboot_spare_head.boot_data.work_mode != WORK_MODE_USB_PRODUCT)
    {
        //VE SRAM:set sram to normal mode, default boot mode
        reg_val = readl(SUNXI_SYSCRL_BASE+0X0004);
        reg_val &= ~(0x1<<24);
        writel(reg_val, SUNXI_SYSCRL_BASE+0X0004);

        //VE gating&VE Bus Reset :brom set them, but not require now
        reg_val = readl(CCMU_VE_BGR_REG);
        reg_val &= ~(0x1<<0);
        reg_val &= ~(0x1<<16);
        writel(reg_val, CCMU_VE_BGR_REG);
    }

    return 0;
}

void dram_init_banksize(void)
{
    gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
    gd->bd->bi_dram[0].size = gd->ram_size;
}

int dram_init(void)
{
    uint dram_size = 0;
    dram_size = uboot_spare_head.boot_data.dram_scan_size;
    if(dram_size)
    {
        gd->ram_size = dram_size * 1024 * 1024;
    }
    else
    {
        gd->ram_size = get_ram_size((long *)PHYS_SDRAM_1, PHYS_SDRAM_1_SIZE);
    }

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

}

int board_mmc_get_num(void)
{
    return gd->boot_card_num;
}


void board_mmc_set_num(int num)
{
    gd->boot_card_num = num;
}

#endif



#ifdef CONFIG_DISPLAY_BOARDINFO
int checkboard(void)
{
    printf("Board: SUN6I\n");
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

extern int axp858_probe(void);

/**
 * platform_axp_probe -detect the pmu on  board
 * @sunxi_axp_dev_pt: pointer to the axp array
 * @max_dev: offset of the property to retrieve
 * returns:
 *	the num of pmu
 */
int platform_axp_probe(sunxi_axp_dev_t  *sunxi_axp_dev_pt[], int max_dev)
{
#ifdef CONFIG_SUNXI_MODULE_AXP
#ifdef CONFIG_AXP_USE_I2C
	i2c_set_bus_num(SUNXI_R_I2C0);
	i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
#endif
    if (axp858_probe()) {
	printf("probe axp858 failed\n");
        sunxi_axp_dev_pt[0] = &sunxi_axp_null;
        return 0;
    }

    /* pmu type AXP81X */
    tick_printf("PMU: AXP858 found\n");
    sunxi_axp_dev_pt[0] = &sunxi_axp_858;
#else
    sunxi_axp_dev_pt[0] = &sunxi_axp_null;
#endif
    //find one axp
    return 1;

}

char* board_hardware_info(void)
{
    static char *hardware_info  = "sun8iw17p1";
    return hardware_info;
}

#ifdef CONFIG_CMD_NET
#ifdef CONFIG_USB_ETHER
extern int sunxi_udc_probe(void);
#ifdef CONFIG_SUNXI_SERIAL
extern int get_serial_num_from_chipid(char* serial);

static int sunxi_serial_num_is_zero(char *serial)
{
    int i = 0;

    get_serial_num_from_chipid(serial);
    while(i < 20) {
        if (serial[i] != '0')
            break;
        i++;
    }
    if (i == 20)
        return 0;
    else
        return 1;
}
#endif

static void sunxi_random_ether_addr(void)
{
    int i = 0;
    char serial[128] = {0};
    ulong tmp = 0;
    char tmp_s[5] = "";
    unsigned long long rand = 0;
    uchar usb_net_addr[6];
    char mac[18] = "";
    char tmp_mac = 0;
    int ret = 0;

    /*
     * get random mac address from serial num if it's not zero, or from timer.
     */
#ifdef CONFIG_SUNXI_SERIAL
    ret = sunxi_serial_num_is_zero(serial);
#endif
    if (ret == 1) {
        for(i = 0; i < 6; i++) {
            if(i == 0)
                strncpy(tmp_s, serial+16, 4);
            else if ((i == 1) || (i == 4))
                strncpy(tmp_s, serial+12, 4);
            else if (i == 2)
                strncpy(tmp_s, serial+8, 4);
            else
                strncpy(tmp_s,serial+4,4);

            tmp = simple_strtoul(tmp_s, NULL, 16);
            rand = (tmp) * 0xfedf4fd;

            rand = rand * 0xd263f967 + 0xea6f22ad8235;
            usb_net_addr[i] = (uchar)(rand % 0x100);
        }
    } else {
        for(i = 0; i < 6; i++) {
            rand = get_timer_masked() * 0xfedf4fd;
            rand = rand * 0xd263f967 + 0xea6f22ad8235;
            usb_net_addr[i] = (uchar)(rand % 0x100);
        }
    }

    /*
     * usbnet_hostaddr, usb_net_addr[0] = 0xxx xx10
     */
    tmp_mac = usb_net_addr[0] & 0x7e;
    tmp_mac = tmp_mac | 0x02;
    sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", tmp_mac, usb_net_addr[1],usb_net_addr[2],
            usb_net_addr[3],usb_net_addr[4],usb_net_addr[5]);
    setenv("usbnet_hostaddr", mac);

    /*
     * usbnet_devaddr, usb_net_addr[0] = 1xxx xx10
     */
    tmp_mac = usb_net_addr[0] & 0xfe;
    tmp_mac = tmp_mac | 0x82;
    sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", tmp_mac, usb_net_addr[1],usb_net_addr[2],
            usb_net_addr[3],usb_net_addr[4],usb_net_addr[5]);
    setenv("usbnet_devaddr", mac);
}
#endif

int board_eth_init(bd_t *bis)
{
    int rc = 0;

#if defined(CONFIG_USB_ETHER)
    sunxi_random_ether_addr();
    sunxi_udc_probe();
    usb_eth_initialize(bis);
#endif

    return rc;
}
#endif

#ifdef TIMESTAMP
void clean_timestamp_counter(void)
{
    u32 val = 0;
    val = readl(TIMESTAMP_CTRL_BASE + 0);
    val &= ~(1 << 0);
    writel(val, (TIMESTAMP_CTRL_BASE + 0));

    writel(0x0, (TIMESTAMP_CTRL_BASE + 0x8));
    writel(0x0, (TIMESTAMP_CTRL_BASE + 0xC));

    asm volatile ("DSB");
    asm volatile ("ISB");

    val = readl(TIMESTAMP_CTRL_BASE + 0);
    val |= (1 << 0);
    writel(val, (TIMESTAMP_CTRL_BASE + 0));
}
#endif
