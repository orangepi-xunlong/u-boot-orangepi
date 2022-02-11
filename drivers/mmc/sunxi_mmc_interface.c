// SPDX-License-Identifier: GPL-2.0+
/*
 * MMC driver for allwinner sunxi platform.
 *
 */
#include <config.h>
#include <common.h>
#include <command.h>
#include <errno.h>
#include <mmc.h>
#include <part.h>
#include <malloc.h>
#include <linux/list.h>
#include <div64.h>
#include "mmc_private.h"
#include "mmc_def.h"

int mmc_switch(struct mmc *mmc, u8 set, u8 index, u8 value);

#if 0
static int mmc_wp_grp_aligned(struct mmc *mmc, unsigned int from, unsigned int nr)
{
	if (!mmc->wp_grp_size)
		return 0;
	if (from % mmc->wp_grp_size || nr % mmc->wp_grp_size)
		return 0;
	return 1;
}

static void mmc_align_wp_grp(struct mmc *mmc, unsigned int from,
	unsigned int nr, unsigned int *align_from, unsigned int *align_nr)
{
	unsigned int rem, start, cnt;

	MMCDBG("--1-- from: %d, nr: %d, wp_grp: %d\n", from, nr, mmc->wp_grp_size);
	start = from;
	cnt = nr;

	rem = start % mmc->wp_grp_size;
	if (rem) {
		rem = mmc->wp_grp_size - rem;
		start += rem;
		if (cnt > rem)
			cnt -= rem;
		else {
			MMCINFO("after adjust start addr, no more space need to wp!!\n");
			goto RET;
		}
	}
	rem = cnt % mmc->wp_grp_size;
	if (rem)
		cnt -= rem;

	if (cnt == 0)
		MMCINFO("after adjust nr, no more space need to wp!!\n");


RET:
	MMCINFO("after adjust(0x%x): from %d, nr %d\n", mmc->wp_grp_size, start, cnt);
	*align_from = start;
	*align_nr = cnt;
	return ;
}

static int mmc_set_wp(struct mmc *mmc, u32 wp_grp_sect_addr)
{
	struct mmc_cmd cmd;
	int err = 0;

	memset(&cmd, 0, sizeof(cmd));

	cmd.cmdidx = MMC_CMD_SET_WRITE_PROT;
	cmd.resp_type = MMC_RSP_R1b;
	cmd.cmdarg = mmc->high_capacity ? wp_grp_sect_addr : wp_grp_sect_addr*512;

	err = mmc_send_cmd(mmc, &cmd, NULL);
	if (err)
	MMCINFO("mmc set wp failed\n");

	return 0;
}

static int mmc_clr_wp(struct mmc *mmc, u32 wp_grp_sect_addr)
{
	struct mmc_cmd cmd;
	int err = 0;

	memset(&cmd, 0, sizeof(cmd));

	cmd.cmdidx = MMC_CMD_CLR_WRITE_PROT;
	cmd.resp_type = MMC_RSP_R1b;
	cmd.cmdarg = mmc->high_capacity ? wp_grp_sect_addr : wp_grp_sect_addr*512;

	err = mmc_send_cmd(mmc, &cmd, NULL);
	if (err)
		MMCINFO("mmc clr wp failed\n");

	return 0;
}


static int mmc_clear_wp(struct mmc *mmc, u32 wp_grp_sect_addr)
{
	struct mmc_cmd cmd;
	int err = 0;

	memset(&cmd, 0, sizeof(cmd));

	cmd.cmdidx = MMC_CMD_CLR_WRITE_PROT;
	cmd.resp_type = MMC_RSP_R1b;
	cmd.cmdarg = mmc->high_capacity ? wp_grp_sect_addr : wp_grp_sect_addr*512;

	err = mmc_send_cmd(mmc, &cmd, NULL);
	if (err)
		MMCINFO("mmc clear wp failed\n");

	return 0;
}

static int mmc_send_wp(struct mmc *mmc, u32 wp_grp_sect_addr, u8 *wp_sta)
{
	struct mmc_cmd cmd;
	struct mmc_data data;
	int err = 0;

	memset(&cmd, 0, sizeof(cmd));
	memset(&data, 0, sizeof(data));

	cmd.cmdidx = MMC_CMD_SEND_WRITE_PROT;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = mmc->high_capacity ? wp_grp_sect_addr : wp_grp_sect_addr*512;

	data.dest = (char *)wp_sta;
	data.blocks = 1;
	data.blocksize = 4;  /*32bit*/
	data.flags = MMC_DATA_READ;

	err = mmc_send_cmd(mmc, &cmd, &data);
	if (err)
		MMCINFO("mmc send wp failed\n");

	return 0;
}

static int mmc_send_wp_type(struct mmc *mmc, u32 wp_grp_sect_addr, u8 *wp_type)
{
	struct mmc_cmd cmd;
	struct mmc_data data;
	int err = 0;

	memset(&cmd, 0, sizeof(cmd));
	memset(&data, 0, sizeof(data));

	cmd.cmdidx = MMC_CMD_SEND_WRITE_PROT_TYPE;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = mmc->high_capacity ? wp_grp_sect_addr : wp_grp_sect_addr*512;

	data.dest = (char *)wp_type;
	data.blocks = 1;
	data.blocksize = 8; /*64bit*/
	data.flags = MMC_DATA_READ;

	err = mmc_send_cmd(mmc, &cmd, &data);
	if (err)
		MMCINFO("mmc send wp type failed\n");

	return err;
}

s32 mmc_switch_user_wp(struct mmc *mmc, u32 val)
{
	return mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL, EXT_CSD_USER_WP, val);
}

static int mmc_set_one_poweron_wp(struct mmc *mmc, u32 wp_grp_sect_addr)
{
	s32 err = 0, ret = 0;
	u32 rval = 0;
	u8 ext_csd[512];

	if (mmc->csd_perm_wp) {
		MMCINFO("the entire device are write protected by csd[13]\n");
		return 0;
	}

	ret = mmc_send_ext_csd(mmc, ext_csd);
	if (ret) {
		MMCINFO("get ext_csd fail -- %d\n", __LINE__);
		err = -1;
		goto ERR_OUT;
	}

	rval = ext_csd[EXT_CSD_USER_WP];
	/*this bit is R/W, one time programable and readable, it will PERMANENTLY disable the use of
	 * permanent write protection for write protection groups within all the partitions in the user
	 * area from the point this bit is set forward.
	 */
	/*rval |= (MMC_SWITCH_USER_WP_US_PERM_WP_DIS);*/
	rval &= (~MMC_SWITCH_USER_WP_US_PERM_WP_EN);
	rval &= (~MMC_SWITCH_USER_WP_US_PWR_WP_DIS);
	rval |= (MMC_SWITCH_USER_WP_US_PWR_WP_EN);

	ret = mmc_switch_user_wp(mmc, rval);
	if (ret) {
		MMCINFO("mmc_swtich_user_wp fail -- %d\n", __LINE__);
		err = -1;
		goto ERR_OUT;
	}

	ret = mmc_send_ext_csd(mmc, ext_csd);
	if (ret) {
		MMCINFO("get ext_csd fail -- %d\n", __LINE__);
		err = -1;
		goto ERR_OUT;
	}

	if (/*(card->extcsd.user_wp & MMC_SWITCH_USER_WP_US_PERM_WP_DIS) || */
			(ext_csd[EXT_CSD_USER_WP] & MMC_SWITCH_USER_WP_US_PERM_WP_EN)
			|| (ext_csd[EXT_CSD_USER_WP] & MMC_SWITCH_USER_WP_US_PWR_WP_DIS)
			|| !(ext_csd[EXT_CSD_USER_WP] & MMC_SWITCH_USER_WP_US_PWR_WP_EN)) {
		MMCINFO("set US_PWR_WP_EN fail 0x%x -- %d\n", ext_csd[EXT_CSD_USER_WP], __LINE__);
		err = -1;
		goto ERR_OUT;
	}

	ret = mmc_set_wp(mmc, wp_grp_sect_addr);
	if (ret) {
		MMCINFO("mmc_set_wp error -- %d\n", __LINE__);
		err = -1;
		goto ERR_OUT;
	}

	mdelay(100);

ERR_OUT:
	return err;
}

