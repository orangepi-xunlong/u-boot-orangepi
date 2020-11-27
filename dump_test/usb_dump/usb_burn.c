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
#include <boot_type.h>
#include "usb_burn.h"
#include "usb_efex.h"
#include <smc.h>
#include <securestorage.h>
#include <sunxi_board.h>
#include "key_deal.h"
#include <smc.h>
#include "../sprite/sprite_verify.h"
DECLARE_GLOBAL_DATA_PTR;
volatile int sunxi_usb_burn_from_boot_handshake, sunxi_usb_burn_from_boot_init, sunxi_usb_burn_from_boot_setup;


__attribute__((weak))
int smc_set_sst_crypt_name(char *name)
{
	printf("call weak fun: %s\n",__func__);
	return 0;
}

__attribute__((weak))
int sunxi_verify_rotpk_hash(void *input_hash_buf, int len)
{
	printf("call weak fun: %s\n",__func__);
	return -1;
}

__attribute__((weak))
int sunxi_deal_hdcp_key(char *keydata, int keylen)
{
	printf("call weak fun: %s\n",__func__);
	return 0;
}

__attribute__((weak))
int sunxi_secure_object_down( const char *name , char *buf, int len, int encrypt, int write_protect)

{
	printf("call weak fun: %s\n",__func__);
	return 0;
}

__attribute__((weak))
int sunxi_efuse_write(void *key_buf)
{
	printf("call weak fun: %s\n",__func__);
	return -1;
}

__attribute__((weak))
int sunxi_efuse_read(void *key_name, void *read_buf,int *len)
{
	printf("call weak fun: %s\n",__func__);
	return -1;
}

static  int sunxi_usb_pburn_write_enable = 0;
#if defined(SUNXI_USB_30)
static  int sunxi_usb_pburn_status_enable = 1;
#endif
static  int sunxi_usb_pburn_status = SUNXI_USB_PBURN_IDLE;

static  pburn_trans_set_t  trans_data;
#define DATA_BUFFER_MAX_SIZE					(1*1024*1024)

static  u8 *private_data_ext_buff_step = NULL;
static u8 *private_data_ext_buff = NULL;

static int burn_part_result_state = ERR_NO_SUCCESS;
long long burn_part_bytes = 0;
extern volatile int sunxi_usb_burn_from_boot_handshake;
extern volatile int sunxi_usb_burn_from_boot_setup;
static uint burn_private_start, burn_private_len;
static uint burn_part_start, burn_part_len;
extern int sunxi_get_securemode(void);
#ifdef CONFIG_BURN_NORMAL_EFUSE
extern int sunxi_efuse_write(void *key_buf);
extern int  sunxi_efuse_read(void *key_name, void *read_buf,int *len);
#endif

