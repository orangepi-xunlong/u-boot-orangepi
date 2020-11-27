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
#include "common.h"
#include "asm/io.h"
#include "asm/arch/efuse.h"

#define SID_OP_LOCK  (0xAC)

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
uint sid_read_key(uint key_index)
{
	uint key_val;

	memcpy((void *)&key_val, (void *)(SUNXI_SID_SRAM + key_index), 4);

	return key_val;
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
void sid_program_key(uint key_index, uint key_value)
{
	uint reg_val;

	writel(key_value, SID_PRKEY);

	reg_val = readl(SID_PRCTL);
	reg_val &= ~((0x1ff<<16)|0x3);
	reg_val |= key_index<<16;
	writel(reg_val, SID_PRCTL);

	reg_val &= ~((0xff<<8)|0x3);
	reg_val |= (SID_OP_LOCK<<8) | 0x1;
	writel(reg_val, SID_PRCTL);

	while(readl(SID_PRCTL)&0x1){};

	reg_val &= ~((0x1ff<<16)|(0xff<<8)|0x3);
	writel(reg_val, SID_PRCTL);

	return;
}

/*
*
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
void sid_read_rotpk(void *dst)
{
	uint chipid_index = EFUSE_ROTPK;
	uint id_length = 32;
	uint i = 0;
	for(i = 0 ; i < id_length ;i+=4 )
	{
		*(u32*)dst  = sid_read_key(chipid_index + i );
		dst += 4 ;
	}
	return ;
}

/*
*
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
void sid_set_security_mode(void)
{
	uint reg_val;

	reg_val  = sid_read_key(EFUSE_LCJS);
	reg_val |= (0x01 << 11);
	sid_program_key(EFUSE_LCJS, reg_val);
	reg_val = (sid_read_key(EFUSE_LCJS) >> 11) & 1;

	printf("burn finished,secure mode: %d\n", reg_val);

	return;
}

/*
*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :"0" : no_secure ; "1" : one key_ladder; "2" : two key_ladder
*
*    note          :
*
*
************************************************************************************************************
*/

int sid_prode_key_ladder(void)
{
	uint reg_val;

	reg_val  = sid_read_key(EFUSE_LCJS);
	if (reg_val & (0x1 << 11)) {
		reg_val  = sid_read_key(EFUSE_BROM_CONFIG);
		if (reg_val & (0x1 << 15))
			return 1;
		else
			return 2;
	}
	return 0;

}
