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
#include <asm/arch/dma.h>
#include <sys_partition.h>
#include <fastboot.h>
#include "usb_fastboot.h"
#include <android_misc.h>
#include <sunxi_board.h>
#include <power/sunxi/pmu.h>
#include <sunxi_mbr.h>
#include <sunxi_flash.h>
#include <private_uboot.h>
#include "../sprite/sparse/sparse.h"
#include "../sprite/sprite_download.h"
#include <private_toc.h>
#include <securestorage.h>

DECLARE_GLOBAL_DATA_PTR;

int do_go(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);

static  int sunxi_usb_fastboot_write_enable = 0;
static  int sunxi_usb_fastboot_status = SUNXI_USB_FASTBOOT_IDLE;

static  fastboot_trans_set_t  trans_data;

static  uint  all_download_bytes;

int     fastboot_data_flag;

extern int sunxi_usb_exit(void);

int get_fastboot_data_flag(void)
{
	return fastboot_data_flag;
}

int __attribute__((weak)) sunxi_secure_storage_init(void)
{
	return 0;
}

int __attribute__((weak)) sunxi_secure_storage_exit(void)
{
	return 0;
}

int __attribute__((weak)) sunxi_secure_object_read(const char *item_name, char *buffer, int buffer_len, int *data_len)
{
	return 0;
}

