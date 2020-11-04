/* SPDX-License-Identifier: GPL-2.0 */

#include "nand_for_clock.h"

#define PLL_PERIPHO_CTRL_REG (0x20)
#define NAND0_0_CLK_REG (0x0810)
#define NAND0_1_CLK_REG (0x0814)
#define NAND_BUS_GATING_REG (0X082C)
#define MBUS_MASTER_CLK_GATING_REG (0x0804)
#define SPI0_CLK_REG (0x0940)
#define SPI_BUS_GATING_REG (0x096C)

#define PRCM_PLL_PERI_CTRL_REG (0x1010)

/*function pin select*/
#define Pn_CFG0(n) ((n) * (0x24) + 0x00)
/*function pin select*/
#define Pn_CFG1(n) ((n) * (0x24) + 0x04)
/*function pin select*/
#define Pn_CFG2(n) ((n) * (0x24) + 0x08)
/*pull-up/down select*/
#define Pn_PUL0(n) ((n) * (0x24) + 0x1c)
/*pull-up/down select*/
#define Pn_PUL1(n) ((n) * (0x24) + 0x20)

__u32 _get_pll6_clk_v1(void)
{
#ifdef CONFIG_MACH_SUN50IW11
	__u32 reg_val;
	__u32 factor_n;
	__u32 factor_m;
	__u32 factor_p0;
	__u32 clock;

	reg_val   = get_wvalue(SUNXI_PRCM_BASE + PRCM_PLL_PERI_CTRL_REG);
	factor_p0 = ((reg_val >> 16) & 0x3) + 1;
	factor_n  = ((reg_val >> 8) & 0xFF) + 1;
	factor_m = ((reg_val >> 1) & 0x1) + 1;

	clock = 24000000 * factor_n / factor_m / factor_p0 / 2;
	/*NAND_Print("pll6 clock is %d Hz\n", clock);*/
	/* if(clock != 600000000) */
	/* printf("pll6 clock rate error, %d!!!!!!!\n", clock); */

	return clock;
#else
	__u32 reg_val;
	__u32 factor_n;
	__u32 factor_m0;
	__u32 factor_m1;
	__u32 clock;

	reg_val   = get_wvalue(SUNXI_CCM_BASE + PLL_PERIPHO_CTRL_REG);
	factor_n  = ((reg_val >> 8) & 0xFF) + 1;
	factor_m0 = ((reg_val >> 0) & 0x1) + 1;
	factor_m1 = ((reg_val >> 1) & 0x1) + 1;
	/* div_m = ((reg_val >> 0) & 0x3) + 1; */

	clock = 24000000 * factor_n / factor_m0 / factor_m1 / 2;
	/*NAND_Print("pll6 clock is %d Hz\n", clock);*/
	/* if(clock != 600000000) */
	/* printf("pll6 clock rate error, %d!!!!!!!\n", clock); */

	return clock;
#endif
}

