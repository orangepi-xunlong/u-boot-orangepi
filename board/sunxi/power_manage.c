/*
 * Copyright (C) 2019 Allwinner.
 * weidonghui <weidonghui@allwinnertech.com>
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#include <sunxi_power/power_manage.h>
#include <sys_config.h>
#include <sunxi_board.h>
#include <spare_head.h>
#include <sunxi_display2.h>
#include <console.h>
/*
 * Global data (for the gd->bd)
 */
DECLARE_GLOBAL_DATA_PTR;

int set_gpio_bias(void)
{
	char bias_name[32];
	int bias_vol;
	int val;
	int nodeoffset = -1, offset;
	const struct fdt_property *prop;
	const char *pname;
	const int *pdata;
	nodeoffset = fdt_path_offset(working_fdt, FDT_PATH_GPIO_BIAS);
	for (offset = fdt_first_property_offset(working_fdt, nodeoffset);
	     offset > 0; offset = fdt_next_property_offset(working_fdt, offset)) {
		prop  = fdt_get_property_by_offset(working_fdt, offset, NULL);
		pname = fdt_string(working_fdt, fdt32_to_cpu(prop->nameoff));
		pdata = (const int *)prop->data;
		bias_vol = fdt32_to_cpu(pdata[0]);
		memset(bias_name, 0, sizeof(bias_name));
		strcpy(bias_name, pname);
		if (strstr((const char *)bias_name, "bias") == NULL) {
			continue;
		}
		printf("bias_name:%s\t bias_vol:%d\n", bias_name, bias_vol);
		if (bias_name[1] == 'l') {
			val = readl(SUNXI_R_PIO_BASE + 0x340);
			if (bias_vol <= 1800) {
				val |= (1 << 0);
			} else {
				val &= ~(1 << 0);
			}
			writel(val, SUNXI_R_PIO_BASE + 0x340);
		} else if ((bias_name[1] <= 'j') && (bias_name[1] >= 'a')) {
			val = readl(SUNXI_PIO_BASE + 0x340);
			if (bias_vol <= 1800) {
				val |= (1 << (bias_name[1] - 'a'));
			} else {
				val &= ~(1 << (bias_name[1] - 'a'));
			}
			writel(val, SUNXI_PIO_BASE + 0x340);
		}
	}
	return 0;
}