int __attribute__((weak)) sunxi_secure_object_write(const char *item_name, char *buffer, int length)
{
	return 0;
}

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
extern int get_serial_num_from_file(char *serial);
extern int get_serial_num_from_chipid(char *serial);
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
			dev_dscrptr->bDeviceClass       = 0xff;		//设备类：大容量存储
			dev_dscrptr->bDeviceSubClass    = 0xff;
			dev_dscrptr->bDeviceProtocol    = 0xff;
			dev_dscrptr->bMaxPacketSize0    = 0x40;
			dev_dscrptr->idVendor           = DEVICE_VENDOR_ID;
			dev_dscrptr->idProduct          = DEVICE_PRODUCT_ID;
			dev_dscrptr->bcdDevice          = DEVICE_BCD;
			dev_dscrptr->iManufacturer      = SUNXI_FASTBOOT_DEVICE_STRING_MANUFACTURER_INDEX;
			dev_dscrptr->iProduct           = SUNXI_FASTBOOT_DEVICE_STRING_PRODUCT_INDEX;
			dev_dscrptr->iSerialNumber      = SUNXI_FASTBOOT_DEVICE_STRING_SERIAL_NUMBER_INDEX;
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
			config_dscrptr->iConfiguration     	= SUNXI_FASTBOOT_DEVICE_STRING_CONFIG_INDEX;
			config_dscrptr->bmAttributes       	= 0xC0;
			config_dscrptr->bMaxPower          	= 0xFA;		//最大电流500ms(0xfa * 2)

			bytes_remaining 				   -= config_dscrptr->bLength;
			/* interface */
			inter_dscrptr->bLength             = MIN (bytes_remaining, sizeof(struct usb_interface_descriptor));
			inter_dscrptr->bDescriptorType     = USB_DT_INTERFACE;
			inter_dscrptr->bInterfaceNumber    = 0x00;
			inter_dscrptr->bAlternateSetting   = 0x00;
			inter_dscrptr->bNumEndpoints       = 0x02;
			inter_dscrptr->bInterfaceClass     = 0xff;		//fastboot storage
			inter_dscrptr->bInterfaceSubClass  = FASTBOOT_INTERFACE_SUB_CLASS;
			inter_dscrptr->bInterfaceProtocol  = FASTBOOT_INTERFACE_PROTOCOL;
			inter_dscrptr->iInterface          = SUNXI_FASTBOOT_DEVICE_STRING_INTERFACE_INDEX;

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

				sunxi_udc_send_setup(bLength, (void *)sunxi_usb_fastboot_dev[0]);
			}
			else if(string_index < SUNXI_USB_FASTBOOT_DEV_MAX)
			{
				/* Size of string in chars */
				unsigned char i = 0;
				unsigned char str_length = strlen ((const char *)sunxi_usb_fastboot_dev[string_index]);
				unsigned char bLength = 2 + (2 * str_length);

				if (string_index == 2) {
					char buffer[4096];
					int ret, data_len;
					char *name = "sn";
					char serial_num[128];
					memset(buffer, 0, 4096);
					ret = sunxi_secure_storage_init();
					if (ret < 0) {
						printf("%s secure storage init err\n", __func__);
						goto get_snum_fail;
					}
					ret = sunxi_secure_object_read(name, buffer, 4096, &data_len);
					if (ret < 0) {

						printf("private data %s is not exist\n", name);
						goto get_snum_fail;
					}
					strcpy(sunxi_usb_fastboot_dev[string_index] , buffer);
					goto get_snum_success;
get_snum_fail:
					memset(serial_num, 0, 128);
					if (get_serial_num_from_file(serial_num))
						get_serial_num_from_chipid(serial_num);
						strcpy(sunxi_usb_fastboot_dev[string_index], serial_num);
				}
get_snum_success:
				debug("string_index=%d sunxi_usb_fastboot_dev[%d]=%s\n" , string_index , string_index , \
						sunxi_usb_fastboot_dev[string_index]);
				str_length = strlen(sunxi_usb_fastboot_dev[string_index]);
				bLength = 2 + (2 * str_length);
				buffer[0] = bLength;        /* length */
				buffer[1] = USB_DT_STRING;  /* descriptor = string */

				/* Copy device string to fifo, expand to simple unicode */
				for(i = 0; i < str_length; i++)
				{
					buffer[2+ 2*i + 0] = sunxi_usb_fastboot_dev[string_index][i];
					buffer[2+ 2*i + 1] = 0;
				}
				bLength = MIN(bLength, req->wLength);

				sunxi_udc_send_setup(bLength, buffer);
			}
			else
			{
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
			uint32_t temp = sizeof(struct usb_qualifier_descriptor);
			qua_dscrpt->bLength = MIN(req->wLength, sizeof(temp));
			qua_dscrpt->bDescriptorType    = USB_DT_DEVICE_QUALIFIER;
			qua_dscrpt->bcdUSB             = 0x200;
			qua_dscrpt->bDeviceClass       = 0xff;
			qua_dscrpt->bDeviceSubClass    = 0xff;
			qua_dscrpt->bDeviceProtocol    = 0xff;
			qua_dscrpt->bMaxPacketSize0    = 0x40;
			qua_dscrpt->bNumConfigurations = 1;
			//qua_dscrpt->bRESERVED          = 0;
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
*                     __usb_get_status
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
static int __sunxi_fastboot_send_status(void *buffer, unsigned int buffer_size)
{
	return sunxi_udc_send_data((uchar *)buffer, buffer_size);
}
/*
*******************************************************************************
*                     __fastboot_reboot
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
static int __fastboot_reboot(int word_mode)
{
	char response[8];

	sprintf(response,"OKAY");
	__sunxi_fastboot_send_status(response, strlen(response));
	__msdelay(1000); /* 1 sec */

	sunxi_board_restart(word_mode);

	return 0;
}
/*
*******************************************************************************
*                     __erase_part
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
static int __erase_part(char *name)
{
	void *addr  = (void *)FASTBOOT_ERASE_BUFFER;
	u32   start, unerased_sectors;
	u32   nblock = FASTBOOT_ERASE_BUFFER_SIZE/512;
	char  response[68];

	if (memcmp(name, "userdata", strlen("userdata")) == 0)
		strcpy(name, "UDISK");

	start            = sunxi_partition_get_offset_byname(name);
	unerased_sectors = sunxi_partition_get_size_byname(name);

	printf("********* starting to erase %s partition, this may take some time, please wait for minutes ***********\n", name);
	if (memcmp(name, "UDISK", strlen("UDISK")) == 0) {
		if (unerased_sectors >= 16 * 1024 * 1024 / 512)
			unerased_sectors = 16 * 1024 * 1024 / 512;
	} else if (memcmp(name, "cache", strlen("cache")) == 0) {
		if (unerased_sectors >= 16 * 1024 * 1024 / 512)
			unerased_sectors = 16 * 1024 * 1024 / 512;
	}

	if((!start) || (!unerased_sectors))
	{
		printf("sunxi fastboot erase FAIL: partition %s does not exist\n", name);
		sprintf(response, "FAILerase: partition %s does not exist", name);

		__sunxi_fastboot_send_status(response, strlen(response));

		return -1;
	}

	memset(addr, 0xff, FASTBOOT_ERASE_BUFFER_SIZE);
	while(unerased_sectors >= nblock)
	{
		if(!sunxi_flash_write(start, nblock, addr))
		{
			printf("sunxi fastboot erase FAIL: failed to erase partition %s \n", name);
			sprintf(response,"FAILerase: failed to erase partition %s", name);

			__sunxi_fastboot_send_status(response, strlen(response));

			return -1;
		}
		start += nblock;
		unerased_sectors -= nblock;
	}
	if(unerased_sectors)
	{
		if(!sunxi_flash_write(start, unerased_sectors, addr))
		{
			printf("sunxi fastboot erase FAIL: failed to erase partition %s \n", name);
			sprintf(response,"FAILerase: failed to erase partition %s", name);

			__sunxi_fastboot_send_status(response, strlen(response));

			return -1;
		}
	}

	printf("sunxi fastboot: partition '%s' erased\n", name);
	sprintf(response, "OKAY");

	__sunxi_fastboot_send_status(response, strlen(response));

	return 0;
}

/*
*******************************************************************************
*                     erase_userdata
*
* Description:
*    for fastboot oem unlock or lock to erase UDISK partition
*
* Parameters:
*    void
*
* Return value:
*    int
*
* note:
*    void
*
*******************************************************************************
*/
static int erase_userdata(void)
{
	void *addr  = (void *)FASTBOOT_ERASE_BUFFER;
	u32   start, unerased_sectors;
	u32   nblock = FASTBOOT_ERASE_BUFFER_SIZE/512;

	start            = sunxi_partition_get_offset_byname("UDISK");
	unerased_sectors = sunxi_partition_get_size_byname("UDISK");

	printf("***start to erase UDISK partition, this may take some time***\n");
	if (unerased_sectors >= 16 * 1024 * 1024 / 512)
		unerased_sectors = 16 * 1024 * 1024 / 512;

	if ((!start) || (!unerased_sectors)) {
		printf("sunxi fastboot erase FAIL: partition UDISK does not exist\n");
		return -1;
	}

	memset(addr, 0xff, FASTBOOT_ERASE_BUFFER_SIZE);
	while (unerased_sectors >= nblock) {
		if (!sunxi_flash_write(start, nblock, addr)) {
			printf("sunxi fastboot erase FAIL: failed to erase partition UDISK \n");
			return -1;
		}
		start += nblock;
		unerased_sectors -= nblock;
	}
	if (unerased_sectors) {
		if (!sunxi_flash_write(start, unerased_sectors, addr)) {
			printf("sunxi fastboot erase FAIL: failed to erase partition UDISK \n");
			return -1;
		}
	}

	printf("sunxi fastboot: partition 'UDISK' erased\n");
	return 0;
}

