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

#include "common.h"
#include "mmc.h"
#include "spare_head.h"
#include "private_toc.h"


extern int sunxi_mmc_init(int sdc_no, unsigned bus_width, normal_gpio_cfg *gpio_info, int offset);
extern int load_toc1_from_nand( void );
extern int verify_addsum( void *mem_base, __u32 size );


extern sbrom_toc0_config_t *toc0_config;
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
int sunxi_flash_init(int boot_type)
{
	int ret;

	if((boot_type == BOOT_FROM_SD0) || (boot_type == BOOT_FROM_SD2))
	{
		u8  *tmp_buff = (u8 *)CONFIG_TOC1_STORE_IN_DRAM_BASE;
		uint head_size;
		sbrom_toc1_head_info_t  *toc1_head;
		int  sunxi_flash_mmc_card_no;
		int start_sector,i;
		int start_sectors[4] = {UBOOT_START_SECTOR_IN_SDMMC,UBOOT_BACKUP_START_SECTOR_IN_SDMMC,0,0};

		if(boot_type == BOOT_FROM_SD0)
		{
			sunxi_flash_mmc_card_no = 0;
		}
		else
		{
			sunxi_flash_mmc_card_no = 2;
		}
		ret = sunxi_mmc_init(sunxi_flash_mmc_card_no, 4, toc0_config->storage_gpio + 24, 8);
		if(ret <= 0)
		{
			printf("mmc: init failed\n");
			return -1;
		}

		for(i = 0; i<4; i++)
		{
			start_sector = start_sectors[i];
			tmp_buff = (u8 *)CONFIG_TOC1_STORE_IN_DRAM_BASE;
			if(start_sector == 0)
			{
				printf("mmc: read all toc1 failed\n");
				return -1;
			}

			ret = mmc_bread(sunxi_flash_mmc_card_no, start_sector, 64, tmp_buff);
			if(!ret)
			{
				printf("mmc: read error,start=%d\n",start_sector);
				continue;
			}
			toc1_head = (struct sbrom_toc1_head_info *)tmp_buff;
			if(toc1_head->magic != TOC_MAIN_INFO_MAGIC)
			{
				printf("mmc: toc1 magic error\n");
				continue;
			}
			head_size = toc1_head->valid_len;
			if(head_size > 64 * 512)
			{
				tmp_buff += 64*512;
				ret = mmc_bread(sunxi_flash_mmc_card_no, start_sector + 64, (head_size - 64*512 + 511)/512, tmp_buff);
				if(!ret)
				{
					printf("mmc:read error\n");
					continue;
				}
			}
			if( verify_addsum( (__u32 *)CONFIG_TOC1_STORE_IN_DRAM_BASE, head_size) != 0 )
			{
				printf("mmc: toc1 checksun error.\n");
				continue;
			}
			printf("read toc1 from emmc %d sector\n",start_sector);
			break;
		}
		return 0;
	}
	else if(boot_type == BOOT_FROM_NFC)
	{
		if(load_toc1_from_nand())
		{
			printf("nand: init failed\n");
			return -1;
		}

		return 0;
	}
	else
	{
		printf("PANIC: boot_type(%d) not support now\n",boot_type);
		return -1;
	}
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
int sunxi_flash_read(u32 start_sector, u32 blkcnt, void *buff)
{
	memcpy(buff, (void *)(CONFIG_TOC1_STORE_IN_DRAM_BASE + 512 * start_sector), 512 * blkcnt);

	return blkcnt;
}
