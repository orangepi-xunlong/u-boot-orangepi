/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/ccmu.h>
#include <asm/arch/cpu.h>

int sunxi_clock_get_pll6(void);

/*
************************************************************************************************************
*
*                                             function
*
*    º¯ÊýÃû³Æ£º
*
*    ²ÎÊýÁÐ±í£º
*
*
*
*    ·µ»ØÖµ  £º
*
*    ËµÃ÷    £º
*
*
************************************************************************************************************
*/
int sunxi_clock_get_corepll(void)
{
	unsigned int reg_val;
	int 	div_p;
	int 	factor_n;
	int 	clock;
    /* pll = 24M*N/P */
	reg_val  = readl(CCMU_PLL_C0CPUX_CTRL_REG);
	div_p    = ((reg_val >> 16) & 0x1) == 0 ? 1:4;
	factor_n = ((reg_val >> 8) & 0xff);

	clock = 24 * factor_n/div_p;

	return clock;
}
/*
************************************************************************************************************
*
*                                             function
*
*    º¯ÊýÃû³Æ£º
*
*    ²ÎÊýÁÐ±í£º
*
*
*
*    ·µ»ØÖµ  £º
*
*    ËµÃ÷    £º
*
*
************************************************************************************************************
*/
int sunxi_clock_get_axi(void)
{
	int clock;
	unsigned int reg_val;
	int clock_src, factor;

	reg_val   = readl(CCMU_CPUX_AXI_CFG_REG);
	clock_src = (reg_val >> 12) & 0x01;
	factor    = (reg_val >> 0) & 0x03;

	if (factor >= 3) {
		factor = 4;
	} else {
		factor++;
	}

	switch (clock_src) {
	case 0:
		clock = 24;
		break;
	case 1:
		clock = sunxi_clock_get_corepll();
		break;
	default:
		return 0;
	}

	return clock/factor;
}
/*
************************************************************************************************************
*
*                                             function
*
*    º¯ÊýÃû³Æ£º
*
*    ²ÎÊýÁÐ±í£º
*
*
*
*    ·µ»ØÖµ  £º
*
*    ËµÃ÷    £º
*
*
************************************************************************************************************
*/
int sunxi_clock_get_ahb(void)
{
	unsigned int reg_val;
	int factor;
	int clock;
    int src = 0;

	reg_val = readl(CCMU_AHB1_APB1_CFG_REG);

    src = (reg_val >> 12)&0x3;
    clock = 0;
    switch (src) {
    case 1:
	    factor = ((reg_val >> 4) & 0x03);
	    clock = 24 / (2 << factor);
    case 2:
    case 3: /* src is pll6,use AHB1_PRE_DIV  * AHB_DIV_RATIO */
	    factor = ((reg_val >> 6) & 0x03) + 1;
	    factor *= (1 << ((reg_val >> 4) & 0x03));
	    clock = sunxi_clock_get_pll6() / factor;
	    break;
    }

    return clock;
}
/*
************************************************************************************************************
*
*                                             function
*
*    º¯ÊýÃû³Æ£º
*
*    ²ÎÊýÁÐ±í£º
*
*
*
*    ·µ»ØÖµ  £º
*
*    ËµÃ÷    £º
*
*
************************************************************************************************************
*/
int sunxi_clock_get_apb1(void)
{
	unsigned int reg_val;
	int          clock, factor;

	reg_val = readl(CCMU_AHB1_APB1_CFG_REG);
	factor  = (reg_val >> 8) & 0x03;
	clock   = sunxi_clock_get_ahb();

	clock = clock/(factor+1);

	return clock;
}
/*
************************************************************************************************************
*
*                                             function
*
*    º¯ÊýÃû³Æ£º
*
*    ²ÎÊýÁÐ±í£º
*
*
*
*    ·µ»ØÖµ  £º
*
*    ËµÃ÷    £º
*
*
************************************************************************************************************
*/
int sunxi_clock_get_apb2(void)
{
	return 24;
}

/*
************************************************************************************************************
*
*                                             function
*
*    º¯ÊýÃû³Æ£º
*
*    ²ÎÊýÁÐ±í£º
*
*    ·µ»ØÖµ  £º
*
*    ËµÃ÷    £º
*
*
************************************************************************************************************
*/
static int clk_set_divd(void)
{
	unsigned int reg_val;
    unsigned int clock;


    clock =  sunxi_clock_get_corepll();
	/* config axi --c0_cpux:axi0 */
	reg_val = readl(CCMU_CPUX_AXI_CFG_REG);
	reg_val &= ~(0x03 << 0);
	if (clock > 1600) {
		reg_val |= (0x03 << 0); /* axi0 clk divide ratio is 4 */
	} else if (clock > 800) {
		reg_val |= (0x02 << 0); /* axi0 clk divide ratio is 3 */
	} else if (clock > 400) {
		reg_val |= (0x01 << 0); /* axi0 clk divide ratio is2 */
	}

	writel(reg_val, CCMU_CPUX_AXI_CFG_REG);

/* config ahb -- pll6:ahb:apb = 6:2:1 */
#if 0
	reg_val = readl(CCMU_AHB1_APB1_CFG_REG);;
	reg_val &= ~((0x03 << 12) | (0x03 << 8) | (0x03 << 6) | (0x03 << 4));
	reg_val |=   (0x03 << 12);/* ahb1 clk src is pll6 */
	reg_val |=  (2 << 6);     /* ahb1 clk pre divide ratio is 3 */
	reg_val |=  (1 << 8);     /* apb1 clk divide ratio is 2 */

	writel(reg_val, CCMU_AHB1_APB1_CFG_REG);
#endif

	return 0;
}


