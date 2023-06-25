/* SPDX-License-Identifier: GPL-2.0 */

#include "nand_for_clock.h"

#define CCMU_BASE (0x01c20000)
#define PLL_PERIPHO_CTRL_REG (0x28)
#define NAND0_CLK_REG (0x80)
#define NAND1_CLK_REG (0x84)
#define BUS_GATING_REG0 (0x60)
#define BUS_SOFT_RST_REG0 (0x2c0)

#define PIO_BASE (0X01C20800)
#define Pn_CFG0(n) ((n) * (0x24) + 0x00)
#define Pn_CFG1(n) ((n) * (0x24) + 0x04)
#define Pn_CFG2(n) ((n) * (0x24) + 0x08)


__u32 _get_pll6_clk_v0(void)
{
	__u32 reg_val;
	__u32 factor_n;
	__u32 factor_k;
	__u32 clock;

	reg_val  = *(volatile __u32 *)(CCMU_BASE + PLL_PERIPHO_CTRL_REG);
	factor_n = ((reg_val >> 8) & 0x1f) + 1;
	factor_k = ((reg_val >> 4) & 0x3) + 1;
	//div_m = ((reg_val >> 0) & 0x3) + 1;

	clock = 24000000 * factor_n * factor_k / 2;
	//NAND_Print("pll6 clock is %d Hz\n", clock);
	//if(clock != 600000000)
	//printf("pll6 clock rate error, %d!!!!!!!\n", clock);

	return clock;
}

__s32 _get_ndfc_clk_v0(__u32 nand_index, __u32 *pdclk, __u32 *pcclk)
{
	__u32 sclk0_reg_adr;
	__u32 sclk_src, sclk_src_sel;
	__u32 sclk_pre_ratio_n, sclk_ratio_m;
	__u32 reg_val, sclk0;

	if (nand_index > 1) {
		printf("wrong nand id: %d\n", nand_index);
		return -1;
	}

	if (nand_index == 0) {
		sclk0_reg_adr =
			(CCMU_BASE + NAND0_CLK_REG); //CCM_NAND0_CLK0_REG;
	} else if (nand_index == 1) {
		sclk0_reg_adr =
			(CCMU_BASE + NAND1_CLK_REG); //CCM_NAND1_CLK0_REG;
	}

	// sclk0
	reg_val		 = get_wvalue(sclk0_reg_adr);
	sclk_src_sel     = (reg_val >> 24) & 0x3;
	sclk_pre_ratio_n = (reg_val >> 16) & 0x3;
	;
	sclk_ratio_m = (reg_val)&0xf;
	if (sclk_src_sel == 0)
		sclk_src = 24;
	else
		sclk_src = _get_pll6_clk_v0() / 1000000;
	sclk0		 = (sclk_src >> sclk_pre_ratio_n) / (sclk_ratio_m + 1);

	if (nand_index == 0) {
		//NAND_Print("Reg 0x01c20080: 0x%x\n",
		//	*(volatile __u32 *)(0x01c20080));
	} else {
		//NAND_Print("Reg 0x01c20084: 0x%x\n",
		//	*(volatile __u32 *)(0x01c20084));
	}
	//NAND_Print("NDFC%d:  sclk0(2*dclk): %d MHz\n", nand_index, sclk0);

	*pdclk = sclk0 / 2;

	return 0;
}

