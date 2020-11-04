// SPDX-License-Identifier: GPL-2.0+
#ifndef __SUNXI_HOST_MMC_H__
#define __SUNXI_HOST_MMC_H__

//static int mmc_clear_timing_para(int sdc_no);
//static int mmc_init_default_timing_para(int sdc_no);
//static int mmc_update_timing_para(int sdc_no);
//static void mmc_get_para_from_fex(int sdc_no);
//static void mmc_get_para_from_dtb(int sdc_no);
//static int mmc_resource_init(int sdc_no);
int sunxi_host_mmc_config(int sdc_no);

#endif /*  __SUNXI_HOST_MMC_H__ */
