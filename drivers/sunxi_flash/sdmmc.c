#include <common.h>
#include <sys_config.h>
#include <sys_partition.h>
#include <boot_type.h>
#include <sunxi_board.h>
#include <mmc.h>
#include <malloc.h>
#include <securestorage.h>

#include "flash_interface.h"

static int mmc_secure_storage_read_key(int item, unsigned char *buf, unsigned int len);
static int mmc_secure_storage_read_map(int item, unsigned char *buf, unsigned int len);

static struct mmc *mmc_boot,*mmc_sprite;
static int mmc_no;
static unsigned char _inner_buffer[4096+64]; /*align temp buffer*/


//-------------------------------------noraml interface--------------------------------------------
static int
sunxi_flash_mmc_read(unsigned int start_block, unsigned int nblock, void *buffer)
{
	debug("mmcboot read: start 0x%x, sector 0x%x\n", start_block, nblock);

	return mmc_boot->block_dev.block_read(mmc_boot->block_dev.dev, start_block + CONFIG_MMC_LOGICAL_OFFSET,
					nblock, buffer);
}


static int
sunxi_flash_mmc_write(unsigned int start_block, unsigned int nblock, void *buffer)
{
	debug("mmcboot write: start 0x%x, sector 0x%x\n", start_block, nblock);

	return mmc_boot->block_dev.block_write(mmc_boot->block_dev.dev, start_block + CONFIG_MMC_LOGICAL_OFFSET,
					nblock, buffer);
}

static uint
sunxi_flash_mmc_size(void){

	return mmc_boot->block_dev.lba;
}

static int
sunxi_flash_mmc_init(int stage){
	return 0;
}

static int
sunxi_flash_mmc_exit(int force){

	return mmc_exit();
	//return 0;
}

int sunxi_flash_mmc_phyread(unsigned int start_block, unsigned int nblock, void *buffer)
{
	return mmc_boot->block_dev.block_read_mass_pro(mmc_boot->block_dev.dev, start_block, nblock, buffer);
}

int sunxi_flash_mmc_phywrite(unsigned int start_block, unsigned int nblock, void *buffer)
{
	return mmc_boot->block_dev.block_write_mass_pro(mmc_boot->block_dev.dev, start_block, nblock, buffer);
}


//-------------------------------------sprite interface--------------------------------------------
static int
sunxi_sprite_mmc_read(unsigned int start_block, unsigned int nblock, void *buffer)
{
	debug("mmcsprite read: start 0x%x, sector 0x%x\n", start_block, nblock);

	return mmc_sprite->block_dev.block_read(mmc_sprite->block_dev.dev, start_block + CONFIG_MMC_LOGICAL_OFFSET,
					nblock, buffer);
}


int sunxi_sprite_mmc_bootop_enable(int part_nu, int enable)
{
	return mmc_sprite->block_dev.block_enable_bootop(mmc_sprite->block_dev.dev, part_nu, enable);
}

int sunxi_sprite_mmc_bootp_phyread(unsigned int start_block, unsigned int nblock, void *buffer)
{
#ifdef CONFIG_MMC_BOOT_START
	printf("read form mmc boot part: start blk=%d, nblk=%d\n", start_block, nblock);
	return mmc_sprite->block_dev.block_read_emmc_bootp(mmc_sprite->block_dev.dev, start_block,
					nblock, 1, buffer);
#else
	return -1;
#endif
}


int sunxi_sprite_mmc_bootp_phywrite(unsigned int start_block, unsigned int nblock, void *buffer)
{
#ifdef CONFIG_MMC_BOOT_START
	printf("write to mmc boot part: start blk=%d, nblk=%d\n", start_block, nblock);
	return mmc_sprite->block_dev.block_write_emmc_bootp(mmc_sprite->block_dev.dev, start_block,
					nblock, 1, buffer);
#else
	return -1;
#endif
}

int sunxi_sprite_mmc_bootp_phyerase(unsigned int start_block, unsigned int nblock, void *skip)
{
	return -1;
}

