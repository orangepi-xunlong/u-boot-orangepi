/*
 * (C) Copyright 2018-2020
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * ouyangkun <ouyangkun@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <asm/io.h>
#include <smc.h>
#include <fdt_support.h>
#include <sys_partition.h>
#include <memalign.h>
#include <sunxi_image_verifier.h>
#include <openssl_ext.h>
#include <securestorage.h>
#include "usb.h"
#include <asm/arch/clock.h>
#include "sunxi_challenge.h"

#define DEBUG_PRINT 0

enum MIPS_DATA_TYPE_t {
	MIPS_CODE,
	/*
	 * order db->db_table->db_project if critical
	 * since *end of first one if *start of next one
	 */
	MIPS_DATABASE,
	MIPS_PROJECT_TABLE,
	MIPS_DB_PROJECT,
	MIPS_XML
};
const char *mips_names[] = { [MIPS_CODE]	  = "mips_code",
			     [MIPS_PROJECT_TABLE] = "mips_project_table",
			     [MIPS_DATABASE]	  = "mips_database",
			     [MIPS_DB_PROJECT]	  = "mips_db_project",
			     [MIPS_XML]		  = "mips_xml" };
struct mips_data_info_t {
	char file_name[64];
	uint32_t file_len;
	char cert_name[64];
	uint32_t cert_len;
	uint32_t load_addr;
	char str_addr[64];
	uint8_t flag;
};
#define FILE_NAME_NOT_FOUNT (1 << 0)
#define CERT_NAME_NOT_FOUNT (1 << 1)
#define LOAD_ADDR_NOT_FOUNT (1 << 2)
extern int do_sunxi_flash(cmd_tbl_t *cmdtp, int flag, int argc,
			  char *const argv[]);
static int usb_debug_enabled;

static void load_setting_from_fdt(struct mips_data_info_t *info, int index)
{
	char *pstr_tmp;
	int node;
	char node_name[64];
	sprintf(node_name, "/firmware/mips/%s", mips_names[index]);
	node = fdt_path_offset(working_fdt, node_name);
	if (fdt_getprop_string(working_fdt, node, "file_name", &pstr_tmp) < 0)
		info->flag |= FILE_NAME_NOT_FOUNT;
	else
		strcpy(info->file_name, pstr_tmp);

	if (fdt_getprop_string(working_fdt, node, "cert_name", &pstr_tmp) < 0)
		info->flag |= CERT_NAME_NOT_FOUNT;
	else
		strcpy(info->cert_name, pstr_tmp);

	if (fdt_getprop_u32(working_fdt, node, "load_addr", &info->load_addr) <
	    0)
		info->flag |= LOAD_ADDR_NOT_FOUNT;
	else
		sprintf(info->str_addr, "%x", info->load_addr);

	if (index == MIPS_DB_PROJECT) {
		char projectID_name[64];
		int projectID_len;
		char tmp_name[64];
		/*project database file name build dynamically*/
		if (sunxi_secure_object_read("mips_projectID", projectID_name,
					     64, &projectID_len)) {
			strcpy(projectID_name, "0x0001");
		}
		strcpy(tmp_name, info->file_name);

		snprintf(info->cert_name, sizeof(info->cert_name), "%s%s%s",
			 tmp_name, projectID_name, ".der");
		info->flag &= ~CERT_NAME_NOT_FOUNT;

		snprintf(info->file_name, sizeof(info->file_name), "%s%s%s",
			 tmp_name, projectID_name, ".TSE");
		info->flag &= ~FILE_NAME_NOT_FOUNT;
	}

	if (index == MIPS_DB_PROJECT || index == MIPS_PROJECT_TABLE) {
		/*addr decided by prev item *end, not need to load from fdt*/
		info->flag &= ~LOAD_ADDR_NOT_FOUNT;
	}
#if DEBUG_PRINT == 1
	pr_err(" file_name:%s\n", info->file_name);
	pr_err(" cert_name:%s\n", info->cert_name);
	pr_err(" load_addr:%x\n", info->load_addr);
	pr_err(" flag		:%x\n", info->flag);
#endif
}

