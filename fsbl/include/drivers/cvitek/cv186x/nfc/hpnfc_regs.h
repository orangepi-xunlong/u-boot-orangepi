 /*
 * Copyright (c) 2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef __REG_HPNFC_REGS_H__
#define __REG_HPNFC_REGS_H__

//#include <hpnfc_regs_macro.h>

#ifndef __REG_RF_CONTROLLER_REGS__
#define __REG_RF_CONTROLLER_REGS__

struct rf_controller_regs {
  volatile uint32_t cmd_reg0;                     /*        0x0 - 0x4        */
  volatile uint32_t cmd_reg1;                     /*        0x4 - 0x8        */
  volatile uint32_t cmd_reg2;                     /*        0x8 - 0xc        */
  volatile uint32_t cmd_reg3;                     /*        0xc - 0x10       */
  volatile uint32_t cmd_status_ptr;               /*       0x10 - 0x14       */
  volatile uint32_t cmd_status;                   /*       0x14 - 0x18       */
  volatile uint32_t cmd_status_ext;               /*       0x18 - 0x1c       */
  volatile char pad__0[0x4];                      /*       0x1c - 0x20       */
  volatile uint32_t cmd_reg4;                     /*       0x20 - 0x24       */
  volatile char pad__1[0xec];                     /*       0x24 - 0x110      */
  volatile uint32_t intr_status;                  /*      0x110 - 0x114      */
  volatile uint32_t intr_enable;                  /*      0x114 - 0x118      */
  volatile uint32_t ctrl_status;                  /*      0x118 - 0x11c      */
  volatile char pad__2[0x4];                      /*      0x11c - 0x120      */
  volatile uint32_t trd_status;                   /*      0x120 - 0x124      */
  volatile char pad__3[0x4];                      /*      0x124 - 0x128      */
  volatile uint32_t trd_error_intr_status;        /*      0x128 - 0x12c      */
  volatile char pad__4[0x4];                      /*      0x12c - 0x130      */
  volatile uint32_t trd_error_intr_en;            /*      0x130 - 0x134      */
  volatile char pad__5[0x4];                      /*      0x134 - 0x138      */
  volatile uint32_t trd_comp_intr_status;         /*      0x138 - 0x13c      */
  volatile char pad__6[0x4];                      /*      0x13c - 0x140      */
  volatile uint32_t dma_target_error_l;           /*      0x140 - 0x144      */
  volatile uint32_t dma_target_error_h;           /*      0x144 - 0x148      */
  volatile uint32_t boot_status;                  /*      0x148 - 0x14c      */
  volatile uint32_t trd_timeout_intr_status;      /*      0x14c - 0x150      */
  volatile char pad__7[0x4];                      /*      0x150 - 0x154      */
  volatile uint32_t trd_timeout_intr_en;          /*      0x154 - 0x158      */
  volatile char pad__8[0x4];                      /*      0x158 - 0x15c      */
  volatile uint32_t zq_cal_stat;                  /*      0x15c - 0x160      */
};

#endif /* __REG_RF_CONTROLLER_REGS__ */

#ifndef __REG_RF_CTRL_CONFIG__
#define __REG_RF_CTRL_CONFIG__

struct rf_ctrl_config {
  volatile uint32_t transfer_cfg_0;               /*        0x0 - 0x4        */
  volatile uint32_t transfer_cfg_1;               /*        0x4 - 0x8        */
  volatile uint32_t long_polling;                 /*        0x8 - 0xc        */
  volatile uint32_t short_polling;                /*        0xc - 0x10       */
  volatile uint32_t rdst_ctrl_0;                  /*       0x10 - 0x14       */
  volatile uint32_t rdst_ctrl_1;                  /*       0x14 - 0x18       */
  volatile uint32_t lun_status_cmd;               /*       0x18 - 0x1c       */
  volatile uint32_t lun_interleaved_cmd;          /*       0x1c - 0x20       */
  volatile uint32_t lun_addr_offset;              /*       0x20 - 0x24       */
  volatile uint32_t nf_dev_layout;                /*       0x24 - 0x28       */
  volatile uint32_t ecc_config_0;                 /*       0x28 - 0x2c       */
  volatile uint32_t ecc_config_1;                 /*       0x2c - 0x30       */
  volatile uint32_t device_ctrl;                  /*       0x30 - 0x34       */
  volatile uint32_t multiplane_config;            /*       0x34 - 0x38       */
  volatile uint32_t cache_config;                 /*       0x38 - 0x3c       */
  volatile uint32_t dma_settings;                 /*       0x3c - 0x40       */
  volatile uint32_t sdma_size;                    /*       0x40 - 0x44       */
  volatile uint32_t sdma_trd_num;                 /*       0x44 - 0x48       */
  volatile uint32_t time_out;                     /*       0x48 - 0x4c       */
  volatile uint32_t sdma_addr0;                   /*       0x4c - 0x50       */
  volatile uint32_t sdma_addr1;                   /*       0x50 - 0x54       */
  volatile uint32_t fifo_trigg_level;             /*       0x54 - 0x58       */
  volatile uint32_t zq_cal_config;                /*       0x58 - 0x5c       */
  volatile uint32_t zq_cal_en;                    /*       0x5c - 0x60       */
  volatile uint32_t long_cal_period;              /*       0x60 - 0x64       */
  volatile uint32_t short_cal_period;             /*       0x64 - 0x68       */
  volatile char pad__0[0x18];                     /*       0x68 - 0x80       */
  volatile uint32_t remap_ctrl;                   /*       0x80 - 0x84       */
  volatile uint32_t remap_mask;                   /*       0x84 - 0x88       */
  volatile uint32_t remap_access;                 /*       0x88 - 0x8c       */
  volatile uint32_t remap_log_addr;               /*       0x8c - 0x90       */
  volatile uint32_t remap_phys_addr;              /*       0x90 - 0x94       */
};