static int mmc_set_one_permanent_wp(struct mmc *mmc, u32 wp_grp_sect_addr)
{
	s32 err = 0, ret = 0;
	u32 rval = 0;
	u8 ext_csd[512];

	if (mmc->csd_perm_wp) {
		MMCINFO("the entire device are write protected by csd[13]\n");
		return 0;
	}

	ret = mmc_send_ext_csd(mmc, ext_csd);
	if (ret) {
		MMCINFO("get ext_csd fail -- %d\n", __LINE__);
		err = -1;
		goto ERR_OUT;
	}

	rval = ext_csd[EXT_CSD_USER_WP];
	rval &= (~MMC_SWITCH_USER_WP_US_PERM_WP_DIS);
	rval |= (MMC_SWITCH_USER_WP_US_PERM_WP_EN);
	rval |= (MMC_SWITCH_USER_WP_US_PWR_WP_DIS);
	rval &= (~MMC_SWITCH_USER_WP_US_PWR_WP_EN);

	ret = mmc_switch_user_wp(mmc, rval);
	if (ret) {
		MMCINFO("mmc_swtich_user_wp fail -- %d\n", __LINE__);
		err = -1;
		goto ERR_OUT;
	}

	ret = mmc_send_ext_csd(mmc, ext_csd);
	if (ret) {
		MMCINFO("get ext_csd fail -- %d\n", __LINE__);
		err = -1;
		goto ERR_OUT;
	}

	if ((ext_csd[EXT_CSD_USER_WP] & MMC_SWITCH_USER_WP_US_PERM_WP_DIS)
			|| !(ext_csd[EXT_CSD_USER_WP] & MMC_SWITCH_USER_WP_US_PERM_WP_EN)
			|| !(ext_csd[EXT_CSD_USER_WP] & MMC_SWITCH_USER_WP_US_PWR_WP_DIS)
			|| (ext_csd[EXT_CSD_USER_WP] & MMC_SWITCH_USER_WP_US_PWR_WP_EN)) {
		MMCINFO("set US_PERM_WP_EN fail 0x%x -- %d\n", ext_csd[EXT_CSD_USER_WP], __LINE__);
		err = -1;
		goto ERR_OUT;
	}

	ret = mmc_set_wp(mmc, wp_grp_sect_addr);
	if (ret) {
		MMCINFO("mmc_set_wp error -- %d\n", __LINE__);
		err = -1;
		goto ERR_OUT;
	}

	mdelay(100);

ERR_OUT:
	return err;
}

static int mmc_set_one_temporary_wp(struct mmc *mmc, u32 wp_grp_sect_addr)
{
	s32 err = 0, ret = 0;
	u32 rval = 0;
	u8 ext_csd[512];

	if (mmc->csd_perm_wp) {
		MMCINFO("the entire device are write protected by csd[13]\n");
		return 0;
	}

	ret = mmc_send_ext_csd(mmc, ext_csd);
	if (ret) {
		MMCINFO("get ext_csd fail -- %d\n", __LINE__);
		err = -1;
		goto ERR_OUT;
	}

	rval = ext_csd[EXT_CSD_USER_WP];
	rval &= (~MMC_SWITCH_USER_WP_US_PERM_WP_DIS);
	rval &= (~MMC_SWITCH_USER_WP_US_PWR_WP_DIS);
	rval &= (~MMC_SWITCH_USER_WP_US_PERM_WP_EN);
	rval &= (~MMC_SWITCH_USER_WP_US_PWR_WP_EN);


	ret = mmc_switch_user_wp(mmc, rval);
	if (ret) {
		MMCINFO("mmc_swtich_user_wp fail -- %d\n", __LINE__);
		err = -1;
		goto ERR_OUT;
	}

	ret = mmc_send_ext_csd(mmc, ext_csd);
	if (ret) {
		MMCINFO("get ext_csd fail -- %d\n", __LINE__);
		err = -1;
		goto ERR_OUT;
	}

	if ((ext_csd[EXT_CSD_USER_WP] & MMC_SWITCH_USER_WP_US_PERM_WP_DIS)
			|| (ext_csd[EXT_CSD_USER_WP] & MMC_SWITCH_USER_WP_US_PWR_WP_DIS)
			|| (ext_csd[EXT_CSD_USER_WP] & MMC_SWITCH_USER_WP_US_PERM_WP_EN)
			|| (ext_csd[EXT_CSD_USER_WP] & MMC_SWITCH_USER_WP_US_PWR_WP_EN)) {
		MMCINFO("set US_TEM_WP_EN fail 0x%x -- %d\n", ext_csd[EXT_CSD_USER_WP], __LINE__);
		err = -1;
		goto ERR_OUT;
	}

	ret = mmc_set_wp(mmc, wp_grp_sect_addr);
	if (ret) {
		MMCINFO("mmc_set_wp error -- %d\n", __LINE__);
		err = -1;
		goto ERR_OUT;
	}

	mdelay(100);
	MMCINFO("******set US_TEM_WP_EN ok\n");
ERR_OUT:
	return err;
}

static int mmc_clr_one_tem_wp(struct mmc *mmc, u32 wp_grp_sect_addr)
{
	s32 err = 0, ret = 0;

	if (mmc->csd_perm_wp) {
		MMCINFO("the entire device are write protected by csd[13]\n");
		return 0;
	}

	ret = mmc_clr_wp(mmc, wp_grp_sect_addr);
	if (ret) {
		MMCINFO("mmc_clr_wp error -- %d\n", __LINE__);
		err = -1;
	}

	mdelay(100);

	return err;
}



static int mmc_get_current_wp_info(struct mmc *mmc, u32 wp_grp_addr,
		u8 *wp_sta, u8 *wp_type)
{
	s32 err = 0, ret = 0;
	u8 tmp_wp_sta[4] = {0};
	u8 tmp_wp_type[8] = {0};

	ret = mmc_send_wp(mmc, wp_grp_addr, &tmp_wp_sta[0]);
	if (ret) {
		MMCINFO("mmc_send_wp error -- %d\n", __LINE__);
		err = -1;
		goto ERR_OUT;
	}

	ret = mmc_send_wp_type(mmc, wp_grp_addr, &tmp_wp_type[0]);
	if (ret) {
		MMCINFO("mmc_send_wp_type error -- %d\n", __LINE__);
		err = -1;
		goto ERR_OUT;
	}

{
	MMCINFO("mmc_get_current_wp_info: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x \n",
		tmp_wp_type[0], tmp_wp_type[1], tmp_wp_type[2], tmp_wp_type[3], tmp_wp_type[4], tmp_wp_type[5], tmp_wp_type[6], tmp_wp_type[7]);
}

	/*e.g. 0x0, 0x02000000, big-endian ???*/
	switch (tmp_wp_type[7] & 0x3) {
	case 0:
		MMCINFO("not protected - %d\n", (tmp_wp_type[7]&0x3));
		break;
	case 1:
		MMCINFO("temporary wp - %d\n", (tmp_wp_type[7]&0x3));
		break;
	case 2:
		MMCINFO("power-no wp - %d\n", (tmp_wp_type[7]&0x3));
		break;
	case 3:
		MMCINFO("permanent wp - %d\n", (tmp_wp_type[7]&0x3));
		break;
	default:
		MMCINFO("unknown wp type!!!!!\n");
		break;
	}

	*wp_sta = tmp_wp_sta[3] & 0x1;
	*wp_type = tmp_wp_type[7] & 0x3;

ERR_OUT:
	return err;
}

