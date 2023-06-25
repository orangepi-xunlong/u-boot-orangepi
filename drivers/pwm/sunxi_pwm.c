/*
 * (C) Copyright 2012
 *     tyle@allwinnertech.com
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program;
 *
 */
#include <asm/arch-sunxi/cpu.h>
#include <asm/io.h>
#include <common.h>
#include <asm/arch/pwm.h>
#include <sys_config.h>
#include <pwm.h>
#include <fdt_support.h>
#include <malloc.h>
#include "sunxi_pwm.h"

#define sys_get_wvalue(n)   (*((volatile uint *)(n)))          /* word input */
#define sys_put_wvalue(n, c) (*((volatile uint *)(n))  = (c))   /* word output */
#ifndef abs
#define abs(x) (((x) & 0x80000000) ? (0 - (x)) : (x))
#endif

uint pwm_active_sta[4] = {1, 0, 0, 0};

#define sunxi_pwm_debug 0
#undef  sunxi_pwm_debug

#ifdef sunxi_pwm_debug
	#define pwm_debug(fmt, args...)	printf(fmt, ##args)
#else
	#define pwm_debug(fmt, args...)
#endif

#define PWM_PIN_STATE_ACTIVE "active"
#define PWM_PIN_STATE_SLEEP "sleep"

#define PWM_CCU_OFFSET		0x7ac
#define PWM_RCM_OFFSET		0x13c

#define SETMASK(width, shift)   ((width ? ((-1U) >> (32 - width)) : 0)  << (shift))
#define CLRMASK(width, shift)   (~(SETMASK(width, shift)))
#define GET_BITS(shift, width, reg)     \
	    (((reg) & SETMASK(width, shift)) >> (shift))
#define SET_BITS(shift, width, reg, val) \
	    (((reg) & CLRMASK(width, shift)) | (val << (shift)))

#define CLK_CCU_SUPPORT
uint clk_count;
#if !defined CONFIG_ARCH_SUN8IW20 && !defined CONFIG_ARCH_SUN20IW1
#define CLK_RCM_SUPPORT
uint sclk_count;
#endif

#if 0
//legacy pwm config
struct sunxi_pwm_cfg {
	unsigned int reg_busy_offset;
	unsigned int reg_busy_shift;
	unsigned int reg_enable_offset;
	unsigned int reg_enable_shift;
	unsigned int reg_clk_gating_offset;
	unsigned int reg_clk_gating_shift;
	unsigned int reg_bypass_offset;
	unsigned int reg_bypass_shift;
	unsigned int reg_pulse_start_offset;
	unsigned int reg_pulse_start_shift;
	unsigned int reg_mode_offset;
	unsigned int reg_mode_shift;
	unsigned int reg_polarity_offset;
	unsigned int reg_polarity_shift;
	unsigned int reg_period_offset;
	unsigned int reg_period_shift;
	unsigned int reg_period_width;
	unsigned int reg_active_offset;
	unsigned int reg_active_shift;
	unsigned int reg_active_width;
	unsigned int reg_prescal_offset;
	unsigned int reg_prescal_shift;
	unsigned int reg_prescal_width;
};
#endif

struct sunxi_pwm_chip {
	//struct pwm_chip chip;
	struct list_head	   list;
	const struct pwm_ops	*ops;
	unsigned int            base;
	int                     pwm;
	int			pwm_base;
	struct sunxi_pwm_cfg *config;
};

static LIST_HEAD(pwm_list);

static inline struct sunxi_pwm_chip *to_sunxi_pwm_chip(struct pwm_chip *chip)
{
	//return container_of(chip, struct sunxi_pwm_chip, chip);
	return 0;
}

static inline u32 sunxi_pwm_readl(struct sunxi_pwm_chip *chip, u32 offset)
{
	unsigned int value;

	value = readl((void *)(unsigned long)(chip->base + offset));

	return value;
}

static inline u32 sunxi_pwm_writel(struct sunxi_pwm_chip *chip, u32 offset, u32 value)
{
	writel(value, (void *)(unsigned long)(chip->base + offset));

	return 0;
}

