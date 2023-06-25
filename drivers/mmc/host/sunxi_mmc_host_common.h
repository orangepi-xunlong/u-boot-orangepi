// SPDX-License-Identifier: GPL-2.0+
#ifndef __SUNXI_MMC_HOST_COMMON_H__
#define __SUNXI_MMC_HOST_COMMON_H__

struct sunxi_mmc_host_table {
		char *tm_str;
		void (*sunxi_mmc_host_init_priv)(int sdc_no);
};

int sunxi_host_mmc_config(int sdc_no);

int sunxi_mmc_get_src_clk_no(int sdc_no, int mod_hz, int tm);
int sunxi_host_src_clk(int sdc_no, int src_clk, int tm);

#endif /*  __SUNXI_MMC_HOST_COMMON_H__ */
