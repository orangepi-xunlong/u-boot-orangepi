// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2007-2012
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * Description: UFS driver for General UFS operations
 * Author: lixiang <lixiang@allwinnertech.com>
 * Date: 2022/11/15 15:23
 */
//#include <types.h>
#include <common.h>
#include "ufs.h"

#include "./uinclude/scsi.h"
#include <device.h>
#include <./uinclude/blk.h>
#include <log.h>
#include "errno.h"
//#include "include.h"
#include "device_compat.h"
#include "ufs_main.h"
//#include "lib.h"

#include "sunxi-ufs-trace.h"
#include "sunxi-ufs.h"

extern int  ufs_bind(struct udevice *udev);
extern int  ufs_unbind(struct udevice *udev);
extern void ufshcd_hba_exit(struct ufs_hba *hba);
extern int ufshcd_probe(struct udevice *ufs_dev, struct ufs_hba_ops *hba_ops);
extern int ufs_probe(struct udevice *dev);
extern void ufs_hda_ops_bind(struct udevice *dev);

struct udevice g_dev;
struct blk_desc g_bds;
struct blk_ops g_sc_bops;

extern int vblk_create(struct blk_desc *);

int sunxi_ufs_init(struct udevice *udev)
{
	int ret = 0;
	struct blk_desc *pbds = g_dev.bd;

	ufs_bind(udev);
	ret = ufs_probe(udev);
	if (ret)
		return ret;
	ret = scsi_scan_dev(udev, true);
	vblk_create(pbds);
	return ret;
}

void sunxi_ufs_exit(struct udevice *udev)
{
	ufshcd_hba_exit(&udev->uhba);
	ufs_unbind(udev);
}

void sunxi_ufs_global_exit(void)
{
	sunxi_ufs_exit(&g_dev);
	memset(&g_dev, 0, sizeof(struct udevice));
	memset(&g_bds, 0, sizeof(struct blk_desc));
	memset(&g_sc_bops, 0, sizeof(struct blk_ops));
}


int sunxi_ufs_read(struct udevice *udev, lbaint_t start,
			lbaint_t blkcnt, void *buffer)
{
	const struct blk_ops *ops = (udev->scsi_blk_ops);
	ulong blks_read;

	if (!ops->read)
		return -ENOSYS;

	blks_read = ops->read(udev, start, blkcnt, buffer);

	return blks_read;
}

int sunxi_ufs_write(struct udevice *udev, lbaint_t start,
			lbaint_t blkcnt, void *buffer)
{
	const struct blk_ops *ops = (udev->scsi_blk_ops);
	ulong blks_write;

/*
	if(start==0){
		volatile int i=1;
		while(i);
	}
*/

	if (!ops->write)
		return -ENOSYS;

	blks_write = ops->write(udev, start, blkcnt, buffer);

	return blks_write;
}




int sunxi_ufs_global_init(void)
{
	memset(&g_dev, 0, sizeof(struct udevice));
	memset(&g_bds, 0, sizeof(struct blk_desc));
	memset(&g_sc_bops, 0, sizeof(struct blk_ops));

	g_dev.bd = &g_bds;
	g_dev.scsi_blk_ops = &g_sc_bops;

	return sunxi_ufs_init(&g_dev);

}

int sunxi_ufs_global_read(lbaint_t start,
			lbaint_t blkcnt, void *buffer)
{
	return sunxi_ufs_read(&g_dev, start, blkcnt, buffer);
}


int sunxi_ufs_global_write(lbaint_t start,
			lbaint_t blkcnt, void *buffer)
{
	return sunxi_ufs_write(&g_dev, start, blkcnt, buffer);
}

u64 sunxi_ufs_global_ufs_size(void)
{
	struct blk_desc *pbds = g_dev.bd;
	return pbds->lba;
}

u64 sunxi_ufs_global_update_ufs_size(void)
{
	struct blk_desc *pbds = g_dev.bd;
	struct scsi_cmd ccb = {0};
	lbaint_t capacity = 0;
	unsigned long blksz = 0;

	ccb.target = pbds->target;
	ccb.lun = pbds->lun;
	if (scsi_read_capacity(&g_dev, &ccb, &capacity, &blksz)) {
		//scsi_print_error(pccb);
		return -EINVAL;
	}
	/*From ufs spec,read capacity cmd only read :
	 * "last addressable block on medium under control of logical unit (LU)"
	 * not the number of blocks.
	 * Also in kernel,if we not plus one,kernel will report
	 * "GPT:Alternate GPT header not at the end of the disk"**/
	capacity++;
	dev_info(NULL, "blk size %ld,capacity %ld block\n", blksz, capacity);

	pbds->lba = capacity;
	/*only support 4k block size,ufs use min block size 4k,if over 4k,
	 * should repartition to make ufs device use 4k block size
	 * */
	pbds->blksz = 4096;

	return pbds->lba;
}


s32 sunxi_ufs_global_sync_cache(void)
{
	return scsi_sync_cache(&g_dev); ;
}

s32 sunxi_ufs_global_format_unit(void)
{
	return scsi_format_unit(&g_dev); ;
}

s32 sunxi_ufs_global_unmap(u64 start_lba, u32 nr_blocks)
{
	return scsi_unmap(&g_dev, start_lba, nr_blocks);
}

s32 sunxi_ufs_global_enable_erase(void)
{
	return ufshcd_enable_erase_by_lun(&g_dev.uhba, 0);
}

s32 sunxi_ufs_global_enable_data_reliability(void)
{
	return ufshcd_enable_data_reliability_by_lun(&g_dev.uhba, 0);
}

s32 sunxi_ufs_global_logical_unit_config(void)
{
	return ufshcd_logical_unit_config(&g_dev.uhba, 1);
}