__s32 _change_ndfc_clk_v0(__u32 nand_index, __u32 dclk_src_sel, __u32 dclk,
		__u32 cclk_src_sel, __u32 cclk)
{
	u32 reg_val;
	u32 sclk0_src_sel, sclk0, sclk0_src, sclk0_pre_ratio_n, sclk0_src_t,
	    sclk0_ratio_m;
	u32 sclk0_reg_adr;

	if (nand_index == 0) {
		sclk0_reg_adr =
			(CCMU_BASE + NAND0_CLK_REG); //CCM_NAND0_CLK0_REG;
	} else if (nand_index == 1) {
		sclk0_reg_adr =
			(CCMU_BASE + NAND1_CLK_REG); //CCM_NAND1_CLK0_REG;
	} else {
		printf("_change_ndfc_clk error, wrong nand index: %d\n",
				nand_index);
		return -1;
	}

	/*close dclk and cclk*/
	if (dclk == 0) {
		reg_val = get_wvalue(sclk0_reg_adr);
		reg_val &= (~(0x1U << 31));
		put_wvalue(sclk0_reg_adr, reg_val);

		//printf("_change_ndfc_clk, close sclk0 and sclk1\n");
		return 0;
	}

	sclk0_src_sel = dclk_src_sel;
	sclk0	 = dclk * 2; //set sclk0 to 2*dclk.

	if (sclk0_src_sel == 0x0) {
		//osc pll
		sclk0_src = 24;
	} else {
		//pll6 for ndfc version 1
		sclk0_src = _get_pll6_clk_v0() / 1000000;
	}

	//////////////////// sclk0: 2*dclk
	//sclk0_pre_ratio_n
	sclk0_pre_ratio_n = 3;
	if (sclk0_src > 4 * 16 * sclk0)
		sclk0_pre_ratio_n = 3;
	else if (sclk0_src > 2 * 16 * sclk0)
		sclk0_pre_ratio_n = 2;
	else if (sclk0_src > 1 * 16 * sclk0)
		sclk0_pre_ratio_n = 1;
	else
		sclk0_pre_ratio_n = 0;

	sclk0_src_t = sclk0_src >> sclk0_pre_ratio_n;

	//sclk0_ratio_m
	sclk0_ratio_m = (sclk0_src_t / (sclk0)) - 1;
	if (sclk0_src_t % (sclk0)) {
		sclk0_ratio_m += 1;
	}

	/////////////////////////////// close clock
	reg_val = get_wvalue(sclk0_reg_adr);
	reg_val &= (~(0x1U << 31));
	put_wvalue(sclk0_reg_adr, reg_val);

	///////////////////////////////configure
	//sclk0 <--> 2*dclk
	reg_val = get_wvalue(sclk0_reg_adr);
	//clock source select
	reg_val &= (~(0x3 << 24));
	reg_val |= (sclk0_src_sel & 0x3) << 24;
	//clock pre-divide ratio(N)
	reg_val &= (~(0x3 << 16));
	reg_val |= (sclk0_pre_ratio_n & 0x3) << 16;
	//clock divide ratio(M)
	reg_val &= ~(0xf << 0);
	reg_val |= (sclk0_ratio_m & 0xf) << 0;
	put_wvalue(sclk0_reg_adr, reg_val);

	/////////////////////////////// open clock
	reg_val = get_wvalue(sclk0_reg_adr);
	reg_val |= 0x1U << 31;
	put_wvalue(sclk0_reg_adr, reg_val);

	//NAND_Print("NAND_SetClk for nand index %d \n", nand_index);
	if (nand_index == 0) {
		//NAND_Print("Reg 0x01c20080: 0x%x\n",
		//	*(volatile __u32 *)(0x01c20080));
	} else {
		//NAND_Print("Reg 0x01c20084: 0x%x\n",
		//	*(volatile __u32 *)(0x01c20084));
	}

	return 0;
}

__s32 _close_ndfc_clk_v0(__u32 nand_index)
{
	u32 reg_val;
	u32 sclk0_reg_adr;

	if (nand_index == 0) {
		sclk0_reg_adr =
			(CCMU_BASE + NAND0_CLK_REG); //CCM_NAND0_CLK0_REG;
	} else if (nand_index == 1) {
		sclk0_reg_adr =
			(CCMU_BASE + NAND1_CLK_REG); //CCM_NAND1_CLK0_REG;
	} else {
		printf("close_ndfc_clk error, wrong nand index: %d\n",
				nand_index);
		return -1;
	}

	/*close dclk and cclk*/
	reg_val = *(volatile __u32 *)((unsigned long)sclk0_reg_adr);
	reg_val &= (~(0x1U << 31));
	*(volatile __u32 *)((unsigned long)sclk0_reg_adr) = reg_val;

	reg_val = *(volatile __u32 *)((unsigned long)sclk0_reg_adr);
	//printf("ndfc clk release,sclk reg adr %x:%x\n",sclk0_reg_adr,reg_val);

	//printf(" close sclk0 and sclk1\n");
	return 0;
}