int axp_set_power_supply_output(void)
{
	int onoff, val = 0, ret = 0;
	int power_delay = 0;
	char delay_name[32];
	char power_name[32];
	int power_vol;
	int power_vol_d;
	int nodeoffset = -1, nodeoffset2 = -1, offset;
	int twi_nodeoffset = -1;
	int pmu_nodeoffset = -1;
	const struct fdt_property *prop;
	const char *pname;
	const int *pdata;
	const char *twi_regu_name;
	char pmu_node_str[8] = {0};
	char twi_path[128] = {0};
	int work_mode = get_boot_work_mode();

#ifdef CONFIG_SUNXI_TRY_POWER_SPLY
	char axp_name[16] = {0}, chipid;
	char axp_sply_path[64] = {0};
	pmu_get_info(axp_name, (unsigned char *)&chipid);
	sprintf(axp_sply_path, "/soc/%s_power_sply", axp_name);
	nodeoffset = fdt_path_offset(working_fdt, axp_sply_path);
#endif
	if (nodeoffset < 0) {
		nodeoffset = fdt_path_offset(working_fdt, FDT_PATH_POWER_SPLY);
	}

	nodeoffset2 =  fdt_path_offset(working_fdt, FDT_PATH_GPIO_BIAS);
	if (nodeoffset2 < 0) {
		pr_msg("%s get gpio bias information fail!\n", __func__);
	}

	char sply_node[32], bias_node[32];
	int reg_base, pin_base;
	int i = 0, ret1, ret2;

	struct pin_bias_t {
		const char *pin_name;
		char *supply_name;
		int gpio_bias;
	} pin_bias[] = {
		{"pc"},
		{"pl"},
	};

	/* Get pmu node and its parent twi node*/
	sprintf(pmu_node_str, "pmu0");
	pmu_nodeoffset = fdt_path_offset(working_fdt, pmu_node_str);
	twi_nodeoffset = fdt_parent_offset(working_fdt, pmu_nodeoffset);
	fdt_get_path(working_fdt, twi_nodeoffset, twi_path, sizeof(twi_path));

	/* Get the regulator for pmu's twi, for changing pinctrl when change voltage*/
	twi_regu_name = fdt_get_regulator_name(twi_nodeoffset, "twi");

	/* For change GPIO[x] bias when gpio voltage is changed */
	for (i = 0; i < sizeof(pin_bias)/sizeof(pin_bias[0]); i++) {
		sprintf(sply_node, "%s_supply", pin_bias[i].pin_name);
		sprintf(bias_node, "%s_bias", pin_bias[i].pin_name);
		ret1 = fdt_getprop_string(working_fdt, nodeoffset2, sply_node, (char **)(&(pin_bias[i].supply_name)));
		ret2 = script_parser_fetch(FDT_PATH_GPIO_BIAS, bias_node, &(pin_bias[i].gpio_bias), 0);
		pr_msg("gpio_bias, %s: %4d, %s: %-9s\n",
				bias_node, ret2 < 0 ? -1  : pin_bias[i].gpio_bias,
				sply_node, ret1 < 0 ? "not set" : pin_bias[i].supply_name);
	};

	for (offset = fdt_first_property_offset(working_fdt, nodeoffset);
	     offset > 0; offset = fdt_next_property_offset(working_fdt, offset)) {
		prop  = fdt_get_property_by_offset(working_fdt, offset, NULL);
		pname = fdt_string(working_fdt, fdt32_to_cpu(prop->nameoff));
		pdata = (const int *)prop->data;
		power_vol = fdt32_to_cpu(pdata[0]);
		memset(power_name, 0, sizeof(power_name));
		strcpy(power_name, pname);
		if (strstr((const char *)power_name, "vol") == NULL) {
			continue;
		}

		onoff       = -1;
		power_vol_d = 0;

		if (power_vol > 1100000) {
			if (work_mode != WORK_MODE_BOOT) {
				onoff       = 0;
			} else {
				onoff       = 1;
			}
			power_vol_d = power_vol % 10000;
		} else if (power_vol > 10000) {
			onoff       = 1;
			power_vol_d = power_vol % 10000;

		} else if (power_vol >= 0) {
			onoff       = 0;
			power_vol_d = power_vol;
		}

		debug("%s = %d, onoff=%d\n", power_name, power_vol_d, onoff);

		if (pmu_set_voltage(power_name, power_vol_d, onoff)) {
			debug("axp set %s to %d failed\n", power_name,
			       power_vol_d);
		}

		/*set delay for each output*/
		sprintf(delay_name, "%s_delay", power_name);
		ret = script_parser_fetch(FDT_PATH_POWER_DELAY, delay_name, &power_delay, 0);
		if (ret < 0)
			power_delay = 0;
		if (power_delay != 0) {
			pr_msg("%s need to wait stable!\n", power_name);

			/* change twi pinctrl to shorten delay time */
			if (!strncmp(twi_regu_name, power_name, strlen(twi_regu_name))) {
				fdt_set_all_pin(twi_path, "pinctrl-1");
				pr_msg("%s has set pinctrl-1\n", twi_path);
			}

			mdelay(power_delay / 1000);

			if (!strncmp(twi_regu_name, power_name, strlen(twi_regu_name))) {
				fdt_set_all_pin(twi_path, "pinctrl-0");
				pr_msg("%s has set pinctrl-0\n", twi_path);
			}
		}

		for (i = 0; i < sizeof(pin_bias)/sizeof(pin_bias[0]); i++) {
			if (!strncmp(pin_bias[i].supply_name, power_name, sizeof(power_name))) {
				if (pin_bias[i].gpio_bias == 0)
					pin_bias[i].gpio_bias = power_vol_d;

				if (pin_bias[i].pin_name[1] >= 'l') {
					reg_base = SUNXI_R_PIO_BASE;
					pin_base = 'l';
				} else {
					reg_base = SUNXI_PIO_BASE;
					pin_base = 'a';
				}

				val = readl(reg_base + 0x340);
				if (pin_bias[i].gpio_bias <= 1800)
					val |=  (1 << (pin_bias[i].pin_name[1] - pin_base));
				else
					val &= ~(1 << (pin_bias[i].pin_name[1] - pin_base));

				writel(val, reg_base + 0x340);
				pr_msg("GPIO%c change bias done!\n", pin_bias[i].pin_name[1] - 'a' + 'A');
			}
		}

		pr_msg("%s = %d, onoff=%d\n", power_name, pmu_get_voltage(power_name), onoff);
	}

#ifndef CONFIG_GPIO_BIAS_SKIP
	set_gpio_bias();
#endif

	return 0;
}

