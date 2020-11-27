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
#include "nand_uboot_platform.h"
#include "../../../nand_interface/nand_bsp.h"

extern int NAND_Print_DBG(const char *fmt, ...);
extern int NAND_Print(const char *fmt, ...);

extern __u32 get_wvalue(__u32 addr);

extern void put_wvalue(__u32 addr,__u32 v);

int NAND_IS_Secure_sys(void);
int NAND_Get_Platform_NO(void);
int NAND_Get_Boot0_Acess_Pagetable_Mode(void);
int nand_print_platform(void);

__u32 _Getpll6Clk(void);
__u32 _Getpll6Clk_h5_a33(void);
__u32 _Getpll6Clk_h8(void);
__u32 _Getpll6Clk_b100(void);

__u32 _cfg_ndfc_gpio_v1(__u32 nand_index);
__s32 _cfg_ndfc_gpio_v1_h5_a33(__u32 nand_index);
__s32 _cfg_ndfc_gpio_v1_b100(__u32 nand_index);
__s32 _cfg_ndfc_gpio_v1_v40(__u32 nand_index);

__s32 _get_ndfc_clk_v1(__u32 nand_index, __u32 *pdclk);
__s32 _get_ndfc_clk_v1_h5_a33(__u32 nand_index, __u32 *pdclk);
__s32 _get_ndfc_clk_v1_b100(__u32 nand_index, __u32 *pdclk);

__s32 _change_ndfc_clk_v1(__u32 nand_index, __u32 dclk_src_sel, __u32 dclk);
__s32 _change_ndfc_clk_v1_h5_a33(__u32 nand_index, __u32 dclk_src_sel, __u32 dclk);
__s32 _change_ndfc_clk_v1_b100(__u32 nand_index, __u32 dclk_src_sel, __u32 dclk);


DECLARE_GLOBAL_DATA_PTR;

/*****************************************************************************
//int sunxi_get_securemode(void)
//return 0:normal mode
//rerurn 1:secure mode ,but could change clock
	//return !0 && !1: secure mode ,and couldn't change clock=

*****************************************************************************/
int NAND_IS_Secure_sys(void)
{
	if(PLATFORM_CLASS == 0)
	{
	    int mode=0;
        int toc_file = (gd->bootfile_mode == SUNXI_BOOT_FILE_TOC);
	    mode = sunxi_get_securemode() || toc_file;
	    if(mode==0) //normal mode
	    {
		    return 0;
		}
	    else if((mode==1)||(mode==2))//secure
	    {
		    return 1;
		}
		else
		{
            return 0;
		}
	}
	else if(PLATFORM_CLASS == 1)
	{
	    return 1;
	}
	else
	{
	    return -1;
	}
}

/*****************************************************************************

*****************************************************************************/
int NAND_Get_Boot0_Acess_Pagetable_Mode(void)
{
	return PLATFORM_BOOT0_ACESS_PAGE_TABLE_MODE;
}

/*****************************************************************************

*****************************************************************************/
int NAND_Get_Platform_NO(void)
{
	return PLATFORM_NO;
}
/*****************************************************************************

*****************************************************************************/
int nand_print_platform(void)
{
	NAND_Print(PLATFORM_STRINGS);
	NAND_Print("\n");
	return 0;
}

/*****************************************************************************

*****************************************************************************/
__u32 _Getpll6Clk(void)
{
    if(PLATFORM_NO == 0)
    {
        return _Getpll6Clk_h5_a33();
    }
    else if(PLATFORM_NO == 1)
    {
        return _Getpll6Clk_h5_a33();
    }
    else if(PLATFORM_NO == 2)
    {
        return _Getpll6Clk_h8();
    }
    else if(PLATFORM_NO == 3)
    {
        return _Getpll6Clk_b100();
    }
    else if(PLATFORM_NO == 4)
    {
        return _Getpll6Clk_h5_a33();
    }
    else
    {
        return 0xff;
    }
}

