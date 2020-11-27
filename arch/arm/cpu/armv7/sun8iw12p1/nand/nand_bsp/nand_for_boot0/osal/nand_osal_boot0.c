/*
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */
/*
**********************************************************************************************************************
*											        eGon
*						           the Embedded GO-ON Bootloader System
*									       eGON arm boot sub-system
*
*						  Copyright(C), 2006-2010, SoftWinners Microelectronic Co., Ltd.
*                                           All Rights Reserved
*
* File    : nand_osal_boot0.c
*
* By      : Jerry
*
* Version : V2.00
*
* Date	  :
*
* Descript:
**********************************************************************************************************************
*/
#include "../../nand_common.h"

extern int printf(const char *fmt, ...);
#define NAND_Print(fmt, args...)        printf(fmt,##args)

#define get_wvalue(addr)	(*((volatile unsigned long  *)(addr)))
#define put_wvalue(addr, v)	(*((volatile unsigned long  *)(addr)) = (unsigned long)(v))

u32 _get_pll4_periph1_clk(void)
{
#if 1
	return 24000000;
#else
	u32 n,div1,div2;
	u32 rval;

	rval = get_wvalue((0x06000000 + 0xC)); //read pll4-periph1 register

	n = (0xff & (rval >> 8));
	div1 = (0x1 & (rval >> 16));
	div2 = (0x1 & (rval >> 18));

	rval = 24000000 * n / (div1+1) / (div2+1);;

	return rval; //24000000 * n / (div1+1) / (div2+1);
#endif
}

__u32 _Getpll6Clk(void)
{
	return 600;
}


#define NAND_CLK_BASE_ADDR (0x03001000)
int NAND_ClkRequest(__u32 nand_index)
{
	u32 reg_val;
/*
		1. release ahb reset and open ahb clock gate
*/
	if (nand_index == 0) {
		// reset nand
		reg_val = *(volatile __u32 *)(NAND_CLK_BASE_ADDR + 0x082C);
		reg_val &= (~(0x1U<<16));
		reg_val |= (0x1U<<16);
		reg_val &= (~(0x1U<<0));
		reg_val |= (0x1U<<0);
		*(volatile __u32 *)(NAND_CLK_BASE_ADDR + 0x082C) = reg_val;

		//enable nand0  mclock gating
		reg_val = *(volatile __u32 *)(NAND_CLK_BASE_ADDR + 0x0804);
		reg_val &= (~(0x1U<<5));
		reg_val |= (0x1U<<5);
		*(volatile __u32 *)(NAND_CLK_BASE_ADDR + 0x0804) = reg_val;

		//enable nand sclock gating
		reg_val = *(volatile __u32 *)(NAND_CLK_BASE_ADDR + 0x0810);

		//enable sclock gating
		reg_val &= (~(0x1U<<31));
		reg_val |= (0x1U<<31);
		//sclect the source clk :PLL_PREIO2x
		reg_val &= (~(0x7U<<24));
		reg_val |= (0x3U<<24);
		//the PLL_preio2x=1200M, setting the nand clock =10M ,
		reg_val &= (~(0x3U<<8));
		reg_val |= (0x3U<<8);
		reg_val &= (~(0xFU<<0));
		reg_val |= (0x0E<<0);
		*(volatile __u32 *)(NAND_CLK_BASE_ADDR + 0x0810) = reg_val;
	} else if (nand_index == 1) {
		// reset nand
		reg_val = *(volatile __u32 *)(NAND_CLK_BASE_ADDR + 0x082C);
		reg_val &= (~(0x1U<<16));
		reg_val |= (0x1U<<16);
		reg_val &= (~(0x1U<<0));
		reg_val |= (0x1U<<0);
		*(volatile __u32 *)(NAND_CLK_BASE_ADDR + 0x082C) = reg_val;

		//enable nand0  mclock gating
		reg_val = *(volatile __u32 *)(NAND_CLK_BASE_ADDR + 0x0804);
		reg_val &= (~(0x1U<<5));
		reg_val |= (0x1U<<5);
		*(volatile __u32 *)(NAND_CLK_BASE_ADDR + 0x0804) = reg_val;

		//enable nand sclock gating
		reg_val = *(volatile __u32 *)(NAND_CLK_BASE_ADDR + 0x0814);

		//enable sclock gating
		reg_val &= (~(0x1U<<31));
		reg_val |= (0x1U<<31);
		//sclect the source clk :PLL_PREIO2x
		reg_val &= (~(0x7U<<24));
		reg_val |= (0x3U<<24);
		//the PLL_preio2x=1200M, setting the nand clock =10M ,
		reg_val &= (~(0x3U<<8));
		reg_val |= (0x3U<<8);

		reg_val &= (~(0xFU<<0));
		reg_val |= (0x0E<<0);

		*(volatile __u32 *)(NAND_CLK_BASE_ADDR + 0x0814) = reg_val;
	} else {
		NAND_Print("NAND_ClkRequest error--1, wrong nand index: %d\n", nand_index);
		return -1;
	}

	return 0;
}