#endif
/*
* --argument
* @mmc:
* @wp_type: write protection type
*     0-power-no write protection
*     1-permanent write protection
	2-temporary write protection
* @align_type:
*     0-only support write protect group size aligned @from and @nr.
*     1-support any @from and @nr, but only execute write protect aligned write protect group size.
* @from: start sector
* @nr: the number of sectors
*
* --return
*    0-ok, execute write protect operation successfully.
*    others-fail.
*/
#if 0
int mmc_user_write_protect(struct mmc *mmc, u32 wp_type, u32 align_type,
	u32 from, u32 nr)
{
	s32 ret = 0;
	u32 align_from, align_nr;
	u32 iwp, wp_start, wp_cnt, wp_addr;
	u8 wp_sta = 0;
	u8 wp_tp = 0;

#if 0
	if (wp_type) {
		MMCINFO("!!!!!!!!!!!!!!  wp_type %d, return !!!!!!!!!!!!!!!\n", wp_type);
		return 0;
	}

	if (!(mmc->drv_wp_feature & DRV_PARA_ENABLE_EMMC_USER_PART_WP)) {
		MMCINFO("don't support write protect operation!!\n");
		return -1;
	}

	if (IS_SD(mmc)) {
		MMCINFO("SD Card don't support write protect operation!!!\n");
		return -1;
	}
#endif
	if (mmc_wp_grp_aligned(mmc, from, nr) == 0) {
		MMCINFO("@from and(or) @nr are not wp grp size(0x%x)aligned.\n", mmc->wp_grp_size);
		if (align_type == 0) {
			MMCINFO("input @from and(or) @nr error!");
			return -1;
		}
		mmc_align_wp_grp(mmc, from, nr, &align_from, &align_nr);
	} else {
		align_from = from;
		align_nr = nr;
	}
	wp_start = align_from;
	wp_cnt = align_nr / mmc->wp_grp_size;

	for (iwp = 0; iwp < wp_cnt; iwp++) {
		wp_addr = wp_start + iwp*mmc->wp_grp_size;

		/* if wp grp has already been protected, jump to next wp grp */
		ret = mmc_get_current_wp_info(mmc, wp_addr, &wp_sta, &wp_tp);
		if (ret) {
			MMCINFO("mmc get wp info fail, before execute write protect!\n");
			ret = -1;
			goto ERR_OUT;
		}

		if (wp_type) {
			if (wp_tp == 0x3) {
				MMCINFO("already permanent wp\n");
				continue;
			} else if (wp_tp == 0x1) {
				MMCINFO("already temporary wp\n");
				continue;
			}
		} else {
			if (wp_tp == 0x2) {
				MMCINFO("already power-on wp\n");
				continue;
			}
		}

		/* execute write protect operation */
		if (wp_type) {
			if (wp_type == 1)
				ret = mmc_set_one_permanent_wp(mmc, wp_addr);
			else
				ret = mmc_set_one_temporary_wp(mmc, wp_addr);
		} else
			ret = mmc_set_one_poweron_wp(mmc, wp_addr);
		if (ret) {
			MMCINFO("mmc set wp error\n");
			ret = -1;
			goto ERR_OUT;
		}

		/* check write protect status */
		ret = mmc_get_current_wp_info(mmc, wp_addr, &wp_sta, &wp_tp);
		if (ret) {
			MMCINFO("mmc get wp info error!\n");
			ret = -1;
			goto ERR_OUT;
		}

		if (wp_type) {
			if (wp_type == 1) {
				if (wp_tp != 0x3) {
					MMCINFO("mmc set permanent wp fail\n");
					ret = -1;
					goto ERR_OUT;
				}
			} else {
				if (wp_type == 2) {
					if (wp_tp != 1) {
						MMCINFO("mmc set temporary wp fail\n");
						ret = -1;
						goto ERR_OUT;
					}
				}
			}
		} else {
			if (wp_tp != 0x2) {
				MMCINFO("mmc set power-on wp fail\n");
				ret = -1;
				goto ERR_OUT;
			}
		}
	}

ERR_OUT:
	return ret;
}
#endif
/*
* --argument
* @mmc:
* @align_type:
*     0-only support write protect group size aligned @from and @nr.
*     1-support any @from and @nr, but only execute write protect aligned write protect group size.
* @from: start sector
* @nr: the number of sectors
*
* --return
*    0-ok, execute clear temporary write protection successfully.
*    others-fail.
*/
#if 0
int mmc_user_clr_tem_write_protect(struct mmc *mmc, u32 align_type,
	u32 from, u32 nr)
{
	s32 ret = 0;
	u32 align_from, align_nr;
	u32 iwp, wp_start, wp_cnt, wp_addr;
	u8 wp_sta = 0;
	u8 wp_tp = 0;

#if 0
	if (wp_type) {
		MMCINFO("!!!!!!!!!!!!!!  wp_type %d, return !!!!!!!!!!!!!!!\n", wp_type);
		return 0;
	}

	if (!(mmc->drv_wp_feature & DRV_PARA_ENABLE_EMMC_USER_PART_WP)) {
		MMCINFO("don't support write protect operation!!\n");
		return -1;
	}

	if (IS_SD(mmc)) {
		MMCINFO("SD Card don't support write protect operation!!!\n");
		return -1;
	}
#endif
	MMCINFO("!!!!!!!!!!!!!!  clr tem !!!!!!!!!!!!!!!\n");
	if (mmc_wp_grp_aligned(mmc, from, nr) == 0) {
		MMCINFO("@from and(or) @nr are not wp grp size(0x%x)aligned.\n", mmc->wp_grp_size);
		if (align_type == 0) {
			MMCINFO("input @from and(or) @nr error!");
			return -1;
		}
		mmc_align_wp_grp(mmc, from, nr, &align_from, &align_nr);
	} else {
		align_from = from;
		align_nr = nr;
	}
	wp_start = align_from;
	wp_cnt = align_nr / mmc->wp_grp_size;

	for (iwp = 0; iwp < wp_cnt; iwp++) {
		wp_addr = wp_start + iwp*mmc->wp_grp_size;

		/* if wp grp has already been protected, jump to next wp grp */
		ret = mmc_get_current_wp_info(mmc, wp_addr, &wp_sta, &wp_tp);
		if (ret) {
			MMCINFO("mmc get wp info fail, before execute write protect!\n");
			ret = -1;
			goto ERR_OUT;
		}

		if (wp_tp != 0x1) {
			MMCINFO("no temporary wp\n");
			continue;
		}

		/* execute write protect operation */
		ret = mmc_clr_one_tem_wp(mmc, wp_addr);
		if (ret) {
			MMCINFO("mmc clear temporary error\n");
			ret = -1;
			goto ERR_OUT;
		}

		/* check write protect status */
		ret = mmc_get_current_wp_info(mmc, wp_addr, &wp_sta, &wp_tp);
		if (ret) {
			MMCINFO("mmc get wp info error!\n");
			ret = -1;
			goto ERR_OUT;
		}

		if (wp_tp != 0x0) {
			MMCINFO("clear  temporary wp failed\n");
			ret = -1;
			goto ERR_OUT;
		}
	}

ERR_OUT:
	return ret;
}
#endif
/*
* --argument
* @dev_num: card number
* @from: start sector
* @nr: the number of sectors
*
* --return
*    0-ok, execute clear temporary write protection successfully.
*    others-fail.
*    only support write protect group size aligned @from and @nr.
*/
#if 0
int mmc_clr_tem_wp(int dev_num, unsigned start, unsigned blkcnt)
{
	struct mmc *mmc = find_mmc_device(dev_num);
	int ret = 0;

	MMCDBG("start %s ...\n", __FUNCTION__);
	if (IS_SD(mmc)) {
		MMCINFO("%s: sd card don't support write protect!\n", __FUNCTION__);
		ret = -1;
		goto ERR_RET;
	}

	if (blkcnt == 0) {
		MMCINFO("%s: no space need to erase, from:%d nr:%d\n",
			__FUNCTION__, start, blkcnt);
		ret = 0;
		goto ERR_RET;
	}

	if ((start+blkcnt) > mmc->block_dev.lba) {
		MMCINFO("%s: input lenght error!!!\n", __FUNCTION__);
		blkcnt = mmc->block_dev.lba - start;
		MMCINFO("%s: after clip, from: %d, nr: %d\n",
			__FUNCTION__, start, blkcnt);
	}

	if (!(mmc->cfg->drv_wp_feature & DRV_PARA_ENABLE_EMMC_USER_PART_WP)) {
		MMCINFO("don't support write protect operation!!\n");
		ret = -1;
		goto ERR_RET;
	}

	ret = mmc_user_clr_tem_write_protect(mmc, 0, start, blkcnt);
	if (ret)
		MMCINFO("mmc_user_write_protect error!\n");

ERR_RET:
	MMCDBG("end %s %d\n", __FUNCTION__, ret);
	return ret;
}


