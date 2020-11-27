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
#include <linux/ctype.h>
#include <linux/types.h>

#include <power/sunxi/power.h>
#include <power/sunxi/pmu.h>
#include <sys_config.h>
#include <sys_config_old.h>
#include <power/sunxi/axp.h>
#include <fdt_support.h>
#include <private_uboot.h>

DECLARE_GLOBAL_DATA_PTR;

extern int platform_axp_probe(sunxi_axp_dev_t* sunxi_axp_dev[], int max_dev);

 __attribute__((section(".data")))
static sunxi_axp_dev_t  *sunxi_axp_dev[SUNXI_AXP_DEV_MAX]  = {NULL};

__attribute__((section(".data")))
static ulong pmu_nodeoffset_in_config = 0;


#ifdef CONFIG_CHARGER_PMU
	#define CHARGER_PMU 1
#else
	#define CHARGER_PMU 0
#endif

void sunxi_axp_dummy_init(void)
{
	int i;

	pr_msg("probe axp is dummy\n");
	memset(sunxi_axp_dev, 0, SUNXI_AXP_DEV_MAX * 4);
	for (i = 0; i < SUNXI_AXP_DEV_MAX; i++) {
		sunxi_axp_dev[i] = &sunxi_axp_null;
	}
}

int axp_probe(void)
{
	int axp_num = 0;
	memset(sunxi_axp_dev, 0, SUNXI_AXP_DEV_MAX * 4);

	pmu_nodeoffset_in_config = script_parser_offset(PMU_SCRIPT_NAME);
	if(pmu_nodeoffset_in_config == 0 )
	{
		pr_msg("axp: get node[%s] error\n",PMU_SCRIPT_NAME);
	}

	axp_num = platform_axp_probe((sunxi_axp_dev_t* *)&sunxi_axp_dev,
		SUNXI_AXP_DEV_MAX);

	return axp_num;
}


int axp_reinit(void)
{
	int i;

	for(i=0;i<SUNXI_AXP_DEV_MAX;i++)
	{
		if(sunxi_axp_dev[i] != NULL)
		{
			sunxi_axp_dev[i] = (sunxi_axp_dev_t *)((ulong)sunxi_axp_dev[i] + gd->reloc_off);
		}
	}
	//pmu_nodeoffset = fdt_path_offset(working_fdt,PMU_SCRIPT_NAME);
	pmu_nodeoffset_in_config = script_parser_offset(PMU_SCRIPT_NAME);
	if(pmu_nodeoffset_in_config == 0)
	{
		pr_msg("axp: get node[%s] error\n",PMU_SCRIPT_NAME);
		return 0;
	}

	return 0;
}

int axp_get_power_vol_level(void)
{
	return gd->power_step_level;
}

int axp_probe_startup_cause(void)
{
	return sunxi_axp_dev[CHARGER_PMU]->probe_this_poweron_cause();
}

int axp_probe_factory_mode(void)
{
	int buffer_value, status;
	int poweron_reason;

	buffer_value = sunxi_axp_dev[0]->probe_pre_sys_mode();

	if(buffer_value == PMU_PRE_FACTORY_MODE)	//factory mode: need the power key and dc or vbus
	{
		pr_msg("factory mode detect\n");
		status = sunxi_axp_dev[CHARGER_PMU]->probe_power_status();
		if(status > 0)  //has the dc or vbus
		{
			poweron_reason = sunxi_axp_dev[CHARGER_PMU]->probe_this_poweron_cause();
			if(poweron_reason == AXP_POWER_ON_BY_POWER_KEY)
			{
				//set the system next powerom status as 0x0e(the system mode)
				pr_msg("factory mode release\n");
				sunxi_axp_dev[0]->set_next_sys_mode(PMU_PRE_SYS_MODE);
			}
			else
			{
				pr_msg("factory mode: try to poweroff without power key\n");
				axp_set_hardware_poweron_vol();  //poweroff
				axp_set_power_off();
				for(;;);
			}
		}
		else
		{
			pr_msg("factory mode: try to poweroff without power in\n");
			axp_set_hardware_poweroff_vol();  //poweroff
			axp_set_power_off();
			for(;;);
		}
	}

	return 0;
}