/*
*******************************************************************************
*                     __flash_to_part
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
static void __flash_to_uboot(void)
{
	sbrom_toc1_head_info_t *temp_buf = (sbrom_toc1_head_info_t *)trans_data.base_recv_buffer;
	char  response[68];
	int ret = -1;

	if(strcmp((char*)temp_buf->name,"sunxi-package"))
	{
		printf("sunxi fastboot error: there is not sunxi-package file\n");
		sprintf(response, "FAILdownload:there is not boot package file \n");
		__sunxi_fastboot_send_status(response, strlen(response));
		return ;
	}

	printf("ready to download bytes 0x%x\n", trans_data.try_to_recv);
	ret = sunxi_sprite_download_uboot((char *)temp_buf,get_boot_storage_type() ,1);
	if (!ret)
	{
		printf("sunxi fastboot: successed in downloading uboot package\n");
		sprintf(response, "OKAY");
	}
	else
	{
		printf("sunxi fastboot: fail in downloading uboot package\n");
		sprintf(response, "FAIL");
	}
	__sunxi_fastboot_send_status(response, strlen(response));
	return ;
}

static int __flash_to_part(char *name)
{
	char *addr = trans_data.base_recv_buffer;
	u32   start, data_sectors;
	u32   part_sectors;
	u32   nblock = FASTBOOT_TRANSFER_BUFFER_SIZE/512;
	char  response[68];

	start        = sunxi_partition_get_offset_byname(name);
	part_sectors = sunxi_partition_get_size_byname(name);

	if((!start) || (!part_sectors))
	{
		uint  addr_in_hex;
		int   ret;

		printf("sunxi fastboot download FAIL: partition %s does not exist\n", name);
		printf("probe it as a dram address\n");

		ret = strict_strtoul((const char *)name, 16, (long unsigned int*)&addr_in_hex);
		if(ret)
		{
			printf("sunxi fatboot download FAIL: it is not a dram address\n");

			sprintf(response, "FAILdownload: partition %s does not exist", name);
			__sunxi_fastboot_send_status(response, strlen(response));

			return -1;
		}
		else
		{
			printf("ready to move data to 0x%x, bytes 0x%x\n", addr_in_hex, trans_data.try_to_recv);
			memcpy((void *)(ulong)addr_in_hex, addr, trans_data.try_to_recv);
		}
	}
	else
	{
		int  format;

		printf("ready to download bytes 0x%x\n", trans_data.try_to_recv);
		format = unsparse_probe(addr, trans_data.try_to_recv, start);

		if(ANDROID_FORMAT_DETECT == format)
		{
			if(unsparse_direct_write(addr, trans_data.try_to_recv))
			{
				printf("sunxi fastboot download FAIL: failed to write partition %s \n", name);
				sprintf(response,"FAILdownload: write partition %s err", name);

				return -1;
			}
		}
		else
		{
		    data_sectors = (trans_data.try_to_recv + 511)/512;
		    if(data_sectors > part_sectors)
		    {
		    	printf("sunxi fastboot download FAIL: partition %s size 0x%x is smaller than data size 0x%x\n", name, trans_data.act_recv, data_sectors * 512);
				sprintf(response, "FAILdownload: partition size < data size");

				__sunxi_fastboot_send_status(response, strlen(response));

				return -1;
		    }
			while(data_sectors >= nblock)
			{
				if(!sunxi_flash_write(start, nblock, addr))
				{
					printf("sunxi fastboot download FAIL: failed to write partition %s \n", name);
					sprintf(response,"FAILdownload: write partition %s err", name);

					__sunxi_fastboot_send_status(response, strlen(response));

					return -1;
				}
				start += nblock;
				data_sectors -= nblock;
				addr  += FASTBOOT_TRANSFER_BUFFER_SIZE;
			}
			if(data_sectors)
			{
				if(!sunxi_flash_write(start, data_sectors, addr))
				{
					printf("sunxi fastboot download FAIL: failed to write partition %s \n", name);
					sprintf(response,"FAILdownload: write partition %s err", name);

					__sunxi_fastboot_send_status(response, strlen(response));

					return -1;
				}
			}
		}
	}

	printf("sunxi fastboot: successed in downloading partition '%s'\n", name);
	sprintf(response, "OKAY");

	__sunxi_fastboot_send_status(response, strlen(response));

	return 0;
}


/*
*******************************************************************************
*                     __try_to_download
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
static int __try_to_download(char *download_size, char *response)
{
	int  ret = -1;

	trans_data.try_to_recv = simple_strtoul (download_size, NULL, 16);
	all_download_bytes = trans_data.try_to_recv;
	printf("Starting download of %d BYTES\n", trans_data.try_to_recv);
	printf("Starting download of %d MB\n", trans_data.try_to_recv >> 20);

	if (0 == trans_data.try_to_recv)
	{
		/* bad user input */
		sprintf(response, "FAILdownload: data size is 0");
	}
	else if (trans_data.try_to_recv > SUNXI_USB_FASTBOOT_BUFFER_MAX)
	{
		sprintf(response, "FAILdownload: data > buffer");
	}
	else
	{
		/* The default case, the transfer fits
		   completely in the interface buffer */
		sprintf(response, "DATA%08x", trans_data.try_to_recv);
		printf("download response: %s\n", response);

		ret = 0;
	}

	return ret;
}
/*
*******************************************************************************
*                     __boot
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
static void __boot(void)
{
	char  response[68];

	if(all_download_bytes > CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE)
	{
		char start[32];
		char *bootm[3] = { "bootm", NULL, NULL, };
		char *go[3]    = { "go",    NULL, NULL, };

		struct fastboot_boot_img_hdr *fb_hdr =
			(struct fastboot_boot_img_hdr *) trans_data.base_recv_buffer;

		/* Skip the mkbootimage header */
		image_header_t *hdr =
			(image_header_t *)
			&trans_data.base_recv_buffer[CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE];

		bootm[1] = go[1] = start;
		sprintf(start, "0x%lx", (ulong)hdr);

		printf("start addr %s\n", start);
		/* Execution should jump to kernel so send the response
		   now and wait a bit.  */
		sprintf(response, "OKAY");
