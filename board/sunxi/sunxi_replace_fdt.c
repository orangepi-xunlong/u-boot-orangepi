/*
 * (C) Copyright 2018-2020wangwei@allwinnertech.com>
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 *
 * SPDX-License-Identifier:>GPL-2.0+
 */
#include <common.h>
#include <fdt_support.h>
#include <sys_partition.h>
#include <image.h>
#include <android_image.h>

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_SUNXI_REPLACE_FDT_FROM_PARTITION

static int parse_replace_fdt(char *replace_fdt,
		 char *partition_name, char *partition_type, char *file_name)
{
	char *str;
	char tmp[256] = {0};
	if (strlen(replace_fdt) > 256) {
		pr_err("replace_fdt is longer than 256 characters\n");
		return -1;
	}
	strcpy(tmp, replace_fdt);

	str = strtok(tmp, ":");
	if (str) {
		snprintf(partition_name, 128, "%s", str);
	}

	str = strtok(NULL, ":");
	if (str) {
		snprintf(partition_type, 128, "%s", str);
	} else {
		return 0;
	}

	str = strtok(NULL, ":");
	if (str) {
		snprintf(file_name, 128, "%s", str);
	} else {
		return 0;
	}

	return 0;
}

static int __sunxi_replace_fdt(void *buffer)
{
	int err;
	u32 new_fdt_totalsize;
	err = fdt_check_header(buffer);

	if (err < 0) {
		pr_err("libfdt fdt_check_header(): %s\n", fdt_strerror(err));
		return -1;
	} else {
		pr_msg("fdt check ok\n");
	}
	new_fdt_totalsize = fdt_totalsize(buffer);
	if (new_fdt_totalsize > (gd->fdt_size)) {
		pr_err("new fdt(0x%08x) biger than now fdt(0x%08x)\n",
			 new_fdt_totalsize, gd->fdt_size);
		return -1;
	}

	memcpy((void *)gd->fdt_blob, buffer, new_fdt_totalsize);

	/* fdt_size is the space reserved by uboot for fdt, now set to new fdt */
	fdt_set_totalsize((void *)gd->fdt_blob, gd->fdt_size);

	return 0;
}

static int sunxi_replace_fdt_from_raw(char *partition_name)
{
	disk_partition_t disk_part_info;
	int ret;
	void *buffer = NULL;

	if (!sunxi_partition_get_info(partition_name, &disk_part_info)) {
		buffer = malloc(disk_part_info.size * 512);
		if (buffer) {
			memset(buffer, 0xff, disk_part_info.size * 512);
			ret = sunxi_flash_read(disk_part_info.start, disk_part_info.size, buffer);
			if (ret) {
				if (__sunxi_replace_fdt(buffer)) {
					goto error;
				}
			} else {
				pr_err("replaced fdt : read raw partition(%s): start,0x%x sectors 0x%x fail\n",
				 partition_name, disk_part_info.start, disk_part_info.size);
				goto error;
			}
			pr_force("replace fdt from raw partition(%s) success\n", partition_name);
		} else {
			pr_err("replace fdt : malloc fail\n");
			return -1;
		}
	} else {
		pr_err("%s partition is not exist\n", partition_name);
		return -1;
	}

	return 0;

error:
	free(buffer);
	return -1;
}

