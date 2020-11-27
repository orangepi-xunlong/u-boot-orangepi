/*
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */
/*
**********************************************************************************************************************
*
*						           the Embedded Secure Bootloader System
*
*
*						       Copyright(C), 2006-2014, Allwinnertech Co., Ltd.
*                                           All Rights Reserved
*
* File    :
*
* By      :
*
* Version : V2.00
*
* Date	  :
*
* Descript:
**********************************************************************************************************************
*/
#include <common.h>
#include <asm/io.h>
#include <asm/arch/spc.h>

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
*    note          :
*
*
************************************************************************************************************
*/
void sunxi_spc_set_to_ns(uint type)
{
#if 0
	int i = 0;
	for (i = 0; i < 14; i++)
		writel(0xFFFFFFFF, SPC_SET_REG(i));
	printf("set non-secure periperal(ccmu)\n");

	/* set ccmu security switch: set mbus_sec bus_sec pll_sec to non-sec */
	writel(0x7, SUNXI_CCM_BASE+0xf00);

	/* set R_PRCM security switch: set power_sec  pll_sec cpus_clk to non-sec */
	writel(0x7, SUNXI_RPRCM_BASE+0x290);

	/* set dma security switch: set DMA channel0-7 to non-sec */
	writel(0xfff, SUNXI_DMA_BASE+0x20);

	/* enable non-secure access cci-400 registers */
	writel(0x1, SUNXI_CCI400_BASE + 0x8);

#endif
}

int sunxi_deassert_arisc(void)
{
	return 0;
}
