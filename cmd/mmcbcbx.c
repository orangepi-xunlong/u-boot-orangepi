// SPDX-License-Identifier: GPL-2.0+

#include <stdlib.h>
#include <common.h>
#include <config.h>
#include <command.h>
#include <part.h>
#include <vsprintf.h>

#define DEBUG
#ifdef DEBUG
# undef CONFIG_LOGLEVEL
# define CONFIG_LOGLEVEL 8
#endif

static int get_part_info(const char *part_name, struct blk_desc *desc,
			 struct disk_partition *info)
{
	int p;

	for (p = 1; p < 128; p++) {
		int r = part_get_info(desc, p, info);

		if (r == 0)
			if (!strcmp((char *)info->name, part_name))
				break;
	}

	return p;
}

static int do_mmcbcbx(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
	int ret;
	struct blk_desc *desc;
	char *var = NULL;
	char *bcb_ptr = NULL;
	struct disk_partition info;
	int count = 0;
	unsigned long index;
	char *vars[32] = {0}, *token;
	char env_str[64];
	int recovery = 0, ota = 0;
	int misc_part_num = 0, ota_part_num = 0;
	int argv_offset;

	if (argc < 4)
		return CMD_RET_USAGE;

	if (!strncmp(argv[1], "boot-recovery", 13))
		recovery = 1;
	else if (!strncmp(argv[1], "ota", 3))
		ota = 1;

	argv_offset = (recovery || ota);
	ret = blk_get_device_by_str(argv[1 + argv_offset], argv[2 + argv_offset], &desc);
	if (ret < 0)
		return CMD_RET_FAILURE;

	misc_part_num = get_part_info("MISC", desc, &info);

	if (misc_part_num >= 128)
		return CMD_RET_FAILURE;

	if (!part_get_info(desc, misc_part_num, &info)) {
		bcb_ptr = memalign(info.blksz, info.blksz * 2);
		if (blk_dread(desc, (lbaint_t)info.start, 2, bcb_ptr) != 2) {
			printf("blk_dread err, count != 2\n");
			return CMD_RET_FAILURE;
		}
	}

	token = strtok(bcb_ptr, "\n");
	while (token) {
		vars[count] = malloc(strlen(token) + 1);
		if (!vars[count])
			return -ENOMEM;
		memset(vars[count], 0, strlen(token) + 1);
		memcpy(vars[count++], token, strlen(token));
		token = strtok(NULL, "\n");
	}
	vars[count] = NULL;

	if (recovery) {
		var = argv[4];
		sprintf(env_str, "%d", misc_part_num);
		env_set(var, env_str);

		var = argv[5];
		env_set(var, vars[0] ?: "None");

		// printf("%s: %s\n",var, env_get(var));

		return CMD_RET_SUCCESS;
	}

	if (ota) {
		char *save;

		if (!vars[1] || !strlen(vars[1]))
			return CMD_RET_FAILURE;

		save = malloc(strlen(vars[1]) + 1);
		if (!save)
			return -ENOMEM;

		memset(save, 0, strlen(vars[1]) + 1);
		memcpy(save, vars[1], strlen(vars[1]));
		token = strtok(vars[1], "/");
		if (token) {
			ota_part_num = get_part_info(token, desc, &info);
			if (ota_part_num >= 128)
				return CMD_RET_FAILURE;

			var = argv[4];
			sprintf(env_str, "%d", ota_part_num);
			env_set(var, env_str);

			var = argv[5];
			env_set(var, token ? save + strlen(token) + 1 : "None");
			return CMD_RET_SUCCESS;
		}
	}

	if (argc == 5) {
		var = argv[4];
		if (!str2long(argv[3], &index)) {
			printf("args index not a number\n");
			goto fail;
		}

		if (index < count) {
			env_set(var, vars[index]);
			return CMD_RET_SUCCESS;
		}
	}

fail:
	return CMD_RET_FAILURE;
}

U_BOOT_CMD(
	mmcbcbx, 6,	1, do_mmcbcbx,
	"bootloader control block command extend\n",
	"get the bcb info\n"
	"mmcbcbx boot-recovery <interface> <dev> <misc_part> <is_recovery>\n"
	"mmcbcbx ota <interface> <dev> <ota_part> <ota_path>\n"
	"mmcbcbx <interface> <dev> <index> <var>\n"
);

