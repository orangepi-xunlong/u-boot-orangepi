/*
 * (C) Copyright 2016
 *Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <command.h>
#include <common.h>
#include <environment.h>
#include <fs.h>
#include <malloc.h>
#include <sprite.h>
#include <sunxi_board.h>
#include <sunxi_flash.h>
#include <sunxi_mbr.h>
#include <sys_partition.h>

/*debug printf, undefine it,will be clos most of printf */
#define DEBUG
#ifdef DEBUG
#define part_debug(format, ...) printf(format, ##__VA_ARGS__)
#else
#define part_debug(format, ...)
#endif

/*To define path of img in sdcar */
#define FS_PATH "/update"
/*
 *if you set FS_PATH="/update",
 *those image and verify file need to put this dir, like follow:
 *boot.img #kernel img
 *boot.sum #kernel verify file
 *rootfs.fex
 *rootfs.sum
 *...
 * */

/*define the buf size of file that will be read from sdcard */
#define BUFFSIZE 16 * 1024 * 1024

/*define it,that will update uboot*/
#define UPDATE_UBOOT

/*define it, the partiiton will be erase befor writed*/
#define ERASE_PART

extern int do_card0_probe(cmd_tbl_t *cmdtp, int flag, int argc,
			  char *const argv[]);
extern int sunxi_sprite_download_uboot(void *buffer, int production_media,
				       int generate_checksum);
extern int sunxi_sprite_download_boot0(void *buffer, int production_media);

static int update_write_flash(uint start, uint nblock, void *buffer)
{
	return sunxi_flash_write(start, nblock, buffer);
}

static int update_get_partition_info(int index, disk_partition_t *info)
{
	struct blk_desc *desc;
	int ret;

	desc = blk_get_devnum_by_typename("sunxi_flash", 0);
	if (desc == NULL) {
		printf("%s: get desc fail\n", __func__);
		ret = -ENODEV;
	}

	ret = part_get_info(desc, index, info);
	part_debug("%s: try part %d, ret = %d\n", __func__, index, ret);
	if (ret < 0) {
		printf("partno erro : can't find Num %d partition \n", index);
		return -1;
	}

	return 0;
}

static void update_next_process(int flag)
{
	if (flag) {
		printf("update finishing, will be reboot \n");
		get_boot_storage_type() == STORAGE_NAND ? sunxi_sprite_exit(0) : 0;
		udelay(3000000);

		/*wil not return and reboot system*/
		reset_cpu(0);
		for (;;)
			;
	} else {
		printf("Nothing updated, start normal boot\n");
	}
}

static int check_sdcard_exsit(void)
{
	printf("sunxi_card_check...\n");

	if (do_card0_probe(NULL, 0, 0, NULL)) {
		printf("No sd-card insert,will normal boot\n");
		return -1;
	}

	printf("sd-card found.\n");
	return 0;
}

static char mmc_part[6] = "0:0";
static void check_card_mulit_partitions(void)
{
	/*To check whether if 0:0 or 0*/
	if (fs_set_blk_dev("mmc", "0:0", FS_TYPE_FAT)) {
		/*try "0:0" fail so modify "0" */
		sprintf(mmc_part, "0");
	} else {
		sprintf(mmc_part, "0:0");
	}
	part_debug("try mmc %s success \n", mmc_part);
}

static int check_file_exsit(char *filename)
{
	int ret;

	if (filename == NULL) {
		return -1;
	}

	if (fs_set_blk_dev("mmc", mmc_part, FS_TYPE_FAT)) {
		printf("fs set mmc blk dev fail!\n");
		return -1;
	}
	ret = fs_exists(filename);
	if (!ret) {
		part_debug("not found %s file \n", filename);
		return -1;
	}

	return 0;
}

