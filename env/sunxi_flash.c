// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2008-2011 Freescale Semiconductor, Inc.
 */

/* #define DEBUG */

#include <common.h>

#ifdef CONFIG_SUNXI_UBIFS
#include <ubi_uboot.h>
#endif

#include <command.h>
#include <environment.h>
#include <fdtdec.h>
#include <linux/stddef.h>
#include <malloc.h>
#include <memalign.h>
#include <sunxi_flash.h>
#include <part.h>
#include <search.h>
#include <errno.h>
#include <sunxi_board.h>

DECLARE_GLOBAL_DATA_PTR;

const uchar sunxi_sprite_environment[] = {
#ifdef SUNXI_SPRITE_ENV_SETTINGS
	SUNXI_SPRITE_ENV_SETTINGS
#endif
	"\0"
};

static void use_sprite_env(void)
{
	extern struct hsearch_data env_htab;

	if (himport_r(&env_htab, (char *)sunxi_sprite_environment,
		      sizeof(sunxi_sprite_environment), '\0', H_INTERACTIVE, 0,
		      0, NULL) == 0)
		pr_err("Environment import failed: errno = %d\n", errno);
	gd->flags |= GD_FLG_ENV_READY;

	return;
}

#if defined(CONFIG_CMD_SAVEENV) && !defined(CONFIG_SPL_BUILD)
static inline int write_env(struct blk_desc *desc, uint blk_cnt, uint blk_start,
			    const void *buffer)
{
	uint n;

	n = blk_dwrite(desc, blk_start, blk_cnt, (u_char *)buffer);

	return (n == blk_cnt) ? 0 : -1;
}

#if 0//def CONFIG_SUNXI_UBIFS
static int env_sunxi_flash_save(void)
{
	ALLOC_CACHE_ALIGN_BUFFER(env_t, env_new, 1);

	int ret;

	ret = env_export(env_new);
	if (ret)
		goto fini;

	printf("Writing to env...\n");
	if (ubi_volume_write("env", (u_char *)env_new,
				CONFIG_ENV_SIZE)) {
		printf("failed\n");
		ret = 1;
		goto fini;
	}
	ret = 0;

fini:
	return ret;
}

#else

#ifdef CONFIG_SUNXI_REDUNDAND_ENVIRONMENT
static int env_sunxi_flash_save(void)
{
	ALLOC_CACHE_ALIGN_BUFFER(env_t, env_new, 1);

	int ret;
	struct blk_desc *desc;
	disk_partition_t info = { 0 };

	desc = blk_get_devnum_by_typename("sunxi_flash", 0);
	if (desc == NULL)
		return -ENODEV;

	ret = env_export(env_new);
	if (ret)
		goto fini;

	if (gd->env_valid == ENV_VALID) {
		puts("Writing to redundant env... ");
		ret = sunxi_flash_try_partition(desc, CONFIG_SUNXI_ENV_REDUNDAND_PARTITION, &info);
		if (ret < 0)
			return -ENODEV;

		if (write_env(desc, (CONFIG_ENV_SIZE + 511) / 512, (uint)info.start,
			      (u_char *)env_new)) {
			puts("failed\n");
			ret = 1;
			goto fini;
		}
	} else {
		puts("Writing to env... ");
		ret = sunxi_flash_try_partition(desc, CONFIG_SUNXI_ENV_PARTITION, &info);
		if (ret < 0)
			return -ENODEV;

		if (write_env(desc, (CONFIG_ENV_SIZE + 511) / 512, (uint)info.start,
			      (u_char *)env_new)) {
			puts("failed\n");
			ret = 1;
			goto fini;
		}
	}

	sunxi_flash_flush();
	sunxi_flash_write_end();
	ret = 0;

	gd->env_valid = gd->env_valid == ENV_REDUND ? ENV_VALID : ENV_REDUND;

fini:
	return ret;
}
#else
static int env_sunxi_flash_save(void)
{
	ALLOC_CACHE_ALIGN_BUFFER(env_t, env_new, 1);

	int ret;
	struct blk_desc *desc;
	disk_partition_t info = { 0 };

	desc = blk_get_devnum_by_typename("sunxi_flash", 0);
	if (desc == NULL)
		return -ENODEV;

	ret = sunxi_flash_try_partition(desc, "env", &info);
	if (ret < 0)
		return -ENODEV;
	ret = env_export(env_new);
	if (ret)
		goto fini;

	printf("Writing to env...\n");
	if (write_env(desc, (CONFIG_ENV_SIZE + 511) / 512, (uint)info.start,
		      (u_char *)env_new)) {
		puts("failed\n");
		ret = 1;
		goto fini;
	}

	sunxi_flash_flush();
	sunxi_flash_write_end();
	ret = 0;

fini:
	return ret;
}
#endif /* CONFIG_SUNXI_REDUNDAND_ENVIRONMENT */
#endif
#endif /* CONFIG_CMD_SAVEENV && !CONFIG_SPL_BUILD */

#if 0//def CONFIG_SUNXI_UBIFS