//		fastboot_tx_status(response, strlen(response));
		__msdelay (1000); /* 1 sec */

		if (ntohl(hdr->ih_magic) == IH_MAGIC) {
			/* Looks like a kernel.. */
			printf ("Booting kernel..\n");

			/*
			 * Check if the user sent a bootargs down.
			 * If not, do not override what is already there
			 */
			if (strlen ((char *) &fb_hdr->cmdline[0])) {
				printf("Image has cmdline:");
				printf("%s\n", &fb_hdr->cmdline[0]);
				setenv ("bootargs", (char *) &fb_hdr->cmdline[0]);
			}
			do_bootm (NULL, 0, 2, bootm);
		} else {
			/* Raw image, maybe another uboot */
			printf ("Booting raw image..\n");

			do_go (NULL, 0, 2, go);
		}
		printf ("ERROR : bootting failed\n");
		printf ("You should reset the board\n");
	}
	else
	{
		sprintf(response, "FAILinvalid boot image");
	}
}
/*
*******************************************************************************
*                     __get_var
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
static void __get_var(char *ver_name)
{
	char response[68];

	memset(response, 0, 68);
	strcpy(response,"OKAY");

	if(!strcmp(ver_name, "version"))
	{
		strcpy(response + 4, FASTBOOT_VERSION);
	}
	else if(!strcmp(ver_name, "product"))
	{
		strcpy(response + 4, SUNXI_FASTBOOT_DEVICE_PRODUCT);
	}
	else if(!strcmp(ver_name, "serialno"))
	{
		strcpy(response + 4, SUNXI_FASTBOOT_DEVICE_SERIAL_NUMBER);
	}
	else if(!strcmp(ver_name, "downloadsize"))
	{
		sprintf(response + 4, "0x%08x", SUNXI_USB_FASTBOOT_BUFFER_MAX);
		printf("response: %s\n", response);
	}
	else if(!strcmp(ver_name, "secure"))
	{
		strcpy(response + 4, "yes");
	}
	else if(!strcmp(ver_name, "max-download-size"))
	{
		sprintf(response + 4, "0x%08x", SUNXI_USB_FASTBOOT_BUFFER_MAX);
		printf("response: %s\n", response);
	}
	else
	{
		strcpy(response + 4, "not supported");
	}

	__sunxi_fastboot_send_status(response, strlen(response));

	return ;
}

/*
*****************************************************************************
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
*****************************************************************************
*/
int sunxi_oem_op_lock(int request_flag, char *info)
{
	char local_info[64];
	char *info_p;
	int ret;

	if (info == NULL) {
		memset(local_info, 0, 64);
		info_p = local_info;
	} else {
		info_p = info;
	}

	switch (request_flag) {
	case SUNXI_LOCKED:
		if (gd->lockflag == SUNXI_LOCKED) {
			strcpy(info_p, "system is already locked");
			ret = -1;
		} else if (!erase_userdata()) {
			gd->lockflag = SUNXI_LOCKED;
			strcpy(info_p, "Lock device successfully!");
			ret = 0;
		} else {
			strcpy(info_p, "the lock flag is invalid");
			ret = -2;
		}

		break;
	case SUNXI_UNLOCKED:
		if (gd->lockflag == SUNXI_UNLOCKED) {
			strcpy(info_p, "system is already unlocked");
			ret = -3;
		} else if (!erase_userdata()) {
			gd->lockflag = SUNXI_UNLOCKED;
			strcpy(info_p, "Unlock device successfully!");
			ret = 0;
		} else {
			strcpy(info_p, "the unlock flag is invalid");
			ret = -4;
		}

		break;

	default:
		strcpy(info_p, "the requst is invalid");
		ret = -5;
	}

	return ret;
}

