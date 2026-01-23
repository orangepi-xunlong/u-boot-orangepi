/*
 * drivers/block/vblk.c
 * Minimal virtual block device (vblk) for U-Boot v2018.05
 * Supports FS probe (fs_set_blk_dev)
 */

#include <dm/device.h>
#include <dm/uclass.h>
#include <linux/errno.h>
#include <malloc.h>
#include <string.h>
#include <common.h>
#include <blk.h>
#include <dm.h>
#include <dm/device-internal.h>
#include <dm/lists.h>
#include <dm/uclass-internal.h>

#define VBLK_DRV_NAME    "ufs"
#define VBLK_DEV_NAME    "ufs"
#define VBLK_BLKSZ       4096
#define VBLK_BLKCNT      32768

/* Storage buffer */
static u8 vblk_storage[VBLK_BLKSZ * VBLK_BLKCNT];


struct mbr_part_entry {
	u8  boot_ind;     /* 0x80 = bootable */
	u8  start_head;
	u8  start_sector;
	u8  start_cyl;
	u8  sys_ind;      /* partition type */
	u8  end_head;
	u8  end_sector;
	u8  end_cyl;
	u32 start_lba;
	u32 size_lba;
} __packed;

/* ------------------ MBR ------------------ */
static void vblk_init_mbr(void)
{
	struct mbr_part_entry *p;
	u8 *mbr = vblk_storage;

	memset(mbr, 0, VBLK_BLKSZ);

	p = (struct mbr_part_entry *)(mbr + 0x1BE);

	p->boot_ind  = 0x00;
	p->sys_ind   = 0x83;        /* Linux */

	p->start_lba = cpu_to_le32(1024);
	p->size_lba  = cpu_to_le32(VBLK_BLKCNT - 1);

	mbr[510] = 0x55;
	mbr[511] = 0xAA;
}
/*
static void dump_mbr_parts(const u8 *buf)
{
	const struct mbr_part_entry *p;
	int i;

	for (i = 0; i < 4; i++) {
		p = (const struct mbr_part_entry *)(buf + 446 + i * 16);

		printf("Partition %d:\n", i);
		printf("  boot     : 0x%02x\n", p->boot_ind);
		printf("  type     : 0x%02x\n", p->sys_ind);
		printf("  start LBA: %u\n", p->start_lba);
		printf("  size LBA : %u\n", p->size_lba);
	}
}
*/
extern int sunxi_ufs_global_read(lbaint_t start, lbaint_t blkcnt, void *buffer);
extern int sunxi_ufs_global_write(lbaint_t start, lbaint_t blkcnt, void *buffer);

#define UFS_BLKSZ     4096
#define UBOOT_BLKSZ  512
#define BLK_RATIO    (UFS_BLKSZ / UBOOT_BLKSZ) // 8

static ulong vblk_read(struct udevice *dev, lbaint_t start,
					   lbaint_t blkcnt, void *buffer)
{
	int cnt = sunxi_ufs_global_read(start, blkcnt, buffer);
	//if(start == 0)
	//	dump_mbr_parts(buffer);
	return cnt;

	//if ((start + blkcnt) > VBLK_BLKCNT)
	//    return 0;

	//printf("%s, %d, %lu, %lu\n", __func__, __LINE__, start, blkcnt);
	//memcpy(buffer, &vblk_storage[start * VBLK_BLKSZ], blkcnt * VBLK_BLKSZ);
	//if(start == 0)
	//    dump_mbr_parts(buffer);

	//return blkcnt;
}

static ulong vblk_write(struct udevice *dev, lbaint_t start,
						lbaint_t blkcnt, const void *buffer)
{
	return sunxi_ufs_global_write(start, blkcnt, (void *)buffer);
	//if ((start + blkcnt) > VBLK_BLKCNT)
	//	return 0;

	//memcpy(&vblk_storage[start * VBLK_BLKSZ], buffer, blkcnt * VBLK_BLKSZ);
	//return blkcnt;
}

static const struct blk_ops vblk_ops = {
	.read  = vblk_read,
	.write = vblk_write,
};

static int vblk_bind(struct udevice *dev)
{
	struct blk_desc *desc = dev_get_uclass_platdata(dev);
	if (!desc)
		return -ENOMEM;

	//desc->devnum   = dev->seq;       /* devnum */
	//desc->blksz    = VBLK_BLKSZ;
	//desc->lba      = VBLK_BLKCNT;
	//desc->if_type  = IF_TYPE_UFS;

	//desc->part_type = PART_TYPE_DOS;
	//desc->hwpart    = 0;

	return 0;
}

static int vblk_probe(struct udevice *dev)
{
	vblk_init_mbr();
	return 0;
}

U_BOOT_DRIVER(vblk) = {
	.name   = VBLK_DRV_NAME,
	.id     = UCLASS_BLK,
	.bind   = vblk_bind,
	.probe  = vblk_probe,
	.ops    = &vblk_ops,
};

struct blk_desc *vblk_get_devnum_by_typename(const char *if_typename, int devnum)
{
	struct udevice *dev;
	struct uclass *uc;
	struct blk_desc *desc;
	int ret;

	if (strcmp(if_typename, "ufs") != 0)
		return NULL;

	ret = uclass_get(UCLASS_BLK, &uc);
	if (ret)
		return NULL;

	uclass_foreach_dev(dev, uc) {
		desc = dev_get_uclass_platdata(dev);
		if (!desc)
			continue;

		if (desc->devnum != devnum)
			continue;

		if (device_probe(dev))
			return NULL;

		return desc;
	}

	return NULL;
}

int vblk_create(struct blk_desc *pbds)
{
	struct udevice *dev;
	int ret;

	ret = blk_create_devicef(NULL,
				 VBLK_DRV_NAME,
				 VBLK_DEV_NAME,
				 IF_TYPE_UFS,
				 0,      /* devnum */
				 pbds->blksz,//VBLK_BLKSZ,
				 pbds->lba,//VBLK_BLKCNT,
				 &dev);
	if (ret) {
		printf("vblk: create device failed %d\n", ret);
		return ret;
	}

	printf("vblk: virtual block device created\n");
	return 0;
}