extern int fdt_set_all_pin(const char *node_path, const char *pinctrl_name);
static int sunxi_pwm_pin_set_state(char *dev_name, char *name)
{
	char compat[32];
	u32 len = 0;
	int state = 0;
	int ret = -1;

	if (!strcmp(name, PWM_PIN_STATE_ACTIVE))
		state = 1;
	else
		state = 0;

	len = sprintf(compat, "/soc/%s", dev_name);
	if (len > 32)
		printf("disp_sys_set_state, size of mian_name is out of range\n");

	ret = fdt_set_all_pin(compat, (state == 1) ? "pinctrl-0" : "pinctrl-1");
	if (ret != 0)
		printf("%s, fdt_set_all_pin, ret=%d\n", __func__, ret);

	return ret;
}

int sunxi_pwm_set_polarity(struct sunxi_pwm_chip *pchip, enum pwm_polarity polarity)
{
	uint sel;
	uint temp;

	unsigned int reg_offset, reg_shift;

	sel = pchip->pwm;
	reg_offset = PWM_PCR_BASE + sel * 0x20;
	reg_shift = PWM_ACT_STA_SHIFT;

	temp = sunxi_pwm_readl(pchip, reg_offset);

	if (polarity == PWM_POLARITY_NORMAL)
		temp = SET_BITS(reg_shift, 1, temp, 1);
	else
		temp = SET_BITS(reg_shift, 1, temp, 0);

	sunxi_pwm_writel(pchip, reg_offset, temp);
/*
    temp = sunxi_pwm_read_reg(0);
    if(polarity == PWM_POLARITY_NORMAL) {
	pwm_active_sta[pwm] = 1;
	if(pwm == 0)
	    temp |= 1 << 5;
	else
	    temp |= 1 << 20;
	}else {
	    pwm_active_sta[pwm] = 0;
	    if(pwm == 0)
		temp &= ~(1 << 5);
	    else
		temp &= ~(1 << 20);
	    }

	sunxi_pwm_write_reg(0, temp);
*/
    return 0;
}

int sunxi_pwm_config(struct sunxi_pwm_chip *pchip, int duty_ns, int period_ns)
{

#if defined(CONFIG_MACH_SUN8IW20) || defined(CONFIG_MACH_SUN20IW1)
	uint pre_scal[][2] = {
		{0, 1},
		{1, 2},
		{2, 4},
		{3, 8},
		{4, 16},
		{5, 32},
		{6, 64},
		{7, 128},
		{8, 256},
	};
#else

	uint pre_scal[11][2] = {{15, 1}, {0, 120}, {1, 180}, {2, 240}, {3, 360}, {4, 480},
		{8, 12000}, {9, 24000}, {10, 36000}, {11, 48000}, {12, 72000} };
#endif

	uint freq;
	uint pre_scal_id = 0;
	uint entire_cycles = 256;
	uint active_cycles = 192;
	uint entire_cycles_max = 65536;
	uint temp;
	uint sel = pchip->pwm;
	unsigned int reg_offset, reg_shift, reg_width;

	reg_offset = PCGR;
	reg_shift = sel + PWM_CLK_BYPASS_SHIFT;

	if (period_ns < 42) {
		/* if freq lt 24M, then direct output 24M clock */
		temp = sunxi_pwm_readl(pchip, reg_offset);
		//temp |= (0x1 << reg_shift);//pwm bypass

		temp = SET_BITS(reg_shift, 1, temp, 1);
		sunxi_pwm_writel(pchip, reg_offset, temp);

		return 0;
	}

	/* disable bypass function */
	temp = sunxi_pwm_readl(pchip, reg_offset);
	temp = SET_BITS(reg_shift, 1, temp, 0);
	sunxi_pwm_writel(pchip, reg_offset, temp);

	if (period_ns < 10667)
		freq = 93747;
	else if (period_ns > 1000000000)
		freq = 1;
	else
		freq = 1000000000 / period_ns;

	entire_cycles = 24000000 / freq / pre_scal[pre_scal_id][1];

	while (entire_cycles > entire_cycles_max) {
		pre_scal_id++;

		if (pre_scal_id > (ARRAY_SIZE(pre_scal) - 1))
			break;

		entire_cycles = 24000000 / freq / pre_scal[pre_scal_id][1];
	}

	if (period_ns < 5 * 100 * 1000)
		active_cycles = (duty_ns * entire_cycles + (period_ns / 2)) / period_ns;
	else if (period_ns >= 5 * 100 * 1000 && period_ns < 6553500)
		active_cycles = ((duty_ns / 100) * entire_cycles + (period_ns / 2 / 100)) / (period_ns / 100);
	else
		active_cycles = ((duty_ns / 10000) * entire_cycles + (period_ns / 2 / 10000)) / (period_ns / 10000);

	reg_offset = PWM_PCR_BASE + 0x20 * sel;
	reg_shift = PWM_PRESCAL_SHIFT;
	reg_width = PWM_PRESCAL_WIDTH;
	temp = sunxi_pwm_readl(pchip, reg_offset);

	temp = SET_BITS(reg_shift, reg_width, temp, (pre_scal[pre_scal_id][0]));

	pwm_debug("%s: reg_shift = %d, reg_width = %d, prescale temp = %x, pres=%d\n",
		  __func__, reg_shift, reg_width, temp, pre_scal[pre_scal_id][0]);
	sunxi_pwm_writel(pchip, reg_offset, temp);

	/*config active cycles*/
	reg_offset = PWM_PPR_BASE + 0x20 * sel;
	reg_shift = PWM_ACT_CYCLES_SHIFT;
	reg_width = PWM_ACT_CYCLES_WIDTH;
	temp = sunxi_pwm_readl(pchip, reg_offset);
	temp = SET_BITS(reg_shift, reg_width, temp, active_cycles);
	sunxi_pwm_writel(pchip, reg_offset, temp);

	/*config period cycles*/
	reg_offset = PWM_PPR_BASE + 0x20 * sel;
	reg_shift = PWM_PERIOD_CYCLES_SHIFT;
	reg_width = PWM_PERIOD_CYCLES_WIDTH;
	temp = sunxi_pwm_readl(pchip, reg_offset);
	temp = SET_BITS(reg_shift, reg_width, temp, (entire_cycles - 1));
	sunxi_pwm_writel(pchip, reg_offset, temp);

	pwm_debug("PWM %s: duty_ns=%d, period_ns=%d, freq=%d, per_scal=%d, period_reg=0x%x\n",
		  __func__, duty_ns, period_ns, freq, pre_scal_id, temp);
	/*
	   if(pwm == 0)
	   temp = (temp & 0xfffffff0) |pre_scal[pre_scal_id][0];
	   else
	   temp = (temp & 0xfff87fff) |pre_scal[pre_scal_id][0];

	   sunxi_pwm_write_reg(0, temp);

	   sunxi_pwm_write_reg((pwm + 1)  * 0x04, ((entire_cycles - 1)<< 16) | active_cycles);

	   pwm_debug("PWM _TEST: duty_ns=%d, period_ns=%d, freq=%d, per_scal=%d, period_reg=0x%x\n", duty_ns, period_ns, freq, pre_scal_id, temp);
	   */
	return 0;
}