__s32 _get_ndfc_clk_v1(__u32 nand_index, __u32 *pdclk, __u32 *pcclk)
{
	__u32 sclk0_reg_adr, sclk1_reg_adr;
	__u32 sclk_src, sclk_src_sel;
	__u32 sclk_pre_ratio_n, sclk_ratio_m;
	__u32 reg_val, sclk0, sclk1;

	if (nand_index == 0) {
		sclk0_reg_adr = (SUNXI_CCM_BASE +
				NAND0_0_CLK_REG); /* CCM_NAND0_CLK0_REG; */
		sclk1_reg_adr = (SUNXI_CCM_BASE +
				NAND0_1_CLK_REG); /* CCM_NAND0_CLK1_REG; */
	} else {
		printf("_get_ndfc_clk_v1 error, wrong nand index: %d\n",
				nand_index);
		return -1;
	}

	/* sclk0 */
	reg_val		 = get_wvalue(sclk0_reg_adr);
	sclk_src_sel     = (reg_val >> 24) & 0x7;
	sclk_pre_ratio_n = (reg_val >> 8) & 0x3;
	;
	sclk_ratio_m = (reg_val)&0xf;

	if (sclk_src_sel == 0)
		sclk_src = 24;
	else if (sclk_src_sel < 0x3)
		sclk_src = _get_pll6_clk_v1() / 1000000;
	else
		sclk_src = 2 * _get_pll6_clk_v1() / 1000000;

	sclk0 = (sclk_src >> sclk_pre_ratio_n) / (sclk_ratio_m + 1);

	/* sclk1 */
	reg_val		 = get_wvalue(sclk1_reg_adr);
	sclk_src_sel     = (reg_val >> 24) & 0x7;
	sclk_pre_ratio_n = (reg_val >> 8) & 0x3;

	sclk_ratio_m = (reg_val)&0xf;

	if (sclk_src_sel == 0)
		sclk_src = 24;
	else if (sclk_src_sel < 0x3)
		sclk_src = _get_pll6_clk_v1() / 1000000;
	else
		sclk_src = 2 * _get_pll6_clk_v1() / 1000000;
	sclk1		 = (sclk_src >> sclk_pre_ratio_n) / (sclk_ratio_m + 1);

	if (nand_index == 0) {
		/* printf("Reg 0x03001000 + 0x0810: 0x%x\n", *(volatile __u32 *)(SUNXI_CCM_BASE + 0x0810)); */
		/* printf("Reg 0x03001000 + 0x0814: 0x%x\n", *(volatile __u32 *)(SUNXI_CCM_BASE + 0x0814)); */
	} else {
		/* printf("Reg 0x06000408: 0x%x\n", *(volatile __u32 *)(0x06000400)); */
		/* printf("Reg 0x0600040C: 0x%x\n", *(volatile __u32 *)(0x06000404)); */
	}
	/* printf("NDFC%d:  sclk0(2*dclk): %d MHz   sclk1(cclk): %d MHz\n", nand_index, sclk0, sclk1); */

	*pdclk = sclk0 / 2;
	*pcclk = sclk1;

	return 0;
}

