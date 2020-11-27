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
#include <asm/armv7.h>
#include <asm/arch/cpu.h>
#include <asm/arch/timer.h>

#define  RPCM_R_PIO_HOLD_CTRL_REG           (SUNXI_RPRCM_BASE + 0x1f0)
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
uint rtc_region_probe_fel_flag(void)
{
	uint fel_flag, reg_value;
	int  i;

	writel(readl(RPCM_R_PIO_HOLD_CTRL_REG) | (1<<16), RPCM_R_PIO_HOLD_CTRL_REG);
	CP15ISB;
	CP15DMB;
    fel_flag = readl(RPCM_R_PIO_HOLD_CTRL_REG) & 0xff;

	for(i=1;i<=3;i++)
	{
		reg_value = readl(RPCM_R_PIO_HOLD_CTRL_REG) & 0xffff;
		reg_value |= (i<<16);
		writel(reg_value, RPCM_R_PIO_HOLD_CTRL_REG);
		CP15ISB;
		CP15DMB;
		reg_value = readl(RPCM_R_PIO_HOLD_CTRL_REG);
		printf("rtc[%d] value = 0x%x\n", i, reg_value);
	}

	return fel_flag;
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
*    note          :
*
*
************************************************************************************************************
*/
void rtc_region_clear_fel_flag(void)
{
	volatile uint reg_val;

    do
    {
	    writel(1<<16, RPCM_R_PIO_HOLD_CTRL_REG);
	    writel((1<<16) | (1U<<31), RPCM_R_PIO_HOLD_CTRL_REG);
	    __usdelay(10);
	    CP15ISB;
	    CP15DMB;
	    writel(1<<16, RPCM_R_PIO_HOLD_CTRL_REG);
	    reg_val = readl(RPCM_R_PIO_HOLD_CTRL_REG);
    }
    while((reg_val & 0xff) != 0);
}