void NAND_ClkRelease(__u32 nand_index)
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

int NAND_SetClk(__u32 nand_index, __u32 nand_clock)
{
	__u32 edo_clk;
	__u32 cfg;
	__u32 m = 0, n = 0;
	__u32 clk_src;


	if(0==nand_index)
		clk_src = ((*(volatile __u32 *)(NAND_CLK_BASE_ADDR + 0x0810))>>24)&0x07;
	else
		clk_src = ((*(volatile __u32 *)(NAND_CLK_BASE_ADDR + 0x0814))>>24)&0x07;

	if(clk_src == 0)
	{
	}
	else
	{
		edo_clk = nand_clock * 2;
		if(edo_clk <= 20)  //10M
		{
			n =  3;
			m = 14;
		}
		else if((edo_clk >20)&&(edo_clk <= 40))  //20M
		{
			n =  2;
			m = 14;
		}
		else if((edo_clk >40)&&(edo_clk <= 50))  //25M
		{
			n =  1;
			m = 13;
		}
		else if((edo_clk >50)&&(edo_clk <= 60))  //30M
		{
			n = 2;
			m = 9;
		}
		else //40M
		{
			n = 1;
			m = 14;
		}
//if the source clock is not 2x ,
		if(clk_src<3)
		{
			n--;
		}
	}

	if(nand_index == 0)
	{
		/*set nand clock*/
		/*set nand clock gate on*/
		cfg = *(volatile __u32 *)(NAND_CLK_BASE_ADDR + 0x0810);
		cfg &= (~(0x03 << 8));
		cfg &= (~(0xf));
		cfg |= ((n&0x3)<<8);
		cfg |= ((m&0xf));
		*(volatile __u32 *)(NAND_CLK_BASE_ADDR + 0x0810) = cfg;

		NAND_Print("NAND_SetClk, nand_index: 0x%x\n", nand_index);
		NAND_Print("Reg 0x01c20080: 0x%x\n", *(volatile __u32 *)(0x01c20080));

	}
	else
	{
		/*set nand clock*/
		/*set nand clock gate on*/
		cfg = *(volatile __u32 *)(NAND_CLK_BASE_ADDR + 0x0814);
		cfg &= (~(0x03 << 8));
		cfg &= (~(0xf));
		cfg |= ((n&0x3)<<8);
		cfg |= ((m&0xf));
		*(volatile __u32 *)(NAND_CLK_BASE_ADDR + 0x0814) = cfg;

		NAND_Print("NAND_SetClk, nand_index: 0x%x\n", nand_index);
		NAND_Print("Reg 0x01c20080: 0x%x\n", *(volatile __u32 *)(0x01c20080));
	}

	return 0;
}