#endif /* __REG_RF_CTRL_CONFIG__ */

#ifndef __REG_RF_DI_REGS__
#define __REG_RF_DI_REGS__

struct rf_di_regs {
  volatile uint32_t di_control;                   /*        0x0 - 0x4        */
  volatile uint32_t di_inject0;                   /*        0x4 - 0x8        */
  volatile uint32_t di_inject1;                   /*        0x8 - 0xc        */
  volatile uint32_t di_error_reg_addr;            /*        0xc - 0x10       */
  volatile uint32_t di_inject2;                   /*       0x10 - 0x14       */
};

#endif /* __REG_RF_DI_REGS__ */

#ifndef __REG_RF_DBG_REGS__
#define __REG_RF_DBG_REGS__

struct rf_dbg_regs {
  volatile uint32_t dbg_ctrl;                     /*        0x0 - 0x4        */
  volatile uint32_t dbg_stat;                     /*        0x4 - 0x8        */
  volatile uint32_t dbg_data;                     /*        0x8 - 0xc        */
};

#endif /* __REG_RF_DBG_REGS__ */

#ifndef __REG_RF_CONTROLLER_PARAMS__
#define __REG_RF_CONTROLLER_PARAMS__

struct rf_controller_params {
  volatile uint32_t ctrl_version;                 /*        0x0 - 0x4        */
  volatile uint32_t ctrl_features_reg;            /*        0x4 - 0x8        */
  volatile uint32_t manufacturer_id;              /*        0x8 - 0xc        */
  volatile uint32_t nf_device_areas;              /*        0xc - 0x10       */
  volatile uint32_t device_params_0;              /*       0x10 - 0x14       */
  volatile uint32_t device_params_1;              /*       0x14 - 0x18       */
  volatile uint32_t device_features;              /*       0x18 - 0x1c       */
  volatile uint32_t device_blocks_per_lun;        /*       0x1c - 0x20       */
  volatile uint32_t device_revision;              /*       0x20 - 0x24       */
  volatile uint32_t onfi_timing_modes_0;          /*       0x24 - 0x28       */
  volatile uint32_t onfi_timing_modes_1;          /*       0x28 - 0x2c       */
  volatile uint32_t onfi_iterlv_op_attr;          /*       0x2c - 0x30       */
  volatile uint32_t onfi_sync_opt_0;              /*       0x30 - 0x34       */
  volatile uint32_t onfi_sync_opt_1;              /*       0x34 - 0x38       */
  volatile uint32_t bch_cfg_0;                    /*       0x38 - 0x3c       */
  volatile uint32_t bch_cfg_1;                    /*       0x3c - 0x40       */
  volatile uint32_t bch_cfg_2;                    /*       0x40 - 0x44       */
  volatile uint32_t bch_cfg_3;                    /*       0x44 - 0x48       */
};

#endif /* __REG_RF_CONTROLLER_PARAMS__ */

#ifndef __REG_HPNFC_REGS__
#define __REG_HPNFC_REGS__

struct hpnfc_regs {
  struct rf_controller_regs cmd_status;           /*        0x0 - 0x160      */
  volatile char pad__0[0x2a0];                    /*      0x160 - 0x400      */
  struct rf_ctrl_config ctrl_config;              /*      0x400 - 0x494      */
  volatile char pad__1[0x26c];                    /*      0x494 - 0x700      */
  struct rf_di_regs di_config;                    /*      0x700 - 0x714      */
  volatile char pad__2[0x2c];                     /*      0x714 - 0x740      */
  struct rf_dbg_regs dbg_regs;                    /*      0x740 - 0x74c      */
  volatile char pad__3[0xb4];                     /*      0x74c - 0x800      */
  struct rf_controller_params controller_params;  /*      0x800 - 0x848      */
  volatile char pad__4[0x38];                     /*      0x848 - 0x880      */
};

#endif /* __REG_HPNFC_REGS__ */

#endif /* __REG_HPNFC_REGS_H__ */
