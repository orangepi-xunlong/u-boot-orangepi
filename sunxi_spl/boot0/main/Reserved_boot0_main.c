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
#include <private_boot0.h>
#include <private_uboot.h>
#include <asm/arch/clock.h>
#include <asm/arch/timer.h>
#include <asm/arch/uart.h>
#include <asm/arch/dram.h>
#include <asm/arch/rtc_region.h>
#include <private_toc.h>
#include <boot0_helper.h>

extern const boot0_file_head_t  BT0_head;

static void print_version(void);
static int boot0_clear_env(void);
static void update_uboot_info(__u32 dram_size);
static int boot0_check_uart_input(void);
#ifdef	SUNXI_OTA_TEST
static void print_ota_test(void);
#endif


extern char boot0_hash_value[64];

int mmc_config_addr = (int)(BT0_head.prvt_head.storage_data);

//phoenixcard will set this bit
#define BOOT0_BURN_SECURE_BIT (1<<31)

/*******************************************************
we should implement below interfaces if platform support
handler standby flag in boot0
*******************************************************/
void __attribute__((weak)) handler_super_standby(void)
{
	return;
}
void __attribute__((weak)) sid_set_security_mode(void)
{
	return;
}
void __attribute__((weak)) reboot(void)
{
	return;
}
void __attribute__((weak)) set_gpio_gate(void)
{
	return;
}


/*******************************************************************************
main:   body for c runtime 
*******************************************************************************/
void main( void )
{
	__u32 status;
	__s32 dram_size;
	__u32 fel_flag;
	__u32 boot_cpu=0;
	int use_monitor = 0;

	timer_init();
	sunxi_serial_init( BT0_head.prvt_head.uart_port, (void *)BT0_head.prvt_head.uart_ctrl, 6 );
	set_debugmode_flag();
	printf("HELLO! BOOT0 is starting!\n");
	printf("boot0 commit : %s \n",boot0_hash_value);
	print_version();
#ifdef	SUNXI_OTA_TEST
	print_ota_test();
#endif

	set_pll();
	set_gpio_gate();
	boot0_check_uart_input();
	if (BT0_head.prvt_head.enable_jtag&BOOT0_BURN_SECURE_BIT)
	{
		printf("ready to burn secure bit\n");
		sid_set_security_mode();
		reboot();
	}
	if( BT0_head.prvt_head.enable_jtag )
	{
		boot_set_gpio((normal_gpio_cfg *)BT0_head.prvt_head.jtag_gpio, 6, 1);
	}

	fel_flag = rtc_region_probe_fel_flag();
	if(fel_flag == SUNXI_RUN_EFEX_FLAG)
	{
		rtc_region_clear_fel_flag();
		printf("eraly jump fel\n");
		goto __boot0_entry_err0;
	}

#ifdef FPGA_PLATFORM
	dram_size = mctl_init((void *)BT0_head.prvt_head.dram_para);
#else
	dram_size = init_DRAM(0, (void *)BT0_head.prvt_head.dram_para);
#endif
	if(dram_size)
	{
		printf("dram size =%d\n", dram_size);
	}
	else
	{
		printf("initializing SDRAM Fail.\n");
		goto  __boot0_entry_err0;
	}
	//on some platform, boot0 should handler standby flag.
	handler_super_standby();

	mmu_setup(dram_size);
	status = load_boot1();
	if(status == 0 )
	{
		use_monitor = 0;
		status = load_fip(&use_monitor);
	}

	printf("Ready to disable icache.\n");

    // disable instruction cache
	mmu_turn_off( ); 

	if( status == 0 )
	{
		//update bootpackage size for uboot
		update_uboot_info(dram_size);
		//update flash para
		update_flash_para();
		//update dram para before jmp to boot1
		set_dram_para((void *)&BT0_head.prvt_head.dram_para, dram_size, boot_cpu);
		printf("Jump to secend Boot.\n");
                if(use_monitor)
		{
			boot0_jmp_monitor();
		}
		else
		{
			boot0_jmp_boot1(CONFIG_SYS_TEXT_BASE);
		}
	}

__boot0_entry_err0:
	boot0_clear_env();

	boot0_jmp_other(FEL_BASE);
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
static void print_version()
{
	//brom modify: nand-4bytes, sdmmc-2bytes
	printf("boot0 version : %s\n", BT0_head.boot_head.platform + 4);

	return;
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
static int boot0_clear_env(void)
{

	reset_pll();
	mmu_turn_off();

	__msdelay(10);
    
	return 0;
}

//
static void update_uboot_info(__u32 dram_size)
{
	struct spare_boot_head_t  *bfh = (struct spare_boot_head_t *) CONFIG_SYS_TEXT_BASE;
	struct sbrom_toc1_head_info *toc1_head = (struct sbrom_toc1_head_info *)CONFIG_BOOTPKG_STORE_IN_DRAM_BASE;
	bfh->boot_data.boot_package_size = toc1_head->valid_len;
	bfh->boot_data.dram_scan_size = dram_size;
	//printf("boot package size: 0x%x\n",bfh->boot_data.boot_package_size);
}

static int boot0_check_uart_input(void)
{
	int c = 0;
	int i = 0;
	for(i = 0;i < 3;i++)
	{
		__msdelay(10);
		if(sunxi_serial_tstc())
		{
			c = sunxi_serial_getc();
			break;
		}
	}

	if(c == '2')
	{
		printf("enter 0x%x,ready jump to fes\n", c-0x30);  // ASCII to decimal digit
		boot0_clear_env();
		boot0_jmp_other(FEL_BASE);
	}
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
#ifdef	SUNXI_OTA_TEST
static void print_ota_test(void)
{
	printf("*********************************************\n");
	printf("*********************************************\n");
	printf("*********************************************\n");
	printf("*********************************************\n");
	printf("********[OTA TEST]:update boot0 sucess*******\n");
	printf("*********************************************\n");
	printf("*********************************************\n");
	printf("*********************************************\n");
	printf("*********************************************\n");
	return;
}
#endif