static int env_sunxi_flash_load(void)
{
	ALLOC_CACHE_ALIGN_BUFFER(char, buf, CONFIG_ENV_SIZE);
	int ret;
	char *errmsg = "!no device";
	int workmode = get_boot_work_mode();

	if ((workmode & WORK_MODE_PRODUCT) &&
	    (!(workmode & WORK_MODE_UPDATE))) {
		use_sprite_env();
		return 0;
	}

	ret = ubi_volume_read("env", buf, (CONFIG_ENV_SIZE + 511)/512);
	if(ret)
		goto err;

	ret = env_import(buf, 1);
err:
	if (ret)
		set_default_env(errmsg);

	return ret;
}
#else

static inline int read_env(struct blk_desc *desc, uint blk_cnt, uint blk_start,
			   const void *buffer)
{
	uint n;

	n = blk_dread(desc, blk_start, blk_cnt, (uchar *)buffer);

	return (n == blk_cnt) ? 0 : -1;
}

#ifdef CONFIG_SUNXI_REDUNDAND_ENVIRONMENT
static int env_sunxi_flash_load(void)
{
	ALLOC_CACHE_ALIGN_BUFFER(char, env1_buf, CONFIG_ENV_SIZE);
	ALLOC_CACHE_ALIGN_BUFFER(char, env2_buf, CONFIG_ENV_SIZE);
	struct blk_desc *desc;
	disk_partition_t info = { 0 };
	int ret;
	char *errmsg = "!no device";
	int workmode = get_boot_work_mode();

	if ((workmode & WORK_MODE_PRODUCT) &&
	    (!(workmode & WORK_MODE_UPDATE))) {
		use_sprite_env();
		return 0;
	}

	desc = blk_get_devnum_by_typename("sunxi_flash", 0);
	if (desc == NULL) {
		ret = -ENODEV;
		goto err;
	}

	int read1_fail, read2_fail;
	env_t *tmp_env1, *tmp_env2;

	memset(env1_buf, 0x0, CONFIG_ENV_SIZE);
	memset(env2_buf, 0x0, CONFIG_ENV_SIZE);

	tmp_env1 = (env_t *)env1_buf;
	tmp_env2 = (env_t *)env2_buf;

	ret = sunxi_flash_try_partition(desc, CONFIG_SUNXI_ENV_PARTITION, &info);
	if (ret < 0) {
		printf("Can't find %s partition\n", CONFIG_SUNXI_ENV_PARTITION);
		ret = -ENODEV;
		goto err;
	}
	read1_fail = read_env(desc, (CONFIG_ENV_SIZE + 511) / 512, (uint)info.start, env1_buf);
	if (read1_fail)
		printf("\n** Unable to read env data from %s partition **\n",
		       CONFIG_SUNXI_ENV_PARTITION);

	memset(&info, 0x0, sizeof(disk_partition_t));
	ret = sunxi_flash_try_partition(desc, CONFIG_SUNXI_ENV_REDUNDAND_PARTITION, &info);
	if (ret < 0) {
		printf("Can't find %s partition\n", CONFIG_SUNXI_ENV_REDUNDAND_PARTITION);
		ret = -ENODEV;
		goto err;
	}
	read2_fail = read_env(desc, (CONFIG_ENV_SIZE + 511) / 512, (uint)info.start, env2_buf);
	if (read2_fail)
		printf("\n** Unable to read env data from %s partition **\n",
		       CONFIG_SUNXI_ENV_REDUNDAND_PARTITION);

	return env_import_redund((char *)tmp_env1, read1_fail, (char *)tmp_env2,
							 read2_fail);

err:
	if (ret)
		set_default_env(errmsg);

	return ret;
}
#else
static int env_sunxi_flash_load(void)
{
	ALLOC_CACHE_ALIGN_BUFFER(char, buf, CONFIG_ENV_SIZE);
	struct blk_desc *desc;
	disk_partition_t info = { 0 };
	int ret;
	char *errmsg = "!no device";
	int workmode = get_boot_work_mode();

	if ((workmode & WORK_MODE_PRODUCT) &&
	    (!(workmode & WORK_MODE_UPDATE))) {
		use_sprite_env();
		return 0;
	}

	desc = blk_get_devnum_by_typename("sunxi_flash", 0);
	if (desc == NULL) {
		ret = -ENODEV;
		goto err;
	}
	ret = sunxi_flash_try_partition(desc, "env", &info);
	/* printf ("name:%s start:0x%x, size: 0x%x\n", info.name, (u32)info.start, (u32)info.size); */
	if (ret < 0) {
		ret = -ENODEV;
		goto err;
	}

	if (read_env(desc, (CONFIG_ENV_SIZE + 511) / 512, (uint)info.start,
		     buf)) {
		errmsg = "!read failed";
		ret    = -EIO;
		goto err;
	}
	ret = env_import(buf, 1);
err:
	if (ret)
		set_default_env(errmsg);

	return ret;
}
#endif /* CONFIG_SUNXI_REDUNDAND_ENVIRONMENT */
#endif

U_BOOT_ENV_LOCATION(sunxi_flash) = {
	.location		     = ENVL_SUNXI_FLASH,
	ENV_NAME("SUNXI_FLASH").load = env_sunxi_flash_load,
	.save			     = env_save_ptr(env_sunxi_flash_save),

};
