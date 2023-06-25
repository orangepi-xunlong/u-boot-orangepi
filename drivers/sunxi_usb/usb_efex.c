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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
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
#include "usb_efex.h"
#include "efex_queue.h"
//#include <sys_config_old.h>
#include <sunxi_mbr.h>
#include <sprite.h>


#ifndef CONFIG_SUNXI_SPINOR
#define _EFEX_USE_BUF_QUEUE_
#endif

#define  SUNXI_USB_EFEX_IDLE					 (0)
#define  SUNXI_USB_EFEX_SETUP					 (1)
#define  SUNXI_USB_EFEX_SEND_DATA				 (2)
#define  SUNXI_USB_EFEX_RECEIVE_DATA			 (3)
#define  SUNXI_USB_EFEX_STATUS					 (4)
#define  SUNXI_USB_EFEX_EXIT					 (5)

#define  SUNXI_USB_EFEX_SETUP_NEW                (11)
#define  SUNXI_USB_EFEX_SEND_DATA_NEW            (12)
#define  SUNXI_USB_EFEX_RECEIVE_DATA_NEW         (13)
#define  FES_NEW_CMD_LEN                         (20)



#define  SUNXI_USB_EFEX_APPS_MAST				 (0xf0000)

#define  SUNXI_USB_EFEX_APPS_IDLE				 (0x10000)
#define  SUNXI_USB_EFEX_APPS_CMD				 (0x20000)
#define  SUNXI_USB_EFEX_APPS_DATA				 (0x30000)
#define  SUNXI_USB_EFEX_APPS_SEND_DATA		     (SUNXI_USB_EFEX_APPS_DATA | SUNXI_USB_EFEX_SEND_DATA)
#define  SUNXI_USB_EFEX_APPS_RECEIVE_DATA		 (SUNXI_USB_EFEX_APPS_DATA | SUNXI_USB_EFEX_RECEIVE_DATA)
#define  SUNXI_USB_EFEX_APPS_STATUS				 ((0x40000)  | SUNXI_USB_EFEX_STATUS)
#define  SUNXI_USB_EFEX_APPS_EXIT				 ((0x50000)  | SUNXI_USB_EFEX_EXIT)

extern int sunxi_flash_get_boot0_size(void);
extern int sunxi_flash_get_boot1_size(void);
extern void sunxi_nand_boot1_dump_for_efex(void *mem, int len);
static  int sunxi_usb_efex_write_enable = 0;
static  int sunxi_usb_efex_status = SUNXI_USB_EFEX_IDLE;
static  int sunxi_usb_efex_app_step = SUNXI_USB_EFEX_APPS_IDLE;
static  efex_trans_set_t  trans_data;
static  u8  *cmd_buf;
static  u32 sunxi_efex_next_action = 0;
static  struct pmu_config_t  pmu_config;
static  struct multi_unseq_mem_s global_unseq_mem_addr;
#if defined(SUNXI_USB_30)
static  int sunxi_usb_efex_status_enable = 1;
#endif

static u32 dma_recv_time_out;
extern int do_bootelf(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
int runtime_tick(void);

int  efex_suspend_flag = 0;
int efex_ubi_init_flag;
static int usb_debug_mode = -1;

DECLARE_GLOBAL_DATA_PTR;
/*
*******************************************************************************
*                     do_usb_req_set_interface
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
static int __usb_set_interface(struct usb_device_request *req)
{
	sunxi_usb_dbg("set interface\n");
	/* Only support interface 0, alternate 0 */
	if((0 == req->wIndex) && (0 == req->wValue))
	{
		sunxi_udc_ep_reset();
	}
	else
	{
		printf("err: invalid wIndex and wValue, (0, %d), (0, %d)\n", req->wIndex, req->wValue);
		return SUNXI_USB_REQ_OP_ERR;
	}

	return SUNXI_USB_REQ_SUCCESSED;
}

/*
*******************************************************************************
*                     do_usb_req_set_address
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
static int __usb_set_address(struct usb_device_request *req)
{
	uchar address;

	address = req->wValue & 0x7f;
	__udelay(10);
	printf("set address 0x%x\n", address);

	sunxi_udc_set_address(address);

	return SUNXI_USB_REQ_SUCCESSED;
}

/*
*******************************************************************************
*                     do_usb_req_set_configuration
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
static int __usb_set_configuration(struct usb_device_request *req)
{
	sunxi_usb_dbg("set configuration\n");
	/* Only support 1 configuration so nak anything else */
	if (1 == req->wValue)
	{
		sunxi_udc_ep_reset();
	}
	else
	{
		printf("err: invalid wValue, (0, %d)\n", req->wValue);

		return SUNXI_USB_REQ_OP_ERR;
	}

	sunxi_udc_set_configuration(req->wValue);

	return SUNXI_USB_REQ_SUCCESSED;
}
/*
*******************************************************************************
*                     do_usb_req_get_descriptor
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
static int __usb_get_descriptor(struct usb_device_request *req, uchar *buffer)
{
	int ret = SUNXI_USB_REQ_SUCCESSED;

	//获取描述符
	switch(req->wValue >> 8)
	{
		case USB_DT_DEVICE:		//设备描述符
		{
			struct usb_device_descriptor *dev_dscrptr;

			sunxi_usb_dbg("get device descriptor\n");

			dev_dscrptr = (struct usb_device_descriptor *)buffer;
			memset((void *)dev_dscrptr, 0, sizeof(struct usb_device_descriptor));

			dev_dscrptr->bLength = MIN(req->wLength, sizeof (struct usb_device_descriptor));
			dev_dscrptr->bDescriptorType    = USB_DT_DEVICE;
#ifdef CONFIG_USB_1_1_DEVICE
			dev_dscrptr->bcdUSB             = 0x110;
#else
			dev_dscrptr->bcdUSB             = 0x200;
#endif
			dev_dscrptr->bDeviceClass       = 0;
			dev_dscrptr->bDeviceSubClass    = 0;
			dev_dscrptr->bDeviceProtocol    = 0;
			dev_dscrptr->bMaxPacketSize0    = 0x40;
			dev_dscrptr->idVendor           = DRIVER_VENDOR_ID;
			dev_dscrptr->idProduct          = DRIVER_PRODUCT_ID;
			dev_dscrptr->bcdDevice          = 0xffff;
			//ignored
			//dev_dscrptr->iManufacturer      = SUNXI_USB_STRING_IMANUFACTURER;
			//dev_dscrptr->iProduct           = SUNXI_USB_STRING_IPRODUCT;
			//dev_dscrptr->iSerialNumber      = SUNXI_USB_STRING_ISERIALNUMBER;
			dev_dscrptr->iManufacturer      = 0;
			dev_dscrptr->iProduct           = 0;
			dev_dscrptr->iSerialNumber      = 0;
			dev_dscrptr->bNumConfigurations = 1;

			sunxi_udc_send_setup(dev_dscrptr->bLength, buffer);
		}
		break;

		case USB_DT_CONFIG:		//配置描述符
		{
			struct usb_configuration_descriptor *config_dscrptr;
			struct usb_interface_descriptor 	*inter_dscrptr;
			struct usb_endpoint_descriptor 		*ep_in, *ep_out;
			unsigned char bytes_remaining = req->wLength;
			unsigned char bytes_total = 0;

			sunxi_usb_dbg("get config descriptor\n");

			bytes_total = sizeof (struct usb_configuration_descriptor) + \
						  sizeof (struct usb_interface_descriptor) 	   + \
						  sizeof (struct usb_endpoint_descriptor) 	   + \
						  sizeof (struct usb_endpoint_descriptor);

			memset(buffer, 0, bytes_total);

			config_dscrptr = (struct usb_configuration_descriptor *)(buffer + 0);
			inter_dscrptr  = (struct usb_interface_descriptor 	  *)(buffer + 						\
																	 sizeof(struct usb_configuration_descriptor));
			ep_in 		   = (struct usb_endpoint_descriptor 	  *)(buffer + 						\
																	 sizeof(struct usb_configuration_descriptor) + 	\
																	 sizeof(struct usb_interface_descriptor));
			ep_out 		   = (struct usb_endpoint_descriptor 	  *)(buffer + 						\
																	 sizeof(struct usb_configuration_descriptor) + 	\
																	 sizeof(struct usb_interface_descriptor)	 +	\
																	 sizeof(struct usb_endpoint_descriptor));

			/* configuration */
			config_dscrptr->bLength            	= MIN(bytes_remaining, sizeof (struct usb_configuration_descriptor));
			config_dscrptr->bDescriptorType    	= USB_DT_CONFIG;
			config_dscrptr->wTotalLength 		= bytes_total;
			config_dscrptr->bNumInterfaces     	= 1;
			config_dscrptr->bConfigurationValue	= 1;
			config_dscrptr->iConfiguration     	= 0;
			config_dscrptr->bmAttributes       	= 0x80;		//not self powered
			config_dscrptr->bMaxPower          	= 0xFA;		//最大电流500ms(0xfa * 2)

			bytes_remaining 				   -= config_dscrptr->bLength;
			/* interface */
			inter_dscrptr->bLength             = MIN (bytes_remaining, sizeof(struct usb_interface_descriptor));
			inter_dscrptr->bDescriptorType     = USB_DT_INTERFACE;
			inter_dscrptr->bInterfaceNumber    = 0x00;
			inter_dscrptr->bAlternateSetting   = 0x00;
			inter_dscrptr->bNumEndpoints       = 0x02;
			inter_dscrptr->bInterfaceClass     = 0xff;
			inter_dscrptr->bInterfaceSubClass  = 0xff;
			inter_dscrptr->bInterfaceProtocol  = 0xff;
			inter_dscrptr->iInterface          = 0;

			bytes_remaining 				  -= inter_dscrptr->bLength;
			/* ep_in */
			ep_in->bLength            = MIN (bytes_remaining, sizeof (struct usb_endpoint_descriptor));
			ep_in->bDescriptorType    = USB_DT_ENDPOINT;
			ep_in->bEndpointAddress   = sunxi_udc_get_ep_in_type(); /* IN */
			ep_in->bmAttributes       = USB_ENDPOINT_XFER_BULK;
			ep_in->wMaxPacketSize 	  = sunxi_udc_get_ep_max();
			ep_in->bInterval          = 0x00;

			bytes_remaining 		 -= ep_in->bLength;
			/* ep_out */
			ep_out->bLength            = MIN (bytes_remaining, sizeof (struct usb_endpoint_descriptor));
			ep_out->bDescriptorType    = USB_DT_ENDPOINT;
			ep_out->bEndpointAddress   = sunxi_udc_get_ep_out_type(); /* OUT */
			ep_out->bmAttributes       = USB_ENDPOINT_XFER_BULK;
			ep_out->wMaxPacketSize 	   = sunxi_udc_get_ep_max();
			ep_out->bInterval          = 0x00;

			bytes_remaining 		  -= ep_out->bLength;

			sunxi_udc_send_setup(MIN(req->wLength, bytes_total), buffer);
		}
		break;

		case USB_DT_STRING:
		{
			unsigned char bLength = 0;
			unsigned char string_index = req->wValue & 0xff;

			sunxi_usb_dbg("get string descriptor\n");

			/* Language ID */
			if(string_index == 0)
			{
				bLength = MIN(4, req->wLength);

				buffer[0] = bLength;
				buffer[1] = USB_DT_STRING;
				buffer[2] = 9;
				buffer[3] = 4;
				sunxi_udc_send_setup(bLength, (void *)buffer);
			}
			else
			{
				bLength = MIN(4, req->wLength);

				buffer[0] = bLength;
				buffer[1] = USB_DT_STRING;
				buffer[2] = 9;
				buffer[3] = 4;
				sunxi_udc_send_setup(bLength, (void *)buffer);

				printf("sunxi usb err: string line %d is not supported\n", string_index);
			}
		}
		break;

		case USB_DT_DEVICE_QUALIFIER:
		{
#ifdef CONFIG_USB_1_1_DEVICE
			/* This is an invalid request for usb 1.1, nak it */
			USBC_Dev_EpSendStall(sunxi_udc_source.usbc_hd, USBC_EP_TYPE_EP0);
#else
			struct usb_qualifier_descriptor *qua_dscrpt;

			sunxi_usb_dbg("get qualifier descriptor\n");

			qua_dscrpt = (struct usb_qualifier_descriptor *)buffer;
			memset(&buffer, 0, sizeof(struct usb_qualifier_descriptor));

			qua_dscrpt->bLength = MIN(req->wLength, sizeof(struct usb_qualifier_descriptor));
			qua_dscrpt->bDescriptorType    = USB_DT_DEVICE_QUALIFIER;
			qua_dscrpt->bcdUSB             = 0x200;
			qua_dscrpt->bDeviceClass       = 0xff;
			qua_dscrpt->bDeviceSubClass    = 0xff;
			qua_dscrpt->bDeviceProtocol    = 0xff;
			qua_dscrpt->bMaxPacketSize0    = 0x40;
			qua_dscrpt->bNumConfigurations = 1;
			qua_dscrpt->breserved          = 0;

			sunxi_udc_send_setup(qua_dscrpt->bLength, buffer);
#endif
		}
		break;

		default:
			printf("err: unkown wValue(%d)\n", req->wValue);

			ret = SUNXI_USB_REQ_OP_ERR;
	}

	return ret;
}

