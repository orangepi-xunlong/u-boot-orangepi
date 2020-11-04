/*
 * Copyright (C) 2016 Allwinner.
 * wangwei <wangwei@allwinnertech.com>
 *
 * SUNXI AXP858  Driver
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#ifndef __AXP858_H__
#define __AXP858_H__

//PMIC chip id reg03:bit7-6  bit3-0
#define   AXP858_ADDR              (0x14)

#define AXP858_DEVICE_ADDR			(0x745)

#ifndef CONFIG_SYS_SUNXI_R_I2C0_SLAVE
#define AXP858_RUNTIME_ADDR                     (0x2d)
#else
#ifndef CONFIG_AXP858_SUNXI_I2C_SLAVE
#define AXP858_RUNTIME_ADDR                    CONFIG_SYS_SUNXI_R_I2C0_SLAVE
#else
#define AXP858_RUNTIME_ADDR                    CONFIG_AXP858_SUNXI_I2C_SLAVE
#endif
#endif

/*define AXP858 REGISTER*/
#define   PMU_POWER_ON_SOURCE               (0x00)
#define   PMU_POWER_OFF_SOURCE              (0x01)
#define   AXP858_CHIP_ID                       (0x03)
#define   PMU_DATA_BUFFER0                  (0x04)
#define   PMU_DATA_BUFFER1                  (0x05)
#define   PMU_DATA_BUFFER2                  (0x06)
#define   PMU_DATA_BUFFER3                  (0x07)

#define   PMU_ONOFF_CTL1                    (0x10)
#define   PMU_ONOFF_CTL2                    (0x11)
#define   PMU_ONOFF_CTL3                    (0x12)

#define   PMU_DC1OUT_VOL                    (0x13)
#define   PMU_DC2OUT_VOL                    (0x14)
#define   PMU_DC3OUT_VOL                    (0x15)
#define   PMU_DC4OUT_VOL                    (0x16)
#define   PMU_DC5OUT_VOL                    (0x17)
#define   PMU_DC6OUT_VOL                    (0x18)

#define   PMU_ALDO1_VOL                     (0x19)
#define   PMU_ALDO2_VOL                     (0x20)
#define   PMU_ALDO3_VOL                     (0x21)
#define   PMU_ALDO4_VOL                     (0x22)
#define   PMU_ALDO5_VOL                     (0x23)

#define   PMU_BLDO1_VOL                     (0x24)
#define   PMU_BLDO2_VOL                     (0x25)
#define   PMU_BLDO3_VOL                     (0x26)
#define   PMU_BLDO4_VOL                     (0x27)
#define   PMU_BLDO5_VOL                     (0x28)

#define   PMU_CLDO1_VOL                     (0x29)
#define   PMU_CLDO2_VOL                     (0x2A)
#define   PMU_CLDO3_GPIO1_VOL               (0x2B)
#define   PMU_CLDO4_GPIO2_CTL               (0x2C)
#define   PMU_CLDO4_VOL                     (0x2C)

#define   PMU_POWER_DISABLE_DOWN            (0x32)

#define   PMU_IRQ_STATU1                    (0x48)
#define   PMU_IRQ_STATU2                    (0x49)

#endif /* __AXP858_REGS_H__ */