int sunxi_pwm_enable(struct sunxi_pwm_chip *pchip)
{
	int value;
	char pin_name[6];
	unsigned int reg_offset, reg_shift;
	int ret = 0;
	int pwm = pchip->pwm;
	int base = pchip->pwm_base;

	/*active pin config.*/
	if (base > 0)
		snprintf(pin_name, 6, "spwm%d", pwm - base);
	else
		snprintf(pin_name, 6,  "pwm%d", pwm);
	ret = sunxi_pwm_pin_set_state(pin_name, PWM_PIN_STATE_ACTIVE);

	/* enable clk for pwm controller. */
	reg_offset = PCGR;
	reg_shift = pwm;
	value = sunxi_pwm_readl(pchip, reg_offset);
	value = SET_BITS(reg_shift, 1, value, 1);
	sunxi_pwm_writel(pchip, reg_offset, value);

	/* enable pwm controller. */
	reg_offset = PWM_PER;
	reg_shift = pwm;
	value = sunxi_pwm_readl(pchip, reg_offset);
	value = SET_BITS(reg_shift, 1, value, 1);
	sunxi_pwm_writel(pchip, reg_offset, value);

	return ret;
}

void sunxi_pwm_disable(struct sunxi_pwm_chip *pchip)
{
    uint temp;
	int pwm, base;
	unsigned int reg_offset, reg_shift;
	char pin_name[6];

	base = pchip->pwm_base;
	pwm = pchip->pwm;

	/* disable pwm controller. */
	reg_offset = PWM_PER;
	reg_shift = pwm;
	temp = sunxi_pwm_readl(pchip, reg_offset);
	temp = SET_BITS(reg_shift, 1, temp, 0);
	sunxi_pwm_writel(pchip, reg_offset, temp);

	/* disable clk for pwm controller. */
	reg_offset =  PCGR;
	reg_shift = pwm;
	temp = sunxi_pwm_readl(pchip, reg_offset);
	temp = SET_BITS(reg_shift, 1, temp, 0);
	sunxi_pwm_writel(pchip, reg_offset, temp);

	/* disable pin config. */
	if (base > 0) {
		snprintf(pin_name, 6, "spwm%d", pwm - base);
#if defined CLK_RCM_SUPPORT
		sclk_count--;
#endif
	} else {
		snprintf(pin_name, 6, "pwm%d", pwm);
#if defined CLK_CCU_SUPPORT
		clk_count--;
#endif
	}
	sunxi_pwm_pin_set_state(pin_name, PWM_PIN_STATE_SLEEP);
}

