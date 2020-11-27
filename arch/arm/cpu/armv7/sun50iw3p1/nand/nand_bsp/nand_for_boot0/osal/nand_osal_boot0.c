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

#define get_wvalue(addr)	readl(addr)
#define put_wvalue(addr, v)	 writel(v,addr)
#define NAND_CLK_BASE_ADDR (0x03001000)

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

__u32 _Get_PLL_PERI(void)
{
	__u32 reg_val;
	__u32 factor_n;
	__u32 factor_m0;
	__u32 factor_m1;
	__u32 clock;

	reg_val  = get_wvalue(NAND_CLK_BASE_ADDR + 0x20);
	factor_n = ((reg_val >> 8) & 0xFF) + 1;
	factor_m0 = ((reg_val >> 0) & 0x1) + 1;
	factor_m1 = ((reg_val >> 1) & 0x1) + 1;
	clock = 24000000 * factor_n / factor_m0/factor_m1/4;

	return clock;
}


int _change_ndfc_clk_v1(__u32 nand_index, __u32 dclk_src_sel, __u32 dclk, __u32 cclk_src_sel, __u32 cclk)
{
	u32 reg_val;
	u32 sclk0_src_sel, sclk0, sclk0_src, sclk0_pre_ratio_n, sclk0_src_t, sclk0_ratio_m;
	u32 sclk1_src_sel, sclk1, sclk1_src, sclk1_pre_ratio_n, sclk1_src_t, sclk1_ratio_m;
	u32 sclk0_reg_adr, sclk1_reg_adr;

	if (nand_index == 0) {
		sclk0_reg_adr = (NAND_CLK_BASE_ADDR + 0x0810); //CCM_NAND0_CLK0_REG;
		sclk1_reg_adr = (NAND_CLK_BASE_ADDR + 0x0814); //CCM_NAND0_CLK1_REG;
	}  else {
		printf("_change_ndfc_clk_v1 error, wrong nand index: %d\n", nand_index);
		return -1;
	}

	/*close dclk and cclk*/
	if ((dclk == 0) && (cclk == 0))
	{
		reg_val = get_wvalue(sclk0_reg_adr);
		reg_val &= (~(0x1U<<31));
		put_wvalue(sclk0_reg_adr, reg_val);

		reg_val = get_wvalue(sclk1_reg_adr);
		reg_val &= (~(0x1U<<31));
		put_wvalue(sclk1_reg_adr, reg_val);

		printf("_change_ndfc_clk_v1, close sclk0 and sclk1\n");
		return 0;
	}

	sclk0_src_sel = dclk_src_sel;
	sclk0 = dclk;
	sclk1_src_sel = cclk_src_sel;
	sclk1 = cclk;

	if(sclk0_src_sel == 0x0) {
		//osc pll
        sclk0_src = 24;
	} else if(sclk0_src_sel < 0x3)
		sclk0_src = _Get_PLL_PERI()/1000000;
	else
		sclk0_src = 2*_Get_PLL_PERI()/1000000;

	if(sclk1_src_sel == 0x0) {
		//osc pll
		sclk1_src = 24;
	} else if(sclk1_src_sel < 0x3)
		sclk1_src = _Get_PLL_PERI()/1000000;
	else
		sclk1_src = 2*_Get_PLL_PERI()/1000000;

	//////////////////// sclk0: 2*dclk
	//sclk0_pre_ratio_n
	sclk0_pre_ratio_n = 3;
	if(sclk0_src > 4*16*sclk0)
		sclk0_pre_ratio_n = 3;
	else if (sclk0_src > 2*16*sclk0)
		sclk0_pre_ratio_n = 2;
	else if (sclk0_src > 1*16*sclk0)
		sclk0_pre_ratio_n = 1;
	else
		sclk0_pre_ratio_n = 0;

	sclk0_src_t = sclk0_src>>sclk0_pre_ratio_n;

	//sclk0_ratio_m
	sclk0_ratio_m = (sclk0_src_t/(sclk0)) - 1;
	if( sclk0_src_t%(sclk0) )
	sclk0_ratio_m +=1;

	//////////////// sclk1: cclk
	//sclk1_pre_ratio_n
	sclk1_pre_ratio_n = 3;
	if(sclk1_src > 4*16*sclk1)
		sclk1_pre_ratio_n = 3;
	else if (sclk1_src > 2*16*sclk1)
		sclk1_pre_ratio_n = 2;
	else if (sclk1_src > 1*16*sclk1)
		sclk1_pre_ratio_n = 1;
	else
		sclk1_pre_ratio_n = 0;

	sclk1_src_t = sclk1_src>>sclk1_pre_ratio_n;

	//sclk1_ratio_m
	sclk1_ratio_m = (sclk1_src_t/(sclk1)) - 1;
	if( sclk1_src_t%(sclk1) )
	sclk1_ratio_m +=1;

	/////////////////////////////// close clock
	reg_val = get_wvalue(sclk0_reg_adr);
	reg_val &= (~(0x1U<<31));
	put_wvalue(sclk0_reg_adr, reg_val);

	reg_val = get_wvalue(sclk1_reg_adr);
	reg_val &= (~(0x1U<<31));
	put_wvalue(sclk1_reg_adr, reg_val);

	///////////////////////////////configure
	//sclk0 <--> 2*dclk
	reg_val = get_wvalue(sclk0_reg_adr);
	//clock source select
	reg_val &= (~(0x7<<24));
	reg_val |= (sclk0_src_sel&0x7)<<24;
	//clock pre-divide ratio(N)
	reg_val &= (~(0x3<<8));
	reg_val |= (sclk0_pre_ratio_n&0x3)<<8;
	//clock divide ratio(M)
	reg_val &= ~(0xf<<0);
	reg_val |= (sclk0_ratio_m&0xf)<<0;
	put_wvalue(sclk0_reg_adr, reg_val);

	//sclk1 <--> cclk
	reg_val = get_wvalue(sclk1_reg_adr);
	//clock source select
	reg_val &= (~(0x7<<24));
	reg_val |= (sclk1_src_sel&0x7)<<24;
	//clock pre-divide ratio(N)
	reg_val &= (~(0x3<<8));
	reg_val |= (sclk1_pre_ratio_n&0x3)<<8;
	//clock divide ratio(M)
	reg_val &= ~(0xf<<0);
	reg_val |= (sclk1_ratio_m&0xf)<<0;
	put_wvalue(sclk1_reg_adr, reg_val);

	/////////////////////////////// open clock
	reg_val = get_wvalue(sclk0_reg_adr);
	reg_val |= 0x1U<<31;
	put_wvalue(sclk0_reg_adr, reg_val);

	reg_val = get_wvalue(sclk1_reg_adr);
	reg_val |= 0x1U<<31;
	put_wvalue(sclk1_reg_adr, reg_val);

	if (nand_index == 0) {
		sclk0_ratio_m = (get_wvalue(sclk0_reg_adr)&0xf) +1;
		sclk0_pre_ratio_n = 1<<((get_wvalue(sclk0_reg_adr)>>8)&0x3);

		sclk1_ratio_m = (get_wvalue(sclk1_reg_adr)&0xf) +1;
		sclk1_pre_ratio_n = 1<<((get_wvalue(sclk1_reg_adr)>>8)&0x3);
		NAND_Print("boot0 Nand0 clk: %dMHz,PERI=%d,N=%d,M=%d,T=%d\n",
		sclk0_src/sclk0_ratio_m/sclk0_pre_ratio_n,
		sclk0_src,sclk0_pre_ratio_n,sclk0_ratio_m,dclk);
		NAND_Print("boot0 Nand Ecc clk: %dMHz,PERI=%d,N=%d,M=%d,T=%d\n",
		sclk1_src/sclk1_ratio_m/sclk1_pre_ratio_n,
		sclk1_src,sclk1_pre_ratio_n,sclk1_ratio_m,cclk);

	} else {
		printf("change nfdc clk error, wrong nand index: %d\n", nand_index);
	}

	return 0;
}