/*****************************************************************************

*****************************************************************************/
__u32 _cfg_ndfc_gpio_v1(__u32 nand_index)
{
    if(PLATFORM_NO == 0)
    {
        return _cfg_ndfc_gpio_v1_h5_a33(nand_index);
    }
    else if(PLATFORM_NO == 1)
    {
        return _cfg_ndfc_gpio_v1_h5_a33(nand_index);
    }
    else if(PLATFORM_NO == 2)
    {
        return _cfg_ndfc_gpio_v1_h5_a33(nand_index);
    }
    else if(PLATFORM_NO == 3)
    {
        return _cfg_ndfc_gpio_v1_b100(nand_index);
    }
    else if(PLATFORM_NO == 4)
    {
        return _cfg_ndfc_gpio_v1_v40(nand_index);
    }
    else
    {
        return 0xff;
    }
}
/*****************************************************************************

*****************************************************************************/
__s32 _get_ndfc_clk_v1(__u32 nand_index, __u32 *pdclk)
{
    if(PLATFORM_NO == 0)
    {
        return _get_ndfc_clk_v1_h5_a33(nand_index,pdclk);
    }
    else if(PLATFORM_NO == 1)
    {
        return _get_ndfc_clk_v1_h5_a33(nand_index,pdclk);
    }
    else if(PLATFORM_NO == 2)
    {
        return _get_ndfc_clk_v1_h5_a33(nand_index,pdclk);
    }
    else if(PLATFORM_NO == 3)
    {
        return _get_ndfc_clk_v1_b100(nand_index,pdclk);
    }
    else if(PLATFORM_NO == 4)
    {
        return _get_ndfc_clk_v1_h5_a33(nand_index,pdclk);
    }
    else
    {
        return 0xff;
    }
}
/*****************************************************************************

*****************************************************************************/
__s32 _change_ndfc_clk_v1(__u32 nand_index, __u32 dclk_src_sel, __u32 dclk)
{
    if(PLATFORM_NO == 0)
    {
        return _change_ndfc_clk_v1_h5_a33(nand_index,dclk_src_sel,dclk);
    }
    else if(PLATFORM_NO == 1)
    {
        return _change_ndfc_clk_v1_h5_a33(nand_index,dclk_src_sel,dclk);
    }
    else if(PLATFORM_NO == 2)
    {
        return _change_ndfc_clk_v1_h5_a33(nand_index,dclk_src_sel,dclk);
    }
    else if(PLATFORM_NO == 3)
    {
        return _change_ndfc_clk_v1_b100(nand_index,dclk_src_sel,dclk);
    }
    else if(PLATFORM_NO == 4)
    {
        return _change_ndfc_clk_v1_h5_a33(nand_index,dclk_src_sel,dclk);
    }
    else
    {
        return 0xff;
    }
}