extern int do_fat_size(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
static ulong __get_fdt_file_size_from_fat(int partno, char *file_name)
{
	char *filesize_old = env_get("filesize");
	char part_info[16] = {0};  /* format is "0:partno" */
	char file_info[64] = {0};
	char *bmp_argv_get_file_size[5] = { "fatsize", "sunxi_flash", part_info,
		 file_info, NULL };
	ulong size;

	/* clean filesize value */
	env_set("filesize", "");
	sprintf(part_info, "0:%x", partno);
	sprintf(file_info, "%s", file_name);
	if (!do_fat_size(0, 0, 4, bmp_argv_get_file_size)) {
		size = env_get_hex("filesize", 0);
		/* has get filesize value, clean filesize value */
		env_set("filesize", "");
	} else {
		pr_err("get file(%s) size error\n", file_name);
		return 0;
	}

	/* save the old filesize back to env*/
	if (filesize_old) {
		env_set("filesize", filesize_old);
	}
	return size;
}

static int __load_fdt_file_from_fat(int partno, char *file_name, void *buffer)
{
	char part_info[16] = {0};  /* format is "0:partno" */
	char addr_info[32] = {0};  /* "00000000" */
	char file_info[64] = {0};
	char *bmp_argv_load_file[6] = { "fatload", "sunxi_flash", part_info,
		 addr_info, file_info, NULL };

	sprintf(part_info, "0:%x", partno);
	sprintf(addr_info, "%lx", (ulong)buffer);
	sprintf(file_info, "%s", file_name);
	if (do_fat_fsload(0, 0, 5, bmp_argv_load_file)) {
		pr_err("load file(%s) error\n", file_name);
		return -1;
	}
	return 0;
}

static int sunxi_replace_fdt_from_fat(char *partition_name, char *file_name)
{
	int partno = -1;
	ulong filesize;
	void *buffer = NULL;

	partno = sunxi_partition_get_partno_byname(partition_name);
	if (partno >= 0) {
		filesize = __get_fdt_file_size_from_fat(partno, file_name);
		if (filesize <= 0) {
			pr_err("Please check if the file(%s) exists\n", file_name);
			return -1;
		}

		if (filesize > gd->fdt_size) {
			pr_err("filesize(%u) biger than gd->fdt_size(%u)\n", filesize, gd->fdt_size);
			return -1;
		}

		buffer = malloc_align(gd->fdt_size, 64);
		if (buffer) {
			memset(buffer, 0xff, gd->fdt_size);
			if (!__load_fdt_file_from_fat(partno, file_name, buffer)) {
				if (__sunxi_replace_fdt(buffer)) {
					goto error;
				}
			} else {
				pr_err("replaced fdt : read fat partition(%s) file(%s) fail\n",
				 partition_name, file_name);
				goto error;
			}
			pr_force("replace fdt from fat partition(%s) file(%s) success\n",
					 partition_name, file_name);
		} else {
			pr_err("replaced fdt : malloc fail\n");
			return -1;
		}
	} else {
		pr_err("%s partition is not exist\n", partition_name);
		return -1;
	}

	return 0;

error:
	free_align(buffer);
	return -1;
}

int sunxi_replace_fdt(void)
{
	char *replace_fdt = env_get("replace_fdt");
	char partition_name[128] = {0};
	char partition_type[128] = {0};
	char file_name[128] = {0};

	if (replace_fdt) {
		parse_replace_fdt(replace_fdt, partition_name, partition_type, file_name);
		if (strcmp(partition_type, "raw") == 0) {
			pr_msg("replaced fdt : partition type (%s)\n", partition_type);
			if (sunxi_replace_fdt_from_raw(partition_name)) {
				pr_err("replace fdt form raw partition(%s) fail!\n", partition_name);
			}
		} else if (strcmp(partition_type, "fat") == 0) {
			pr_msg("replaced fdt : partition type (%s)\n", partition_type);
			if (sunxi_replace_fdt_from_fat(partition_name, file_name)) {
				pr_err("replace fdt form fat partition(%s) file(%s) fail!\n",
					 partition_name, file_name);
			}
		} else {
			pr_err("replaced fdt error : partition type (%s) not support\n", partition_type);
		}
	} else {
		return 0;
	}

	return 0;
}
#endif

#if defined(CONFIG_SUNXI_NECESSARY_REPLACE_FDT)
static int __sunxi_replace_fdt_v2(void *buffer)
{
	int err;
	u32 new_fdt_totalsize;
	err = fdt_check_header(buffer);

	if (err < 0) {
		pr_err("libfdt fdt_check_header(): %s\n", fdt_strerror(err));
		return -1;
	} else {
		pr_msg("fdt check ok\n");
	}
	new_fdt_totalsize = fdt_totalsize(buffer);
	if (new_fdt_totalsize > (gd->fdt_ext_size)) {
		pr_err("new fdt(0x%08x) biger than now fdt(0x%08x)\n",
			new_fdt_totalsize, gd->fdt_ext_size);
		return -1;
	}
	pr_force("change working_fdt 0x%x to 0x%x\n", (ulong)working_fdt, (ulong)gd->new_ext_fdt);
	working_fdt = (struct fdt_header *)gd->new_ext_fdt;
	gd->fdt_blob = gd->new_ext_fdt;
	memcpy((void *)gd->new_ext_fdt, buffer, new_fdt_totalsize);
	/* fdt_size is the space reserved by uboot for fdt, now set to new fdt */
	fdt_set_totalsize((void *)gd->new_ext_fdt, gd->fdt_ext_size);

	return 0;
}


int sunxi_get_dtb(ulong *dtb_data, ulong *dtb_len)
{
	int work_mode = get_boot_work_mode();
	if (work_mode == WORK_MODE_BOOT) {
		u32 part_start;
		char env_boot_normal[8][16];
		sunxi_parsed_specific_string(env_get("boot_normal"), env_boot_normal, ' ', 0);
		u32 boot_head_addr = (u32)simple_strtoul(env_boot_normal[2], NULL, 16);

		part_start = sunxi_partition_get_offset_byname("dtb");
		if (part_start != 0) {
			*dtb_data = CONFIG_SUNXI_FDT_ADDR;
			sunxi_flash_read(part_start, ALIGN(sizeof(struct fdt_header), 512)/512, (void *)(ulong)(*dtb_data));
			*dtb_len = fdt_totalsize((char *)(*dtb_data));
			sunxi_flash_read(part_start, ALIGN(*dtb_len, 512)/512, (char *)(*dtb_data));
		}
		if (fdt_check_header((void *)*dtb_data) < 0) {
			part_start = sunxi_partition_get_offset_byname("boot");
			sunxi_flash_read(part_start, ALIGN(sizeof(struct andr_img_hdr), 512)/512, (void *)(ulong)boot_head_addr);
			android_image_get_dtb((const struct andr_img_hdr *)(ulong)boot_head_addr, dtb_data, dtb_len);
		}
	}
	return 0;
}

int sunxi_replace_fdt_v2(void)
{
	ulong dtb_data = 0, dtb_len = 0;
	sunxi_get_dtb(&dtb_data, &dtb_len);
	dtb_data = (fdt_check_header((void *)dtb_data) < 0 ? (CONFIG_SYS_TEXT_BASE + SUNXI_DTB_OFFSET) : dtb_data);
	pr_msg("replace_fdt %s addr:0x%x\n", __sunxi_replace_fdt_v2((void *)dtb_data) < 0 ? "fail" : "success", dtb_data);
	return 0;
}

int do_set_working_fdt(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	if (argc < 2) {
		return -1;
	} else {
		printf("change working_fdt 0x%lx to ", (ulong)working_fdt);
		working_fdt = (struct fdt_header *)simple_strtoul(argv[1], NULL, 16);
		printf("0x%lx\n", (ulong)working_fdt);
	}
	return 0;
}

U_BOOT_CMD(set_working_fdt, 6, 1, do_set_working_fdt, "set_working_fdt fdt_addr",
	   "set_working_fdt <fdt_addr>\n");
#endif