__s32 _open_ndfc_ahb_gate_and_reset_v0(__u32 nand_index)
{
	__u32 reg_val = 0;

	/*
	   1. release ahb reset and open ahb clock gate for ndfc version 1.
	   */
	if (nand_index == 0) {
		// ahb clock gate
		reg_val = *(volatile __u32 *)(CCMU_BASE + BUS_GATING_REG0);
		reg_val &= (~(0x1U << 13));
		reg_val |= (0x1U << 13);
		*(volatile __u32 *)(CCMU_BASE + BUS_GATING_REG0) = reg_val;

		// reset
		reg_val = *(volatile __u32 *)(CCMU_BASE + BUS_SOFT_RST_REG0);
		reg_val &= (~(0x1U << 13));
		*(volatile __u32 *)(CCMU_BASE + BUS_SOFT_RST_REG0) = reg_val;

		reg_val = *(volatile __u32 *)(CCMU_BASE + BUS_SOFT_RST_REG0);
		reg_val |= (0x1U << 13);
		*(volatile __u32 *)(CCMU_BASE + BUS_SOFT_RST_REG0) = reg_val;

		reg_val = *(volatile __u32 *)(CCMU_BASE + BUS_SOFT_RST_REG0);
		reg_val &= (~(0x1U << 13));
		*(volatile __u32 *)(CCMU_BASE + BUS_SOFT_RST_REG0) = reg_val;

		reg_val = *(volatile __u32 *)(CCMU_BASE + BUS_SOFT_RST_REG0);
		reg_val |= (0x1U << 13);
		*(volatile __u32 *)(CCMU_BASE + BUS_SOFT_RST_REG0) = reg_val;

	} else {
		printf("open ahb gate and reset, wrong nand index: %d\n",
				nand_index);
		return -1;
	}

	return 0;
}

__s32 _close_ndfc_ahb_gate_and_reset_v0(__u32 nand_index)
{
	__u32 reg_val = 0;

	/*
	   1. release ahb reset and open ahb clock gate for ndfc version 1.
	   */
	if (nand_index == 0) {
		// reset
		reg_val = *(volatile __u32 *)(CCMU_BASE + BUS_SOFT_RST_REG0);
		reg_val &= (~(0x1U << 13));
		//	*(volatile __u32 *)(0x01c20000 + 0x2c0) = reg_val;
		*(volatile __u32 *)(CCMU_BASE + BUS_SOFT_RST_REG0) = reg_val;

		// ahb clock gate
		reg_val = *(volatile __u32 *)(CCMU_BASE + BUS_GATING_REG0);
		reg_val &= (~(0x1U << 13));
		//	*(volatile __u32 *)(0x01c20000 + 0x60) = reg_val;
		*(volatile __u32 *)(CCMU_BASE + BUS_GATING_REG0) = reg_val;
	} else {
		printf("close ahb gate and reset, wrong nand index: %d\n",
				nand_index);
		return -1;
	}
	reg_val = *(volatile __u32 *)(CCMU_BASE + BUS_SOFT_RST_REG0);
	//	printf("0x01c202c0:%x\n",reg_val);
	reg_val = *(volatile __u32 *)(CCMU_BASE + BUS_GATING_REG0);
	//	printf("0x01c20060:%x\n",reg_val);

	return 0;
}