/*****************************************************************************

*****************************************************************************/
__u32 _Getpll6Clk_h5_a33(void)
{
	__u32 reg_val;
	__u32 factor_n;
	__u32 factor_k;
	__u32 clock;

	reg_val  = get_wvalue(0x01c20000 + 0x28);
	factor_n = ((reg_val >> 8) & 0x1f) + 1;
	factor_k = ((reg_val >> 4) & 0x3) + 1;
	//div_m = ((reg_val >> 0) & 0x3) + 1;

	clock = 24000000 * factor_n * factor_k/2;
	//NAND_Print("pll6 clock is %d Hz\n", clock);
	//if(clock != 600000000)
		//printf("pll6 clock rate error, %d!!!!!!!\n", clock);

	return clock;
}
/*****************************************************************************

*****************************************************************************/
__s32 _get_ndfc_clk_v1_h5_a33(__u32 nand_index, __u32 *pdclk)
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
		sclk0_reg_adr = (0x01c20000 + 0x80); //CCM_NAND0_CLK0_REG;
	} else if (nand_index == 1) {
		sclk0_reg_adr = (0x01c20000 + 0x84); //CCM_NAND1_CLK0_REG;
	}

	// sclk0
	reg_val = get_wvalue(sclk0_reg_adr);
	sclk_src_sel     = (reg_val>>24) & 0x3;
	sclk_pre_ratio_n = (reg_val>>16) & 0x3;;
	sclk_ratio_m     = (reg_val) & 0xf;
	if (sclk_src_sel == 0)
		sclk_src = 24;
	else
		sclk_src = _Getpll6Clk()/1000000;
	sclk0 = (sclk_src >> sclk_pre_ratio_n) / (sclk_ratio_m+1);

	if (nand_index == 0) {
		//NAND_Print("Reg 0x01c20080: 0x%x\n", get_wvalue(0x01c20080));
	} else {
		//NAND_Print("Reg 0x01c20084: 0x%x\n", get_wvalue(0x01c20084));
	}
	//NAND_Print("NDFC%d:  sclk0(2*dclk): %d MHz\n", nand_index, sclk0);

	*pdclk = sclk0/2;

	return 0;
}
/*****************************************************************************

*****************************************************************************/
__s32 _change_ndfc_clk_v1_h5_a33(__u32 nand_index, __u32 dclk_src_sel, __u32 dclk)
{
	u32 reg_val;
	u32 sclk0_src_sel, sclk0, sclk0_src, sclk0_pre_ratio_n, sclk0_src_t, sclk0_ratio_m;
	u32 sclk0_reg_adr;

	if (nand_index == 0) {
		sclk0_reg_adr = (0x01c20000 + 0x80); //CCM_NAND0_CLK0_REG;
	} else if (nand_index == 1) {
		sclk0_reg_adr = (0x01c20000 + 0x84); //CCM_NAND1_CLK0_REG;
	} else {
		printf("_change_ndfc_clk error, wrong nand index: %d\n", nand_index);
		return -1;
	}

	/*close dclk and cclk*/
	if (dclk == 0)
	{
		reg_val = get_wvalue(sclk0_reg_adr);
		reg_val &= (~(0x1U<<31));
		put_wvalue(sclk0_reg_adr, reg_val);

		//printf("_change_ndfc_clk, close sclk0 and sclk1\n");
		return 0;
	}

	sclk0_src_sel = dclk_src_sel;
	sclk0 = dclk*2; //set sclk0 to 2*dclk.

	if(sclk0_src_sel == 0x0) {
		//osc pll
        sclk0_src = 24;
	} else {
		//pll6 for ndfc version 1
		sclk0_src = _Getpll6Clk()/1000000;
	}

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

	/////////////////////////////// close clock
	reg_val = get_wvalue(sclk0_reg_adr);
	reg_val &= (~(0x1U<<31));
	put_wvalue(sclk0_reg_adr, reg_val);

	///////////////////////////////configure
	//sclk0 <--> 2*dclk
	reg_val = get_wvalue(sclk0_reg_adr);
	//clock source select
	reg_val &= (~(0x3<<24));
	reg_val |= (sclk0_src_sel&0x3)<<24;
	//clock pre-divide ratio(N)
	reg_val &= (~(0x3<<16));
	reg_val |= (sclk0_pre_ratio_n&0x3)<<16;
	//clock divide ratio(M)
	reg_val &= ~(0xf<<0);
	reg_val |= (sclk0_ratio_m&0xf)<<0;
	put_wvalue(sclk0_reg_adr, reg_val);

	/////////////////////////////// open clock
	reg_val = get_wvalue(sclk0_reg_adr);
	reg_val |= 0x1U<<31;
	put_wvalue(sclk0_reg_adr, reg_val);

	//NAND_Print("NAND_SetClk for nand index %d \n", nand_index);
	if (nand_index == 0) {
		//NAND_Print("Reg 0x01c20080: 0x%x\n", get_wvalue(0x01c20080));
	} else {
		//NAND_Print("Reg 0x01c20084: 0x%x\n", get_wvalue(0x01c20084));
	}

	return 0;
}