__s32 _change_ndfc_clk_v1(__u32 nand_index, __u32 dclk_src_sel, __u32 dclk,
		__u32 cclk_src_sel, __u32 cclk)
{
	u32 reg_val;
	u32 sclk0_src_sel, sclk0, sclk0_src, sclk0_pre_ratio_n, sclk0_src_t,
	    sclk0_ratio_m;
	u32 sclk1_src_sel, sclk1, sclk1_src, sclk1_pre_ratio_n, sclk1_src_t,
	    sclk1_ratio_m;
	u32 sclk0_reg_adr, sclk1_reg_adr;

	if (nand_index == 0) {
		sclk0_reg_adr = (SUNXI_CCM_BASE +
				NAND0_0_CLK_REG); /* CCM_NAND0_CLK0_REG; */
		sclk1_reg_adr = (SUNXI_CCM_BASE +
				NAND0_1_CLK_REG); /* CCM_NAND0_CLK1_REG; */
	} else {
		printf("_change_ndfc_clk_v1 error, wrong nand index: %d\n",
				nand_index);
		return -1;
	}

	/*close dclk and cclk*/
	if ((dclk == 0) && (cclk == 0)) {
		reg_val = get_wvalue(sclk0_reg_adr);
		reg_val &= (~(0x1U << 31));
		put_wvalue(sclk0_reg_adr, reg_val);

		reg_val = get_wvalue(sclk1_reg_adr);
		reg_val &= (~(0x1U << 31));
		put_wvalue(sclk1_reg_adr, reg_val);

		printf("_change_ndfc_clk_v1, close sclk0 and sclk1\n");
		return 0;
	}

	sclk0_src_sel = dclk_src_sel;
	sclk0	 = dclk * 2; /* set sclk0 to 2*dclk. */
	sclk1_src_sel = cclk_src_sel;
	sclk1	 = cclk;

	if (sclk0_src_sel == 0x0) {
		/* osc pll */
		sclk0_src = 24;
	} else if (sclk0_src_sel < 0x3)
		sclk0_src = _get_pll6_clk_v1() / 1000000;
	else
		sclk0_src = 2 * _get_pll6_clk_v1() / 1000000;

	if (sclk1_src_sel == 0x0) {
		/* osc pll */
		sclk1_src = 24;
	} else if (sclk1_src_sel < 0x3)
		sclk1_src = _get_pll6_clk_v1() / 1000000;
	else
		sclk1_src = 2 * _get_pll6_clk_v1() / 1000000;

	/* sclk0: 2*dclk */
	/* sclk0_pre_ratio_n */
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

	/* sclk0_ratio_m */
	sclk0_ratio_m = (sclk0_src_t / (sclk0)) - 1;
	if (sclk0_src_t % (sclk0))
		sclk0_ratio_m += 1;

	/* sclk1: cclk */
	/* sclk1_pre_ratio_n */
	sclk1_pre_ratio_n = 3;
	if (sclk1_src > 4 * 16 * sclk1)
		sclk1_pre_ratio_n = 3;
	else if (sclk1_src > 2 * 16 * sclk1)
		sclk1_pre_ratio_n = 2;
	else if (sclk1_src > 1 * 16 * sclk1)
		sclk1_pre_ratio_n = 1;
	else
		sclk1_pre_ratio_n = 0;

	sclk1_src_t = sclk1_src >> sclk1_pre_ratio_n;

	/* sclk1_ratio_m */
	sclk1_ratio_m = (sclk1_src_t / (sclk1)) - 1;
	if (sclk1_src_t % (sclk1))
		sclk1_ratio_m += 1;

	/* close clock */
	reg_val = get_wvalue(sclk0_reg_adr);
	reg_val &= (~(0x1U << 31));
	put_wvalue(sclk0_reg_adr, reg_val);

	reg_val = get_wvalue(sclk1_reg_adr);
	reg_val &= (~(0x1U << 31));
	put_wvalue(sclk1_reg_adr, reg_val);

	/* configure */
	/* sclk0 <--> 2*dclk */
	reg_val = get_wvalue(sclk0_reg_adr);
	/* clock source select */
	reg_val &= (~(0x7 << 24));
	reg_val |= (sclk0_src_sel & 0x7) << 24;
	/* clock pre-divide ratio(N) */
	reg_val &= (~(0x3 << 8));
	reg_val |= (sclk0_pre_ratio_n & 0x3) << 8;
	/* clock divide ratio(M) */
	reg_val &= ~(0xf << 0);
	reg_val |= (sclk0_ratio_m & 0xf) << 0;
	put_wvalue(sclk0_reg_adr, reg_val);

	/* sclk1 <--> cclk */
	reg_val = get_wvalue(sclk1_reg_adr);
	/* clock source select */
	reg_val &= (~(0x7 << 24));
	reg_val |= (sclk1_src_sel & 0x7) << 24;
	/* clock pre-divide ratio(N) */
	reg_val &= (~(0x3 << 8));
	reg_val |= (sclk1_pre_ratio_n & 0x3) << 8;
	/* clock divide ratio(M) */
	reg_val &= ~(0xf << 0);
	reg_val |= (sclk1_ratio_m & 0xf) << 0;
	put_wvalue(sclk1_reg_adr, reg_val);

	/* open clock */
	reg_val = get_wvalue(sclk0_reg_adr);
	reg_val |= 0x1U << 31;
	put_wvalue(sclk0_reg_adr, reg_val);

	reg_val = get_wvalue(sclk1_reg_adr);
	reg_val |= 0x1U << 31;
	put_wvalue(sclk1_reg_adr, reg_val);

	/* printf("NAND_SetClk for nand index %d \n", nand_index); */
	if (nand_index == 0) {
		sclk0_ratio_m     = (get_wvalue(sclk0_reg_adr) & 0xf) + 1;
		sclk0_pre_ratio_n = 1
			<< ((get_wvalue(sclk0_reg_adr) >> 8) & 0x3);

		sclk1_ratio_m     = (get_wvalue(sclk1_reg_adr) & 0xf) + 1;
		sclk1_pre_ratio_n = 1
			<< ((get_wvalue(sclk1_reg_adr) >> 8) & 0x3);

		/* NAND_Print_DBG("uboot Nand0 clk: %dMHz,PERI=%d,N=%d,M=%d,"
		 *	"T=%d\n",sclk0_src/sclk0_ratio_m/sclk0_pre_ratio_n,
			sclk0_src, sclk0_pre_ratio_n, sclk0_ratio_m, dclk); */
		/* NAND_Print_DBG("uboot Nand Ecc clk: %dMHz,PERI=%d,N=%d,M=%d,"
		 *	"T=%d\n",sclk1_src/sclk1_ratio_m/sclk1_pre_ratio_n,
			sclk1_src, sclk1_pre_ratio_n, sclk1_ratio_m, cclk); */
		/* printf("Reg 0x03001000 + 0x0810: 0x%x\n",
		 *	*(volatile __u32 *)(SUNXI_CCM_BASE + 0x0810)); */
		/* printf("Reg 0x03001000 + 0x0814: 0x%x\n",
		 *	*(volatile __u32 *)(SUNXI_CCM_BASE + 0x0814)); */
	} else {
		/* printf("Reg 0x06000408: 0x%x\n",
		 *	*(volatile __u32 *)(0x06000400)); */
		/* printf("Reg 0x0600040C: 0x%x\n",
		 *	*(volatile __u32 *)(0x06000404)); */
	}

	return 0;
}

