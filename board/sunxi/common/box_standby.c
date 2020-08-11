/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Liaoyongming <liaoyongming@allwinnertech.com>
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

#include <common.h>
#include <asm/io.h>
#include <asm/arch/platform.h>
#include <linux/err.h>
#include <fdtdec.h>
#include <sys_config.h>
#include <fdt_support.h>
#include <arisc.h>
#include <sunxi_board.h>
#include <spare_head.h>
#include <smc.h>

#define SUNXI_MODE_REG (SUNXI_RTC_BASE + 0x100 + 2*0x04)

#define GPIO_REG_READ(reg)						(smc_readl((ulong)(reg)))
#define GPIO_REG_WRITE(reg, value)				(smc_writel((value), (ulong)(reg)))

#define _R_PIO_REG_INT_GATE(n, i)               ((volatile unsigned int *)( SUNXI_RPIO_BASE + 0x200 + ((n)-12)*0x20 + 0x10))
#define _R_PIO_REG_INT_GATE_VALUE(n, i)          smc_readl( SUNXI_RPIO_BASE + 0x200 + ((n)-12)*0x20 + 0x10)

#define _R_PIO_REG_INT_MODE(n, i)               ((volatile unsigned int *)( SUNXI_RPIO_BASE + 0x200 + ((n)-12)*0x20 + ((i>>3) * 0x04) + 0x00))
#define _R_PIO_REG_INT_MODE_VALUE(n, i)          smc_readl( SUNXI_RPIO_BASE + 0x200 + ((n)-12)*0x20 + ((i>>3)*0x04) + 0x00)


#define _R_PIO_REG_INT_PENDING(n, i)               ((volatile unsigned int *)( SUNXI_RPIO_BASE + 0x200 + ((n)-12)*0x20 +  0x14))
#define _R_PIO_REG_INT_PENDING_VALUE(n, i)          smc_readl( SUNXI_RPIO_BASE + 0x200 + ((n)-12)*0x20 + 0x14)

DECLARE_GLOBAL_DATA_PTR;

// system start mode
#define MODE_NULL				(0x0)
#define MODE_SHUTDOWN_OS		(0x1)
#define MODE_WAIT_WAKE_UP		(0x2)
#define MODE_RUN_OS				(0xf)

 /*
************************************************************************************************************
*
*                                             function
*
*    function	:
*
*    parameter	:
*
*    return		:
*
*    note		:
*
************************************************************************************************************
*/
uint read_start_mode(void)
{
	uint reg_value;

	reg_value = readl(SUNXI_MODE_REG);

	return reg_value;
}

 /*
************************************************************************************************************
*
*                                             function
*
*    function	:
*
*    parameter	:
*
*    return		:
*
*    note		:
*
************************************************************************************************************
*/
void write_start_mode(uint mode)
{
	writel(SUNXI_MODE_REG, mode);
}


int set_int_gate_onoff(int port, int port_num, int onoff)
{
	u32 tmp = 0;
	volatile u32 *tmp_group_pull_addr = NULL;

	if (port < 12)
	{
		printf("not support enable INT with %d, %d\n", port, port_num);
		return -1;
	}

	tmp_group_pull_addr = _R_PIO_REG_INT_GATE(port, port_num);

	tmp = GPIO_REG_READ(tmp_group_pull_addr);

	if (onoff)
		tmp |= (1 << port_num);
	else
		tmp &= (~(1 << port_num));

	GPIO_REG_WRITE(tmp_group_pull_addr, tmp);

	return 0;
}

int set_int_mode(int port, int port_num, int mode)
{
	u32 tmp = 0;
	volatile u32 *tmp_group_pull_addr = NULL;

	if (port < 12)
	{
		printf("not support set INT mode with %d, %d\n", port, port_num);
		return -1;
	}

	tmp_group_pull_addr = _R_PIO_REG_INT_MODE(port, port_num);

	tmp = GPIO_REG_READ(tmp_group_pull_addr);
	tmp &= (~(0xF << ((port_num%8) * 0x04)));
	tmp |= (mode << ((port_num%8) * 0x04));
	GPIO_REG_WRITE(tmp_group_pull_addr, tmp);

	return 0;
}

int clean_int_pending(int port, int port_num)
{
	u32 tmp = 0;
	volatile u32 *tmp_group_pull_addr = NULL;

	if (port < 12)
	{
		printf("not support clean int pending with %d, %d\n", port, port_num);
		return -1;
	}

	tmp_group_pull_addr = _R_PIO_REG_INT_PENDING(port, port_num);

	tmp = GPIO_REG_READ(tmp_group_pull_addr);
	tmp |= 0xFFF;
	GPIO_REG_WRITE(tmp_group_pull_addr, tmp);

	return 0;
}


 /*
************************************************************************************************************
*
*                                             function
*
*    function	:
*
*    parameter	:
*
*    return		:
*
*    note		:
*
************************************************************************************************************
*/
int standby_led_control(void)
{
	int ret;
	char  tmp_gpio_name[16]={0};
	user_gpio_set_t	tmp_gpio_config;
	int count = 0;
	int nodeoffset = -1;

	nodeoffset = fdt_path_offset(working_fdt, "/soc/box_standby_led");
	if(nodeoffset < 0)
	{
	    printf("box_standby_led no exist\n");
		return 0;
	}

	printf("set box_stanby led\n");
    while(1)
    {
        sprintf(tmp_gpio_name, "gpio%d", count);
        ret = fdt_get_one_gpio("/soc/box_standby_led", tmp_gpio_name, &tmp_gpio_config);
	    if(ret < 0)
	    {
		    break;
	    }

	    ret = gpio_request_early(&tmp_gpio_config, 1, 1);
	    if(ret < 0)
	    {
		    printf("[box_standby_led] %s gpio_request_early failed\n", tmp_gpio_name);
		    return -1;
	    }
	    count++;
	}
	return  0;
}


