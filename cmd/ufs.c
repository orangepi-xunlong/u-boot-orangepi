#include <common.h>
#include <command.h>
#include <console.h>

extern int sunxi_ufs_global_init(void);
extern int sunxi_ufs_global_read(lbaint_t start, lbaint_t blkcnt, void *buffer);
extern int sunxi_ufs_global_write(lbaint_t start, lbaint_t blkcnt, void *buffer);
extern u64 sunxi_ufs_global_ufs_size(void);

static int do_ufs_init(void)
{
	int ret = sunxi_ufs_global_init();
	if (ret) {
		printf("UFS init failed: %d\n", ret);
		return CMD_RET_FAILURE;
	}

	u64 size = sunxi_ufs_global_ufs_size();
	printf("UFS initialized: %llu blocks (", size);
	print_size((u64)size * 4096, ")\n");
	return CMD_RET_SUCCESS;
}

static int do_ufs_read(ulong addr, lbaint_t start, lbaint_t cnt)
{
	int ret = sunxi_ufs_global_read(start, cnt, (void *)addr);
	if (ret < 0) {
		printf("Read failed: %d\n", ret);
		return CMD_RET_FAILURE;
	}
	printf("Read %d blocks\n", ret);
	return CMD_RET_SUCCESS;
}

static int nvme_curr_dev;

static int do_ufs(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	if (argc < 2)
		goto usage;

	if (!strcmp(argv[1], "init")) {
		return do_ufs_init();
	}

	if (!strcmp(argv[1], "read") && argc == 5) {
		ulong addr = simple_strtoul(argv[2], NULL, 16);
		lbaint_t start = simple_strtoul(argv[3], NULL, 10);
		lbaint_t cnt = simple_strtoul(argv[4], NULL, 10);
		return do_ufs_read(addr, start, cnt);
	}

	if (!strcmp(argv[1], "info")) {
		u64 size = sunxi_ufs_global_ufs_size();
		printf("UFS blocks: %llu\n", size);
		printf("UFS size: ");
		print_size((u64)size * 4096, "\n");
		return CMD_RET_SUCCESS;
	}
	return blk_common_cmd(argc, argv, IF_TYPE_UFS, &nvme_curr_dev);

usage:
	printf("Usage:\n");
	printf("  ufs init                    - Initialize UFS\n");
	printf("  ufs info                    - Show UFS info\n");
	printf("  ufs read <addr> <blk> <cnt> - Read blocks\n");
	return CMD_RET_USAGE;
}

U_BOOT_CMD(
	ufs, 5, 1, do_ufs,
	"Simple UFS commands",
	"init                    - Initialize UFS\n"
	"info                    - Show UFS information\n"
	"read <addr> <blk> <cnt> - Read blocks\n"
);