int mmc_user_scan_wp_sta(struct mmc *mmc)
{
	s32 err = 0, ret = 0;
	u32 wp_addr = 0;
	u8 wp_type[8] = {0};
	u32 wp_hit_addr[10] = {0}; /* record the first 10 wp group info */
	u32 wp_hit_type[10] = {0};
	u32 wp_hit_cnt = 0;
	u32 wp_grp_cnt = mmc->block_dev.lba / mmc->wp_grp_size;
	u32 sta, val = 0;
	int i, j, k;

	if (!(mmc->cfg->drv_wp_feature & DRV_PARA_ENABLE_EMMC_USER_PART_WP)) {
		MMCINFO("don't support write protect operation!!\n");
		return -1;
	}

	if (IS_SD(mmc)) {
		MMCINFO("SD Card don't support write protect operation!!!\n");
		return -1;
	}

	MMCINFO("total wp cnt: %d(0x%x/0x%x)\n", wp_grp_cnt, mmc->block_dev.lba, mmc->wp_grp_size);

	/*mmc_user_write_protect(mmc,2,0,32768,32768);*/
	/*mmc_user_clr_tem_write_protect(mmc,0,32768,32768);*/
	/*mmc_clr_tem_wp(2, 32768,32768);*/

	for (i = 0; i < wp_grp_cnt; i += 32) /*#define WP_GRP_CNT_EACH_CMD 32*/ {
		wp_addr = i * mmc->wp_grp_size;

		memset(wp_type, 0, 8);
		ret = mmc_send_wp_type(mmc, wp_addr, &wp_type[0]);
		if (ret) {
			MMCINFO("mmc_send_wp_type error -- %d\n", __LINE__);
			err = -1;
			goto ERR_OUT;
		}

{
	/*
	MMCINFO("mmc_user_scan_wp_sta: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x \n",
	wp_type[0], wp_type[1], wp_type[2], wp_type[3], wp_type[4], wp_type[5], wp_type[6], wp_type[7]);
	*/
}

		/* e.g. 0 x0, 0x02000000, big-endian ??? */
		for (j = 0; j < 8; j++) {
			val = wp_type[7-j];
			/*MMCINFO("j%d, val 0x%x\n", j, val);*/
			for (k = 0; k < 4 ; k++) {
				sta = (val>>(2*k)) & 0x3;
				if (sta) {
					wp_addr = mmc->wp_grp_size * (i + j*4 + k);
					/*MMCINFO("%d %d %d, 0x%x, sta %d\n", i, j, k, wp_addr, sta);
					*/
					if (wp_hit_cnt < 10) {
						wp_hit_addr[wp_hit_cnt] = wp_addr;
						wp_hit_type[wp_hit_cnt] = sta;
					}
					wp_hit_cnt++;
				}
			}
		}
	}

	if (wp_hit_cnt) {
		MMCINFO("%d wp are write protected, some wp are as follow: \n", wp_hit_cnt);
		if (wp_hit_cnt > 10)
			k = 10;
		else
			k = wp_hit_cnt;
		for (i = 0; i < k; i++) {
			switch (wp_hit_type[i]) {
			case 0:
				MMCINFO("wp grp %d-0x%x: not protected\n",
				wp_hit_addr[i]/mmc->wp_grp_size, wp_hit_addr[i]);
				break;
			case 1:
				MMCINFO("wp grp %d-0x%x: temporary wp\n",
				wp_hit_addr[i]/mmc->wp_grp_size, wp_hit_addr[i]);
				break;
			case 2:
				MMCINFO("wp grp %d-0x%x: power-no wp\n",
				wp_hit_addr[i]/mmc->wp_grp_size, wp_hit_addr[i]);
				break;
			case 3:
				MMCINFO("wp grp %d-0x%x: permanent wp\n",
				wp_hit_addr[i]/mmc->wp_grp_size, wp_hit_addr[i]);
				break;
			default:
				MMCINFO("unknown wp type!!!!!\n");
				break;
			}
		}
	}

ERR_OUT:
	return err;
}
#endif

/*
* --argument
* @dev_num: card number
* @wp_type: write protection type
*     0-power-no write protection
*     1-permanent write protection
	2-temporary write protection
* @from: start sector
* @nr: the number of sectors
*
* --return
*    0-ok, execute write protect operation successfully.
*    others-fail.
*    only support write protect group size aligned @from and @nr.
*/