int axp_set_charge_vol_limit(char *dev)
{
	int limit_vol = 0;
	if (strstr(dev, "vol") == NULL) {
		debug("Illegal string");
		return -1;
	}
	if (script_parser_fetch(FDT_PATH_CHARGER0, dev, &limit_vol, 1)) {
		return -1;
	}
	pmu_set_bus_vol_limit(limit_vol);
	return 0;
}

int axp_set_current_limit(char *dev)
{
	int limit_cur = 0;
	if (strstr(dev, "cur") == NULL) {
		debug("Illegal string");
		return -1;
	}
	if (script_parser_fetch(FDT_PATH_CHARGER0, dev, &limit_cur, 1)) {
		return -1;
	}
	if (!strncmp(dev, "pmu_runtime_chgcur", sizeof("pmu_runtime_chgcur")) ||
	    !strncmp(dev, "pmu_suspend_chgcur", sizeof("pmu_suspend_chgcur"))) {
		bmu_set_charge_current_limit(limit_cur);
	} else {
		bmu_set_vbus_current_limit(limit_cur);
	}
	return 0;
}

int axp_get_battery_status(void)
{
	int dcin_exist, bat_vol;
	int ratio;
	int safe_vol = 0, safe_ratio = 1;
	dcin_exist   = bmu_get_axp_bus_exist();
	bat_vol      = bmu_get_battery_vol();
	script_parser_fetch(FDT_PATH_CHARGER0, "pmu_safe_vol", &safe_vol, -1);
	script_parser_fetch(FDT_PATH_CHARGER0, "pmu_safe_ratio", &safe_ratio, 1);
	ratio = bmu_get_battery_capacity();
	pr_msg("bat_vol=%d, ratio=%d\n", bat_vol, ratio);
	pr_msg("safe_vol=%d, safe_ratio=%d\n", safe_vol, safe_ratio);
	if (ratio < safe_ratio) {
		if (dcin_exist) {
			return BATTERY_RATIO_TOO_LOW_WITH_DCIN;
		}
		return BATTERY_RATIO_TOO_LOW_WITHOUT_DCIN;
	}
	if (bat_vol < safe_vol) {
		return BATTERY_VOL_TOO_LOW;
	}
	return BATTERY_RATIO_ENOUGH;
}

int sunxi_bat_low_vol_handle(void)
{
	int i = 0, safe_vol = 0, safe_ratio = 1;
	int onoff = DISP_LCD_BACKLIGHT_ENABLE;
	__maybe_unused char arg[3] = {0};
	int bat_vol      = bmu_get_battery_vol();
	int dcin_exist   = bmu_get_axp_bus_exist();
	int bat_ratio    = bmu_get_battery_capacity();
	script_parser_fetch(FDT_PATH_CHARGER0, "pmu_safe_vol", &safe_vol, -1);
	script_parser_fetch(FDT_PATH_CHARGER0, "pmu_safe_ratio", &safe_ratio, 1);
	pr_force("bat_ratio:%d\tsafe_ratio:%d\tbat_vol:%dmV\tsafe_vol:%dmV\n",
			bat_ratio, safe_ratio, bat_vol, safe_vol);
	while (bat_vol < safe_vol || bat_ratio < safe_ratio) {
		bat_vol = bmu_get_battery_vol();
		bat_ratio = bmu_get_battery_capacity();
		dcin_exist = bmu_get_axp_bus_exist();
		if (onoff == DISP_LCD_BACKLIGHT_ENABLE) {
			if (i++ >= 500) {
				i = 0;
				onoff = DISP_LCD_BACKLIGHT_DISABLE;
				pr_notice("onoff:DISP_LCD_BACKLIGHT_DISABLE\n");
				pr_force("bat_ratio:%d\tsafe_ratio:%d\tbat_vol:%dmV\tsafe_vol:%dmV\n",
						bat_ratio, safe_ratio, bat_vol, safe_vol);
#ifdef CONFIG_DISP2_SUNXI
				disp_ioctl(NULL, onoff, (void *)arg);
#endif
			}
		} else {
			if (pmu_get_key_irq() > 0) {
				i = 0;
				onoff = DISP_LCD_BACKLIGHT_ENABLE;
				pr_notice("onoff:DISP_LCD_BACKLIGHT_ENABLE\n");
				pr_force("bat_ratio:%d%\tbat_vol:%dmV\tsafe_vol:%dmV\n", bat_ratio, bat_vol, safe_vol);
#ifdef CONFIG_DISP2_SUNXI
				disp_ioctl(NULL, onoff, (void *)arg);
#endif
			}
		}
		if (!dcin_exist) {
#ifdef CONFIG_CMD_SUNXI_BMP
			sunxi_bmp_display("bat\\low_pwr.bmp");
#endif
			tick_printf("battery ratio is low without dcin,to be shutdown\n");
			mdelay(3000);
			sunxi_board_shutdown();
		}
		if (ctrlc())
			break;
		mdelay(10);
	}
	return 0;
}

