/*
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/smc.h>

int sunxi_smc_config(uint dram_size, uint secure_region_size)
{
#ifdef CONFIG_SECURE_MEMORY_TEST
	writel(0x3c000000, SMC_REGIN_SETUP_LOW_REG(2));
	/* 0xc:1100b 0x19:64M from 0x3c000000
	 * config secure dram to secure rw, non-secure can't access
	 */
	writel((0xc<<28) | (0x19<<1)| (0x1<<0),SMC_REGIN_ATTRIBUTE_REG(2));
#endif
	return 0;
}

int sunxi_drm_config(u32 dram_end, u32 secure_region_size)
{

	u32 ctrl = 0;
	u32 drm_region_ofs,bit_map_ofs,bitmap_write_len,i;
	u32 drm_region_sel = 0;
	u32 dram_size = dram_end - CONFIG_SYS_SDRAM_BASE;
	if(secure_region_size <= 0)
	{
			return 0;
	}
	/*bitmap: total len 32K Bytes
	          1bit   --->4K
	          256bit --->1M
	          32*1024 Bytes ---> 1G Bytes
	*/
	drm_region_ofs = (dram_size-secure_region_size)%1024;
	bit_map_ofs = (drm_region_ofs*256);
	bitmap_write_len = secure_region_size*256;
	printf("drm_region_ofs = %d, bitmap_ofs = %d, bitmap_write_len = %d\n",
		drm_region_ofs,bit_map_ofs,bitmap_write_len);
	for(i = 0; i < bitmap_write_len; i+=8)
	{
		/* data 8bit | len | addr */
		ctrl = (0xff<<24) | (0x8<<20) |  (bit_map_ofs+i);
		writel(ctrl ,DRM_BITMAP_CTRL_REG);
	}

	/* 0x4000 0000 ~ 0x7FFF FFFF  region0
	   0x8000 0000 ~ 0xBFFF FFFF  region1
	   0xC000 0000 ~ 0xFFFF FFFF  region2
	*/
	if(dram_size > (2<<30)) /* > 2G*/
	{
		drm_region_sel = 0x2;
	}
	else if(dram_size > (1<<30)) /* > 1G */
	{
		drm_region_sel = 0x1;
	}
	else
	{
		drm_region_sel = 0x0;
	}
	writel(drm_region_sel ,DRM_BITMAP_SEL_REG);

	/* drm enable */
	/* writel(1<<31, SMC_DRM_MATER0_EN_REG); */
	/* config smc action: non-secure master access protected region get 0*/
	writel(0x0,SMC_ACTION_REG);
	/* config all master(cpu|gpu...) access dram by SMC*/
	writel(0,SMC_MST0_BYP_REG);
	writel(0,SMC_MST0_BYP_REG);
	writel(0,SMC_MST0_BYP_REG);
	/* config config all master(cpu|gpu...) to non-secure mode */
	writel(0xffffffff, SMC_MST0_SEC_REG);
	writel(0xffffffff, SMC_MST1_SEC_REG);
	writel(0xffffffff, SMC_MST2_SEC_REG);
	/* 0xf : config dram to secure rw, non-secure rw */
	/* 0x1f: 4G area */
	/* 0x1 : enable */
	writel((0xf<<28) | (0x1f<<1)| (0x1<<0),SMC_REGIN_ATTRIBUTE_REG(1));
#ifdef DRM_DEBUG
	u32 val;
	for(i = 0; i < bitmap_write_len/32; i+=32)
	{
		ctrl = (0x0<<20) |  (bit_map_ofs+i);
		writel(ctrl ,DRM_BITMAP_CTRL_REG);
		val = readl(DRM_BITMAP_VAL_REG);
		printf("0x%x ", val);
		if((i+1)%16 == 0) printf("\n");
	}
	printf("\n");
	printf("secure enable bit: %x\n", readl(0x03006248));
	printf("secure DRM bit: %x\n", readl(SMC_DRM_MATER0_EN_REG));
#endif
	return 0;
}


