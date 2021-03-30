#include <linux/types.h>
#include <config.h>
#include <private_uboot.h>
#include <asm/io.h>
#include <private_toc.h>


//**********************************************************************
extern int printf(const char *fmt, ...);
extern void * memcpy(void * dest,const void *src,size_t count);
extern void * memset(void * s, int c, size_t count);
extern int strncmp(const char * cs,const char * ct,size_t count);


int toc1_flash_read(u32 start_sector, u32 blkcnt, void *buff)
{
	memcpy(buff, (void *)(CONFIG_BOOTPKG_STORE_IN_DRAM_BASE + 512 * start_sector), 512 * blkcnt);

	return blkcnt;
}

uint toc1_item_read(struct sbrom_toc1_item_info *p_toc_item, void * p_dest, u32 buff_len)
{
	u32 to_read_blk_start = 0;
	u32 to_read_blk_sectors = 0;
	s32 ret = 0;

	if( buff_len  < p_toc_item->data_len )
	{
		printf("PANIC : Toc1_item_read() error --1--,buff error\n");

		return 0;
	}

	to_read_blk_start   = (p_toc_item->data_offset)>>9;
	to_read_blk_sectors = (p_toc_item->data_len + 0x1ff)>>9;

	ret = toc1_flash_read(to_read_blk_start, to_read_blk_sectors, p_dest);
	if(ret != to_read_blk_sectors)
	{
		printf("PANIC: toc1_item_read() error --2--, read error\n");

		return 0;
	}

	return ret * 512;
}


int load_fip(int *use_monitor)
{
	int i;
	//int len;
	
	struct sbrom_toc1_head_info  *toc1_head = NULL;
	struct sbrom_toc1_item_info  *item_head = NULL;

	struct sbrom_toc1_item_info  *toc1_item = NULL;
	
	toc1_head = (struct sbrom_toc1_head_info *)CONFIG_BOOTPKG_STORE_IN_DRAM_BASE;
	item_head = (struct sbrom_toc1_item_info *)(CONFIG_BOOTPKG_STORE_IN_DRAM_BASE + sizeof(struct sbrom_toc1_head_info));

#ifdef BOOT_DEBUG
	printf("*******************TOC1 Head Message*************************\n");
	printf("Toc_name          = %s\n",   toc1_head->name);
	printf("Toc_magic         = 0x%x\n", toc1_head->magic);
	printf("Toc_add_sum	      = 0x%x\n", toc1_head->add_sum);

	printf("Toc_serial_num    = 0x%x\n", toc1_head->serial_num);
	printf("Toc_status        = 0x%x\n", toc1_head->status);

	printf("Toc_items_nr      = 0x%x\n", toc1_head->items_nr);
	printf("Toc_valid_len     = 0x%x\n", toc1_head->valid_len);
	printf("TOC_MAIN_END      = 0x%x\n", toc1_head->end);
	printf("***************************************************************\n\n");
#endif
	//init
	toc1_item = item_head;
	for(i=0;i<toc1_head->items_nr;i++,toc1_item++)
	{
#ifdef BOOT_DEBUG
		printf("\n*******************TOC1 Item Message*************************\n");
		printf("Entry_name        = %s\n",   toc1_item->name);
		printf("Entry_data_offset = 0x%x\n", toc1_item->data_offset);
		printf("Entry_data_len    = 0x%x\n", toc1_item->data_len);

		printf("encrypt	          = 0x%x\n", toc1_item->encrypt);
		printf("Entry_type        = 0x%x\n", toc1_item->type);
		printf("run_addr          = 0x%x\n", toc1_item->run_addr);
		printf("index             = 0x%x\n", toc1_item->index);
		printf("Entry_end         = 0x%x\n", toc1_item->end);
		printf("***************************************************************\n\n");
#endif
		printf("Entry_name        = %s\n",   toc1_item->name);

		if(strncmp(toc1_item->name, ITEM_UBOOT_NAME, sizeof(ITEM_UBOOT_NAME)) == 0)
		{
			toc1_flash_read(toc1_item->data_offset/512, (toc1_item->data_len+511)/512, (void *)CONFIG_SYS_TEXT_BASE);
		}
		else
		{
			printf("unknow boot package file \n");
			return -1;
		}

	}
	
	return 0;
}
