/*
 * (C) Copyright 2017-2020
 *Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/arch/gpio.h>
#include <private_boot0.h>
#include <asm/io.h>


int SPINAND_Print(const char * str, ...);

#define get_wvalue(addr)	(*((volatile unsigned long  *)(addr)))
#define put_wvalue(addr, v)	(*((volatile unsigned long  *)(addr)) = (unsigned long)(v))

#define SPIC0_BASE_ADDR				0x05010000
#define CCMU_BASE_ADDR				(0x03001000)
#define GPIO_BASE_ADDR				(0x0300B000)

__u32 SPINAND_GetIOBaseAddr(void)
{
	return SPIC0_BASE_ADDR;
}

__u32 Getpll6Clk(void)
{
	return 600;
}

int SPINAND_ClkRequest(__u32 nand_index)
{
	__u32 cfg;

	/*reset*/
	cfg = readl(CCMU_BASE_ADDR + 0x096c);
	cfg &= (~(0x1<<16));
	writel(cfg, CCMU_BASE_ADDR + 0x096c);

	cfg = readl(CCMU_BASE_ADDR + 0x096c);
	cfg |= (0x1<<16);
	writel(cfg, CCMU_BASE_ADDR + 0x096c);

	/*open ahb*/
	cfg = readl(CCMU_BASE_ADDR + 0x096c);
	cfg |= (0x1<<0);
	writel(cfg, CCMU_BASE_ADDR + 0x096c);
	/*printf("CCMU_BASE_ADDR + 0x096c 0x%x\n",readl(CCMU_BASE_ADDR + 0x096c));*/

	return 0;
}

void SPINAND_ClkRelease(__u32 nand_index)
{
    return ;
}

/*
**********************************************************************************************************************
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
**********************************************************************************************************************
*/
int SPINAND_SetClk(__u32 nand_index, __u32 nand_clock)
{
	u32 reg_val;
	u32 sclk0_src_sel, sclk0, sclk0_src, sclk0_pre_ratio_n, sclk0_src_t, sclk0_ratio_m;
	u32 sclk0_reg_adr;

	sclk0_reg_adr = (CCMU_BASE_ADDR + 0x0940); /*CCM_SPI0_CLK_REG*/

	/*close dclk and cclk*/
	if (nand_clock == 0) {
		reg_val = readl(sclk0_reg_adr);
		reg_val &= (~(0x1U << 31));
		writel(reg_val, sclk0_reg_adr);

		return 0;
	}

	sclk0_src_sel = 1;
	sclk0 = nand_clock * 2; /*set sclk0 to 2*dclk.*/

	sclk0_src = Getpll6Clk()/1000000;

	/* sclk0: 2*dclk*/
	/*sclk0_pre_ratio_n*/
	sclk0_pre_ratio_n = 3;
	if (sclk0_src > 4*16*sclk0)
		sclk0_pre_ratio_n = 3;
	else if (sclk0_src > 2*16*sclk0)
		sclk0_pre_ratio_n = 2;
	else if (sclk0_src > 1*16*sclk0)
		sclk0_pre_ratio_n = 1;
	else
		sclk0_pre_ratio_n = 0;

	sclk0_src_t = sclk0_src >> sclk0_pre_ratio_n;

	/*sclk0_ratio_m*/
	sclk0_ratio_m = (sclk0_src_t / (sclk0)) - 1;
	if (sclk0_src_t % (sclk0))
		sclk0_ratio_m += 1;

	/*close clock*/
	reg_val = readl(sclk0_reg_adr);
	reg_val &= (~(0x1U << 31));
	writel(reg_val, sclk0_reg_adr);

	/*configure*/
	/*sclk0 <--> 2*dclk*/
	reg_val = readl(sclk0_reg_adr);
	/*clock source select*/
	reg_val &= (~(0x7 << 24));
	reg_val |= (sclk0_src_sel & 0x7) << 24;
	/*clock pre-divide ratio(N)*/
	reg_val &= (~(0x3 << 8));
	reg_val |= (sclk0_pre_ratio_n & 0x3) << 8;
	/*clock divide ratio(M)*/
	reg_val &= ~(0xf << 0);
	reg_val |= (sclk0_ratio_m & 0xf) << 0;
	writel(reg_val, sclk0_reg_adr);

	/* open clock*/
	reg_val = readl(sclk0_reg_adr);
	reg_val |= 0x1U << 31;
	writel(reg_val, sclk0_reg_adr);

	/*printf("(NAND_CLK_BASE_ADDR + 0x0940) 0x%x\n",*((__u32 *)sclk0_reg_adr));*/

	return 0;

}

int SPINAND_GetClk(__u32 nand_index)
{
	__u32 pll6_clk;
	__u32 cfg;
	__u32 nand_max_clock;
	__u32 m, n;

	/*set nand clock*/
	pll6_clk = Getpll6Clk();

    /*set nand clock gate on*/
	cfg = readl(CCMU_BASE_ADDR + 0x0940);
	m = ((cfg) & 0xf) + 1;
	n = ((cfg >> 8) & 0x3);
	nand_max_clock = pll6_clk / (2 * (1 << n) * m);
	/*printf("(NAND_CLK_BASE_ADDR + 0x0940): 0x%x\n", *(volatile __u32 *)(CCMU_BASE_ADDR + 0x0940));*/

	return nand_max_clock;
}

void SPINAND_PIORequest(__u32 nand_index)
{
	writel(0x44774474, GPIO_BASE_ADDR + 0x48);
	writel(0x47777777, GPIO_BASE_ADDR + 0x4c);
	writel(0x4, GPIO_BASE_ADDR + 0x50);
	writel(0x40005000, GPIO_BASE_ADDR + 0x64);

/*	printf("(GPIO_BASE_ADDR + 0x48): 0x%x\n", *(volatile __u32 *)(GPIO_BASE_ADDR + 0x48));
	printf("(GPIO_BASE_ADDR + 0x4c): 0x%x\n", *(volatile __u32 *)(GPIO_BASE_ADDR + 0x4c));
	printf("(GPIO_BASE_ADDR + 0x5c): 0x%x\n", *(volatile __u32 *)(GPIO_BASE_ADDR + 0x5c));
	printf("(GPIO_BASE_ADDR + 0x64): 0x%x\n", *(volatile __u32 *)(GPIO_BASE_ADDR + 0x64));
*/

}

void SPINAND_PIORelease(__u32 nand_index)
{
	return;
}


/*
************************************************************************************************************
*
*                                             OSAL_malloc
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ： 这是一个虚假的malloc函数，目的只是提供这样一个函数，避免编译不通过
*               本身不提供任何的函数功能
*
************************************************************************************************************
*/
void* SPINAND_Malloc(unsigned int Size)
{
	return (void *)CONFIG_SYS_SDRAM_BASE;
}
#if 0
__s32 SPINAND_Print(const char * str, ...)
{
	printf(str);
    return 0;
}
#endif
