#ifndef   _MCTL_REG_H
#define   _MCTL_REG_H

#define mctl_write_w(v, addr)   (*((volatile unsigned long  *)(addr)) = (unsigned long)(v))
#define mctl_read_w(addr)       (*((volatile unsigned long  *)(addr)))
#define __REG(x)                (*(volatile unsigned int   *)(x))

#define DRAM_RET_OK     0
#define DRAM_RET_FAIL   1


#define DRAMC_PHY_BASE                  0x01C01000
#define DRAMC_MEM_SIZE                  0x400

#define __REG(x)                        (*(volatile unsigned int   *)(x))

#define DRAM_REG_SCONR                  __REG(DRAMC_PHY_BASE + 0x00)
#define DRAM_REG_STMG0R                 __REG(DRAMC_PHY_BASE + 0x04)
#define DRAM_REG_STMG1R                 __REG(DRAMC_PHY_BASE + 0x08)
#define DRAM_REG_SCTLR                  __REG(DRAMC_PHY_BASE + 0x0c)
#define DRAM_REG_SREFR                  __REG(DRAMC_PHY_BASE + 0x10)
#define DRAM_REG_SEXTMR                 __REG(DRAMC_PHY_BASE + 0x14)
#define DRAM_REG_DDLYR                  __REG(DRAMC_PHY_BASE + 0x24)
#define DRAM_REG_DADRR                  __REG(DRAMC_PHY_BASE + 0x28)
#define DRAM_REG_DVALR                  __REG(DRAMC_PHY_BASE + 0x2c)
#define DRAM_REG_DRPTR0                 __REG(DRAMC_PHY_BASE + 0x30)
#define DRAM_REG_DRPTR1                 __REG(DRAMC_PHY_BASE + 0x34)
#define DRAM_REG_DRPTR2                 __REG(DRAMC_PHY_BASE + 0x38)
#define DRAM_REG_DRPTR3                 __REG(DRAMC_PHY_BASE + 0x3c)
#define DRAM_REG_SEFR                   __REG(DRAMC_PHY_BASE + 0x40)
#define DRAM_REG_MAE                    __REG(DRAMC_PHY_BASE + 0x44)
#define DRAM_REG_ASPR                   __REG(DRAMC_PHY_BASE + 0x48)
#define DRAM_REG_SDLY0                  __REG(DRAMC_PHY_BASE + 0x4C)
#define DRAM_REG_SDLY1                  __REG(DRAMC_PHY_BASE + 0x50)
#define DRAM_REG_SDLY2                  __REG(DRAMC_PHY_BASE + 0x54)
#define DRAM_REG_MCR0                   __REG(DRAMC_PHY_BASE + 0x100 + 4*0)
#define DRAM_REG_MCR1                   __REG(DRAMC_PHY_BASE + 0x100 + 4*1)
#define DRAM_REG_MCR2                   __REG(DRAMC_PHY_BASE + 0x100 + 4*2)
#define DRAM_REG_MCR3                   __REG(DRAMC_PHY_BASE + 0x100 + 4*3)
#define DRAM_REG_MCR4                   __REG(DRAMC_PHY_BASE + 0x100 + 4*4)
#define DRAM_REG_MCR5                   __REG(DRAMC_PHY_BASE + 0x100 + 4*5)
#define DRAM_REG_MCR6                   __REG(DRAMC_PHY_BASE + 0x100 + 4*6)
#define DRAM_REG_MCR7                   __REG(DRAMC_PHY_BASE + 0x100 + 4*7)
#define DRAM_REG_MCR8                   __REG(DRAMC_PHY_BASE + 0x100 + 4*8)
#define DRAM_REG_MCR9                   __REG(DRAMC_PHY_BASE + 0x100 + 4*9)
#define DRAM_REG_MCR10                  __REG(DRAMC_PHY_BASE + 0x100 + 4*10)
#define DRAM_REG_MCR11                  __REG(DRAMC_PHY_BASE + 0x100 + 4*11)
#define DRAM_REG_BWCR                   __REG(DRAMC_PHY_BASE + 0x140)

//PIO register for dram
#define DRAM_PIO_BASE                   0x01c20800
#define DRAM_PIO_MEMSIZE                0x400
#define SDR_PAD_DRV_REG                 __REG(DRAM_PIO_BASE + 0x2C0)
#define SDR_PAD_PUL_REG                 __REG(DRAM_PIO_BASE + 0x2C4)
#define SDR_VREF                        __REG(DRAM_PIO_BASE + 0x24)


//CCM register for dram
#define _CCM_BASE                       0x01c20000
#define PLL_DDR_CTRL_REG                __REG(_CCM_BASE + 0x20)

#define BUS_CLK_GATING_REG0             __REG(_CCM_BASE + 0x60)
#define DRAM_GATING_REG                 __REG(_CCM_BASE + 0x100)
#define PLL_DDR_PAT_CTRL_REG            __REG(_CCM_BASE + 0x290)
#define BUS_SOFT_RST_REG0               __REG(_CCM_BASE + 0x2C0)


#define SDR_T_CAS                       0x2  // CL
#define SDR_T_RAS                       0x8  // 120000ns>=tRAS>=42ns SDR166
#define SDR_T_RCD                       0x2  // tRCD>=15ns           SDR166
#define SDR_T_RP                        0x2  // tRP>=15ns            SDR166
#define SDR_T_WR                        0x2  // tRP>=12ns            SDR166
#define SDR_T_RFC                       0xd  // tRFC>=60ns           SDR166
#define SDR_T_XSR                       0xf9 // tXSRD>=200CK
#define SDR_T_RC                        0xa  // tRC>=60ns            SDR166
#define SDR_T_INIT                      0x8  //
#define SDR_T_INIT_REF                  0x7
#define SDR_T_WTR                       0x1  // tWTR>=2CK            SDR166
#define SDR_T_RRD                       0x1  // tRRD>=12ns           SDR166
#define SDR_T_XP                        0x0  // one clk is ok




#endif  //_MCTL_REG_H
