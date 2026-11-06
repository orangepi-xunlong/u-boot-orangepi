/*
 * drivers/usb/gadget/cvi_udc.h
 * Designware CVI on-chip full/high speed USB device controllers
 * Copyright (C) 2005 for Samsung Electronics
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CVI_USB_GADGET
#define __CVI_USB_GADGET

#include <linux/usb/otg.h>
#include "../core.h"

#define _DEBUG	0

#define cvidbg_cond(cond, fmt, args...)			\
	do {						\
		if (cond)				\
			printf(fmt, ##args);	\
	} while (0)

#define cvidbg(fmt, args...)			\
	cvidbg_cond(_DEBUG, fmt, ##args)

#define DEBUG_SETUP 0
#define DEBUG_EP0 0
#define DEBUG_ISR 0
#define DEBUG_OUT_EP 0
#define DEBUG_IN_EP 0

#define PHY0_SLEEP              BIT(5)

#ifndef CONFIG_SYS_CACHELINE_SIZE
#define CONFIG_SYS_CACHELINE_SIZE       64
#endif

#define ROUND(a, b)		(((a) + (b) - 1) & ~((b) - 1))

#define min(x, y) ({				\
	typeof(x) _min1 = (x);			\
	typeof(y) _min2 = (y);			\
	(void) (&_min1 == &_min2);		\
	_min1 < _min2 ? _min1 : _min2; })

#define max(x, y) ({				\
	typeof(x) _max1 = (x);			\
	typeof(y) _max2 = (y);			\
	(void) (&_max1 == &_max2);		\
	_max1 > _max2 ? _max1 : _max2; })

#define OTG_DMA_MODE		1

#define EP0_CON		0
#define EP_MASK		0xF

// struct cvi_plat_otg_data {
// 	void		*handler;
// 	unsigned int	size;
// 	int		phy_of_node;
// 	int		(*phy_control)(int on);
// 	uintptr_t	regs_phy;
// 	uintptr_t	regs_otg;
// 	unsigned int    usb_phy_ctrl;
// 	unsigned int    usb_flags;
// 	unsigned int	usb_gusbcfg;
// 	unsigned int	rx_fifo_sz;
// 	unsigned int	np_tx_fifo_sz;
// 	unsigned int	tx_fifo_sz;
// 	void            *ctrl_req;
// };


struct dwc3_generic_plat {
	void __iomem *regs;
	u32 maximum_speed;
	enum usb_dr_mode dr_mode;
};

struct dwc3_generic_priv {
	void *handler;
	unsigned int	size;
	struct dwc3 dwc3;
	// struct phy_bulk phys;
	struct dwc3_generic_plat plat;
};

struct cvi_drv_obj {
	struct usb_gadget		*gadget;
	struct dwc3_generic_priv	priv;
};

struct usb_udc {
	struct usb_gadget_driver	*driver;
	struct usb_gadget		*gadget;
	// struct device			dev;
	struct list_head		list;
};

// int cvi_udc_probe(struct cvi_plat_otg_data *pdata);
int dwc3_generic_probe(struct dwc3_generic_priv *priv);
int dwc3_generic_remove(struct dwc3_generic_priv *priv);
// void cvi_udc_ep_activate(struct cvi_ep *ep);
// int cvi_udc_irq(int irq, void *_dev);
// int cvi_queue(struct usb_ep *_ep, struct usb_request *_req);
// void cvi_ep0_read(struct cvi_udc *dev);
// void cvi_ep0_kick(struct cvi_udc *dev, struct cvi_ep *ep);
// void cvi_handle_ep0(struct cvi_udc *dev);
// int cvi_ep0_write(struct cvi_udc *dev);
// int cvi_write_fifo_ep0(struct cvi_ep *ep, struct cvi_request *req);
// void cvi_set_address(struct cvi_udc *dev, unsigned char address);
// int cvi_udc_set_halt(struct usb_ep *_ep, int value);
// void cvi_udc_set_nak(struct cvi_ep *ep);
// void cvi_done(struct cvi_ep *ep, struct cvi_request *req, int status);
// void cvi_nuke(struct cvi_ep *ep, int status);
// void cvi_reconfig_usbd(struct cvi_udc *dev, int disconnect);
// const char *cvi_get_ep0_name(void);

#endif	/* __CVI_USB_GADGET */
