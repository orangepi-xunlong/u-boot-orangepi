/*
 * drivers/usb/sunxi_usb/udc/sunxi_udc_board.c
 * (C) Copyright 2010-2015
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * javen, 2010-12-20, create this file
 *
 * usb board config.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 */

///#include <linux/init.h>
#include <linux/list.h>

#include <common.h>
#include <usb/lin_gadget_compat.h>
///#include <asm/arch-sunxi/cpu.h>
#include <asm/arch/usb.h>
#include "sunxi_udc_config.h"
#include "sunxi_udc_board.h"

__s32 sunxi_udc_bsp_init(__u32 usbc_no, sunxi_udc_io_t *sunxi_udc_io)
{
	sunxi_udc_io->usbc.usbc_info[usbc_no].num = usbc_no;
	sunxi_udc_io->usbc.usbc_info[usbc_no].base = (u32 __force)sunxi_udc_io->usb_pbase;
	sunxi_udc_io->usbc.sram_base = (u32 __force)sunxi_udc_io->sram_pbase;

	//USBC_init(&sunxi_udc_io->usbc);
	sunxi_udc_io->usb_bsp_hdle = USBC_open_otg(usbc_no);
	if (sunxi_udc_io->usb_bsp_hdle == 0) {
		DMSG_PANIC("ERR: sunxi_udc_init: USBC_open_otg failed\n");
		return -1;
	}

	USBC_EnhanceSignal(sunxi_udc_io->usb_bsp_hdle);

	USBC_EnableDpDmPullUp(sunxi_udc_io->usb_bsp_hdle);
	USBC_EnableIdPullUp(sunxi_udc_io->usb_bsp_hdle);
	USBC_ForceId(sunxi_udc_io->usb_bsp_hdle, USBC_ID_TYPE_DEVICE);
	USBC_ForceVbusValid(sunxi_udc_io->usb_bsp_hdle, USBC_VBUS_TYPE_HIGH);

	USBC_SelectBus(sunxi_udc_io->usb_bsp_hdle, USBC_IO_TYPE_PIO, 0, 0);

{
	__u32 reg_val = 0;
	reg_val = USBC_Readl(sunxi_udc_io->usb_pbase + USBC_REG_o_PHYCTL);
	reg_val &= ~(0x01 << 1);
	USBC_Writel(reg_val, (sunxi_udc_io->usb_pbase + USBC_REG_o_PHYCTL));
}

	return 0;
}

static __s32 sunxi_udc_bsp_exit(__u32 usbc_no, sunxi_udc_io_t *sunxi_udc_io)
{
	USBC_DisableDpDmPullUp(sunxi_udc_io->usb_bsp_hdle);
	USBC_DisableIdPullUp(sunxi_udc_io->usb_bsp_hdle);
	USBC_ForceId(sunxi_udc_io->usb_bsp_hdle, USBC_ID_TYPE_DISABLE);
	USBC_ForceVbusValid(sunxi_udc_io->usb_bsp_hdle, USBC_VBUS_TYPE_DISABLE);

	USBC_close_otg(sunxi_udc_io->usb_bsp_hdle);
	sunxi_udc_io->usb_bsp_hdle = 0;

	//USBC_exit(&sunxi_udc_io->usbc);
	return 0;
}

__s32 sunxi_udc_io_init(__u32 usbc_no, sunxi_udc_io_t *sunxi_udc_io)
{
	sunxi_udc_io->usb_pbase  = (void __iomem *)SUNXI_USBOTG_BASE;
	sunxi_udc_io->sram_pbase = (void __iomem *)SUNXI_SRAMC_BASE;

	DMSG_INFO_UDC("usb_pbase  = 0x%x\n", (u32 __force)sunxi_udc_io->usb_pbase);
	DMSG_INFO_UDC("sram_pbase = 0x%x\n", (u32 __force)sunxi_udc_io->sram_pbase);

	usb_open_clock();

	/* initialize usb bsp */
	sunxi_udc_bsp_init(usbc_no, sunxi_udc_io);

	/* config usb fifo */
	USBC_ConfigFIFO_Base(sunxi_udc_io->usb_bsp_hdle, (u32 __force)sunxi_udc_io->sram_pbase, USBC_FIFO_MODE_8K);

	return 0;
}

__s32 sunxi_udc_io_exit(__u32 usbc_no, sunxi_udc_io_t *sunxi_udc_io)
{
	sunxi_udc_bsp_exit(usbc_no, sunxi_udc_io);

	usb_close_clock();

	sunxi_udc_io->usb_pbase  = NULL;
	sunxi_udc_io->sram_pbase = NULL;

	return 0;
}