/* mmc write protect for boot */
int sunxi_flash_mmc_user_phyget_wp_grp_size(unsigned int *wp_grp_size)
{
	return mmc_boot->block_dev.block_mmc_user_get_wp_grp_size(mmc_boot->block_dev.dev, wp_grp_size);
}

int sunxi_flash_mmc_user_phywrite_protect(unsigned wp_type, unsigned start, unsigned blkcnt)
{
	return mmc_boot->block_dev.block_mmc_user_write_protect(mmc_boot->block_dev.dev, wp_type, start, blkcnt);
}

int sunxi_flash_mmc_phyclr_tem_wp(unsigned start, unsigned blkcnt)
{
	return mmc_boot->block_dev.block_mmc_clr_tem_wp(mmc_boot->block_dev.dev, start, blkcnt);
}

/*
* --argument
* @wp_grp_size:
*      return the wp group size
* --return
*    0-ok,
*    others-fail.
*/
int sunxi_sprite_mmc_user_phyget_wp_grp_size(unsigned int *wp_grp_size)
{
	return mmc_sprite->block_dev.block_mmc_user_get_wp_grp_size(mmc_sprite->block_dev.dev, wp_grp_size);
}

/*
* --argument
* @wp_type: write protection type
*     0-power-no write protection
*     1-permanent write protection
       2-temporary write protection
* @start:start sector
* @blknr: the number of sectors
*
* --return
*    0-ok, execute write protect operation successfully.
*    others-fail.
*    only support write protect group size aligned @from and @nr.
*/
int sunxi_sprite_mmc_user_phywrite_protect(unsigned wp_type, unsigned start, unsigned blkcnt)
{
	return mmc_sprite->block_dev.block_mmc_user_write_protect(mmc_sprite->block_dev.dev, wp_type, start, blkcnt);
}

/*
* --argument
* @start: start sector
* @blkcnt: the number of sectors
*
* --return
*    0-ok, execute clear temporary write protection successfully.
*    others-fail.
*    only support write protect group size aligned @from and @nr.
*/
int sunxi_sprite_mmc_phyclr_tem_wp(unsigned start, unsigned blkcnt)
{
	return mmc_sprite->block_dev.block_mmc_clr_tem_wp(mmc_sprite->block_dev.dev, start, blkcnt);
}



static int
sunxi_sprite_mmc_write(unsigned int start_block, unsigned int nblock, void *buffer)
{

	return mmc_sprite->block_dev.block_write(mmc_sprite->block_dev.dev, start_block + CONFIG_MMC_LOGICAL_OFFSET,
					nblock, buffer);
}

static int
sunxi_sprite_mmc_erase(int erase, void *mbr_buffer)
{
	return card_erase(erase, mbr_buffer);
}

static uint
sunxi_sprite_mmc_size(void){

	return mmc_sprite->block_dev.lba;
}

static int
sunxi_sprite_mmc_init(int stage){
	return 0;
}

static int
sunxi_sprite_mmc_exit(int force){
	return 0;
}


int sunxi_sprite_mmc_phyread(unsigned int start_block, unsigned int nblock, void *buffer)
{
	return mmc_sprite->block_dev.block_read_mass_pro(mmc_sprite->block_dev.dev, start_block, nblock, buffer);
}

int sunxi_sprite_mmc_phywrite(unsigned int start_block, unsigned int nblock, void *buffer)
{
	return mmc_sprite->block_dev.block_write_mass_pro(mmc_sprite->block_dev.dev, start_block, nblock, buffer);
}

int sunxi_sprite_mmc_phyerase(unsigned int start_block, unsigned int nblock, void *skip)
{
	if (nblock == 0) {
		printf("%s: @nr is 0, erase from @from to end\n", __FUNCTION__);
		nblock = mmc_sprite->block_dev.lba - start_block - 1;
	}
	return mmc_sprite->block_dev.block_mmc_erase(mmc_sprite->block_dev.dev, start_block, nblock, skip);
}

int sunxi_sprite_mmc_phywipe(unsigned int start_block, unsigned int nblock, void *skip)
{
	if (nblock == 0) {
		printf("%s: @nr is 0, wipe from @from to end\n", __FUNCTION__);
		nblock = mmc_sprite->block_dev.lba - start_block - 1;
	}
	return mmc_sprite->block_dev.block_secure_wipe(mmc_sprite->block_dev.dev, start_block, nblock, skip);
}