int NAND_GetClk(__u32 nand_index)
{
	__u32 pll6_clk;
	__u32 cfg;
	__u32 nand_max_clock;
	__u32 m, n;

	if(nand_index == 0)
	{
		/*set nand clock*/
		pll6_clk = _Getpll6Clk();

		/*set nand clock gate on*/
		cfg = *(volatile __u32 *)(0x01c20000 + 0x80);
		m = ((cfg)&0xf) +1;
		n = ((cfg>>16)&0x3);
		nand_max_clock = pll6_clk/(2*(1<<n)*(m+1));
		NAND_Print("NAND_GetClk, nand_index: 0x%x, nand_clk: %dM\n", nand_index, nand_max_clock);
		NAND_Print("Reg 0x01c20080: 0x%x\n", *(volatile __u32 *)(0x01c20080));
	}
	else
	{
		/*set nand clock*/
		pll6_clk = _Getpll6Clk();

		/*set nand clock gate on*/
		cfg = *(volatile __u32 *)(0x01c20000 + 0x84);
		m = ((cfg)&0xf) +1;
		n = ((cfg>>16)&0x3);
		nand_max_clock = pll6_clk/(2*(1<<n)*(m+1));
		NAND_Print("NAND_GetClk, nand_index: 0x%x, nand_clk: %dM\n", nand_index, nand_max_clock);
		NAND_Print("Reg 0x01c20084: 0x%x\n", *(volatile __u32 *)(0x01c20084));
	}

	return nand_max_clock;
}


#define NAND_PIO_BASE_ADDRESS (0x0300B000)
void NAND_PIORequest(__u32 nand_index)
{
	__u32 cfg;

	if(nand_index == 0)
	{
		//setting PC0 port as Nand control line
		*(volatile __u32 *)(NAND_PIO_BASE_ADDRESS + 0x48) = 0x22222222;
		//setting PC1 port as Nand data line
		*(volatile __u32 *)(NAND_PIO_BASE_ADDRESS + 0x4c) = 0x22222222;
		//setting PC2 port as Nand RB1
		cfg = *(volatile __u32 *)(NAND_PIO_BASE_ADDRESS + 0x50);
		cfg &= (~0x7);
		cfg |= 0x2;
		*(volatile __u32 *)(NAND_PIO_BASE_ADDRESS + 0x50) = cfg;

		//pull-up/down --only setting RB & CE pin pull-up
		*(volatile __u32 *)(NAND_PIO_BASE_ADDRESS + 0x64) = 0x40000440;
		cfg = *(volatile __u32 *)(NAND_PIO_BASE_ADDRESS + 0x68);
		cfg &= (~0x3);
		cfg |= 0x01;
		*(volatile __u32 *)(NAND_PIO_BASE_ADDRESS + 0x68) = cfg;

	}
	else if(nand_index == 1)
	{
		NAND_Print("NAND_PIORequest error, wrong nand_index: 0x%x\n", nand_index);
	}
	else
	{
		NAND_Print("NAND_PIORequest error, wrong nand_index: 0x%x\n", nand_index);
	}
}


void NAND_PIORelease(__u32 nand_index)
{
	return;
}

void NAND_EnRbInt(void)
{
	return ;
}


void NAND_ClearRbInt(void)
{
	return ;
}

int NAND_WaitRbReady(void)
{
	return 0;
}

int NAND_WaitDmaFinish(void)
{
	return 0;
}

void NAND_RbInterrupt(void)
{
	return ;
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
void* NAND_Malloc(unsigned int Size)
{
	//return (void *)malloc(Size);
	return (void *)CONFIG_SYS_SDRAM_BASE;
}
/*
************************************************************************************************************
*
*                                             OSAL_free
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ： 这是一个虚假的free函数，目的只是提供这样一个函数，避免编译不通过
*               本身不提供任何的函数功能
*
************************************************************************************************************
*/
void NAND_Free(void *pAddr, unsigned int Size)
{
	//return free(pAddr);
}

void *NAND_IORemap(unsigned int base_addr, unsigned int size)
{
	return (void *)base_addr;
}