/*****************************************************************************

*****************************************************************************/
__s32 _cfg_ndfc_gpio_v1_h5_a33(__u32 nand_index)
{
	if (nand_index == 0) {
		*(volatile __u32 *)(0x01c20800 + 0x48) = 0x22222222;
		*(volatile __u32 *)(0x01c20800 + 0x4c) = 0x22222222;
		*(volatile __u32 *)(0x01c20800 + 0x50) = 0x222;
		NAND_Print("NAND_PIORequest, nand_index: 0x%x\n", nand_index);
		NAND_Print("Reg 0x01c20848: 0x%x\n", *(volatile __u32 *)(0x01c20848));
		NAND_Print("Reg 0x01c2084c: 0x%x\n", *(volatile __u32 *)(0x01c2084c));
		NAND_Print("Reg 0x01c20850: 0x%x\n", *(volatile __u32 *)(0x01c20850));
	} else {
		printf("_cfg ndfc gpio v1, wrong nand index %d\n", nand_index);
		return -1;
	}

	return 0;
}
/*****************************************************************************

*****************************************************************************/
__u32 _Getpll6Clk_b100(void)
{
	__u32 reg_val;
	__u32 factor_n;
	__u32 factor_k;
	__u32 clock;

	reg_val  = get_wvalue(0x01c20000 + 0x28);
	factor_n = ((reg_val >> 8) & 0x1f) + 1;
	factor_k = ((reg_val >> 4) & 0x3) + 1;
	//div_m = ((reg_val >> 0) & 0x3) + 1;

	clock = 24000000 * factor_n * factor_k;
	//NAND_Print("pll6 clock is %d Hz\n", clock);
	//if(clock != 600000000)
		//printf("pll6 clock rate error, %d!!!!!!!\n", clock);

	return clock;
}
/*****************************************************************************

*****************************************************************************/
__s32 _get_ndfc_clk_v1_b100(__u32 nand_index, __u32 *pdclk)
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
		sclk0_reg_adr = (0x01c20000 + 0x80); //CCM_NAND0_CLK0_REG;
	} else if (nand_index == 1) {
		sclk0_reg_adr = (0x01c20000 + 0x84); //CCM_NAND1_CLK0_REG;
	}

	// sclk0
	reg_val = get_wvalue(sclk0_reg_adr);
	sclk_src_sel     = (reg_val>>24) & 0x3;
	sclk_pre_ratio_n = (reg_val>>16) & 0x3;;
	sclk_ratio_m     = (reg_val) & 0xf;
	if (sclk_src_sel == 0)
		sclk_src = 26;
	else
		sclk_src = _Getpll6Clk()/1000000;
	sclk0 = (sclk_src >> sclk_pre_ratio_n) / (sclk_ratio_m+1);

	if (nand_index == 0) {
		//NAND_Print("Reg 0x01c20080: 0x%x\n", get_wvalue(0x01c20080));
	} else {
		//NAND_Print("Reg 0x01c20084: 0x%x\n", get_wvalue(0x01c20084));
	}
	//NAND_Print("NDFC%d:  sclk0(2*dclk): %d MHz\n", nand_index, sclk0);

	*pdclk = sclk0/2;

	return 0;
}
/*****************************************************************************

*****************************************************************************/
__s32 _change_ndfc_clk_v1_b100(__u32 nand_index, __u32 dclk_src_sel, __u32 dclk)
{
	u32 reg_val;
	u32 sclk0_src_sel, sclk0, sclk0_src, sclk0_pre_ratio_n, sclk0_src_t, sclk0_ratio_m;
	u32 sclk0_reg_adr;

	if (nand_index == 0) {
		sclk0_reg_adr = (0x01c20000 + 0x80); //CCM_NAND0_CLK0_REG;
	} else if (nand_index == 1) {
		sclk0_reg_adr = (0x01c20000 + 0x84); //CCM_NAND1_CLK0_REG;
	} else {
		printf("_change_ndfc_clk error, wrong nand index: %d\n", nand_index);
		return -1;
	}

	/*close dclk and cclk*/
	if (dclk == 0)
	{
		reg_val = get_wvalue(sclk0_reg_adr);
		reg_val &= (~(0x1U<<31));
		put_wvalue(sclk0_reg_adr, reg_val);

		//printf("_change_ndfc_clk, close sclk0 and sclk1\n");
		return 0;
	}

	sclk0_src_sel = dclk_src_sel;
	sclk0 = dclk*2; //set sclk0 to 2*dclk.

	if(sclk0_src_sel == 0x0) {
		//osc pll
        sclk0_src = 26;
	} else {
		//pll6 for ndfc version 1
		sclk0_src = _Getpll6Clk()/1000000;
	}

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

	/////////////////////////////// close clock
	reg_val = get_wvalue(sclk0_reg_adr);
	reg_val &= (~(0x1U<<31));
	put_wvalue(sclk0_reg_adr, reg_val);

	///////////////////////////////configure
	//sclk0 <--> 2*dclk
	reg_val = get_wvalue(sclk0_reg_adr);
	//clock source select
	reg_val &= (~(0x3<<24));
	reg_val |= (sclk0_src_sel&0x3)<<24;
	//clock pre-divide ratio(N)
	reg_val &= (~(0x3<<16));
	reg_val |= (sclk0_pre_ratio_n&0x3)<<16;
	//clock divide ratio(M)
	reg_val &= ~(0xf<<0);
	reg_val |= (sclk0_ratio_m&0xf)<<0;
	put_wvalue(sclk0_reg_adr, reg_val);

	/////////////////////////////// open clock
	reg_val = get_wvalue(sclk0_reg_adr);
	reg_val |= 0x1U<<31;
	put_wvalue(sclk0_reg_adr, reg_val);

	//NAND_Print("NAND_SetClk for nand index %d \n", nand_index);
	if (nand_index == 0) {
		//NAND_Print("Reg 0x01c20080: 0x%x\n", get_wvalue(0x01c20080));
	} else {
		//NAND_Print("Reg 0x01c20084: 0x%x\n", get_wvalue(0x01c20084));
	}

	return 0;
}

