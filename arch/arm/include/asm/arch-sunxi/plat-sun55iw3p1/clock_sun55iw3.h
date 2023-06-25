/* SPDX-License-Identifier: GPL-2.0+ */
#include <asm/arch/plat-sun55iw3p1/clock_autogen.h>
#define  sunxi_ccm_reg CCMU_st
#define  pll1_cfg                pll_cpu0_ctrl_reg
#define  apb2_cfg                apb1_clk_reg
#define  uart_gate_reset         uart_bgr_reg
#define  cpu_axi_cfg             cpu_clk_reg
#define  pll6_cfg                pll_peri0_ctrl_reg
#define  psi_ahb1_ahb2_cfg       ahb_clk_reg
#define  apb1_cfg                apb0_clk_reg
#define  mbus_cfg                mbus_clk_reg
#define  ve_clk_cfg              ve_clk_reg
#define  de_clk_cfg              de0_clk_reg
#define  mbus_gate               mbus_mat_clk_gating_reg
#define  dma_gate_reset          dma_bgr_reg
#define  sd0_clk_cfg             smhc0_clk_reg
#define  sd1_clk_cfg             smhc1_clk_reg
#define  sd2_clk_cfg             smhc2_clk_reg
#define  sd_gate_reset           smhc_bgr_reg
#define  twi_gate_reset          twi_bgr_reg
#define  ce_gate_reset           ce_bgr_reg
#define  ce_clk_cfg              ce_clk_reg

#define  APB2_CLK_SRC_OSC24M     (APB1_CLK_REG_CLK_SRC_SEL_HOSC << APB1_CLK_REG_CLK_SRC_SEL_OFFSET)
#define  APB2_CLK_SRC_OSC32K     (APB2_CLK_SRC_OSC32K << APB1_CLK_REG_CLK_SRC_SEL_OFFSET)
#define  APB2_CLK_SRC_PSI        (APB1_CLK_REG_CLK_SRC_SEL_CLK16M_RC << APB1_CLK_REG_CLK_SRC_SEL_OFFSET)
#define  APB2_CLK_SRC_PLL6       (APB1_CLK_REG_CLK_SRC_SEL_PERI0_600M_BUS << APB1_CLK_REG_CLK_SRC_SEL_OFFSET)

#define APB2_CLK_RATE_N_1               (0x0 << 8)
#define APB2_CLK_RATE_N_2               (0x1 << 8)
#define APB2_CLK_RATE_N_4               (0x2 << 8)
#define APB2_CLK_RATE_N_8               (0x3 << 8)
#define APB2_CLK_RATE_N_MASK            (3 << 8)
#define APB2_CLK_RATE_M(m)              (((m)-1) << APB1_CLK_REG_FACTOR_M_OFFSET)
#define APB2_CLK_RATE_M_MASK            (3 << APB1_CLK_REG_FACTOR_M_OFFSET)

/* MMC clock bit field */
#define CCM_MMC_CTRL_M(x)               ((x) - 1)
#define CCM_MMC_CTRL_N(x)               ((x) << SMHC0_CLK_REG_FACTOR_N_OFFSET)
#define CCM_MMC_CTRL_OSCM24             (SMHC0_CLK_REG_CLK_SRC_SEL_HOSC << SMHC0_CLK_REG_CLK_SRC_SEL_OFFSET)
#define CCM_MMC_CTRL_PLL6X2             (SMHC0_CLK_REG_CLK_SRC_SEL_PERI0_400M << SMHC0_CLK_REG_CLK_SRC_SEL_OFFSET)
#define CCM_MMC_CTRL_PLL_PERIPH2X2      (SMHC0_CLK_REG_CLK_SRC_SEL_PERI0_300M << SMHC0_CLK_REG_CLK_SRC_SEL_OFFSET)
#define CCM_MMC_CTRL_ENABLE             (SMHC0_CLK_REG_SMHC0_CLK_GATING_CLOCK_IS_ON << SMHC0_CLK_REG_SMHC0_CLK_GATING_OFFSET)
/* if doesn't have these delays */
#define CCM_MMC_CTRL_OCLK_DLY(a)        ((void) (a), 0)
#define CCM_MMC_CTRL_SCLK_DLY(a)        ((void) (a), 0)

/* Module gate/reset shift*/
#define RESET_SHIFT                     (16)
#define GATING_SHIFT                    (0)

/*CE*/
#define CE_CLK_SRC_MASK                   (0x1)
#define CE_CLK_SRC_SEL_BIT                (CE_CLK_REG_CLK_SRC_SEL_OFFSET)
#define CE_CLK_SRC                        (0x01)

#define CE_CLK_DIV_RATION_N_BIT           (8)
#define CE_CLK_DIV_RATION_N_MASK          (0x3)
#define CE_CLK_DIV_RATION_N               (0)

#define CE_CLK_DIV_RATION_M_BIT           (CE_BGR_REG_CE_GATING_OFFSET)
#define CE_CLK_DIV_RATION_M_MASK          (0xF)
#define CE_CLK_DIV_RATION_M               (2)

#define CE_SCLK_ONOFF_BIT                 (CE_CLK_REG_CE_CLK_GATING_OFFSET)
#define CE_SCLK_ON                        (CE_CLK_REG_CE_CLK_GATING_CLOCK_IS_ON)

#define CE_GATING_PASS                    (CE_BGR_REG_CE_GATING_PASS)
#define CE_GATING_BIT                     (CE_BGR_REG_CE_GATING_OFFSET)

#define CE_MBUS_GATING_MASK               (1)
#define CE_MBUS_GATING_BIT                (MBUS_MAT_CLK_GATING_REG_CE_MCLK_EN_OFFSET)
#define CE_MBUS_GATING                    (MBUS_MAT_CLK_GATING_REG_CE_MCLK_EN_OFFSET)

#define CE_RST_BIT                        (CE_BGR_REG_CE_RST_OFFSET)
#define CE_DEASSERT                       (CE_BGR_REG_CE_SYS_RST_DE_ASSERT)