int sunxi_sprite_mmc_force_erase(int erase, void *mbr_buffer)
{
    if (mbr_buffer == NULL)
		return 0;

	card_erase(1, mbr_buffer);
    return 0;
}

//-----------------------------secure interface---------------------------------------
int sunxi_flash_mmc_secread( int item, unsigned char *buf, unsigned int nblock)
{
	return mmc_boot->block_dev.block_read_secure(mmc_no, item, (u8 *)buf, nblock);
}

int sunxi_flash_mmc_secread_backup( int item, unsigned char *buf, unsigned int nblock)
{
	return mmc_boot->block_dev.block_read_secure_backup(mmc_no, item, (u8 *)buf, nblock);
}

int sunxi_flash_mmc_secwrite( int item, unsigned char *buf, unsigned int nblock)
{
	return mmc_boot->block_dev.block_write_secure(mmc_no, item, (u8 *)buf, nblock);
}

int sunxi_sprite_mmc_secwrite(int item ,unsigned char *buf,unsigned int nblock)
{
    if(mmc_sprite->block_dev.block_write_secure(mmc_no, item, (u8 *)buf, nblock) >=0)
        return 0;
    else
        return -1;
}

int sunxi_sprite_mmc_secread(int item ,unsigned char *buf,unsigned int nblock)
{
    if(mmc_sprite->block_dev.block_read_secure(mmc_no, item, (u8 *)buf, nblock) >=0)
        return 0;
    else
        return -1;
}

int sunxi_sprite_mmc_secread_backup(int item ,unsigned char *buf,unsigned int nblock)
{
    if(mmc_sprite->block_dev.block_read_secure_backup(mmc_no, item, (u8 *)buf, nblock) >=0)
        return 0;
    else
        return -1;
}

int mmc_secure_storage_read(int item, unsigned char *buf, unsigned int len)
{
	if (item == 0)
		return mmc_secure_storage_read_map(item, buf, len);
	else
		return mmc_secure_storage_read_key(item, buf, len);
}

int mmc_secure_storage_write(int item, unsigned char *buf, unsigned int len)
{
	unsigned char * align ;
	unsigned int blkcnt;
	int workmode;

	if(((unsigned int)buf%32)){ // input buf not align
		align = (unsigned char *)(((unsigned int)_inner_buffer + 0x20)&(~0x1f)) ;
		memcpy(align, buf, len);
	}else
		align=buf;

	blkcnt = (len+511)/512 ;
	workmode = uboot_spare_head.boot_data.work_mode;
	if(workmode == WORK_MODE_BOOT || workmode == WORK_MODE_SPRITE_RECOVERY)
	{
		return (sunxi_flash_mmc_secwrite(item, align, blkcnt) == blkcnt) ? 0 : -1;
	}
	else if((workmode & WORK_MODE_PRODUCT) || (workmode == 0x30))
	{
		return sunxi_sprite_mmc_secwrite(item, align, blkcnt);
	}
	else
	{
		printf("workmode=%d is err\n", workmode);
		return -1;
	}

}

//-----------------------------------end ------------------------------------------------------

