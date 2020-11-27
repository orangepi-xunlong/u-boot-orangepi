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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/timer.h>
#include <asm/arch/ccmu.h>
#include <asm/arch/clock.h>
#include <asm/arch/platform.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/key.h>
#include <asm/arch/dma.h>

#include <boot_type.h>
#include <sys_partition.h>
#include <sys_config.h>
#include <power/sunxi/pmu.h>
#include <power/sunxi/power.h>
#include <smc.h>
#include <sunxi_board.h>
#include <fdt_support.h>
#include <sys_config_old.h>
#include <arisc.h>
#include <private_toc.h>

/* The sunxi internal brom will try to loader external bootloader
 * from mmc0, nannd flash, mmc2.
 * We check where we boot from by checking the config
 * of the gpio pin.
 */
DECLARE_GLOBAL_DATA_PTR;

/* do some early init */
void s_init(void)
{
    watchdog_disable();
}

void reset_cpu(ulong addr)
{
    watchdog_enable();
    while(1);
}

void enable_caches(void)
{
    icache_enable();
    dcache_enable();
}

void disable_caches(void)
{
    icache_disable();
    dcache_disable();
}

int display_inner(void)
{
    printf("version: %s\n", uboot_spare_head.boot_head.version);

    return 0;
}

static int soc_script_init(void)
{
    char *addr = NULL;

    addr   = (char *)CONFIG_SOCCFG_STORE_IN_DRAM_BASE;
    script_head_t *orign_head = (script_head_t *)addr;

    if (orign_head->length == 0)
    {
        soc_script_parser_init(NULL);
        return 0;
    }

    if (!(gd->flags & GD_FLG_RELOC))
    {
        soc_script_parser_init((char *)addr);
    }
    else
    {
        soc_script_parser_init((char *)get_script_reloc_buf(SOC_SCRIPT));
    }

    return 0;
}

#ifdef USE_BOARD_CONFIG
static int bd_script_init(void)
{
    char *addr = NULL;

    addr   = (char *)CONFIG_BDCFG_STORE_IN_DRAM_BASE;
    script_head_t *orign_head = (script_head_t *)addr;

    if (orign_head->length == 0)
    {
        bd_script_parser_init(NULL);
        return 0;
    }

    if (!(gd->flags & GD_FLG_RELOC))
    {
        bd_script_parser_init((char *)addr);
    }
    else
    {
        bd_script_parser_init((char *)get_script_reloc_buf(BD_SCRIPT));
    }

    return 0;
}
#endif

int script_init(void)
{
    script_parser_func_init();
    soc_script_init();
#ifdef USE_BOARD_CONFIG
    bd_script_init();
#endif
    return 0;
}


void sunxi_set_fel_flag(void)
{
    *((volatile unsigned int *)(SUNXI_RUN_EFEX_ADDR)) = SUNXI_RUN_EFEX_FLAG;
}

void sunxi_clear_fel_flag(void)
{
    *((volatile unsigned int *)(SUNXI_RUN_EFEX_ADDR)) = 0;
}

void sunxi_set_rtc_flag(u8 flag)
{
    *((volatile unsigned int *)(SUNXI_RUN_EFEX_ADDR)) = flag;
}



void sunxi_board_close_source(void)
{
    //timer_exit should be call after sunxi_flash,because delay function
    sunxi_flash_exit(1);
    sunxi_sprite_exit(1);
#ifdef CONFIG_SUNXI_DMA
    sunxi_dma_exit();
#endif
    timer_exit();
    sunxi_key_exit();
    disable_interrupts();
    interrupt_exit();
    return ;
}

int sunxi_board_restart(int next_mode)
{
    if(!next_mode)
    {
        next_mode = PMU_PRE_SYS_MODE;
    }
    printf("set next mode %d\n", next_mode);
    axp_set_next_poweron_status(next_mode);

#ifdef CONFIG_SUNXI_DISPLAY
    board_display_set_exit_mode(0);
    drv_disp_exit();
#endif
    sunxi_board_close_source();
    reset_cpu(0);

    return 0;
}

int sunxi_board_shutdown(void)
{
    printf("set next system normal\n");
    axp_set_next_poweron_status(0x0);

#ifdef CONFIG_SUNXI_DISPLAY
    board_display_set_exit_mode(0);
    drv_disp_exit();
#endif
    sunxi_flash_exit(1);
    sunxi_sprite_exit(1);
    disable_interrupts();
    interrupt_exit();

    tick_printf("power off\n");
    axp_set_hardware_poweroff_vol();
    axp_set_power_off();

    return 0;

}

int sunxi_board_prepare_kernel(void)
{
	axp_set_next_poweron_status(PMU_PRE_SYS_MODE);
#ifdef CONFIG_SUNXI_DISPLAY
#ifndef CONFIG_SUNXI_MULITCORE_BOOT
	board_display_wait_lcd_open();
	board_display_set_exit_mode(1);
#endif
#endif
	sunxi_board_close_source();

	return 0;
}

void sunxi_flush_allcaches(void)
{
    icache_disable();
    flush_dcache_all();
    dcache_disable();
}


int sunxi_board_run_fel(void)
{
    sunxi_set_fel_flag();
    printf("set next system status\n");
    axp_set_next_poweron_status(PMU_PRE_SYS_MODE);
#ifdef CONFIG_SUNXI_DISPLAY
    board_display_set_exit_mode(0);
    drv_disp_exit();
#endif
    printf("sunxi_board_close_source\n");
    sunxi_board_close_source();
    sunxi_flush_allcaches();
    printf("reset cpu\n");
    reset_cpu(0);

    return 0;
}

int sunxi_board_run_fel_eraly(void)
{
    sunxi_set_fel_flag();
    printf("set next system status\n");
    axp_set_next_poweron_status(PMU_PRE_SYS_MODE);
    timer_exit();
    sunxi_key_exit();
    printf("reset cpu\n");
    reset_cpu(0);

    return 0;
}

__weak void cpu_spin_lock(unsigned int *lock)
{
    return ;
}
__weak unsigned int cpu_spin_trylock(unsigned int *lock)
{
    return 0;
}
__weak void cpu_spin_unlock(unsigned int *lock)
{
    return ;
}