int axp_set_hardware_poweron_vol(void)
{
	int vol_value = 0;

	if(script_parser_fetch_by_offset(pmu_nodeoffset_in_config,"pmu_pwron_vol", (uint32_t*)&vol_value)<0)
	{
		pr_msg("set power on vol to default\n");
	}

	return sunxi_axp_dev[0]->set_power_onoff_vol(vol_value, 1);
}

int axp_set_hardware_poweroff_vol(void)
{
	int vol_value = 0;

	if(script_parser_fetch_by_offset(pmu_nodeoffset_in_config,"pmu_pwroff_vol", (uint32_t*)&vol_value)<0)
	{
		puts("set power off vol to default\n");
	}

	return sunxi_axp_dev[0]->set_power_onoff_vol(vol_value, 0);
}

int  axp_set_power_off(void)
{

#ifdef	CONFIG_SUNXI_AXP2585
	if (sunxi_axp_dev[CHARGER_PMU]->set_power_off()) {
		puts("bmu power off err\n");
		return -1;
	}
#endif
	if (sunxi_axp_dev[0]->set_power_off()) {
		puts("pmu power off err\n");
		return -1;
	}
	return 0;
}

int axp_set_next_poweron_status(int value)
{
	return sunxi_axp_dev[0]->set_next_sys_mode(value);
}

int axp_probe_pre_sys_mode(void)
{
#ifdef	CONFIG_SUNXI_AXP2585
	return sunxi_axp_dev[0]->probe_pre_sys_mode();
#else
	return sunxi_axp_dev[CHARGER_PMU]->probe_pre_sys_mode();
#endif
}

int  axp_power_get_dcin_battery_exist(int *dcin_exist, int *battery_exist)
{
	*dcin_exist    = sunxi_axp_dev[CHARGER_PMU]->probe_power_status();
	*battery_exist = sunxi_axp_dev[CHARGER_PMU]->probe_battery_exist();

	return 0;
}

int  axp_probe_battery_exist(void)
{
	return sunxi_axp_dev[CHARGER_PMU]->probe_battery_exist();

}

int axp_probe_power_source(void)
{
	int status = 0;
	status = sunxi_axp_dev[CHARGER_PMU]->probe_power_status();
	return status;
}

int  axp_probe_battery_vol(void)
{
	return sunxi_axp_dev[CHARGER_PMU]->probe_battery_vol();
}

int  axp_probe_rest_battery_capacity(void)
{
	return sunxi_axp_dev[CHARGER_PMU]->probe_battery_ratio();
}

int  axp_probe_key(void)
{
	return sunxi_axp_dev[0]->probe_key();
}

int  axp_probe_charge_current(void)
{
	return sunxi_axp_dev[CHARGER_PMU]->probe_charge_current();
}

int  axp_set_charge_current(int current)
{
	return sunxi_axp_dev[CHARGER_PMU]->set_charge_current(current);
}

int  axp_set_charge_control(void)
{
	return sunxi_axp_dev[CHARGER_PMU]->set_charge_control();
}

int axp_set_vbus_limit_dc(void)
{
	script_parser_fetch_by_offset(pmu_nodeoffset_in_config,
		"pmu_ac_vol", (uint32_t *)&gd->limit_vol);
	script_parser_fetch_by_offset(pmu_nodeoffset_in_config,
		"pmu_ac_cur", (uint32_t *)&gd->limit_cur);
	sunxi_axp_dev[CHARGER_PMU]->set_vbus_vol_limit(gd->limit_vol);
	sunxi_axp_dev[CHARGER_PMU]->set_vbus_cur_limit(gd->limit_cur);
	return 0;
}

int axp_set_vbus_limit_pc(void)
{
	script_parser_fetch_by_offset(pmu_nodeoffset_in_config,
		"pmu_usbpc_vol", (uint32_t *)&gd->limit_pcvol);
	script_parser_fetch_by_offset(pmu_nodeoffset_in_config,
		"pmu_usbpc_cur", (uint32_t *)&gd->limit_pccur);
	sunxi_axp_dev[CHARGER_PMU]->set_vbus_vol_limit(gd->limit_pcvol);
	sunxi_axp_dev[CHARGER_PMU]->set_vbus_cur_limit(gd->limit_pccur);
	return 0;
}