int sunxi_bat_key_handle(void)
{
	int dcin_exist   = bmu_get_axp_bus_exist();
	__maybe_unused char arg[3] = {0};
	while (pmu_get_key_irq() <= 0) {
		dcin_exist = bmu_get_axp_bus_exist();
		if (!dcin_exist) {
			tick_printf("dcin is out,to be shutdown\n");
			mdelay(3000);
			sunxi_board_shutdown();
		}
		if (ctrlc())
			break;
		mdelay(10);
	}
	return 0;
}

/* reset bat capacity when system is writing firmware*/
int axp_reset_capacity(void)
{
	return bmu_reset_capacity();
}

/* set dcdc pwm mode */
int axp_set_dcdc_mode(void)
{
	const struct fdt_property *prop;
	int nodeoffset = -1, offset, mode;
	const char *pname;
	const int *pdata;

	if (nodeoffset < 0) {
		nodeoffset = fdt_path_offset(working_fdt, FDT_PATH_POWER_SPLY);
	}

	for (offset = fdt_first_property_offset(working_fdt, nodeoffset);
	     offset > 0; offset = fdt_next_property_offset(working_fdt, offset)) {
		prop  = fdt_get_property_by_offset(working_fdt, offset, NULL);
		pname = fdt_string(working_fdt, fdt32_to_cpu(prop->nameoff));
		pdata = (const int *)prop->data;
		mode = fdt32_to_cpu(pdata[0]);
		if (strstr(pname, "mode") == NULL) {
			continue;
		}

		if (pmu_set_dcdc_mode(pname, mode) < 0) {
			debug("set %s fail!\n", pname);
		}
	}

	return 0;
}


int axp_battery_status_handle(void)
{
	int battery_status;
	int ret = 0, bat_exist = 0, ntc_status = -1, charge_mode;

	ret = script_parser_fetch(FDT_PATH_POWER_SPLY, "battery_exist", &bat_exist, 1);
	if (ret < 0)
		bat_exist = 1;

	if (!bat_exist)
		return 0;

	ret = bmu_get_battery_probe();
	if (ret < 1)
		return 0;

	ret = script_parser_fetch(FDT_PATH_POWER_SPLY, "ntc_status", &ntc_status, 0);
	if (ret < 0)
		ntc_status = 0;

	bmu_set_ntc_onoff(ntc_status);

	ret = script_parser_fetch(FDT_PATH_POWER_SPLY, "charge_mode", &charge_mode, 1);
	if (ret < 0)
		charge_mode = 1;

#ifdef CONFIG_AXP_LATE_INFO
	battery_status = axp_get_battery_status();
#else
	battery_status = gd->pmu_runtime_chgcur;
#endif
	if (gd->chargemode == 1) {
		if ((battery_status == BATTERY_RATIO_TOO_LOW_WITH_DCIN)
			|| (battery_status == BATTERY_RATIO_TOO_LOW_WITHOUT_DCIN)
			|| (battery_status == BATTERY_VOL_TOO_LOW)) {

#if defined(CONFIG_EINK200_SUNXI) || defined(CONFIG_CMD_SUNXI_BMP)
			sunxi_bmp_display("bat\\bat0.bmp");
#endif
#if 0
			tick_printf("battery ratio is low with dcin,to be shutdown\n");
			mdelay(3000);
			sunxi_board_shutdown();
#else
		sunxi_bat_low_vol_handle();
#endif
		} else if (charge_mode == 2) {
			tick_printf("press power_on to start up\n");
			sunxi_bat_key_handle();
		} else {
#if defined(CONFIG_EINK200_SUNXI) || defined(CONFIG_CMD_SUNXI_BMP)
			sunxi_bmp_display("bat\\battery_charge.bmp");
#endif
		}
	} else if ((battery_status == BATTERY_RATIO_TOO_LOW_WITHOUT_DCIN) || (battery_status == BATTERY_VOL_TOO_LOW)) {
#if defined(CONFIG_EINK200_SUNXI) || defined(CONFIG_CMD_SUNXI_BMP)
		sunxi_bmp_display("bat\\low_pwr.bmp");
#endif
		tick_printf("battery ratio or vol is low ,to be shutdown\n");
		mdelay(3000);
		sunxi_board_shutdown();
	}

	if (charge_mode == 2 || charge_mode == 0)
		gd->chargemode = 0;

	return 0;
}