enum USB_VERIFY_ST_t {
	NOT_DONE,
	/*
	 * order db->db_table->db_project if critical
	 * since *end of first one if *start of next one
	 */
	PASS,
	FAILED
};
static enum USB_VERIFY_ST_t usb_key_valid;

enum read_source_en {
	FROM_PART,
	FROM_USB,
};
struct read_param_t {
	char *cmd;
	char interface[16];
	char dev[16];
	char *addr;
	char *file_name;
	char *null;
};
static int prepare_read_argv(struct read_param_t *read_argv)
{
	int partno = -1;

	memset(&read_argv[FROM_PART], 0, sizeof(struct read_param_t));
	partno = sunxi_partition_get_partno_byname("mips"); /*android*/
	if (partno >= 0) {
		read_argv[FROM_PART].cmd = "fatload";
		strcpy(read_argv[FROM_PART].interface, "sunxi_flash");
		snprintf(read_argv[FROM_PART].dev, 16, "0:%x", partno);
	} else {
		pr_err("Get mips partition number fail!\n");
		return -1;
	}

	memset(&read_argv[FROM_USB], 0, sizeof(struct read_param_t));

#ifdef CONFIG_USB_STORAGE
	char *str = env_get("load_mips_from_usb");
	if ((str) && (strcmp(str, "1") == 0)) {
		/*todo: get a better place to init usb_debug_enabled*/
		usb_debug_enabled = 1;
	}
	if (usb_debug_enabled) {
		run_command("usb start", CMD_FLAG_ENV);
		if (!usb_stor_info()) {
			read_argv[FROM_USB].cmd = "fatload";
			strcpy(read_argv[FROM_USB].interface, "usb");
			snprintf(read_argv[FROM_USB].dev, 16, "0");
			usb_key_valid = NOT_DONE;
		} else {
			/*no usb storage found, disable usb_debug to skip useless try*/
			usb_debug_enabled = 0;
		}
	}
#endif
	return 0;
}

static void verify_usb_key(struct read_param_t *read_argv)
{
	char *tmp_argv[6];
	unsigned char *key_buf = memalign(CACHE_LINE_SIZE, 1024 * 16);
	char key_buf_str[24];
	if (!key_buf) {
		usb_key_valid = FAILED;
	}
	snprintf(key_buf_str, 24, "%x", (uint32_t)key_buf);
	tmp_argv[0] = read_argv[FROM_USB].cmd;
	tmp_argv[1] = read_argv[FROM_USB].interface;
	tmp_argv[2] = read_argv[FROM_USB].dev;
	tmp_argv[3] = key_buf_str;
	tmp_argv[4] = "usb_debug_key.der";
	if (do_fat_fsload(0, 0, 5, tmp_argv)) {
		pr_msg("unable to open %s\n from usb", tmp_argv[4]);
		usb_key_valid = FAILED;
	} else {
		unsigned char *salted_challenge =
			memalign(CACHE_LINE_SIZE, 512);
		memset(salted_challenge, 0, 512);
		strcpy((char *)salted_challenge,
		       "salt to derived challenge into mips key challenge");
		memcpy(&salted_challenge[128], sunxi_challenge,
		       sunxi_challenge_len);
		if (sunxi_verify_mips(salted_challenge,
				      128 + sunxi_challenge_len, key_buf,
				      1024 * 16)) {
			pr_err("verify usb debug key failed\n");
			usb_key_valid = FAILED;
		} else {
			usb_key_valid = PASS;
		}
	}
	free(key_buf);
	return;
}

