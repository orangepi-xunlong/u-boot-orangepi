/*
 * MMC driver for allwinner sunxi platform , Field Firmware Update(FFU)
 *
 * (C) Copyright 2021  chenguodong
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <config.h>
#include <common.h>
#include <command.h>
#include <mmc.h>
#include <malloc.h>
#include <div64.h>
#include <sys_config.h>

#include <memalign.h>
#include <fdt_support.h>

#include <private_toc.h>
#include <sunxi_board.h>

#include "sunxi_mmc.h"
#include "mmc_def.h"

#ifdef SUPPORT_SUNXI_MMC_FFU

extern int mmc_switch(struct mmc *mmc, u8 set, u8 index, u8 value);
extern int mmc_send_ext_csd(struct mmc *mmc, u8 *ext_csd);
extern int mmc_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd,
			struct mmc_data *data);
extern int mmc_set_blocklen(struct mmc *mmc, int len);
extern int mmc_switch_ffu(struct mmc *mmc, u8 set, u8 index, u8 value,
			  u32 timeout, u8 check_status);
extern void mmc_hw_reset(struct mmc *mmc);

int mmc_get_firmware_version(struct mmc *mmc, unsigned char ver[8])
{
	ALLOC_CACHE_ALIGN_BUFFER(char, ext_csd, MMC_MAX_BLOCK_LEN);
	int ret = -1;
	int i;

	/* send ext_csd */
	ret = mmc_send_ext_csd(mmc, (u8 *)ext_csd);
	if (ret) {
		MMCINFO("mmc get ext csd failed\n");
		return ret;
	}

	//dumphex32((char *)__FUNCTION__,(char *)ext_csd,512);

	MMCINFO("current Firmware Version is");
	for (i = 261; i >= 254; i--) {
		ver[i - 254] = ext_csd[i];
		printf(" 0x%02X", ext_csd[i]);
	}
	printf("\n");

	return 0;
}

int mmc_enter_ffu_mode(struct mmc *mmc, unsigned int *ffu_arg)
{
	ALLOC_CACHE_ALIGN_BUFFER(char, ext_csd, MMC_MAX_BLOCK_LEN);
	int ret = 0;

	/* send ext_csd */
	ret = mmc_send_ext_csd(mmc, (u8 *)ext_csd);
	if (ret) {
		MMCINFO("mmc get ext csd failed\n");
		return -1;
	}

	MMCDBG("%s[%d], ext_csd[493] = 0x%02x\n", __FUNCTION__, __LINE__,
	       ext_csd[EXT_CSD_SUPPORTED_MODES]);
	if (!(ext_csd[EXT_CSD_SUPPORTED_MODES] & 1)) {
		MMCINFO("%s: FFU is not supported by the device\n",
			__FUNCTION__);
		return -EPROTONOSUPPORT;
	}

	/******/
	/*
	MMCINFO("%s: make FW updates enabled\n", __FUNCTION__);
	if (mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL, EXT_CSD_FW_CONFIG, 0)) {
	MMCINFO("%s: change device mode to FFU fail\n", __FUNCTION__);
	return -4;
	}
	MMCINFO("Bit[0]: Update_Disable 0x0: FW updates enabled.  \n");
	*/
	MMCINFO("%s: we don't set EXT_CSD_FW_CONFIG here\n", __FUNCTION__);

	/******/
	if (mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL, EXT_CSD_MODE_CONFIG, 1)) {
		MMCINFO("%s: change device mode to FFU fail\n", __FUNCTION__);
		return -EPROTO;
	}
	MMCINFO("0x01  FFU Mode  \n");

	//dumphex32((char *)__FUNCTION__,(char *)ext_csd,512);

	*ffu_arg = (ext_csd[490] << 24) + (ext_csd[489] << 16) +
		   (ext_csd[488] << 8) + ext_csd[487];
	MMCINFO(" *ffu_arg = 0x%08x\n", *ffu_arg);

	return 0;
}

