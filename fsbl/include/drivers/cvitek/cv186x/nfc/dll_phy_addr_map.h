 /*
 * Copyright (c) 2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __REG_DLL_PHY_ADDR_MAP_H__
#define __REG_DLL_PHY_ADDR_MAP_H__

#include "dll_phy_addr_map_macro.h"

#ifndef __REG_DATASLICE_RFILE__
#define __REG_DATASLICE_RFILE__

struct dataslice_Rfile {
  volatile uint32_t phy_dq_timing_reg;            /*        0x0 - 0x4        */
  volatile uint32_t phy_dqs_timing_reg;           /*        0x4 - 0x8        */
  volatile uint32_t phy_gate_lpbk_ctrl_reg;       /*        0x8 - 0xc        */
  volatile uint32_t phy_dll_master_ctrl_reg;      /*        0xc - 0x10       */
  volatile uint32_t phy_dll_slave_ctrl_reg;       /*       0x10 - 0x14       */
  volatile uint32_t phy_obs_reg_0;                /*       0x14 - 0x18       */
  volatile uint32_t phy_dll_obs_reg_0;            /*       0x18 - 0x1c       */
  volatile uint32_t phy_dll_obs_reg_1;            /*       0x1c - 0x20       */
  volatile uint32_t phy_static_togg_reg;          /*       0x20 - 0x24       */
};

#endif /* __REG_DATASLICE_RFILE__ */

#ifndef __REG_CTB_RFILE__
#define __REG_CTB_RFILE__

struct ctb_Rfile {
  volatile uint32_t phy_ctrl_reg;                 /*        0x0 - 0x4        */
  volatile uint32_t phy_tsel_reg;                 /*        0x4 - 0x8        */
  volatile uint32_t phy_gpio_ctrl;                /*        0x8 - 0xc        */
  volatile uint32_t phy_gpio_status;              /*        0xc - 0x10       */
};

#endif /* __REG_CTB_RFILE__ */

#ifndef __REG_DLL_PHY_ADDR_MAP__
#define __REG_DLL_PHY_ADDR_MAP__

struct dll_phy_addr_map {
  volatile char pad__0[0x2000];                   /*        0x0 - 0x2000     */
  struct dataslice_Rfile phy_dataslice;           /*     0x2000 - 0x2024     */
  volatile char pad__1[0x5c];                     /*     0x2024 - 0x2080     */
  struct ctb_Rfile phy_ctb;                       /*     0x2080 - 0x2090     */
};

#endif /* __REG_DLL_PHY_ADDR_MAP__ */

#endif /* __REG_DLL_PHY_ADDR_MAP_H__ */
