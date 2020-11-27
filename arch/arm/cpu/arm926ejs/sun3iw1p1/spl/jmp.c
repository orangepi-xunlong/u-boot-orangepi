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
#include <asm/arch/platform.h>



extern void RMR_TO64(void);

void boot0_jmp_boot1(unsigned int addr)
{
    asm volatile(" mov pc, r0");
}

void boot0_jmp_monitor(void)
{
    __u32 opdata = 0;
    //this just to make compiler happy
    opdata = opdata;

__LOOP:
    goto __LOOP;

}


void boot0_jmp_other(unsigned int addr)
{
    asm volatile(" mov pc, r0");
}

