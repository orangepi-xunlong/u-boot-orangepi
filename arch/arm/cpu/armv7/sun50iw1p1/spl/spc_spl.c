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
	writel(0xff, SPC_SET_REG(0));
	writel(0xff, SPC_SET_REG(1));
	writel(0xff, SPC_SET_REG(2));
	writel(0xff, SPC_SET_REG(3));
	writel(0x1f, SPC_SET_REG(4));
}