/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :   µ±Ð¡ÓÚ288M£¬²ÉÓÃP=1³ýÆµ(¼´³ýÒÔ4)
*                      µ±´óÓÚµÈÓÚ288M£¬²ÉÓÃP=0³ýÆµ(¼´³ýÒÔ1)
*
*
************************************************************************************************************
*/
static int clk_set_pll_c0_para(int frequency)
{
	unsigned int reg_val;
	int 	div_p = 0;
	int 	factor_n;

	reg_val  = readl(CCMU_PLL_C0CPUX_CTRL_REG);

	if (frequency <= 288) {
		div_p = 1;
		frequency <<= 2;
	}
	factor_n = frequency / 24;

	reg_val &= ~(0x1ff << 8);
	reg_val |= (div_p << 16) | (factor_n << 8);

	writel(reg_val, CCMU_PLL_C0CPUX_CTRL_REG);

/* wait clock stable */
#ifndef CONFIG_A73_FPGA
	do {
		reg_val = readl(CCMU_PLL_STB_STATUS_REG);
	} while (!(reg_val & 0x1));
#endif

	return 0;
}

#if 0
static int clk_set_pll_c1_para(int frequency)
{
	unsigned int reg_val;
	int 	div_p = 0;
	int 	factor_n;

	reg_val  = readl(CCMU_PLL_C1CPUX_CTRL_REG);

	if (frequency <= 288) {
		div_p = 1;
		frequency <<= 2;
	}
	factor_n = frequency/24;

	reg_val &= ~(0x1ff << 8);
	reg_val |=  (div_p<<16) | (factor_n << 8);

	writel(reg_val, CCMU_PLL_C1CPUX_CTRL_REG);

	return 0;
}
#endif
/*
************************************************************************************************************
*
*                                             function
*
*    º¯ÊýÃû³Æ£º
*
*    ²ÎÊýÁÐ±í£º
*
*
*
*    ·µ»ØÖµ  £º
*
*    ËµÃ÷    £ºÖ»ÏÞÓÚµ÷ÕûCOREPLL£¬¹Ì¶¨·ÖÆµ±È£¬4:2:1
*
*
************************************************************************************************************
*/
int sunxi_clock_set_corepll(int frequency, int core_vol)
{
    unsigned int reg_val;
    unsigned int i;

    if (!frequency) {
	    /* Ä¬ÈÏÆµÂÊ */
	    frequency = 408;
    } else if (frequency > 3000) {
	    frequency = 3000;
    } else if (frequency < 200) {
	    frequency = 24;
    }
    /* ÇÐ»»µ½24M */
    reg_val = readl(CCMU_CPUX_AXI_CFG_REG);
    reg_val &= ~(0x01 << 12);
	/* reg_val |=  (0x00 << 12); */
    writel(reg_val, CCMU_CPUX_AXI_CFG_REG);
	/* ÑÓÊ±£¬µÈ´ýÊ±ÖÓÎÈ¶¨ */
    for (i = 0; i < 0x400; i++)
		;

    if (frequency != 24) {
	    clk_set_pll_c0_para(frequency);
    }

    /* ÐÞ¸ÄAXI,AHB,APB·ÖÆµ */
    clk_set_divd();
	/* ÇÐ»»Ê±ÖÓµ½COREPLLÉÏ,C0_CPUX clk src is pll1 */
    reg_val = readl(CCMU_CPUX_AXI_CFG_REG);
    reg_val |=  (0x01 << 12);
    writel(reg_val, CCMU_CPUX_AXI_CFG_REG);

    return  0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    º¯ÊýÃû³Æ£º
*
*    ²ÎÊýÁÐ±í£º
*
*    ·µ»ØÖµ  £º
*
*    ËµÃ÷    £º
*
*
************************************************************************************************************
*/
int sunxi_clock_get_pll6(void)
{
    unsigned int reg_val;
    int factor_n, div1, div2, pll6;
    reg_val = readl(CCMU_PLL_PERIPH_CTRL_REG);
    factor_n = ((reg_val >> 8) & 0xff);
    div1 = ((reg_val >> 16) & 0x1) + 1;
    div2 = ((reg_val >> 18) & 0x1) + 1;
    pll6 = 24 * factor_n/div1/div2;
    return pll6;
}

void sunxi_disable_dma_clock(void)
{
    unsigned int reg_val = 0;
    reg_val = readl(CCMU_BUS_CLK_GATING_REG0);
    reg_val &= ~(0x01 << 6);
    writel(reg_val , CCMU_BUS_CLK_GATING_REG0);

    return ;
}
