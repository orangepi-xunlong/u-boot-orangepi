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
#include <asm/io.h>
#include <asm/arch/spc.h>
#include <asm/arch/smc.h>
#include <asm/arch/mmc_boot0.h>
#include <private_toc.h>
#include <private_uboot.h>
#include "asm/arch/platform.h"

extern sbrom_toc0_config_t *toc0_config;

void secure_switch_unsecure(u32 run_addr, u32 para_addr);
void secure_switch_other(u32 run_addr, u32 para_addr);
static void boot0_jmp_monitor(unsigned int addr);

unsigned int go_exec (u32 run_addr, u32 para_addr, int out_secure, unsigned int dram_size)
{
	struct spare_boot_head_t *bfh = (struct spare_boot_head_t *)para_addr;
	toc0_private_head_t *toc0 = (toc0_private_head_t *)CONFIG_SBROMSW_BASE;
	int boot_type = toc0->platform[0]&0xf;
	int card_work_mode = toc0_config->card_work_mode;

	if(!boot_type)
	{
		boot_type = 1;
	}
	else if(boot_type == 1)
	{
		boot_type = 0;
	}else if(boot_type == 2){
		//char  storage_data[384];  // 0-159,存储nand信息；160-255,存放卡信息^M
		set_mmc_para(2,(void *)(toc0_config->storage_data+160));
	}

	if(card_work_mode)
	{
		bfh->boot_data.work_mode = card_work_mode;
		printf("card_work_mode=%d\n", card_work_mode);
	}

	sunxi_smc_config(dram_size, toc0_config->secure_dram_mbytes);
	sunxi_drm_config(PLAT_SDRAM_BASE + (dram_size<<20), toc0_config->secure_dram_mbytes<<20);

	printf("storage_type=%d\n", boot_type);
	bfh->boot_data.storage_type = boot_type;

    printf("dram = %d M \n",dram_size);
    bfh->boot_data.dram_scan_size = dram_size;

    bfh->boot_data.secureos_exist = 1;

	memcpy(bfh->boot_data.dram_para, toc0_config->dram_para, 32 * 4);

	boot0_jmp_monitor(run_addr);

	return 0;
}

static void boot0_jmp_monitor(unsigned int addr)
{
	// jmp to AA64
	//set the cpu boot entry addr:
	writel(addr,RVBARADDR0_L);
	writel(0,RVBARADDR0_H);

	//*(volatile unsigned int*)0x40080000 =0x14000000; //
	//note: warm reset to 0x40080000 when run on fpga,
	//*(volatile unsigned int*)0x40080000 =0xd61f0060; //hard code: br x3
	//asm volatile("ldr r3, =(0x7e000000)");   //set r3

	//asm volatile("ldr r0, =(0x7f000200)");
	//asm volatile("ldr r1, =(0x12345678)");

	//set cpu to AA64 execution state when the cpu boots into after a warm reset
	asm volatile("MRC p15,0,r2,c12,c0,2");
	asm volatile("ORR r2,r2,#(0x3<<0)");
	asm volatile("DSB");
	asm volatile("MCR p15,0,r2,c12,c0,2");
	asm volatile("ISB");
__LOOP:
	asm volatile("WFI");
	goto __LOOP;

}

int sunxi_deassert_arisc(void)
{
	printf("set arisc reset to de-assert state\n");
	{
		volatile unsigned long value;
		value = readl(SUNXI_RCPUCFG_BASE + 0x0);
		value &= ~1;
		writel(value, SUNXI_RCPUCFG_BASE + 0x0);
		value = readl(SUNXI_RCPUCFG_BASE + 0x0);
		value |= 1;
		writel(value, SUNXI_RCPUCFG_BASE + 0x0);
	}

	return 0;
}

