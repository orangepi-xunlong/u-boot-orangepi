/*
 *  * Copyright 2000-2009
 *   * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *    *
 *     * SPDX-License-Identifier:	GPL-2.0+
 *     */
#include <common.h>
#include <command.h>
#include "usb_base.h"
#include <scsi.h>
#include <asm/arch/dma.h>
#include <sys_partition.h>
#include <fastboot.h>
#include "usb_fastboot.h"
/* #include <android_misc.h> */
#include <sunxi_board.h>
#include <axp_pmic.h>
#include <sunxi_mbr.h>
#include <sunxi_flash.h>
#include <private_uboot.h>
#include "sprite_download.h"
#include <private_toc.h>
#include <securestorage.h>
#include <sunxi_flash.h>
#include <sprite_verify.h>
#include <private_boot0.h>
#include "../../sprite/sparse/sparse.h"
#include <android_misc.h>
#include <sunxi_avb.h>
#include <asm/arch/efuse.h>
#include <sunxi_image_verifier.h>
DECLARE_GLOBAL_DATA_PTR;

/* int do_go(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[]); */

static int sunxi_usb_fastboot_write_enable ;
static int sunxi_usb_fastboot_status       = SUNXI_USB_FASTBOOT_IDLE;

static fastboot_trans_set_t trans_data;

static uint all_download_bytes;

int fastboot_data_flag;

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

int __attribute__((weak))
sunxi_secure_object_read(const char *item_name, char *buffer, int buffer_len,
			 int *data_len)
{
	return 0;
}

int __attribute__((weak))
sunxi_secure_object_write(const char *item_name, char *buffer, int length)
{
	return 0;
}