int mmc_ffu_get_fw(unsigned char **fw, u32 *fw_len)
{
	int i;

	struct sbrom_toc1_head_info *toc1_head = NULL;
	struct sbrom_toc1_item_info *item_head = NULL;

	struct sbrom_toc1_item_info *toc1_item = NULL;

	toc1_head = (struct sbrom_toc1_head_info *)
		SUNXI_CFG_TOC1_STORE_IN_DRAM_BASE;
	item_head = (struct sbrom_toc1_item_info
			     *)(SUNXI_CFG_TOC1_STORE_IN_DRAM_BASE +
				sizeof(struct sbrom_toc1_head_info));

	MMCDBG("*******************TOC1 Head Message*************************\n");
	MMCDBG("Toc_name          = %s\n", toc1_head->name);
	MMCDBG("Toc_magic         = 0x%x\n", toc1_head->magic);
	MMCDBG("Toc_add_sum           = 0x%x\n", toc1_head->add_sum);

	MMCDBG("Toc_serial_num    = 0x%x\n", toc1_head->serial_num);
	MMCDBG("Toc_status        = 0x%x\n", toc1_head->status);

	MMCDBG("Toc_items_nr      = 0x%x\n", toc1_head->items_nr);
	MMCDBG("Toc_valid_len     = 0x%x\n", toc1_head->valid_len);
	MMCDBG("TOC_MAIN_END      = 0x%x\n", toc1_head->end);
	MMCDBG("***************************************************************\n\n");

	//init
	toc1_item = item_head;
	for (i = 0; i < toc1_head->items_nr; i++, toc1_item++) {
		MMCDBG("\n*******************TOC1 Item Message*************************\n");
		MMCDBG("Entry_name        = %s\n", toc1_item->name);
		MMCDBG("Entry_data_offset = 0x%x\n", toc1_item->data_offset);
		MMCDBG("Entry_data_len    = 0x%x\n", toc1_item->data_len);

		MMCDBG("encrypt           = 0x%x\n", toc1_item->encrypt);
		MMCDBG("Entry_type        = 0x%x\n", toc1_item->type);
		MMCDBG("run_addr          = 0x%x\n", toc1_item->run_addr);
		MMCDBG("index             = 0x%x\n", toc1_item->index);
		MMCDBG("Entry_end         = 0x%x\n", toc1_item->end);
		MMCDBG("***************************************************************\n\n");

		if (strncmp(toc1_item->name, ITEM_EMMC_FW_NAME,
			    sizeof(ITEM_EMMC_FW_NAME)) == 0) {
			*fw_len = toc1_item->data_len;
			*fw     = (unsigned char *)SUNXI_CFG_TOC1_STORE_IN_DRAM_BASE +
				       toc1_item->data_offset;

			MMCINFO("fw point = %x;fw len: %d sector\n",
				(unsigned int)(SUNXI_CFG_TOC1_STORE_IN_DRAM_BASE +
					       toc1_item->data_offset),
				*fw_len);
			break;
		}
	}

	if (i == toc1_head->items_nr) {
		MMCINFO("get emmc fw from toc0 fail\n");
		return -1;
	}

	return 0;
}

static int mmc_ffu_download_firmware(struct mmc *mmc, unsigned int ffu_arg,
				     int blkcnt, unsigned char *src)
{
	struct mmc_cmd cmd;
	struct mmc_data data;
	//int timeout = 1000;

	if (blkcnt == 0)
		return 0;
	else if (blkcnt == 1)
		cmd.cmdidx = MMC_CMD_WRITE_SINGLE_BLOCK;
	else
		cmd.cmdidx = MMC_CMD_WRITE_MULTIPLE_BLOCK;

	/* the argument of ffu write is different from normal write */
	cmd.cmdarg    = ffu_arg;
	cmd.resp_type = MMC_RSP_R1;
	/* send cmd12 manually, auto send cmd12 may cause download fw error, like samsung's emmc */
	mmc->manual_stop_flag = 1;

	data.src       = (const char *)src;
	data.blocks    = blkcnt;
	data.blocksize = mmc->write_bl_len;
	data.flags     = MMC_DATA_WRITE;

	if (mmc_send_cmd(mmc, &cmd, &data)) {
		MMCINFO("mmc downlaod fw failed\n");
		return 0;
	}

	/* SPI multiblock writes terminate using a special
	 * token, not a STOP_TRANSMISSION request.
	 */
	if (!mmc_host_is_spi(mmc) && blkcnt > 1) {
		cmd.cmdidx    = MMC_CMD_STOP_TRANSMISSION;
		cmd.cmdarg    = 0;
		cmd.resp_type = MMC_RSP_R1b;
		if (mmc_send_cmd(mmc, &cmd, NULL)) {
			MMCINFO("mmc fail to send stop cmd\n");
			return 0;
		}
	}

	return blkcnt;
}