/*
*******************************************************************************
*                     do_usb_req_get_status
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
static int __usb_get_status(struct usb_device_request *req, uchar *buffer)
{
	unsigned char bLength = 0;

	sunxi_usb_dbg("get status\n");
	if(0 == req->wLength)
	{
		/* sent zero packet */
		sunxi_udc_send_setup(0, NULL);

		return SUNXI_USB_REQ_OP_ERR;
	}

	bLength = MIN(req->wValue, 2);

	buffer[0] = 1;
	buffer[1] = 0;

	sunxi_udc_send_setup(bLength, buffer);

	return SUNXI_USB_REQ_SUCCESSED;
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
static int __sunxi_efex_send_status(void *buffer, unsigned int buffer_size)
{
	return sunxi_udc_send_data((uchar *)buffer, buffer_size);
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
static int sunxi_efex_init(void)
{
	sunxi_usb_dbg("sunxi_efex_init\n");
	memset(&trans_data, 0, sizeof(efex_trans_set_t));
	sunxi_usb_efex_write_enable = 0;
    sunxi_usb_efex_status = SUNXI_USB_EFEX_IDLE;
    sunxi_usb_efex_app_step = SUNXI_USB_EFEX_APPS_IDLE;

	cmd_buf = (u8 *)malloc_align(CBW_MAX_CMD_SIZE, 64);
	if(!cmd_buf)
	{
		printf("sunxi usb efex err: unable to malloc memory for cmd\n");

		return -1;
	}
    trans_data.base_recv_buffer = (u8 *)malloc_align(SUNXI_EFEX_RECV_MEM_SIZE,64);
    if(!trans_data.base_recv_buffer)
    {
    	printf("sunxi usb efex err: unable to malloc memory for efex receive\n");
		free(cmd_buf);

    	return -1;
    }

	trans_data.base_send_buffer = (u8 *)malloc_align(SUNXI_EFEX_RECV_MEM_SIZE, 64);
    if(!trans_data.base_send_buffer)
    {
    	printf("sunxi usb efex err: unable to malloc memory for efex send\n");
    	free(trans_data.base_recv_buffer);
		free(cmd_buf);

    	return -1;
    }
    /*     sunxi_usb_dbg("recv addr 0x%x\n", (ulong)trans_data.base_recv_buffer);
     * sunxi_usb_dbg("send addr 0x%x\n", (ulong)trans_data.base_send_buffer); */

#ifdef _EFEX_USE_BUF_QUEUE_
    if(efex_queue_init())
    {
        return -1;
    }
#endif
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
static int sunxi_efex_exit(void)
{
	sunxi_usb_dbg("sunxi_efex_exit\n");
    if(trans_data.base_recv_buffer)
    {
		free_align(trans_data.base_recv_buffer);
    }
    if(trans_data.base_send_buffer)
    {
		free_align(trans_data.base_send_buffer);
    }
    if(cmd_buf)
    {
		free_align(cmd_buf);
	}
#ifdef _EFEX_USE_BUF_QUEUE_
    efex_queue_exit();
#endif
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
static void sunxi_efex_reset(void)
{
	sunxi_usb_efex_write_enable = 0;
    sunxi_usb_efex_status = SUNXI_USB_EFEX_IDLE;
    sunxi_usb_efex_app_step = SUNXI_USB_EFEX_APPS_IDLE;
    trans_data.to_be_recved_size = 0;
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
static void  sunxi_efex_usb_rx_dma_isr(void *p_arg)
{
	sunxi_usb_dbg("dma int for usb rx occur\n");
	//通知主循环，可以写入数据
	sunxi_usb_efex_write_enable = 1;
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
static void  sunxi_efex_usb_tx_dma_isr(void *p_arg)
{
	sunxi_usb_dbg("dma int for usb tx occur\n");

#if defined(SUNXI_USB_30)
	sunxi_usb_efex_status_enable ++;
#endif
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
static int sunxi_efex_standard_req_op(uint cmd, struct usb_device_request *req, uchar *buffer)
{
	int ret = SUNXI_USB_REQ_OP_ERR;

	switch(cmd)
	{
		case USB_REQ_GET_STATUS:
		{
			ret = __usb_get_status(req, buffer);

			break;
		}
		//case USB_REQ_CLEAR_FEATURE:
		//case USB_REQ_SET_FEATURE:
		case USB_REQ_SET_ADDRESS:
		{
			ret = __usb_set_address(req);

			break;
		}
		case USB_REQ_GET_DESCRIPTOR:
		//case USB_REQ_SET_DESCRIPTOR:
		case USB_REQ_GET_CONFIGURATION:
		{
			ret = __usb_get_descriptor(req, buffer);

			break;
		}
		case USB_REQ_SET_CONFIGURATION:
		{
			ret = __usb_set_configuration(req);

			break;
		}
		//case USB_REQ_GET_INTERFACE:
		case USB_REQ_SET_INTERFACE:
		{
			ret = __usb_set_interface(req);

			break;
		}
		//case USB_REQ_SYNCH_FRAME:
		default:
		{
			printf("sunxi efex error: standard req is not supported\n");

			ret = SUNXI_USB_REQ_DEVICE_NOT_SUPPORTED;

			break;
		}
	}

	return ret;
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
static int sunxi_efex_nonstandard_req_op(uint cmd, struct usb_device_request *req, uchar *buffer, uint data_status)
{
	int ret = SUNXI_USB_REQ_SUCCESSED;

	switch(req->bmRequestType)
	{
		case 161:
			if(req->bRequest == 0xFE)
			{
				sunxi_usb_dbg("efex ask for max lun\n");

				buffer[0] = 0;

				sunxi_udc_send_setup(1, buffer);
			}
			else
			{
				printf("sunxi usb err: unknown ep0 req in efex\n");

				ret = SUNXI_USB_REQ_DEVICE_NOT_SUPPORTED;
			}
			break;

		default:
			printf("sunxi usb err: unknown non standard ep0 req\n");

			ret = SUNXI_USB_REQ_DEVICE_NOT_SUPPORTED;

			break;
	}

	return ret;
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
static void __sunxi_usb_efex_fill_status(void)
{
	Status_t     *efex_status;

	efex_status = (Status_t *)trans_data.base_send_buffer;
	memset(efex_status, 0, sizeof(Status_t));
	efex_status->mark  = 0xffff;
	efex_status->tag   = 0;
	efex_status->state = 0;

	trans_data.act_send_buffer = trans_data.base_send_buffer;
	trans_data.send_size = sizeof(Status_t);

	return;
}

int sunxi_print_usb_efex_cmd(u8 *cmd_buffer)
{
	struct global_cmd_s  *cmd = (struct global_cmd_s *)cmd_buffer;
	if (usb_debug_mode < 0)
		script_parser_fetch("/soc/platform", "usb_debug_mode",
				    &usb_debug_mode, 0);
	if (usb_debug_mode) {
		printf("\t app_cmd: ");
		switch (cmd->app_cmd) {
		case APP_LAYER_COMMEN_CMD_VERIFY_DEV:
			printf("APP_LAYER_COMMEN_CMD_VERIFY_DEV\t\n");
			break;
		case APP_LAYER_COMMEN_CMD_SWITCH_ROLE:
			printf("APP_LAYER_COMMEN_CMD_SWITCH_ROLE\t\n");
			break;
		case APP_LAYER_COMMEN_CMD_IS_READY:
			printf("APP_LAYER_COMMEN_CMD_IS_READY\t\n");
			break;
		case APP_LAYER_COMMEN_CMD_GET_CMD_SET_VER:
			printf("APP_LAYER_COMMEN_CMD_GET_CMD_SET_VER\t\n");
			break;
		case APP_LAYER_COMMEN_CMD_DISCONNECT:
			printf("APP_LAYER_COMMEN_CMD_DISCONNECT\t\n");
			break;
		case FEX_CMD_fes_trans:
			printf("FEX_CMD_fes_trans\t\n");
			break;
		case FEX_CMD_fes_run:
			printf("FEX_CMD_fes_run\t\n");
			break;
		case FEX_CMD_fes_down:
			printf("FEX_CMD_fes_down\t\n");
			break;
		case FEX_CMD_fes_up:
			printf("FEX_CMD_fes_up\t\n");
			break;
		case FEX_CMD_fes_verify_value:
			printf("FEX_CMD_fes_verify_value\t\n");
			break;
		case FEX_CMD_fes_verify_status:
			printf("FEX_CMD_fes_verify_status\t\n");
			break;
		case FEX_CMD_fes_query_storage:
			printf("FEX_CMD_fes_query_storage\t\n");
			break;
		case FEX_CMD_fes_flash_set_on:
			printf("FEX_CMD_fes_flash_set_on\t\n");
			break;
		case FEX_CMD_fes_flash_set_off:
			printf("FEX_CMD_fes_flash_set_off\t\n");
			break;
		case FEX_CMD_fes_flash_size_probe:
			printf("FEX_CMD_fes_flash_size_probe\t\n");
			break;
		case FEX_CMD_fes_tool_mode:
			printf("FEX_CMD_fes_tool_mode\t\n");
			break;
		case FEX_CMD_fes_memset:
			printf("FEX_CMD_fes_memset\t\n");
			break;
		case FEX_CMD_fes_pmu:
			printf("FEX_CMD_fes_pmu\t\n");
			break;
		case FEX_CMD_fes_unseqmem_read:
			printf("FEX_CMD_fes_unseqmem_read\t\n");
			break;
		case FEX_CMD_fes_unseqmem_write:
			printf("FEX_CMD_fes_unseqmem_write\t\n");
			break;
		case FEX_CMD_fes_force_erase:
			printf("FEX_CMD_fes_force_erase\t\n");
			break;
		case FEX_CMD_fes_force_erase_key:
			printf("FEX_CMD_fes_force_erase_key\t\n");
			break;
		case FEX_CMD_fes_query_secure:
			printf("FEX_CMD_fes_query_secure\t\n");
			break;
		case FEX_CMD_fes_query_info:
			printf("FEX_CMD_fes_query_info\t\n");
			break;
		default:
			printf("%d unknown cmd:%d\t\n", __LINE__, cmd->app_cmd);
			break;
		}
	}
	return 0;
}

int sunxi_print_usb_efex_next_status(int app_next_status)
{
	if (usb_debug_mode < 0)
		script_parser_fetch("/soc/platform", "usb_debug_mode",
				    &usb_debug_mode, 0);
	if (usb_debug_mode) {
		printf("\t app_next_status: ");
		switch (app_next_status) {
		case SUNXI_USB_EFEX_APPS_IDLE:
			printf("SUNXI_USB_EFEX_APPS_IDLE\t\n");
			break;
		case SUNXI_USB_EFEX_APPS_CMD:
			printf("SUNXI_USB_EFEX_APPS_CMD\t\n");
			break;
		case SUNXI_USB_EFEX_APPS_DATA:
			printf("SUNXI_USB_EFEX_APPS_DATA\t\n");
			break;
		case SUNXI_USB_EFEX_APPS_SEND_DATA:
			printf("SUNXI_USB_EFEX_APPS_SEND_DATA\t\n");
			break;
		case SUNXI_USB_EFEX_APPS_RECEIVE_DATA:
			printf("SUNXI_USB_EFEX_APPS_RECEIVE_DATA\t\n");
			break;
		case SUNXI_USB_EFEX_APPS_STATUS:
			printf("SUNXI_USB_EFEX_APPS_STATUS\t\n");
			break;
		case SUNXI_USB_EFEX_APPS_EXIT:
			printf("SUNXI_USB_EFEX_APPS_EXIT\t\n");
			break;
		default:
			printf("%d unkown app_next_status:%d\n", __LINE__, app_next_status);
			break;
		}

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
static int __sunxi_usb_efex_op_cmd(u8 *cmd_buffer)
{
	struct global_cmd_s  *cmd = (struct global_cmd_s *)cmd_buffer;

	sunxi_print_usb_efex_cmd(cmd_buffer);
	switch(cmd->app_cmd)
	{
		case APP_LAYER_COMMEN_CMD_VERIFY_DEV:
			{
				struct verify_dev_data_s *app_verify_dev;

				app_verify_dev = (struct verify_dev_data_s *)trans_data.base_send_buffer;

				memcpy(app_verify_dev->tag, AL_VERIFY_DEV_TAG_DATA, sizeof(AL_VERIFY_DEV_TAG_DATA));
				app_verify_dev->platform_id_hw 		= FES_PLATFORM_HW_ID;
				app_verify_dev->platform_id_fw 		= 0x0001;
				app_verify_dev->mode 				= AL_VERIFY_DEV_MODE_SRV;//固定的，
				app_verify_dev->pho_data_flag 		= 'D';
				app_verify_dev->pho_data_len 		= PHOENIX_PRIV_DATA_LEN_NR;
				app_verify_dev->pho_data_start_addr = PHOENIX_PRIV_DATA_ADDR;

				trans_data.act_send_buffer   = trans_data.base_send_buffer;
				trans_data.send_size         = sizeof(struct verify_dev_data_s);
				trans_data.last_err          = 0;
				trans_data.app_next_status   = SUNXI_USB_EFEX_APPS_SEND_DATA;
  			}

			break;

		case APP_LAYER_COMMEN_CMD_SWITCH_ROLE:
			sunxi_usb_dbg("not supported\n");

			trans_data.last_err          = -1;
			trans_data.app_next_status   = SUNXI_USB_EFEX_APPS_STATUS;

			break;

		case APP_LAYER_COMMEN_CMD_IS_READY:

			{
				struct is_ready_data_s *app_is_ready_data;

				app_is_ready_data = (struct is_ready_data_s *)trans_data.base_send_buffer;

				app_is_ready_data->interval_ms = 500;
				app_is_ready_data->state = AL_IS_READY_STATE_READY;

				trans_data.act_send_buffer   = trans_data.base_send_buffer;
				trans_data.send_size         = sizeof(struct is_ready_data_s);
				trans_data.last_err          = 0;
				trans_data.app_next_status   = SUNXI_USB_EFEX_APPS_SEND_DATA;
			}

			break;

		case APP_LAYER_COMMEN_CMD_GET_CMD_SET_VER:

			{
				struct get_cmd_set_ver_data_s *app_get_cmd_ver_data;

				app_get_cmd_ver_data = (struct get_cmd_set_ver_data_s *)trans_data.base_send_buffer;

				app_get_cmd_ver_data->ver_high = 2;
				app_get_cmd_ver_data->ver_low = 0;

				trans_data.act_send_buffer   = trans_data.base_send_buffer;
				trans_data.send_size         = sizeof(struct get_cmd_set_ver_data_s);
				trans_data.last_err          = 0;
				trans_data.app_next_status   = SUNXI_USB_EFEX_APPS_SEND_DATA;
			}

			break;

		case APP_LAYER_COMMEN_CMD_DISCONNECT:
			sunxi_usb_dbg("not supported\n");

			trans_data.last_err          = -1;
			trans_data.app_next_status   = SUNXI_USB_EFEX_APPS_STATUS;

			break;

		case FEX_CMD_fes_trans:

			//需要发送数据
			{
				fes_trans_old_t  *fes_old_data = (fes_trans_old_t *)cmd_buf;

				if(fes_old_data->len)
				{
					if(fes_old_data->u2.DOU == 2)		//上传数据
					{
#ifdef SUNXI_USB_DEBUG
						ulong value;
						value = *(uint *)(ulong)fes_old_data->addr;
						sunxi_usb_dbg("send id 0x%lx, addr 0x%x, length 0x%x\n", value, fes_old_data->addr, fes_old_data->len);
#endif
						trans_data.act_send_buffer   = (void*)(ulong)fes_old_data->addr;	//设置发送地址
						trans_data.send_size         = fes_old_data->len;	//设置发送长度
						trans_data.last_err          = 0;
						trans_data.app_next_status   = SUNXI_USB_EFEX_APPS_SEND_DATA;
					}
					else	//(fes_old_data->u2.DOU == (0 or 1))	//下载数据
					{
#ifdef SUNXI_USB_DEBUG
						ulong value;
						value = *(uint *)(ulong)fes_old_data->addr;
						sunxi_usb_dbg("receive id 0x%lx, addr 0x%x, length 0x%x\n", value, fes_old_data->addr, fes_old_data->len);
#endif

						trans_data.type = SUNXI_EFEX_DRAM_TAG;		//写到dram的数据
						trans_data.act_recv_buffer   = (void*)(ulong)fes_old_data->addr;	//设置接收地址
						trans_data.recv_size         = fes_old_data->len;	//设置接收长度
						trans_data.last_err          = 0;
						trans_data.app_next_status   = SUNXI_USB_EFEX_APPS_RECEIVE_DATA;
					}
				}
				else
				{
					printf("FEX_CMD_fes_trans: no data need to send or receive\n");

					trans_data.app_next_status = SUNXI_USB_EFEX_APPS_STATUS;
				}
			}
			trans_data.last_err = 0;

			break;

		case FEX_CMD_fes_run:
			{
#ifdef  CONFIG_CMD_ELF
				fes_run_t   *runs = (fes_run_t *)cmd_buf;
				int         *app_ret;
				char run_addr[32] = {0};
				char paras[8][16];
				char *const  usb_runs_args[12] = {NULL, run_addr, 					\
					                            (char *)&paras[0][0], 				\
					                            (char *)&paras[1][0],				\
					                            (char *)&paras[2][0],				\
					                            (char *)&paras[3][0],				\
					                            (char *)&paras[4][0],				\
					                            (char *)&paras[5][0],				\
					                            (char *)&paras[6][0],				\
					                        	(char *)&paras[7][0]};

				sprintf(run_addr, "%x", runs->addr);
				printf("usb run addr      = %s\n", run_addr);
				printf("usb run paras max = %d\n", runs->max_para);
				{
					int i;
					int *data;

					data = runs->para_addr;
					for(i=0;i<runs->max_para;i++)
					{
						printf("usb run paras[%d] = 0x%x\n", i, data[i]);
						sprintf((char *)&paras[i][0], "%x", data[i]);
					}
				}

				app_ret = (int *)trans_data.base_send_buffer;
				*app_ret = do_bootelf(NULL, 0, runs->max_para + 1, usb_runs_args);
				printf("usb get result = %d\n", *app_ret);
				trans_data.act_send_buffer   = trans_data.base_send_buffer;
				trans_data.send_size         = 4;
				trans_data.last_err          = 0;
				trans_data.app_next_status   = SUNXI_USB_EFEX_APPS_SEND_DATA;
#else
				int         *app_ret;

				app_ret = (int *)trans_data.base_send_buffer;
				*app_ret = -1;
				trans_data.act_send_buffer   = trans_data.base_send_buffer;
				trans_data.send_size         = 4;
				trans_data.last_err          = 0;
				trans_data.app_next_status   = SUNXI_USB_EFEX_APPS_SEND_DATA;
#endif
			}

			break;

		case FEX_CMD_fes_down:
			{
				fes_trans_t  *trans = (fes_trans_t *)cmd_buf;

				trans_data.type  = trans->type;									 //数据类型，MBR,BOOT1,BOOT0...以及分区类型
				if((trans->type & SUNXI_EFEX_DRAM_MASK) == SUNXI_EFEX_DRAM_MASK) //如果属于内存数据，则执行这里
				{
					if((SUNXI_EFEX_DRAM_MASK | SUNXI_EFEX_TRANS_FINISH_TAG) == trans->type)
					{
					    trans_data.act_recv_buffer = trans_data.base_recv_buffer;
						trans_data.dram_trans_buffer = (void*)(ulong)trans->addr;
						//printf("dram write: start 0x%x: length 0x%x\n", trans->addr, trans->len);
					}
					else
					{
						trans_data.act_recv_buffer   = trans_data.base_recv_buffer + trans_data.to_be_recved_size;	 //设置接收地址
					}
					trans_data.recv_size         = trans->len;	//设置接收长度，字节单位
					trans_data.to_be_recved_size += trans->len;
					sunxi_usb_dbg("down dram: start 0x%x, sectors 0x%x\n", trans_data.flash_start, trans_data.flash_sectors);
				}
				else	//属于flash数据，分别表示起始扇区，扇区数
				{
					trans_data.act_recv_buffer   = (trans_data.base_recv_buffer + SUNXI_EFEX_RECV_MEM_SIZE/2);	 //设置接收地址
					trans_data.recv_size         = trans->len;	//设置接收长度，字节单位

					trans_data.flash_start       = trans->addr;
					trans_data.flash_sectors     = (trans->len + 511) >> 9;
					sunxi_usb_dbg("down flash: start 0x%x, sectors 0x%x\n", trans_data.flash_start, trans_data.flash_sectors);
				}
				trans_data.last_err          = 0;
				trans_data.app_next_status   = SUNXI_USB_EFEX_APPS_RECEIVE_DATA;
			}

			break;
		case FEX_CMD_fes_up:
#ifdef CONFIG_SUNXI_UBIFS
			if ((get_boot_storage_type() == STORAGE_NAND) &&
					efex_ubi_init_flag == 0 &&
					nand_use_ubi()) {
				ubi_nand_probe_uboot();
				ubi_nand_attach_mtd();
				efex_ubi_init_flag = 1;
			}
#endif

			{
				fes_trans_t  *trans = (fes_trans_t *)cmd_buf;

				trans_data.last_err          	 = 0;
				trans_data.app_next_status   	 = SUNXI_USB_EFEX_APPS_SEND_DATA;
				trans_data.type  = trans->type;									 //数据类型，MBR,BOOT1,BOOT0...以及分区类型
				if((trans->type & SUNXI_EFEX_DRAM_MASK) == SUNXI_EFEX_DRAM_MASK) //如果属于内存数据，则执行这里
				{
#if 0
					if((SUNXI_EFEX_DRAM_MASK | SUNXI_EFEX_TRANS_FINISH_TAG) == trans->type)
					{
					    trans_data.act_send_buffer = (uint)trans_data.base_send_buffer;
						trans_data.dram_trans_buffer = trans->addr;
						printf("dram write: start 0x%x: length 0x%x\n", trans->addr, trans->len);
					}
					else
					{
						trans_data.act_send_buffer   = (uint)trans_data.base_send_buffer + trans_data.send_size;	 //设置发送数据地址
					}
#endif

					trans_data.act_send_buffer   = (void*)(ulong)trans->addr;	//设置发送地址，属于字节单位
					trans_data.send_size         = trans->len;	//设置接收长度，字节单位
					sunxi_usb_dbg("dram read: start 0x%x: length 0x%x\n", trans->addr, trans->len);
				} else if ((trans->type & SUNXI_EFEX_FLASH_BOOT0_TAG) == SUNXI_EFEX_FLASH_BOOT0_TAG) {
					trans_data.act_send_buffer   = trans_data.base_send_buffer;
					trans_data.send_size         = trans->len;
					trans_data.flash_start       = trans->addr;
					trans_data.flash_sectors     = (trans->len + 511) >> 9;
					printf("upload boot0 flash: start 0x%x, sectors 0x%x\n", trans_data.flash_start, trans_data.flash_sectors);
					if (get_boot_storage_type() == STORAGE_EMMC ||
							get_boot_storage_type() == STORAGE_EMMC3 ||
							get_boot_storage_type() == STORAGE_EMMC0) {
						if (!sunxi_sprite_phyread(trans_data.flash_start, trans_data.flash_sectors, (void *)trans_data.act_send_buffer)) {
							printf("flash read err: start 0x%x, sectors 0x%x\n", trans_data.flash_start, trans_data.flash_sectors);
							trans_data.last_err      = -1;
						}
					} else {
						/* TODO: fix for nand/spinor */
						printf("not support dump boot0 now, skip\n");
						memcpy(trans_data.act_send_buffer, "dump boot0 not support", 22);
						trans_data.last_err      = 0;
					}
				} else if ((trans->type & SUNXI_EFEX_FLASH_BOOT1_TAG) == SUNXI_EFEX_FLASH_BOOT1_TAG) {
					trans_data.act_send_buffer   = trans_data.base_send_buffer;
					trans_data.send_size         = trans->len;
					trans_data.flash_start       = trans->addr;
					trans_data.flash_sectors     = (trans->len + 511) >> 9;
					sunxi_usb_dbg("upload boot1 flash: start 0x%x, sectors 0x%x\n", trans_data.flash_start, trans_data.flash_sectors);
					if (get_boot_storage_type() == STORAGE_EMMC ||
							get_boot_storage_type() == STORAGE_EMMC3 ||
							get_boot_storage_type() == STORAGE_EMMC0) {
						if (!sunxi_sprite_phyread(trans_data.flash_start, trans_data.flash_sectors, trans_data.act_send_buffer)) {
							trans_data.last_err      = -1;
						}
					} else {
						/* TODO: fix for nand/spinor */
						printf("not support dump uboot now, skip\n");
						memset(trans_data.act_send_buffer, 0, trans_data.send_size);
						/* memcpy(trans_data.act_send_buffer, "sunxi-package,dump uboot not support", 32); */
						trans_data.last_err      = 0;
					}
					/*sunxi_dump(trans_data.act_send_buffer,64);*/
				}
				else	//属于flash数据，分别表示起始扇区，扇区数
				{
					trans_data.act_send_buffer   = trans_data.base_send_buffer;	 //设置发送地址
					trans_data.send_size         = trans->len;	//设置接收长度，字节单位

					trans_data.flash_start       = trans->addr; //设置发送地址，属于扇区单位
					trans_data.flash_sectors     = (trans->len + 511) >> 9;

					sunxi_usb_dbg("upload flash: start 0x%x, sectors 0x%x\n", trans_data.flash_start, trans_data.flash_sectors);
					if (!sunxi_flash_read(trans_data.flash_start, trans_data.flash_sectors, (void *)trans_data.act_send_buffer)) {
						printf("flash read err: start 0x%x, sectors 0x%x\n", trans_data.flash_start, trans_data.flash_sectors);

						trans_data.last_err = -1;
					}
					/* gpt */
					if (trans_data.flash_start == 0 && trans_data.flash_sectors == 0x80) {
						printf("convert gpt to mbr\n");
						sunxi_mbr_t *sunxi_mbr = malloc(SUNXI_MBR_SIZE);
						int boot_storage_type;
						if ((get_boot_storage_type() == STORAGE_EMMC3) || (get_boot_storage_type() == STORAGE_EMMC0)) {
							boot_storage_type = STORAGE_EMMC;
						} else {
							boot_storage_type = get_boot_storage_type();
						}
						gpt_convert_to_sunxi_mbr(sunxi_mbr, (char *)trans_data.act_send_buffer, boot_storage_type);
						memcpy(trans_data.act_send_buffer, sunxi_mbr, SUNXI_MBR_SIZE);
						free(sunxi_mbr);
					}
				}

			}

			break;

//		case FEX_CMD_fes_verify:
//			printf("FEX_CMD_fes_verify\n");
//			{
//				fes_cmd_verify_t  *cmd_verify = (fes_cmd_verify_t *)cmd_buf;
//				fes_efex_verify_t *verify_data= (fes_efex_verify_t *)trans_data.base_send_buffer;
//
//				printf("FEX_CMD_fes_verify cmd tag = 0x%x\n", cmd_verify->tag);
//				if(cmd_verify->tag == 0)		//来自于flash的校验
//				{
//					verify_data->media_crc = sunxi_sprite_part_rawdata_verify(cmd_verify->start, cmd_verify->size);
//				}
//				else							//来自于特别数据的校验
//				{
//					verify_data->media_crc = trans_data.last_err;
//				}
//				verify_data->flag = EFEX_CRC32_VALID_FLAG;
//				printf("FEX_CMD_fes_verify last err=%d\n", verify_data->media_crc);
//
//				trans_data.act_send_buffer   = (uint)trans_data.base_send_buffer;
//				trans_data.send_size         = sizeof(fes_efex_verify_t);
//				trans_data.app_next_status   = SUNXI_USB_EFEX_APPS_SEND_DATA;
////				//目前只支持校验和，或者查看状态的方式
////				if(data_type == SUNXI_EFEX_MBR_TAG)			//传输MBR已经完成
////				{
////					verify_data->flag = EFEX_CRC32_VALID_FLAG;
////				}
////				else if(data_type == SUNXI_EFEX_BOOT1_TAG)	//传输BOOT1已经完成
////				{
////					verify_data->flag = EFEX_CRC32_VALID_FLAG;
////				}
////				else if(data_type == SUNXI_EFEX_BOOT0_TAG)	//传输BOOT0已经完成
////				{
////					verify_data->flag = EFEX_CRC32_VALID_FLAG;
////				}
////				else										//其它数据，直接写入内存
////				{
////					memcpy((void *)trans_data.start, sunxi_ubuf->rx_data_buffer, trans_data.size);
////				}
//			}
//			break;

		case FEX_CMD_fes_verify_value:
			{
				fes_cmd_verify_value_t  *cmd_verify = (fes_cmd_verify_value_t *)cmd_buf;
				fes_efex_verify_t 		*verify_data= (fes_efex_verify_t *)trans_data.base_send_buffer;
#ifdef CONFIG_DISABLE_SUNXI_PART_DOWNLOAD
				printf("DISABLE SUNXI MBR : so start of verify would offset to -16K \n");
				printf("old offset :%x \n", cmd_verify->start);
				cmd_verify->start -= SUNXI_MBR_SIZE / 512;
#endif
				verify_data->media_crc = sunxi_sprite_part_rawdata_verify(cmd_verify->start, cmd_verify->size);
				verify_data->flag 	   = EFEX_CRC32_VALID_FLAG;

				printf("FEX_CMD_fes_verify_value, start 0x%x, size high 0x%x:low 0x%x\n", cmd_verify->start, (uint)(cmd_verify->size>>32), (uint)(cmd_verify->size));
				printf("FEX_CMD_fes_verify_value 0x%x\n", verify_data->media_crc);
			}
			trans_data.act_send_buffer   = trans_data.base_send_buffer;
			trans_data.send_size         = sizeof(fes_efex_verify_t);
			trans_data.app_next_status   = SUNXI_USB_EFEX_APPS_SEND_DATA;

			break;
		case FEX_CMD_fes_verify_status:
			printf("FEX_CMD_fes_verify_status\n");
			{
//				fes_cmd_verify_status_t *cmd_verify = (fes_cmd_verify_status_t *)cmd_buf;
				fes_efex_verify_t 	*verify_data= (fes_efex_verify_t *)trans_data.base_send_buffer;

				verify_data->flag 	   = EFEX_CRC32_VALID_FLAG;
				verify_data->media_crc = trans_data.last_err;

				printf("FEX_CMD_fes_verify last err=%d\n", verify_data->media_crc);
			}
			trans_data.act_send_buffer   = trans_data.base_send_buffer;
			trans_data.send_size         = sizeof(fes_efex_verify_t);
			trans_data.app_next_status   = SUNXI_USB_EFEX_APPS_SEND_DATA;

			break;
		case FEX_CMD_fes_query_storage:

			{
				uint *storage_type = (uint *)trans_data.base_send_buffer;

				if ((get_boot_storage_type() == STORAGE_EMMC3) || (get_boot_storage_type() == STORAGE_EMMC0))
                                {
                                        *storage_type = STORAGE_EMMC;
				} else
                                {
				        *storage_type = get_boot_storage_type();
                                }
				trans_data.act_send_buffer   = trans_data.base_send_buffer;
				trans_data.send_size         = 4;
				trans_data.last_err          = 0;
				trans_data.app_next_status   = SUNXI_USB_EFEX_APPS_SEND_DATA;
			}

			break;

		case FEX_CMD_fes_flash_set_on:

			trans_data.last_err = sunxi_sprite_init(0);
			trans_data.app_next_status   = SUNXI_USB_EFEX_APPS_STATUS;

			break;

		case FEX_CMD_fes_flash_set_off:

			trans_data.last_err = sunxi_sprite_exit(1);
			trans_data.app_next_status   = SUNXI_USB_EFEX_APPS_STATUS;

			break;

		case FEX_CMD_fes_flash_size_probe:

			{
				uint *flash_size = (uint *)trans_data.base_send_buffer;

				*flash_size = sunxi_flash_size();
				printf("flash sectors: 0x%x\n", *flash_size);
				trans_data.act_send_buffer   = trans_data.base_send_buffer;
				trans_data.send_size         = 4;
				trans_data.last_err          = 0;
				trans_data.app_next_status   = SUNXI_USB_EFEX_APPS_SEND_DATA;
			}
			break;

		case FEX_CMD_fes_tool_mode:

			{
				fes_efex_tool_t *fes_work = (fes_efex_tool_t *)cmd_buf;

				if(fes_work->tool_mode== WORK_MODE_USB_TOOL_UPDATE)
				{	//如果是升级工具，则直接重启
					if(fes_work->next_mode == 0)
					{
						sunxi_efex_next_action = SUNXI_UPDATE_NEXT_ACTION_REBOOT;
					}
					else
					{
						sunxi_efex_next_action = fes_work->next_mode;
					}
					trans_data.app_next_status = SUNXI_USB_EFEX_APPS_EXIT;
				}
		                else if(fes_work->tool_mode == WORK_MODE_ERASE_KEY)
		                {
		                    sunxi_efex_next_action = SUNXI_UPDATE_NEXT_ACTION_SHUTDOWN;
		                    trans_data.app_next_status = SUNXI_USB_EFEX_APPS_EXIT;
		                }
				else  //如果是量产工具，则根据配置处理
				{
					if(!fes_work->next_mode)
					{
						int ret=0;

						ret = script_parser_fetch("/soc/platform", "next_work", (int *)&sunxi_efex_next_action, 1);
						if (ret < 0)
						{
							sunxi_efex_next_action = SUNXI_UPDATE_NEXT_ACTION_NORMAL;
						}
					}
					else
					{
						sunxi_efex_next_action = SUNXI_UPDATE_NEXT_ACTION_REBOOT;
					}
					if((sunxi_efex_next_action <= SUNXI_UPDATE_NEXT_ACTION_NORMAL) || (sunxi_efex_next_action > SUNXI_UPDATE_NEXT_ACTION_REUPDATE))
					{
						sunxi_efex_next_action = SUNXI_UPDATE_NEXT_ACTION_NORMAL;
						trans_data.app_next_status   = SUNXI_USB_EFEX_APPS_STATUS;
					}
					else
					{
						trans_data.app_next_status   = SUNXI_USB_EFEX_APPS_EXIT;
					}
				}
			}
			printf("sunxi_efex_next_action=%d\n", sunxi_efex_next_action);
			trans_data.last_err          = 0;
			//before product finish, clear suspend flag
			efex_suspend_flag            = 0;

			break;

		case  FEX_CMD_fes_memset:
		    {
		        fes_efex_memset_t *fes_memset = (fes_efex_memset_t *)cmd_buf;

		        sunxi_usb_dbg("start 0x%x, value 0x%x, length 0x%x\n", fes_memset->start_addr, fes_memset->value & 0xff, fes_memset->length);
		        memset((void *)(ulong)fes_memset->start_addr, fes_memset->value & 0xff, fes_memset->length);
		    }
		    trans_data.last_err          = 0;
		    trans_data.app_next_status   = SUNXI_USB_EFEX_APPS_STATUS;

		    break;

		case FEX_CMD_fes_pmu:
			{
				fes_efex_pmu_t *fes_pmu = (fes_efex_pmu_t *)cmd_buf;

				trans_data.recv_size = fes_pmu->size;
				trans_data.type      = fes_pmu->type;

				memset(&pmu_config, 0, sizeof(struct pmu_config_t));

				trans_data.last_err        = 0;
				trans_data.app_next_status = SUNXI_USB_EFEX_APPS_RECEIVE_DATA;
			}

			break;

		case FEX_CMD_fes_unseqmem_read:
			{
				tag_efex_unseq_mem_t  *fes_unseq = (tag_efex_unseq_mem_t *)cmd_buf;

				trans_data.recv_size = fes_unseq->size;
				trans_data.type      = fes_unseq->type;

				if(global_unseq_mem_addr.unseq_mem == NULL)
				{
					printf("there is no memory to load unsequence data\n");
					trans_data.last_err        = -1;
					trans_data.act_send_buffer = (void*)(ulong)CONFIG_SYS_SDRAM_BASE;
				}
				else
				{
					int i;
					struct unseq_mem_config *unseq_mem = global_unseq_mem_addr.unseq_mem;

					for(i=0;i<global_unseq_mem_addr.count;i++)
					{
						unseq_mem[i].value = readl((const volatile void __iomem *)(ulong)unseq_mem[i].addr);
						sunxi_usb_dbg("read 0x%x, value 0x%x\n", unseq_mem[i].addr, unseq_mem[i].value);
					}
					trans_data.last_err        = 0;
					trans_data.act_send_buffer = (void*)global_unseq_mem_addr.unseq_mem;

				}
				trans_data.send_size       = global_unseq_mem_addr.count * sizeof(struct unseq_mem_config);
				trans_data.app_next_status = SUNXI_USB_EFEX_APPS_SEND_DATA;
			}

			break;
		case FEX_CMD_fes_unseqmem_write:
			{
				tag_efex_unseq_mem_t  *fes_unseq = (tag_efex_unseq_mem_t *)cmd_buf;

				trans_data.recv_size = fes_unseq->size;
				trans_data.type      = fes_unseq->type;

				if(global_unseq_mem_addr.unseq_mem != NULL)
				{
					free(global_unseq_mem_addr.unseq_mem);
				}
				global_unseq_mem_addr.unseq_mem = (struct unseq_mem_config *)malloc(fes_unseq->count * sizeof(struct unseq_mem_config));
				memset(global_unseq_mem_addr.unseq_mem, 0, fes_unseq->count * sizeof(struct unseq_mem_config));
				global_unseq_mem_addr.count = fes_unseq->count;

				trans_data.last_err        = 0;
				trans_data.app_next_status = SUNXI_USB_EFEX_APPS_RECEIVE_DATA;
			}
		    break;

		case FEX_CMD_fes_force_erase:
			printf("FEX_CMD_fes_force_erase\n");
			{
				trans_data.last_err = sunxi_flash_force_erase();
				printf("FEX_CMD_fes_force_erase last err=%d\n", trans_data.last_err);
			}
			trans_data.app_next_status   = SUNXI_USB_EFEX_APPS_STATUS;

			break;
        case FEX_CMD_fes_force_erase_key:
            printf("FEX_CMD_fes_force_erase_key \n");
            {
#ifdef CONFIG_SUNXI_UBIFS
		if ((get_boot_storage_type() == STORAGE_NAND) &&
					efex_ubi_init_flag == 0 &&
					nand_use_ubi()) {
			ubi_nand_probe_uboot();
			ubi_nand_attach_mtd();
			efex_ubi_init_flag = 1;
		}
#endif
		    trans_data.last_err = sunxi_sprite_force_erase_key();
		    printf("FEX_CMD_fes_force_erase_key last err = %d \n",
			   trans_data.last_err);
            }
            trans_data.app_next_status = SUNXI_USB_EFEX_APPS_STATUS ;
            break;

		case FEX_CMD_fes_query_secure:
			{
				uint *bootfile_mode = (uint *)trans_data.base_send_buffer;

				*bootfile_mode = gd->bootfile_mode;

				printf("bootfile_mode=%ld\n", gd->bootfile_mode);
				trans_data.act_send_buffer   = trans_data.base_send_buffer;
				trans_data.send_size         = 4;
				trans_data.last_err          = 0;
				trans_data.app_next_status   = SUNXI_USB_EFEX_APPS_SEND_DATA;
			}
			break;
#ifdef CONFIG_CMD_SUNXI_NAND
		case FEX_CMD_fes_nand:
			{
				if (get_boot_storage_type() == STORAGE_NAND) {
					fes_trans_t  *trans = (fes_trans_t *)cmd_buf;
					trans_data.flash_start       = trans->addr; //设置发送地址，属于扇区单位
					trans_data.flash_sectors     = (trans->len + 511) >> 9;

					trans_data.last_err          	 = 0;
					trans_data.type  = trans->type;
					//功能区分：读id,读boot0,uboot,读整个flash物理数据，写入boot0,uboot,读某个逻辑地址的历史数据，擦除功能，修改读写条件（调频，改单线双线四线操作）
					trans_data.send_size = trans->len;
					trans_data.app_next_status   	 = SUNXI_USB_EFEX_APPS_SEND_DATA;
					trans_data.act_send_buffer   = trans_data.base_send_buffer;	 //设置发送地址
					if ((trans->type & SUNXI_EFEX_NAND_ID_TAG) == SUNXI_EFEX_NAND_ID_TAG) {
						printf("read nand flash ID\n");
						if (sunxi_nand_info_dump((void *)trans_data.act_send_buffer)) {
							printf("read nand flash ID err\n");
							trans_data.last_err = -1;
						}

						printf("read nand flash id last err %d\n", trans_data.last_err);
					}
					if ((trans->type & SUNXI_EFEX_NAND_PAGE_TABLE_TAG) == SUNXI_EFEX_NAND_PAGE_TABLE_TAG) {
						printf("dump flash page table\n");
						sunxi_nand_page_table(trans_data.act_send_buffer);
						if (trans_data.act_send_buffer == NULL) {
							trans_data.last_err = -1;
							printf("dump flash page table error\n");
						}
					}
					if ((trans->type & SUNXI_EFEX_NAND_PHY_BLOCK) == SUNXI_EFEX_NAND_PHY_BLOCK) {
						sunxi_nand_phy_page(trans->addr, trans->len, trans_data.act_send_buffer);
					}
					if ((trans->type & SUNXI_EFEX_NAND_BOOT0_SIZE) == SUNXI_EFEX_NAND_BOOT0_SIZE) {
						uint *boot0_size = (uint *)trans_data.base_send_buffer;
						*boot0_size = sunxi_flash_get_boot0_size();
					}

					if ((trans->type & SUNXI_EFEX_NAND_BOOT1_SIZE) == SUNXI_EFEX_NAND_BOOT1_SIZE) {
						int *boot1_size = (int *)trans_data.base_send_buffer;
						*boot1_size = sunxi_flash_get_boot1_size();
					}
					if ((trans->type & SUNXI_EFEX_NAND_BOOT0) == SUNXI_EFEX_NAND_BOOT0) {

						sunxi_nand_boot0_dump(trans_data.act_send_buffer, trans->addr, 0);
					}

					if ((trans->type & SUNXI_EFEX_NAND_BOOT1) == SUNXI_EFEX_NAND_BOOT1) {

						sunxi_nand_boot1_dump_for_efex(trans_data.act_send_buffer, trans->len);
					}
					if ((trans->type & SUNXI_EFEX_NAND_HISTORY_DATA) == SUNXI_EFEX_NAND_HISTORY_DATA) {
						sunxi_nand_logic_history_data_dump(trans_data.flash_start,
								trans_data.flash_sectors,
								trans_data.act_send_buffer, 0);
					}
					if ((trans->type & SUNXI_EFEX_NAND_FORCE_ERASE) == SUNXI_EFEX_NAND_FORCE_ERASE) {
						NAND_Uboot_Force_Erase();
					}
					if ((trans->type & SUNXI_EFEX_NAND_IO_STRESS) == SUNXI_EFEX_NAND_IO_STRESS) {
						sunxi_nand_test_read_write_normal();
					}
					if ((trans->type & SUNXI_EFEX_NAND_IO_WPERF) == SUNXI_EFEX_NAND_IO_WPERF) {
						sunxi_nand_wperf_test((void *)trans_data.act_send_buffer);
					}
					if ((trans->type & SUNXI_EFEX_NAND_IO_RPERF) == SUNXI_EFEX_NAND_IO_RPERF) {
						sunxi_nand_rperf_test((void *)trans_data.act_send_buffer);
					}

				} else
					printf("not nand\n");
			}
			break;
#endif
	case FEX_CMD_fes_query_info:
		{

			if (cmd->tag == SUNXI_EFEX_LOG_BUFF_INFO_TAG) {
				fex_log_buf_info_t *log_info;
				log_info = (fex_log_buf_info_t *)trans_data.base_send_buffer;
#if CONFIG_IS_ENABLED(PRE_CONSOLE_BUFFER)
				log_info->buf_start = CONFIG_PRE_CON_BUF_ADDR;
				log_info->buf_size = min(ALIGN((int)(strlen((char *)CONFIG_PRE_CON_BUF_ADDR) + 1), 512), CONFIG_PRE_CON_BUF_SZ);
#else
				log_info->buf_start = 0;
				log_info->buf_size = 0;
#endif
				trans_data.act_send_buffer   = trans_data.base_send_buffer;
				trans_data.send_size         = 8;
				trans_data.last_err          = 0;
				trans_data.app_next_status   = SUNXI_USB_EFEX_APPS_SEND_DATA;
			}
		}
		break;

		default:
			printf("not supported command 0x%x now\n", cmd->app_cmd);

			trans_data.last_err        = -1;
			trans_data.app_next_status = SUNXI_USB_EFEX_APPS_STATUS;

			break;
	}
	sunxi_print_usb_efex_next_status(trans_data.app_next_status);
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
static void dram_data_recv_finish(uint data_type)
{
	if(data_type == SUNXI_EFEX_MBR_TAG)			//传输MBR已经完成
        {
		//检查MBR的正确性


                trans_data.last_err = sunxi_sprite_verify_mbr((void *)trans_data.base_recv_buffer);
		if(!trans_data.last_err )
		{
#ifdef  CONFIG_SUNXI_NAND
		    nand_get_mbr((char *)trans_data.base_recv_buffer, 16 * 1024);
#endif
		        //准备擦除
		    if(!sunxi_sprite_erase_flash((void *)trans_data.base_recv_buffer))
		    {       //烧录mbr
                        printf("SUNXI_EFEX_MBR_TAG\n");
                        printf("mbr size = 0x%x\n", trans_data.to_be_recved_size);
#ifndef CONFIG_DISABLE_SUNXI_PART_DOWNLOAD
                        trans_data.last_err = sunxi_sprite_download_mbr((void *)trans_data.base_recv_buffer, trans_data.to_be_recved_size);
#else
			printf("DISABLE SUNXI MBR : so do not need to write mbr to flash \n");
			trans_data.last_err = 0;
#endif
		    }
		    else
		    {
		        trans_data.last_err = -1;
		    }
	        }


	}
	else if(data_type == SUNXI_EFEX_BOOT1_TAG)	//传输BOOT1已经完成
	{
		printf("SUNXI_EFEX_BOOT1_TAG\n");
		printf("boot1 size = 0x%x, max size = 0x%x\n", trans_data.to_be_recved_size, SUNXI_EFEX_RECV_MEM_SIZE);
		if (trans_data.to_be_recved_size > SUNXI_EFEX_RECV_MEM_SIZE)
			trans_data.last_err = -1;
		else
			trans_data.last_err = sunxi_sprite_download_uboot((void *)trans_data.base_recv_buffer, get_boot_storage_type(), 0);
	}
	else if(data_type == SUNXI_EFEX_BOOT0_TAG)	//传输BOOT0已经完成
	{
		printf("SUNXI_EFEX_BOOT0_TAG\n");
		printf("boot0 size = 0x%x\n", trans_data.to_be_recved_size);
		trans_data.last_err = sunxi_sprite_download_boot0((void *)trans_data.base_recv_buffer, get_boot_storage_type());
	}
	else if(data_type == SUNXI_EFEX_ERASE_TAG)
	{
		uint erase_flag;
		int origin_erase_flag;

		int node = fdt_path_offset(working_fdt, "/soc/platform");

		printf("SUNXI_EFEX_ERASE_TAG\n");
		erase_flag = *(uint *)trans_data.base_recv_buffer;

		printf("erase_flag = 0x%x\n", erase_flag);

		if (fdt_getprop_u32(working_fdt, node, "eraseflag",
					(uint32_t *)&origin_erase_flag) < 0) {
			printf("get eraseflag fail\n");
			origin_erase_flag = 0;
		}
		printf("origin_erase_flag = 0x%x\n", origin_erase_flag);

		/* for special erase_flag like 0x11, no need to overlap. */
		if (origin_erase_flag == 0 || origin_erase_flag == 1) {
			if (fdt_setprop_u32(working_fdt, node, "eraseflag", erase_flag) < 0)
				printf("set eraseflag fail\n");
		}


	}
	else if(data_type == SUNXI_EFEX_PMU_SET)
	{
		memcpy(&pmu_config, (void *)trans_data.act_recv_buffer, trans_data.recv_size);

		trans_data.last_err = -1;
		/*axp_set_supply_status_byname(pmu_config.pmu_type, pmu_config.vol_name, pmu_config.voltage, pmu_config.gate);*/
	}
	else if(data_type == SUNXI_EFEX_UNSEQ_MEM_FOR_WRITE)
	{
		int i;
		struct unseq_mem_config *unseq_mem = global_unseq_mem_addr.unseq_mem;

		printf("begin to load data to unsequency memory\n");
		memcpy(unseq_mem, (void *)trans_data.act_recv_buffer, trans_data.recv_size);
		for(i=0;i<global_unseq_mem_addr.count;i++)
		{
			sunxi_usb_dbg("write 0x%x, value 0x%x\n", unseq_mem[i].addr, unseq_mem[i].value);
			writel(unseq_mem[i].value, (volatile void __iomem *)(ulong)unseq_mem[i].addr);
		}
	}
	else if(data_type == SUNXI_EFEX_UNSEQ_MEM_FOR_READ)
	{
		struct unseq_mem_config *unseq_mem = global_unseq_mem_addr.unseq_mem;

		printf("begin to set address to unsequency memory\n");
		memcpy(unseq_mem, (void *)trans_data.act_recv_buffer, trans_data.recv_size);
	}
#ifdef CONFIG_SUNXI_UBIFS
	else if (data_type == SUNXI_EFEX_EXT4_UBIFS_TAG) {
		printf("SUNXI_EFEX_EXT4_UBIFS_TAG & do nothing\n");
		trans_data.last_err = 0;
	}
#endif
    else//其它数据，直接写入内存
	{
        memcpy((void *)trans_data.dram_trans_buffer, (void *)trans_data.act_recv_buffer, trans_data.recv_size);

		sunxi_usb_dbg("SUNXI_EFEX_DRAM_TAG\n");

		trans_data.last_err = 0;
	}
	trans_data.to_be_recved_size = 0;
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

void sunxi_print_efex_status(int efex_status)
{
	static int temp_efex_status = -1;
	static int cnt;
	if (usb_debug_mode < 0)
		script_parser_fetch("/soc/platform", "usb_debug_mode",
				    &usb_debug_mode, 0);
	if (usb_debug_mode) {
		if (temp_efex_status != efex_status || cnt < 5) {
			if (efex_status == SUNXI_USB_EFEX_IDLE) {
				cnt++;
			} else {
				cnt = 0;
			}
			printf("\t sunxi_usb_efex_status: ");
			switch (efex_status) {
			case SUNXI_USB_EFEX_IDLE:
				printf("SUNXI_USB_EFEX_IDLE\t\n");
				break;
			case SUNXI_USB_EFEX_SETUP:
				printf("SUNXI_USB_EFEX_SETUP\t\n");
				break;
			case SUNXI_USB_EFEX_SEND_DATA:
				printf("SUNXI_USB_EFEX_SEND_DATA\t\n");
				break;
			case SUNXI_USB_EFEX_RECEIVE_DATA:
				printf("SUNXI_USB_EFEX_RECEIVE_DATA\t\n");
				break;
			case SUNXI_USB_EFEX_SETUP_NEW:
				printf("SUNXI_USB_EFEX_SETUP_NEW\t\n");
				break;
			case SUNXI_USB_EFEX_SEND_DATA_NEW:
				printf("SUNXI_USB_EFEX_SEND_DATA_NEW\t\n");
				break;
			case SUNXI_USB_EFEX_RECEIVE_DATA_NEW:
				printf("SUNXI_USB_EFEX_RECEIVE_DATA_NEW\t\n");
				break;
			case SUNXI_USB_EFEX_STATUS:
				printf("SUNXI_USB_EFEX_STATUS\t\n");
				break;
			case SUNXI_USB_EFEX_EXIT:
				printf("SUNXI_USB_EFEX_EXIT\t\n");
				break;
			default:
				printf(" %d unknown cmd:%d\t\n", __LINE__, efex_status);
				break;
			}
			temp_efex_status = efex_status;
		}
	}
}

void sunxi_print_efex_app_step(int efex_app_status)
{
	static int temp_efex_status = -1;
	if (usb_debug_mode < 0)
		script_parser_fetch("/soc/platform", "usb_debug_mode",
				    &usb_debug_mode, 0);
	if (usb_debug_mode) {
		if (temp_efex_status != efex_app_status) {
			printf("\t sunxi_usb_efex_app_step: ");
			switch (efex_app_status) {
			case SUNXI_USB_EFEX_APPS_IDLE:
				printf("SUNXI_USB_EFEX_APPS_IDLE \t\n");
				break;
			case SUNXI_USB_EFEX_APPS_CMD:
				printf("SUNXI_USB_EFEX_APPS_CMD \t\n");
				break;
			case SUNXI_USB_EFEX_APPS_SEND_DATA:
				printf("SUNXI_USB_EFEX_APPS_SEND_DATA \t\n");
				break;
			case SUNXI_USB_EFEX_APPS_RECEIVE_DATA:
				printf("SUNXI_USB_EFEX_APPS_RECEIVE_DATA \t\n");
				break;
			case SUNXI_USB_EFEX_APPS_STATUS:
				printf("SUNXI_USB_EFEX_APPS_STATUS \t\n");
				break;
			case SUNXI_USB_EFEX_APPS_EXIT:
				printf("SUNXI_USB_EFEX_APPS_EXIT \t\n");
				break;
			default:
				printf(" %d unknown cmd:%d\t\n", __LINE__, efex_app_status);
				break;
			}
			temp_efex_status = efex_app_status;
		}
	}
}


static int sunxi_efex_state_loop(void  *buffer)
{
	static struct sunxi_efex_cbw_t  *cbw;
	static struct sunxi_efex_csw_t   csw;
	sunxi_ubuf_t *sunxi_ubuf = (sunxi_ubuf_t *)buffer;
    int  efex_write_error_flag = 0;

	sunxi_print_efex_status(sunxi_usb_efex_status);
	sunxi_print_efex_app_step(sunxi_usb_efex_app_step);
	switch(sunxi_usb_efex_status)
	{
		case SUNXI_USB_EFEX_IDLE:
			if(sunxi_ubuf->rx_ready_for_data == 1)
			{
				sunxi_usb_efex_status = SUNXI_USB_EFEX_SETUP;
			}
			//when product finish and usb disconnect ,shutdown machine
			if( sunxi_efex_next_action == SUNXI_UPDATE_NEXT_ACTION_NORMAL ||
				sunxi_efex_next_action >  SUNXI_UPDATE_NEXT_ACTION_REUPDATE )
			{
				if(efex_suspend_flag)
				{
					return SUNXI_UPDATE_NEXT_ACTION_SHUTDOWN;
				}
			}

			break;

		case SUNXI_USB_EFEX_SETUP:		//cbw


            if((sunxi_ubuf->rx_req_length == sizeof(struct sunxi_efex_cbw_t)))
            {
                cbw = (struct sunxi_efex_cbw_t *)sunxi_ubuf->rx_req_buffer;
                if(CBW_MAGIC != cbw->magic)
                {
                    printf("sunxi usb error: the cbw signature 0x%x is bad, need 0x%x\n", cbw->magic, CBW_MAGIC);
                    sunxi_ubuf->rx_ready_for_data = 0;
                    sunxi_usb_efex_status = SUNXI_USB_EFEX_IDLE;
                    return -1;
                }
            }
            else if(sunxi_ubuf->rx_req_length == FES_NEW_CMD_LEN)
            {
                sunxi_usb_dbg("----------new cmd format--------\n");
                if(CBW_MAGIC != ((u32*)(sunxi_ubuf->rx_req_buffer))[4])
                {
                    printf("sunxi usb error: the cmd signature 0x%x is bad, need 0x%x\n",
                          ((u32*)(sunxi_ubuf->rx_req_buffer))[4],CBW_MAGIC);


                    sunxi_ubuf->rx_ready_for_data = 0;
                    sunxi_usb_efex_status = SUNXI_USB_EFEX_IDLE;
                    //data value err
                    return -1;
                }

                sunxi_usb_efex_status = SUNXI_USB_EFEX_SETUP_NEW;
                break;
            }
            else
            {
                printf("sunxi usb error: received bytes 0x%x is not equal cbw struct size 0x%zx or new cmd size 0x%x\n",
                    sunxi_ubuf->rx_req_length, sizeof(struct sunxi_efex_cbw_t),FES_NEW_CMD_LEN);
                sunxi_ubuf->rx_ready_for_data = 0;
                sunxi_usb_efex_status = SUNXI_USB_EFEX_IDLE;
				return -1;
            }

			csw.magic = CSW_MAGIC;		//"AWUS"
			csw.tag   = cbw->tag;

#if defined(SUNXI_USB_30)
			sunxi_usb_efex_status_enable = 1;
#endif

			sunxi_usb_dbg("usb cbw trans direction = 0x%x\n", cbw->cmd_package.direction);
			if(sunxi_usb_efex_app_step == SUNXI_USB_EFEX_APPS_IDLE)
			{
				sunxi_usb_dbg("APPS: SUNXI_USB_EFEX_APPS_IDLE\n");
				if(cbw->cmd_package.direction == TL_CMD_RECEIVE)	//小机端接收数据
				{
					sunxi_usb_dbg("APPS: SUNXI_USB_EFEX_APPS_IDLE: TL_CMD_RECEIVE\n");
					sunxi_ubuf->request_size = min(cbw->data_transfer_len, (u32)CBW_MAX_CMD_SIZE);
					sunxi_usb_dbg("try to receive data 0x%x\n", sunxi_ubuf->request_size);
					sunxi_usb_efex_write_enable = 0;
					if(sunxi_ubuf->request_size)
					{
						sunxi_udc_start_recv_by_dma(cmd_buf, sunxi_ubuf->request_size);	//start dma to receive data
					}
					else
					{
						printf("APPS: SUNXI_USB_EFEX_APPS_IDLE: the send data length is 0\n");

						return -1;
					}
					//下一阶段接收到的数据是app
					sunxi_usb_efex_status   = SUNXI_USB_EFEX_RECEIVE_DATA;	//传输阶段，下一阶段将接收数据
					sunxi_usb_efex_app_step = SUNXI_USB_EFEX_APPS_CMD;		//命令阶段，下一阶段接收的是命令
				}
				else	//setup阶段即usb的bulk传输第一阶段，只能接收数据，不能发送
				{
					printf("APPS: SUNXI_USB_EFEX_APPS_IDLE: INVALID direction\n");
					printf("sunxi usb efex app cmd err: usb transfer direction is receive only\n");

					return -1;
				}
			}
			else if((sunxi_usb_efex_app_step == SUNXI_USB_EFEX_APPS_SEND_DATA) ||			\
				(sunxi_usb_efex_app_step == SUNXI_USB_EFEX_APPS_RECEIVE_DATA))	//收到的第二阶段，此时开始解析命令
			{
				sunxi_usb_dbg("APPS: SUNXI_USB_EFEX_APPS_DATA\n");
				if(cbw->cmd_package.direction == TL_CMD_RECEIVE)	//如果要接收数据，则事先启动dma开始接收
				{
					sunxi_usb_dbg("APPS: SUNXI_USB_EFEX_APPS_DATA: TL_CMD_RECEIVE\n");
					sunxi_ubuf->request_size = MIN(cbw->data_transfer_len, trans_data.recv_size);	//接收长度
					//sunxi_usb_dbg("try to receive data 0x%x\n", sunxi_ubuf->request_size);
					sunxi_usb_efex_write_enable = 0;				//设置标志
					if(sunxi_ubuf->request_size)
					{
						sunxi_usb_dbg("dma recv addr = 0x%lx\n", (ulong)trans_data.act_recv_buffer);
						sunxi_udc_start_recv_by_dma(trans_data.act_recv_buffer, sunxi_ubuf->request_size);	//start dma to receive data
					}
					else
					{
						printf("APPS: SUNXI_USB_EFEX_APPS_DATA: the send data length is 0\n");

						return -1;
					}
				}
				//处理命令，返回命令阶段的下一个状态
				sunxi_usb_efex_app_step = trans_data.app_next_status;	//根据命令，获取下一命令阶段状态
				//sunxi_usb_dbg("APPS: SUNXI_USB_EFEX_APPS_CMD_DECODE finish\n");
				//sunxi_usb_dbg("sunxi_usb_efex_app_step = 0x%x\n", sunxi_usb_efex_app_step);
				sunxi_usb_efex_status   = sunxi_usb_efex_app_step & 0xffff;						//识别出传输阶段下一阶段状态
																								//可能是发送数据，接收数据，发送状态(csw)
			}
			else if(sunxi_usb_efex_app_step == SUNXI_USB_EFEX_APPS_STATUS)
			{
				sunxi_usb_dbg("APPS: SUNXI_USB_EFEX_APPS_STATUS\n");
				if(cbw->cmd_package.direction == TL_CMD_TRANSMIT)		//发送数据
				{
					sunxi_usb_dbg("APPS: SUNXI_USB_EFEX_APPS_STATUS: TL_CMD_TRANSMIT\n");
					__sunxi_usb_efex_fill_status();

					sunxi_usb_efex_status = SUNXI_USB_EFEX_SEND_DATA;
					sunxi_usb_efex_app_step = SUNXI_USB_EFEX_APPS_IDLE;
				}
				else	//最后一个阶段，只能发送数据，不能接收
				{
					printf("APPS: SUNXI_USB_EFEX_APPS_STATUS: INVALID direction\n");
					printf("sunxi usb efex app status err: usb transfer direction is transmit only\n");
				}
			}
			else if(sunxi_usb_efex_app_step == SUNXI_USB_EFEX_APPS_EXIT)
			{
				sunxi_usb_dbg("APPS: SUNXI_USB_EFEX_APPS_EXIT\n");
				__sunxi_usb_efex_fill_status();

				sunxi_usb_efex_status = SUNXI_USB_EFEX_SEND_DATA;
			}

			break;

	  	case SUNXI_USB_EFEX_SEND_DATA:

			{
				uint tx_length = MIN(cbw->data_transfer_len, trans_data.send_size);

#if defined(SUNXI_USB_30)
				sunxi_usb_efex_status_enable = 0;
#endif
				sunxi_usb_dbg("send data start 0x%lx, size 0x%x\n", (ulong)trans_data.act_send_buffer, tx_length);
				if(tx_length)
				{
					sunxi_udc_send_data((void *)trans_data.act_send_buffer, tx_length);
				}
				sunxi_usb_efex_status = SUNXI_USB_EFEX_STATUS;
				if(sunxi_usb_efex_app_step == SUNXI_USB_EFEX_APPS_SEND_DATA)//来自于命令阶段，要求发送数据，下一阶段只能是发送状态(status_t)
				{
					sunxi_usb_dbg("SUNXI_USB_EFEX_SEND_DATA next: SUNXI_USB_EFEX_APPS_STATUS\n");
					sunxi_usb_efex_app_step = SUNXI_USB_EFEX_APPS_STATUS;
				}
				else if(sunxi_usb_efex_app_step == SUNXI_USB_EFEX_APPS_EXIT)
				{
					sunxi_usb_dbg("SUNXI_USB_EFEX_SEND_DATA next: SUNXI_USB_EFEX_APPS_EXIT\n");
					sunxi_usb_efex_status = SUNXI_USB_EFEX_EXIT;
					//sunxi_usb_efex_app_step = SUNXI_USB_EFEX_APPS_EXIT;
				}
			}
	  		break;

	  	case SUNXI_USB_EFEX_RECEIVE_DATA:

			if(sunxi_usb_efex_write_enable == 1)		//数据部分接收完毕
			{
				csw.status = 0;
				//区分出是命令还是数据
				if(sunxi_usb_efex_app_step == SUNXI_USB_EFEX_APPS_CMD)
				{
					//拷贝到cmd_buf，下次处理也需要
					sunxi_usb_dbg("SUNXI_USB_RECEIVE_DATA: SUNXI_USB_EFEX_APPS_CMD\n");
					if(sunxi_ubuf->request_size != CBW_MAX_CMD_SIZE)		//错误的数据，则返回
					{
						printf("sunxi usb efex err: received cmd size 0x%x is not equal to CBW_MAX_CMD_SIZE 0x%x\n", sunxi_ubuf->request_size, CBW_MAX_CMD_SIZE);

						sunxi_usb_efex_status = SUNXI_USB_EFEX_IDLE;
						csw.status = -1;
					}
					else
					{
						__sunxi_usb_efex_op_cmd(cmd_buf);
						csw.status = trans_data.last_err;
					}
					//sunxi_usb_efex_app_step = SUNXI_USB_EFEX_APPS_DATA;	//命令阶段，命令接收完成，下一阶段处理数据
					sunxi_usb_efex_app_step = trans_data.app_next_status;
				}
				else if(sunxi_usb_efex_app_step == SUNXI_USB_EFEX_APPS_RECEIVE_DATA)//来自于命令阶段，要求接收数据，下一阶段只能是发送状态(status_t)
				{
					//表示当次数据已经接收完成
					uint data_type = trans_data.type & SUNXI_EFEX_DATA_TYPE_MASK;

					sunxi_usb_dbg("SUNXI_USB_RECEIVE_DATA: SUNXI_USB_EFEX_APPS_RECEIVE_DATA\n");
					sunxi_usb_efex_app_step = SUNXI_USB_EFEX_APPS_STATUS;
					if(trans_data.type & SUNXI_EFEX_DRAM_MASK)		//表示属于内存数据，需要事先保存到内存中
					{
						sunxi_usb_dbg("SUNXI_EFEX_DRAM_MASK\n");
						if(trans_data.type & SUNXI_EFEX_TRANS_FINISH_TAG)	//表示当前类型数据已经接收完成
						{
				dram_data_recv_finish(data_type);
                        }
						//数据还没有接收完毕，等待继续接收
					}
					else		//表示当前数据需要写入flash
					{
						sunxi_usb_dbg("SUNXI_EFEX_FLASH_MASK\n");
#ifdef CONFIG_DISABLE_SUNXI_PART_DOWNLOAD
						//printf("DISABLE SUNXI MBR : so start of wirte would offset to -16K \n");
						trans_data.flash_start -= SUNXI_MBR_SIZE / 512 ;
#endif
						if(!sunxi_flash_write(trans_data.flash_start, trans_data.flash_sectors, (void *)trans_data.act_recv_buffer))
						{
							printf("sunxi usb efex err: write flash from 0x%x, sectors 0x%x failed\n", trans_data.flash_start, trans_data.flash_sectors);
							csw.status = -1;
							trans_data.last_err = -1;

							sunxi_usb_efex_app_step = SUNXI_USB_EFEX_APPS_IDLE;
						}

					}
				}
				sunxi_usb_efex_status   = SUNXI_USB_EFEX_STATUS;			//传输阶段，下一阶段传输状态(csw)
			}

			break;

            case SUNXI_USB_EFEX_SETUP_NEW:      //
            {

                memcpy(cmd_buf,sunxi_ubuf->rx_req_buffer,FES_NEW_CMD_LEN);
#ifdef _EFEX_USE_BUF_QUEUE_
                //flush queue buff   when verify cmd or flash set off cmd coming
                if(FEX_CMD_fes_verify_value == ((struct global_cmd_s *)cmd_buf)->app_cmd
                   ||  FEX_CMD_fes_flash_set_off==  ((struct global_cmd_s *)cmd_buf)->app_cmd )
                {
                    if(efex_queue_write_all_page())
                    {
                        printf("efex queue error: buf_queue_write_all_page fail\n");
                        efex_write_error_flag = 1;
                    }
                }
#endif
                __sunxi_usb_efex_op_cmd(cmd_buf);
                csw.status = trans_data.last_err;
                csw.magic = CSW_MAGIC;      //"AWUS"
                csw.tag   = 0;

                if(csw.status != 0 )
                {
                    printf("sunxi usb cmd error: 0x%x\n",csw.status);
                    sunxi_ubuf->rx_ready_for_data = 0;
                    sunxi_usb_efex_status = SUNXI_USB_EFEX_IDLE;
                    return -1;
                }

#if defined(SUNXI_USB_30)
                sunxi_usb_efex_status_enable = 1;
#endif
                if(trans_data.app_next_status == SUNXI_USB_EFEX_APPS_SEND_DATA)
                {
                    sunxi_usb_efex_status = SUNXI_USB_EFEX_SEND_DATA_NEW;
                }
                else if(trans_data.app_next_status == SUNXI_USB_EFEX_APPS_RECEIVE_DATA)
                {
                    sunxi_usb_efex_status = SUNXI_USB_EFEX_RECEIVE_DATA_NEW;
					dma_recv_time_out = runtime_tick();
                    sunxi_ubuf->request_size =  trans_data.recv_size;   //接收长度
                    sunxi_usb_efex_write_enable = 0;                //设置标志
                    if(sunxi_ubuf->request_size)
                    {
                        sunxi_usb_dbg("dma recv addr = 0x%lx, size =0x%x\n", (ulong)trans_data.act_recv_buffer,sunxi_ubuf->request_size);
                        sunxi_udc_start_recv_by_dma(trans_data.act_recv_buffer, sunxi_ubuf->request_size);  //start dma to receive data
                    }
                    else
                    {
                        printf("sunxi usb trans error: the request data length is 0\n");

                        return -1;
                    }

                }
                else if(trans_data.app_next_status == SUNXI_USB_EFEX_APPS_STATUS)
                {
                    sunxi_usb_efex_status = SUNXI_USB_EFEX_STATUS;
                }
                else if(trans_data.app_next_status == SUNXI_USB_EFEX_APPS_EXIT)
                {
                    sunxi_usb_efex_status = SUNXI_USB_EFEX_EXIT;
                }
                else
                {
                    printf("sunxi usb next status set error:0x%x\n", trans_data.app_next_status);
                    sunxi_ubuf->rx_ready_for_data = 0;
                    sunxi_usb_efex_status = SUNXI_USB_EFEX_IDLE;

                    return -1;
                }
                break;
			}

            case SUNXI_USB_EFEX_SEND_DATA_NEW:
                {
                    uint tx_length =  trans_data.send_size;
#if defined(SUNXI_USB_30)
                    sunxi_usb_efex_status_enable = 0;
#endif
                    sunxi_usb_dbg("dma send data start 0x%lx, size 0x%x\n", (ulong)trans_data.act_send_buffer, tx_length);
                    if(tx_length)
                    {
                        sunxi_udc_send_data((void *)trans_data.act_send_buffer, tx_length);
                    }
                    sunxi_usb_efex_status = SUNXI_USB_EFEX_STATUS;
                }
                break;

            case SUNXI_USB_EFEX_RECEIVE_DATA_NEW:
            {
                sunxi_usb_dbg("wait dma recv finish...\n");
                //wait for dma recv finish
				if (runtime_tick() - dma_recv_time_out > 30000) {
					tick_printf("err:dma recv time out\n");
					sunxi_board_run_fel();
				}
                if(!sunxi_usb_efex_write_enable)
                {
#ifdef _EFEX_USE_BUF_QUEUE_
                    if(efex_queue_write_one_page())
                    {
                        printf("sunxi efex queue: buf_queue_write_one_page() err\n");
                        efex_write_error_flag = 1;
                    }
#endif
                    break;
                }
                sunxi_usb_dbg("SUNXI_USB_RECEIVE_DATA_NEW\n");

                //表示当次数据已经接收完成
                uint data_type = trans_data.type & SUNXI_EFEX_DATA_TYPE_MASK;
                if(trans_data.type & SUNXI_EFEX_DRAM_MASK)      //表示属于内存数据，需要事先保存到内存中
                {
                    sunxi_usb_dbg("SUNXI_EFEX_DRAM_MASK\n");
                    if(trans_data.type & SUNXI_EFEX_TRANS_FINISH_TAG)   //表示当前类型数据已经接收完成
                    {
                        dram_data_recv_finish(data_type);
                    }
                    //数据还没有接收完毕，等待继续接收
                }
                else        //表示当前数据需要写入flash
                {
                    sunxi_usb_dbg("SUNXI_EFEX_FLASH_MASK\n");
#ifdef _EFEX_USE_BUF_QUEUE_
                    if(0 != efex_save_buff_to_queue(trans_data.flash_start,trans_data.flash_sectors,(void *)trans_data.act_recv_buffer))
                    {
                        printf("efex queue not enough space...\n");
                        trans_data.last_err = -1;
                    }

#else
#ifdef CONFIG_DISABLE_SUNXI_PART_DOWNLOAD
		    //printf("DISABLE SUNXI MBR : so start of wirte would offset to -16K \n");
		    trans_data.flash_start -= SUNXI_MBR_SIZE / 512 ;
#endif
                    if(!sunxi_flash_write(trans_data.flash_start, trans_data.flash_sectors, (void *)trans_data.act_recv_buffer))
                    {
                        printf("sunxi usb efex err: write flash from 0x%x, sectors 0x%x failed\n", trans_data.flash_start, trans_data.flash_sectors);
                        trans_data.last_err = -1;
                    }


#endif
                }
                csw.status = trans_data.last_err;
                sunxi_usb_efex_status   = SUNXI_USB_EFEX_STATUS;
            }
            break;

		case SUNXI_USB_EFEX_STATUS:
#if defined(SUNXI_USB_30)
			if(sunxi_usb_efex_status_enable)
#endif
			{
				sunxi_usb_efex_status = SUNXI_USB_EFEX_IDLE;

				sunxi_ubuf->rx_ready_for_data = 0;
                                //when call efex queue write error,set stauts to tell usbtools
                                if(efex_write_error_flag)
                                {
                                        csw.status = -1;
                                }
				__sunxi_efex_send_status(&csw, sizeof(struct sunxi_efex_csw_t));
			}

			break;

		case SUNXI_USB_EFEX_EXIT:

                        //when call efex queue write error,set stauts to tell usbtools
                        if(efex_write_error_flag)
                        {
                                csw.status = -1;
                        }
#if defined(SUNXI_USB_30)
			if(sunxi_usb_efex_status_enable == 1)
			{
				sunxi_ubuf->rx_ready_for_data = 0;

				__sunxi_efex_send_status(&csw, sizeof(struct sunxi_efex_csw_t));
			}
			else if(sunxi_usb_efex_status_enable >= 2)
			{
				return sunxi_efex_next_action;
			}
#else
			sunxi_ubuf->rx_ready_for_data = 0;

			__sunxi_efex_send_status(&csw, sizeof(struct sunxi_efex_csw_t));

			return sunxi_efex_next_action;
#endif
	  	default:
	  		break;
	}

	return 0;
}


sunxi_usb_module_init(SUNXI_USB_DEVICE_EFEX,					\
					  sunxi_efex_init,							\
					  sunxi_efex_exit,							\
					  sunxi_efex_reset,							\
					  sunxi_efex_standard_req_op,				\
					  sunxi_efex_nonstandard_req_op,			\
					  sunxi_efex_state_loop,					\
					  sunxi_efex_usb_rx_dma_isr,				\
					  sunxi_efex_usb_tx_dma_isr					\
					  );