int sdmmc_init_for_boot(int workmode, int card_no)
{

	tick_printf("MMC:	 %d\n", card_no);

	board_mmc_set_num(card_no);
	debug("set card number\n");
	board_mmc_pre_init(card_no);
	debug("begin to find mmc\n");
	mmc_boot = find_mmc_device(card_no);
	mmc_no = card_no;
	if(!mmc_boot){
		printf("fail to find one useful mmc card\n");
		return -1;
	}
	debug("try to init mmc\n");
	if (mmc_init(mmc_boot)) {
		puts("MMC init failed\n");
		return  -1;
	}
	debug("mmc %d init ok\n", card_no);
	sunxi_flash_init_pt  = sunxi_flash_mmc_init;
	sunxi_flash_read_pt  = sunxi_flash_mmc_read;
	sunxi_flash_write_pt = sunxi_flash_mmc_write;
	sunxi_flash_size_pt  = sunxi_flash_mmc_size;
	sunxi_flash_exit_pt  = sunxi_flash_mmc_exit;
	sunxi_flash_phyread_pt  = sunxi_flash_mmc_phyread;
	sunxi_flash_phywrite_pt = sunxi_flash_mmc_phywrite;

	//for write protect
	sunxi_flash_user_phyget_wp_grp_size_pt = sunxi_flash_mmc_user_phyget_wp_grp_size;
	sunxi_flash_user_phywrite_protect_pt=sunxi_flash_mmc_user_phywrite_protect;
	sunxi_flash_phyclr_tem_wp_pt=sunxi_flash_mmc_phyclr_tem_wp;

	//for fastboot
	sunxi_sprite_phyread_pt  = sunxi_flash_mmc_phyread;
	sunxi_sprite_phywrite_pt = sunxi_flash_mmc_phywrite;
	sunxi_sprite_read_pt  = sunxi_flash_read_pt;
	sunxi_sprite_write_pt = sunxi_flash_write_pt;

	/* for secure stoarge */
	sunxi_secstorage_read_pt  = mmc_secure_storage_read;
	sunxi_secstorage_write_pt = mmc_secure_storage_write;

	return 0;

}

int sdmmc_init_for_sprite(int workmode)
{
	printf("try nand fail\n");
	printf("try card 2 \n");
        board_mmc_pre_init(2);
	mmc_sprite = find_mmc_device(2);
	mmc_no = 2;
	if(!mmc_sprite){
		printf("fail to find one useful mmc card2\n");
#ifdef CONFIG_MMC3_SUPPORT
                printf("try to find card3 \n");
                board_mmc_pre_init(3);
                mmc_sprite = find_mmc_device(3);
		mmc_no = 3;
                if(!mmc_sprite)
                {
                        printf("try card3 fail \n");
                        return -1;
                }
                else
                {
						set_boot_storage_type(STORAGE_EMMC3);
                }
#else
                return -1;
#endif
	}
        else
        {
	       set_boot_storage_type(STORAGE_EMMC);
        }
	if (mmc_init(mmc_sprite)) {
		printf("MMC init failed\n");
		return  -1;
	}
	sunxi_sprite_init_pt  = sunxi_sprite_mmc_init;
	sunxi_sprite_exit_pt  = sunxi_sprite_mmc_exit;
	sunxi_sprite_read_pt  = sunxi_sprite_mmc_read;
	sunxi_sprite_write_pt = sunxi_sprite_mmc_write;
	sunxi_sprite_erase_pt = sunxi_sprite_mmc_erase;
	sunxi_sprite_size_pt  = sunxi_sprite_mmc_size;
	sunxi_sprite_phyread_pt  = sunxi_sprite_mmc_phyread;
	sunxi_sprite_phywrite_pt = sunxi_sprite_mmc_phywrite;
	sunxi_sprite_force_erase_pt = sunxi_sprite_mmc_force_erase;

	/* for write protect */
	sunxi_sprite_user_phyget_wp_grp_size_pt=sunxi_sprite_mmc_user_phyget_wp_grp_size;
	sunxi_sprite_user_phywrite_protect_pt=sunxi_sprite_mmc_user_phywrite_protect;
	sunxi_sprite_phyclr_tem_wp_pt=sunxi_sprite_mmc_phyclr_tem_wp;

	/* for secure stoarge */
	sunxi_secstorage_read_pt  = mmc_secure_storage_read;
	sunxi_secstorage_write_pt = mmc_secure_storage_write;
	debug("sunxi sprite has installed sdcard2 function\n");

	return 0;
}