int axp_set_vol(char *name, uint onoff)
{
	return pmu_set_voltage(name, 0, onoff);
}


int sunxi_update_axp_info(void)
{
	int val = -1;
	char bootreason[16] = {0};
	int ret = 0, bat_exist = 0;

	ret = script_parser_fetch(FDT_PATH_POWER_SPLY, "battery_exist", &bat_exist, 1);
	if (ret < 0)
		bat_exist = 1;

#ifdef CONFIG_SUNXI_BMU
#ifdef CONFIG_AXP_LATE_INFO
	val = bmu_get_poweron_source();
#else
	val = gd->pmu_saved_status;
#endif
#endif
	if ((val == -1) && (pmu_get_sys_mode() == SUNXI_CHARGING_FLAG)) {
		val = AXP_BOOT_SOURCE_CHARGER;
		pmu_set_sys_mode(0);
	}
	switch (val) {
	case AXP_BOOT_SOURCE_BUTTON:
		strncpy(bootreason, "button", sizeof("button"));
		break;
	case AXP_BOOT_SOURCE_IRQ_LOW:
		strncpy(bootreason, "irq", sizeof("irq"));
		break;
	case AXP_BOOT_SOURCE_VBUS_USB:
		strncpy(bootreason, "usb", sizeof("usb"));
		break;
	case AXP_BOOT_SOURCE_CHARGER:
		strncpy(bootreason, "charger", sizeof("charger"));
		if (bat_exist)
			gd->chargemode = 1;
		break;
	case AXP_BOOT_SOURCE_BATTERY:
		strncpy(bootreason, "battery", sizeof("battery"));
		break;
	default:
		strncpy(bootreason, "unknow", sizeof("unknow"));
		break;
	}
	env_set("bootreason", bootreason);
	return 0;
}


int do_sunxi_axp(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	u8 reg_addr;
	u8 reg_value;
	if (argc < 4)
		return -1;
	reg_addr = (u8)simple_strtoul(argv[3], NULL, 16);
	if (!strncmp(argv[1], "pmu", 3)) {
#ifdef CONFIG_SUNXI_PMU
		if (!strncmp(argv[2], "read", 4)) {
			printf("pmu_value:0x%x\n", pmu_get_reg_value(reg_addr));
		} else if (!strncmp(argv[2], "write", 5)) {
			reg_value = (u8)simple_strtoul(argv[4], NULL, 16);
			printf("pmu_value:0x%x\n", pmu_set_reg_value(reg_addr, reg_value));
		} else {
			printf("input error\n");
			return -1;
		}
#endif
	} else if (!strncmp(argv[1], "bmu", 3)) {
#ifdef CONFIG_SUNXI_BMU
		if (!strncmp(argv[2], "read", 4)) {
			printf("bmu_value:0x%x\n", bmu_get_reg_value(reg_addr));
		} else if (!strncmp(argv[2], "write", 5)) {
			reg_value = (u8)simple_strtoul(argv[4], NULL, 16);
			printf("bmu_value:0x%x\n", bmu_set_reg_value(reg_addr, reg_value));
		} else {
			printf("input error\n");
			return -1;
		}
#endif
	} else {
		printf("input error\n");
		return -1;
	}
	return 0;
}

U_BOOT_CMD(sunxi_axp, 6, 1, do_sunxi_axp, "sunxi_axp sub-system",
	"sunxi_axp <pmu/bmu> <read> <reg_addr>\n"
	"sunxi_axp <pmu/bmu> <write> <reg_addr> <reg_value>\n");


