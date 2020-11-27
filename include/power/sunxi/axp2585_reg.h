/*
 * Copyright (C) 2016 Allwinner.
 * wangwei <wangwei@allwinnertech.com>
 *
 * SUNXI AXP1506  Driver
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#ifndef   __AXP2585_REGS_H__
#define   __AXP2585_REGS_H__

#define   AXP2585_ADDR			(0x56)

/* For BMU1760 */
#define	PMU_CHG_STATUS			(0x00)
#define PMU_BAT_STATUS			(0x02)
#define PMU_IC_TYPE			(0x03)
#define PMU_BATFET_CTL			(0x10)
#define PMU_BOOST_EN                    (0x12)
#define PMU_BOOST_CTL                   (0x13)
#define PWR_ON_CTL			(0x17)
#define PMU_POWER_ON_STATUS		(0x4A)
#define	PMU_BAT_VOL_H			(0x78)
#define	PMU_BAT_VOL_L			(0x79)
#define PMU_CHG_CUR_LIMIT		(0x8b)
#define PMU_BAT_PERCENTAGE		(0xB9)
#define	PMU_REG_LOCK			(0xF2)
#define	PMU_REG_EXTENSION_EN		(0xF4)
#define	PMU_ADDR_EXTENSION		(0xFF)

#endif /* __AXP2585_REGS_H__ */

