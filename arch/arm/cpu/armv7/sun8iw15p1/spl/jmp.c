/*
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */
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

void boot0_jmp_other(unsigned int addr);
void mmu_turn_off(void);
void boot0_jmp_monitor(void)
{
	unsigned int optee_entry = OPTEE_BASE;
	unsigned int uboot_entry = CONFIG_SYS_TEXT_BASE;

	mmu_turn_off();

	asm volatile ("mov lr, %0" : : "r" (uboot_entry) : "memory");
	asm volatile ("bx      %0" : : "r" (optee_entry) : "memory");
}

void boot0_jmp_other(unsigned int addr)
{
    asm volatile("mov r2, #0");
    asm volatile("mcr p15, 0, r2, c7, c5, 6");
    asm volatile("bx r0");
}

void boot0_jmp_boot1(unsigned int addr)
{
    boot0_jmp_other(addr);
}