__s32 _close_ndfc_clk_v1(__u32 nand_index)
{
	u32 reg_val;
	u32 sclk0_reg_adr, sclk1_reg_adr;

	if (nand_index == 0) {
		/* disable nand sclock gating */
		sclk0_reg_adr = (SUNXI_CCM_BASE +
				NAND0_0_CLK_REG); /* CCM_NAND0_CLK0_REG; */
		sclk1_reg_adr = (SUNXI_CCM_BASE +
				NAND0_1_CLK_REG); /* CCM_NAND0_CLK1_REG; */

		reg_val = get_wvalue(sclk0_reg_adr);
		reg_val &= (~(0x1U << 31));
		put_wvalue(sclk0_reg_adr, reg_val);

		reg_val = get_wvalue(sclk1_reg_adr);
		reg_val &= (~(0x1U << 31));
		put_wvalue(sclk1_reg_adr, reg_val);
	} else {
		printf("close_ndfc_clk error, wrong nand index: %d\n",
				nand_index);
		return -1;
	}

	return 0;
}

__s32 _open_ndfc_ahb_gate_and_reset_v1(__u32 nand_index)
{
	__u32 reg_val = 0;

	/*
	   1. release ahb reset and open ahb clock gate for ndfc version 1.
	   */
	if (nand_index == 0) {
		/* reset */
		reg_val = get_wvalue(SUNXI_CCM_BASE + NAND_BUS_GATING_REG);
		reg_val &= (~(0x1U << 16));
		reg_val |= (0x1U << 16);
		put_wvalue((SUNXI_CCM_BASE + NAND_BUS_GATING_REG), reg_val);
		/* ahb clock gate */
		reg_val = get_wvalue(SUNXI_CCM_BASE + NAND_BUS_GATING_REG);
		reg_val &= (~(0x1U << 0));
		reg_val |= (0x1U << 0);
		put_wvalue((SUNXI_CCM_BASE + NAND_BUS_GATING_REG), reg_val);

		/* enable nand mbus gate */
		reg_val = get_wvalue(SUNXI_CCM_BASE +
				MBUS_MASTER_CLK_GATING_REG);
		reg_val &= (~(0x1U << 5));
		reg_val |= (0x1U << 5);
		put_wvalue((SUNXI_CCM_BASE + MBUS_MASTER_CLK_GATING_REG),
				reg_val);
	} else {
		printf("open ahb gate and reset, wrong nand index: %d\n",
				nand_index);
		return -1;
	}

	return 0;
}

__s32 _close_ndfc_ahb_gate_and_reset_v1(__u32 nand_index)
{
	__u32 reg_val = 0;

	/*
	   1. release ahb reset and open ahb clock gate for ndfc version 1.
	   */
	if (nand_index == 0) {
		/* reset */
		reg_val = get_wvalue(SUNXI_CCM_BASE + NAND_BUS_GATING_REG);
		reg_val &= (~(0x1U << 16));
		put_wvalue((SUNXI_CCM_BASE + NAND_BUS_GATING_REG), reg_val);
		/* ahb clock gate */
		reg_val = get_wvalue(SUNXI_CCM_BASE + NAND_BUS_GATING_REG);
		reg_val &= (~(0x1U << 0));
		put_wvalue((SUNXI_CCM_BASE + NAND_BUS_GATING_REG), reg_val);

		/* disable nand mbus gate */
		reg_val = get_wvalue(SUNXI_CCM_BASE +
				MBUS_MASTER_CLK_GATING_REG);
		reg_val &= (~(0x1U << 5));
		put_wvalue((SUNXI_CCM_BASE + MBUS_MASTER_CLK_GATING_REG),
				reg_val);

	} else {
		printf("close ahb gate and reset, wrong nand index: %d\n",
				nand_index);
		return -1;
	}
	return 0;
}