int _close_ndfc_clk_v1(__u32 nand_index)
{
	u32 reg_val;
	u32 sclk0_reg_adr, sclk1_reg_adr;

	if (nand_index == 0) {
		//disable nand sclock gating
		sclk0_reg_adr = (NAND_CLK_BASE_ADDR + 0x0810); //CCM_NAND0_CLK0_REG;
		sclk1_reg_adr = (NAND_CLK_BASE_ADDR + 0x0814); //CCM_NAND0_CLK1_REG;

		reg_val = get_wvalue(sclk0_reg_adr);
		reg_val &= (~(0x1U<<31));
		put_wvalue(sclk0_reg_adr, reg_val);

		reg_val = get_wvalue(sclk1_reg_adr);
		reg_val &= (~(0x1U<<31));
		put_wvalue(sclk1_reg_adr, reg_val);
	} else {
		printf("close_ndfc_clk error, wrong nand index: %d\n", nand_index);
		return -1;
	}

	return 0;
}

int _open_ndfc_ahb_gate_and_reset_v1(__u32 nand_index)
{
	u32 reg_val=0;

	/*
	1. release ahb reset and open ahb clock gate for ndfc version 1.
	*/
	if (nand_index == 0) {
		// reset
		reg_val = get_wvalue(NAND_CLK_BASE_ADDR + 0x082C);
		reg_val &= (~(0x1U<<16));
		reg_val |= (0x1U<<16);
		put_wvalue((NAND_CLK_BASE_ADDR + 0x082C),reg_val);
		// ahb clock gate
		reg_val = get_wvalue(NAND_CLK_BASE_ADDR + 0x082C);
		reg_val &= (~(0x1U<<0));
		reg_val |= (0x1U<<0);
		put_wvalue((NAND_CLK_BASE_ADDR + 0x082C),reg_val);

		// enable nand mbus gate
		reg_val = get_wvalue(NAND_CLK_BASE_ADDR + 0x0804);
		reg_val &= (~(0x1U<<5));
		reg_val |= (0x1U<<5);
		put_wvalue((NAND_CLK_BASE_ADDR + 0x0804),reg_val);
	} else {
		printf("open ahb gate and reset, wrong nand index: %d\n", nand_index);
		return -1;
	}

	return 0;
}