__s32 _cfg_ndfc_gpio_v0(__u32 nand_index)
{
	if (nand_index == 0) {
		*(volatile __u32 *)(PIO_BASE + Pn_CFG0(2)) = 0x22222222;
		*(volatile __u32 *)(PIO_BASE + Pn_CFG1(2)) = 0x22222222;
		*(volatile __u32 *)(PIO_BASE + Pn_CFG2(2)) = 0x222;
		NAND_Print("NAND_PIORequest, nand_index: 0x%x\n", nand_index);
		NAND_Print("Reg 0x01c20848: 0x%x\n",
				*(volatile __u32 *)(PIO_BASE + Pn_CFG0(2)));
		NAND_Print("Reg 0x01c2084c: 0x%x\n",
				*(volatile __u32 *)(PIO_BASE + Pn_CFG1(2)));
		NAND_Print("Reg 0x01c20850: 0x%x\n",
				*(volatile __u32 *)(PIO_BASE + Pn_CFG2(2)));
	} else {
		printf("_cfg_ndfc_gpio_v0, wrong nand index %d\n", nand_index);
		return -1;
	}

	return 0;
}

int NAND_ClkRequest_v0(__u32 nand_index)
{
	__s32 ret	  = 0;
	__u32 ndfc_version = NAND_GetNdfcVersion();

	if (ndfc_version == 1) {
		if (nand_index != 0) {
			printf("NAND_ClkRequest, wrong nand index %d for ndfc"
				" version %d\n", nand_index, ndfc_version);
			return -1;
		}
		// 1. release ahb reset and open ahb clock gate
		_open_ndfc_ahb_gate_and_reset_v0(nand_index);

		// 2. configure ndfc's sclk0
		ret = _change_ndfc_clk_v0(nand_index, 1, 10, 0, 0);
		if (ret < 0) {
			printf("NAND_ClkRequest, set dclk failed!\n");
			return -1;
		}

	} else {
		printf("NAND_ClkRequest, wrong ndfc version, %d\n",
				ndfc_version);
		return -1;
	}

	return 0;
}

void NAND_ClkRelease_v0(__u32 nand_index)
{
	__u32 ndfc_version = NAND_GetNdfcVersion();

	if (ndfc_version == 1) {
		if (nand_index != 0) {
			printf("NAND_Clkrelease, wrong nand index %d for ndfc"
				" version %d\n", nand_index, ndfc_version);
			return;
		}
		_close_ndfc_clk_v0(nand_index);

		_close_ndfc_ahb_gate_and_reset_v0(nand_index);
	}

	return;
}

/*
 ******************************************************************************
 *
 *             NAND_GetCmuClk
 *
 *  Description:
 *
 *
 *  Parameters:
 *
 *
 *  Return value:
 *
 *
 ******************************************************************************
 */
int NAND_SetClk_v0(__u32 nand_index, __u32 nand_clk0, __u32 nand_clk1)
{
	__u32 ndfc_version = NAND_GetNdfcVersion();
	__u32 dclk_src_sel, dclk;
	__s32 ret = 0;

	if (ndfc_version == 1) {
		if (nand_index != 0) {
			printf("NAND_ClkRequest, wrong nand index %d for ndfc"
				" version %d\n", nand_index, ndfc_version);
			return -1;
		}

		////////////////////////////////////////////////
		dclk_src_sel = 1;
		dclk	 = nand_clk0;
		////////////////////////////////////////////////
		ret = _change_ndfc_clk_v0(nand_index, dclk_src_sel, dclk, 0, 0);
		if (ret < 0) {
			printf("NAND_SetClk, change ndfc clock failed\n");
			return -1;
		}

	} else {
		printf("NAND_SetClk, wrong ndfc version, %d\n", ndfc_version);
		return -1;
	}

	return 0;
}

int NAND_GetClk_v0(__u32 nand_index, __u32 *pnand_clk0, __u32 *pnand_clk1)
{
	__s32 ret;
	__u32 ndfc_version = NAND_GetNdfcVersion();

	if (ndfc_version == 1) {
		//NAND_Print("NAND_GetClk for nand index %d \n", nand_index);
		ret = _get_ndfc_clk_v0(nand_index, pnand_clk0, 0);
		if (ret < 0) {
			printf("NAND_GetClk, failed!\n");
			return -1;
		}

	} else {
		printf("NAND_SetClk, wrong ndfc version, %d\n", ndfc_version);
		return -1;
	}

	return 0;
}