__s32 _cfg_ndfc_gpio_v1(__u32 nand_index)
{
	__u32 cfg;
	if (nand_index == 0) {
		/* setting PC0 port as Nand control line */
		*(volatile __u32 *)(SUNXI_PIO_BASE + Pn_CFG0(2)) =
			0x22222222;
		/* setting PC1 port as Nand data line */
		*(volatile __u32 *)(SUNXI_PIO_BASE + Pn_CFG1(2)) =
			0x22222222;
		/* setting PC2 port as Nand RB1 */
		cfg = *(volatile __u32 *)(SUNXI_PIO_BASE + Pn_CFG2(2));
		cfg &= (~0x7);
		cfg |= 0x2;
		*(volatile __u32 *)(SUNXI_PIO_BASE + Pn_CFG2(2)) = cfg;

		/* pull-up/down --only setting RB & CE pin pull-up */
		*(volatile __u32 *)(SUNXI_PIO_BASE + Pn_PUL0(2)) =
			0x40000440;
		cfg = *(volatile __u32 *)(SUNXI_PIO_BASE + Pn_PUL1(2));
		cfg &= (~0x3);
		cfg |= 0x01;
		*(volatile __u32 *)(SUNXI_PIO_BASE + Pn_PUL1(2)) = cfg;

	} else {
		printf("_cfg_ndfc_gpio_v1, wrong nand index %d\n", nand_index);
		return -1;
	}

	return 0;
}

__s32 _get_spic_clk_v1(__u32 spinand_index, __u32 *pdclk)
{
	__u32 sclk0_reg_adr;
	__u32 sclk_src, sclk_src_sel;
	__u32 sclk_pre_ratio_n, sclk_ratio_m;
	__u32 reg_val, sclk0;

	if (spinand_index == 0) {
		/* CCM_SPI0_CLK_REG */
		sclk0_reg_adr = (SUNXI_CCM_BASE + SPI0_CLK_REG);
	} else {
		printf("_get_spic_clk_v1 error, wrong spinand index: %d\n",
				spinand_index);
		return -1;
	}

	/* sclk0 */
	reg_val		 = get_wvalue(sclk0_reg_adr);
	sclk_src_sel     = (reg_val >> 24) & 0x7;
	sclk_pre_ratio_n = (reg_val >> 8) & 0x3;
	sclk_ratio_m     = (reg_val)&0xf;

	if (sclk_src_sel == 0)
		sclk_src = 24;
	else if (sclk_src_sel < 0x3)
		sclk_src = _get_pll6_clk_v1() / 1000000;
	else
		sclk_src = 2 * _get_pll6_clk_v1() / 1000000;

	sclk0 = (sclk_src >> sclk_pre_ratio_n) / (sclk_ratio_m + 1);

	*pdclk = sclk0;

	return 0;
}

__s32 _change_spic_clk_v1(__u32 spinand_index, __u32 dclk_src_sel, __u32 dclk)
{
	u32 reg_val;
	u32 sclk0_src_sel, sclk0, sclk0_src, sclk0_pre_ratio_n, sclk0_src_t,
	    sclk0_ratio_m;
	u32 sclk0_reg_adr;

	if (spinand_index == 0) {
		/* CCM_SPI0_CLK_REG */
		sclk0_reg_adr = (SUNXI_CCM_BASE + SPI0_CLK_REG);
	} else {
		printf("_change_spic_clk_v1 error, wrong spinand index: %d\n",
				spinand_index);
		return -1;
	}

	/* close dclk */
	if (dclk == 0) {
		reg_val = get_wvalue(sclk0_reg_adr);
		reg_val &= (~(0x1U << 31));
		put_wvalue(sclk0_reg_adr, reg_val);

		printf("_change_spic_clk_v1, close sclk0\n");
		return 0;
	}

	sclk0_src_sel = dclk_src_sel;
	sclk0	 = dclk;

	if (sclk0_src_sel == 0x0) {
		/* osc pll */
		sclk0_src = 24;
	} else if (sclk0_src_sel < 0x3)
		sclk0_src = _get_pll6_clk_v1() / 1000000;
	else
		sclk0_src = 2 * _get_pll6_clk_v1() / 1000000;

	/* sclk0: 2*dclk */
	/* sclk0_pre_ratio_n */
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

	/* sclk0_ratio_m */
	sclk0_ratio_m = (sclk0_src_t / (sclk0)) - 1;
	if (sclk0_src_t % (sclk0))
		sclk0_ratio_m += 1;

	/* close clock */
	reg_val = get_wvalue(sclk0_reg_adr);
	reg_val &= (~(0x1U << 31));
	put_wvalue(sclk0_reg_adr, reg_val);

	/* configure */
	/* sclk0 <--> 2*dclk */
	reg_val = get_wvalue(sclk0_reg_adr);
	/* clock source select */
	reg_val &= (~(0x7 << 24));
	reg_val |= (sclk0_src_sel & 0x7) << 24;
	/* clock pre-divide ratio(N) */
	reg_val &= (~(0x3 << 8));
	reg_val |= (sclk0_pre_ratio_n & 0x3) << 8;
	/* clock divide ratio(M) */
	reg_val &= ~(0xf << 0);
	reg_val |= (sclk0_ratio_m & 0xf) << 0;
	put_wvalue(sclk0_reg_adr, reg_val);

	/* open clock */
	reg_val = get_wvalue(sclk0_reg_adr);
	reg_val |= 0x1U << 31;
	put_wvalue(sclk0_reg_adr, reg_val);

	if (spinand_index == 0)
		printf("sclk0_reg_adr: 0x%x, src clock: 0x%x\n",
			*(volatile __u32 *)(sclk0_reg_adr),
			get_wvalue(SUNXI_CCM_BASE + PLL_PERIPHO_CTRL_REG));

	return 0;
}