int _close_ndfc_ahb_gate_and_reset_v1(__u32 nand_index)
{
	u32 reg_val=0;
	/*
	1. release ahb reset and open ahb clock gate for ndfc version 1.
	*/
	if (nand_index == 0) {
		// reset
		reg_val = get_wvalue(NAND_CLK_BASE_ADDR + 0x082C);
		reg_val &= (~(0x1U<<16));
		put_wvalue((NAND_CLK_BASE_ADDR + 0x082C),reg_val);
		// ahb clock gate
		reg_val = get_wvalue(NAND_CLK_BASE_ADDR + 0x082C);
		reg_val &= (~(0x1U<<0));
		put_wvalue((NAND_CLK_BASE_ADDR + 0x082C),reg_val);

		// disable nand mbus gate
		reg_val = get_wvalue(NAND_CLK_BASE_ADDR + 0x0804);
		reg_val &= (~(0x1U<<5));
		put_wvalue((NAND_CLK_BASE_ADDR + 0x0804),reg_val);

	} else {
		NAND_Print("close ahb gate and reset, wrong nand index:%d\n", nand_index);
		return -1;
	}
	return 0;
}

int NAND_ClkRequest(__u32 nand_index)
{
    int  ret = 0;
    if (nand_index != 0) {
        NAND_Print("NAND_ClkRequest, wrong nand index %d \n", nand_index);
        return -1;
    }
    // 1. release ahb reset and open ahb clock gate
    _open_ndfc_ahb_gate_and_reset_v1(nand_index);
    // 2. configure ndfc's sclk0
    ret = _change_ndfc_clk_v1(nand_index, 3, 10, 3, 10*2);
    if (ret<0) {
        NAND_Print("NAND_ClkRequest, set dclk failed!\n");
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
		NAND_Print("Reg (NAND_CLK_BASE_ADDR + 0x0810): 0x%x\n", *(volatile __u32 *)(NAND_CLK_BASE_ADDR + 0x0810));

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
		NAND_Print("Reg NAND_CLK_BASE_ADDR + 0x0814: 0x%x\n", *(volatile __u32 *)(NAND_CLK_BASE_ADDR + 0x0814));
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