static int fat_read(const char *filename, void *buf, int offset, int len)
{
	unsigned long time;
	int len_read;
	loff_t actread;
	ulong load_addr;

	if ((buf == NULL) || (filename == NULL))
		return -1;

	load_addr = (ulong)buf;

	if (fs_set_blk_dev("mmc", mmc_part, FS_TYPE_FAT)) {
		printf("fs set mmc blk dev fail!\n");
		return -1;
	}

	time = get_timer(0);
	len_read = fs_read(filename, load_addr, offset, len, &actread);
	time = get_timer(time);
	if (len_read < 0)
		return -1;

	len_read = actread;

	printf("%d bytes read in %lu ms", len_read, time);
	if (time > 0) {
		puts(" (");
		print_size(len_read / time * 1000, "/s");
		puts(")");
	}
	puts("\n");

	return len_read;
}

static int check_file_checksum(const char *part_name, void *buf,
			       unsigned int len, unsigned int *sum_val)
{
	int ret;
	char file_sum[32] = {0};
	unsigned int a_sum_val, env_sum_val;
	char env_sum_name[32] = {0};
	char *env_sum = NULL;
	unsigned int *f_sum_val = NULL;

	f_sum_val =
	    (unsigned int *)memalign(CONFIG_SYS_CACHELINE_SIZE,
				 ALIGN((4 * 1024), CONFIG_SYS_CACHELINE_SIZE));
	if (f_sum_val == NULL) {
		printf("cant malloc buff \n");
		return -1;
	}

	/* checking checksum file about the img */
	sprintf(file_sum, "%s/%s.sum", FS_PATH, part_name);
	if (check_file_exsit(file_sum)) {
		part_debug("not find %s file \n", file_sum);
		goto fail;
	}
	ret = fat_read(file_sum, f_sum_val, 0, 0);
	part_debug("sum size :%d \n", ret);
	if (ret <= 0)
		goto fail;

	/*get buf checksum*/
	a_sum_val = add_sum(buf, len);
	part_debug("f_sum:%08x a_sum:%08x \n", *f_sum_val, a_sum_val);

	/*comparison checksum*/
	if (a_sum_val != *f_sum_val) {
		printf("comparison %s.sum fail \n", part_name);
		goto fail;
	}

	*sum_val = a_sum_val;

	/*checking env checksum*/
	/*get env checksum*/
	sprintf(env_sum_name, "%s_sum", part_name);
	env_sum = env_get(env_sum_name);
	if (env_sum == NULL) /* if not found part_check in env,so update*/
		goto update;

	/*comparison checksum*/
	env_sum_val = simple_strtoul(env_sum, NULL, 16);
	part_debug("env_sum:%08x file_sum:%08x \n", env_sum_val, a_sum_val);
	if (env_sum_val != a_sum_val)
		goto update;
	else
		goto fail;

fail:
	if (f_sum_val)
		free(f_sum_val);
	return -1;

update:
	if (f_sum_val)
		free(f_sum_val);
	return 0;
}

static void save_checksum_env(const char *part_name, unsigned int sum_val)
{
	char env_name[32] = {0};
	char env_sum_str[9] = {0};

	sprintf(env_name, "%s_sum", part_name);
	sprintf(env_sum_str, "%x", sum_val);
	part_debug("save %s=%s \n", env_name, env_sum_str);

	env_set(env_name, env_sum_str);
	env_save();
}

#ifdef ERASE_PART
static int erase_flash_by_part_name(const char *part_name)
{
	int ret;
	unsigned int part_offset, part_size;

	/*check part exist*/
	ret = sunxi_partition_get_info_byname(part_name, &part_offset,
					      &part_size);
	if (ret) {
		printf("can not find %s partition, not erase \n", part_name);
		return -1;
	}

	part_debug("erase %s partition, start:%d, size:%d \n", part_name,
		   part_offset, part_size);

	/* call erase function*/
	ret = sunxi_flash_erase_area(part_offset, part_size);

	return ret;
}
#endif

#ifdef UPDATE_UBOOT
/*uboot file list*/
struct uboot_part_t {
	char *name;
	int size;
};