#if 0
int mmc_mmc_user_write_protect(int dev_num, unsigned wp_type, unsigned start, unsigned blkcnt)
{
	struct mmc *mmc = find_mmc_device(dev_num);
	int ret = 0;

	MMCDBG("start %s ...\n", __FUNCTION__);
	if (IS_SD(mmc)) {
		MMCINFO("%s: sd card don't support write protect!\n", __FUNCTION__);
		ret = -1;
		goto ERR_RET;
	}

	if (blkcnt == 0) {
		MMCINFO("%s: no space need to erase, from:%d nr:%d\n",
			__FUNCTION__, start, blkcnt);
		ret = 0;
		goto ERR_RET;
	}

	if ((start+blkcnt) > mmc->block_dev.lba) {
		MMCINFO("%s: input lenght error!!!\n", __FUNCTION__);
		blkcnt = mmc->block_dev.lba - start;
		MMCINFO("%s: after clip, from: %d, nr: %d\n",
			__FUNCTION__, start, blkcnt);
	}

	if (!(mmc->cfg->drv_wp_feature & DRV_PARA_ENABLE_EMMC_USER_PART_WP)) {
		MMCINFO("don't support write protect operation!!\n");
		ret = -1;
		goto ERR_RET;
	}

	ret = mmc_user_write_protect(mmc, wp_type, 0, start, blkcnt);
	if (ret)
		MMCINFO("mmc_user_write_protect error!\n");



ERR_RET:
	MMCDBG("end %s %d\n", __FUNCTION__, ret);
	return ret;
}

/*
* --argument
* @dev_num: card number
* @wp_grp_size:
*	return the wp group size
* --return
*    0-ok,
*    others-fail.
*/
int mmc_mmc_user_get_wp_grp_size(int dev_num, unsigned int *wp_grp_size)
{
	struct mmc *mmc = find_mmc_device(dev_num);
	int ret = 0;

	MMCDBG("start %s ...\n", __FUNCTION__);
	if (IS_SD(mmc)) {
		MMCINFO("%s: sd card don't support write protect!\n", __FUNCTION__);
		ret = -1;
		goto ERR_RET;
	}

	*wp_grp_size = mmc->wp_grp_size;

ERR_RET:
	MMCDBG("end %s %d\n", __FUNCTION__, ret);
	return ret;
}



static ulong mmc_do_burn_boot(int dev_num, lbaint_t start, lbaint_t blkcnt, const void *src)
{
	s32 err = 0;
	ulong rblk, blocks_do = 0;
	void *dst = NULL;
	struct mmc *mmc = find_mmc_device(dev_num);

	dst = (void *)malloc(blkcnt * mmc->write_bl_len);
	if (dst == NULL) {
		MMCINFO("%s: request memory fail\n", __FUNCTION__);
		return 0;
	}

	/* write boot0 */
	blocks_do = mmc_bwrite(dev_num, start, blkcnt, src);
	if (blocks_do != blkcnt) {
		MMCINFO("%s: write boot0 fail\n", __FUNCTION__);
		err = -1;
		goto ERR_RET;
	}

	/* read and check */
	rblk = mmc_bread(dev_num, start, blkcnt, dst);
	if ((rblk < blkcnt) || memcmp(src, dst, blkcnt * mmc->write_bl_len)) {
		MMCINFO("%s: check boot0 fail\n", __FUNCTION__);
		err = -1;
	} else
		MMCDBG("%s: check boot0 ok\n", __FUNCTION__);

ERR_RET:
	free(dst);
	if (err)
		return 0;
	else
		return blocks_do;
}


static ulong mmc_burn_boot(int dev_num, lbaint_t start, lbaint_t blkcnt, const void *src)
{
	lbaint_t blocks_do_user = 0, blocks_do_boot = 0;
	ulong start_todo = start;
	signed int err = 0;
	struct mmc *mmc = find_mmc_device(dev_num);

	if ((mmc->cfg->platform_caps.drv_burn_boot_pos & DRV_PARA_BURN_EMMC_BOOT_PART)
		&& mmc->boot_support) {
		MMCDBG("%s: start burn emmc boot part: %d...\n", __func__, (int)start);
		/* enable boot mode */
		err = mmc_switch_boot_bus_cond(dev_num,
										MMC_SWITCH_BOOT_SDR_NORMAL,
										MMC_SWITCH_BOOT_RST_BUS_COND,
										MMC_SWITCH_BOOT_BUS_SDRx4_DDRx4);
		if (err) {
			MMCINFO("mmc switch boot bus condition failed\n");
			goto DISABLE_BOOT_MODE;
		}

		/* configure boot mode */
		err = mmc_switch_boot_part(dev_num,
									MMC_SWITCH_PART_BOOT_ACK_ENB,
									MMC_SWITCH_PART_BOOT_PART_1);
		if (err) {
			MMCINFO("mmc configure boot partition failed\n");
			goto DISABLE_BOOT_MODE;
		}

		err = mmc_switch_part(dev_num, MMC_SWITCH_PART_BOOT_PART_1);
		if (err) {
			MMCINFO("switch to boot1 partition failed\n");
			goto DISABLE_BOOT_MODE;
		}
		/*for smhc2, in eMMC boot partition,boot0.bin start from 0 sector,so we must change start form 16 to 0
		* for smhc3, in eMMC boot partition,boot0.bin start from 0 sector,so we must change start form 16 to 1
		*/
		if (dev_num == 3)
			start_todo = 1;
		else
			start_todo = 0;
		blocks_do_boot = mmc_do_burn_boot(dev_num, start_todo, blkcnt, src);
		if (blocks_do_boot != blkcnt)
			MMCINFO("burn boot to emmc boot1 partition failed" LBAF "\n", blocks_do_boot);

		/* switch back to user partiton */
		err = mmc_switch_part(dev_num, 0);
		if (err) {
			MMCINFO("switch back to user partition failed\n");
			return 0;
		}
	}

DISABLE_BOOT_MODE:
	if ((err || !(mmc->cfg->platform_caps.drv_burn_boot_pos & DRV_PARA_BURN_EMMC_BOOT_PART))
			&& mmc->boot_support) {
		/* disable boot mode */
		err = mmc_switch_boot_part(dev_num, 0, MMC_SWITCH_PART_BOOT_PART_NONE);
		if (err)
			MMCINFO("mmc disable boot mode failed\n");
	}

	/* burn boot to user partition */
	if (!(mmc->cfg->platform_caps.drv_burn_boot_pos & DRV_PARA_NOT_BURN_USER_PART)) {
		MMCDBG("%s: start burn emmc user part: %d...\n", __func__, (int)start);
		blocks_do_user = mmc_do_burn_boot(dev_num, start, blkcnt, src);
		if (blocks_do_user != blkcnt)
			MMCINFO("burn boot to user partition failed," LBAFU "\n", blocks_do_user);
	}

	if (blocks_do_user == blkcnt || blocks_do_boot == blkcnt)
		return blkcnt;
	else
		return 0;
}


/* use same memory, if get a good boot0, exit.*/
static ulong mmc_get_boot(int dev_num, lbaint_t start, lbaint_t blkcnt, void *dst)
{
	lbaint_t blocks_do = 0;
	ulong start_todo = start;
	struct mmc *mmc = find_mmc_device(dev_num);

	if (!(mmc->cfg->platform_caps.drv_burn_boot_pos & DRV_PARA_NOT_BURN_USER_PART)) {
		blocks_do = mmc_bread(dev_num, start_todo, blkcnt, dst);
		if (blocks_do != blkcnt) {
			MMCINFO("%s: get boot0 from user part err\n", __FUNCTION__);
		} else {
			MMCDBG("%s: get boot0 from user part ok, return..\n", __FUNCTION__);
			goto RET;
		}
	}

	if ((mmc->cfg->platform_caps.drv_burn_boot_pos & DRV_PARA_BURN_EMMC_BOOT_PART)
		&& mmc->boot_support) {
		if (mmc_switch_part(dev_num, MMC_SWITCH_PART_BOOT_PART_1)) {
			MMCINFO("%s: switch to boot1 part failed\n", __FUNCTION__);
			goto RET;
		}

		/*for smhc2, in eMMC boot partition,boot0.bin start from 0 sector,so we must change start form 16 to 0
		* for smhc3, in eMMC boot partition,boot0.bin start from 0 sector,so we must change start form 16 to 1
		*/
		if (dev_num == 3)
			start_todo = 1;
		else
			start_todo = 0;

		blocks_do = mmc_bread(dev_num, start_todo, blkcnt, dst);
		if (blocks_do != blkcnt) {
			MMCINFO("%s: get boot0 from boot part err\n", __FUNCTION__);
		} else {
			MMCDBG("%s: get boot0 from boot part ok\n", __FUNCTION__);
		}

		if (mmc_switch_part(dev_num, 0)) {
			MMCINFO("%s: switch back to user part failed,"
				"it maybe still in boot part!!!\n", __FUNCTION__);
			goto RET;
		}
	}

RET:
	return blocks_do;
}