/*****************************************************************************

*****************************************************************************/
__s32 _cfg_ndfc_gpio_v1_b100(__u32 nand_index)
{
	if (nand_index == 0) {
		*(volatile __u32 *)(0x01c20800 + 0x48) = 0x22222222;
		*(volatile __u32 *)(0x01c20800 + 0x4c) = 0x22222222;
//		*(volatile __u32 *)(0x01c20800 + 0x50) = 0x222;
		*(volatile __u32 *)(0x01c20800 + 0x5c) = 0x15555555;
		*(volatile __u32 *)(0x01c20800 + 0x64) = 0x00000440;
		NAND_Print("NAND_PIORequest, nand_index: 0x%x\n", nand_index);
		NAND_Print("Reg 0x01c20848: 0x%x\n", *(volatile __u32 *)(0x01c20848));
		NAND_Print("Reg 0x01c2084c: 0x%x\n", *(volatile __u32 *)(0x01c2084c));
//		NAND_Print("Reg 0x01c20850: 0x%x\n", *(volatile __u32 *)(0x01c20850));
	} else {
		printf("_cfg ndfc gpio v1, wrong nand index %d\n", nand_index);
		return -1;
	}

	return 0;
}

/*****************************************************************************

*****************************************************************************/
__u32 _Getpll6Clk_h8(void)
{
	__u32 reg_val;
	__u32 factor_n;
	__u32 factor_div1,factor_div2;
	__u32 clock;

	reg_val  = get_wvalue(0x01c20000 + 0x28);
	factor_n = ((reg_val >> 8) & 0xff);
	factor_div1 = ((reg_val >> 16) & 0x1) + 1;
	factor_div2 = ((reg_val >> 18) & 0x1) + 1;

	clock = 24000000 * factor_n / factor_div1 / factor_div2;
	//NAND_Print("pll6 clock is %d Hz\n", clock);
	//if(clock != 600000000)
		//printf("pll6 clock rate error, %d!!!!!!!\n", clock);

	return clock;
}

/*****************************************************************************

*****************************************************************************/
__s32 _cfg_ndfc_gpio_v1_v40(__u32 nand_index)
{
	if (nand_index == 0) {
		*(volatile __u32 *)(0x01c20800 + 0x48) = 0x22222222;
		*(volatile __u32 *)(0x01c20800 + 0x4c) = 0x22222222;
		*(volatile __u32 *)(0x01c20800 + 0x50) = 0x72222222;
		*(volatile __u32 *)(0x01c20800 + 0x54) = 0x2;
		*(volatile __u32 *)(0x01c20800 + 0x5c) = 0x55555555;
		*(volatile __u32 *)(0x01c20800 + 0x60) = 0x15555;
		*(volatile __u32 *)(0x01c20800 + 0x64) = 0x00005140;
		*(volatile __u32 *)(0x01c20800 + 0x68) = 0x00001555;
		NAND_Print("NAND_PIORequest, nand_index: 0x%x\n", nand_index);
		NAND_Print("Reg 0x01c20848: 0x%x\n", *(volatile __u32 *)(0x01c20848));
		NAND_Print("Reg 0x01c2084c: 0x%x\n", *(volatile __u32 *)(0x01c2084c));
		NAND_Print("Reg 0x01c20850: 0x%x\n", *(volatile __u32 *)(0x01c20850));
		NAND_Print("Reg 0x01c20854: 0x%x\n", *(volatile __u32 *)(0x01c20854));
	} else {
		printf("_cfg_ndfc_gpio_v1, wrong nand index %d\n", nand_index);
		return -1;
	}

	return 0;
}
