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

#define   AXP2585_ADDR              (0x36)

#define   PMU_INPUT_CURRENT_LIMIT   (0x00)
#define   PMU_INPUT_VOL_LIMIT       (0x02)
#define   PMU_IC_TYPE               (0x03)
#define   PMU_VBUS_CHARG_STATUS     (0x0C)
#define   PMU_POWER_ON_STATUS       (0x65)
#define   PMU_BAT_VOL_H             (0x78)
#define   PMU_BAT_VOL_L             (0x79)
#define   PMU_DATA_BUFFER0          (0x98)
#define   PMU_DATA_BUFFER1          (0x9C)
#define   PMU_BAT_PERCENTAGE        (0xE5)
#define   PMU_ADDR_EXTENSION        (0xff)

#endif /* __AXP1506_REGS_H__ */

