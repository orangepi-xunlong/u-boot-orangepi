/*
 * (C) Copyright 2016
 *     zengqi@allwinnertech.com
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
#include <div64.h>
#include <malloc.h>
#include "sunxi_pwm_enhance.h"

#define sys_get_wvalue(n)   (*((volatile uint *)(n)))          /* word input */
#define sys_put_wvalue(n, c) (*((volatile uint *)(n))  = (c))   /* word output */
#ifndef abs
#define abs(x) (((x) & 0x80000000) ? (0 - (x)) : (x))
#endif

uint pwm_active_sta[4] = {1, 0, 0, 0};
uint pwm_pin_count[4] = {0};

#define sunxi_pwm_debug 0
#undef  sunxi_pwm_debug

#ifdef sunxi_pwm_debug
	#define pwm_debug(fmt, args...)	printf(fmt, ##args)
#else
	#define pwm_debug(fmt, args...)
#endif

#define PWM_PIN_STATE_ACTIVE "active"
#define PWM_PIN_STATE_SLEEP "sleep"

#define SETMASK(width, shift)   ((width ? ((-1U) >> (32 - width)) : 0)  << (shift))
#define CLRMASK(width, shift)   (~(SETMASK(width, shift)))
#define GET_BITS(shift, width, reg)     \
	    (((reg) & SETMASK(width, shift)) >> (shift))
#define SET_BITS(shift, width, reg, val) \
	    (((reg) & CLRMASK(width, shift)) | (val << (shift)))

#if ((defined CONFIG_MACH_SUN8IW12) ||\
		(defined CONFIG_MACH_SUN8IW17) ||\
		(defined CONFIG_MACH_SUN50IW6) ||\
		(defined CONFIG_MACH_SUN50IW3) ||\
		(defined CONFIG_MACH_SUN8IW18) ||\
		(defined CONFIG_MACH_SUN8IW16) ||\
		(defined CONFIG_MACH_SUN8IW19) ||\
		(defined CONFIG_MACH_SUN50IW9) ||\
		(defined CONFIG_MACH_SUN50IW5))
#define CLK_GATE_SUPPORT
uint clk_count;
uint sclk_count;
#endif

struct sunxi_pwm_config {
	//unsigned int dead_time;
	//unsigned int bind_pwm;
	unsigned int clk_bypass_output;
};

struct sunxi_pwm_chip {
	struct list_head	list;
	const struct pwm_ops	*ops;
	unsigned int            base;
	int                     pwm;
	int			pwm_base;
	struct sunxi_pwm_config *config;
};

static LIST_HEAD(pwm_list);

static inline struct sunxi_pwm_chip *to_sunxi_pwm_chip(struct pwm_chip *chip)
{
	return 0;
}

static inline u32 sunxi_pwm_readl(struct sunxi_pwm_chip *chip, u32 offset)
{
	unsigned int value;

	value = readl((unsigned long)chip->base + offset);

	return value;
}

static inline u32 sunxi_pwm_writel(struct sunxi_pwm_chip *chip, u32 offset, u32 value)
{
	writel(value, chip->base + offset);

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
		pr_msg("disp_sys_set_state, size of mian_name is out of range\n");

	ret = fdt_set_all_pin(compat, (state == 1) ? "pinctrl-0" : "pinctrl-1");
	if (ret != 0)
		pr_msg("%s, fdt_set_all_pin, ret=%d\n", __func__, ret);

	return ret;
}

int sunxi_pwm_set_polarity(struct sunxi_pwm_chip *pchip, enum pwm_polarity polarity)
{
	u32 temp;
	u32 sel;
	unsigned int reg_offset, reg_shift, reg_width;

	sel = pchip->pwm - pchip->pwm_base;
	reg_offset = PWM_PCR_BASE + sel * 0x20;
	reg_shift = PWM_ACT_STA_SHIFT;
	reg_width = PWM_ACT_STA_WIDTH;
	temp = sunxi_pwm_readl(pchip, reg_offset);
	if (polarity == PWM_POLARITY_NORMAL)
		temp = SET_BITS(reg_shift, reg_width, temp, 0);
	else
		temp = SET_BITS(reg_shift, reg_width, temp, 1);

	sunxi_pwm_writel(pchip, reg_offset, temp);

	return 0;
}