int mmc_download_fw(struct mmc *mmc, unsigned int ffu_arg)
{
	u32 package_size, blkcnt;
	unsigned char *buf	 = NULL;
	unsigned char *ffu_package = NULL;
	int err			   = 0;

	err = mmc_ffu_get_fw(&ffu_package, &package_size);
	if (err) {
		MMCINFO("%s[%d]: mmc_ffu_get_fw fail\n", __FUNCTION__,
			__LINE__);
		return -1;
	}

	MMCINFO("get fw point = %p;fw len: %d sector\n", ffu_package,
		package_size);

	if (package_size % 512) {
		MMCINFO("%s[%d]:Firmware size is not 512Bytes aligned, sizeof(ffu_package) = %d\n",
			__FUNCTION__, __LINE__, package_size);
		return -1;
	}

	buf = (unsigned char *)memalign(512, package_size + 512);
	if (buf == NULL) {
		MMCINFO("get mem for buf failed !\n");
		return -1;
	}

	MMCINFO("Firmware bin size  = %d\n", package_size);

	memcpy(buf, ffu_package, package_size); /* buf align handle */
	blkcnt = package_size / 512;
	MMCINFO("ffu_arg = 0x%08x, blkcnt = %d, buf = %p\n", ffu_arg, blkcnt,
		buf);

	//dumphex32("buf", (char *)(buf), 2048);

	mmc_ffu_download_firmware(mmc, ffu_arg, blkcnt, buf);
	if (buf) {
		free(buf);
		buf = NULL;
	}

	return 0;
}

int mmc_enter_normal_mode(struct mmc *mmc)
{
	MMCINFO("%s: make FW updates enabled\n", __FUNCTION__);
	if (mmc_switch(mmc, 0, EXT_CSD_MODE_CONFIG,
		       0)) { //EXT_CSD_CMD_SET_NORMAL = 0
		MMCINFO("%s: make FW updates enabled fail\n", __FUNCTION__);
		return -1;
	}

	return 0;
}

int mmc_enable_reset_func(struct mmc *mmc)
{
	ALLOC_CACHE_ALIGN_BUFFER(char, ext_csd, MMC_MAX_BLOCK_LEN);
	int err = 0;

	err = mmc_send_ext_csd(mmc, (u8 *)ext_csd);
	if (ext_csd[EXT_CSD_RST_N_FUNCTION] & 0x1) {
		MMCINFO("%s[%d], reset_func_already\n", __FUNCTION__, __LINE__);
		return 0;
	}

	MMCINFO("%s[%d], enable_reset\n", __FUNCTION__, __LINE__);
	err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL, EXT_CSD_RST_N_FUNCTION,
			 1);
	if (err) {
		MMCINFO("mmc enable hw rst fail\n");
		return err;
	}

	err = mmc_send_ext_csd(mmc, (u8 *)ext_csd);
	if (err) {
		MMCINFO("mmc get extcsd fail -1\n");
		return err;
	}

	if (!(ext_csd[EXT_CSD_RST_N_FUNCTION] & 0x1)) {
		MMCINFO("en_hw_rst_fail, 0x%x\n",
			ext_csd[EXT_CSD_RST_N_FUNCTION]);
		return -1;
	} else {
		MMCINFO("en_hw_rst_ok, 0x%x\n",
			ext_csd[EXT_CSD_RST_N_FUNCTION]);
	}

	return 0;
}