void sunxi_pwm_init(void)
{
}

struct pwm_ops {
	void			(*free)(struct sunxi_pwm_chip *pchip);
	int				(*config)(struct sunxi_pwm_chip *pchip, int duty_ns, int period_ns);
	int				(*set_polarity)(struct sunxi_pwm_chip *pchip, enum pwm_polarity polarity);
	int				(*enable)(struct sunxi_pwm_chip *pchip);
	void			(*disable)(struct sunxi_pwm_chip *pchip);
};

static struct pwm_ops sunxi_pwm_ops = {
	.config = sunxi_pwm_config,
	.enable = sunxi_pwm_enable,
	.disable = sunxi_pwm_disable,
	.set_polarity = sunxi_pwm_set_polarity,
};

int pwm_config(int pwm, int duty_ns, int period_ns)
{
	struct sunxi_pwm_chip *pchip = NULL;

	list_for_each_entry(pchip, &pwm_list, list) {
		if (pchip->pwm == pwm) {
			if (pchip->ops->config)
				return pchip->ops->config(pchip, duty_ns, period_ns);
		}
	}

	return -1;
}

int pwm_enable(int pwm)
{
	struct sunxi_pwm_chip *pchip = NULL;

	list_for_each_entry(pchip, &pwm_list, list) {
		if (pchip->pwm == pwm) {
			if (pchip->ops->enable)
				return pchip->ops->enable(pchip);
		}
	}

	return -1;
}

int pwm_disable(int pwm)
{
	struct sunxi_pwm_chip *pchip = NULL;

	list_for_each_entry(pchip, &pwm_list, list) {
		if (pchip->pwm == pwm) {
			if (pchip->ops->disable)
				pchip->ops->disable(pchip);
		}
	}

	return -1;
}

int pwm_set_polarity(int pwm, enum pwm_polarity polarity)
{
	struct sunxi_pwm_chip *pchip = NULL;

		list_for_each_entry(pchip, &pwm_list, list) {
			if (pchip->pwm == pwm) {
				if (pchip->ops->set_polarity)
					pchip->ops->set_polarity(pchip, polarity);
			}
		}

	return -1;
}

int pwm_init(void)
{
	return 0;
}