static int hex2char(void *dst, void *src,int len)
{
	int i;
	char *p_out = dst ,*p_in = src;

	if ((src ==NULL)||(dst == NULL))
		return -1;

	memset(dst,0x00,len*2);
	for(i=0;i<len;i++)
	{
		sprintf(p_out,"%02x",p_in[i]);
		p_out += 2;
	}

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

			qua_dscrpt->bLength = MIN(req->wLength, sizeof(sizeof(struct usb_qualifier_descriptor)));
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
static int __sunxi_pburn_send_status(void *buffer, unsigned int buffer_size)
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
int __sunxi_burn_key(u8 *buff, uint buff_len)
{
	sunxi_usb_burn_main_info_t *key_main = (sunxi_usb_burn_main_info_t *)buff;
	sunxi_usb_burn_key_info_t  *key_list;
	__attribute__((unused)) int ret;
	sunxi_efuse_key_info_t      efuse_key_info;

	int key_count;
	int offset;
	u8  *p_buff = (u8 *)&key_main->key_info;
	//比较key主体的信息
	if(strcmp((const char *)key_main->magic, "key-group-db"))
	{
		printf("key data magic unmatch, err\n");

		return -1;
	}
	key_count = key_main->count;
	printf("key_count=%d\n", key_count);
#ifdef CONFIG_SUNXI_SECURE_STORAGE
	if(sunxi_secure_storage_init())
	{
		printf("%s secure storage init failed\n", __func__);

		return -1;
	}
#endif
	for(;key_count>0;key_count--, key_list++)
	{
		key_list = (sunxi_usb_burn_key_info_t *)p_buff;
		printf("^^^^^^^^^^^^^^^^^^^\n");
		printf("key index=%d\n", key_main->count - key_count);
		printf("key name=%s\n", key_list->name);
		printf("key type=%d\n", key_list->type);
		printf("key len=%d\n", key_list->len);
		printf("key if_burn=%d\n", key_list->if_burn);
		printf("key if_replace=%d\n", key_list->if_replace);
		printf("key if_crypt=%d\n", key_list->if_crypt);
		printf("key data:\n");
		sunxi_dump(key_list->key_data, key_list->len);
		printf("###################\n");
		offset = (sizeof(sunxi_usb_burn_key_info_t)) + ((key_list->len + 15) & (~15));
		printf("offset=%d\n", offset);
		p_buff += offset;

		if(!key_list->type)
		{
			memset(&efuse_key_info, 0, sizeof(efuse_key_info));
			strcpy(efuse_key_info.name, (const char *)key_list->name);
			efuse_key_info.len = key_list->len;
			efuse_key_info.key_data = (u8 *)key_list->key_data;

			if(!strcmp(efuse_key_info.name, "rotpk"))
			{
				printf("ready to burn rootkey\n");
				if(sunxi_verify_rotpk_hash(key_list->key_data, key_list->len))
				{
					printf("verity the rootkey failed, stop burn it\n");

					return -1;
				}
			}

			if (sunxi_get_securemode() == SUNXI_NORMAL_MODE)
			{
				if (sunxi_efuse_write(&efuse_key_info))
				{
					return -1;
				}
			}
			else
			{
				if(arm_svc_efuse_write(&efuse_key_info))
				{
					return -1;
				}
			}
		}
		else
		{
			if(!strcmp("hdcpkey", key_list->name))
			{
				ret = sunxi_deal_hdcp_key((char *)key_list->key_data, key_list->len);
				if(ret)
				{
					printf("sunxi deal with hdcp key failed\n");
					return -1;
				}
			}else if(!strcmp("widevine", key_list->name))
			{
				ret = sunxi_secure_object_down("widevine", (char *)key_list->key_data,key_list->len,
					key_list->if_crypt,key_list->if_replace);
				if(ret)
				{
					printf("sunxi deal with widevine key failed\n");
					return -1;
				}
			}
			else
			{
#ifdef CONFIG_SUNXI_SECURE_STORAGE
				sunxi_secure_object_set(key_list->name,
										key_list->if_crypt,
										key_list->if_replace,
										0,0,0 ); /*set secure storage feature*/

				printf("write key to secure storage\n");
				ret = sunxi_secure_object_write(key_list->name, (char *)key_list->key_data, key_list->len);
				if(ret)
				{
					printf("write key to secure storage failed\n");
					return -1;
				}
#endif

#ifdef CONFIG_SUNXI_PRIVATE_KEY
                printf("write key to private partition\n");
				ret = save_user_private_data(key_list->name, (char *)key_list->key_data, key_list->len);
				if(ret < 0)
				{
					printf("write key to private failed\n");
					return -1;
				}
				else if(ret == 1)	//烧写的key数据长度过大,无法存储在private分区
				{
					printf("the fuck thing had happened\n");
				}
#endif
			}
		}
	}
#ifdef CONFIG_SUNXI_SECURE_STORAGE
	if(sunxi_secure_storage_exit())
	{
		printf("sunxi_secure_storage_exit err\n");

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
*    name          :	__sunxi_read_key_by_name
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
int __sunxi_read_key_by_name(void *buffer, uint buff_len, int *read_len)
{
	sunxi_usb_burn_key_info_t *key_info = NULL;

	__attribute__((unused)) int ret = -1;
	__attribute__((unused)) int key_data_len = 0;
	char data_buff[4096]={0};

	if(buffer == NULL)
	{
		printf("the memory input or output is NULL\n");
		return -1;
	}
#ifdef CONFIG_SUNXI_SECURE_STORAGE
	if(sunxi_secure_storage_init())
	{
		printf("%s secure storage init failed\n", __func__);
		return -1;
	}
#endif

	key_info = (sunxi_usb_burn_key_info_t *)buffer;

	printf("key name=%s\n", key_info->name);
	printf("key type=%d\n", key_info->type);
	printf("key len=%d\n", key_info->len);
	printf("key if_burn=%d\n", key_info->if_burn);
	printf("key if_replace=%d\n", key_info->if_replace);
	printf("key if_crypt=%d\n", key_info->if_crypt);

	*read_len = sizeof(sunxi_usb_burn_key_info_t);

	if(!key_info->type)
	{
		if ((gd->securemode == SUNXI_SECURE_MODE_WITH_SECUREOS) || (gd->securemode == SUNXI_SECURE_MODE_NO_SECUREOS))
		{
			key_data_len = arm_svc_efuse_read(key_info->name, (void *)data_buff);
			if (key_data_len <= 0)
			{
				printf("efuse read %s err\n", key_info->name);
				return -1;
			}
		}
		else
		{
			if (sunxi_efuse_read(key_info->name, (void *)data_buff,&key_data_len))
			{
				printf("efuse read %s err\n", key_info->name);
				return -1;
			}
		}

		/* convert hex key to char,so that dragonkey tool can display it */
		hex2char((void *)(key_info->key_data), (void *)(data_buff), key_data_len);
		*read_len +=key_data_len*2;
	}
	else
	{
#ifdef CONFIG_SUNXI_SECURE_STORAGE
		printf("read key form securestorage\n");
		ret = sunxi_secure_object_read(key_info->name, data_buff, sizeof(sunxi_usb_burn_key_info_t), &key_data_len);
		if(ret < 0)
		{
			printf("read %s form securestorage failed\n", key_info->name);
		}
#endif
#ifdef CONFIG_SUNXI_PRIVATE_KEY
		printf("read key form private\n");
		memset(data_buff, 0, 4096);
		ret = read_private_key_by_name(key_info->name, data_buff, sizeof(sunxi_usb_burn_key_info_t), &key_data_len);
		if(ret < 0)
		{
			printf("read %s form private failed\n", key_info->name);
			return -1;
		}
#else
			return -1;
#endif

		//组装读取出来的key数据
		key_info->len = key_data_len;
	    memcpy((void *)(key_info->key_data), (void *)data_buff, key_data_len);
	    *read_len += key_data_len;
	}
	return 0;
}

/*
************************************************************************************************************
*
*                                             function
*
*    name          :	__sunxi_read_key_by_name
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
int __sunxi_erase_key(void)
{
	 __attribute__((unused)) int ret;
#ifdef CONFIG_SUNXI_SECURE_STORAGE
	ret = sunxi_secure_storage_init();
	if(ret < 0)
	{
		printf("secure storage init err\n");
		return -1;
	}

	ret = sunxi_secure_storage_erase_all();
	if(ret < 0)
	{
		printf("erase secure storage failed\n");
		return -1;
	}

	printf("erase securestorage success\n");
	sunxi_secure_storage_exit();
#endif

#ifdef CONFIG_SUNXI_PRIVATE_KEY
	ret = erase_all_private_data();
	if(ret < 0)
	{
		printf("erase private data failed\n");
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
*    name          :	__sunxi_read_key_by_name
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
int __sunxi_burn_flag(void)
{
	int ret;
#ifdef CONFIG_SUNXI_SECURE_STORAGE
	ret = sunxi_secure_storage_init();
	if(ret < 0)
	{
		printf("secure storage init err\n");
		return -1;
	}
	if(sunxi_secure_object_write("key_burned_flag", "key_burned", strlen("key_burned")))
	{
		printf("save burned flag to securestorage failed\n");
		return -1;
	}

	printf("save burned flag to securestorage success\n");
	sunxi_secure_storage_exit();

#else

	char buffer[512];
	uint private_start, private_len;

	private_start = sunxi_partition_get_offset_byname("private");
	private_len   = sunxi_partition_get_size_byname("private");

	if((!private_start) || (!private_len))
	{
		printf("private no exist\n");
		return -1;
	}

	memset(buffer, 0, 512);

	strcpy(buffer, "key_burned");
	ret = sunxi_flash_write(private_start + private_len - (8192+512)/512, 1, buffer);
	if(!ret)
	{
		printf("save burned flag to private failed\n");
		return -1;
	}
	sunxi_flash_flush();
	printf("save burned flag to private success\n");
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
int sunxi_burn_part_verify(void *veryfy_info, long long burn_bytes)
{
	long long base_bytes = 0;
	uint active_verify = 0;
	uint origin_verify = 0;
	uint part_start;
	pburn_verify_part_set_t *temp_info = NULL;

	temp_info = (pburn_verify_part_set_t *)veryfy_info;

	part_start = sunxi_partition_get_offset_byname(temp_info->name);
	if(!part_start)
	{
		printf("partition %s is not exist\n", temp_info->name);
		return -1;
	}

	base_bytes = burn_bytes;
	origin_verify = temp_info->check_sum;
	active_verify = sunxi_sprite_part_rawdata_verify(part_start, base_bytes);
	printf("origin checksum=%x, active checksum=%x\n", origin_verify, active_verify);
	if(active_verify != origin_verify)
	{
	    printf("sunxi burn: part %s verify error\n", temp_info->name);
		return -1;
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
int burn_part_probe(void *burn_part_info)
{
	pburn_partition_set_t *part_info = NULL;
	int burn_state = 0;

	part_info = (pburn_partition_set_t *)(burn_part_info);
	if(!strcmp(part_info->magic, "usbburnpart"))
	{
		burn_part_start = sunxi_partition_get_offset_byname(part_info->name);
		burn_part_len   = sunxi_partition_get_size_byname(part_info->name);
		if(!burn_part_start)
		{
			printf("partition %s is not exist\n", part_info->name);
			burn_state = ERR_NO_PART_NOEXIST;
		}
		else if(((!burn_part_len) || (burn_part_len < ((part_info->lenlo + 511))/512)) || (part_info->lenlo == 0))
		{
			printf("the burn data size 0x%x is large then 0x%x\n", part_info->lenlo, burn_part_len);
			burn_state = ERR_NO_PART_SIZE_ERR;
		}
		else
		{
			printf("part info check success\n");
			printf("part_info->lenhi=%d\n", part_info->lenhi);
			printf("part_info->lenlo=%d\n", part_info->lenlo);
			burn_part_bytes = part_info->lenhi;
			burn_part_bytes = (burn_part_bytes << 32) + part_info->lenlo;
			burn_state = ERR_NO_SUCCESS;
		}
	}
	else
	{
		printf("part magic %s need equal usbburnpart\n", part_info->magic);
		burn_state = ERR_NO_PART_MAGIC_ERR;
	}
	return burn_state;
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
static int sunxi_pburn_init(void)
{
	sunxi_usb_dbg("sunxi_pburn_init\n");
	memset(&trans_data, 0, sizeof(pburn_trans_set_t));
	sunxi_usb_pburn_write_enable = 0;
    sunxi_usb_pburn_status = SUNXI_USB_PBURN_IDLE;

    trans_data.base_recv_buffer = (u8 *)malloc(SUNXI_PBURN_RECV_MEM_SIZE);
    if(!trans_data.base_recv_buffer)
    {
    	printf("sunxi usb pburn err: unable to malloc memory for pburn receive\n");

    	return -1;
    }

	trans_data.base_send_buffer = (u8 *)malloc(SUNXI_PBURN_SEND_MEM_SIZE);
    if(!trans_data.base_send_buffer)
    {
    	printf("sunxi usb pburn err: unable to malloc memory for pburn send\n");
    	free(trans_data.base_recv_buffer);

    	return -1;
    }
	sunxi_usb_dbg("recv addr 0x%x\n", (uint)trans_data.base_recv_buffer);
    sunxi_usb_dbg("send addr 0x%x\n", (uint)trans_data.base_send_buffer);

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
static int sunxi_pburn_exit(void)
{
	sunxi_usb_dbg("sunxi_pburn_exit\n");
    if(trans_data.base_recv_buffer)
    {
    	free(trans_data.base_recv_buffer);
		trans_data.base_recv_buffer = NULL;
    }
    if(trans_data.base_send_buffer)
    {
    	free(trans_data.base_send_buffer);
		trans_data.base_send_buffer = NULL;
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
static void sunxi_pburn_reset(void)
{
	sunxi_usb_pburn_write_enable = 0;
    sunxi_usb_pburn_status = SUNXI_USB_PBURN_IDLE;
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
static void  sunxi_pburn_usb_rx_dma_isr(void *p_arg)
{
	sunxi_usb_dbg("dma int for usb rx occur\n");
	//通知主循环，可以写入数据
	sunxi_usb_pburn_write_enable = 1;
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
static void  sunxi_pburn_usb_tx_dma_isr(void *p_arg)
{
	sunxi_usb_dbg("dma int for usb tx occur\n");
#if defined(SUNXI_USB_30)
	sunxi_usb_pburn_status_enable = 1;
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
static int sunxi_pburn_standard_req_op(uint cmd, struct usb_device_request *req, uchar *buffer)
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
			printf("sunxi pburn error: standard req is not supported\n");

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
static int sunxi_pburn_nonstandard_req_op(uint cmd, struct usb_device_request *req, uchar *buffer, uint data_status)
{
	int ret = SUNXI_USB_REQ_SUCCESSED;

	switch(req->bmRequestType)		//PBURN 特有请求
	{
		case 161:
			if(req->bRequest == 0xFE)
			{
				sunxi_usb_dbg("pburn ask for max lun\n");

				buffer[0] = 0;

				sunxi_udc_send_setup(1, buffer);
			}
			else
			{
				printf("sunxi usb err: unknown ep0 req in pburn\n");

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
static int sunxi_pburn_state_loop(void  *buffer)
{
	static struct umass_bbb_cbw_t  *cbw;
	static struct umass_bbb_csw_t  csw;
	static uint pburn_flash_start = 0;
	static uint pburn_flash_sectors = 0;
	int    ret;
	sunxi_ubuf_t *sunxi_ubuf = (sunxi_ubuf_t *)buffer;

	switch(sunxi_usb_pburn_status)
	{
		case SUNXI_USB_PBURN_IDLE:
			if(sunxi_ubuf->rx_ready_for_data == 1)
			{
				sunxi_usb_pburn_status = SUNXI_USB_PBURN_SETUP;
			}

			break;

		case SUNXI_USB_PBURN_SETUP:

			sunxi_usb_dbg("SUNXI_USB_PBURN_SETUP\n");

			if(sunxi_ubuf->rx_req_length != sizeof(struct umass_bbb_cbw_t))
			{
				printf("sunxi usb error: received bytes 0x%x is not equal cbw struct size 0x%x\n", sunxi_ubuf->rx_req_length, sizeof(struct umass_bbb_cbw_t));

				sunxi_ubuf->rx_ready_for_data = 0;
				sunxi_usb_pburn_status = SUNXI_USB_PBURN_IDLE;

				break;
			}

			cbw = (struct umass_bbb_cbw_t *)sunxi_ubuf->rx_req_buffer;
			if(CBWSIGNATURE != cbw->dCBWSignature)
			{
				printf("sunxi usb error: the cbw signature 0x%x is bad, need 0x%x\n", cbw->dCBWSignature, CBWSIGNATURE);

				sunxi_ubuf->rx_ready_for_data = 0;
				sunxi_usb_pburn_status = SUNXI_USB_PBURN_IDLE;

				break;
			}

			csw.dCSWSignature = CSWSIGNATURE;
			csw.dCSWTag 	  = cbw->dCBWTag;

#if defined(SUNXI_USB_30)
			sunxi_usb_pburn_status_enable = 1;
#endif
			sunxi_usb_dbg("usb cbw command = 0x%x\n", cbw->CBWCDB[0]);

			switch(cbw->CBWCDB[0])
	  		{
				case 0xf0:			//自定义命令，用于烧录用户数据
	  				sunxi_usb_dbg("usb burn secure storage data\n");
	  				printf("usb command = %d\n", cbw->CBWCDB[1]);
	  				switch(cbw->CBWCDB[1])
	  				{
	  					case 0:				//握手
	  					{
	  						__usb_handshake_sec_t  *handshake = (__usb_handshake_sec_t *)trans_data.base_send_buffer;

                            memset(handshake, 0, sizeof(__usb_handshake_sec_t));
							strcpy(handshake->magic, "usb_burn_handshake");
							sunxi_usb_pburn_status = SUNXI_USB_PBURN_SEND_DATA;

		  					trans_data.act_send_buffer = trans_data.base_send_buffer;
		  					trans_data.send_size = min(cbw->dCBWDataTransferLength, sizeof(__usb_handshake_sec_t));

							sunxi_usb_burn_from_boot_setup = 1;

							if(private_data_ext_buff == NULL)
							{
								printf("malloc memory\n");
								private_data_ext_buff = (u8 *)malloc(DATA_BUFFER_MAX_SIZE);
								if(private_data_ext_buff == NULL)
								{
									printf("there is no memory to store all user key data\n");
									csw.bCSWStatus = -1;
								}
								else
			  					{
			  						csw.bCSWStatus = 0;
			  						sunxi_usb_burn_from_boot_handshake = 1;
			  					}
							}
							else
		  					{
		  						csw.bCSWStatus = 0;
		  						sunxi_usb_burn_from_boot_handshake = 1;
		  					}
		  					private_data_ext_buff_step = private_data_ext_buff;
						}
						break;

						case 1:				//小机端接收数据
						{
							trans_data.recv_size = cbw->dCBWDataTransferLength;

                            sunxi_usb_pburn_write_enable = 0;

                            memset(private_data_ext_buff, 0, DATA_BUFFER_MAX_SIZE);
							sunxi_udc_start_recv_by_dma(private_data_ext_buff_step, trans_data.recv_size);	//start dma to receive data

							printf("recv_size=%d\n", trans_data.recv_size);
							sunxi_dump(private_data_ext_buff, trans_data.recv_size);

					        sunxi_usb_pburn_status = SUNXI_USB_PBURN_RECEIVE_NULL;
						}
						break;

						case 2:             //工具端声明数据传输已经完毕，要求获取烧录状态
						{
							__usb_handshake_ext_t  *handshake = (__usb_handshake_ext_t *)trans_data.base_send_buffer;

							memset(handshake, 0, sizeof(__usb_handshake_ext_t));
							//strcpy(handshake->magic, "usb_burn_receive_data_all");
							sunxi_usb_pburn_status = SUNXI_USB_PBURN_SEND_DATA;

							printf("recv_size=%d\n", trans_data.recv_size);
							sunxi_dump(private_data_ext_buff, trans_data.recv_size);

							int ret = __sunxi_burn_key(private_data_ext_buff, trans_data.recv_size);

		  					trans_data.act_send_buffer = trans_data.base_send_buffer;
		  					trans_data.send_size = min(cbw->dCBWDataTransferLength, sizeof(__usb_handshake_ext_t));
		  					//开始根据数据类型进行烧录动作

		  					if(!ret)	//数据烧写成功
		  					{
		  						strcpy(handshake->magic, "usb_burn_success");
		  						csw.bCSWStatus = 0;
		  					}
		  					else	//数据烧写失败
		  					{
		  						strcpy(handshake->magic, "usb_burn_error");
		  						csw.bCSWStatus = -1;
		  					}
						}
						break;

//						case 3:             //小机端读取每个key
//						{
//							uint start, sectors;
//							uint offset;
//
//							start   = *(int *)(cbw->CBWCDB + 4);		//读数据的偏移量
//							sectors = *(int *)(cbw->CBWCDB + 8);		//扇区数;
//
//							trans_data.send_size 	   = min(cbw->dCBWDataTransferLength, sectors * 512);
//							trans_data.act_send_buffer = (uint)trans_data.base_send_buffer;
//
//							offset = burn_private_start;
//							ret = sunxi_flash_read(start + offset, sectors, trans_data.base_send_buffer);
//							if(!ret)
//							{
//								printf("sunxi flash read err: start,0x%x sectors 0x%x\n", start, sectors);
//
//								csw.bCSWStatus = 1;
//							}
//							else
//							{
//								csw.bCSWStatus = 0;
//							}
//
//							sunxi_usb_pburn_status = SUNXI_USB_PBURN_SEND_DATA;
//						}
//						break;

						case 4:				//关闭usb
						{
							__usb_handshake_ext_t  *handshake = (__usb_handshake_ext_t *)trans_data.base_send_buffer;

                            memset(handshake, 0, sizeof(__usb_handshake_ext_t));
							strcpy(handshake->magic, "usb_burn_finish");

							trans_data.act_send_buffer = trans_data.base_send_buffer;
		  					trans_data.send_size = min(cbw->dCBWDataTransferLength, sizeof(__usb_handshake_ext_t));

							sunxi_udc_send_data((void *)trans_data.act_send_buffer, trans_data.send_size);

		  					csw.bCSWStatus = 0;

							sunxi_flash_flush();

							if(private_data_ext_buff)
							{
								free(private_data_ext_buff);
                                private_data_ext_buff = NULL;
							}

							sunxi_usb_pburn_status = SUNXI_USB_PBURN_EXIT;
						}
						break;

						case 5:	//设置烧写key flag
						{
							int ret;
							__usb_handshake_ext_t  *handshake = (__usb_handshake_ext_t *)trans_data.base_send_buffer;

							memset(handshake, 0, sizeof(__usb_handshake_ext_t));
							strcpy(handshake->magic, "usb_burn_saved");

							trans_data.act_send_buffer = trans_data.base_send_buffer;
		  					trans_data.send_size = min(cbw->dCBWDataTransferLength, sizeof(__usb_handshake_ext_t));

		  					csw.bCSWStatus = 0;

							ret = __sunxi_burn_flag();
							if(ret < 0)
							{
								printf("sunxi burn flag failed\n");
								csw.bCSWStatus = -1;
							}

							sunxi_usb_pburn_status = SUNXI_USB_PBURN_SEND_DATA;
						}
						break;
#ifdef CONFIG_SUNXI_SECURE_STORAGE
						case 6:		//huk
						{
							u8  huk[32];
							int ret = -1;

							memset(huk, 0, 32);
							//ret = smc_create_huk(huk, 32);

							csw.bCSWStatus = 0;
							trans_data.act_send_buffer = trans_data.base_send_buffer;
							trans_data.send_size = min(cbw->dCBWDataTransferLength, 32);

							if(ret < 0)
							{
								printf("smc_create_huk failed\n");
								csw.bCSWStatus = -1;
							}
							else
							{
								if(!ret)
									printf("smc_create_huk successed\n");
								else
									printf("smc_create_huk already exist\n");
								memcpy((void *)trans_data.act_send_buffer, huk, 32);
							}

		  					sunxi_usb_pburn_status = SUNXI_USB_PBURN_SEND_DATA;
						}
						break;
#endif
						case 7:		//read key one by one
						{
							int ret;
							int read_len = 0;

							sunxi_usb_pburn_status = SUNXI_USB_PBURN_SEND_DATA;

							ret = __sunxi_read_key_by_name(private_data_ext_buff, DATA_BUFFER_MAX_SIZE, &read_len);
							if(!ret)
							{
								printf("read key success\n");
								csw.bCSWStatus = 0;
							}
							else
							{
								printf("read key failed\n");
								csw.bCSWStatus = 1;
							}

							trans_data.act_send_buffer = trans_data.base_send_buffer;
							trans_data.send_size = max(cbw->dCBWDataTransferLength, read_len);

							memcpy((void *)trans_data.act_send_buffer, (void *)private_data_ext_buff, read_len);
							printf("read_len=%d\n", read_len);
							printf("the send data:\n");
							sunxi_dump((void *)private_data_ext_buff, read_len);
						}
						break;

						case 8:			//erase all key
						{
							int ret;
							__usb_handshake_ext_t  *handshake = (__usb_handshake_ext_t *)trans_data.base_send_buffer;

							memset(handshake, 0, sizeof(__usb_handshake_ext_t));

							csw.bCSWStatus = 0;
							sunxi_usb_pburn_status = SUNXI_USB_PBURN_SEND_DATA;

							trans_data.act_send_buffer = trans_data.base_send_buffer;
							trans_data.send_size = min(cbw->dCBWDataTransferLength, sizeof(__usb_handshake_ext_t));

							ret = __sunxi_erase_key();
							if(ret < 0)
							{
								printf("sunxi erase key failed\n");
								strcpy(handshake->magic, "usb_erase_failed");
								handshake->err_no = ERR_NO_ERASE_KEY_FAILED;
								break;
							}

							strcpy(handshake->magic, "usb_erase_success");
							handshake->err_no = ERR_NO_SUCCESS;
							printf("sunxi erase all key success\n");
						}
						break;
					default:
						break;
	  			}
	  			break;

	  			case 0xf3:			//自定义命令，用于烧录用户数据
	  				sunxi_usb_dbg("usb burn private\n");
	  				printf("usb command = %d\n", cbw->CBWCDB[1]);
	  				switch(cbw->CBWCDB[1])
	  				{
	  					case 0:				//握手
	  					{
	  						__usb_handshake_t  *handshake = (__usb_handshake_t *)trans_data.base_send_buffer;

							burn_private_start = sunxi_partition_get_offset_byname("private");
							burn_private_len   = sunxi_partition_get_size_byname("private");

							if(!burn_private_start)
							{
								printf("private partition is not exist\n");

								csw.bCSWStatus = -1;
							}
							else
							{
								csw.bCSWStatus = 0;
							}

                            memset(handshake, 0, sizeof(__usb_handshake_t));
							strcpy(handshake->magic, "usb_burn_handshake");
							handshake->sizelo = burn_private_len;
							handshake->sizehi = 0;
							sunxi_usb_pburn_status = SUNXI_USB_PBURN_SEND_DATA;

							sunxi_usb_burn_from_boot_setup = 1;
							sunxi_usb_burn_from_boot_handshake = 1;

		  					trans_data.act_send_buffer = trans_data.base_send_buffer;
		  					trans_data.send_size = min(cbw->dCBWDataTransferLength, sizeof(__usb_handshake_t));
						}
						break;

						case 1:				//小机端接收数据
						{
							//pburn_flash_sectors  = *(int *)(cbw->CBWCDB + 8);
							//pburn_flash_start    = *(int *)(cbw->CBWCDB + 4);
							memcpy(&pburn_flash_start,(cbw->CBWCDB + 4),4);

							trans_data.recv_size = cbw->dCBWDataTransferLength;
							trans_data.act_recv_buffer = trans_data.base_recv_buffer;

                            pburn_flash_start += burn_private_start;
                            sunxi_usb_pburn_write_enable = 0;
							sunxi_udc_start_recv_by_dma(trans_data.act_recv_buffer, trans_data.recv_size);	//start dma to receive data

					        sunxi_usb_pburn_status = SUNXI_USB_PBURN_RECEIVE_DATA;
						}
						break;

						case 3:             //小机端发送数据
						{
							uint start, sectors;

							//start   = *(int *)(cbw->CBWCDB + 4);		//读数据的偏移量
							//sectors = *(int *)(cbw->CBWCDB + 8);		//扇区数;
							memcpy(&start,(cbw->CBWCDB + 4),4);
							memcpy(&sectors,(cbw->CBWCDB + 8),4);

							printf("start=%d, sectors=%d\n", start, sectors);

							trans_data.send_size 	   = min(cbw->dCBWDataTransferLength, sectors * 512);
							trans_data.act_send_buffer = trans_data.base_send_buffer;

							printf("send size=%d\n", trans_data.send_size);

							ret = sunxi_flash_read(start + burn_private_start, sectors, trans_data.base_send_buffer);
							if(!ret)
							{
								printf("sunxi flash read err: start,0x%x sectors 0x%x\n", start, sectors);

								csw.bCSWStatus = 1;
							}
							else
							{
								csw.bCSWStatus = 0;
							}

							sunxi_usb_pburn_status = SUNXI_USB_PBURN_SEND_DATA;
						}
						break;

						case 4:				//关闭usb
						{
							__usb_handshake_ext_t  *handshake = (__usb_handshake_ext_t *)trans_data.base_send_buffer;

                            memset(handshake, 0, sizeof(__usb_handshake_ext_t));
							strcpy(handshake->magic, "usb_burn_finish");

							trans_data.act_send_buffer = trans_data.base_send_buffer;
		  					trans_data.send_size = min(cbw->dCBWDataTransferLength, sizeof(__usb_handshake_ext_t));

							sunxi_udc_send_data((void *)trans_data.act_send_buffer, trans_data.send_size);

		  					csw.bCSWStatus = 0;

							sunxi_flash_flush();
		  					sunxi_usb_pburn_status = SUNXI_USB_PBURN_EXIT;

						}
						break;

						case 5:
						{
							__usb_handshake_ext_t  *handshake = (__usb_handshake_ext_t *)trans_data.base_send_buffer;
							char buffer[512];

							memset(handshake, 0, sizeof(__usb_handshake_ext_t));
							strcpy(handshake->magic, "usb_burn_saved");

							trans_data.act_send_buffer = trans_data.base_send_buffer;
		  					trans_data.send_size = min(cbw->dCBWDataTransferLength, sizeof(__usb_handshake_ext_t));

		  					csw.bCSWStatus = 0;

		  					sunxi_usb_pburn_status = SUNXI_USB_PBURN_SEND_DATA;

		  					memset(buffer, 0, 512);
							strcpy(buffer, "key_burned");

		  					if(!sunxi_flash_write(burn_private_start + burn_private_len - (8192+512)/512, 1, buffer))
		  					{
		  						printf("save burned flag err\n");

		  						csw.bCSWStatus = -1;
		  					}
		  					sunxi_flash_flush();
#ifdef CONFIG_SUNXI_SECURE_STORAGE
							if(sunxi_secure_storage_init())
							{
								printf("init secure storage failed\n");

								csw.bCSWStatus = -1;
							}
		  					else
		  					{
		  						if(sunxi_secure_object_write("key_burned_flag", "key_burned", strlen("key_burned")))
		  						{
		  							printf("save burned flag err\n");

		  							csw.bCSWStatus = -1;
		  						}
		  					}
		  					sunxi_secure_storage_exit();
#endif
						}
						break;

					default:
						break;
	  				}
					break;

	  			case 0xf7:		//自定义命令字，烧录分区数据
	  				sunxi_usb_dbg("usb burn partition\n");
	  				sunxi_usb_dbg("usb command = %d\n", cbw->CBWCDB[1]);
	  				switch (cbw->CBWCDB[1])
	  				{
	  					case 0:				//握手
	  					{
			  				printf("usb command = %d\n", cbw->CBWCDB[1]);
	  						__usb_handshake_sec_t  *handshake = (__usb_handshake_sec_t *)trans_data.base_send_buffer;

                            memset(handshake, 0, sizeof(__usb_handshake_sec_t));
							strcpy(handshake->magic, "usb_burn_handshake");

		  					trans_data.act_send_buffer = trans_data.base_send_buffer;
		  					trans_data.send_size = min(cbw->dCBWDataTransferLength, sizeof(__usb_handshake_sec_t));

							sunxi_usb_burn_from_boot_setup = 1;

	  						csw.bCSWStatus = 0;
	  						sunxi_usb_burn_from_boot_handshake = 1;

	  						sunxi_usb_pburn_status = SUNXI_USB_PBURN_SEND_DATA;
						}
						break;

						case 1:				//小机端接收分区数据
						{
					        //pburn_flash_start    = *(int *)(cbw->CBWCDB + 4);

							trans_data.recv_size = cbw->dCBWDataTransferLength;
							trans_data.act_recv_buffer = trans_data.base_recv_buffer;

							pburn_flash_start += pburn_flash_sectors;
                            sunxi_usb_pburn_write_enable = 0;

							sunxi_udc_start_recv_by_dma(trans_data.act_recv_buffer, trans_data.recv_size);	//start dma to receive data
							sunxi_usb_dbg("recv_size=%d\n", trans_data.recv_size);
							pburn_flash_sectors = (trans_data.recv_size + 511)/512;

					        sunxi_usb_pburn_status = SUNXI_USB_PBURN_RECEIVE_DATA;
						}
						break;

						case 2:				//查询结果
						{
			  				printf("usb command = %d\n", cbw->CBWCDB[1]);
							__usb_handshake_ext_t  *handshake = (__usb_handshake_ext_t *)trans_data.base_send_buffer;
							memset(handshake, 0, sizeof(__usb_handshake_ext_t));

							trans_data.act_send_buffer = trans_data.base_send_buffer;
		  					trans_data.send_size = min(cbw->dCBWDataTransferLength, sizeof(__usb_handshake_ext_t));

							if(burn_part_result_state)
							{
								strcpy(handshake->magic, "usb_burn_error");
							}
							else
							{
								strcpy(handshake->magic, "usb_burn_success");
							}
							csw.bCSWStatus = 0;
							handshake->err_no = burn_part_result_state;
							sunxi_usb_pburn_status = SUNXI_USB_PBURN_SEND_DATA;
						}
						break;

						case 3:				//小机端接收分区信息
						{
			  				printf("usb command = %d\n", cbw->CBWCDB[1]);
							trans_data.recv_size = cbw->dCBWDataTransferLength;
							trans_data.act_recv_buffer = trans_data.base_recv_buffer;

                            sunxi_usb_pburn_write_enable = 0;

							sunxi_udc_start_recv_by_dma(trans_data.act_recv_buffer, trans_data.recv_size);	//start dma to receive data

							sunxi_usb_dbg("recv_size=%d\n", trans_data.recv_size);

							sunxi_usb_pburn_status = SUNXI_USB_PBURN_RECEIVE_PART_INFO;
						}
						break;

						case 4:				//关闭usb
						{
			  				printf("usb command = %d\n", cbw->CBWCDB[1]);
							__usb_handshake_ext_t  *handshake = (__usb_handshake_ext_t *)trans_data.base_send_buffer;

                            memset(handshake, 0, sizeof(__usb_handshake_ext_t));
							strcpy(handshake->magic, "usb_burn_finish");

							trans_data.act_send_buffer = trans_data.base_send_buffer;
		  					trans_data.send_size = min(cbw->dCBWDataTransferLength, sizeof(__usb_handshake_ext_t));

							sunxi_udc_send_data((void *)trans_data.act_send_buffer, trans_data.send_size);

		  					csw.bCSWStatus = 0;
							sunxi_flash_flush();
		  					sunxi_usb_pburn_status = SUNXI_USB_PBURN_EXIT;

						}
						break;
#ifdef  CONFIG_SUNXI_SECURE_STORAGE
						case 5:	//设置烧写key flag
						{
							printf("usb command = %d\n", cbw->CBWCDB[1]);
							__usb_handshake_ext_t  *handshake = (__usb_handshake_ext_t *)trans_data.base_send_buffer;

							memset(handshake, 0, sizeof(__usb_handshake_ext_t));
							strcpy(handshake->magic, "usb_burn_saved");

							trans_data.act_send_buffer = trans_data.base_send_buffer;
		  					trans_data.send_size = min(cbw->dCBWDataTransferLength, sizeof(__usb_handshake_ext_t));

		  					csw.bCSWStatus = 0;

							if(sunxi_secure_storage_init())
							{
								printf("secure storage init err\n");
								csw.bCSWStatus = -1;
								break;
							}
		  					sunxi_usb_pburn_status = SUNXI_USB_PBURN_SEND_DATA;
		  					if(sunxi_secure_object_write("key_burned_flag", "key_burned", strlen("key_burned")))
		  					{
		  						printf("save burned flag err\n");

		  						csw.bCSWStatus = -1;
		  					}
		  					sunxi_secure_storage_exit();
						}
						break;
#endif
						case 7:             //接收分区校验数据
						{
			  				printf("usb command = %d\n", cbw->CBWCDB[1]);
							trans_data.recv_size = cbw->dCBWDataTransferLength;
							trans_data.act_recv_buffer = trans_data.base_recv_buffer;

                            sunxi_usb_pburn_write_enable = 0;

							sunxi_udc_start_recv_by_dma(trans_data.act_recv_buffer, trans_data.recv_size);	//start dma to receive data

                            printf("recv_size=%d\n", trans_data.recv_size);

							sunxi_usb_pburn_status = SUNXI_USB_PBURN_RECEIVE_PART_VERIFY;
						}
						break;
						default:
							printf("usb command = %d not support\n", cbw->CBWCDB[1]);
						break;
	  				}
	  		}

	  		break;

	  	case SUNXI_USB_PBURN_SEND_DATA:

	  		sunxi_usb_dbg("SUNXI_USB_SEND_DATA\n");

			sunxi_usb_pburn_status = SUNXI_USB_PBURN_STATUS;
			printf("SUNXI_USB_SEND_DATA=%d\n", trans_data.send_size);
			sunxi_udc_send_data((void *)trans_data.act_send_buffer, trans_data.send_size);
#if defined(SUNXI_USB_30)
			sunxi_usb_pburn_status_enable = 0;
#endif
	  		break;

	  	case SUNXI_USB_PBURN_RECEIVE_DATA:

	  		sunxi_usb_dbg("SUNXI_USB_RECEIVE_DATA\n");

			if(sunxi_usb_pburn_write_enable == 1)
			{
				sunxi_usb_dbg("write flash, start 0x%x, sectors 0x%x\n", pburn_flash_start, trans_data.recv_size/512);
				ret = sunxi_flash_write(pburn_flash_start, (trans_data.recv_size+511)/512, (void *)trans_data.act_recv_buffer);
				if(!ret)
				{
					printf("sunxi flash write err: start,0x%x sectors 0x%x\n", pburn_flash_start, (trans_data.recv_size+511)/512);

					csw.bCSWStatus = 1;
				}
				else
				{
					csw.bCSWStatus = 0;
  				}
				sunxi_usb_pburn_write_enable = 0;

  				sunxi_usb_pburn_status = SUNXI_USB_PBURN_STATUS;
			}

	  		break;

		case SUNXI_USB_PBURN_STATUS:

			sunxi_usb_dbg("SUNXI_USB_PBURN_STATUS\n");
#if defined(SUNXI_USB_30)
			if(sunxi_usb_pburn_status_enable)
#endif
			{
				sunxi_usb_pburn_status = SUNXI_USB_PBURN_IDLE;
				sunxi_ubuf->rx_ready_for_data = 0;
				__sunxi_pburn_send_status(&csw, sizeof(struct umass_bbb_csw_t));
			}

			break;

		case SUNXI_USB_PBURN_EXIT:

			printf("SUNXI_USB_PBURN_EXIT\n");

			sunxi_usb_pburn_status = SUNXI_USB_PBURN_IDLE;
			sunxi_ubuf->rx_ready_for_data = 0;
			__sunxi_pburn_send_status(&csw, sizeof(struct umass_bbb_csw_t));

			printf("Device will shutdown in 3 Secends...\n");
			__msdelay(3000);

			return SUNXI_UPDATE_NEXT_ACTION_SHUTDOWN;

	  	case SUNXI_USB_PBURN_RECEIVE_NULL:

	  		sunxi_usb_dbg("SUNXI_USB_PBURN_RECEIVE_NULL\n");
			if(sunxi_usb_pburn_write_enable == 1)
			{
				csw.bCSWStatus = 0;
				sunxi_usb_pburn_write_enable = 0;
  				sunxi_usb_pburn_status = SUNXI_USB_PBURN_STATUS;
			}
	  		break;

		case SUNXI_USB_PBURN_RECEIVE_PART_INFO:
			printf("SUNXI_USB_PBURN_RECEIVE_PART_INFO\n");
			if(sunxi_usb_pburn_write_enable == 1)
			{
				//sunxi_dump((u8 *)(trans_data.act_recv_buffer), 1024);
				burn_part_result_state = burn_part_probe((void*)trans_data.act_recv_buffer);
				if(!burn_part_result_state)
				{
					pburn_flash_start = burn_part_start;
					pburn_flash_sectors = 0;
				}
				csw.bCSWStatus = 0;
				sunxi_usb_pburn_write_enable = 0;
  				sunxi_usb_pburn_status = SUNXI_USB_PBURN_STATUS;
			}
			break;

		case SUNXI_USB_PBURN_RECEIVE_PART_VERIFY:
			printf("SUNXI_USB_PBURN_RECEIVE_PART_VERIFY\n");
			if(sunxi_usb_pburn_write_enable == 1)
			{
				//数据校验
				//sunxi_dump((u8 *)(trans_data.act_recv_buffer), 1024);
				printf("burn_part_bytes=%lld\n", burn_part_bytes);
				ret = sunxi_burn_part_verify((void *)trans_data.act_recv_buffer, burn_part_bytes);
				if(!ret)	//数据校验成功
				{
					printf("data verify success\n");
					burn_part_result_state = ERR_NO_SUCCESS;
				}
				else	//数据校验失败
				{
					printf("data verify failed\n");
					burn_part_result_state = ERR_NO_PART_VERIFY_ERR;
				}
				csw.bCSWStatus = 0;
				sunxi_usb_pburn_write_enable = 0;
  				sunxi_usb_pburn_status = SUNXI_USB_PBURN_STATUS;
			}
			break;

	  	default:
	  		break;
	}

	return 0;
}


sunxi_usb_module_init(SUNXI_USB_DEVICE_BURN,					\
					  sunxi_pburn_init,							\
					  sunxi_pburn_exit,							\
					  sunxi_pburn_reset,							\
					  sunxi_pburn_standard_req_op,				\
					  sunxi_pburn_nonstandard_req_op,			\
					  sunxi_pburn_state_loop,					\
					  sunxi_pburn_usb_rx_dma_isr,				\
					  sunxi_pburn_usb_tx_dma_isr					\
					  );