int axp_set_all_limit(void)
{

	axp_set_vbus_limit_dc();

	return 0;
}

int axp_set_suspend_chgcur(void)
{
	return sunxi_axp_dev[CHARGER_PMU]->set_charge_current(gd->pmu_suspend_chgcur);
}

int axp_set_runtime_chgcur(void)
{
	return sunxi_axp_dev[CHARGER_PMU]->set_charge_current(gd->pmu_runtime_chgcur);
}

int axp_set_charge_vol_limit(void)
{
	int ret1;
	int ret2;

	ret1 = script_parser_fetch_by_offset(pmu_nodeoffset_in_config,"pmu_runtime_chgcur", (uint32_t*)&gd->pmu_runtime_chgcur);
	ret2 = script_parser_fetch_by_offset(pmu_nodeoffset_in_config,"pmu_suspend_chgcur", (uint32_t*)&gd->pmu_suspend_chgcur);
	if(ret1)
	{
		gd->pmu_runtime_chgcur = 600;
	}
	if(ret2)
	{
		gd->pmu_suspend_chgcur = 1500;
	}

	debug("pmu_runtime_chgcur=%ld\n", gd->pmu_runtime_chgcur);
	debug("pmu_suspend_chgcur=%ld\n", gd->pmu_suspend_chgcur);

	axp_set_suspend_chgcur();

	return 0;
}

int axp_set_power_supply_output(void)
{
	int   onoff;
	int power_index1 = 0,power_index2 = 0, ret;
	char power_name[32];
	int  power_vol;
	int  power_vol_d;

	do
	{
		memset(power_name, 0, 16);
		ret = script_parser_fetch_subkey_next("power_sply", power_name, &power_vol, &power_index1,&power_index2);
		if(ret < 0)
		{
			pr_msg("find power_sply to end\n");
			return 0;
		}

		onoff = -1;
		power_vol_d = 0;

			if(power_vol > 10000)
			{
				onoff = 1;
				power_vol_d = power_vol%10000;
			}
			else if(power_vol >= 0)
			{
				onoff = 0;
				power_vol_d = power_vol;
			}

			pr_msg("%s = %d, onoff=%d\n", power_name, power_vol_d, onoff);

			if(sunxi_axp_dev[0]->set_supply_status_byname(power_name, power_vol_d, onoff))
			{
				pr_msg("axp set %s to %d failed\n", power_name, power_vol_d);
				return -1;
			}
	}while(1);

	return 0;
}

int axp_slave_set_power_supply_output(void)
{
	int  ret, onoff;
	char power_name[16];
	int  power_vol, power_index1 = 0,power_index2 = 0;
	int  index = -1;
	int  i;
	int  power_vol_d;

	for(i=1;i<SUNXI_AXP_DEV_MAX;i++)
	{
		if(sunxi_axp_dev[i] != NULL)
		{
			if(strcmp(sunxi_axp_dev[0]->pmu_name, sunxi_axp_dev[i]->pmu_name))
			{
				index = i;

				break;
			}
		}
	}
	if(index == -1)
	{
		pr_msg("unable to find slave pmu\n");
		return -1;
	}
	pr_msg("slave power\n");
	do
	{
		memset(power_name, 0, 16);
		ret = script_parser_fetch_subkey_next("slave_power_sply", power_name, &power_vol, &power_index1,&power_index2);
		if(ret < 0)
		{
		      pr_msg("find slave power sply to end\n");
		     return 0;
		}

		onoff = -1;
		power_vol_d = 0;
		if(power_vol > 10000)
		{
		    onoff = 1;
		    power_vol_d = power_vol%10000;
		}
#if defined(CONFIG_SUNXI_AXP_CONFIG_ONOFF)
		else if(power_vol > 0)
		{
			onoff = 0;
			power_vol_d = power_vol;
		}
#endif
		else if(power_vol == 0)
		{
			onoff = 0;
		}
#if defined(CONFIG_SUNXI_AXP_CONFIG_ONOFF)
		pr_msg("%s = %d, onoff=%d\n", power_name, power_vol_d, onoff);
#else
		pr_msg("%s = %d\n", power_name, power_vol_d);
#endif
		if(sunxi_axp_dev[index]->set_supply_status_byname(power_name, power_vol_d, onoff))
		{
		    pr_msg("axp set %s to %d failed\n", power_name, power_vol_d);
		}
	}while(1);

    return 0;
}