int init_standby_wakeup_src(void)
{
	int ret, count = 0;
	char  tmp_gpio_name[32]={0};
	user_gpio_set_t	tmp_gpio_config;

	while(1)
	{
		sprintf(tmp_gpio_name, "wakeup_src%d", count);
		ret = fdt_get_one_gpio("/soc/box_start_os0", tmp_gpio_name, &tmp_gpio_config);
		if(ret < 0)
		{
			break;
		}

		if (tmp_gpio_config.port < 12)
		{
			printf("not support this wakeup_src:%s\n", tmp_gpio_name);
			return -1;
		}

		if (tmp_gpio_config.mul_sel != 6)
		{
			printf("mul_set must to be 6\n");
			return -1;
		}

		ret = gpio_request_early(&tmp_gpio_config, 1, 1);
		if(ret < 0)
		{
			printf("[boot_init_gpio] %s gpio_request_early failed\n", tmp_gpio_name);
			return -1;
		}

		clean_int_pending(tmp_gpio_config.port, tmp_gpio_config.port_num);
		set_int_mode(tmp_gpio_config.port, tmp_gpio_config.port_num, 1);
		set_int_gate_onoff(tmp_gpio_config.port, tmp_gpio_config.port_num, 1);
		clean_int_pending(tmp_gpio_config.port, tmp_gpio_config.port_num);
		printf("init %s:P%c%d\n",tmp_gpio_name, 'A' + tmp_gpio_config.port - 1, tmp_gpio_config.port_num);
		count++;
	}
	return 0;
}
 /*
************************************************************************************************************
*
*                                             function
*
*    function	:
*
*    parameter	:
*
*    return		:
*
*    note		:
*
************************************************************************************************************
*/
static void do_no_thing_loop(void)
{
	__asm__ volatile  ("dsb");
	__asm__ volatile  ("wfi");
}

 /*
************************************************************************************************************
*
*                                             function
*
*    function	:
*
*    parameter	:
*
*    return		:
*
*    note		:
*
************************************************************************************************************
*/
int sunxi_arisc_standby(void)
{
	sunxi_arisc_fake_poweroff();
	while (1) {
		do_no_thing_loop();
	}
}

 /*
************************************************************************************************************
*
*                                             function
*
*    function	:
*
*    parameter	:
*
*    return		:
*
*    note		:
*
************************************************************************************************************
*/
int do_box_standby(void)
{
	uint mode, start_type, irkey_used, pmukey_used;
	int nodeoffset;

	if(uboot_spare_head.boot_data.work_mode != WORK_MODE_BOOT)
	{
		return 0;
	}

	nodeoffset = fdt_path_offset(working_fdt, "/soc/box_start_os0");
	if (!IS_ERR_VALUE(nodeoffset))
	{
		if(!fdtdec_get_is_enabled(working_fdt, nodeoffset))
		{
			printf("[box standby] not used\n");
			return 0;
		}

		mode = read_start_mode();
		printf("[box standby] read rtc = 0x%x\n", mode);

		if (IS_ERR_VALUE(fdt_getprop_u32(working_fdt, nodeoffset, "start_type", &start_type))) {
			printf("[box standby] get start_type failed\n");
			return 0;
		}

		if (IS_ERR_VALUE(fdt_getprop_u32(working_fdt, nodeoffset, "irkey_used", &irkey_used))) {
			printf("[box standby] get irkey_used failed\n");
			return 0;
		}

		if (IS_ERR_VALUE(fdt_getprop_u32(working_fdt, nodeoffset, "pmukey_used", &pmukey_used))) {
			printf("[box standby] get pmukey_used failed\n");
			return 0;
		}

		printf("[box standby] start_type = 0x%x\n", start_type);
		if((start_type == MODE_NULL) && ((mode & 0xf) != MODE_RUN_OS))		//开机不启动,只要rtc寄存器值不为0x0f，即代表需要进入假关机状态
		{
			if((irkey_used == 0) && (pmukey_used == 0))
			{
				printf("no wakeup source,neet to shutdown\n");
				sunxi_update_subsequent_processing(SUNXI_UPDATE_NEXT_ACTION_SHUTDOWN);	//not set wake up source, neet to shutdown
				return 0;
			}

			init_standby_wakeup_src();
			standby_led_control();

			printf("waiting ir or power key wakeup\n");
			sunxi_arisc_standby();			//goto cpus waiting wake up
		}
		else if((start_type == MODE_SHUTDOWN_OS) && (mode == MODE_WAIT_WAKE_UP)) //开机启动,关机需要进入假关机，必须满足rtc寄存器值为0x02
		{
			if((irkey_used == 0) && (pmukey_used == 0))
			{
				printf("no wakeup source,neet to shutdown\n");
				sunxi_update_subsequent_processing(SUNXI_UPDATE_NEXT_ACTION_SHUTDOWN); //not set wake up source, neet to shutdown
				return 0;
			}

			init_standby_wakeup_src();
			standby_led_control();

			printf("waiting ir or power key wakeup\n");
			sunxi_arisc_standby();			//goto cpus waiting wake up
		}
		else
		{
			printf("[box standby] to kernel\n");	//run to kernel
		}
	}
	return 0;
}
