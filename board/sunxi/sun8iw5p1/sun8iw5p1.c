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

    //we should open this bit before cache&mmu enable.
    //the cache is useless if smp bit is not set,although cache has been enabled.
    enable_smp();

	return 0;
}

char* board_hardware_info(void)
{
	static char * hardware_info  = "sun8iw5p1";
	return hardware_info;
}

extern int axp22_probe(void);

/**
 * platform_axp_probe -detect the pmu on  board
 * @sunxi_axp_dev_pt: pointer to the axp array
 * @max_dev: the max num of pmu
 * returns:
 *	the num of pmu
 */

#if 0
int platform_axp_probe(sunxi_axp_dev_t  *sunxi_axp_dev_pt[], int max_dev)
{
    if(!axp22_probe())
    {
        /* pmu type AXP22X */
        tick_printf("PMU: AXP22X found\n");
        sunxi_axp_dev_pt[0] = &sunxi_axp_22;
        return 0;
    }

    printf("probe axp failed\n");
    sunxi_axp_dev_pt[0] = &sunxi_axp_null;

    return 0;
}
#endif

int platform_axp_probe(sunxi_axp_dev_t  *sunxi_axp_dev_pt[], int max_dev)
{
	if(axp22_probe())
	{
		printf("probe AXP22X failed\n");
		sunxi_axp_dev_pt[0] = &sunxi_axp_null;
		return 0;
	}

	/* pmu type AXP81X */
	tick_printf("PMU: AXP22X found\n");

	sunxi_axp_dev_pt[0] = &sunxi_axp_22;

	//find one axp
	return 1;
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
	/*
	 * We should init the Dram options, and kernel get it by tag.
	 */
	int dram_size;
	int ret;
	//gd->ram_size = get_ram_size((long *)PHYS_SDRAM_1, PHYS_SDRAM_1_SIZE);
	ret = script_parser_fetch("dram_para", "dram_para1", &dram_size, 1);
	if(!ret)
	{
		dram_size &= 0xffff;
		if(dram_size)
		{
			gd->bd->bi_dram[0].size = dram_size * 1024 * 1024;
		}
		else
		{
			gd->bd->bi_dram[0].size = get_ram_size((long *)PHYS_SDRAM_1, PHYS_SDRAM_1_SIZE);
		}
	}
	else
	{
		gd->bd->bi_dram[0].size = get_ram_size((long *)PHYS_SDRAM_1, PHYS_SDRAM_1_SIZE);
	}
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
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
	uint *addr;

	//gd->ram_size = get_ram_size((long *)PHYS_SDRAM_1, PHYS_SDRAM_1_SIZE);

	//memcpy((void *)BOOT_STANDBY_DRAM_PARA_ADDR, uboot_spare_head.boot_data.dram_para, 32 * 4);  //add by jerry
	addr = (uint *)uboot_spare_head.boot_data.dram_para;
	if(addr[4])
	{
		gd->ram_size = (addr[4] & 0xffff) * 1024 * 1024;
	}
	 else
	{
		gd->ram_size = get_ram_size((long *)PHYS_SDRAM_1, PHYS_SDRAM_1_SIZE);
	}
	puts("dram_para_set start\n");
	script_parser_patch("dram_para", "dram_clk", &addr[0], 1);
	script_parser_patch("dram_para", "dram_type", &addr[1], 1);
	script_parser_patch("dram_para", "dram_zq", &addr[2], 1);
	script_parser_patch("dram_para", "dram_odt_en", &addr[3], 1);

	script_parser_patch("dram_para", "dram_para1", &addr[4], 1);
	script_parser_patch("dram_para", "dram_para2", &addr[5], 1);

	script_parser_patch("dram_para", "dram_mr0", &addr[6], 1);
	script_parser_patch("dram_para", "dram_mr1", &addr[7], 1);
	script_parser_patch("dram_para", "dram_mr2", &addr[8], 1);
	script_parser_patch("dram_para", "dram_mr3", &addr[9], 1);

	script_parser_patch("dram_para", "dram_tpr0", &addr[10], 1);
	script_parser_patch("dram_para", "dram_tpr1", &addr[11], 1);
	script_parser_patch("dram_para", "dram_tpr2", &addr[12], 1);
	script_parser_patch("dram_para", "dram_tpr3", &addr[13], 1);
	script_parser_patch("dram_para", "dram_tpr4", &addr[14], 1);
	script_parser_patch("dram_para", "dram_tpr5", &addr[15], 1);
	script_parser_patch("dram_para", "dram_tpr6", &addr[16], 1);
	script_parser_patch("dram_para", "dram_tpr7", &addr[17], 1);
	script_parser_patch("dram_para", "dram_tpr8", &addr[18], 1);
	script_parser_patch("dram_para", "dram_tpr9", &addr[19], 1);
	script_parser_patch("dram_para", "dram_tpr10", &addr[20], 1);
	script_parser_patch("dram_para", "dram_tpr11", &addr[21], 1);
	script_parser_patch("dram_para", "dram_tpr12", &addr[22], 1);
	script_parser_patch("dram_para", "dram_tpr13", &addr[23], 1);
	puts("dram_para_set end\n");

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

#ifdef CONFIG_DISPLAY_BOARDINFO
int checkboard(void)
{
	tick_printf("Board: SUN6I\n");
	return 0;
}
#endif

int cpu0_set_detected_paras(void)
{
	return 0;
}

#ifdef CONFIG_CMD_NET
#ifdef CONFIG_USB_ETHER
extern int sunxi_udc_probe(void);
#ifdef CONFIG_USE_UBOOT_SERIALNO
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
#ifdef CONFIG_USE_UBOOT_SERIALNO
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
