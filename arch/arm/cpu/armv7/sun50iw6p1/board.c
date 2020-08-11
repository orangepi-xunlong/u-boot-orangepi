/*
 * (C) Copyright 2017-2020
 *Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *zhouhuacai <zhouhuacai@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/timer.h>
#include <asm/arch/ccmu.h>
#include <asm/arch/clock.h>
#include <efuse_map.h>
#include <asm/arch/platform.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/key.h>
#include <asm/arch/dma.h>

#include <boot_type.h>
#include <sys_partition.h>
#include <sys_config.h>
#include <power/sunxi/pmu.h>
#include <smc.h>
#include <sunxi_board.h>
#include <fdt_support.h>
#include <sys_config_old.h>
#include <cputask.h>
#include <asm/arch-sun50iw6p1/sid.h>

/* The sunxi internal brom will try to loader external bootloader
 * from mmc0, nannd flash, mmc2.
 * We check where we boot from by checking the config
 * of the gpio pin.
 */
DECLARE_GLOBAL_DATA_PTR;

extern void power_limit_init(void);
extern int sunxi_arisc_probe(void);

int power_source_init(void)
{
	int pll_cpux;
	int axp_exist = 0;
	int pmu_type = 0;

	printf("u0:%x\n", readl(0x44000));
#ifdef CONFIG_SUNXI_ARISC_EXIST
	sunxi_arisc_probe();
#endif

	pmu_type = uboot_spare_head.boot_ext[0].data[0];
	if (pmu_type) {
		axp_exist =  axp_probe();
		if(axp_exist)
		{
			axp_probe_factory_mode();
#ifdef CONFIG_SUN50IW6P1_AXP802
			gd->pmu_saved_status = axp_probe_pre_sys_mode();
#endif
			if (axp_probe_power_supply_condition()) {
				printf("axp_probe_power_supply_condition error\n");
			}
			gd->vbus_status = axp_probe_vbus_type();
			set_sunxi_gpio_power_bias();
			axp_set_charge_vol_limit();
			axp_set_all_limit();
			axp_set_hardware_poweron_vol();
			if (axp_set_power_supply_output() < 0) {
				printf("axp_set_power_supply_output error\n");
				while (1) {
					;
				}
			}
		} else {
			printf("axp_probe error\n");
		}
	} else {
		set_sunxi_gpio_power_bias();
		sunxi_axp_dummy_init();
		gd->vbus_status = axp_probe_vbus_type();
	}

	sunxi_clock_set_corepll(uboot_spare_head.boot_data.run_clock);

	pll_cpux = sunxi_clock_get_corepll();
	tick_printf("PMU: cpux %d Mhz,AXI=%d Mhz\n", pll_cpux,sunxi_clock_get_axi());
	printf("PLL6=%d Mhz,AHB1=%d Mhz, APB1=%dMhz MBus=%dMhz\n", sunxi_clock_get_pll6(),
		sunxi_clock_get_ahb(),
		sunxi_clock_get_apb1(),
		sunxi_clock_get_mbus());

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
int sunxi_probe_securemode(void)
{
	int secure_mode = 0;

	secure_mode =  sid_get_security_status();
	printf("secure enable bit: %d\n", secure_mode);

	if(secure_mode)
	{
		//sbrom  set  secureos_exist flag,
		//1: secure os exist 0: secure os not exist
		if(uboot_spare_head.boot_data.secureos_exist==1)
		{
			gd->securemode = SUNXI_SECURE_MODE_WITH_SECUREOS;
			printf("secure mode: with secureos\n");
		}
		else
		{
			gd->securemode = SUNXI_SECURE_MODE_NO_SECUREOS;
			printf("secure mode: no secureos\n");
		}
		gd->bootfile_mode = SUNXI_BOOT_FILE_TOC;
	}
	else
	{
		//boot0  set  secureos_exist flag,
		//1: secure monitor exist 0: secure monitor  not exist
		int burn_secure_mode=0;

		gd->securemode = SUNXI_NORMAL_MODE;
		gd->bootfile_mode = SUNXI_BOOT_FILE_PKG;
		printf("normal mode: with secure monitor\n");

                //only usb/card prodcution can burn secure mode, not for one-key recovery or other update mode!
                int work_mode = get_boot_work_mode();
                if((work_mode != WORK_MODE_USB_PRODUCT) && (work_mode != WORK_MODE_CARD_PRODUCT))
                {
                        return 0;
                }

		if (script_parser_fetch("target", "burn_secure_mode", &burn_secure_mode, 1))
			return 0;

		if(burn_secure_mode != 1)
		{
			return 0;
		}
		gd->bootfile_mode = SUNXI_BOOT_FILE_TOC;
	}
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
int sunxi_set_secure_mode(void)
{
	int mode;

	if ((gd->securemode == SUNXI_NORMAL_MODE) &&
		(gd->bootfile_mode == SUNXI_BOOT_FILE_TOC))
	{
		mode = sid_probe_security_mode();
		if(!mode)
		{
			sid_set_security_mode();
			gd->bootfile_mode = SUNXI_BOOT_FILE_TOC;
		}
	}

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
int sunxi_get_securemode(void)
{
	return gd->securemode;
}

int sunxi_probe_secure_monitor(void)
{
	return uboot_spare_head.boot_data.monitor_exist == SUNXI_SECURE_MODE_USE_SEC_MONITOR?1:0;
}

static int fdt_set_gpu_dvfstable(int limit_max_freq)
{
	u32 operating_points[50] = {0};
	int len, ret, i;
	int  nodeoffset;	/* node offset from libfdt */
	char path_tmp[128] = {0};
	char node_name[32] = {0};
	int pmu_type = 0, vol = 0;

	pmu_type = uboot_spare_head.boot_ext[0].data[0];

	sprintf(path_tmp, "/gpu/");
	nodeoffset = fdt_path_offset (working_fdt, path_tmp);
	if (nodeoffset < 0) {
			/*
			* Not found or something else bad happened.
			*/

			printf ("can't find node \"%s\"!!!!\n",path_tmp);
			return 0;
	}
	sprintf(node_name, "operating-points");
	len = fdt_getprop_u32(working_fdt, nodeoffset,
				node_name, operating_points);
	if (len < 0) {
		printf("Get %s%s failed\n", path_tmp, node_name);
		return 0;
	}

	for (i = 0; i < len; i++) {
		if (operating_points[i] == limit_max_freq) {
			vol = operating_points[i+1];
			break;
		}
	}
	for (i = 0; i < len; i++) {
		if (pmu_type) {
			if (i % 2 == 0) {
				if (operating_points[i] > limit_max_freq) {
					operating_points[i] = limit_max_freq;
					operating_points[i+1] = ((vol > 0) ?
							vol : 95000);
				}
			}
		} else {
			if (i % 2 == 0) {
				if (operating_points[i] > limit_max_freq) {
					operating_points[i] = limit_max_freq;
				}
			} else {
				operating_points[i] = 980000;
			}
		}
	}

	for (i = 0; i < len; i++) {
		operating_points[i] = cpu_to_fdt32(operating_points[i]);
	}

	ret = fdt_setprop(working_fdt, nodeoffset, node_name,
			operating_points, sizeof(u32) * len);
	if (ret < 0) {
		printf("Set %s%s failed\n", path_tmp, node_name);
		return -1;;
	}

	return 0;
}
static int fdt_set_cpu_dvfstable(int limit_max_freq)
{
	int ret = 0, table_num = 0;
	uint32_t max_freq = 0;
	int  nodeoffset;	/* node offset from libfdt */
	char path_tmp[128] = {0};
	char node_name[32] = {0};
	u32  data_arr[1];

	for (;; ++table_num) {
		sprintf(path_tmp, "/dvfs_table/dvfs_table_%d/", table_num);

		nodeoffset = fdt_path_offset (working_fdt, path_tmp);
		if (nodeoffset < 0) {
			/*
			* Not found or something else bad happened.
			*/

			printf ("can't find node \"%s\"!!!!\n",path_tmp);
			break;
		}

		sprintf(node_name, "max_freq");
		ret = fdt_getprop_u32(working_fdt,nodeoffset,
					node_name,&max_freq);
		if (ret < 0) {
			printf("Get %s%s failed\n", path_tmp, node_name);
			return 0;
		}
		if (max_freq > limit_max_freq)
			max_freq = limit_max_freq;
		else
			continue;

		data_arr[0] = cpu_to_fdt32(max_freq);
		ret = fdt_setprop(working_fdt, nodeoffset, node_name,
					data_arr, sizeof(data_arr));
		if (ret < 0) {
			printf("Set %s%s failed\n", path_tmp, node_name);
			return -1;;
		}
	}
	return 0;
}
int verify_init(void)
{
	int chipid = 0, ret = 0;

	chipid = sid_read_key(0x0) & 0xff;
	if (chipid == 0x03 || chipid == 0x08 || chipid == 0x04 || chipid == 0x11) {
		ret = fdt_set_cpu_dvfstable(1488000000);
		if (ret == 0)
			printf("sussessfully[c%d%d]\n", chipid, 1488);
		fdt_set_gpu_dvfstable(624000);
		if (ret == 0)
			printf("sussessfully[g%d%d]\n", chipid, 624);
	}
	return ret;
}