int axp_probe_power_supply_condition(void)
{
	int   dcin_exist, bat_vol;
	int   ratio;
	int   safe_vol;

	dcin_exist = sunxi_axp_dev[CHARGER_PMU]->probe_power_status();
	bat_vol = sunxi_axp_dev[CHARGER_PMU]->probe_battery_vol();

	safe_vol = 0;
	script_parser_fetch_by_offset(pmu_nodeoffset_in_config,"pmu_safe_vol", (uint32_t*)&safe_vol);

	if(safe_vol < 3000)
	{
		safe_vol = 3500;
	}

	ratio = sunxi_axp_dev[CHARGER_PMU]->probe_battery_ratio();
	pr_msg("bat_vol=%d, ratio=%d\n", bat_vol, ratio);
	if(ratio < 1)
	{
		if(dcin_exist)
		{
			if(bat_vol < safe_vol)
			{
				gd->power_step_level = BATTERY_RATIO_TOO_LOW_WITH_DCIN_VOL_TOO_LOW;
			}
			else
			{
				gd->power_step_level = BATTERY_RATIO_TOO_LOW_WITH_DCIN;
			}
		}
		else
		{
			gd->power_step_level = BATTERY_RATIO_TOO_LOW_WITHOUT_DCIN;
		}
	}
	else
	{
		gd->power_step_level = BATTERY_RATIO_ENOUGH;
	}

	return 0;
}



int axp_int_enable(__u8 *value)
{
	return 0;
}

int axp_int_disable(void)
{

	return 0;
}

int axp_int_query(__u8 *addr)
{
	return 0;
}

/*
pmu_type: 0- main pmu
*/
int axp_set_supply_status(int pmu_type, int vol_name, int vol_value, int onoff)
{
	return sunxi_axp_dev[pmu_type]->set_supply_status(vol_name, vol_value, onoff);
}

int axp_set_supply_status_byname(char *pmu_type, char *vol_type, int vol_value, int onoff)
{
	int axp_index = 0;

	if(0 == strcmp(AXP_SLAVE, pmu_type))
	{
		axp_index = AXP_TYPE_SLAVE;
	}
	else
	{
		axp_index = AXP_TYPE_MAIN;
	}
	if(sunxi_axp_dev[axp_index] != NULL)
		return sunxi_axp_dev[axp_index]->set_supply_status_byname(vol_type, vol_value, onoff);

	return -1;
}

int axp_probe_supply_status(int pmu_type, int vol_name, int vol_value)
{
	return sunxi_axp_dev[pmu_type]->probe_supply_status(vol_name, 0, 0);
}

int axp_probe_supply_status_byname(char *pmu_type, char *vol_type)
{
	int   i;

	for(i=0;i<SUNXI_AXP_DEV_MAX;i++)
	{
		if(sunxi_axp_dev[i] != NULL)
		{
			if(!strcmp(sunxi_axp_dev[i]->pmu_name, pmu_type))
			{
				return sunxi_axp_dev[i]->probe_supply_status_byname(vol_type);
			}
		}
	}

	return -1;
}

//example string  :    "axp81x_dcdc6 vdd-sys vdd-usb0-09 vdd-hdmi-09"
static int find_regulator_str(const char* src, const char* des)
{
	int i,len_src, len_des ;
	int token_index;
	len_src = strlen(src);
	len_des = strlen(des);

	if(len_des > len_src) return 0;

	token_index = 0;
	for(i =0 ; i < len_src+1; i++)
	{
		if(src[i]==' ' || src[i] == '\t' || src[i] == '\0')
		{
			if(i-token_index == len_des)
			{
				if(memcmp(src+token_index,des,len_des) == 0)
				{
					return 1;
				}
			}
			token_index = i+1;
		}
	}
	return 0;
}