int mmc_judge_updata_success(struct mmc *mmc)
{
	int ret = 0;
	ALLOC_CACHE_ALIGN_BUFFER(char, ecsd, MMC_MAX_BLOCK_LEN);

	mmc_send_ext_csd(find_mmc_device(2), (u8 *)ecsd);

	if ((ecsd[261] == ((mmc->cfg->ffu_dest_fw_version >> 56) & 0xFF)) &&
	    (ecsd[260] == ((mmc->cfg->ffu_dest_fw_version >> 48) & 0xFF)) &&
	    (ecsd[259] == ((mmc->cfg->ffu_dest_fw_version >> 40) & 0xFF)) &&
	    (ecsd[258] == ((mmc->cfg->ffu_dest_fw_version >> 32) & 0xFF)) &&
	    (ecsd[257] == ((mmc->cfg->ffu_dest_fw_version >> 24) & 0xFF)) &&
	    (ecsd[256] == ((mmc->cfg->ffu_dest_fw_version >> 16) & 0xFF)) &&
	    (ecsd[255] == ((mmc->cfg->ffu_dest_fw_version >> 8) & 0xFF)) &&
	    (ecsd[254] == ((mmc->cfg->ffu_dest_fw_version >> 0) & 0xFF))) {
		MMCINFO("current_emmc_version = 0x%08x, FW_updata_success\n",
			mmc->cfg->ffu_dest_fw_version);
		ret = 0;
	} else {
		MMCINFO("updata_fw_fail, current_verison = 0x%02x%02x%02x%02x, desc_version = 0x%08x\n",
			ecsd[257], ecsd[256], ecsd[255], ecsd[254],
			mmc->cfg->ffu_dest_fw_version);
		ret = -1;
	}

	if (ecsd[EXT_CSD_FFU_STATUS]) {
		MMCINFO("updata_fw_fail, ffu_status = 0x%02x\n",
			ecsd[EXT_CSD_FFU_STATUS]);
		ret = -1;
	}

	return ret;
}

int sunxi_mmc_ffu(struct mmc *mmc)
{
	unsigned char old_ver[8];
	unsigned int ffu_arg;
	int err = 0;

	MMCINFO("*************sunxi_mmc_ffu*************\n");

	err = mmc_get_firmware_version(mmc, old_ver);
	if (err) {
		goto ERR_RET;
	}

	if ((old_ver[7] == ((mmc->cfg->ffu_src_fw_version >> 56) & 0xFF)) &&
	    (old_ver[6] == ((mmc->cfg->ffu_src_fw_version >> 48) & 0xFF)) &&
	    (old_ver[5] == ((mmc->cfg->ffu_src_fw_version >> 40) & 0xFF)) &&
	    (old_ver[4] == ((mmc->cfg->ffu_src_fw_version >> 32) & 0xFF)) &&
	    (old_ver[3] == ((mmc->cfg->ffu_src_fw_version >> 24) & 0xFF)) &&
	    (old_ver[2] == ((mmc->cfg->ffu_src_fw_version >> 16) & 0xFF)) &&
	    (old_ver[1] == ((mmc->cfg->ffu_src_fw_version >> 8) & 0xFF)) &&
	    (old_ver[0] == ((mmc->cfg->ffu_src_fw_version >> 0) & 0xFF))) {
		MMCINFO("current version is need updata version\n");
	} else {
		MMCINFO("error, current version no need updata version\n");
		return -1;
	}

	/* first reset func */
	/*
	err =mmc_enable_reset_func(mmc);
	if (err) {
		goto ERR_RET;
	}
	*/
	MMCINFO("%s: we don't set EXT_CSD_RST_N_FUNCTION here\n", __FUNCTION__);

	err = mmc_enter_ffu_mode(mmc, &ffu_arg);
	if (err) {
		goto ERR_RET;
	}

	/*********/
	MMCINFO("FFU_ARG = 0x%08X\n");
	MMCINFO("Download Firmware\n");

	err = mmc_download_fw(mmc, ffu_arg);
	if (err) {
		MMCINFO("%s: Download Firmware fail\n", __FUNCTION__);
		goto ERR_RET;
	}

	/*****/
	MMCINFO("%s: change device mode to normal\n", __FUNCTION__);
	err = mmc_enter_normal_mode(mmc);
	if (err) {
		goto ERR_RET;
	}

	return 0;
ERR_RET:
	MMCINFO("%s[%d]: ffu_is_fail\n", __FUNCTION__, __LINE__);
	return err;
}

#endif //SUPPORT_SUNXI_MMC_FFU