static int try_fat_fload(struct read_param_t *read_argv,
			 struct mips_data_info_t *mips_data_info,
			 char *cert_addr)
{
	char *tmp_argv[6];
	uint32_t *file_len;
	int read_src = -1;

	if (usb_debug_enabled) {
		if (usb_key_valid == NOT_DONE)
			verify_usb_key(read_argv);

		if (usb_key_valid == PASS) {
			tmp_argv[0] = read_argv[FROM_USB].cmd;
			tmp_argv[1] = read_argv[FROM_USB].interface;
			tmp_argv[2] = read_argv[FROM_USB].dev;
			if (!cert_addr) {
				tmp_argv[3] = mips_data_info->str_addr;
				tmp_argv[4] = mips_data_info->file_name;
				file_len    = &mips_data_info->file_len;
			} else {
				tmp_argv[3] = cert_addr;
				tmp_argv[4] = mips_data_info->cert_name;
				file_len    = &mips_data_info->cert_len;
			}
			tmp_argv[5] = NULL;
			if (do_fat_fsload(0, 0, 5, tmp_argv)) {
				pr_msg("unable to open %s\n from usb",
				       tmp_argv[4]);
			} else {
				read_src = FROM_USB;
				goto read_done;
			}
		}
	}

	tmp_argv[0] = read_argv[FROM_PART].cmd;
	tmp_argv[1] = read_argv[FROM_PART].interface;
	tmp_argv[2] = read_argv[FROM_PART].dev;
	if (!cert_addr) {
		tmp_argv[3] = mips_data_info->str_addr;
		tmp_argv[4] = mips_data_info->file_name;
		file_len    = &mips_data_info->file_len;
	} else {
		tmp_argv[3] = cert_addr;
		tmp_argv[4] = mips_data_info->cert_name;
		file_len    = &mips_data_info->cert_len;
	}
	tmp_argv[5] = NULL;
	if (do_fat_fsload(0, 0, 5, tmp_argv)) {
		pr_err("unable to open %s\n", tmp_argv[4]);

		return -1;
	}
	read_src = FROM_PART;

read_done:
	*file_len = simple_strtoul(env_get("filesize"), NULL, 16);
#if DEBUG_PRINT == 1
	if (!cert_addr)
		pr_err(" file_len	:%x\n", mips_data_info->file_len);
	else
		pr_err(" cert_len	:%x\n", mips_data_info->cert_len);
#endif
	pr_msg("load %s from %s\n", tmp_argv[4],
	       read_src == FROM_USB ? "usb" : "part");

	return 0;
}

__maybe_unused static void
dump_mips_data_info(struct mips_data_info_t *mips_data_info)
{
	printf("file_name:%s\n"
	       "file_len:%d\n"
	       "cert_name:%s\n"
	       "cert_len:%d\n"
	       "load_addr:%d\n"
	       "str_addr:%s\n"
	       "flag:%d\n",
	       mips_data_info->file_name, mips_data_info->file_len,
	       mips_data_info->cert_name, mips_data_info->cert_len,
	       mips_data_info->load_addr, mips_data_info->str_addr,
	       mips_data_info->flag);
}

static int load_mips_data(uint32_t *run_addr)
{
	int i;
	int ret = -1;
	char cert_addr[32];
	uint8_t *cert_tmp = NULL;
	struct mips_data_info_t mips_data_info[ARRAY_SIZE(mips_names)];

	struct read_param_t read_argv[2];
	if (prepare_read_argv(read_argv)) {
		return -1;
	}

	/*prepare buffer for cert verify*/
	if (sunxi_get_secureboard()) {
		cert_tmp = memalign(CACHE_LINE_SIZE, 8192);
		if (!cert_tmp) {
			pr_err("no memory for cert tmp\n");
			goto err;
		} else {
			sprintf(cert_addr, "%x", (uint32_t)cert_tmp);
#if DEBUG_PRINT == 1
			pr_err(" cert_addr	:%s\n", cert_addr);
#endif
		}
	}

	memset(&mips_data_info, 0, sizeof(mips_data_info));
	for (i = 0; i < ARRAY_SIZE(mips_names); i++) {
		load_setting_from_fdt(&mips_data_info[i], i);

		if (mips_data_info[i].flag &
		    (FILE_NAME_NOT_FOUNT | LOAD_ADDR_NOT_FOUNT)) {
			pr_err("no info for %s\n", mips_names[i]);
			goto err;
		}
	}

	for (i = 0; i < ARRAY_SIZE(mips_names); i++) {
		if (try_fat_fload(read_argv, &mips_data_info[i], NULL)) {
			goto err;
		}
		/*
		 * db,projecttable,db_project should cat toghter,
		 * use prev *end as next *start
		 */
		switch (i) {
		case MIPS_DATABASE:
		case MIPS_PROJECT_TABLE:
			mips_data_info[i + 1].load_addr =
				mips_data_info[i].load_addr +
				mips_data_info[i].file_len;
			sprintf(mips_data_info[i + 1].str_addr, "%x",
				mips_data_info[i + 1].load_addr);
			break;
		}

		/*load cert and verify, xml do not need verify*/
		if (sunxi_get_secureboard() && i != MIPS_XML) {
			if (mips_data_info[i].flag & CERT_NAME_NOT_FOUNT)
				pr_err("no cert for %s\n", mips_names[i]);
			if (try_fat_fload(read_argv, &mips_data_info[i],
					  cert_addr))
				goto err;

			if (sunxi_verify_mips(
				    (uint8_t *)mips_data_info[i].load_addr,
				    mips_data_info[i].file_len, cert_tmp,
				    mips_data_info[i].cert_len)) {
				pr_err("verify %s failed\n",
				       mips_data_info[i].file_name);
				goto err;
			}
		}
	}

	ret	  = 0;
	*run_addr = mips_data_info[MIPS_CODE].load_addr;
err:
	if (cert_tmp)
		free(cert_tmp);
	return ret;
}