struct uboot_part_t uboot_part[] = {
    {"uboot",
     (CONFIG_SPINOR_LOGICAL_OFFSET - CONFIG_SPINOR_UBOOT_OFFSET) * 512},
    {"boot0", (CONFIG_SPINOR_UBOOT_OFFSET * 512)},
    {NULL, 0},
};

static int update_uboot_partition(char *file_buf, int *update_flag)
{
	int i;
	int ret = -1;
	char file_name[32] = {0};
	int file_size;
	int part_size;
	unsigned int sum_val = 0;
	/*To get what flash in the board */
	int storage_type = get_boot_storage_type();

	for (i = 0; uboot_part[i].name != NULL; i++) {
		part_size = uboot_part[i].size;
		part_debug("part_name:%s part_size:%d \n", uboot_part[i].name,
			   part_size);

		/*Get the image filename*/
		memset(file_name, 0, sizeof(file_name));
		sprintf(file_name, "%s/%s.img", FS_PATH, uboot_part[i].name);
		part_debug("To find %s file \n", file_name);

		/*to checking the file exsit in sdcard */
		if (check_file_exsit(file_name))
			continue;

		/*loading the img file to buf
		 * and checking the size whether if over part_size*/
		file_size = fat_read(file_name, file_buf, 0, 0);
		part_debug("read file size:%d \n", file_size);
		if (file_size <= 0) {
			printf("load %s error \n", file_name);
			continue;
		}

		/*check_sum file and env*/
		if (check_file_checksum(uboot_part[i].name, file_buf, file_size,
					&sum_val)) {
			printf("%s checksum fial \n", uboot_part[i].name);
			continue;
		}

		/*check the part size and file size*/
		if (file_size > part_size) {
			printf("%s.img size > %s partition size, will not be "
			       "update\n",
			       uboot_part[i].name, uboot_part[i].name);
			continue;
		} else {
			/*To writ down the uboot or boot0 image*/
			if (!strncmp("uboot", uboot_part[i].name, 5)) {
				ret = sunxi_sprite_download_uboot(
				    (void *)file_buf, storage_type, 0);
				*update_flag = 1;
			} else if (!strncmp("boot0", uboot_part[i].name, 5)) {
				ret = sunxi_sprite_download_boot0(
				    (void *)file_buf, storage_type);
				*update_flag = 1;
			}
			if (ret < 0) {
				printf("update %s fail \n", uboot_part[i].name);
			} else {
				printf("update %s success \n",
				       uboot_part[i].name);
				save_checksum_env(uboot_part[i].name, sum_val);
			}
		}
	}
	return ret;
}
#endif

#define ENV_UPDATE_OVERWRITE 0 /*whether need to overwrite to update env data*/
static int part_env_import(const char *buf, int check)
{
	env_t *ep = (env_t *)buf;

	if (check) {
		uint32_t crc;

		memcpy(&crc, &ep->crc, sizeof(crc));

		if (crc32(0, ep->data, ENV_SIZE) != crc) {
			printf("env date: bad CRC, can't update env \n");
			return -1;
		}
	}

	if (himport_r(&env_htab, (char *)ep->data, ENV_SIZE, '\0',
			   ENV_UPDATE_OVERWRITE ? 0 : H_NOCLEAR, 0, 0, NULL)) {
		return 0;
	}

	pr_err("Cannot import environment: errno = %d\n", errno);

	return -1;
}

static int update_partition(const char *part_name, void *buf, int size)
{
	unsigned int start_block = 0;
	unsigned int rblock = 0;
	int ret;

#ifdef ERASE_PART
	ret = erase_flash_by_part_name(part_name);
	if (ret) {
		printf("erase %s fail \n", part_name);
		return -1;
	}
#endif
	/*if the name is env partitions, will write uboot env buf and then save
	 * to flash. it can save old env var or clear */
	if (!strncmp(part_name, "env", 3)) {
		if (!part_env_import(buf, 1))
			/* save env to flash */
			env_save();
		else
			return -1;

	} else { /*other paritions update*/
		start_block =
		    sunxi_partition_get_offset_byname((const char *)part_name);
		rblock = size / 512;

		ret = update_write_flash(start_block, rblock, (void *)buf);
		part_debug("sunxi flash write :offset %x, %d bytes %s\n",
			   start_block << 9, rblock << 9, ret ? "OK" : "ERROR");
		return ret == 0 ? -1 : 0;
	}
	return 0;
}