/*
*****************************************************************************
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
*****************************************************************************
*/
static int sunxi_fastboot_write_status_flag(char *name)
{
	int  ret;

	/* write locked or unlocked flag to secure storage */
	ret = sunxi_secure_storage_init();
	if (ret < 0) {
		printf("secure storage init err\n");
		return -1;
	}
	if (sunxi_secure_object_write("fastboot_status_flag", name, strlen(name))) {
		printf("save  %s flag to secure storage failed\n", name);
		return -1;
	}
	/* save device unlock flag for boot_verify to judge if the device ever unlock */
	if (memcmp(name, "unlocked", strlen("unlocked")) == 0) {
		if (sunxi_secure_object_write("device_unlock", "unlock", strlen("unlock"))) {
			printf("save device_unlock flag to secure storage failed\n");
			return -1;
		};
	}
	printf("save  %s flag to secure storage success\n", name);
	sunxi_secure_storage_exit();

	return 0;
}

static void __limited_fastboot_unlock(void)
{
	char response[64];

	memset(response, 0, 64);
	tick_printf("GMS,fastboot unlock limited used!!!\n");

	strcpy(response, "FAILability is 0. Permission denied for this command!");

	__sunxi_fastboot_send_status(response, strlen(response));

	return;
}

static char sunxi_read_oem_unlock_ability(void)
{
	u32 start_block, part_sectors;
	u8 addr[512] = {0};
	int  ret;

	/* detect the frp partition */
	start_block = sunxi_partition_get_offset_byname("frp");
	if (!start_block) {
		printf("cant find part named frp\n");
	} else {
		part_sectors = sunxi_partition_get_size_byname("frp");
#if DEBUG
		printf("start block = 0x%x, part_sectors = %d\n", start_block, part_sectors);
#endif
		/*read the last block of frp part to addr[]*/
		ret = sunxi_flash_read(start_block+part_sectors-1, 1, addr);

		printf("sunxi flash read :offset %x, %d bytes %s\n", ((start_block+part_sectors-1)<<9), ((part_sectors-1023)<<9),
				ret ? "OK" : "ERROR");
	}

	return addr[511];
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
static void __oem_operation(char *operation)
{
	char response[68];
	char lock_info[64];
	int  lockflag;
	int  ret;

	memset(lock_info, 0, 64);
	memset(response, 0, 68);

	lockflag = 0;

	if(!strncmp(operation, "lock", 4))
	{
		if ((gd->securemode == SUNXI_SECURE_MODE_WITH_SECUREOS) || (gd->securemode == SUNXI_SECURE_MODE_NO_SECUREOS)) {
			printf("the system is secure\n");
			lockflag = SUNXI_LOCKED;
			sunxi_fastboot_write_status_flag("locked");
		} else {
			printf("the system is normal\n");
		}
	}
	else if(!strncmp(operation, "unlock", 6))
	{
		if ((gd->securemode == SUNXI_SECURE_MODE_WITH_SECUREOS) || (gd->securemode == SUNXI_SECURE_MODE_NO_SECUREOS)) {
			printf("the system is secure\n");
			if (sunxi_read_oem_unlock_ability() == 0x00) {
			__limited_fastboot_unlock();
			return ;
		}
			lockflag = SUNXI_UNLOCKED;
			sunxi_fastboot_write_status_flag("unlocked");
		} else {
			printf("the system is normal\n");
		}
	}
	else
	{
		if(!strncmp(operation, "efex", 4))
		{
			strcpy(response, "OKAY");
			__sunxi_fastboot_send_status(response, strlen(response));

			sunxi_board_run_fel();
		}
		else
		{
			const char *info = "fastboot oem operation fail: unknown cmd";

			printf("%s\n", info);
			strcpy(response, "FAIL");
			strcat(response, info);

			__sunxi_fastboot_send_status(response, strlen(response));
		}

		return ;
	}

	/* erase user data and set the gd->lockflag  */
	ret = sunxi_oem_op_lock(lockflag, lock_info);
	if(!ret)
	{
		strcpy(response, "OKAY");
	}
	else
	{
		strcpy(response, "FAIL");
	}
	strcat(response, lock_info);
	printf("%s\n", response);

	__sunxi_fastboot_send_status(response, strlen(response));

	return ;
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
static void __continue(void)
{
	char response[32];
	int storage_type;

	storage_type = get_boot_storage_type();
	memset(response, 0, 32);
	strcpy(response,"OKAY");

	__sunxi_fastboot_send_status(response, strlen(response));

	sunxi_usb_exit();

	if( storage_type == STORAGE_EMMC || storage_type == STORAGE_SD)
	{
		setenv("bootcmd", "run setargs_mmc boot_normal");
	}
	else if (storage_type == STORAGE_NAND)
	{
		setenv("bootcmd", "run setargs_nand boot_normal");
	}
	do_bootd(NULL, 0, 1, NULL);

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
static void __unsupported_cmd(void)
{
	char response[32];

	memset(response, 0, 32);
	strcpy(response,"FAIL");

	__sunxi_fastboot_send_status(response, strlen(response));

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
static int sunxi_fastboot_init(void)
{
	printf("sunxi_fastboot_init\n");
	memset(&trans_data, 0, sizeof(fastboot_trans_set_t));
	sunxi_usb_fastboot_write_enable = 0;
    sunxi_usb_fastboot_status = SUNXI_USB_FASTBOOT_IDLE;

	all_download_bytes = 0;
	fastboot_data_flag = 0;

    trans_data.base_recv_buffer = (char *)FASTBOOT_TRANSFER_BUFFER;

	trans_data.base_send_buffer = (char *)malloc(SUNXI_FASTBOOT_SEND_MEM_SIZE);
    if(!trans_data.base_send_buffer)
    {
    	printf("sunxi usb fastboot err: unable to malloc memory for fastboot send\n");
    	free(trans_data.base_recv_buffer);

    	return -1;
    }
	printf("recv addr 0x%lx\n", (ulong)trans_data.base_recv_buffer);
	printf("send addr 0x%lx\n", (ulong)trans_data.base_send_buffer);
	printf("start to display fastbootlogo.bmp\n");
	sunxi_bmp_display("fastbootlogo.bmp");

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
static int sunxi_fastboot_exit(void)
{
	printf("sunxi_fastboot_exit\n");
    if(trans_data.base_send_buffer)
    {
    	free(trans_data.base_send_buffer);
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
static void sunxi_fastboot_reset(void)
{
	sunxi_usb_fastboot_write_enable = 0;
    sunxi_usb_fastboot_status = SUNXI_USB_FASTBOOT_IDLE;
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
static void  sunxi_fastboot_usb_rx_dma_isr(void *p_arg)
{
	printf("dma int for usb rx occur\n");
	//通知主循环，可以写入数据
	sunxi_usb_fastboot_write_enable = 1;
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
static void  sunxi_fastboot_usb_tx_dma_isr(void *p_arg)
{
	printf("dma int for usb tx occur\n");
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
static int sunxi_fastboot_standard_req_op(uint cmd, struct usb_device_request *req, uchar *buffer)
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
			tick_printf("sunxi fastboot error: standard req is not supported\n");

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
static int sunxi_fastboot_nonstandard_req_op(uint cmd, struct usb_device_request *req, uchar *buffer, uint data_status)
{
	return SUNXI_USB_REQ_DEVICE_NOT_SUPPORTED;
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
static void __limited_fastboot(void)
{
	char response[64];

	memset(response, 0, 64);
	tick_printf("secure mode,fastboot limited used\n");

	strcpy(response,"FAIL:secure mode,fastboot limited used");

	__sunxi_fastboot_send_status(response, strlen(response));

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
int sunxi_fastboot_status_read(void)
{
	char d_buffer[32];
	char buffer[512];
	int  d_data_len;
	int  data_len;
	int  ret;

	/* read locked or unlocked flag in secure storage */
	if (sunxi_secure_storage_init()) {
		printf("secure storage init fail\n");
	} else {
		memset(buffer, 0x00, sizeof(buffer));
		ret = sunxi_secure_object_read("device_unlock", d_buffer, 32, &d_data_len);
		if (!ret) {
			memset(d_buffer, 0x00, sizeof(d_buffer));
			ret = sunxi_secure_object_read("fastboot_status_flag", buffer, 512, &data_len);
		}

		if ((!ret) && (!strcmp(buffer, "locked")) && (!strcmp(d_buffer, "unlock"))) {
			setenv("verifiedbootstate", "yellow");
		}

		if (ret) {
			printf("sunxi secure storage has no flag\n");
			gd->lockflag = SUNXI_LOCKED;
		} else {
			if (!strcmp(buffer, "unlocked")) {
				printf("find fastboot unlock flag\n");
				gd->lockflag = SUNXI_UNLOCKED;
				return -1;
			} else if (!strcmp(buffer, "locked")) {
				printf("find fastboot locked flag\n");
				gd->lockflag = SUNXI_LOCKED;
				return 0;
			}
			printf("do not find fastboot status flag\n");
		}
		sunxi_secure_storage_exit();
	}
	return 0 ;
}

static int sunxi_fastboot_status(void)
{
	if ((gd->securemode == SUNXI_SECURE_MODE_WITH_SECUREOS) || (gd->securemode == SUNXI_SECURE_MODE_NO_SECUREOS))
	{
		printf("the system is secure\n");
		return sunxi_fastboot_status_read();
	}
	return -1;
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
static int sunxi_fastboot_state_loop(void  *buffer)
{
	int ret;
	sunxi_ubuf_t *sunxi_ubuf = (sunxi_ubuf_t *)buffer;
	char  response[68];

	switch(sunxi_usb_fastboot_status)
	{
		case SUNXI_USB_FASTBOOT_IDLE:
			if(sunxi_ubuf->rx_ready_for_data == 1)
			{
				sunxi_usb_fastboot_status = SUNXI_USB_FASTBOOT_SETUP;
			}

			break;

		case SUNXI_USB_FASTBOOT_SETUP:

			tick_printf("SUNXI_USB_FASTBOOT_SETUP\n");

			tick_printf("fastboot command = %s\n", sunxi_ubuf->rx_req_buffer);

			sunxi_usb_fastboot_status = SUNXI_USB_FASTBOOT_IDLE;
			sunxi_ubuf->rx_ready_for_data = 0;
			if(memcmp(sunxi_ubuf->rx_req_buffer, "reboot-bootloader", strlen("reboot-bootloader")) == 0)
			{
				tick_printf("reboot-bootloader\n");
				__fastboot_reboot(PMU_PRE_FASTBOOT_MODE);
			} else if (memcmp(sunxi_ubuf->rx_req_buffer, "reboot", 6) == 0) {
				tick_printf("reboot\n");
				__fastboot_reboot(0);
			}
			else if(memcmp(sunxi_ubuf->rx_req_buffer, "erase:", 6) == 0)
			{
				tick_printf("erase\n");
				if (!sunxi_fastboot_status())
				{
					__limited_fastboot();
					break;
				}
				__erase_part((char *)(sunxi_ubuf->rx_req_buffer + 6));
			}
			else if(memcmp(sunxi_ubuf->rx_req_buffer, "flash:", 6) == 0)
			{
				tick_printf("flash\n");
				if (!sunxi_fastboot_status())
				{
					__limited_fastboot();
					break;
				}
				if(memcmp((char *)(sunxi_ubuf->rx_req_buffer + 6),"u-boot",6) == 0)
					__flash_to_uboot();
				else
					__flash_to_part((char *)(sunxi_ubuf->rx_req_buffer + 6));
			}
			else if(memcmp(sunxi_ubuf->rx_req_buffer, "download:", 9) == 0)
			{
				tick_printf("download\n");
				if (!sunxi_fastboot_status())
				{
					__limited_fastboot();
					break;
				}
				ret = __try_to_download((char *)(sunxi_ubuf->rx_req_buffer + 9), response);
				if(ret >= 0)
				{
					fastboot_data_flag = 1;
					sunxi_ubuf->rx_req_buffer  = (uchar *)trans_data.base_recv_buffer;
					sunxi_usb_fastboot_status  = SUNXI_USB_FASTBOOT_RECEIVE_DATA;
				}
				__sunxi_fastboot_send_status(response, strlen(response));
			}
			else if(memcmp(sunxi_ubuf->rx_req_buffer, "boot", 4) == 0)
			{
				tick_printf("boot\n");
				if (!sunxi_fastboot_status())
				{
					__limited_fastboot();
					break;
				}
				__boot();
			}
			else if(memcmp(sunxi_ubuf->rx_req_buffer, "getvar:", 7) == 0)
			{
				tick_printf("getvar\n");
				if (!sunxi_fastboot_status())
				{
					__limited_fastboot();
					break;
				}
				__get_var((char *)(sunxi_ubuf->rx_req_buffer + 7));
			}
			else if(memcmp(sunxi_ubuf->rx_req_buffer, "oem", 3) == 0)
			{
				tick_printf("oem operations\n");
				__oem_operation((char *)(sunxi_ubuf->rx_req_buffer + 4));
			}
			else if(memcmp(sunxi_ubuf->rx_req_buffer, "continue", 8) == 0)
			{
				tick_printf("continue\n");
				__continue();
			}
			else
			{
				tick_printf("not supported fastboot cmd\n");
				__unsupported_cmd();
			}

			break;

	  	case SUNXI_USB_FASTBOOT_SEND_DATA:

	  		tick_printf("SUNXI_USB_FASTBOOT_SEND_DATA\n");

	  		break;

	  	case SUNXI_USB_FASTBOOT_RECEIVE_DATA:

	  		//tick_printf("SUNXI_USB_FASTBOOT_RECEIVE_DATA\n");
	  		if((fastboot_data_flag == 1) && ((char *)sunxi_ubuf->rx_req_buffer == all_download_bytes + trans_data.base_recv_buffer))	//传输完毕
	  		{
	  			tick_printf("fastboot transfer finish\n");
	  			fastboot_data_flag = 0;
	  			sunxi_usb_fastboot_status  = SUNXI_USB_FASTBOOT_IDLE;

		  		sunxi_ubuf->rx_req_buffer = sunxi_ubuf->rx_base_buffer;

		  		sprintf(response,"OKAY");
		  		__sunxi_fastboot_send_status(response, strlen(response));
			}

	  		break;

	  	default:
	  		break;
	}

	return 0;
}


sunxi_usb_module_init(SUNXI_USB_DEVICE_FASTBOOT,					\
					  sunxi_fastboot_init,							\
					  sunxi_fastboot_exit,							\
					  sunxi_fastboot_reset,							\
					  sunxi_fastboot_standard_req_op,				\
					  sunxi_fastboot_nonstandard_req_op,			\
					  sunxi_fastboot_state_loop,					\
					  sunxi_fastboot_usb_rx_dma_isr,				\
					  sunxi_fastboot_usb_tx_dma_isr					\
					  );
