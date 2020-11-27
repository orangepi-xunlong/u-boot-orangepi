/*
**********************************************************************************************************************
*
*                                  the Embedded Secure Bootloader System
*
*
*                              Copyright(C), 2006-2014, Allwinnertech Co., Ltd.
*                                           All Rights Reserved
*
* File    :
*
* By      :
*
* Version : V2.00
*
* Date    :
*
* Descript:
**********************************************************************************************************************
*/
#include <common.h>
#include <asm/io.h>
#include <asm/arch/smc.h>


static uint __tzasc_calc_2_power(uint data)
{
    uint ret = 0;

    do
    {
        data >>= 1;
        ret ++;
    }
    while(data);

    return (ret - 1);
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :  dram_start: e.g. 0x20000000
*
*    return        :  dram_size : Mbytes
*
*    note          :  secure_region_size: Mbytes
*
*
************************************************************************************************************
*/
int sunxi_smc_config(uint dram_size, uint secure_region_size)
{
    uint region_size, permission, region_start;

    writel(0, SMC_MASTER_BYPASS0_REG);
    writel(0xffffffff, SMC_MASTER_SECURITY0_REG);
    region_size = (__tzasc_calc_2_power(dram_size*1024/32) + 0b001110)<<1;

    permission  = 0b1111<<28;
    writel(0, SMC_REGIN_SETUP_LOW_REG(1));
    writel(permission | region_size | 1 , SMC_REGIN_ATTRIBUTE_REG(1));

    region_size = (__tzasc_calc_2_power(secure_region_size*1024/32) + 0b001110)<<1;
    permission  = 0b1100<<28;

    region_start = dram_size - secure_region_size;
    if(region_start <= (4 * 1024))
    {
        writel((region_start * 1024 * 1024) & 0xffff8000, SMC_REGIN_SETUP_LOW_REG(2));
    }
    else
    {
        unsigned long long long_regin_start;

        long_regin_start = region_start;
        long_regin_start = long_regin_start * 1024 * 1024;
        writel(long_regin_start & 0xffff8000, SMC_REGIN_SETUP_LOW_REG(2));
    }
    writel(permission | region_size | 1 , SMC_REGIN_ATTRIBUTE_REG(2));

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
*    note          :
*
*
************************************************************************************************************
*/
int sunxi_drm_config(u32 drm_start, u32 dram_size)
{
    return 0;
}