static u32 get_pccr_reg_offset(u32 sel, u32 *reg_offset)
{
	switch (sel) {
	case 0:
	case 1:
		*reg_offset = PWM_PCCR01;
		break;
	case 2:
	case 3:
		*reg_offset = PWM_PCCR23;
		break;
	case 4:
	case 5:
		*reg_offset = PWM_PCCR45;
		break;
	case 6:
	case 7:
		*reg_offset = PWM_PCCR67;
		break;
	case 8:
		*reg_offset = PWM_PCCR8;
		break;
	default:
		pr_error("%s:Not supported!\n", __func__);
		break;
	}
	return 0;
}

#define PRESCALE_MAX 256

int sunxi_pwm_config(struct sunxi_pwm_chip *pchip, int duty_ns, int period_ns)
{
	unsigned int temp;
	unsigned long long c = 0;
	unsigned long entire_cycles = 256, active_cycles = 192;
	unsigned int reg_offset, reg_shift, reg_width;
	unsigned int reg_bypass_shift;
	unsigned int reg_clk_src_shift, reg_clk_src_width;
	unsigned int reg_div_m_shift, reg_div_m_width;
	unsigned int pre_scal_id = 0, div_m = 0, prescale = 0;
	u32 sel = 0;
	u32 pre_scal[][2] = {
		/* reg_value  clk_pre_div */
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

	sel = pchip->pwm;
	get_pccr_reg_offset(sel, &reg_offset);
	if ((sel % 2) == 0)
		reg_bypass_shift = 0x5;
	else
		reg_bypass_shift = 0x6;
	/*src clk reg*/
	reg_clk_src_shift = PWM_CLK_SRC_SHIFT;
	reg_clk_src_width = PWM_CLK_SRC_WIDTH;

	if (period_ns > 0 && period_ns <= 10) {
		/* if freq lt 100M, then direct output 100M clock,set by pass. */
		c = 100000000;
		temp = sunxi_pwm_readl(pchip, reg_offset);
		temp = SET_BITS(reg_bypass_shift, 1, temp, 1);
		sunxi_pwm_writel(pchip, reg_offset, temp);
		/*clk_src_reg*/
		temp = sunxi_pwm_readl(pchip, reg_offset);
		temp = SET_BITS(reg_clk_src_shift, reg_clk_src_width, temp, 1);
		sunxi_pwm_writel(pchip, reg_offset, temp);

		return 0;
	} else if (period_ns > 10 && period_ns <= 334) {
		/* if freq between 3M~100M, then select 100M as clock */
		c = 100000000;
		/*set clk bypass_output reg to 1 when pwm is used as the internal clock source.*/
		if (pchip->config->clk_bypass_output == 1) {
			temp = sunxi_pwm_readl(pchip, reg_offset);
			temp = SET_BITS(reg_bypass_shift, 1, temp, 1);
			sunxi_pwm_writel(pchip, reg_offset, temp);
		}

		/*clk_src_reg*/

		temp = sunxi_pwm_readl(pchip, reg_offset);
		temp = SET_BITS(reg_clk_src_shift, reg_clk_src_width, temp, 1);
		sunxi_pwm_writel(pchip, reg_offset, temp);
	} else if (period_ns > 334) {
		/* if freq < 3M, then select 24M clock */
		c = 24000000;
		/*set clk bypass_output reg to 1 when pwm is used as the internal clock source.*/
		if (pchip->config->clk_bypass_output == 1) {
			temp = sunxi_pwm_readl(pchip, reg_offset);
			temp = SET_BITS(reg_bypass_shift, 1, temp, 1);
			sunxi_pwm_writel(pchip, reg_offset, temp);
		}
		/*clk_src_reg*/

		temp = sunxi_pwm_readl(pchip, reg_offset);
		temp = SET_BITS(reg_clk_src_shift, reg_clk_src_width, temp, 0);
		sunxi_pwm_writel(pchip, reg_offset, temp);
	}

	c = c * period_ns;
	do_div(c, 1000000000);
	entire_cycles = (unsigned long)c;

	for (pre_scal_id = 0; pre_scal_id < 9; pre_scal_id++) {
		if (entire_cycles <= 65536)
				break;
		for (prescale = 0; prescale < PRESCALE_MAX + 1; prescale++) {
			entire_cycles = (entire_cycles / pre_scal[pre_scal_id][1]) / (prescale + 1);
			if (entire_cycles <= 65536) {
				div_m = pre_scal[pre_scal_id][0];
				break;
			}
		}
	}

	c = (unsigned long long)entire_cycles * duty_ns;
	do_div(c, period_ns);
	active_cycles = c;
	if (entire_cycles == 0)
		entire_cycles++;

	/* config  clk div_m*/
	reg_div_m_shift = PWM_DIV_M_SHIFT;
	reg_div_m_width = PWM_DIV_M_WIDTH;

	temp = sunxi_pwm_readl(pchip, reg_offset);
	temp = SET_BITS(reg_div_m_shift, reg_div_m_width, temp, div_m);
	sunxi_pwm_writel(pchip, reg_offset, temp);

	/* config prescal */
	reg_offset = PWM_PCR_BASE + 0x20 * sel;
	reg_shift = PWM_PRESCAL_SHIFT;
	reg_width = PWM_PRESCAL_WIDTH;
	temp = sunxi_pwm_readl(pchip, reg_offset);
	temp = SET_BITS(reg_shift, reg_width, temp, prescale);
	sunxi_pwm_writel(pchip, reg_offset, temp);

	/* config active cycles */
	reg_offset = PWM_PPR_BASE + 0x20 * sel;
	reg_shift = PWM_ACT_CYCLES_SHIFT;
	reg_width = PWM_ACT_CYCLES_WIDTH;
	temp = sunxi_pwm_readl(pchip, reg_offset);
	temp = SET_BITS(reg_shift, reg_width, temp, active_cycles);
	sunxi_pwm_writel(pchip, reg_offset, temp);

	/* config period cycles */
	reg_offset = PWM_PPR_BASE + 0x20 * sel;
	reg_shift = PWM_PERIOD_CYCLES_SHIFT;
	reg_width = PWM_PERIOD_CYCLES_WIDTH;
	temp = sunxi_pwm_readl(pchip, reg_offset);
	temp = SET_BITS(reg_shift, reg_width, temp, (entire_cycles - 1));

	sunxi_pwm_writel(pchip, reg_offset, temp);

	pwm_debug("active_cycles=%lu entire_cycles=%lu prescale=%u div_m=%u\n",
		  active_cycles, entire_cycles, prescale, div_m);
	return 0;
}

static int sunxi_pwm_enable(struct sunxi_pwm_chip *pchip)
{
	u32 value = 0, base = 0, pwm = 0;
	unsigned int reg_offset, reg_shift, reg_width;
	struct sunxi_pwm_chip *pc = pchip;
	char pin_name[8];

	base = pc->pwm_base;
	pwm = pchip->pwm;

	/* enable pin config. */
	if (base > 0)
		sprintf(pin_name, "spwm%d", pwm - base);
	else
		sprintf(pin_name, "pwm%d", pwm);
	sunxi_pwm_pin_set_state(pin_name, PWM_PIN_STATE_ACTIVE);

	/* enable clk for pwm controller. */
	get_pccr_reg_offset(pwm, &reg_offset);
	reg_shift = PWM_CLK_GATING_SHIFT;
	value = sunxi_pwm_readl(pchip, reg_offset);
	value = SET_BITS(reg_shift, reg_offset, value, 1);
	sunxi_pwm_writel(pchip, reg_offset, value);

	/* enable pwm controller. */
	reg_offset = PWM_PER;
	reg_shift = pwm;
	reg_width = 0x1;
	value = sunxi_pwm_readl(pchip, reg_offset);
	value = SET_BITS(reg_shift, reg_width, value, 1);
	sunxi_pwm_writel(pchip, reg_offset, value);

	return 0;
}

static void sunxi_pwm_disable(struct sunxi_pwm_chip *pchip)
{
	u32 value = 0, base = 0, pwm = 0;
	unsigned int reg_offset, reg_shift, reg_width;
	struct sunxi_pwm_chip *pc = pchip;
	char pin_name[8];

	base = pc->pwm_base;
	pwm = pchip->pwm;
	/* disable pwm controller */
	reg_offset = PWM_PER;
	reg_shift = pwm;
	reg_width = 0x1;
	value = sunxi_pwm_readl(pc, reg_offset);
	value = SET_BITS(reg_shift, reg_width, value, 0);
	sunxi_pwm_writel(pc, reg_offset, value);


	/*
	 * 0 , 1 --> 0
	 * 2 , 3 --> 2
	 * 4 , 5 --> 4
	 * 6 , 7 --> 6
	 */
	reg_shift &= ~(1);

	if (GET_BITS(reg_shift, 2, value) == 0) {
		/* disable clk for pwm controller. */
		get_pccr_reg_offset(pwm, &reg_offset);
		reg_shift = PWM_CLK_GATING_SHIFT;
		reg_width = 0x1;
		value = sunxi_pwm_readl(pc, reg_offset);
		value = SET_BITS(reg_shift, reg_width, value, 0);
		sunxi_pwm_writel(pc, reg_offset, value);
	}
	/* disable pin config. */
	if (base > 0)
		sprintf(pin_name, "spwm%d", pwm - base);
	else
		sprintf(pin_name, "pwm%d", pwm);

	sunxi_pwm_pin_set_state(pin_name, PWM_PIN_STATE_SLEEP);
}

struct pwm_ops {
	void		(*free)(struct sunxi_pwm_chip *pchip);
	int		(*config)(struct sunxi_pwm_chip *pchip,
				  int duty_ns, int period_ns);
	int		(*set_polarity)(struct sunxi_pwm_chip *pchip,
					enum pwm_polarity polarity);
	int		(*enable)(struct sunxi_pwm_chip *pchip);
	void		(*disable)(struct sunxi_pwm_chip *pchip);
};

static struct pwm_ops sunxi_pwm_ops = {
	.config = sunxi_pwm_config,
	.enable = sunxi_pwm_enable,
	.disable = sunxi_pwm_disable,
	.set_polarity = sunxi_pwm_set_polarity,
};

void sunxi_pwm_init(void)
{
}

int pwm_config(int pwm, int duty_ns, int period_ns)
{
	struct sunxi_pwm_chip *pchip = NULL;

	list_for_each_entry(pchip, &pwm_list, list) {
		if (pchip->pwm == pwm) {
			if (pchip->ops->config)
				return pchip->ops->config(pchip, duty_ns,
								period_ns);
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
	int ret = 0;
	__attribute__((unused)) unsigned int reg_val = 0;
	int node, sub_node;
	char main_name[20], sub_name[25];
	int pwm_base = 0;
	int pwm_number = 0;
	int handle_num = 0;
	unsigned int handle[16] = {0};
	struct sunxi_pwm_chip *pchip;

	list_for_each_entry(pchip, &pwm_list, list) {
		if (pchip->pwm == pwm) {
			pr_error("%s: err:this pwm has been requested!\n", __func__);
			return -1;
		}
	}

	sprintf(main_name, "pwm");
	sprintf(sub_name, "pwm-base");
	node = fdt_path_offset(working_fdt, main_name);
	if (node < 0) {
		pr_error("fdt err %s\n", fdt_strerror(node));
		return -1;
	}

	ret = fdt_getprop_u32(working_fdt, node, sub_name, (uint32_t *)&pwm_base);
	if (ret < 0) {
		pr_error("get fdt %s.%s fail\n", main_name, sub_name);
		return -1;
	}

	sprintf(sub_name, "pwm-number");
	ret = fdt_getprop_u32(working_fdt, node, sub_name, (uint32_t *)&pwm_number);
	if (ret < 0) {
		pr_error("get fdt %s.%s fail\n", main_name, sub_name);
		return -1;
	}
#if defined(CLK_GATE_SUPPORT)
	clk_count++;
	if (clk_count == 1) {
		reg_val |= 1 << 16;
		reg_val |= 1;
		writel(reg_val, (SUNXI_CCM_BASE + PWM_CCU_OFFSET));
	}
#endif
	/* pwm is included is in pwm area.*/
	if (pwm >= pwm_base && pwm < (pwm_base + pwm_number)) {
		/* get handle in pwm. */
		handle_num = fdt_getprop_u32(working_fdt, node, "pwms", handle);
		if (handle_num < 0) {
			handle_num = fdt_getprop_u32(working_fdt, node, "sunxi-pwms", handle);
			if (handle_num < 0) {
				pr_error("%s:%d:error:get property handle %s error:%s\n",
					__func__, __LINE__, "clocks",
					fdt_strerror(handle_num));
				return -1;
			}
		}
	} else {
		/* pwm is included is not  in pwm area,then find spwm area.*/
		sprintf(main_name, "s_pwm");
		sprintf(sub_name, "pwm-base");

		node = fdt_path_offset(working_fdt, main_name);
		ret = fdt_getprop_u32(working_fdt, node, sub_name, (uint32_t *)&pwm_base);
		if (ret < 0) {
			pr_error("get fdt %s.%s fail\n", main_name, sub_name);
			return -1;
		}

		sprintf(sub_name, "pwm-number");
		ret = fdt_getprop_u32(working_fdt, node, sub_name, (uint32_t *)&pwm_number);
		if (ret < 0) {
			pr_error("get fdt %s.%s fail\n", main_name, sub_name);
			return -1;
		} else
			pr_msg("%s:pwm number = %d\n", __func__, pwm_number);

#if defined(CLK_GATE_SUPPORT)
		sclk_count++;
		if (sclk_count == 1) {
			reg_val |= 1 << 16;
			reg_val |= 1;
			writel(reg_val, (SUNXI_PRCM_BASE + PWM_RCM_OFFSET));
		}
#endif
		if (pwm >= pwm_base && pwm < (pwm_base + pwm_number)) {
			/* get handle in pwm. */
			handle_num = fdt_getprop_u32(working_fdt, node, "pwms", handle);
			if (handle_num < 0) {
				handle_num = fdt_getprop_u32(working_fdt, node, "sunxi-pwms", handle);
				if (handle_num < 0) {
					pr_error("%s:%d:error:get property handle %s error:%s\n",
						__func__, __LINE__, "clocks",
						fdt_strerror(handle_num));
					return -1;
				}
			}
		} else {
			pr_error("the pwm id is wrong,none pwm in dts.\n");
			return -1;
		}
	}

	/* get pwm config.*/

	pchip = malloc(sizeof(*pchip));
	if (!pchip) {
		pr_error("%s:malloc failed!\n", __func__);
		return -1;
	} else {
		memset(pchip, 0, sizeof(*pchip));
	}

	pchip->pwm_base = pwm_base;

	sub_node = fdt_node_offset_by_phandle(working_fdt, handle[pwm - pwm_base]);
	if (sub_node < 0) {
		pr_error("%s:%d: error:get property by handle error\n", __func__, __LINE__);
		goto err_pwm;
	}

	pchip->pwm = pwm;
	pchip->ops = &sunxi_pwm_ops;

	ret = fdt_getprop_u32(working_fdt, sub_node, "reg_base", &pchip->base);
	if (ret < 0) {
		pr_error("%s: err: get reg-base err.\n", __func__);
		goto err_pwm;
	} else
		pr_msg("%s: reg = 0x%x. pwm = %d.\n", __func__, pchip->base, pchip->pwm);

	pchip->config = malloc(sizeof(*pchip->config));
	if (!pchip->config) {
		pr_error("%s: err: pwm[%d] config malloc failed !\n", __func__, pwm);
		goto err_pwm;
	}
	ret = fdt_getprop_u32(working_fdt, sub_node, "clk_bypass_output",
			&pchip->config->clk_bypass_output);
	if (ret >= 0)
		pr_msg("use pwm[%d] as internal clock!\n", pwm);

	list_add_tail(&pchip->list, &pwm_list);

	pr_msg("request pwm success, pwm = %d.\n", pwm);

	return pwm;

err_pwm:
	free(pchip);
	return -1;
}

/* if dont use this pwm chn then remove it */
int pwm_remove(int pwm)
{
#if defined CLK_GATE_SUPPORT
	struct sunxi_pwm_chip *pchip = NULL;

	list_for_each_entry(pchip, &pwm_list, list) {
		if (pchip->pwm == pwm) {
			if (pchip->pwm_base > 0)
				sclk_count--;
			else
				clk_count--;
		}
	}
	free(pchip->config);
	if (clk_count == 0) {
		writel(0, (SUNXI_CCM_BASE + PWM_CCU_OFFSET));
	}
	if (sclk_count == 0) {
		writel(0, (SUNXI_PRCM_BASE + PWM_RCM_OFFSET));
	}
#endif
	return 0;
}