static ulong mmc_bwrite_mass_pro(int dev_num, ulong start, lbaint_t blkcnt, const void *src)
{
	MMCDBG("%s: dev %d, start %ld, blkcnt %ld src 0x%x\n", __FUNCTION__, dev_num, start, blkcnt, (u32)src);

	if ((((dev_num == 2) && (start == BOOT0_SDMMC_START_ADDR)) || ((dev_num == 3) && (start == BOOT0_EMMC3_START_ADDR)))
		&& ((start+blkcnt) < CONFIG_MMC_LOGICAL_OFFSET)) {
		MMCDBG("%s: start burn boot data...\n", __FUNCTION__);
		return mmc_burn_boot(dev_num, start, blkcnt, src);
	} else /*if (start > CONFIG_MMC_LOGICAL_OFFSET)*/ {
		MMCDBG("%s: start burn user data...\n", __FUNCTION__);
		return mmc_bwrite(dev_num, start, blkcnt, src);
	}
	/*
	else
	{
		MMCINFO("%s: input parameter error!\n", __FUNCTION__);
	}
	*/

	return 0;
}

static ulong mmc_bread_mass_pro(int dev_num, ulong start, lbaint_t blkcnt, void *dst)
{
	MMCDBG("%s: dev %d, start %ld, blkcnt %ld src 0x%x\n", __FUNCTION__, dev_num, start, blkcnt, (u32)dst);

	if ((((dev_num == 2) && (start == BOOT0_SDMMC_START_ADDR)) || ((dev_num == 3) && (start == BOOT0_EMMC3_START_ADDR)))
		&& ((start+blkcnt) < CONFIG_MMC_LOGICAL_OFFSET)) {
		MMCDBG("%s: start read boot data...\n", __FUNCTION__);
		return mmc_get_boot(dev_num, start, blkcnt, dst);
	} else /*if (start > CONFIG_MMC_LOGICAL_OFFSET)*/ {
		MMCDBG("%s: start read user data...\n", __FUNCTION__);
		return mmc_bread(dev_num, start, blkcnt, dst);
	}
	/*
	else
	{
		MMCINFO("%s: input parameter error!\n", __FUNCTION__);
	}
	*/

	return 0;
}

/**
 * mmc_enable_bootop - enable boot mode.
 * @dev_num: card number
 * @part_nu: emmc boot partition number,should be 1 or 2
 * @enable: 1 enable boot operation,0 disable
 *
 * return 0 means ok,-1 means failed;
 */
int mmc_enable_bootop(int dev_num,
		int part_nu,
		int enable)
{
	struct mmc *mmc = find_mmc_device(dev_num);
	int err = 0;

	if (mmc->boot_support) {
		if ((part_nu != MMC_SWITCH_PART_BOOT_PART_1)
			&& part_nu != MMC_SWITCH_PART_BOOT_PART_2) {
			MMCINFO("%s,%d:wrong mmc boot partition num %d:\n",
				__FUNCTION__, __LINE__, part_nu);
			return -1;
		}

		if (enable) {
			/* enable boot mode */
			err = mmc_switch_boot_bus_cond(dev_num,
					MMC_SWITCH_BOOT_SDR_NORMAL,
					MMC_SWITCH_BOOT_RST_BUS_COND,
					MMC_SWITCH_BOOT_BUS_SDRx4_DDRx4);
			if (err) {
				MMCINFO("mmc switch boot bus condition failed\n");
				return -1;
			}

			/* configure boot mode */
			err = mmc_switch_boot_part(dev_num,
					MMC_SWITCH_PART_BOOT_ACK_ENB,
					part_nu);
			if (err) {
				MMCINFO("mmc set boot partition failed\n");
				return -1;
			}
			MMCINFO("mmc enable boot mode ok\n");
		} else {
			/* disable boot mode */
			err = mmc_switch_boot_part(dev_num,
				0,
				MMC_SWITCH_PART_BOOT_PART_NONE);
			if (err) {
				MMCINFO("mmc disable boot mode failed\n");
				return -1;
			}
			MMCINFO("mmc disable boot mode ok\n");
		}
		return 0;
	} else {
		MMCINFO("don't support boot partition\n");
		return -1;
	}
}

/**
 * mmc_write_bootp - write data to boot partition.
 * @dev_num: card number
 * @start:start sector
 * @blkcnt:count of sector data to transfer
 * @part_nu: emmc boot partition number,should be 1 or 2
 * @src: data buffer address to be writen to emmc
 *
 * return number of sector has been writen to emmc,if return 0, it mean failed;
 */
unsigned long	mmc_write_bootp(int dev_num,
			lbaint_t start,
			lbaint_t blkcnt,
			int part_nu,
			const void *src)
{
	lbaint_t blocks_do_boot = 0;
	ulong start_todo = start;
	signed int err = 0;
	struct mmc *mmc = find_mmc_device(dev_num);

	if (mmc->boot_support) {
		if ((part_nu != MMC_SWITCH_PART_BOOT_PART_1)
			&& part_nu != MMC_SWITCH_PART_BOOT_PART_2) {
			MMCINFO("%s,%d: wrong mmc boot partition num %d:\n",
			__FUNCTION__, __LINE__, part_nu);
			return 0;
		}

		if ((start + blkcnt) > (mmc->capacity_boot/512)) {
			MMCINFO("%s,%d: over mmc boot part size\n",
			__FUNCTION__, __LINE__);
			return 0;
		}

		MMCDBG("%s: start write emmc boot part: %d...\n",
			__FUNCTION__, (int)start);

		err = mmc_switch_part(dev_num, part_nu);
		if (err) {
			MMCINFO("switch to boot%d partition failed\n",
			part_nu);
			return 0;
		}

		blocks_do_boot = mmc_do_burn_boot(dev_num,
							start_todo,
							blkcnt,
							src);
		if (blocks_do_boot != blkcnt) {
		MMCINFO("burn boot to mmc boot%d partition failed"LBAF"\n",
		part_nu, blocks_do_boot);
		return 0;
		}

		/* switch back to user partiton */
		err = mmc_switch_part(dev_num, 0);
		if (err) {
			MMCINFO("switch back to user partition failed\n");
			return 0;
		}
		return blocks_do_boot;
	} else {
		MMCINFO("%s,%d: don't support boot partition\n",
		__FUNCTION__, __LINE__);
		return 0;
	}
}