int update_main(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	int ret;
	int i;
	int update_flag = 0;
	char part_name[32] = {0};
	char file_name[32] = {0};
	char *file_buf = NULL;
	int file_size;
	int part_size;
	unsigned int sum_val = 0;
	disk_partition_t info;

	/*check sdcar*/
	if (check_sdcard_exsit()) {
		return 0;
	}

	/* To check sdcard whethter if mulit-partition */
	check_card_mulit_partitions();

	/*malloc buf
	* for fatload, if not aligned,a misaligned buffer warning will be
	* printed
	* and performance will suffer for the load.
	*/
	file_buf =
	    (char *)memalign(CONFIG_SYS_CACHELINE_SIZE, ALIGN(BUFFSIZE,
						    CONFIG_SYS_CACHELINE_SIZE));
	if (file_buf == NULL) {
		printf("can not malloc %d byte size \n", BUFFSIZE);
		return 0;
	}

	/*To find each part_img */
	for (i = 1; i < 128; i++) {
		/*To found image in sdcard by partition name */
		ret = update_get_partition_info(i, &info);
		if (ret) {
			printf("To find part end \n");
			break;
		}

		strncpy(part_name, (const char *)info.name, 32);
		part_size = info.size * 512;
		part_debug("part_name:%s part_size:%d \n", part_name,
			   part_size);

		/*Get the image filename*/
		memset(file_name, 0, sizeof(file_name));
		sprintf(file_name, "%s/%s.img", FS_PATH, part_name);
		part_debug("To find %s file \n", file_name);

		/*to checking the file exsit in sdcar */
		if (check_file_exsit(file_name))
			continue;

		/*loading the img file to buf
		 * and checking the size whether if over part_size*/
		file_size = fat_read(file_name, file_buf, 0, 0);
		part_debug("read file size:%d \n", file_size);
		if (file_size <= 0) {
			printf("load %s error \n", file_name);
			continue;
		}

		/*check_sum file and env*/
		if (check_file_checksum(part_name, file_buf, file_size,
					&sum_val)) {
			printf("%s checksum fial \n", part_name);
			continue;
		}

		/*check the part size and file size*/
		if (file_size > part_size) {
			printf("%s.img size > %s partition size, will not be "
			       "update\n",
			       part_name, part_name);
			continue;
		} else {
			/*wirte down the file to flash, there will 512 byte
			 * aligned*/

			if ((file_size + 512) <= part_size) {
				/* set 0xff to the file_date end */
				memset((file_buf + file_size), 0xff, 512);
				ret = update_partition(part_name, file_buf,
						       (file_size + 512));
			} else {
				/* set 0xff to the file_date end */
				memset((file_buf + file_size), 0xff,
						       (part_size - file_size));
				ret = update_partition(part_name, file_buf,
						       part_size);
			}
			/*if updated, need reboot*/
			update_flag = 1;

			/*save the file checksum to env */
			if (!ret)
				save_checksum_env(part_name, sum_val);
		}
	}

#ifdef UPDATE_UBOOT
	/*update uboot/boot0 process*/
	update_uboot_partition(file_buf, &update_flag);
#endif

	if (file_buf)
		free(file_buf);

	sunxi_flash_write_end();
	/*flush the flash*/
	sunxi_flash_flush();
	sunxi_sprite_flush();

	/*To decide to normal boot or reboot*/
	update_next_process(update_flag);
	return 0;
}

U_BOOT_CMD(part_update, CONFIG_SYS_MAXARGS, 1, update_main, "part_update",
	   "you need save part image to sdcard and run it \n");