int __attribute__((weak)) sunxi_verify_toc1_root_cert(void *buffer)
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
	if ((0 == req->wIndex) && (0 == req->wValue)) {
		sunxi_udc_ep_reset();
	} else {
		printf("err: invalid wIndex and wValue, (0, %d), (0, %d)\n",
		       req->wIndex, req->wValue);
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
	if (1 == req->wValue) {
		sunxi_udc_ep_reset();
	} else {
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
	switch (req->wValue >> 8) {
	case USB_DT_DEVICE: //设备描述符
	{
		struct usb_device_descriptor *dev_dscrptr;

		sunxi_usb_dbg("get device descriptor\n");

		dev_dscrptr = (struct usb_device_descriptor *)buffer;
		memset((void *)dev_dscrptr, 0,
		       sizeof(struct usb_device_descriptor));

		dev_dscrptr->bLength =
			MIN(req->wLength, sizeof(struct usb_device_descriptor));
		dev_dscrptr->bDescriptorType = USB_DT_DEVICE;
#ifdef CONFIG_USB_1_1_DEVICE
		dev_dscrptr->bcdUSB = 0x110;
#else
		dev_dscrptr->bcdUSB = 0x200;
#endif
		dev_dscrptr->bDeviceClass = 0xff; //设备类：大容量存储
		dev_dscrptr->bDeviceSubClass = 0xff;
		dev_dscrptr->bDeviceProtocol = 0xff;
		dev_dscrptr->bMaxPacketSize0 = 0x40;
		dev_dscrptr->idVendor	= DEVICE_VENDOR_ID;
		dev_dscrptr->idProduct       = DEVICE_PRODUCT_ID;
		dev_dscrptr->bcdDevice       = DEVICE_BCD;
		dev_dscrptr->iManufacturer =
			SUNXI_FASTBOOT_DEVICE_STRING_MANUFACTURER_INDEX;
		dev_dscrptr->iProduct =
			SUNXI_FASTBOOT_DEVICE_STRING_PRODUCT_INDEX;
		dev_dscrptr->iSerialNumber =
			SUNXI_FASTBOOT_DEVICE_STRING_SERIAL_NUMBER_INDEX;
		dev_dscrptr->bNumConfigurations = 1;

		sunxi_udc_send_setup(dev_dscrptr->bLength, buffer);
	} break;

	case USB_DT_CONFIG: //配置描述符
	{
		struct usb_configuration_descriptor *config_dscrptr;
		struct usb_interface_descriptor *inter_dscrptr;
		struct usb_endpoint_descriptor *ep_in, *ep_out;
		unsigned char bytes_remaining = req->wLength;
		unsigned char bytes_total     = 0;

		sunxi_usb_dbg("get config descriptor\n");

		bytes_total = sizeof(struct usb_configuration_descriptor) +
			      sizeof(struct usb_interface_descriptor) +
			      sizeof(struct usb_endpoint_descriptor) +
			      sizeof(struct usb_endpoint_descriptor);

		memset(buffer, 0, bytes_total);

		config_dscrptr =
			(struct usb_configuration_descriptor *)(buffer + 0);
		inter_dscrptr =
			(struct usb_interface_descriptor
				 *)(buffer +
				    sizeof(struct usb_configuration_descriptor));
		ep_in = (struct usb_endpoint_descriptor
				 *)(buffer +
				    sizeof(struct usb_configuration_descriptor) +
				    sizeof(struct usb_interface_descriptor));
		ep_out =
			(struct usb_endpoint_descriptor
				 *)(buffer +
				    sizeof(struct usb_configuration_descriptor) +
				    sizeof(struct usb_interface_descriptor) +
				    sizeof(struct usb_endpoint_descriptor));

		/* configuration */
		config_dscrptr->bLength =
			MIN(bytes_remaining,
			    sizeof(struct usb_configuration_descriptor));
		config_dscrptr->bDescriptorType     = USB_DT_CONFIG;
		config_dscrptr->wTotalLength	= bytes_total;
		config_dscrptr->bNumInterfaces      = 1;
		config_dscrptr->bConfigurationValue = 1;
		config_dscrptr->iConfiguration =
			SUNXI_FASTBOOT_DEVICE_STRING_CONFIG_INDEX;
		config_dscrptr->bmAttributes = 0xC0;
		config_dscrptr->bMaxPower = 0xFA; //最大电流500ms(0xfa * 2)

		bytes_remaining -= config_dscrptr->bLength;
		/* interface */
		inter_dscrptr->bLength =
			MIN(bytes_remaining,
			    sizeof(struct usb_interface_descriptor));
		inter_dscrptr->bDescriptorType   = USB_DT_INTERFACE;
		inter_dscrptr->bInterfaceNumber  = 0x00;
		inter_dscrptr->bAlternateSetting = 0x00;
		inter_dscrptr->bNumEndpoints     = 0x02;
		inter_dscrptr->bInterfaceClass   = 0xff; //fastboot storage
		inter_dscrptr->bInterfaceSubClass =
			FASTBOOT_INTERFACE_SUB_CLASS;
		inter_dscrptr->bInterfaceProtocol = FASTBOOT_INTERFACE_PROTOCOL;
		inter_dscrptr->iInterface =
			SUNXI_FASTBOOT_DEVICE_STRING_INTERFACE_INDEX;

		bytes_remaining -= inter_dscrptr->bLength;
		/* ep_in */
		ep_in->bLength = MIN(bytes_remaining,
				     sizeof(struct usb_endpoint_descriptor));
		ep_in->bDescriptorType  = USB_DT_ENDPOINT;
		ep_in->bEndpointAddress = sunxi_udc_get_ep_in_type(); /* IN */
		ep_in->bmAttributes     = USB_ENDPOINT_XFER_BULK;
		ep_in->wMaxPacketSize   = sunxi_udc_get_ep_max();
		ep_in->bInterval	= 0x00;

		bytes_remaining -= ep_in->bLength;
		/* ep_out */
		ep_out->bLength = MIN(bytes_remaining,
				      sizeof(struct usb_endpoint_descriptor));
		ep_out->bDescriptorType = USB_DT_ENDPOINT;
		ep_out->bEndpointAddress =
			sunxi_udc_get_ep_out_type(); /* OUT */
		ep_out->bmAttributes   = USB_ENDPOINT_XFER_BULK;
		ep_out->wMaxPacketSize = sunxi_udc_get_ep_max();
		ep_out->bInterval      = 0x00;

		bytes_remaining -= ep_out->bLength;

		sunxi_udc_send_setup(MIN(req->wLength, bytes_total), buffer);
	} break;

	case USB_DT_STRING: {
		unsigned char bLength      = 0;
		unsigned char string_index = req->wValue & 0xff;

		sunxi_usb_dbg("get string descriptor\n");

		/* Language ID */
		if (string_index == 0) {
			bLength = MIN(4, req->wLength);

			sunxi_udc_send_setup(bLength,
					     (void *)sunxi_usb_fastboot_dev[0]);
		} else if (string_index < SUNXI_USB_FASTBOOT_DEV_MAX) {
			/* Size of string in chars */
			unsigned char i		 = 0;
			unsigned char str_length = strlen(
				(const char *)
					sunxi_usb_fastboot_dev[string_index]);
			unsigned char bLength = 2 + (2 * str_length);
			str_length =
				strlen(sunxi_usb_fastboot_dev[string_index]);
			bLength   = 2 + (2 * str_length);
			buffer[0] = bLength; /* length */
			buffer[1] = USB_DT_STRING; /* descriptor = string */

			/* Copy device string to fifo, expand to simple unicode */
			for (i = 0; i < str_length; i++) {
				buffer[2 + 2 * i + 0] =
					sunxi_usb_fastboot_dev[string_index][i];
				buffer[2 + 2 * i + 1] = 0;
			}
			bLength = MIN(bLength, req->wLength);

			sunxi_udc_send_setup(bLength, buffer);
		} else {
			printf("sunxi usb err: string line %d is not supported\n",
			       string_index);
		}
	} break;

	case USB_DT_DEVICE_QUALIFIER: {
#ifdef CONFIG_USB_1_1_DEVICE
		/* This is an invalid request for usb 1.1, nak it */
		USBC_Dev_EpSendStall(sunxi_udc_source.usbc_hd,
				     USBC_EP_TYPE_EP0);
#else
		struct usb_qualifier_descriptor *qua_dscrpt;

		sunxi_usb_dbg("get qualifier descriptor\n");

		qua_dscrpt = (struct usb_qualifier_descriptor *)buffer;
		memset(&buffer, 0, sizeof(struct usb_qualifier_descriptor));
		size_t temp	 = sizeof(struct usb_qualifier_descriptor);
		qua_dscrpt->bLength = MIN(req->wLength, sizeof(temp));
		qua_dscrpt->bDescriptorType    = USB_DT_DEVICE_QUALIFIER;
		qua_dscrpt->bcdUSB	     = 0x200;
		qua_dscrpt->bDeviceClass       = 0xff;
		qua_dscrpt->bDeviceSubClass    = 0xff;
		qua_dscrpt->bDeviceProtocol    = 0xff;
		qua_dscrpt->bMaxPacketSize0    = 0x40;
		qua_dscrpt->bNumConfigurations = 1;
		//qua_dscrpt->bRESERVED          = 0;
		qua_dscrpt->breserved = 0;

		sunxi_udc_send_setup(qua_dscrpt->bLength, buffer);
#endif
	} break;

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
	if (0 == req->wLength) {
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

	sprintf(response, "OKAY");
	__sunxi_fastboot_send_status(response, strlen(response));
	mdelay(1000); /* 1 sec */

	sunxi_board_restart(word_mode);

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
	u32 start, unerased_sectors;
	u32 nblock	    = FASTBOOT_ERASE_BUFFER_SIZE / 512;
	disk_partition_t info = { 0 };
	char *name	    = CONFIG_LAST_PARTITION_NAME;
	int ret = 0;
	void *addr = (void *)memalign(CONFIG_SYS_CACHELINE_SIZE, FASTBOOT_ERASE_BUFFER_SIZE);
	if (addr == NULL) {
		pr_err("%s memalign fail\n", __func__);
		return -1;
	}

	if (sunxi_partition_get_info((const char *)name, &info) < 0) {
		printf("line:%d:get %s partition info fail\n", __LINE__, name);
		ret = -1;
		goto erase_userdata_fail;
	}
	printf("partinfo: name %s, start 0x%lx, size 0x%lx\n", info.name,
	       info.start, info.size);
	start		 = info.start;
	unerased_sectors = info.size;

	printf("***start to erase %s partition, this may take some time***\n", CONFIG_LAST_PARTITION_NAME);
	if (unerased_sectors >= 16 * 1024 * 1024 / 512)
		unerased_sectors = 16 * 1024 * 1024 / 512;

	if ((!start) || (!unerased_sectors)) {
		printf("sunxi fastboot erase FAIL: partition %s does not exist\n", CONFIG_LAST_PARTITION_NAME);
		ret = -1;
		goto erase_userdata_fail;
	}

	memset(addr, 0xff, FASTBOOT_ERASE_BUFFER_SIZE);
	while (unerased_sectors >= nblock) {
		if (!sunxi_flash_write(start, nblock, addr)) {
			printf("sunxi fastboot erase FAIL: failed to erase partition %s \n", CONFIG_LAST_PARTITION_NAME);
			ret = -1;
			goto erase_userdata_fail;
		}
		start += nblock;
		unerased_sectors -= nblock;
	}
	if (unerased_sectors) {
		if (!sunxi_flash_write(start, unerased_sectors, addr)) {
			printf("sunxi fastboot erase FAIL: failed to erase partition %s \n", CONFIG_LAST_PARTITION_NAME);
			ret = -1;
			goto erase_userdata_fail;
		}
	}
	printf("sunxi fastboot: partition '%s' erased\n", CONFIG_LAST_PARTITION_NAME);
erase_userdata_fail:
	free(addr);
	return ret;
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
	u32 start, unerased_sectors;
	u32 nblock = FASTBOOT_ERASE_BUFFER_SIZE / 512;
	char response[68];
	disk_partition_t info = { 0 };
	int ret = 0;
	void *addr = (void *)memalign(CONFIG_SYS_CACHELINE_SIZE, FASTBOOT_ERASE_BUFFER_SIZE);
	if (addr == NULL) {
		pr_err("%s memalign fail\n", __func__);
		return -1;
	}

	if (!memcmp(name, "userdata", strlen("userdata"))) {
		erase_userdata();
	} else {
		if (sunxi_partition_get_info((const char *)name, &info) < 0) {
			pr_err("line:%d:get %s partition info fail\n", __LINE__, name);
		} else {
			start		 = info.start;
			unerased_sectors = info.size;

			printf("********* starting to erase %s partition, this may take some time, please wait for minutes ***********\n",
			       name);
			if (memcmp(name, CONFIG_LAST_PARTITION_NAME, strlen(CONFIG_LAST_PARTITION_NAME)) == 0) {
				if (unerased_sectors >= 16 * 1024 * 1024 / 512)
					unerased_sectors = 16 * 1024 * 1024 / 512;
			} else if (memcmp(name, "cache", strlen("cache")) == 0) {
				if (unerased_sectors >= 16 * 1024 * 1024 / 512)
					unerased_sectors = 16 * 1024 * 1024 / 512;
			}

			if ((!start) || (!unerased_sectors)) {
				printf("sunxi fastboot erase FAIL: partition %s does not exist\n",
				       name);
				sprintf(response, "FAILerase: partition %s does not exist",
					name);

				ret = -1;
				goto __erase_part_fail;
			}

			memset(addr, 0xff, FASTBOOT_ERASE_BUFFER_SIZE);
			while (unerased_sectors >= nblock) {
				if (!sunxi_flash_write(start, nblock, addr)) {
					printf("sunxi fastboot erase FAIL: failed to erase partition %s \n",
					       name);
					sprintf(response,
						"FAILerase: failed to erase partition %s",
						name);

					ret = -1;
					goto __erase_part_fail;
				}
				start += nblock;
				unerased_sectors -= nblock;
			}
			if (unerased_sectors) {
				if (!sunxi_flash_write(start, unerased_sectors, addr)) {
					printf("sunxi fastboot erase FAIL: failed to erase partition %s \n",
					       name);
					sprintf(response,
						"FAILerase: failed to erase partition %s",
						name);

					ret = -1;
					goto __erase_part_fail;
				}
			}
		}
		printf("sunxi fastboot: partition '%s' erased\n", name);
	}
	sprintf(response, "OKAY");

__erase_part_fail:
	free(addr);
	__sunxi_fastboot_send_status(response, strlen(response));
	return ret;
}


#ifdef CONFIG_SUNXI_FASTBOOT_MBR
static void __flash_to_mbr(void)
{
	char response[68];
	int storage_type = get_boot_storage_type();
	int mbr_num      = 4;
	char *img_mbr    = (char *)trans_data.base_recv_buffer;
	if (sunxi_sprite_verify_mbr(img_mbr)) {
		printf("sunxi fastboot: mbr verify fail\n");
		sprintf(response, "FAIL");
		__sunxi_fastboot_send_status(response, strlen(response));
		return;
	}
#if defined(CONFIG_SUNXI_COMM_NAND_V1) || defined(CONFIG_SUNXI_COMM_NAND)
	if (storage_type == STORAGE_NAND) {
		nand_get_mbr((char *)img_mbr, 16 * 1024);
	} else
#endif
		if (storage_type == STORAGE_NOR) {
		mbr_num = 1;
	}
	if (sunxi_sprite_download_mbr(img_mbr, sizeof(sunxi_mbr_t) * mbr_num)) {
		printf("sunxi fastboot error: download mbr err\n");
		sprintf(response, "FAIL");
	} else {
		sunxi_flash_write_end();
		sunxi_flash_flush();
		printf("sunxi fastboot: successed in downloading mbr\n");
		sprintf(response, "OKAY");
	}
	__sunxi_fastboot_send_status(response, strlen(response));
	return;
}
#endif

#ifdef CONFIG_SUNXI_FASTBOOT_BOOT0
static void __flash_to_boot0(void)
{
	char response[68];
	int ret	= -1;
	char *temp_buf = (char *)trans_data.base_recv_buffer;
	if (gd->bootfile_mode == SUNXI_BOOT_FILE_NORMAL ||
	    gd->bootfile_mode == SUNXI_BOOT_FILE_PKG) {
		boot0_file_head_t *temp_boot0 = NULL;
		boot0_file_head_t *boot0 =
			(boot0_file_head_t *)trans_data.base_recv_buffer;
		if (strncmp((const char *)boot0->boot_head.magic, BOOT0_MAGIC,
			    MAGIC_SIZE)) {
			printf("sunxi fastboot error: there is not boot0 file\n");
			sprintf(response, "FAIL");
			goto toc0_error;
		}
		temp_boot0 =
			memalign(64, ALIGN(sizeof(boot0_file_head_t), 512));
		memset(temp_boot0, 0x0, ALIGN(sizeof(boot0_file_head_t), 512));
#if defined(CONFIG_SUNXI_COMM_NAND_V1) || defined(CONFIG_SUNXI_COMM_NAND)
		if (get_boot_storage_type() == STORAGE_NAND) {
			nand_read_boot0(temp_boot0,
					ALIGN(sizeof(boot0_file_head_t), 512) /
						512);
		} else
#endif
		{
			sunxi_flash_phyread(
				BOOT0_SDMMC_START_ADDR,
				ALIGN(sizeof(boot0_file_head_t), 512) / 512,
				temp_boot0);
		}
		memcpy(boot0->prvt_head.storage_data,
		       temp_boot0->prvt_head.storage_data,
		       sizeof(temp_boot0->prvt_head.storage_data));
		boot0->boot_head.check_sum = sunxi_sprite_generate_checksum(
			temp_buf, boot0->boot_head.length,
			boot0->boot_head.check_sum);
		free(temp_boot0);
	} else {
		toc0_private_head_t *temp_toc0 = NULL;
		toc0_private_head_t *toc0 =
			(toc0_private_head_t *)trans_data.base_recv_buffer;
		sbrom_toc0_config_t *temp_toc0_config = NULL;
		sbrom_toc0_config_t *toc0_config      = NULL;
		if (strncmp((const char *)toc0->name, TOC0_MAGIC, MAGIC_SIZE)) {
			printf("sunxi fastboot error: there is not toc0 file\n");
			sprintf(response, "FAIL");
			goto toc0_error;
		}
		temp_toc0 = memalign(64, 2048);
		memset(temp_toc0, 0x0, 2048);
#if defined(CONFIG_SUNXI_COMM_NAND_V1) || defined(CONFIG_SUNXI_COMM_NAND)
		if (get_boot_storage_type() == STORAGE_NAND) {
			nand_read_boot0(temp_toc0,
					ALIGN(sizeof(boot0_file_head_t), 512) /
						512);
		} else
#endif
		{
			sunxi_flash_phyread(BOOT0_SDMMC_START_ADDR, 2048 / 512,
					    temp_toc0);
		}
		if (temp_toc0->items_nr == 3) {
			temp_toc0_config =
				(sbrom_toc0_config_t *)(temp_toc0 + 0xa0);
			toc0_config =
				(sbrom_toc0_config_t
					 *)(trans_data.base_recv_buffer + 0xa0);
		} else {
			temp_toc0_config =
				(sbrom_toc0_config_t *)(temp_toc0 + 0x80);
			toc0_config =
				(sbrom_toc0_config_t
					 *)(trans_data.base_recv_buffer + 0x80);
		}
		memcpy(toc0_config->storage_data,
		       temp_toc0_config->storage_data,
		       sizeof(temp_toc0_config->storage_data));
		toc0->check_sum = sunxi_sprite_generate_checksum(
			temp_buf, toc0->length, toc0->check_sum);
		free(temp_toc0);
	}
	ret = sunxi_sprite_download_boot0((char *)temp_buf,
					  get_boot_storage_type());
	if (!ret) {
		sunxi_flash_write_end();
		sunxi_flash_flush();
		printf("sunxi fastboot: successed in downloading boot0/toc0\n");
		sprintf(response, "OKAY");
	} else {
		printf("sunxi fastboot: fail in downloading boot0/toc0\n");
		sprintf(response, "FAIL");
	}
toc0_error:
	__sunxi_fastboot_send_status(response, strlen(response));
	return;
}
#endif

#ifdef CONFIG_SUNXI_FASTBOOT_UBOOT
static void __flash_to_uboot(void)
{
	sbrom_toc1_head_info_t *temp_buf =
		(sbrom_toc1_head_info_t *)trans_data.base_recv_buffer;
	char response[68];
	int ret = -1;

	if (strncmp((char *)temp_buf->name, "sunxi-package",
		    sizeof("sunxi-package")) &&
	    strncmp((char *)temp_buf->name, "sunxi-secure",
		    sizeof("sunxi-secure"))) {
		printf("sunxi fastboot error: there is not sunxi-package/toc1 file\n");
		sprintf(response,
			"FAILdownload:there is not boot-package/toc1 file \n");
		__sunxi_fastboot_send_status(response, strlen(response));
		return;
	}

	if (sid_get_security_status()) {
		ret = sunxi_verify_toc1_root_cert(temp_buf);
		if (ret) {
			printf("sunxi fastboot error: check toc1 root key fail\n");
			sprintf(response,
			"FAILdownload:check toc1 root key fail \n");
			__sunxi_fastboot_send_status(response, strlen(response));
			return;
		}
	}

	printf("ready to download bytes 0x%x\n", trans_data.try_to_recv);
	ret = sunxi_sprite_download_uboot((char *)temp_buf,
					  get_boot_storage_type(), 1);
	if (!ret) {
		if (!strncmp((char *)temp_buf->name, "sunxi-package",
			     sizeof("sunxi-package"))) {
		sunxi_flash_write_end();
		sunxi_flash_flush();
		printf("sunxi fastboot: successed in downloading uboot package\n");
		} else {
			sunxi_flash_write_end();
			sunxi_flash_flush();
			printf("sunxi fastboot: successed in downloading toc1\n");
		}
		sprintf(response, "OKAY");
	} else {
		if (!strncmp((char *)temp_buf->name, "sunxi-package",
			     sizeof("sunxi-package"))) {
		printf("sunxi fastboot: fail in downloading uboot package\n");
		} else {
			printf("sunxi fastboot: fail in downloading toc1\n");
		}
		sprintf(response, "FAIL");
	}
	__sunxi_fastboot_send_status(response, strlen(response));
	return;
}
#endif

static int __flash_to_part(char *name)
{
	char *addr = trans_data.base_recv_buffer;
	u32 start, data_sectors;
	u32 part_sectors;
	u32 nblock = FASTBOOT_TRANSFER_BUFFER_SIZE / 512;
	char response[68];
	disk_partition_t info = { 0 };
	int ret;

	ret = sunxi_partition_get_info((const char *)name, &info);
	if ( ret < 0) {
		uint addr_in_hex;

		printf("sunxi fastboot download FAIL: partition %s does not exist\n",
		       name);
		printf("probe it as a dram address\n");

		ret = strict_strtoul((const char *)name, 16,
				     (long unsigned int *)&addr_in_hex);
		if (ret) {
			printf("sunxi fatboot download FAIL: it is not a dram address\n");

			sprintf(response,
				"FAILdownload: partition %s does not exist",
				name);
			__sunxi_fastboot_send_status(response,
						     strlen(response));

			return -1;
		} else {
			printf("ready to move data to 0x%x, bytes 0x%x\n",
			       addr_in_hex, trans_data.try_to_recv);
			memcpy((void *)(ulong)addr_in_hex, addr,
			       trans_data.try_to_recv);
		}
	} else {
		int format;

		start	= info.start;
		part_sectors = info.size;

		printf("ready to download bytes 0x%x\n",
		       trans_data.try_to_recv);
		format = unsparse_probe(addr, trans_data.try_to_recv, start);

		if (ANDROID_FORMAT_DETECT == format) {
			if (unsparse_direct_write(addr,
						  trans_data.try_to_recv)) {
				printf("sunxi fastboot download FAIL: failed to write partition %s \n",
				       name);
				sprintf(response,
					"FAILdownload: write partition %s err",
					name);

				return -1;
			}
		} else {
			data_sectors = (trans_data.try_to_recv + 511) / 512;
			if (data_sectors > part_sectors) {
				printf("sunxi fastboot download FAIL: partition %s size 0x%x is smaller than data size 0x%x\n",
				       name, trans_data.act_recv,
				       data_sectors * 512);
				sprintf(response,
					"FAILdownload: partition size < data size");

				__sunxi_fastboot_send_status(response,
							     strlen(response));

				return -1;
			}
			while (data_sectors >= nblock) {
				if (!sunxi_flash_write(start, nblock, addr)) {
					printf("sunxi fastboot download FAIL: failed to write partition %s \n",
					       name);
					sprintf(response,
						"FAILdownload: write partition %s err",
						name);

					__sunxi_fastboot_send_status(
						response, strlen(response));

					return -1;
				}
				start += nblock;
				data_sectors -= nblock;
				addr += FASTBOOT_TRANSFER_BUFFER_SIZE;
			}
			if (data_sectors) {
				if (!sunxi_flash_write(start, data_sectors,
						       addr)) {
					printf("sunxi fastboot download FAIL: failed to write partition %s \n",
					       name);
					sprintf(response,
						"FAILdownload: write partition %s err",
						name);

					__sunxi_fastboot_send_status(
						response, strlen(response));

					return -1;
				}
			}
		}
	}

	sunxi_flash_write_end();
	sunxi_flash_flush();
	printf("sunxi fastboot: successed in downloading partition '%s'\n",
	       name);
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
	int ret = -1;

	trans_data.try_to_recv = simple_strtoul(download_size, NULL, 16);
	all_download_bytes     = trans_data.try_to_recv;
	printf("Starting download of %d BYTES\n", trans_data.try_to_recv);
	printf("Starting download of %d MB\n", trans_data.try_to_recv >> 20);

	if (0 == trans_data.try_to_recv) {
		/* bad user input */
		sprintf(response, "FAILdownload: data size is 0");
	} else if (trans_data.try_to_recv > SUNXI_USB_FASTBOOT_BUFFER_MAX) {
		sprintf(response, "FAILdownload: data > buffer");
	} else {
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
	char response[68];

	if (all_download_bytes > CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE) {
		char start[32];
		char *bootm[3] = {
			"bootm", NULL, NULL,
		};
		char *go[3] = {
			"go", NULL, NULL,
		};

		struct fastboot_boot_img_hdr *fb_hdr =
			(struct fastboot_boot_img_hdr *)
				trans_data.base_recv_buffer;

		/* Skip the mkbootimage header */
		image_header_t *hdr =
			(image_header_t *)&trans_data.base_recv_buffer
				[CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE];

		bootm[1] = go[1] = start;
		sprintf(start, "0x%lx", (ulong)hdr);

		printf("start addr %s\n", start);
		/* Execution should jump to kernel so send the response
		   now and wait a bit.  */
		sprintf(response, "OKAY");
		//		fastboot_tx_status(response, strlen(response));
		mdelay(1000); /* 1 sec */

		if (ntohl(hdr->ih_magic) == IH_MAGIC) {
			/* Looks like a kernel.. */
			printf("Booting kernel..\n");

			/*
			 * Check if the user sent a bootargs down.
			 * If not, do not override what is already there
			 */
			if (strlen((char *)&fb_hdr->cmdline[0])) {
				printf("Image has cmdline:");
				printf("%s\n", &fb_hdr->cmdline[0]);
				env_set("bootargs",
					(char *)&fb_hdr->cmdline[0]);
			}
			do_bootm(NULL, 0, 2, bootm);
		} else {
			/* Raw image, maybe another uboot */
			printf("Booting raw image..\n");
			printf("do_go fail\n");
			return ;
			/* do_go(NULL, 0, 2, go); */
		}
		printf("ERROR : bootting failed\n");
		printf("You should reset the board\n");
	} else {
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
	strcpy(response, "OKAY");
	if (!strcmp(ver_name, "version")) {
		strcpy(response + 4, FASTBOOT_VERSION);
	} else if (!strcmp(ver_name, "product")) {
		strcpy(response + 4, SUNXI_FASTBOOT_DEVICE_PRODUCT);
	} else if (!strcmp(ver_name, "serialno")) {
		strcpy(response + 4, SUNXI_FASTBOOT_DEVICE_SERIAL_NUMBER);
	} else if (!strcmp(ver_name, "downloadsize")) {
		sprintf(response + 4, "0x%08x", SUNXI_USB_FASTBOOT_BUFFER_MAX);
		printf("response: %s\n", response);
	} else if (!strcmp(ver_name, "secure")) {
		strcpy(response + 4, "yes");
	} else if (!strcmp(ver_name, "max-download-size")) {
		sprintf(response + 4, "0x%08x", SUNXI_USB_FASTBOOT_BUFFER_MAX);
		printf("response: %s\n", response);
	} else {
		strcpy(response + 4, "not supported");
	}

	__sunxi_fastboot_send_status(response, strlen(response));

	return;
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
	int ret;

	/* write locked or unlocked flag to secure storage */
	ret = sunxi_secure_storage_init();
	if (ret < 0) {
		printf("secure storage init err\n");
		return -1;
	}
	if (sunxi_secure_object_write("fastboot_status_flag", name,
				      strlen(name))) {
		printf("save  %s flag to secure storage failed\n", name);
		return -1;
	}
	/* save device unlock flag for boot_verify to judge if the device ever unlock */
	if (memcmp(name, "unlocked", strlen("unlocked")) == 0) {
		if (sunxi_secure_object_write("device_unlock", "unlock",
					      strlen("unlock"))) {
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
	printf("GMS,fastboot unlock limited used!!!\n");

	strcpy(response,
	       "FAILability is 0. Permission denied for this command!");

	__sunxi_fastboot_send_status(response, strlen(response));

	return;
}

static char sunxi_read_oem_unlock_ability(void)
{
	u32 start_block, part_sectors;
	u8 addr[512]	  = { 0 };
	disk_partition_t info = { 0 };
	int ret;
	char *name = "frp";
	if (sunxi_partition_get_info((const char *)name, &info) < 0) {
		printf("line:%d:get %s partition info fail\n", __LINE__, name);
		return -1;
	}

	printf("partinfo: name %s, start 0x%lx, size 0x%lx\n", info.name,
	       info.start, info.size);
	start_block  = info.start;
	part_sectors = info.size;

	/* detect the frp partition */
	start_block = info.start;
	if (!start_block) {
		printf("cant find part named frp\n");
	} else {
		part_sectors = info.size;
#ifdef DEBUG
		printf("start block = 0x%x, part_sectors = %d\n", start_block,
		       part_sectors);
#endif
		/*read the last block of frp part to addr[]*/
		ret = sunxi_flash_read(start_block + part_sectors - 1, 1, addr);

		printf("sunxi flash read :offset %x, %d bytes %s\n",
		       ((start_block + part_sectors - 1) << 9),
		       ((part_sectors - 1023) << 9), ret ? "OK" : "ERROR");
	}

	return addr[511];
}

static int oem_lock_will_brick_device(char *response_to_pc)
{
#ifndef CONFIG_SUNXI_AVB
	/*only vbmeta part will cause this problem*/
	return 0;
#else
	uint8_t *vb_meta_data;
	size_t vb_len;
	if (sunxi_avb_read_vbmeta_data(&vb_meta_data, &vb_len) == 0) {
		int ret;
		sunxi_certif_info_t sub_certif;

		ret = sunxi_vbmeta_self_verify(vb_meta_data, vb_len,
					       &sub_certif.pubkey);
		free(vb_meta_data);
		if (ret == -1) {
			uint32_t vbmeta_flags;
			sunxi_avb_get_vbmeta_flags(&vbmeta_flags);
			if (vbmeta_flags &
			    AVB_VBMETA_IMAGE_FLAGS_HASHTREE_DISABLED) {
				const char *info =
					"vbmeta modified, try adb enable-verity before oem lock";
				pr_force("%s\n", info);
				strcpy(response_to_pc, info);
				return 1;
			}
		}
	} else {
		/*just skip the check if read vbmeta fail*/
	}
	return 0;
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
static void __oem_operation(char *operation)
{
	char response[68];
	char lock_info[64];
	int lockflag;
	int ret;

	memset(lock_info, 0, 64);
	memset(response, 0, 68);

	lockflag = 0;

	if (!strncmp(operation, "lock", 4)) {
		if ((gd->securemode == SUNXI_SECURE_MODE_WITH_SECUREOS) ||
		    (gd->securemode == SUNXI_SECURE_MODE_NO_SECUREOS)) {
			printf("the system is secure\n");
			if (oem_lock_will_brick_device(&response[4])) {
				const char *info =
					"device will failed to boot after oem lock! Abort oem lock";
				pr_force("%s\n", info);
				if (strlen(&response[4]) == 0) {
					strcpy(&response[4], info);
				}
				memcpy(response, "FAIL", 4);
				goto return_without_lock_op;
			} else {
				lockflag = SUNXI_LOCKED;
				sunxi_fastboot_write_status_flag("locked");
			}
		} else {
			printf("the system is normal\n");
		}
	} else if (!strncmp(operation, "unlock", 6)) {
		if ((gd->securemode == SUNXI_SECURE_MODE_WITH_SECUREOS) ||
		    (gd->securemode == SUNXI_SECURE_MODE_NO_SECUREOS)) {
			printf("the system is secure\n");
			if (sunxi_read_oem_unlock_ability() == 0x00) {
				__limited_fastboot_unlock();
				return;
			}
			lockflag = SUNXI_UNLOCKED;
			sunxi_fastboot_write_status_flag("unlocked");
		} else {
			printf("the system is normal\n");
		}
	} else {
		if (!strncmp(operation, "efex", 4)) {
			strcpy(response, "OKAY");
			__sunxi_fastboot_send_status(response,
						     strlen(response));
#ifndef FPGA_PLATFORM
			sunxi_board_run_fel();
#else
			printf("FPGA NOT EXIT EFEX\n");
#endif
		} else {
			const char *info =
				"fastboot oem operation fail: unknown cmd";

			printf("%s\n", info);
			strcpy(response, "FAIL");
			strcat(response, info);

			__sunxi_fastboot_send_status(response,
						     strlen(response));
		}

		return;
	}

	/* erase user data and set the gd->lockflag  */
	ret = sunxi_oem_op_lock(lockflag, lock_info);
	if (!ret) {
		strcpy(response, "OKAY");
	} else {
		strcpy(response, "FAIL");
	}
	strcat(response, lock_info);
	printf("%s\n", response);

return_without_lock_op:
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
static void __continue(void)
{
	char response[32];
	int storage_type;

	storage_type = get_boot_storage_type();
	memset(response, 0, 32);
	strcpy(response, "OKAY");

	__sunxi_fastboot_send_status(response, strlen(response));

	sunxi_usb_exit();

	if (storage_type == STORAGE_EMMC || storage_type == STORAGE_SD
			|| storage_type == STORAGE_EMMC0) {
		env_set("bootcmd", "run setargs_mmc boot_normal");
	} else if (storage_type == STORAGE_NAND) {
		env_set("bootcmd", "run setargs_nand boot_normal");
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
	strcpy(response, "FAIL");

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
	sunxi_usb_fastboot_status       = SUNXI_USB_FASTBOOT_IDLE;

	all_download_bytes = 0;
	fastboot_data_flag = 0;

	trans_data.base_recv_buffer = (char *)FASTBOOT_TRANSFER_BUFFER;

	trans_data.base_send_buffer =
		(char *)malloc(SUNXI_FASTBOOT_SEND_MEM_SIZE);
	if (!trans_data.base_send_buffer) {
		printf("sunxi usb fastboot err: unable to malloc memory for fastboot send\n");
		free(trans_data.base_recv_buffer);

		return -1;
	}
	printf("recv addr 0x%lx\n", (ulong)trans_data.base_recv_buffer);
	printf("send addr 0x%lx\n", (ulong)trans_data.base_send_buffer);
	printf("start to display fastbootlogo.bmp\n");
#if defined(CONFIG_CMD_SUNXI_BMP) || defined(CONFIG_SUNXI_TV_FASTLOGO) || defined(CONFIG_EINK200_SUNXI)
	sunxi_bmp_display("fastbootlogo.bmp");
#endif
	char *p = NULL;
	p       = env_get("snum");
	if (p) {
		strncpy(sunxi_usb_fastboot_dev[2], p, 24);
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
static int sunxi_fastboot_exit(void)
{
	sunxi_flash_write_end();
	sunxi_flash_flush();
	printf("sunxi_fastboot_exit\n");
	if (trans_data.base_send_buffer) {
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
	sunxi_usb_fastboot_status       = SUNXI_USB_FASTBOOT_IDLE;
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
static void sunxi_fastboot_usb_rx_dma_isr(void *p_arg)
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
static void sunxi_fastboot_usb_tx_dma_isr(void *p_arg)
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
static int sunxi_fastboot_standard_req_op(uint cmd,
					  struct usb_device_request *req,
					  uchar *buffer)
{
	int ret = SUNXI_USB_REQ_OP_ERR;

	switch (cmd) {
	case USB_REQ_GET_STATUS: {
		ret = __usb_get_status(req, buffer);

		break;
	}
	//case USB_REQ_CLEAR_FEATURE:
	//case USB_REQ_SET_FEATURE:
	case USB_REQ_SET_ADDRESS: {
		ret = __usb_set_address(req);

		break;
	}
	case USB_REQ_GET_DESCRIPTOR:
	//case USB_REQ_SET_DESCRIPTOR:
	case USB_REQ_GET_CONFIGURATION: {
		ret = __usb_get_descriptor(req, buffer);

		break;
	}
	case USB_REQ_SET_CONFIGURATION: {
		ret = __usb_set_configuration(req);

		break;
	}
	//case USB_REQ_GET_INTERFACE:
	case USB_REQ_SET_INTERFACE: {
		ret = __usb_set_interface(req);

		break;
	}
	//case USB_REQ_SYNCH_FRAME:
	default: {
		printf("sunxi fastboot error: standard req is not supported\n");

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
static int sunxi_fastboot_nonstandard_req_op(uint cmd,
					     struct usb_device_request *req,
					     uchar *buffer, uint data_status)
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
	printf("secure mode,fastboot limited used\n");

	strcpy(response, "FAIL:secure mode,fastboot limited used");

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
	int d_data_len;
	int data_len;
	int ret;

	/* read locked or unlocked flag in secure storage */
	memset(buffer, 0x00, sizeof(buffer));
	memset(d_buffer, 0x00, sizeof(d_buffer));
	if (sunxi_secure_storage_init()) {
		pr_err("secure storage init fail\n");
	} else {
		ret = sunxi_secure_object_read("device_unlock", d_buffer, 32,
					       &d_data_len);
		ret = sunxi_secure_object_read("fastboot_status_flag", buffer,
					       512, &data_len);

		if ((!ret) && (!strcmp(buffer, "locked")) &&
		    (!strcmp(d_buffer, "unlock"))) {
			env_set("verifiedbootstate", "yellow");
		}

		if (ret) {
			pr_msg("sunxi secure storage has no flag\n");
			gd->lockflag = SUNXI_LOCKED;
		} else {
			if (!strcmp(buffer, "unlocked")) {
				pr_force("find fastboot unlock flag\n");
				gd->lockflag = SUNXI_UNLOCKED;
				return -1;
			} else if (!strcmp(buffer, "locked")) {
				pr_force("find fastboot locked flag\n");
				gd->lockflag = SUNXI_LOCKED;
				return 0;
			}
			pr_force("do not find fastboot status flag\n");
		}
	}
	return 0;
}

static int sunxi_fastboot_status(void)
{
	if ((gd->securemode == SUNXI_SECURE_MODE_WITH_SECUREOS) ||
	    (gd->securemode == SUNXI_SECURE_MODE_NO_SECUREOS)) {
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
int temp = 100;
static int sunxi_fastboot_state_loop(void *buffer)
{
	int ret;
	sunxi_ubuf_t *sunxi_ubuf = (sunxi_ubuf_t *)buffer;
	char response[68];
	if (sunxi_usb_fastboot_status != temp) {
		printf("sunxi_usb_fastboot_status=%d\n",
		       sunxi_usb_fastboot_status);
		temp = sunxi_usb_fastboot_status;
	}

	switch (sunxi_usb_fastboot_status) {
	case SUNXI_USB_FASTBOOT_IDLE:
		if (sunxi_ubuf->rx_ready_for_data == 1) {
			sunxi_usb_fastboot_status = SUNXI_USB_FASTBOOT_SETUP;
		}

		break;

	case SUNXI_USB_FASTBOOT_SETUP:

		printf("SUNXI_USB_FASTBOOT_SETUP\n");

		printf("fastboot command = %s\n", sunxi_ubuf->rx_req_buffer);

		sunxi_usb_fastboot_status     = SUNXI_USB_FASTBOOT_IDLE;
		sunxi_ubuf->rx_ready_for_data = 0;
		if (memcmp(sunxi_ubuf->rx_req_buffer, "reboot-bootloader",
			   strlen("reboot-bootloader")) == 0) {
			printf("reboot-bootloader\n");
			__fastboot_reboot(SUNXI_FASTBOOT_FLAG);
		} else if (memcmp(sunxi_ubuf->rx_req_buffer, "reboot-fastboot", strlen("reboot-fastboot")) == 0) {
				tick_printf("reboot-fastboot\n");
				u32 misc_offset = sunxi_partition_get_offset_byname("misc");
				char  misc_args[2048] = {0};
				struct bootloader_message *misc_info;
				if (!misc_offset) {
					pr_error("no misc partition is found\n");
					break;
				} else {
					sunxi_flash_read(misc_offset, 2048/512, misc_args);
				}
				misc_info = (struct bootloader_message *)misc_args;
				memset(misc_info->recovery, 0, sizeof(misc_info->recovery));
				memcpy(misc_info->recovery, "recovery\n--fastboot", strlen("recovery\n--fastboot"));
				sunxi_flash_write(misc_offset, 2048/512, misc_args);
				__fastboot_reboot(SUNXI_BOOT_RECOVERY_FLAG);
		} else if (memcmp(sunxi_ubuf->rx_req_buffer, "reboot", 6) ==
			   0) {
			printf("reboot\n");
			__fastboot_reboot(0);
		} else if (memcmp(sunxi_ubuf->rx_req_buffer, "erase:", 6) ==
			   0) {
			printf("erase\n");
			if (!sunxi_fastboot_status()) {
				__limited_fastboot();
				break;
			}
			__erase_part((char *)(sunxi_ubuf->rx_req_buffer + 6));
		} else if (memcmp(sunxi_ubuf->rx_req_buffer, "flash:", 6) ==
			   0) {
			printf("flash\n");
			if (!sunxi_fastboot_status()) {
				__limited_fastboot();
				break;
			}
#ifdef CONFIG_SUNXI_FASTBOOT_UBOOT
			if (!memcmp((char *)(sunxi_ubuf->rx_req_buffer + 6),
				    "u-boot", 6) ||
			    !memcmp((char *)(sunxi_ubuf->rx_req_buffer + 6),
				    "toc1", 4)) {
				__flash_to_uboot();
			} else
#endif
#ifdef CONFIG_SUNXI_FASTBOOT_BOOT0
				if (!memcmp((char *)(sunxi_ubuf->rx_req_buffer +
						     6),
					    "boot0", 5) ||
				    !memcmp((char *)(sunxi_ubuf->rx_req_buffer +
						     6),
					    "toc0", 4)) {
				__flash_to_boot0();
			} else
#endif
#ifdef CONFIG_SUNXI_FASTBOOT_MBR
				if (!memcmp((char *)(sunxi_ubuf->rx_req_buffer +
						     6),
					    "mbr", 3)) {
				__flash_to_mbr();
			} else
#endif
				__flash_to_part((
					char *)(sunxi_ubuf->rx_req_buffer + 6));
		} else if (memcmp(sunxi_ubuf->rx_req_buffer, "download:", 9) ==
			   0) {
			printf("download\n");
			if (!sunxi_fastboot_status()) {
				__limited_fastboot();
				break;
			}
			ret = __try_to_download(
				(char *)(sunxi_ubuf->rx_req_buffer + 9),
				response);
			if (ret >= 0) {
				fastboot_data_flag = 1;
				sunxi_ubuf->rx_req_buffer =
					(uchar *)trans_data.base_recv_buffer;
				sunxi_usb_fastboot_status =
					SUNXI_USB_FASTBOOT_RECEIVE_DATA;
			}
			__sunxi_fastboot_send_status(response,
						     strlen(response));
		} else if (memcmp(sunxi_ubuf->rx_req_buffer, "boot", 4) == 0) {
			printf("boot\n");
			if (!sunxi_fastboot_status()) {
				__limited_fastboot();
				break;
			}
			__boot();
		} else if (memcmp(sunxi_ubuf->rx_req_buffer, "getvar:", 7) ==
			   0) {
			printf("getvar\n");
			if (!sunxi_fastboot_status()) {
				__limited_fastboot();
				break;
			}
			__get_var((char *)(sunxi_ubuf->rx_req_buffer + 7));
		} else if ((memcmp(sunxi_ubuf->rx_req_buffer, "oem", 3) == 0) ||
			(memcmp(sunxi_ubuf->rx_req_buffer, "flashing", 8) == 0)) {
			printf("oem operations\n");
			__oem_operation(
				(char *)(sunxi_ubuf->rx_req_buffer + 4));
		} else if (memcmp(sunxi_ubuf->rx_req_buffer, "continue", 8) ==
			   0) {
			printf("continue\n");
			__continue();
		} else {
			printf("not supported fastboot cmd\n");
			__unsupported_cmd();
		}

		break;

	case SUNXI_USB_FASTBOOT_SEND_DATA:

		printf("SUNXI_USB_FASTBOOT_SEND_DATA\n");

		break;

	case SUNXI_USB_FASTBOOT_RECEIVE_DATA:

		//printf("SUNXI_USB_FASTBOOT_RECEIVE_DATA\n");
		if ((fastboot_data_flag == 1) &&
		    ((char *)sunxi_ubuf->rx_req_buffer ==
		     all_download_bytes +
			     trans_data.base_recv_buffer)) {
			//传输完毕
			printf("fastboot transfer finish\n");
			fastboot_data_flag	= 0;
			sunxi_usb_fastboot_status = SUNXI_USB_FASTBOOT_IDLE;

			sunxi_ubuf->rx_req_buffer = sunxi_ubuf->rx_base_buffer;

			sprintf(response, "OKAY");
			__sunxi_fastboot_send_status(response,
						     strlen(response));
		}

		break;

	default:
		break;
	}

	return 0;
}

sunxi_usb_module_init(SUNXI_USB_DEVICE_FASTBOOT, sunxi_fastboot_init,
		      sunxi_fastboot_exit, sunxi_fastboot_reset,
		      sunxi_fastboot_standard_req_op,
		      sunxi_fastboot_nonstandard_req_op,
		      sunxi_fastboot_state_loop, sunxi_fastboot_usb_rx_dma_isr,
		      sunxi_fastboot_usb_tx_dma_isr);
