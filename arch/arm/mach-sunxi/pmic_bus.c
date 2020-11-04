// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2015 Hans de Goede <hdegoede@redhat.com>
 *
 * Sunxi PMIC bus access helpers
 *
 * The axp152 & axp209 use an i2c bus, the axp221 uses the p2wi bus and the
 * axp223 uses the rsb bus, these functions abstract this.
 */

#include <common.h>
#include <asm/arch/p2wi.h>
#include <asm/arch/rsb.h>
#include <i2c.h>
#include "sunxi_i2c.h"
#include <asm/arch/pmic_bus.h>

#define AXP152_I2C_ADDR			0x30

#define AXP209_I2C_ADDR			0x34

#define AXP221_CHIP_ADDR		0x68
#define AXP221_CTRL_ADDR		0x3e
#define AXP221_INIT_DATA		0x3e


int pmic_bus_init(u16 device_addr, u16 runtime_addr)
{
	/* This cannot be 0 because it is used in SPL before BSS is ready */
	//static int needs_init = 1;
	__maybe_unused int ret;

	//if (!needs_init)
	//	return 0;

#if defined CONFIG_AXP221_POWER || defined CONFIG_AXP809_POWER || defined CONFIG_AXP818_POWER\
	||  defined CONFIG_AXP858_POWER || defined CONFIG_AXP2585_POWER || defined CONFIG_AXP2101_POWER\
	|| defined CONFIG_SUNXI_AXP152_POWER || defined CONFIG_AXP1530_POWER || defined CONFIG_AXP81X_POWER\
	|| defined CONFIG_AXP806_POWER
# ifdef CONFIG_MACH_SUN6I
	p2wi_init();
	ret = p2wi_change_to_p2wi_mode(AXP221_CHIP_ADDR, AXP221_CTRL_ADDR,
				       AXP221_INIT_DATA);
# elif defined CONFIG_MACH_SUN8I_R40 || defined CONFIG_R_I2C0_ENABLE
	/* Nothing. R40 uses the AXP221s in I2C mode */
	ret = 0;
	if (i2c_get_bus_num() != SUNXI_VIR_R_I2C0)
		i2c_set_bus_num(SUNXI_VIR_R_I2C0);
# else
	ret = rsb_init();
	if (ret)
		return ret;

	ret = rsb_set_device_address(device_addr, runtime_addr);
#endif
	if (ret)
		return ret;
#endif

	//needs_init = 0;
	return 0;
}

int pmic_bus_read(u16 runtime_addr, u8 reg, u8 *data)
{
#ifdef CONFIG_AXP152_POWER
	return i2c_read(AXP152_I2C_ADDR, reg, 1, data, 1);
#elif defined CONFIG_AXP209_POWER || defined CONFIG_AXP2101_POWER || defined CONFIG_SUNXI_AXP152_POWER\
	|| defined CONFIG_AXP1530_POWER || defined CONFIG_AXP81X_POWER || defined CONFIG_AXP806_POWER
	return i2c_read(runtime_addr, reg, 1, data, 1);
#elif defined CONFIG_AXP221_POWER || defined CONFIG_AXP809_POWER || defined CONFIG_AXP818_POWER\
	|| defined CONFIG_AXP858_POWER || defined CONFIG_AXP2585_POWER
# ifdef CONFIG_MACH_SUN6I
	return p2wi_read(reg, data);
# elif defined CONFIG_MACH_SUN8I_R40
	return i2c_read(AXP209_I2C_ADDR, reg, 1, data, 1);
# else
	return rsb_read(runtime_addr, reg, data);
# endif
#endif
}

int pmic_bus_write(u16 runtime_addr, u8 reg, u8 data)
{
#ifdef CONFIG_AXP152_POWER
	return i2c_write(AXP152_I2C_ADDR, reg, 1, &data, 1);
#elif defined CONFIG_AXP209_POWER || defined CONFIG_AXP2101_POWER || defined CONFIG_SUNXI_AXP152_POWER\
	|| defined CONFIG_AXP1530_POWER || defined CONFIG_AXP81X_POWER || defined CONFIG_AXP806_POWER
	return i2c_write(runtime_addr, reg, 1, &data, 1);
#elif defined CONFIG_AXP221_POWER || defined CONFIG_AXP809_POWER || defined CONFIG_AXP818_POWER\
	||  defined CONFIG_AXP858_POWER ||  defined CONFIG_AXP2585_POWER || defined CONFIG_AXP2101_POWER
# ifdef CONFIG_MACH_SUN6I
	return p2wi_write(reg, data);
# elif defined CONFIG_MACH_SUN8I_R40
	return i2c_write(AXP209_I2C_ADDR, reg, 1, &data, 1);
# else
	return rsb_write(runtime_addr, reg, data);
# endif
#endif
}

int pmic_bus_setbits(u16 runtime_addr, u8 reg, u8 bits)
{
	int ret;
	u8 val;

	ret = pmic_bus_read(runtime_addr, reg, &val);
	if (ret)
		return ret;

	val |= bits;
	return pmic_bus_write(runtime_addr, reg, val);
}

int pmic_bus_clrbits(u16 runtime_addr, u8 reg, u8 bits)
{
	int ret;
	u8 val;

	ret = pmic_bus_read(runtime_addr, reg, &val);
	if (ret)
		return ret;

	val &= ~bits;
	return pmic_bus_write(runtime_addr, reg, val);
}