__maybe_unused static void release_mips(uint32_t img_addr)
{
#define MIPS_CFG_BASE 0x3061000
	/* set mips clock to 400M*/
	uint32_t src_clk = clock_get_pll6() * 2;
	uint32_t div	 = src_clk / 400;
	uint32_t reg	 = readl(SUNXI_CCM_BASE + 0x600);
	reg &= ~(0x7);
	reg |= div - 1;
	writel(reg, SUNXI_CCM_BASE + 0x600);

	/*enable mips clock, but dont release mips, set mips table addr first*/
	reg |= 1 << 31;
	writel(reg, SUNXI_CCM_BASE + 0x600);
	writel(0x00030001, SUNXI_CCM_BASE + 0x60C);
	udelay(100); /*wait module ready for configuration*/
	writel(img_addr, MIPS_CFG_BASE + 0x30);

	/*mips table addr set up done, now release mips*/
	writel(0x00070001, SUNXI_CCM_BASE + 0x60C);
}

int do_boot_mips(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	uint32_t img_addr;
	int ret = -1;

	if (strcmp("start", argv[1]) == 0) {
		if (load_mips_data(&img_addr) == 0) {
			uint32_t mips_only_size, shm_size;
#if defined(FPGA_PLATFORM)
			/*init uart gpio*/
			writel(0x00000200, SUNXI_PIO_BASE + 0x34);
#endif
			int node;
			/*read memory layout setting*/
			mips_only_size = 0;
			shm_size       = 0;
			node	       = fdt_path_offset(working_fdt,
						 "/firmware/mips/mips_memory");
			if (node >= 0) {
				fdt_getprop_u32(working_fdt, node,
						"mips_only_size",
						&mips_only_size);
				fdt_getprop_u32(working_fdt, node, "shm_size",
						&shm_size);
#if DEBUG_PRINT == 1
				pr_err("mips_only_size %x\n", mips_only_size);
				pr_err("shm_size %x\n", shm_size);
#endif
			}
			if (!mips_only_size && !shm_size) {
				pr_err("no info for mips memory\n");
				ret = -1;
			} else {
#if 0 //do not release mips for now
				if (sunxi_get_secureboard()) {
					smc_tee_setup_mips(img_addr,
							   mips_only_size);
				} else {
					release_mips(img_addr);
				}
#endif
				ret = fdt_add_mem_rsv(working_fdt, img_addr,
						      mips_only_size +
							      shm_size);
				if (ret) {
					pr_err("##add mem rsv error: %s : %s\n",
					       __func__, fdt_strerror(ret));
				}
			}
		}
	}
	return ret;
}

static char boot_mips_help[] = "start - boot mips application\n";

U_BOOT_CMD(sunxi_mips, CONFIG_SYS_MAXARGS, 1, do_boot_mips,
	   "boot application image from memory", boot_mips_help);
