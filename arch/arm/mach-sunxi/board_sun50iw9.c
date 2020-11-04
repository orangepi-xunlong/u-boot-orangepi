/*
 * Allwinner Sun50iw3 clock register definitions
 *
 * (C) Copyright 2017  <weidonghui@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <sunxi_board.h>
#include <fdt_support.h>
#include <sys_config.h>
#include <sunxi_power/axp.h>
#include <asm/arch/efuse.h>

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_SUNXI_OVERLAY
int sunxi_overlay_apply_merged(void *dtb_base, void *dtbo_base)
{
	int nodeoffset = 0;
	char axp_name[16] = {0};
	u8 axp_chipid = 0;
	char axp_path[] = "pmu0";
	char *node_compatible;

	nodeoffset = fdt_path_offset(dtb_base, axp_path);
	if (nodeoffset < 0) {
		pr_err("error: %s : %s\n", __func__, fdt_strerror(nodeoffset));
		return -1;
	}
	fdt_getprop_string(dtb_base, nodeoffset, "compatible", &node_compatible);
	pmu_get_info(axp_name, &axp_chipid);
	if (strstr(node_compatible, axp_name) == NULL) {
		return fdt_check_header(dtbo_base);
	}
	return -1;

}
#endif

#ifdef CONFIG_SUNXI_LIMIT_CPU_FREQ
static int fdt_set_cpu_dvfstable(u32 limit_max_freq)
{
	int ret = 0;
	uint64_t max_freq = 0;
	int  nodeoffset;	/* node offset from libfdt */
	char path_tmp[128] = {0};
	char node_name[32] = {0};
	int i;
	u32 opp_dvfs_table[] = {816000000, 1008000000, 1200000000, 1416000000};
	uchar max_flag;
	max_flag = 0;
	for (i = 0 ; i < sizeof(opp_dvfs_table)/sizeof(opp_dvfs_table[0]) ; i++) {
		sprintf(path_tmp, "/opp_l_table/opp@%d", opp_dvfs_table[i]);

		nodeoffset = fdt_path_offset (working_fdt, path_tmp);
		if (nodeoffset < 0) {
			/*
			* Not found or something else bad happened.
			*/
			debug ("can't find node \"%s\"!!!!\n", path_tmp);
			return 0;
		}
		sprintf(node_name, "opp-hz");
		ret = fdt_getprop_u64(working_fdt, nodeoffset,
					node_name, &max_freq);
		if (ret < 0) {
			pr_error("Get %s%s failed\n", path_tmp, node_name);
			return 0;
		}
		debug("max_freq:%lld\n", max_freq);
		if (max_freq >= limit_max_freq) {
			if (!max_flag) {
				max_freq = limit_max_freq;
			}
		} else {
			continue;
		}
		if (!max_flag) {
			ret = fdt_setprop_u64(working_fdt, nodeoffset, node_name,
					max_freq);
			max_flag = 1;
			if (ret < 0) {
				debug("Set %s%s failed\n", path_tmp, node_name);
				return -1;
			}
		} else {
			ret = fdt_del_node(working_fdt, nodeoffset);
			if (ret < 0) {
				debug("del %d failed\n", nodeoffset);
				return -1;
			}
		}
	}

	return 0;
}
#endif

#ifdef CONFIG_SUNXI_CHECK_LIMIT_VERIFY
int sunxi_check_cpu_gpu_verify(void)
{
	u32 chipid = 0, ret = 0;
	__maybe_unused u32 cpu_dvf = -1;

	chipid = sid_read_key(0x0) & 0xffff;
	switch (chipid) {
	case 0x5000:
	case 0x5400:
	case 0x7400:
	case 0x2400:
	case 0x6c00:
	/*H616 || VMP1002 || T507 || T517 || H700*/
		cpu_dvf = 1416000000;
		break;
	case 0x5c00:
	case 0x2c00:
	case 0x7c00:
	/*H313 || H513 || H503*/
		cpu_dvf = 1200000000;
		break;
	default:
		pr_force("illegal markid:%x !!!", chipid);
		sunxi_board_shutdown();
		return 0;
	}
#ifdef CONFIG_SUNXI_LIMIT_CPU_FREQ
	ret = fdt_set_cpu_dvfstable(cpu_dvf);
#endif
	return ret;
}
#endif


