// SPDX-License-Identifier: GPL-2.0+
#ifndef __SUNXI_MMC_HOST_COMMON_H__
#define __SUNXI_MMC_HOST_COMMON_H__

struct sunxi_mmc_host_table {
		char *tm_str;
		void (*sunxi_mmc_host_init_priv)(int sdc_no);
};

int sunxi_host_mmc_config(int sdc_no);

#endif /*  __SUNXI_MMC_HOST_COMMON_H__ */
