/*
 * (C) Copyright 2007-2013
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include <malloc.h>
#include <asm/io.h>
#include <fastboot.h>

#include <asm/arch/nand_bsp.h>
#include <mmc.h>
#include <android_misc.h>
#include <sunxi_mbr.h>
#include <boot_type.h>
#include <sys_partition.h>
#include <sys_config_old.h>
#include <power/sunxi/axp.h>
#include <power/sunxi/pmu.h>

DECLARE_GLOBAL_DATA_PTR;

extern int sunxi_clock_get_ahb(void);
extern int sunxi_clock_get_apb(void);

void enable_smp(void)
{
       //SMP status is controlled by bit 6 of the CP15 Aux Ctrl Reg
	asm volatile("MRC     p15, 0, r0, c1, c0, 1");  // Read ACTLR
	asm volatile("ORR     r0, r0, #0x040");         // Set bit 6
	asm volatile("MCR     p15, 0, r0, c1, c0, 1");  // Write ACTLR
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
/* add board specific code here */
int board_init(void)
{
	gd->bd->bi_arch_number = LINUX_MACHINE_ID;
	gd->bd->bi_boot_params = (PHYS_SDRAM_1 + 0x100);
	debug("board_init storage_type = %d\n",uboot_spare_head.boot_data.storage_type);

	//
	if(uboot_spare_head.boot_data.work_mode != WORK_MODE_USB_PRODUCT) {
		/* enable de,ee sram */
		writel(readl(0x01c00004) & 0xfeffffff, 0x01c00004);

		/* enable pll_de clk, because de and ee clks have enabled */
		writel(readl(0x01c20048) | 0x80000000, 0x01c20048);

		extern void set_pll_ahb_apb(void);
		//sramc map to de|ee, so set ahb clk to 200M here
		//when sram map to cpu,the clock of AHB max to 100M
		//notice: not call this function in clock.c because of the SP
		//of uboot at sramc before relocation.
		set_pll_ahb_apb();
	}

	//* mask eink engine gating clock *
	writel(readl(0x01c20064) & 0xffffdfff, 0x01c20064);
	//* assert eink engine *
	writel(readl(0x01c202c4) & 0xffffdfff, 0x01c202c4);

	//set smp bit before mmu&dcache enable.
	enable_smp();
	return 0;
}

/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int show_platform_info(void)
{
	printf("After SRAMC Map to DE|EE,AHB=%dM APB=%dM\n", sunxi_clock_get_ahb(),sunxi_clock_get_apb());
	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
void dram_init_banksize(void)
{
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
        gd->bd->bi_dram[0].size  = gd->ram_size;
}

/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
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
    //gd->bd->bi_card_num = card_num;
}

int board_mmc_get_num(void)
{
    return gd->boot_card_num;
}


void board_mmc_set_num(int num)
{
    gd->boot_card_num = num;
}


//int mmc_get_env_addr(struct mmc *mmc, u32 *env_addr) {
//
//	*env_addr = sunxi_partition_get_offset_byname(CONFIG_SUNXI_ENV_PARTITION);
//	return 0;
//}
#endif


int cpu0_set_detected_paras(void)
{
	return 0;
}

/**
 * platform_axp_probe -detect the pmu on  board
 * @sunxi_axp_dev_pt: pointer to the axp array
 * @max_dev: offset of the property to retrieve
 * returns:
 *	the num of pmu
 */

extern int axp22_probe(void);
extern int axp15_probe(void);

int platform_axp_probe(sunxi_axp_dev_t  *sunxi_axp_dev_pt[], int max_dev)
{
	if(!axp22_probe())
	{
		/* pmu type AXP22X */
		tick_printf("PMU: AXP22X found\n");
		sunxi_axp_dev_pt[0] = &sunxi_axp_22;
		return 0;
	}

	if(!axp15_probe())
	{
		/* pmu type AXP15X */
		tick_printf("PMU: AXP15X found\n");
		sunxi_axp_dev_pt[0] = &sunxi_axp_15;
		return 0;
	}

	printf("probe axp failed\n");
	sunxi_axp_dev_pt[0] = &sunxi_axp_null;

	return 0;
}

char* board_hardware_info(void)
{
	static char * hardware_info  = "sun8iw10p1";
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
