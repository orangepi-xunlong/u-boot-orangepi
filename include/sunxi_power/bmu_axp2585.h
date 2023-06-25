/*
 * Copyright (C) 2016 Allwinner.
 * wangwei <wangwei@allwinnertech.com>
 *
 * SUNXI AXP1506  Driver
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#ifndef __AXP2585_H__
#define __AXP2585_H__

#define AXP2585_ADDR (0x56)

#define AXP2585_DEVICE_ADDR (0x3a3)

#ifndef CONFIG_SYS_SUNXI_R_I2C0_SLAVE
#define AXP2585_RUNTIME_ADDR                     (0x3a)
#else
#ifndef CONFIG_AXP858_SUNXI_I2C_SLAVE
#define AXP2585_RUNTIME_ADDR                    CONFIG_SYS_SUNXI_R_I2C0_SLAVE
#else
#define AXP2585_RUNTIME_ADDR                    CONFIG_AXP858_SUNXI_I2C_SLAVE
#endif
#endif
/* For BMU1760 */
#define BMU_CHG_STATUS (0x00)
#define BMU_BAT_STATUS (0x02)
#define AXP2585_CHIP_ID (0x03)
#define BMU_BATFET_CTL (0x10)
#define BMU_BOOST_EN (0x12)
#define BMU_BOOST_CTL (0x13)
#define PWR_ON_CTL (0x17)
#define BMU_POWER_ON_STATUS (0x4A)
#define BMU_BAT_VOL_H (0x78)
#define BMU_BAT_VOL_L (0x79)
#define BMU_CHG_CUR_LIMIT (0x8b)
#define BMU_BAT_PERCENTAGE (0xB9)
#define BMU_REG_LOCK (0xF2)
#define BMU_REG_EXTENSION_EN (0xF4)
#define BMU_ADDR_EXTENSION (0xFF)

#endif /* __AXP2585_H__ */