int sdmmc_init_card0_for_sprite(void)
{
	//init card0
	board_mmc_pre_init(0);
	mmc_boot = find_mmc_device(0);
	if(!mmc_boot)
	{
		printf("fail to find one useful mmc card\n");
		return -1;
	}

	if (mmc_init(mmc_boot))
	{
		printf("MMC sprite init failed\n");
		return  -1;
	}
	else
	{
		printf("mmc init ok\n");
	}

	sunxi_flash_init_pt  = sunxi_flash_mmc_init;
	sunxi_flash_read_pt  = sunxi_flash_mmc_read;
	sunxi_flash_write_pt = sunxi_flash_mmc_write;
	sunxi_flash_size_pt  = sunxi_flash_mmc_size;
	sunxi_flash_phyread_pt  = sunxi_flash_mmc_phyread;
	sunxi_flash_phywrite_pt = sunxi_flash_mmc_phywrite;
	sunxi_flash_exit_pt  = sunxi_flash_mmc_exit;

	/* for write protect */
	sunxi_flash_user_phyget_wp_grp_size_pt = sunxi_flash_mmc_user_phyget_wp_grp_size;
	sunxi_flash_user_phywrite_protect_pt=sunxi_flash_mmc_user_phywrite_protect;
	sunxi_flash_phyclr_tem_wp_pt=sunxi_flash_mmc_phyclr_tem_wp;

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
int card_read_boot0( void *buffer, uint length )
{
	int ret;
	int storage_type;
	storage_type = get_boot_storage_type();
	if(STORAGE_EMMC == storage_type)
	{
#ifdef CONFIG_MMC_BOOT_START
		ret = sunxi_sprite_mmc_bootp_phyread(BOOT0_SDMMC_START_ADDR, (length+511)/512, buffer);
#else
		ret = sunxi_sprite_phyread(BOOT0_SDMMC_START_ADDR, (length+511)/512, buffer);
#endif
	}
	else
	{
		ret = sunxi_sprite_phyread(BOOT0_EMMC3_BACKUP_START_ADDR, (length+511)/512, buffer);
	}

	if(!ret)
	{
		printf("%s: call fail\n", __func__);
		return -1;
	}
	return 0;
}


static int check_secure_storage_key(unsigned char *buffer)
{
	store_object_t *obj = (store_object_t *)buffer;

	if( obj->magic != STORE_OBJECT_MAGIC ){
		printf("Input object magic fail [0x%x]\n", obj->magic);
		return -1 ;
	}

	if( obj->crc != crc32( 0 , (void *)obj, sizeof(*obj)-4 ) ){
		printf("Input object crc fail [0x%x]\n", obj->crc);
		return -1 ;
	}
	return 0;
}

static int mmc_secure_storage_read_key(int item, unsigned char *buf, unsigned int len)
{
	unsigned char *align ;
	unsigned int blkcnt;
	int ret ,workmode;

	if(((unsigned int)buf%32)){
		align = (unsigned char *)(((unsigned int)_inner_buffer + 0x20)&(~0x1f)) ;
		memset(align,0,4096);
	}else {
		align = buf ;
	}

	blkcnt = (len+511)/512 ;

	workmode = uboot_spare_head.boot_data.work_mode;
	if(workmode == WORK_MODE_BOOT || workmode == WORK_MODE_SPRITE_RECOVERY)
	{
		ret = (sunxi_flash_mmc_secread(item, align, blkcnt) == blkcnt) ? 0 : -1;
	}
	else if ((workmode & WORK_MODE_PRODUCT) || (workmode == 0x30))
	{
		ret = sunxi_sprite_mmc_secread(item, align, blkcnt);
	}
	else
	{
		printf("workmode=%d is err\n", workmode);
		return -1;
	}
	if (!ret)
	{
		/*check copy 0 */
		if (!check_secure_storage_key(align)){
			printf("the secure storage item%d copy0 is good\n",item);
			goto ok ; /*copy 0 pass*/
		}
		printf("the secure storage item%d copy0 is bad\n", item);
	}

	// read backup
	memset(align, 0x0, len);
	printf("read item%d copy1\n", item);
	if(workmode == WORK_MODE_BOOT || workmode == WORK_MODE_SPRITE_RECOVERY)
	{
		ret = (sunxi_flash_mmc_secread_backup(item, align, blkcnt) == blkcnt) ? 0 : -1;
	}
	else if ((workmode & WORK_MODE_PRODUCT) || (workmode == 0x30))
	{
		ret = sunxi_sprite_mmc_secread_backup(item, align, blkcnt);
	}
	else
	{
		printf("workmode=%d is err\n", workmode);
		return -1;
	}
	if (!ret)
	{
		/*check copy 1 */
		if (!check_secure_storage_key(align)){
			printf("the secure storage item%d copy1 is good\n",item);
			goto ok ; /*copy 1 pass*/
		}
		printf("the secure storage item%d copy1 is bad\n", item);
	}

	printf("sunxi_secstorage_read fail\n");
	return -1;

ok:
	if(((unsigned int)buf%32))
		memcpy(buf,align,len);
	return 0 ;
}

static int mmc_secure_storage_read_map(int item, unsigned char *buf, unsigned int len)
{
	int have_map_copy0 ;
	unsigned char *align ;
	unsigned int blkcnt;
	int ret ,workmode;
	char * map_copy0_buf;

	if(((unsigned int)buf%32)){
		align = (unsigned char *)(((unsigned int)_inner_buffer + 0x20)&(~0x1f)) ;
		memset(align,0,4096);
	}else {
		align = buf ;
	}

	blkcnt = (len+511)/512 ;

	map_copy0_buf=(char *)malloc(blkcnt*512);
	if(!map_copy0_buf){
		printf("out of memory\n");
		return -1 ;
	}

	printf("read item%d copy0\n", item);
	workmode = uboot_spare_head.boot_data.work_mode;
	if(workmode == WORK_MODE_BOOT || workmode == WORK_MODE_SPRITE_RECOVERY)
	{
		ret = (sunxi_flash_mmc_secread(item, align, blkcnt) == blkcnt) ? 0 : -1;
	}
	else if ((workmode & WORK_MODE_PRODUCT) || (workmode == 0x30))
	{
		ret = sunxi_sprite_mmc_secread(item, align, blkcnt);
	}
	else
	{
		printf("workmode=%d is err\n", workmode);
		free(map_copy0_buf);
		return -1;
	}
	if (!ret)
	{
		/*read ok*/
		ret = check_secure_storage_map(align);
		if (ret == 0)
		{
			printf("the secure storage item0 copy0 is good\n");
			goto ok ; /*copy 0 pass*/
		}else if (ret == 2){
			memcpy(map_copy0_buf, align, len);
			have_map_copy0 = 1;
			printf("the secure storage item0 copy0 is bad\n");
		}else
			printf("the secure storage item0 copy 0 crc fail, the data is bad\n");
	}

	// read backup
	memset(align, 0x0, len);
	printf("read item%d copy1\n", item);
	if(workmode == WORK_MODE_BOOT || workmode == WORK_MODE_SPRITE_RECOVERY)
	{
		ret = (sunxi_flash_mmc_secread(item, align, blkcnt) == blkcnt) ? 0 : -1;
	}
	else if ((workmode & WORK_MODE_PRODUCT) || (workmode == 0x30))
	{
		ret = sunxi_sprite_mmc_secread(item, align, blkcnt);
	}
	else
	{
		printf("workmode=%d is err\n", workmode);
		free(map_copy0_buf);
		return -1;
	}
	if (!ret)
	{
		ret = check_secure_storage_map(align);
		if (ret == 0){
			printf("the secure storage item0 copy1 is good\n");
			goto ok ;
		}else if (ret == 2){
			if (have_map_copy0 && !memcmp(map_copy0_buf, align, len))
			{
				printf("the secure storage item0 copy0 == copy1, the data is good\n");
				goto ok ; /*copy have no magic and crc*/
			}
			else
			{
				printf("the secure storage item0 copy0 != copy1, the data is bad\n");
				free(map_copy0_buf);
				return -1;
			}
		}else{
			printf("the secure storage item0 copy 1 crc fail, the data is bad\n");
			free(map_copy0_buf);
			return -1;
		}
	}
	printf("unknown error happen in item 0 read\n");
	free(map_copy0_buf);
	return -1 ;

ok:
	if(((unsigned int)buf%32))
		memcpy(buf,align,len);
	free(map_copy0_buf);
	return 0 ;
}