__s32 _close_spic_clk_v1(__u32 spinand_index)
{
	u32 reg_val;
	u32 sclk0_reg_adr;

	if (spinand_index == 0) {
		/*disable nand sclock gating*/
		sclk0_reg_adr = (SUNXI_CCM_BASE + SPI0_CLK_REG);

		reg_val = get_wvalue(sclk0_reg_adr);
		reg_val &= (~(0x1U << 31));
		put_wvalue(sclk0_reg_adr, reg_val);

	} else {
		printf("close_spic_clk error, wrong spinand index: %d\n",
				spinand_index);
		return -1;
	}

	return 0;
}

__s32 _open_spic_ahb_gate_and_reset_v1(__u32 spinand_index)
{
	__u32 reg_val = 0;

	/*
	   1. release ahb reset and open ahb clock gate for ndfc version 1.
	   */
	if (spinand_index == 0) {
		/* reset */
		reg_val = get_wvalue(SUNXI_CCM_BASE + SPI_BUS_GATING_REG);
		reg_val &= (~(0x1U << 16));
		reg_val |= (0x1U << 16);
		put_wvalue((SUNXI_CCM_BASE + SPI_BUS_GATING_REG), reg_val);
		/* ahb clock gate */
		reg_val = get_wvalue(SUNXI_CCM_BASE + SPI_BUS_GATING_REG);
		reg_val &= (~(0x1U << 0));
		reg_val |= (0x1U << 0);
		put_wvalue((SUNXI_CCM_BASE + SPI_BUS_GATING_REG), reg_val);

	} else {
		printf("open ahb gate and reset, wrong spinand index: %d\n",
				spinand_index);
		return -1;
	}

	return 0;
}

__s32 _close_spic_ahb_gate_and_reset_v1(__u32 spinand_index)
{
	__u32 reg_val = 0;

	/*
	   1. release ahb reset and open ahb clock gate for ndfc version 1.
	   */
	if (spinand_index == 0) {
		/* reset */
		reg_val = get_wvalue(SUNXI_CCM_BASE + SPI_BUS_GATING_REG);
		reg_val &= (~(0x1U << 16));
		put_wvalue((SUNXI_CCM_BASE + SPI_BUS_GATING_REG), reg_val);
		/* ahb clock gate */
		reg_val = get_wvalue(SUNXI_CCM_BASE + SPI_BUS_GATING_REG);
		reg_val &= (~(0x1U << 0));
		put_wvalue((SUNXI_CCM_BASE + SPI_BUS_GATING_REG), reg_val);

	} else {
		printf("close ahb gate and reset, wrong spinand index: %d\n",
				spinand_index);
		return -1;
	}
	return 0;
}

