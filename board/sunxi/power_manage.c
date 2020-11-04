/*
 * Copyright (C) 2019 Allwinner.
 * weidonghui <weidonghui@allwinnertech.com>
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#include <sunxi_power/power_manage.h>
#include <sys_config.h>
#include <sunxi_board.h>
/*
 * Global data (for the gd->bd)
 */
DECLARE_GLOBAL_DATA_PTR;

int axp_set_power_supply_output(void)
{
	int onoff;
	char power_name[32];
	int power_vol;
	int power_vol_d;
	int nodeoffset = -1, offset;
	const struct fdt_property *prop;
	const char *pname;
	const int *pdata;
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
	for (offset = fdt_first_property_offset(working_fdt, nodeoffset);
	     offset > 0; offset = fdt_next_property_offset(working_fdt, offset))

	{
		prop  = fdt_get_property_by_offset(working_fdt, offset, NULL);
		pname = fdt_string(working_fdt, fdt32_to_cpu(prop->nameoff));
		pdata = (const int *)prop->data;
		power_vol = fdt32_to_cpu(pdata[0]);
		memset(power_name, 0, sizeof(power_name));
		strcpy(power_name, pname);
		if ((strstr((const char *)power_name, "phandle") != NULL) ||
		    (strstr((const char *)power_name, "device_type") != NULL)) {
			continue;
		}

		onoff       = -1;
		power_vol_d = 0;

		if (power_vol > 10000) {
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

		pr_msg("%s = %d, onoff=%d\n", power_name, pmu_get_voltage(power_name), onoff);

	}

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
	int safe_vol = 0;
	dcin_exist   = bmu_get_axp_bus_exist();
	bat_vol      = bmu_get_battery_vol();
	script_parser_fetch(FDT_PATH_CHARGER0, "pmu_safe_vol", &safe_vol, 1);
	if (safe_vol < 3000) {
		safe_vol = 3500;
	}
	ratio = bmu_get_battery_capacity();
	debug("bat_vol=%d, ratio=%d\n", bat_vol, ratio);
	if (ratio < 1) {
		if (dcin_exist) {
			if (bat_vol < safe_vol) {
				return BATTERY_RATIO_TOO_LOW_WITH_DCIN_VOL_TOO_LOW;
			} else {
				return BATTERY_RATIO_TOO_LOW_WITH_DCIN;
			}
		} else {
			return BATTERY_RATIO_TOO_LOW_WITHOUT_DCIN;
		}
	} else {
		return BATTERY_RATIO_ENOUGH;
	}
}

int axp_set_vol(char *name, uint onoff)
{
	return pmu_set_voltage(name, 0, onoff);
}

int sunxi_update_axp_info(void)
{
	int val = -1;
	char bootreason[16] = {0};
#ifdef CONFIG_SUNXI_BMU
	val = bmu_get_poweron_source();
#endif
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


