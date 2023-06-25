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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 */
#include "usb_base.h"
#include <scsi.h>
//#include <asm/arch/dma.h>
//#include <sys_partition.h>
#include <sys_config.h>
//#include <sprite.h>
//#include <boot_type.h>
//#include <power/sunxi/pmu.h>
#include <asm/io.h>
#include <fdt_support.h>
#include <sunxi_board.h>
#include <sprite_download.h>
#include <sprite_verify.h>
#include <asm/arch/timer.h>
#include <sunxi_flash.h>
//#include <sys_config_old.h>

volatile int sunxi_usb_detect_flag;

DECLARE_GLOBAL_DATA_PTR;

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
static int sunxi_usb_detect_init(void)
{
	sunxi_usb_dbg("sunxi_usb_detect_init\n");
	sunxi_usb_detect_flag = 0;
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
static int sunxi_usb_detect_exit(void)
{
	sunxi_usb_dbg("sunxi_usb_detect_exit\n");
	sunxi_usb_detect_flag = 0;

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
static void sunxi_usb_detect_reset(void)
{
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
static void  sunxi_usb_detect_usb_rx_dma_isr(void *p_arg)
{
	sunxi_usb_dbg("dma int for usb rx occur\n");
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
static void  sunxi_usb_detect_usb_tx_dma_isr(void *p_arg)
{
	sunxi_usb_dbg("dma int for usb tx occur\n");
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
static int sunxi_usb_detect_standard_req_op(uint cmd, struct usb_device_request *req, uchar *buffer)
{
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
static int sunxi_usb_detect_nonstandard_req_op(uint cmd, struct usb_device_request *req, uchar *buffer, uint data_status)
{
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
static int sunxi_usb_detect_state_loop(void  *buffer)
{
	return 0;
}


sunxi_usb_module_init(SUNXI_USB_DEVICE_DETECT,                                 \
					  sunxi_usb_detect_init,               \
					  sunxi_usb_detect_exit,               \
					  sunxi_usb_detect_reset,              \
					  sunxi_usb_detect_standard_req_op,    \
					  sunxi_usb_detect_nonstandard_req_op, \
					  sunxi_usb_detect_state_loop,         \
					  sunxi_usb_detect_usb_rx_dma_isr,     \
					  sunxi_usb_detect_usb_tx_dma_isr      \
					  );
