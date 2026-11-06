#ifndef __REG_NFC9_MINICTRL_REGS_H__
#define __REG_NFC9_MINICTRL_REGS_H__

#include "nfc9_minictrl_regs_macro.h"

#ifndef __REG_RF_MINICTRL_REGS__
#define __REG_RF_MINICTRL_REGS__

struct rf_minictrl_regs {
  volatile uint32_t wp_settings;                  /*        0x0 - 0x4        */
  volatile uint32_t rbn_settings;                 /*        0x4 - 0x8        */
  volatile uint32_t common_settings;              /*        0x8 - 0xc        */
  volatile uint32_t skip_bytes_conf;              /*        0xc - 0x10       */
  volatile uint32_t skip_bytes_offset;            /*       0x10 - 0x14       */
  volatile uint32_t toggle_timings_0;             /*       0x14 - 0x18       */
  volatile uint32_t toggle_timings_1;             /*       0x18 - 0x1c       */
  volatile uint32_t async_toggle_timings;         /*       0x1c - 0x20       */
  volatile uint32_t sync_timings;                 /*       0x20 - 0x24       */
  volatile uint32_t timings0;                     /*       0x24 - 0x28       */
  volatile uint32_t timings1;                     /*       0x28 - 0x2c       */
  volatile uint32_t timings2;                     /*       0x2c - 0x30       */
  volatile uint32_t dll_phy_update_cnt;           /*       0x30 - 0x34       */
  volatile uint32_t dll_phy_ctrl;                 /*       0x34 - 0x38       */
};

#endif /* __REG_RF_MINICTRL_REGS__ */

#ifndef __REG_NFC9_MINICTRL_REGS__
#define __REG_NFC9_MINICTRL_REGS__

struct nfc9_minictrl_regs {
  volatile char pad__0[0x1000];                   /*        0x0 - 0x1000     */
  struct rf_minictrl_regs minictrl_settings;      /*     0x1000 - 0x1038     */
  volatile char pad__1[0x8];                      /*     0x1038 - 0x1040     */
};

#endif /* __REG_NFC9_MINICTRL_REGS__ */

#endif /* __REG_NFC9_MINICTRL_REGS_H__ */