int pwm_request(int pwm, const char *label)
{
	__attribute__((unused)) unsigned int reg_val = 0;
	int ret = 0;
	int node, sub_node;
	char main_name[20], sub_name[25];
	int pwm_base = 0;
	int pwm_number = 0;
	int handle_num = 0;
	unsigned int handle[16] = {0};
	struct sunxi_pwm_chip *pchip;

	list_for_each_entry(pchip, &pwm_list, list) {
		if (pchip->pwm == pwm) {
			printf("%s: err:this pwm has been requested!\n", __func__);
			return -1;
		}
	}

	/* get pwm config.*/
	pchip = malloc(sizeof(*pchip));
	if (!pchip) {
		printf("%s: error:pwm chip malloc failed!\n", __func__);
		return -1;
	} else {
		memset(pchip, 0, sizeof(*pchip));
	}

	sprintf(main_name, "pwm");
	sprintf(sub_name, "pwm-base");
	node = fdt_path_offset(working_fdt, main_name);
	if (node < 0) {
		printf("error:fdt err returned %s\n", fdt_strerror(node));
		goto err_pwm;
	}

	ret = fdt_getprop_u32(working_fdt, node, sub_name, (uint32_t *)&pwm_base);
	if (ret < 0) {
		printf("fdt_getprop_u32 %s.%s fail\n", main_name, sub_name);
		goto err_pwm;
	}

	sprintf(sub_name, "pwm-number");
	ret = fdt_getprop_u32(working_fdt, node, sub_name, (uint32_t *)&pwm_number);
	if (ret < 0) {
		printf("fdt_getprop_u32 %s.%s fail\n", main_name, sub_name);
		goto err_pwm;
	}
#if defined(CLK_CCU_SUPPORT)
	clk_count++;
	if (clk_count == 1) {
		reg_val |= 1 << 16;
		reg_val |= 1;
		writel(reg_val, (void *)(SUNXI_CCM_BASE + PWM_CCU_OFFSET));
	}
#endif
	/* pwm is included is in pwm area.*/
	if (pwm >= pwm_base && pwm < (pwm_base + pwm_number)) {
		/* get handle in pwm. */
		handle_num = fdt_getprop_u32(working_fdt, node, "pwms", handle);
		if (handle_num < 0) {
			handle_num = fdt_getprop_u32(working_fdt, node, "sunxi-pwms", handle);
			if (handle_num < 0) {
				printf("%s:%d:error:get property handle %s error:%s\n",
					__func__, __LINE__, "clocks",
						fdt_strerror(handle_num));
				goto err_pwm;
			}
		}
		sprintf(sub_name, "pwm%d", pwm);
	} else {
		/* pwm is included is not  in pwm area,then find spwm area.*/
		sprintf(main_name, "s_pwm");
		sprintf(sub_name, "pwm-base");

		node = fdt_path_offset(working_fdt, main_name);
		ret = fdt_getprop_u32(working_fdt, node, sub_name, (uint32_t *)&pwm_base);
		if (ret < 0) {
			printf("fdt_getprop_u32 %s.%s fail\n", main_name, sub_name);
			goto err_pwm;
		}

		sprintf(sub_name, "pwm-number");
		ret = fdt_getprop_u32(working_fdt, node, sub_name, (uint32_t *)&pwm_number);
		if (ret < 0) {
			printf("fdt_getprop_u32 %s.%s fail\n", main_name, sub_name);
			goto err_pwm;
		} else
			printf("%s:pwm number = %d\n", __func__, pwm_number);
#if defined CLK_RCM_SUPPORT
		sclk_count++;
		if (sclk_count == 1) {
			reg_val |= 1 << 16;
			reg_val |= 1;
			writel(reg_val, (void *)(SUNXI_PRCM_BASE + PWM_RCM_OFFSET));
		}
#endif
		if (pwm >= pwm_base && pwm < (pwm_base + pwm_number)) {
		/* get handle in pwm. */
			handle_num = fdt_getprop_u32(working_fdt, node, "pwms", handle);
			if (handle_num < 0) {
				handle_num = fdt_getprop_u32(working_fdt, node, "sunxi-pwms", handle);
				if (handle_num < 0) {
					printf("%s:%d:error:get property handle %s error:%s\n",
						__func__, __LINE__, "clocks", fdt_strerror(handle_num));
					goto err_pwm;
				}
			}
		} else {
			printf("the pwm id is wrong,none pwm in dts.\n");
			goto err_pwm;
		}
		sprintf(sub_name, "spwm%d", pwm - pwm_base);
	}

	pchip->pwm_base = pwm_base;

	sub_node = fdt_path_offset(working_fdt, sub_name);
	if (sub_node < 0) {
		printf("error:fdt %s err returned %s\n", sub_name,
		       fdt_strerror(sub_node));
		goto err_pwm;
	}

	/*pchip->config = (struct sunxi_pwm_cfg *)malloc(sizeof(struct sunxi_pwm_cfg));
	if (!pchip->config) {
		printf("%s: error:pwm chip malloc failed!\n", __func__);
		goto err_pwm;
	} else {
		memset(pchip->config, 0, sizeof(struct sunxi_pwm_cfg));
	}*/
	pchip->pwm = pwm;
	pchip->ops = &sunxi_pwm_ops;

	ret = fdt_getprop_u32(working_fdt, sub_node, "reg_base", &pchip->base);
	if (ret < 0) {
		printf("%s: err: get reg-base err.\n", __func__);
		goto err_pwm;
	}

	//legacy function
	/* ret = sunxi_pwm_get_config(sub_node, pchip->config); */

	list_add_tail(&pchip->list, &pwm_list);

	printf("request pwm success, %s:pwm%d:0x%x.\n", sub_name, pwm,
	       pchip->base);

	return pwm;

err_pwm:
	if (pchip)
		free(pchip);
	return -1;
}