int axp_set_supply_status_byregulator(const char* id, int onoff)
{
	char main_key[32];
	char pmu_type[32];
	char vol_type[32];
	char sub_key_name[32];
	char sub_key_value[256];
	int main_hd = 0;
	unsigned int i = 0, j = 0, index = 0;
	int ldo_count = 0;
	int find_flag = 0;
	int ret = 0;

	do
	{
		strcpy(main_key,FDT_PATH_REGU);
		main_hd = script_parser_fetch(main_key,"regulator_count",&ldo_count, 1);
		if (main_hd != 0)
		{
			pr_msg("unable to get ldo_count  from [%s] \n",main_key);
			break;
		}

		for (index = 1; index <= ldo_count; index++)
		{
			sprintf(sub_key_name, "regulator%u", index);
			main_hd = script_parser_fetch(main_key,sub_key_name,(int*)(sub_key_value), sizeof(sub_key_value)/sizeof(int));
			if (main_hd != 0)
			{
				pr_msg("unable to get subkey %s from [%s]\n",sub_key_name, main_key);
				break;
			}
			if (find_regulator_str(sub_key_value, id))
			{
				find_flag = 1;
				break;
			}
		}

		if (find_flag)
			break;

	} while(0);

	if (!find_flag)
	{
		pr_msg("unable to find regulator %s from [pmu1_regu] or [pmu2_regu] \n",id);
		return -1;
	}

	//example :    ldo6      = "axp81x_dcdc6 vdd-sys vdd-usb0-09 vdd-hdmi-09"
	memset(pmu_type, 0, sizeof(pmu_type));
	memset(vol_type, 0, sizeof(vol_type));
	//get pmu type
	for(j = 0,i =0; i < strlen(sub_key_value); i++)
	{
		if(sub_key_value[i] == '_')
		{
			i++;
			break;
		}
		pmu_type[j++] = sub_key_value[i];
	}
	//get vol type
	j = 0;
	for(; i < strlen(sub_key_value); i++)
	{
		if(sub_key_value[i] == ' ')
		{
		    break;
		}
		vol_type[j++]= sub_key_value[i];
	}

	//para vol = 0  indcate not set vol,only open or close  voltage switch
	ret = axp_set_supply_status_byname(pmu_type,vol_type,0, onoff);
	if(ret != 0)
	{
		pr_msg("error: supply regelator %s=id, pmu_type=%s_%s onoff=%d fail\n", id,pmu_type,vol_type,onoff );
	}

	return ret;
}

int axp_probe_supply_pmu_name(char *axpname)
{
	static int current_pmu = -1;
	int i = 0;
	if(axpname == NULL)
	{
		return -1;
	}
	for(i=0;i<SUNXI_AXP_DEV_MAX;i++)
	{
		if((sunxi_axp_dev[i] != NULL) && (current_pmu < i))
		{
			current_pmu = i;
			strcpy(axpname, sunxi_axp_dev[i]->pmu_name);
			return 0;
		}
	}
	return -1;

}

int axp_probe_vbus_cur_limit(void)
{
	return sunxi_axp_dev[0]->probe_vbus_cur_limit();
}

int axp_set_coulombmeter_onoff(int onoff )
{
	return sunxi_axp_dev[0]->set_coulombmeter_onoff(onoff);
}


__weak int axp_probe_vbus_type(void)
{
	return SUNXI_VBUS_PC;
}


#ifdef CONFIG_SUNXI_PIO_POWER_MODE
struct sunxi_bias_set {
	const char*  name;
	u32  base;
	u32  shift;
};

#define SUNXI_PIO_BIAS(_name, _shift)	\
	{				\
		.name = _name,		\
		.base = SUNXI_PIO_BASE,	\
		.shift = _shift,	\
	}
#define SUNXI_R_PIO_BIAS(_name, _shift)	\
	{				\
		.name = _name,		\
		.base = SUNXI_RPIO_BASE,\
		.shift = _shift,	\
	}

