#include <linux/log2.h>
#include "soph_vo_sys_reg.h"
#include "soph_mipipll_cfg.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

static void _cal_pll_reg(u8 dsi_id, u32 clkkHz, u32 VCORx10000, u32 *reg_txpll, u32 *reg_set, u32 factor)
{
	u8 gain = 1 << ilog2(MAX(1, 25000000UL / VCORx10000));
	u32 VCOCx1000 = VCORx10000 * gain / 10;
	u8 reg_disp_div_sel = VCOCx1000 / clkkHz;
	u8 dig_dig = ilog2(gain);
	u8 reg_divout_sel = MIN(3, dig_dig);
	u8 reg_div_sel = dig_dig - reg_divout_sel;
	u32 loop_gain = VCOCx1000 / 133000;
	u32 loop_c = loop_gain & ~0x7;
	u8 reg_div_loop = (loop_c > 32 ? 3 : loop_c / 8) - 1;
	u8 loop_gain1 = (reg_div_loop + 1) * 8;

	*reg_set = ((u64)(factor * loop_gain1) << 26) / VCOCx1000;
	*reg_txpll = (reg_div_loop << 24) | (reg_div_sel << 10) | (reg_divout_sel << 8) | reg_disp_div_sel;

	debug("clkkHz(%d) VCORx10000(%d) gain(%d)\n", clkkHz, VCORx10000, gain);
	debug("VCOCx1000(%d) loop_gain(%d) loop_c(%d) loop_gain1(%d)\n",
			VCOCx1000, loop_gain, loop_c, loop_gain1);
	debug("regs: disp_div_sel(%d) div_loop(%d) divout_sel(%d) div_sel(%d), set(%#x)\n",
			reg_disp_div_sel, reg_div_loop, reg_divout_sel, reg_div_sel, *reg_set);
}

void mipi_dphy_set_pll(u8 dsi_id, u32 clkkHz, u8 lane, u8 bits)
{
	u32 VCORx10000 = clkkHz * bits * 10 / lane;
	u32 reg_txpll, reg_set;

	_cal_pll_reg(dsi_id, clkkHz, VCORx10000, &reg_txpll, &reg_set, 1800000);
	_reg_write_mask(REG_DSI_PHY_TXPLL_SETUP(dsi_id), 0x30007ff, reg_txpll);

	_reg_write_mask(REG_MPLL_CTRL1(dsi_id), BIT(12), 0);
	_reg_write(REG_FPLL_CTRL2(dsi_id), reg_set);
	// update
	_reg_write_mask(REG_FPLL_CTRL5(dsi_id), BIT(0), 1);
	_reg_write_mask(REG_FPLL_CTRL5(dsi_id), BIT(0), 0);
}

void mipipll_clk_set(u32 clkkhz)
{
	_reg_write(REG_DSI_PHY_POWER_DOWN_CFG(1), 0x0);
	mipi_dphy_set_pll(1, clkkhz, 4, 24);
}