/**
 * mmc_read_bootp - read  boot partition data
 * @dev_num: card number
 * @start:start sector
 * @blkcnt:count of sector data to transfer
 * @part_nu: emmc boot partition number,should be 1 or 2
 * @dst: data buffer address to be writen from emmc
 *
 * return number of sector has been read from emmc,if return 0, it mean failed;
 */
unsigned long	mmc_read_bootp(int dev_num,
			lbaint_t start,
			lbaint_t blkcnt,
			int part_nu,
			void *dst)
{
		lbaint_t blocks_do_boot = 0;
		ulong start_todo = start;
		struct mmc *mmc = find_mmc_device(dev_num);

	if (mmc->boot_support) {
		if ((part_nu != MMC_SWITCH_PART_BOOT_PART_1)
			&& part_nu != MMC_SWITCH_PART_BOOT_PART_2) {
			MMCINFO("%s,%d: wrong mmc boot partition num %d:\n",
			__FUNCTION__, __LINE__, part_nu);
			return 0;
		}

		if ((start + blkcnt) > (mmc->capacity_boot/512)) {
			MMCINFO("%s,%d: over emmc boot part size\n",
			__FUNCTION__, __LINE__);
			return 0;
		}

		if (mmc_switch_part(dev_num, part_nu)) {
			MMCINFO("%s: switch to boot%d part failed\n",
			__FUNCTION__, part_nu);
			return 0;
		}

		blocks_do_boot = mmc_bread(dev_num, start_todo, blkcnt, dst);
		if (blocks_do_boot != blkcnt) {
			MMCINFO("%s: get boot0 from boot part err\n",
			__FUNCTION__);
			return 0;
		} else {
			MMCDBG("%s: get boot0 from boot part ok\n",
			__FUNCTION__);
		}

		if (mmc_switch_part(dev_num, 0)) {
			MMCINFO("%s: switch back to user part failed\n",
			__FUNCTION__);
			return 0;
		}
		return blocks_do_boot;
	} else {
		MMCINFO("%s,%d: don't support boot partition\n",
		__FUNCTION__, __LINE__);
		return 0;
	}
}


/**
 * mmc_get_boot_cap - get emmc boot partition capacity
 * @dev_num: card number
 *
 * return number of sector of emmc boot partition capacity;
 */
u64 mmc_get_boot_cap(int dev_num)
{
	struct mmc *mmc = find_mmc_device(dev_num);
	return mmc->capacity_boot/512;
}


#ifdef CONFIG_SUNXI_SECURE_STORAGE

static int
check_secure_area(ulong start, lbaint_t blkcnt)
{
	u32 sta_add = start;
	u32 end_add = start + blkcnt -1;
	u32 se_sta_add = SDMMC_SECURE_STORAGE_START_ADD;
	u32 se_end_add = SDMMC_SECURE_STORAGE_START_ADD + (SDMMC_ITEM_SIZE * 2 * MAX_SECURE_STORAGE_MAX_ITEM) - 1;
	if (blkcnt <= (SDMMC_ITEM_SIZE * 2 * MAX_SECURE_STORAGE_MAX_ITEM)) {
		if (((sta_add >= se_sta_add) && (sta_add <= se_end_add))
			|| ((end_add >= se_sta_add) && (end_add <= se_end_add))) {
			return 1;
		}
	} else {
		if (((se_sta_add >= sta_add) && (se_sta_add <= end_add))
			|| ((se_end_add >= sta_add) && (se_end_add <= end_add))) {
			return 1;
		}
	}

	return 0;
}

static ulong
mmc_bread_secure(int dev_num, ulong start, lbaint_t blkcnt, void *dst)
{
	if (check_secure_area(start, blkcnt)) {
		MMCINFO("Should not w/r secure area in fun %s,line,%d in start %ld,end %ld\n",\
			__FUNCTION__, __LINE__, start, start + blkcnt-1);
		return -1;
	}

	return mmc_bread(dev_num, start, blkcnt, dst);
}

static ulong
mmc_bwrite_secure(int dev_num, ulong start, lbaint_t blkcnt, const void *src)
{
	if (check_secure_area(start, blkcnt)) {
		MMCINFO("Should not w/r secure area in fun %s,line,%d in start %ld,end %ld\n",\
			__FUNCTION__, __LINE__, start, start + blkcnt - 1);
		return -1;
	}

	return mmc_bwrite(dev_num, start, blkcnt, src);
}

static ulong
mmc_berase_secure(int dev_num, unsigned long start, lbaint_t blkcnt)
{
	if (check_secure_area(start, blkcnt)) {
		MMCINFO("Should not w/r secure area in fun %s,line,%d in start %ld,end %ld\n",\
			__FUNCTION__, __LINE__, start, start + blkcnt-1);
		return -1;
	}

	return mmc_berase(dev_num, start, blkcnt);
}

static ulong
mmc_bwrite_mass_pro_secure(int dev_num, ulong start, lbaint_t blkcnt, const void *src)
{
	if (check_secure_area(start, blkcnt)) {
		MMCINFO("Should not w/r secure area in fun %s,line,%d in start %ld,end %ld\n",\
			__FUNCTION__, __LINE__, start, start + blkcnt-1);
		return -1;
	}

	return mmc_bwrite_mass_pro(dev_num, start, blkcnt, src);
}

static ulong
mmc_bread_mass_pro_secure(int dev_num, ulong start, lbaint_t blkcnt, void *dst)
{
	if (check_secure_area(start, blkcnt)) {
		MMCINFO("Should not w/r secure area in fun %s,line,%d\n",
			__FUNCTION__, __LINE__);
		MMCINFO("start %ld,end %ld\n",
			start, start + blkcnt-1);
		return -1;
	}

	return mmc_bread_mass_pro(dev_num, start, blkcnt, dst);
}


static int
mmc_secure_wipe_secure(int dev_num, unsigned int start, unsigned int blkcnt, unsigned int *skip_space)
{
	if (check_secure_area(start, blkcnt)) {
		MMCINFO("Should not w/r secure area in fun %s,line,%d\n",
			__FUNCTION__, __LINE__);
		MMCINFO("start %ld,end %ld\n",
			start, start + blkcnt-1);
		return -1;
	}

	return mmc_secure_wipe(dev_num, start, blkcnt, skip_space);
}

static int
mmc_mmc_erase_secure(int dev_num, unsigned int start, unsigned int blkcnt, unsigned int *skip_space)
{
	if (check_secure_area(start, blkcnt)) {
		MMCINFO("Should not w/r secure area in fun %s,line,%d in start %d,end %d\n",\
			__FUNCTION__, __LINE__, start, start + blkcnt-1);
		return -1;
	}
	return mmc_mmc_erase(dev_num, start, blkcnt, skip_space);
}

static int
mmc_mmc_trim_secure(int dev_num, unsigned int start, unsigned int blkcnt)
{
	if (check_secure_area(start, blkcnt)) {
		MMCINFO("Should not w/r secure area in fun %s,line,%d in start %d,end %d\n",\
			__FUNCTION__, __LINE__, start, start + blkcnt-1);
		return -1;
	}
	return mmc_mmc_trim(dev_num, start, blkcnt);
}

static int
mmc_mmc_discard_secure(int dev_num, unsigned int start, unsigned int blkcnt)
{
	if (check_secure_area(start, blkcnt)) {
		MMCINFO("Should not w/r secure area in fun %s,line,%d in start %d,end %d\n",\
			__FUNCTION__, __LINE__, start, start + blkcnt-1);
		return -1;
	}
	return mmc_mmc_discard(dev_num, start, blkcnt);
}