static const struct sunxi_bias_set gpio_bias_tlb[] = {
	SUNXI_PIO_BIAS("pa_bias", 0),

	SUNXI_PIO_BIAS("pc_bias", 2),
	SUNXI_PIO_BIAS("pd_bias", 3),
	SUNXI_PIO_BIAS("pe_bias", 4),
	SUNXI_PIO_BIAS("pf_bias", 5),
	SUNXI_PIO_BIAS("pg_bias", 6),

	SUNXI_PIO_BIAS("pi_bias", 8),
	SUNXI_PIO_BIAS("pj_bias", 9),

	SUNXI_PIO_BIAS("vcc_bias", 12),


	SUNXI_R_PIO_BIAS("pl_bias", 0),
	SUNXI_R_PIO_BIAS("pm_bias", 1),
};

static char* __parse_axp_name(char *gpio_bias, int *offset)
{
	char *axp = gpio_bias + *offset;

	while ( (axp[*offset]!=':') && (axp[*offset]!='\0') )
		(*offset)++;

	axp[(*offset)++]='\0';

	return axp;
}

static char* __parse_supply_name(char *gpio_bias,int *offset)
{
	char *supply = gpio_bias + *offset;
	while ( (gpio_bias[*offset]!=':') && (gpio_bias[*offset]!='\0') )
		(*offset)++;
	gpio_bias[(*offset)++]='\0';

	return supply;
}

static u32 __parse_setting_vol(char *vol, char *gpio_bias,int *offset)
{
	vol = gpio_bias + *offset;
	while ( (gpio_bias[*offset]!=':') && (gpio_bias[*offset]!='\0') )
		(*offset)++;
	return simple_strtoul(vol, NULL, 10);
}

static int __gpio_name_is_valid(char *gpio_name)
{
	int i,ret = -1;
	for (i = 0; i<ARRAY_SIZE(gpio_bias_tlb); i++) {
		if (!memcmp(gpio_bias_tlb[i].name,gpio_name,
		            strlen(gpio_name))) {
			ret = i;
			break;
		}
	}
	return ret;
}

static void __pio_power_mode_select(u32 vol, u32 index)
{
	int mode,val;

	if (vol <= 1800)
		mode = GPIO_1_8V_MODE;
	else
		mode = GPIO_3_3V_MODE;

	val = readl(gpio_bias_tlb[index].base + GPIO_POW_MODE_REG);
	val |= mode << gpio_bias_tlb[index].shift;
	writel(val, gpio_bias_tlb[index].base + GPIO_POW_MODE_REG);
}

int set_sunxi_gpio_power_bias(void)
{
	char gpio_bias[GPIO_BIAS_MAX_LEN], gpio_name[GPIO_BIAS_MAX_LEN];
	char *axp=NULL, *supply=NULL, *vol=NULL;
	u32 bias_vol_set;
	int port_index, supply_set;
	int  index1 = 0,index2 = 0,offset = 0;

	do {
		offset = 0;
		memset(gpio_bias, 0, GPIO_BIAS_MAX_LEN);
		memset(gpio_name, 0, GPIO_BIAS_MAX_LEN);

		if (script_parser_fetch_subkey_next(GPIO_BIAS_MAIN_NAME,
		    gpio_name, (int *)gpio_bias, &index1,&index2))
				break;

			port_index =__gpio_name_is_valid(gpio_name);
			if (port_index < 0) {
				pr_msg("Unsupport to set gpio %s\n",gpio_name);
				break;
			}

		axp = __parse_axp_name(gpio_bias, &offset);
		supply = __parse_supply_name(gpio_bias, &offset);
		bias_vol_set = __parse_setting_vol(vol, gpio_bias, &offset);
		supply_set = 0;
		if (bias_vol_set > 10000) {
			supply_set = 1;
			bias_vol_set = bias_vol_set%10000;
		} else if (bias_vol_set >= 0) {
			supply_set = 0;

		}
		pr_msg("set %s(%d) bias:%d\n", gpio_name,
					port_index, bias_vol_set);
		__pio_power_mode_select(bias_vol_set, port_index);
		if (uboot_spare_head.boot_ext[0].data[0]) {
			if (supply_set) {
				pr_msg("axp=%s, supply=%s, vol=%d\n", axp,
						supply, bias_vol_set);
				if (axp_set_supply_status_byname(axp, supply,
						bias_vol_set, 1) < 0)
				pr_msg("pmu set fail!\n");
			}
		}
	} while (1);

		return 0;
}
#endif /*CONFIG_SUNXI_PIO_POWER_MODE*/