int NAND_ClkRequest_v1(__u32 nand_index)
{
	__s32 ret	  = 0;
	__u32 ndfc_version = NAND_GetNdfcVersion();

	if (ndfc_version == 1) {
		if (nand_index != 0) {
			NAND_Print("NAND_ClkRequest, wrong nand index %d for "
				"ndfc version %d\n", nand_index, ndfc_version);
			return -1;
		}

		if (get_storage_type() == 1) {
			/* 1. release ahb reset and open ahb clock gate */
			_open_ndfc_ahb_gate_and_reset_v1(nand_index);

			/* 2. configure ndfc's sclk0 */
			ret = _change_ndfc_clk_v1(nand_index, 3, 10, 3, 10 * 2);
			if (ret < 0) {
				NAND_Print("NAND_ClkRequest, set dclk failed!"
						"\n");
				return -1;
			}
		} else if (get_storage_type() == 2) {
			/* 1. release ahb reset and open ahb clock gate */
			_open_spic_ahb_gate_and_reset_v1(nand_index);

			/* 2. configure spic's sclk0 */
			ret = _change_spic_clk_v1(nand_index, 3, 10);
			if (ret < 0) {
				printf("NAND_ClkRequest, set spi0 dclk failed!"
						"\n");
				return -1;
			}
		}

	} else {
		NAND_Print("NAND_ClkRequest, wrong ndfc version, %d\n",
				ndfc_version);
		return -1;
	}

	return 0;
}

void NAND_ClkRelease_v1(__u32 nand_index)
{
	__u32 ndfc_version = NAND_GetNdfcVersion();

	if (ndfc_version == 1) {
		if (nand_index != 0) {
			NAND_Print("NAND_Clkrelease, wrong nand index %d for "
				"ndfc version %d\n", nand_index, ndfc_version);
			return;
		}

		if (get_storage_type() == 1) {
			_close_ndfc_clk_v1(nand_index);

			_close_ndfc_ahb_gate_and_reset_v1(nand_index);
		} else if (get_storage_type() == 2) {
			_close_spic_clk_v1(nand_index);

			_close_spic_ahb_gate_and_reset_v1(nand_index);
		}
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
int NAND_SetClk_v1(__u32 nand_index, __u32 nand_clk0, __u32 nand_clk1)
{
	__u32 ndfc_version = NAND_GetNdfcVersion();
	__u32 dclk_src_sel, dclk, cclk_src_sel, cclk;
	__s32 ret = 0;

	if (ndfc_version == 1) {
		if (nand_index != 0) {
			printf("NAND_ClkRequest, wrong nand index %d for ndfc"
				" version %d\n", nand_index, ndfc_version);
			return -1;
		}

		dclk_src_sel = 3;
		dclk	 = nand_clk0;
		cclk_src_sel = 3;
		cclk	 = nand_clk1;

		if (get_storage_type() == 1) {
			ret = _change_ndfc_clk_v1(nand_index, dclk_src_sel,
					dclk, cclk_src_sel, cclk);
			if (ret < 0) {
				NAND_Print("NAND_SetClk, change ndfc clock"
						" failed\n");
				return -1;
			}
		} else if (get_storage_type() == 2) {
			ret = _change_spic_clk_v1(nand_index, dclk_src_sel,
					dclk);
			if (ret < 0) {
				printf("NAND_SetClk, change spic clock failed"
						"\n");
				return -1;
			}
		}
	} else {
		NAND_Print("NAND_SetClk, wrong ndfc version, %d\n",
				ndfc_version);
		return -1;
	}

	return 0;
}

int NAND_GetClk_v1(__u32 nand_index, __u32 *pnand_clk0, __u32 *pnand_clk1)
{
	__s32 ret;
	__u32 ndfc_version = NAND_GetNdfcVersion();

	if (ndfc_version == 1) {
		if (get_storage_type() == 1) {
			ret = _get_ndfc_clk_v1(nand_index, pnand_clk0,
					pnand_clk1);
			if (ret < 0) {
				NAND_Print("NAND_GetClk, failed!\n");
				return -1;
			}
		} else if (get_storage_type() == 2) {
			ret = _get_spic_clk_v1(nand_index, pnand_clk0);
			if (ret < 0) {
				printf("NAND_GetClk(spi), failed!\n");
				return -1;
			}
		}
	} else {
		NAND_Print("NAND_GetClk, wrong ndfc version, %d\n",
				ndfc_version);
		return -1;
	}

	return 0;
}