static int
mmc_mmc_sanitize_secure(int dev_num)
{
	return mmc_mmc_sanitize(dev_num);
}
static int
mmc_mmc_secure_erase_secure(int dev_num, unsigned int start, unsigned int blkcnt, unsigned int *skip_space)
{
	if (check_secure_area(start, blkcnt)) {
		MMCINFO("Should not w/r secure area in fun %s,line,%d in start %d,end %d\n",\
			__FUNCTION__, __LINE__, start, start + blkcnt-1);
		return -1;
	}
	return mmc_mmc_secure_erase(dev_num, start, blkcnt, skip_space);
}

static int
mmc_mmc_secure_trim_secure(int dev_num, unsigned int start, unsigned int blkcnt)
{
	if (check_secure_area(start, blkcnt)) {
		MMCINFO("Should not w/r secure area in fun %s,line,%d in start %d,end %d\n",\
			__FUNCTION__, __LINE__, start, start + blkcnt-1);
		return -1;
	}
	return mmc_mmc_secure_trim(dev_num, start, blkcnt);
}

static int
get_sdmmc_secure_storage_max_item(void)
{
	return MAX_SECURE_STORAGE_MAX_ITEM;
}

static int
sdmmc_secure_storage_write(s32 dev_num, u32 item, u8 *buf, lbaint_t blkcnt)
{
	s32 ret = 0;

	if (buf == NULL) {
		MMCINFO("intput buf is NULL \n");
		return -1;
	}

	if (item > MAX_SECURE_STORAGE_MAX_ITEM) {
		MMCINFO("item exceed %d\n", MAX_SECURE_STORAGE_MAX_ITEM);
		return -1;
	}

	if (blkcnt > SDMMC_ITEM_SIZE) {
		MMCINFO("block count exceed %d\n", SDMMC_ITEM_SIZE);
		return -1;
	}
	/* first backups*/
	ret = mmc_bwrite(dev_num, SDMMC_SECURE_STORAGE_START_ADD + SDMMC_ITEM_SIZE * 2 * item, blkcnt, buf);
	if (ret != blkcnt) {
		MMCINFO("Write first backup failed\n");
		return -1;
	}
	/*second backups*/
	ret = mmc_bwrite(dev_num, SDMMC_SECURE_STORAGE_START_ADD + SDMMC_ITEM_SIZE * 2 * item + SDMMC_ITEM_SIZE, blkcnt, buf);
	if (ret != blkcnt) {
		MMCINFO("Write second backup failed\n");
		return -1;
	}
	return blkcnt;
}

static int
sdmmc_secure_storage_read(s32 dev_num, u32 item, u8 *buf, lbaint_t blkcnt)
{
	s32 ret = 0;
	s32 *fst_bak = NULL;
	s32 *sec_bak = NULL;

	if (buf == NULL) {
		MMCINFO("intput buf is NULL\n");
		ret = -1;
		goto out;
	}

	if (item > MAX_SECURE_STORAGE_MAX_ITEM) {
		MMCINFO("item exceed %d\n", MAX_SECURE_STORAGE_MAX_ITEM);
		ret = -1;
		goto out;
	}

	if (blkcnt > SDMMC_ITEM_SIZE) {
		MMCINFO("block count exceed %d\n", SDMMC_ITEM_SIZE);
		ret = -1;
		goto out;
	}

	fst_bak = malloc(blkcnt*512);
	if (fst_bak == NULL) {
		MMCINFO("malloc buff failed in fun %s line %d\n", __FUNCTION__, __LINE__);
		ret = -1;
		goto out;
	}
	sec_bak = malloc(blkcnt*512);
	if (sec_bak == NULL) {
		MMCINFO("malloc buff failed in fun %s line %d\n", __FUNCTION__, __LINE__);
		ret = -1;
		goto out_fst;
	}

	/*first backups*/
	ret = mmc_bread(dev_num, SDMMC_SECURE_STORAGE_START_ADD + SDMMC_ITEM_SIZE * 2 * item, blkcnt, fst_bak);
	if (ret != blkcnt) {
		MMCINFO("read first backup failed in fun %s line %d\n", __FUNCTION__, __LINE__);
		ret = -1;
		goto out_sec;
	}
	/*second backups*/
	ret = mmc_bread(dev_num, SDMMC_SECURE_STORAGE_START_ADD + SDMMC_ITEM_SIZE * 2 * item + SDMMC_ITEM_SIZE, blkcnt, sec_bak);
	if (ret != blkcnt) {
		MMCINFO("read second backup failed fun %s line %d\n", __FUNCTION__, __LINE__);
		ret = -1;
		goto out_sec;
	}

	if (memcmp(fst_bak, sec_bak, blkcnt * 512)) {
		MMCINFO("first and second bak compare failed fun %s line %d\n", __FUNCTION__, __LINE__);
		ret = -1;
	} else {
		memcpy(buf, fst_bak, blkcnt * 512);
		ret = blkcnt;
	}

out_sec:
	free(sec_bak);
out_fst:
	free(fst_bak);
out:
	return ret;
}

static int sdmmc_secure_storage_read_backup(s32 dev_num, u32 item, u8 *buf, lbaint_t blkcnt)
{
	s32 ret = 0;
	s32 *sec_bak = (s32 *)buf;

	if (buf == NULL) {
		MMCINFO("intput buf is NULL\n");
		ret = -1;
		goto out;

	}

	if (item > MAX_SECURE_STORAGE_MAX_ITEM) {
		MMCINFO("item exceed %d\n", MAX_SECURE_STORAGE_MAX_ITEM);
		ret = -1;
		goto out;
	}

	if (blkcnt > SDMMC_ITEM_SIZE) {
		MMCINFO("block count exceed %d\n", SDMMC_ITEM_SIZE);
		ret = -1;
		goto out;
	}

	//second backups
	ret = mmc_bread(dev_num, SDMMC_SECURE_STORAGE_START_ADD + SDMMC_ITEM_SIZE * 2 * item + SDMMC_ITEM_SIZE, blkcnt, sec_bak);
	if (ret != blkcnt) {
		MMCINFO("read second backup failed fun %s line %d\n", __FUNCTION__, __LINE__);
		ret = -1;
	}

out:
	return ret;
}
#endif

#endif

int mmc_init_blk_ops(struct mmc *mmc)
{
	mmc->block_dev.block_read = mmc_bread;
	mmc->block_dev.block_write = mmc_bwrite;
	mmc->block_dev.block_mmc_erase = mmc_mmc_erase;
	mmc->block_dev.block_erase = mmc_berase;
	mmc->block_dev.block_mmc_trim = mmc_mmc_trim;
	mmc->block_dev.block_mmc_discard = mmc_mmc_discard;
	mmc->block_dev.block_mmc_sanitize = mmc_mmc_sanitize;
	mmc->block_dev.block_mmc_secure_erase = mmc_mmc_secure_erase;
	mmc->block_dev.block_mmc_secure_trim = mmc_mmc_secure_trim;
	mmc->block_dev.block_mmc_secure_wipe = mmc_mmc_secure_wipe;

	//mmc->block_dev.block_mmc_user_get_wp_grp_size = mmc_mmc_user_get_wp_grp_size;
	//mmc->block_dev.block_mmc_user_write_protect = mmc_mmc_user_write_protect;
	//mmc->block_dev.block_mmc_clr_tem_wp = mmc_clr_tem_wp;

	return 0;
}


