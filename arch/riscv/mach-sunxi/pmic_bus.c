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


static int twi_bus_num;

/*
 *para u16 device_addr has two meaning: twi mode is twi bus num, and rsb mode is rsb slave address.
 *para u16 runtime_addr has two meaning: twi mode is invaild, and rsb mode is rsb runtime address.
 *
 */

int pmic_bus_init(u16 device_addr, u16 runtime_addr)
{
	__maybe_unused int ret = 0;

#ifdef CONFIG_SYS_I2C_SUNXI
#ifdef CONFIG_R_I2C0_ENABLE
	if ((device_addr == 0x745) || (device_addr == 0x3a3)) {
	/*all axp call pmic_bus_init, device_addr defaults to the slave address in rsb mode
	 * so i2c mode need change device_addr = SUNXI_VIR_R_I2C0
	 */
		device_addr = SUNXI_VIR_R_I2C0;
	}
#endif
	twi_bus_num = i2c_get_bus_num();
	if (twi_bus_num != device_addr)
		ret = i2c_set_bus_num(device_addr);
#else
	ret = rsb_init();
	if (ret)
		return ret;

	ret = rsb_set_device_address(device_addr, runtime_addr);
#endif
	return ret;
}

int pmic_bus_read(u16 runtime_addr, u8 reg, u8 *data)
{
#ifdef CONFIG_SYS_I2C_SUNXI
	return i2c_read(runtime_addr, reg, 1, data, 1);
#else
	return rsb_read(runtime_addr, reg, data);
#endif
}

int pmic_bus_write(u16 runtime_addr, u8 reg, u8 data)
{
#ifdef CONFIG_SYS_I2C_SUNXI
	return i2c_write(runtime_addr, reg, 1, &data, 1);
#else
	return rsb_write(runtime_addr, reg, data);
#endif
}

int pmic_bus_exit(void)
{
#ifdef CONFIG_SYS_I2C_SUNXI
	return i2c_set_bus_num(twi_bus_num);
#else
	return twi_bus_num;
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
