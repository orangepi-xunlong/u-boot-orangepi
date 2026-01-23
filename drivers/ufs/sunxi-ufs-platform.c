// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2007-2012
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * Description: UFS driver for General UFS operations
 * Author: lixiang <lixiang@allwinnertech.com>
 * Date: 2022/11/15 15:23
 */

/*
#include <clk.h>
#include <common.h>
#include <dm.h>
#include <ufs.h>
#include <asm/io.h>
#include <dm/device_compat.h>
#include <linux/bitops.h>
#include <linux/err.h>
*/
#include "common.h"
#include <types.h>
#include "./uinclude/scsi.h"

#include <device_compat.h>
#include <string.h>

#include "ufs.h"
#include "scsi.h"
#include "blk.h"

#include <device.h>
#include <ufs.h>
#include <ufshcd-dwc.h>
#include <tc-dwc.h>
#include <ufshci-dwc.h>
#include <linux/kernel.h>
#include <asm/arch/efuse.h>
#include <ufshcd-pltfrm.h>
#include "sunxi-ufs.h"
#include "sunxi-ufs-trace.h"


#define SUNXI_UFS_DRIVER_VESION "0.0.24 2024.12.30 16:00"

//#define PHY_DEBUG_DUMP

#define UNUSED_OTEHR_PHY_SETTING
#define USEC_PER_SEC	1000000L


struct ufs_sunxi_priv {
	int phy_hs_rate;
} sunxi_ufs_host_priv;



extern u32 get_hosc_type(void);
extern u32 get_hosc_type_ufs(void);
extern int ufshcd_wait_for_register(struct ufs_hba *hba, u32 reg, u32 mask,
				    u32 val, unsigned long long timeout_ms);


#define SUNXI_UFS_CAL_WORDS_EFUSE_ALIGN_LOW			(0x60)
#define SUNXI_UFS_CAL_WORDS_EFUSE_ALIGN_HIGH		(0x64)

static inline int sunxi_ufs_get_cal_words(struct ufs_hba *hba, u32 *pll_rate_a, u32 *pll_rate_b, \
										u32 *att_lane0, u32 *ctle_lane0, \
										u32 *att_lane1, u32 *ctle_lane1)
{
	u32 rval_l  = sid_read_key(SUNXI_UFS_CAL_WORDS_EFUSE_ALIGN_LOW);
	u32 rval_h  = sid_read_key(SUNXI_UFS_CAL_WORDS_EFUSE_ALIGN_HIGH);

	//dev_info(hba->dev, "Cal words 0x%x:val 0x%x, 0x%x:val 0x%x\n",
	//					SUNXI_UFS_CAL_WORDS_EFUSE_ALIGN_LOW, rval_l,
	//					SUNXI_UFS_CAL_WORDS_EFUSE_ALIGN_HIGH, rval_h);
	*pll_rate_a = (rval_h >> 16) & 0xff;
	*pll_rate_b = (rval_h >> 24) & 0xff;

	if (*pll_rate_a && *pll_rate_b) {
		if ((*pll_rate_a < 0x14) \
			|| (*pll_rate_a > 0x4b)\
			|| (*pll_rate_b < 0x4b)\
			|| (*pll_rate_b > 0x73)) {
			dev_err(hba->dev, "pll cal words over valid range"
					"pll rate a 0x%08x, pll rate b 0x%08x\n", *pll_rate_a, *pll_rate_b);

			if ((*pll_rate_b < 0x4b) ||\
				(*pll_rate_b > 0x73))
				*pll_rate_b = 0;
			if ((*pll_rate_a < 0x14) \
				|| (*pll_rate_a > 0x4b))\
				*pll_rate_a = 0;
		}
	}


	*att_lane0 = (rval_l >> 16) & 0xff;
	*ctle_lane0 = (rval_l >> 24) & 0xff;
	if ((*att_lane0) \
		&& (*ctle_lane0)) {
		if ((*att_lane0 == 0) \
			|| (*att_lane0 == 255)\
			|| (*ctle_lane0 == 0)\
			|| (*ctle_lane0 == 255)) {
			dev_err(hba->dev, "afe cal words over valid range"
					"att lane0 0x%08x, ctle_lane0 0x%08x\n",
					*att_lane0, *ctle_lane0);
			return -1;
		}
	}

	*att_lane1 = (rval_h >> 0) & 0xff;
	*ctle_lane1 = (rval_h >> 8) & 0xff;
	if ((*att_lane1) \
		&& (*ctle_lane1)) {
		if ((*att_lane1 == 0) \
			|| (*att_lane1 == 255)\
			|| (*ctle_lane1 == 0)\
			|| (*ctle_lane1 == 255)) {
			dev_err(hba->dev, "afe cal words over valid range"
					"att lane1 0x%08x, ctle_lane1 0x%08x\n",
					*att_lane1, *ctle_lane1);
			return -1;
		}
	}

	dev_dbg(hba->dev, "pll rate a 0x%08x, pll rate b 0x%08x\n",\
						*pll_rate_a, *pll_rate_b);
	dev_dbg(hba->dev, "att lane0 0x%08x, ctle_lane0 0x%08x\n",\
						*att_lane0, *ctle_lane0);
	dev_dbg(hba->dev, "att lane1 0x%08x, ctle_lane1 0x%08x\n",
						*att_lane1, *ctle_lane1);

	if (!(*pll_rate_a) && !(*pll_rate_b)\
		&& !(*att_lane0) && !(*ctle_lane0)\
		&& !(*att_lane1) && !(*ctle_lane1)) {
		dev_info(hba->dev, "Auto mode,default afe\n");
	}

	return 0;
}




#define HOSC_24M        0x0
#define HOSC_19_2M      0x1
#define HOSC_26M        0x2
#define HOSC_CLK	0
#define PLL_CLK		1

int global_hosc_type = HOSC_24M;

u32 get_hosc_type(void)
{
	u32 val = 0, last_val = 0;
	u32 counter = 0;

	last_val = (readl(SUNXI_XO_CTRL_REG) >> 14) & 0x3;
	while (counter < 3) {
		val = (readl(SUNXI_XO_CTRL_REG) >> 14) & 0x3;
		if (val == last_val) {
			counter++;
		} else {
			counter = 0;
			last_val = val; //update
		}
		udelay(3);
	}

	//init
	global_hosc_type = val;

	switch (val) {
	case HOSC_24M:
		val = HOSC_24M;
		break;
	case HOSC_19_2M:
		val = HOSC_19_2M;
		break;
	case HOSC_26M:
		val = HOSC_26M;
		break;
	default:
		val = HOSC_24M;
		break;
	}
	return val;
}

u32 get_hosc_type_ufs(void)
{
	return global_hosc_type;
}

#define SYS_CFG_BASE							SUNXI_SYSCFG
#define RESCAL_CTRL_REG							(SYS_CFG_BASE + 0x0160)
#define RES0_CTRL_REG									(SYS_CFG_BASE + 0x0164)
#define RES1_CTRL_REG									(SYS_CFG_BASE + 0x0168)
#define RESCAL_STATUS_REG						(SYS_CFG_BASE + 0x016C)

void sunxi_soc_ext_res(void)
{
	writel(readl(RESCAL_CTRL_REG) | (0x1 << 13), RESCAL_CTRL_REG);//use EXT Res200
	writel(readl(RES1_CTRL_REG) & ~(0xFF << 24), RES1_CTRL_REG);
}




s32 efuse_cfg_ufs_use_ccu_en_app(void)
{
/*
	if (readl(SUNXI_SID_BASE + EFUSE_BROM_CONFIG) & EFUSE_CFG_UFS_USE_CCU_EN_APP) {
		return 1;
	} else {
		return 0;
	}
*/
	dev_dbg(NULL, "%s,unsupport now\n", __FUNCTION__);
	return 0;
}


/**
 * ufshcd_dwc_link_is_up()
 * Check if link is up
 * @hba: private structure pointer
 *
 * Returns 0 on success, non-zero value on failure
 */
static int ufshcd_dwc_link_is_up(struct ufs_hba *hba)
{
	u32 dme_result = 0;

	ufshcd_dme_get(hba, UIC_ARG_MIB(VS_POWERSTATE), &dme_result);

	if (dme_result == UFSHCD_LINK_IS_UP) {
		/*ufshcd_set_link_active(hba);*/
		return 0;
	}

	return 1;
}


int ufshcd_dwc_dme_set_attrs(struct ufs_hba *hba,
				const struct ufshcd_dme_attr_val *v, int n)
{
	int ret = 0;
	int attr_node = 0;

	for (attr_node = 0; attr_node < n; attr_node++) {
		ret = ufshcd_dme_set_attr(hba, v[attr_node].attr_sel,
			ATTR_SET_NOR, v[attr_node].mib_val, v[attr_node].peer);
		if (ret)
			return ret;
	}

	return 0;
}

int ufshcd_dwc_dme_dump_attrs(struct ufs_hba *hba,
				const struct ufshcd_dme_attr_val *v, int n)
{
	int ret = 0;
	int attr_node = 0;
	u32 value = 0;

	for (attr_node = 0; attr_node < n; attr_node++) {
		ret = ufshcd_dme_get_attr(hba, v[attr_node].attr_sel,
			&value, v[attr_node].peer);
		if (ret)
			return ret;
		dev_err(hba->dev, "dme attr sel 0x%08x, value 0x%08x\n",\
					v[attr_node].attr_sel, value);
	}

	return 0;
}


/**
 * tc_dwc_g240_c10_read()
 *
 * This function reads from the c10 interface in Synopsys G240 TC
 * @hba: Pointer to driver structure
 * @c10_reg: c10 register to read from
 * @c10_val: c10 value read
 *
 * Returns 0 on success or non-zero value on failure
 */
int tc_dwc_cr_read(struct ufs_hba *hba, u16 reg, u16 *val)
{
	struct ufshcd_dme_attr_val data[] = {
		{ UIC_ARG_MIB(TC_CBC_REG_ADDR_LSB), TC_LSB(0), DME_LOCAL },
		{ UIC_ARG_MIB(TC_CBC_REG_ADDR_MSB), TC_MSB(0), DME_LOCAL },
/*		{ UIC_ARG_MIB(TC_CBC_REG_WR_LSB), 0xff, DME_LOCAL },
		{ UIC_ARG_MIB(TC_CBC_REG_WR_MSB), 0xff, DME_LOCAL },
*/
		{ UIC_ARG_MIB(TC_CBC_REG_RD_WR_SEL), 0x0, DME_LOCAL },
		{ UIC_ARG_MIB(VS_MPHYCFGUPDT), 0x1, DME_LOCAL },
	};
	u32 tmp_rd_lsb = 0;
	u32 tmp_rd_msb = 0;
	int ret = 0;

	data[0].mib_val = TC_LSB(reg);
	data[1].mib_val = TC_MSB(reg);


	ret = ufshcd_dwc_dme_set_attrs(hba, data, ARRAY_SIZE(data));

/*	ret |= ufshcd_dme_set_attr(hba, UIC_ARG_MIB(TC_CBC_REG_RD_LSB), 0x00,
					0x00, DME_LOCAL);*/
	ret |= ufshcd_dme_get_attr(hba, UIC_ARG_MIB(TC_CBC_REG_RD_LSB),
					&tmp_rd_lsb, DME_LOCAL);
/*	ret |= ufshcd_dme_set_attr(hba, UIC_ARG_MIB(TC_CBC_REG_RD_MSB), 0x00,
					0x00, DME_LOCAL);*/
	ret |= ufshcd_dme_get_attr(hba, UIC_ARG_MIB(TC_CBC_REG_RD_MSB),
					&tmp_rd_msb, DME_LOCAL);

	if (!ret)
		*val = (tmp_rd_msb << 8) | tmp_rd_lsb;

	return ret;
}

/**
 * tc_dwc_c10_write()
 *
 * This function writes to the c10 interface in Synopsys G240 TC
 * @hba: Pointer to driver structure
 * @c10_reg: c10 register to write into
 * @c10_val: c10 value to be written
 *
 * Returns 0 on success or non-zero value on failure
 */
int tc_dwc_cr_write(struct ufs_hba *hba, u16 reg, u16 val)
{
	struct ufshcd_dme_attr_val data[] = {
		{ UIC_ARG_MIB(TC_CBC_REG_ADDR_LSB), REG_16_LSB(0),
			DME_LOCAL },
		{ UIC_ARG_MIB(TC_CBC_REG_ADDR_MSB), REG_16_MSB(0),
			DME_LOCAL },
		{ UIC_ARG_MIB(TC_CBC_REG_WR_LSB), REG_16_LSB(0), DME_LOCAL },
		{ UIC_ARG_MIB(TC_CBC_REG_WR_MSB), REG_16_MSB(0), DME_LOCAL },
		{ UIC_ARG_MIB(CBC_REG_RD_WR_SEL), CBC_REG_WRITE, DME_LOCAL },
		{ UIC_ARG_MIB(VS_MPHYCFGUPDT), 0x1, DME_LOCAL },
	};

	data[0].mib_val = REG_16_LSB(reg);
	data[1].mib_val = REG_16_MSB(reg);
	data[2].mib_val = REG_16_LSB(val);
	data[3].mib_val = REG_16_MSB(val);


	return ufshcd_dwc_dme_set_attrs(hba, data, ARRAY_SIZE(data));
}

static int sunxi_ufs_rmmi_config(struct ufs_hba *hba)
{
	u16 phy_val = 0;
	int ret = 0;
	int i = 0;
	u32 v = 0;
	u32 pll_rate_a = 0;
	u32 pll_rate_b = 0;
	u32 att_lane0 = 0;
	u32 ctle_lane0 = 0;
	u32 att_lane1 = 0;
	u32 ctle_lane1 = 0;

	struct ufshcd_dme_attr_val rmmi_config[] = {
		/*Configure CB attribute CBRATESEL (0x8114) with value 0x0 for Rate A and 0x1 for Rate B*/
		{ UIC_ARG_MIB(CBRATSEL), 0x1, DME_LOCAL },
		/*Because RMMI_CBREFCLKCTRL2 bit 4 defualt value is 1,
		 * here bit 4 set to 1 too
		 *according to SNPS ip
		 * */
		{ UIC_ARG_MIB(RMMI_CBREFCLKCTRL2), 0x90, DME_LOCAL },
		{ UIC_ARG_MIB_SEL(RMMI_RXSQCONTROL, SELIND_LN0_RX), 0x01,
					DME_LOCAL },
		{ UIC_ARG_MIB_SEL(RMMI_RXSQCONTROL, SELIND_LN1_RX), 0x01,
					DME_LOCAL },
		{ UIC_ARG_MIB_SEL(RMMI_RXRHOLDCTRLOPT, SELIND_LN0_RX), 0x02,
					DME_LOCAL },
		{ UIC_ARG_MIB_SEL(RMMI_RXRHOLDCTRLOPT, SELIND_LN1_RX), 0x02,
					DME_LOCAL },
#if 1
		{ UIC_ARG_MIB(EXT_COARSE_TUNE_RATEA), 0x2,
					DME_LOCAL },/*reset value 0x2*/
		{ UIC_ARG_MIB(EXT_COARSE_TUNE_RATEB), 0x80,
					DME_LOCAL },/*reset value 0x80*/
#endif
		{ UIC_ARG_MIB(RMMI_CBCRCTRL), 0x01, DME_LOCAL },
		{ UIC_ARG_MIB(VS_MPHYCFGUPDT), 0x01, DME_LOCAL },
	};

	static const struct ufshcd_dme_attr_val rmmi_config_1[] = {
		{ UIC_ARG_MIB(VS_MPHYCFGUPDT), 0x1, DME_LOCAL },
		};


	struct pair_addr phy_cr_data_coarse_tune[] = {
		{RAWAONLANEN_DIG_MPLLA_COARSE_TUNE, 0x0},/*reg default value**/
		{MPLL_SKIPCAL_COARSE_TUNE, 0x0},/*reg default value**/
	};


	static const struct pair_addr phy_cr_data_afe_flags[] = {
		{RAWAONLANEN_DIG_FAST_FLAGS0, 0x0004},
		{RAWAONLANEN_DIG_FAST_FLAGS1, 0x0004},
	};

	static const struct pair_addr phy_cr_data_pll_auto_cal[] = {
		//	{SUP_DIG_MPLLA_MPLL_PWR_CTL_CAL_CTRL, 0x0001},//PLL calibration 4.10.2 step1 for Rate A
		{MPLL_PWR_CTL_CAL_CTRL, 0x0008},//PLL calibration 4.10.2 step1 for Rate A
	};


	struct pair_addr phy_cr_data_afe_cal_words[] = {
		{RAWAONLANEN_DIG_AFE_ATT_IDAC_OFST0, 0x80},/*reg default value**/
		{RAWAONLANEN_DIG_AFE_ATT_IDAC_OFST1, 0x80},/*reg default value**/
		{RAWAONLANEN_DIG_AFE_CTLE_IDAC_OFST0, 0x80},/*reg default value**/
		{RAWAONLANEN_DIG_AFE_CTLE_IDAC_OFST1, 0x80},/*reg default value**/
		{RAWAONLANEN_DIG_RX_ADPT_DFE_TAP3_0, 0xa00},
		{RAWAONLANEN_DIG_RX_ADPT_DFE_TAP3_1, 0xa00},
	};

	ret = sunxi_ufs_get_cal_words(hba, &pll_rate_a, &pll_rate_b, \
										&att_lane0, &ctle_lane0, \
										&att_lane1, &ctle_lane1);
	if (ret)
		return ret;

	/*Force use auto pll cal,for the value in efuse will cause uic error*/
	//pll_rate_a = pll_rate_b = 0;

	/*update ext coarse tune rate from efuse**/
	if (pll_rate_a || pll_rate_b) {
		if ((rmmi_config[6].attr_sel != UIC_ARG_MIB(EXT_COARSE_TUNE_RATEA))
		   && (rmmi_config[7].attr_sel != UIC_ARG_MIB(EXT_COARSE_TUNE_RATEB))) {
				dev_err(hba->dev, "ext coarse tune arrary setting failed\n");
				return -1;
		   }
		if (pll_rate_a)
			rmmi_config[6].mib_val = pll_rate_a;
		if (pll_rate_b)
			rmmi_config[7].mib_val = pll_rate_b;
		dev_dbg(hba->dev, "update ext coarse tune rate ok,"
				"rate a(0x%08x) 0x%08x, rate b(0x%08x) 0x%08x\n",\
				rmmi_config[6].attr_sel, rmmi_config[6].mib_val,\
				rmmi_config[7].attr_sel, rmmi_config[7].mib_val);

	}


	if (!pll_rate_b && pll_rate_a) {
		rmmi_config[0].mib_val = 0;//rate a
		sunxi_ufs_host_priv.phy_hs_rate = PA_HS_MODE_A;
	} else {
		sunxi_ufs_host_priv.phy_hs_rate = PA_HS_MODE_B;
	}
/*
 *startup sequence_ufs_mphy_812a.docx step 8.b, step 9 to 10,13 to 14
 */

	ret = ufshcd_dwc_dme_set_attrs(hba, rmmi_config,
				       ARRAY_SIZE(rmmi_config));
	if (ret) {
		dev_err(hba->dev, "set rmmi_config failed\n");
		return ret;
	}

#ifdef PHY_DEBUG_DUMP
	ufshcd_dwc_dme_dump_attrs(hba, rmmi_config, ARRAY_SIZE(rmmi_config));
#endif


/*
 *15.De-assert.phy_reset
 */
	v = readl(SUNXI_UFS_BGR_REG);
	v |= SUNXI_UFS_PHY_RST_BIT;
	writel(v, SUNXI_UFS_BGR_REG);

	/**16.Wait for sram_init_done signal from MPHY.**/
	/*test_top.ufs_rtl.u_mphy_top.sram_init_done 1*/
	ret = ufshcd_wait_for_register(hba, REG_UFS_CFG, MPHY_SRAM_INIT_DONE_BIT, MPHY_SRAM_INIT_DONE_BIT, 10);
	if (ret) {
		sunxi_ufs_trace_point(SUNXI_UFS_RAMINIT_ERR);
		dev_err(hba->dev, "%s: wait sram init done timeout\n", __func__);
		return ret;
	}

	/*17.Access SRAM contents through indirect register access if needed.**/
	/*test sram*/
	ret = tc_dwc_cr_read(hba, RAWMEM_DIG_RAM_CMN0_B0_R0 + 2, &phy_val);
	if (ret) {
			dev_err(hba->dev,
				"Failed to read  RAWMEM_DIG_RAM_CMN0_B0_R0 + 2results\n");
			return ret;
	}

	ret = tc_dwc_cr_write(hba, RAWMEM_DIG_RAM_CMN0_B0_R0 + 2, phy_val);
	if (ret) {
			dev_err(hba->dev,
				"Failed to write  RAWMEM_DIG_RAM_CMN0_B0_R0 +2 results\n");
			return ret;
	}




/*
 *startup sequence_ufs_mphy_812a.docx step 18,19
 */
	/*update coarse tune rate from efuse**/
	if (pll_rate_b || pll_rate_a) {
		phy_cr_data_coarse_tune[0].value = pll_rate_b?pll_rate_b:pll_rate_a;
		phy_cr_data_coarse_tune[1].value = pll_rate_b?pll_rate_b:pll_rate_a;
		dev_dbg(hba->dev, "update coarse tune for rate b ok,"
				"addr 0x%08x value 0x%08x,"
				"addr 0x%08x, value 0x%08x\n",\
				phy_cr_data_coarse_tune[0].addr, phy_cr_data_coarse_tune[0].value, \
				phy_cr_data_coarse_tune[1].addr, phy_cr_data_coarse_tune[1].value);

		for (i = 0; i < ARRAY_SIZE(phy_cr_data_coarse_tune); i++) {
			const struct pair_addr *data = &phy_cr_data_coarse_tune[i];
			ret = tc_dwc_cr_write(hba, data->addr, data->value);
			if (ret) {
				dev_err(hba->dev, "%s: c10 write failed\n", __func__);
				return ret;
			}
		}
#ifdef PHY_DEBUG_DUMP
		for (i = 0; i < ARRAY_SIZE(phy_cr_data_coarse_tune); i++) {
			const struct pair_addr *data = &phy_cr_data_coarse_tune[i];
			u16 value = 0;
			ret = tc_dwc_cr_read(hba, data->addr, &value);
			if (ret) {
				dev_err(hba->dev, "%s: c10 write failed\n", __func__);
				return ret;
			}
			dev_err(hba->dev, "phy cr addr 0x%08x, value 0x%08x\n", data->addr, value);
		}
#endif
	}


/*
 *startup sequence_ufs_mphy_812a.docx step 20
 */
	for (i = 0; i < ARRAY_SIZE(phy_cr_data_afe_flags); i++) {
		const struct pair_addr *data = &phy_cr_data_afe_flags[i];
		ret = tc_dwc_cr_write(hba, data->addr, data->value);
		if (ret) {
			dev_err(hba->dev, "%s: c10 write failed\n", __func__);
			return ret;
		}
	}

	if (att_lane0 && ctle_lane0 && att_lane1 && ctle_lane1) {
		phy_cr_data_afe_cal_words[0].value = att_lane0;
		phy_cr_data_afe_cal_words[1].value = att_lane1;
		phy_cr_data_afe_cal_words[2].value = ctle_lane0;
		phy_cr_data_afe_cal_words[3].value = ctle_lane1;
		dev_dbg(hba->dev, "update pll cal words, att_lane0(0x%08x) 0x%x,att_lane1(0x%08x) 0x%08x,"
				"ctle_lane0(0x%08x) 0x%08x, ctle_lane1(0x%08x) 0x%08x\n", \
				phy_cr_data_afe_cal_words[0].addr, phy_cr_data_afe_cal_words[0].value,
				phy_cr_data_afe_cal_words[1].addr, phy_cr_data_afe_cal_words[1].value,\
				phy_cr_data_afe_cal_words[2].addr, phy_cr_data_afe_cal_words[2].value,\
				phy_cr_data_afe_cal_words[3].addr, phy_cr_data_afe_cal_words[3].value);

		/*
		*startup sequence_ufs_mphy_812a.docx step 21,22,23
		 */
		for (i = 0; i < ARRAY_SIZE(phy_cr_data_afe_cal_words); i++) {
			const struct pair_addr *data = &phy_cr_data_afe_cal_words[i];
			ret = tc_dwc_cr_write(hba, data->addr, data->value);
			if (ret) {
				dev_err(hba->dev, "%s: c10 write failed\n", __func__);
				return ret;
			}
		}

#ifdef PHY_DEBUG_DUMP
		for (i = 0; i < ARRAY_SIZE(phy_cr_data_afe_cal_words); i++) {
			const struct pair_addr *data = &phy_cr_data_afe_cal_words[i];
			u16 value = 0;
			ret = tc_dwc_cr_read(hba, data->addr, &value);
			if (ret) {
				dev_err(hba->dev, "%s: c10 read failed\n", __func__);
				return ret;
			}
			dev_err(hba->dev, "phy cr addr 0x%08x, value 0x%08x\n", data->addr, value);
		}

		for (i = 0; i < ARRAY_SIZE(phy_cr_data_pll_auto_cal); i++) {
			const struct pair_addr *data = &phy_cr_data_pll_auto_cal[i];
			u16 value = 0;
			ret = tc_dwc_cr_read(hba, data->addr, &value);
			if (ret) {
				dev_err(hba->dev, "%s: c10 read failed\n", __func__);
				return ret;
			}
			dev_err(hba->dev, "phy cr addr 0x%08x, value 0x%08x\n", data->addr, value);
		}

#endif

	}
	if (!pll_rate_b && !pll_rate_a) {
		/*use auto pll cal**/
		for (i = 0; i < ARRAY_SIZE(phy_cr_data_pll_auto_cal); i++) {
			const struct pair_addr *data = &phy_cr_data_pll_auto_cal[i];
			ret = tc_dwc_cr_write(hba, data->addr, data->value);
			if (ret) {
				dev_err(hba->dev, "%s: c10 write failed\n", __func__);
				return ret;
			}
		}
		dev_info(hba->dev, "auto pll cal\n");

#ifdef PHY_DEBUG_DUMP
		for (i = 0; i < ARRAY_SIZE(phy_cr_data_pll_auto_cal); i++) {
			const struct pair_addr *data = &phy_cr_data_pll_auto_cal[i];
			u16 value = 0;
			ret = tc_dwc_cr_read(hba, data->addr, &value);
			if (ret) {
				dev_err(hba->dev, "%s: c10 read failed\n", __func__);
				return ret;
			}
			dev_err(hba->dev, "phy cr addr 0x%08x, value 0x%08x\n", data->addr, value);
		}
#endif
	}



	/**24.Trigger FW execution start according to FW usage scenario, for detailed
	 * information see the (PHY databook) ¡°Firmware Usage Scenario¡±**/
	/*force test_top.ufs_rtl.u_mphy_top.sram_ext_ld_done*/
	ufshcd_writel(hba, ufshcd_readl(hba, REG_UFS_CFG) | MPHY_SRAM_EXT_LD_DONE_BIT, REG_UFS_CFG);

/*
 * 25.Configure the VS_MphyCfgUpdt (0xD085) attribute to 1. The Write access
 * to this attribute triggers a configuration update of all connected TX
 * and RX M-PHY modules. It returns 0 upon read.
 *
*/
	ret = ufshcd_dwc_dme_set_attrs(hba, rmmi_config_1,
				       ARRAY_SIZE(rmmi_config_1));
	if (ret) {
		dev_err(hba->dev, "set rmmi_config1 failed\n");
		return ret;
	}

	return ret;
}


static int sunxi_ufs_phy_config(struct ufs_hba *hba)
{
	int ret = 0;
#if 0
	/*default value is 1,no need to set ,here only for safe*/
	ret = ufshcd_dme_set(hba, UIC_ARG_MIB(VS_MPHY_DISABLE), 0x01);
	if (ret) {
		dev_err(hba->dev, "VS_MPHY_ENABLE failed\n");
		return ret;
	}
#endif
	ret = sunxi_ufs_rmmi_config(hba);
	if (ret) {
		dev_err(hba->dev, "Failed to config rmmi");
		return ret;
	}

/*
 *26.Configure VS_mphy_disable(0xD0C1) with the value 0x0
 *
 */
	ret = ufshcd_dme_set(hba, UIC_ARG_MIB(VS_MPHY_DISABLE), 0x00);
	if (ret) {
		dev_err(hba->dev, "VS_MPHY_DISABLE failed\n");
		return ret;
	}

/*
 *28.Configure VS_DebugSaveConfigTime(0xD0A0). Set 0xD0A0[1:0] = 0x3.
 *29.Configure VS_ClkMuxSwitchingTimer (0xD0FB) with the value 0xA.
 */
	ret = ufshcd_dme_set(hba, UIC_ARG_MIB(VS_DEBUG_SAVE_CONFIG_TIME), 0x1b);
	if (ret) {
		dev_err(hba->dev, "VS_DEBUG_SAVE_CONFIG_TIME failed\n");
		return ret;
	} else {
		dev_dbg(hba->dev, "%s:VS_DEBUG_SAVE_CONFIG_TIME 0x1b\n", __FUNCTION__);
	}

	ret = ufshcd_dme_set(hba, UIC_ARG_MIB(VS_CLK_MUX_SWITCHING_TIMER), 0xa);
	if (ret) {
		dev_err(hba->dev, "VS_CLK_MUX_SWITCHING_TIMER failed\n");
		return ret;
	}

	return ret;
}

/**
 * ufshcd_dwc_connection_setup()
 * This function configures both the local side (host) and the peer side
 * (device) unipro attributes to establish the connection to application/
 * cport.
 * This function is not required if the hardware is properly configured to
 * have this connection setup on reset. But invoking this function does no
 * harm and should be fine even working with any ufs device.
 *
 * @hba: pointer to drivers private data
 *
 * Returns 0 on success non-zero value on failure
 */
static int ufshcd_dwc_connection_setup(struct ufs_hba *hba)
{
	int ret = 0;
	u32 val = 0;
	static const struct ufshcd_dme_attr_val setup_attrs[] = {
		{ UIC_ARG_MIB(T_CONNECTIONSTATE), 0, DME_LOCAL },
/*
		{ UIC_ARG_MIB(N_DEVICEID), 0, DME_LOCAL },
		{ UIC_ARG_MIB(N_DEVICEID_VALID), 0, DME_LOCAL },
		{ UIC_ARG_MIB(T_PEERDEVICEID), 1, DME_LOCAL },
		{ UIC_ARG_MIB(T_PEERCPORTID), 0, DME_LOCAL },
		{ UIC_ARG_MIB(T_TRAFFICCLASS), 0, DME_LOCAL },
*/
		{ UIC_ARG_MIB(T_CPORTFLAGS), 0x6, DME_LOCAL },
/*
		{ UIC_ARG_MIB(T_CPORTMODE), 1, DME_LOCAL },
*/
		{ UIC_ARG_MIB(T_CONNECTIONSTATE), 1, DME_LOCAL },
/*
		{ UIC_ARG_MIB(T_CONNECTIONSTATE), 0, DME_PEER },
		{ UIC_ARG_MIB(N_DEVICEID), 1, DME_PEER },
		{ UIC_ARG_MIB(N_DEVICEID_VALID), 1, DME_PEER },
		{ UIC_ARG_MIB(T_PEERDEVICEID), 1, DME_PEER },
		{ UIC_ARG_MIB(T_PEERCPORTID), 0, DME_PEER },
		{ UIC_ARG_MIB(T_TRAFFICCLASS), 0, DME_PEER },
		{ UIC_ARG_MIB(T_CPORTFLAGS), 0x6, DME_PEER },
		{ UIC_ARG_MIB(T_CPORTMODE), 1, DME_PEER },
		{ UIC_ARG_MIB(T_CONNECTIONSTATE), 1, DME_PEER }
*/
	};

	ret = ufshcd_dwc_dme_set_attrs(hba, setup_attrs, ARRAY_SIZE(setup_attrs));
	if (ret) {
		dev_err(hba->dev, "connection set up failed\n");
		return ret;
	}

	ret = ufshcd_dme_get_attr(hba, UIC_ARG_MIB(T_CONNECTIONSTATE),
					&val, DME_LOCAL);
	if (ret) {
		dev_err(hba->dev, "read  T_CONNECTIONSTATE)failed\n");
		return ret;
	}
	if (val != 1) {
		dev_err(hba->dev, "read  T_CONNECTIONSTATE not 1,unipro init failed\n");
		return ret;
	}

	return ret;
}



static int sunxi_ufs_link_startup_notify(struct ufs_hba *hba,
					enum ufs_notify_change_status status)
{
	int err = 0;
	switch (status) {
	case PRE_CHANGE:
		if (hba->ops->phy_initialization) {
			err = hba->ops->phy_initialization(hba);
			if (err) {
				dev_err(hba->dev, "Phy setup failed (%d)\n",
									err);
				goto out;
			}
		}
#if 0
		if (hba->hwv_params.tc_rule == 2) {
			/* Necessary for HSG4 FPGA environment */
			err = ufshcd_dme_set_attr(hba,
						UIC_ARG_MIB(DL_AFC0CREDITTHRESHOLD),
						ATTR_SET_NOR, 0x6a, DME_LOCAL);
		}
#endif
		break;
	case POST_CHANGE:
		err = ufshcd_dwc_link_is_up(hba);
		if (err) {
			dev_err(hba->dev, "Link is not up\n");
			sunxi_ufs_trace_point(SUNXI_UFS_LINK_NOT_UP);
			goto out;
		}

		err = ufshcd_dwc_connection_setup(hba);
		if (err)
			dev_err(hba->dev, "Connection setup failed (%d)\n",
									err);
		break;
	}

out:
	return err;
}

/**
 * ufshcd_get_pwr_dev_param - get finally agreed attributes for
 *                            power mode change
 * @pltfrm_param: pointer to platform parameters
 * @dev_max: pointer to device attributes
 * @agreed_pwr: returned agreed attributes
 *
 * Return: 0 on success, non-zero value on failure.
 */
int ufshcd_get_pwr_dev_param(const struct ufs_dev_params *pltfrm_param,
			     const struct ufs_pa_layer_attr *dev_max,
			     struct ufs_pa_layer_attr *agreed_pwr)
{
	int min_pltfrm_gear;
	int min_dev_gear;
	bool is_dev_sup_hs = false;
	bool is_pltfrm_max_hs = false;

	if (dev_max->pwr_rx == FAST_MODE)
		is_dev_sup_hs = true;

	if (pltfrm_param->desired_working_mode == UFS_HS_MODE) {
		is_pltfrm_max_hs = true;
		min_pltfrm_gear = min_t(u32, pltfrm_param->hs_rx_gear,
					pltfrm_param->hs_tx_gear);
	} else {
		min_pltfrm_gear = min_t(u32, pltfrm_param->pwm_rx_gear,
					pltfrm_param->pwm_tx_gear);
	}

	/*
	 * device doesn't support HS but
	 * pltfrm_param->desired_working_mode is HS,
	 * thus device and pltfrm_param don't agree
	 */
	if (!is_dev_sup_hs && is_pltfrm_max_hs) {
		pr_info("%s: device doesn't support HS\n",
			__func__);
		return -ENOTSUPP;
	} else if (is_dev_sup_hs && is_pltfrm_max_hs) {
		/*
		 * since device supports HS, it supports FAST_MODE.
		 * since pltfrm_param->desired_working_mode is also HS
		 * then final decision (FAST/FASTAUTO) is done according
		 * to pltfrm_params as it is the restricting factor
		 */
		agreed_pwr->pwr_rx = pltfrm_param->rx_pwr_hs;
		agreed_pwr->pwr_tx = agreed_pwr->pwr_rx;
	} else {
		/*
		 * here pltfrm_param->desired_working_mode is PWM.
		 * it doesn't matter whether device supports HS or PWM,
		 * in both cases pltfrm_param->desired_working_mode will
		 * determine the mode
		 */
		agreed_pwr->pwr_rx = pltfrm_param->rx_pwr_pwm;
		agreed_pwr->pwr_tx = agreed_pwr->pwr_rx;
	}

	/*
	 * we would like tx to work in the minimum number of lanes
	 * between device capability and vendor preferences.
	 * the same decision will be made for rx
	 */
	agreed_pwr->lane_tx = min_t(u32, dev_max->lane_tx,
				    pltfrm_param->tx_lanes);
	agreed_pwr->lane_rx = min_t(u32, dev_max->lane_rx,
				    pltfrm_param->rx_lanes);

	/* device maximum gear is the minimum between device rx and tx gears */
	min_dev_gear = min_t(u32, dev_max->gear_rx, dev_max->gear_tx);

	/*
	 * if both device capabilities and vendor pre-defined preferences are
	 * both HS or both PWM then set the minimum gear to be the chosen
	 * working gear.
	 * if one is PWM and one is HS then the one that is PWM get to decide
	 * what is the gear, as it is the one that also decided previously what
	 * pwr the device will be configured to.
	 */
	if ((is_dev_sup_hs && is_pltfrm_max_hs) ||
	    (!is_dev_sup_hs && !is_pltfrm_max_hs)) {
		agreed_pwr->gear_rx =
			min_t(u32, min_dev_gear, min_pltfrm_gear);
	} else if (!is_dev_sup_hs) {
		agreed_pwr->gear_rx = min_dev_gear;
	} else {
		agreed_pwr->gear_rx = min_pltfrm_gear;
	}
	agreed_pwr->gear_tx = agreed_pwr->gear_rx;

	agreed_pwr->hs_rate = pltfrm_param->hs_rate;

	return 0;
}

void ufshcd_init_pwr_dev_param(struct ufs_dev_params *dev_param)
{
	*dev_param = (struct ufs_dev_params){
		.tx_lanes = UFS_LANE_2,
		.rx_lanes = UFS_LANE_2,
		.hs_rx_gear = UFS_HS_G3,
		.hs_tx_gear = UFS_HS_G3,
		.pwm_rx_gear = UFS_PWM_G4,
		.pwm_tx_gear = UFS_PWM_G4,
		.rx_pwr_pwm = SLOW_MODE,
		.tx_pwr_pwm = SLOW_MODE,
		.rx_pwr_hs = FAST_MODE,
		.tx_pwr_hs = FAST_MODE,
		.hs_rate = PA_HS_MODE_B,
		.desired_working_mode = UFS_HS_MODE,
	};
}


static int sunxi_ufs_pre_pwr_change(struct ufs_hba *hba,
				  struct ufs_pa_layer_attr *dev_max_params,
				  struct ufs_pa_layer_attr *dev_req_params)
{
	struct ufs_dev_params host_cap;
	int ret;

	ufshcd_init_pwr_dev_param(&host_cap);
	host_cap.hs_rx_gear = UFS_HS_G4;
	host_cap.hs_tx_gear = UFS_HS_G4;
	host_cap.hs_rate = sunxi_ufs_host_priv.phy_hs_rate ;


	ret = ufshcd_get_pwr_dev_param(&host_cap,
				       dev_max_params,
				       dev_req_params);
	if (ret) {
		pr_info("%s: failed to determine capabilities\n",
			__func__);
		goto out;
	}


	ret = ufshcd_dme_configure_adapt(hba,
					   dev_req_params->gear_tx,
					   PA_INITIAL_ADAPT);
	if (ret) {
		pr_err("%s: failed to set PA INIT ADAPT\n",
			__func__);
	}
out:
	return ret;
}



static int sunxi_ufs_hce_enable_notify(struct ufs_hba *hba,
				enum ufs_notify_change_status status)
{
	switch (status) {
	case PRE_CHANGE:
		;
	case POST_CHANGE:
		;
	}

	return 0;
}

static void sunxi_ufs_cfg_clk_init(void)
{
	u32 v = readl(SUNXI_UFS_CFG_CLK_REG);
	v &= ~SUNXI_UFS_CFG_CLK_GATING_BIT;
	writel(v, SUNXI_UFS_CFG_CLK_REG);

	v = readl(SUNXI_UFS_CFG_CLK_REG);
	v &= ~SUNXI_UFS_CFG_FACTOR_M_MASK;
	/*use defult value,that is cfg clk is19.2M*/
	v |= 0x18;
	writel(v, SUNXI_UFS_CFG_CLK_REG);
	udelay(10);

	v = readl(SUNXI_UFS_CFG_CLK_REG);
	v &= ~SUNXI_UFS_CFG_CLK_SRC_SEL_MASK;
	v |= SUNXI_UFS_CFG_SRC_PERI0_480M;
	writel(v, SUNXI_UFS_CFG_CLK_REG);
	udelay(10);

	v = readl(SUNXI_UFS_CFG_CLK_REG);
	v |= SUNXI_UFS_CFG_CLK_GATING_BIT;
	writel(v, SUNXI_UFS_CFG_CLK_REG);
	/*only for safe*/
	udelay(10);
	dev_dbg(hba->dev, "SUNXI_UFS_CFG_CLK_REG %x\n", readl(SUNXI_UFS_CFG_CLK_REG));
}
/*for cfg clk use default value,so it deinit is the same with init*/
static void sunxi_ufs_cfg_clk_deinit(void)
{
	u32 v = readl(SUNXI_UFS_CFG_CLK_REG);
	v &= ~SUNXI_UFS_CFG_CLK_GATING_BIT;
	writel(v, SUNXI_UFS_CFG_CLK_REG);

	v = readl(SUNXI_UFS_CFG_CLK_REG);
	v &= ~SUNXI_UFS_CFG_FACTOR_M_MASK;
	/*use defult value,that is cfg clk is19.2M*/
	v |= 0x18;
	writel(v, SUNXI_UFS_CFG_CLK_REG);
	udelay(10);

	v = readl(SUNXI_UFS_CFG_CLK_REG);
	v &= ~SUNXI_UFS_CFG_CLK_SRC_SEL_MASK;
	v |= SUNXI_UFS_CFG_SRC_PERI0_480M;
	writel(v, SUNXI_UFS_CFG_CLK_REG);
	udelay(10);

	v = readl(SUNXI_UFS_CFG_CLK_REG);
	v |= SUNXI_UFS_CFG_CLK_GATING_BIT;
	writel(v, SUNXI_UFS_CFG_CLK_REG);
	/*only for safe*/
	udelay(10);
	dev_dbg(hba->dev, "SUNXI_UFS_CFG_CLK_REG %x\n", readl(SUNXI_UFS_CFG_CLK_REG));
}



static void sunxi_ufs_sys_ref_clk_init(u32 *ref_val)
{
	u32 reg_val = 0;
//	int ret = 0;
	u32 ref_ext = 0;
	u32 ref_cfg_val = 0;

	get_hosc_type();
	switch (get_hosc_type_ufs()) {
	case SUNXI_UFS_EXT_REF_26M:
		ref_cfg_val |= REF_CLK_UNIPRO_SEL_BIT | REF_CLK_APP_SEL_BIT | REF_CLK_FREQ_SEL_26M;
		ref_ext = 1;
		sunxi_ufs_trace_point(SUNXI_UFS_EXT_REF_26M);
		break;
	case SUNXI_UFS_INT_REF_19_2M:
		ref_cfg_val |= REF_CLK_FREQ_SEL_19_2M;
		ref_ext  = 0;
		sunxi_ufs_trace_point(SUNXI_UFS_INT_REF_19_2M);
		break;
	case SUNXI_UFS_INT_REF_26M:
		ref_cfg_val |= REF_CLK_FREQ_SEL_26M;
		ref_ext = 0;
		sunxi_ufs_trace_point(SUNXI_UFS_INT_REF_26M);
		break;
	case SUNXI_UFS_EXT_REF_19_2M:
		ref_cfg_val |= REF_CLK_UNIPRO_SEL_BIT | REF_CLK_APP_SEL_BIT | REF_CLK_FREQ_SEL_19_2M;
		ref_ext = 1;
		sunxi_ufs_trace_point(SUNXI_UFS_EXT_REF_19_2M);
		break;
	default:
		ref_cfg_val |= REF_CLK_FREQ_SEL_26M;
		ref_ext = 0;
		sunxi_ufs_trace_point(SUNXI_UFS_INT_REF_26M);
		break;
	}

	/*dcxo ufs gating should set according to ref clock*/
	reg_val = readl(SUNXI_XO_CTRL_WP_REG) & (~0xffff);
	writel(reg_val | 0x16aa, SUNXI_XO_CTRL_WP_REG);
	dev_dbg(hba->dev, "XO_CTRL_WP %x\n", readl(SUNXI_XO_CTRL_WP_REG));

	if (ref_ext == 1) {
		/*disable refclk_out to ufs device*/
		writel(readl(SUNXI_XO_CTRL_REG) | SUNXI_CLK_REQ_EN, SUNXI_XO_CTRL_REG);

		reg_val = readl(SUNXI_XO_CTRL1_REG);
		reg_val &= ~SUNXI_DCXO_UFS_GATING;
		writel(reg_val, SUNXI_XO_CTRL1_REG);

	} else {
		/*enable ref clk to host controller*/
		reg_val = readl(SUNXI_XO_CTRL1_REG);
		reg_val |= SUNXI_DCXO_UFS_GATING;
		writel(reg_val, SUNXI_XO_CTRL1_REG);

		/*enable refclk_out to ufs device*/
		writel(readl(SUNXI_XO_CTRL_REG) & (~SUNXI_CLK_REQ_EN), SUNXI_XO_CTRL_REG);

	}
	dev_dbg(hba->dev, "XO_CTRL %x\n", readl(SUNXI_XO_CTRL_REG));
	dev_dbg(hba->dev, "XO_CTRL1 %x\n", readl(SUNXI_XO_CTRL1_REG));
	dev_dbg(hba->dev, "XO_CTRL_WP %x\n", readl(SUNXI_XO_CTRL_WP_REG));
	//dev_info(hba->dev,"U3U2_PHY_MUX_CTRL %x\n", readl(SUNXI_U3U2_PHY_MUX_CTRL_REG));
//	dev_info(hba->dev,"SUNXI_UFS_REF_CLK_EN_REG %x\n", readl(SUNXI_UFS_REF_CLK_EN_REG));
	*ref_val =  ref_cfg_val;
}

static void sunxi_ufs_sys_ref_clk_deinit(void)
{
	u32 reg_val = readl(SUNXI_XO_CTRL_WP_REG) & (~0xffff);
	writel(reg_val | 0x16aa, SUNXI_XO_CTRL_WP_REG);
	/*set refclk_out to default*/
	writel(readl(SUNXI_XO_CTRL_REG) | SUNXI_CLK_REQ_EN, SUNXI_XO_CTRL_REG);

	/*set dcxo ufs gating to default value*/
	reg_val = readl(SUNXI_XO_CTRL1_REG);
	reg_val &= ~SUNXI_DCXO_UFS_GATING;
	writel(reg_val, SUNXI_XO_CTRL1_REG);
}

static void sunxi_ufs_axi_onoff(u32 on)
{
	u32 reg_val = 0;

	if (!on) {
		reg_val = readl(SUNXI_UFS_AXI_CLK_REG);
		reg_val &= ~SUNXI_UFS_AXI_CLK_GATING_BIT;
		writel(reg_val, SUNXI_UFS_AXI_CLK_REG);

		reg_val = readl(SUNXI_UFS_BGR_REG);
		reg_val &= ~SUNXI_UFS_AXI_RST_BIT;
		writel(reg_val, SUNXI_UFS_BGR_REG);

		reg_val = readl(SUNXI_UFS_AXI_CLK_REG);
		reg_val &= ~SUNXI_UFS_AXI_FACTOR_M_MASK;
		writel(reg_val, SUNXI_UFS_AXI_CLK_REG);
		udelay(10);

		reg_val = readl(SUNXI_UFS_AXI_CLK_REG);
		reg_val &= ~SUNXI_UFS_CLK_SRC_SEL_MASK;
		reg_val |= SUNXI_UFS_SRC_PERI0_300M;
		writel(reg_val, SUNXI_UFS_AXI_CLK_REG);
		udelay(10);
	} else  {
		/*close clk gate before change clock according to ccmu spec
		 *although we will call sunxi_ufs_axi_onoff(0) before sunxi_ufs_axi_onoff(1)
		 *here close clk gate for safe.
		 * */
		reg_val = readl(SUNXI_UFS_AXI_CLK_REG);
		reg_val &= ~SUNXI_UFS_AXI_CLK_GATING_BIT;
		writel(reg_val, SUNXI_UFS_AXI_CLK_REG);

		reg_val = readl(SUNXI_UFS_AXI_CLK_REG);
		reg_val &= ~SUNXI_UFS_CLK_SRC_SEL_MASK;
		reg_val |= SUNXI_UFS_SRC_PERI0_200M;
		writel(reg_val, SUNXI_UFS_AXI_CLK_REG);
		udelay(10);

		reg_val = readl(SUNXI_UFS_AXI_CLK_REG);
		reg_val &= ~SUNXI_UFS_AXI_FACTOR_M_MASK;
		writel(reg_val, SUNXI_UFS_AXI_CLK_REG);
		udelay(10);

		reg_val = readl(SUNXI_UFS_BGR_REG);
		reg_val |= SUNXI_UFS_AXI_RST_BIT;
		writel(reg_val, SUNXI_UFS_BGR_REG);

		reg_val = readl(SUNXI_UFS_AXI_CLK_REG);
		reg_val |= SUNXI_UFS_AXI_CLK_GATING_BIT;
		writel(reg_val, SUNXI_UFS_AXI_CLK_REG);
	}
}

static void sunxi_ufs_ahb_onoff(u32 on)
{
	u32 reg_val = 0;
	/******ahb*******/
	if (!on) {
		reg_val = readl(SUNXI_UFS_BGR_REG);
		reg_val &= ~SUNXI_UFS_GATING_BIT;
		writel(reg_val, SUNXI_UFS_BGR_REG);

		reg_val = readl(SUNXI_UFS_BGR_REG);
		reg_val &= ~SUNXI_UFS_RST_BIT;
		writel(reg_val, SUNXI_UFS_BGR_REG);
	} else {
		reg_val = readl(SUNXI_UFS_BGR_REG);
		reg_val |= SUNXI_UFS_RST_BIT;
		writel(reg_val, SUNXI_UFS_BGR_REG);

		reg_val = readl(SUNXI_UFS_BGR_REG);
		reg_val |= SUNXI_UFS_GATING_BIT;
		writel(reg_val, SUNXI_UFS_BGR_REG);
	}
}


static int sunxi_ufs_host_init(struct ufs_hba *hba)
{
	u32 reg_val = 0;
//	int ret = 0;
	u32 ref_cfg_val = 0;

	sunxi_soc_ext_res();
	sunxi_ufs_ahb_onoff(0);
	sunxi_ufs_axi_onoff(0);
	sunxi_ufs_axi_onoff(1);
	sunxi_ufs_ahb_onoff(1);

/*
 * 1.Issue Power on Reset(assert Reset_n and phy_reset)
 */
	reg_val = readl(SUNXI_UFS_BGR_REG);
	reg_val &= ~(SUNXI_UFS_CORE_RST_BIT | SUNXI_UFS_PHY_RST_BIT);
	writel(reg_val, SUNXI_UFS_BGR_REG);

/*
*a.Set sram_bypass/sram_ext_ld_done according to FW usage scenario,
*for detailed information see the (PHY databook) ¡ÈFirmware Usage Scenario¡É
* here brom use sram mode
*/
	reg_val = ufshcd_readl(hba, REG_UFS_CFG);
	reg_val &= ~(MPHY_SRAM_BYPASS_BIT | MPHY_SRAM_EXT_LD_DONE_BIT);
	ufshcd_writel(hba, reg_val, REG_UFS_CFG);
/*
*b.Provide stable config clock and reference clock (required in host mode, optional in device mode)
*to MPHY before reset release. See ¡ÈDevice mode support¡É of RMMI databook for reference
*clock requirement in device mode
*/
	sunxi_ufs_cfg_clk_init();
	sunxi_ufs_sys_ref_clk_init(&ref_cfg_val);

	/*open cfg clk gate in host, and open clk24m too*/
	reg_val = ufshcd_readl(hba, REG_UFS_CLK_GATE);
	reg_val &= ~(MPHY_CFGCLK_GATE_BIT | CLK24M_GATE_BIT);
	ufshcd_writel(hba, reg_val, REG_UFS_CLK_GATE);

/*
 *c.Set the cfg_clock_freq port according to the cfg_clock frequency
 */
	reg_val = ufshcd_readl(hba, REG_UFS_CFG);
	reg_val &=  ~(CFG_CLK_FREQ_MASK);
	reg_val |= CFG_CLK_19_2M;
	ufshcd_writel(hba, reg_val, REG_UFS_CFG);

/*
 *d.Set the ref_clock_sel port value according to the ref_clock selection
 */
	reg_val = ufshcd_readl(hba, REG_UFS_CFG);
	reg_val &=  ~(REF_CLK_FREQ_SEL_MASK);
	reg_val |= ref_cfg_val & REF_CLK_FREQ_SEL_MASK;
	ufshcd_writel(hba, reg_val, REG_UFS_CFG);

	hba->dev_ref_clk_freq = ref_cfg_val & REF_CLK_FREQ_SEL_MASK;
/*
 *e.For host mode, set ref_clk_en_unipro/ref_clk_en_app at top level input ports.
 *Application needs to set ref_clk_en_app to expected value.
 *For more details, please refer to section ¡ÈSetting up M-PHY for Device mode vs Host mode¡É of RMMI databook.
 */

	if (efuse_cfg_ufs_use_ccu_en_app()) {
		u32 rval = ufshcd_readl(hba, REG_UFS_CFG);
		rval &= ~SUNXI_UFS_REF_CLK_EN_APP_EN;
		ufshcd_writel(hba, rval, REG_UFS_CFG);
		if (ref_cfg_val & REF_CLK_APP_SEL_BIT) {
			rval = readl(SUNXI_UFS_REF_CLK_EN_REG);
			rval |= SUNXI_UFS_CCU_REF_CLK_EN_APP_BIT;
			writel(rval, SUNXI_UFS_REF_CLK_EN_REG);
		} else {
			rval = readl(SUNXI_UFS_REF_CLK_EN_REG);
			rval &= ~SUNXI_UFS_CCU_REF_CLK_EN_APP_BIT;
			writel(rval, SUNXI_UFS_REF_CLK_EN_REG);
		}
	} else {
		u32 rval = ufshcd_readl(hba, REG_UFS_CFG);
		rval |= SUNXI_UFS_REF_CLK_EN_APP_EN;
		ufshcd_writel(hba, rval, REG_UFS_CFG);
	}

	reg_val = ufshcd_readl(hba, REG_UFS_CFG);
	reg_val &= ~(REF_CLK_UNIPRO_SEL_BIT | REF_CLK_APP_SEL_BIT);
	reg_val |= ref_cfg_val & (REF_CLK_UNIPRO_SEL_BIT | REF_CLK_APP_SEL_BIT) ;
	ufshcd_writel(hba, reg_val, REG_UFS_CFG);

#if 0 /*ic deisigned as power on defautl, so no need to power on again*/
	/*******power on,must power on before operation other register*******/
	reg_val = ufshcd_readl(hba, REG_UFS_PD_PSW_DLY) & (~PD_PSWON_EN);
	ufshcd_writel(hba, reg_val, REG_UFS_PD_PSW_DLY);
	reg_val = ufshcd_readl(hba, REG_UFS_PD_PSW_DLY);
	reg_val &= ~PD_PSWON_DLY_MASK;
	reg_val |= 0xf;
	ufshcd_writel(hba, reg_val, REG_UFS_PD_PSW_DLY);

	reg_val = ufshcd_readl(hba, REG_UFS_PD_CTRL);
	reg_val &= ~PD_POLICY_MASK;
	reg_val |= 0x1;
	ufshcd_writel(hba, reg_val, REG_UFS_PD_CTRL);

	ret = ufshcd_wait_for_register(hba, REG_UFS_PD_STAT, \
			TRANS_CPT, TRANS_CPT, 20);
	if (ret) {
		dev_err(hba->dev, "%s: wait powr on timeout\n", __func__);
		return ret;
	}
	reg_val = ufshcd_readl(hba, REG_UFS_PD_STAT);
	ufshcd_writel(hba, reg_val, REG_UFS_PD_STAT);
#endif

	/* Ufsch psw power down by defautl.
	 * If power on is not finish,We can't give clock to psw
	 * So disable auto gate after powe on,not before
	 * */
	ufshcd_writel(hba, ufshcd_readl(hba, REG_UFS_CLK_GATE) | 0x800003FF, REG_UFS_CLK_GATE);
	dev_dbg(hba->dev, "REG_UFS_CFG %x\n", ufshcd_readl(hba, REG_UFS_CFG));
	dev_dbg(hba->dev, " REG_UFS_CLK_GATE %x\n", ufshcd_readl(hba,  REG_UFS_CLK_GATE));

/*
 * 3.De-assert Reset_n
 */
	reg_val = readl(SUNXI_UFS_BGR_REG);
	reg_val |= SUNXI_UFS_CORE_RST_BIT;
	writel(reg_val, SUNXI_UFS_BGR_REG);

	return 0;
}

void sunxi_ufs_host_exit(struct ufs_hba *hba)
{
	u32 reg_val = 0;
//	int ret = 0;

#if 0 /*ic deisigned as power on defautl, so no need to power on again, no need power off*/
	/*power off*/
	reg_val = ufshcd_readl(hba, REG_UFS_PD_CTRL);
	reg_val &= ~PD_POLICY_MASK;
	reg_val |= 0x2;
	ufshcd_writel(hba, reg_val, REG_UFS_PD_CTRL);

	ret = ufshcd_wait_for_register(hba, REG_UFS_PD_STAT, \
				TRANS_CPT, TRANS_CPT, 20);
	if (ret) {
		dev_err(hba->dev, "%s: wait powr off timeout\n", __func__);
		//return ret;
	}
	reg_val = ufshcd_readl(hba, REG_UFS_PD_STAT);
	ufshcd_writel(hba, reg_val, REG_UFS_PD_STAT);
#endif

	reg_val = readl(SUNXI_UFS_BGR_REG);
	reg_val &= ~SUNXI_UFS_PHY_RST_BIT;
	writel(reg_val, SUNXI_UFS_BGR_REG);

	reg_val = readl(SUNXI_UFS_BGR_REG);
	reg_val &= ~SUNXI_UFS_CORE_RST_BIT;
	writel(reg_val, SUNXI_UFS_BGR_REG);

	if (efuse_cfg_ufs_use_ccu_en_app()) {
		reg_val = readl(SUNXI_UFS_REF_CLK_EN_REG);
		reg_val &= ~SUNXI_UFS_CCU_REF_CLK_EN_APP_BIT;
		writel(reg_val, SUNXI_UFS_REF_CLK_EN_REG);
	}

	sunxi_ufs_sys_ref_clk_deinit();
	sunxi_ufs_cfg_clk_deinit();
	sunxi_ufs_ahb_onoff(0);
	sunxi_ufs_axi_onoff(0);
}

static int sunxi_ufs_pwr_change_notify(struct ufs_hba *hba,
				     enum ufs_notify_change_status stage,
				     struct ufs_pa_layer_attr *dev_max_params,
				     struct ufs_pa_layer_attr *dev_req_params)
{
	int ret = 0;

	switch (stage) {
	case PRE_CHANGE:
		ret = sunxi_ufs_pre_pwr_change(hba, dev_max_params,
					     dev_req_params);
		break;
	case POST_CHANGE:
		break;
	default:
		ret = -EINVAL;
		break;
	}

	return ret;
}

static void sunxi_ufs_device_reset(struct ufs_hba *hba)
{
    u32 rv = 0;
	//ufs_mtk_device_reset_ctrl(0, res);
	rv = ufshcd_readl(hba, REG_UFS_CFG);
	rv &= ~HW_RST_N;
	ufshcd_writel(hba, rv, REG_UFS_CFG);
    /*
	 ** The reset signal is active low. UFS devices shall detect
	 ** more than or equal to 1us of positive or negative RST_n
	 ** pulse width.
	 **
	 ** To be on safe side, keep the reset low for at least 10us.
	 * */
	udelay(15);

	rv |= HW_RST_N;
	ufshcd_writel(hba, rv, REG_UFS_CFG);
    //ufs_mtk_device_reset_ctrl(1, res);

    /* Some devices may need time to respond to rst_n */
	udelay(15000);
//	dev_dbg(hba->dev,"dev rst\n");

}

//static struct ufs_hba_ops sunxi_pltfm_hba_ops;

void ufs_hda_ops_bind(struct udevice *dev)
{
	memset(&dev->sunxi_pltfm_hba_ops, 0, sizeof(struct ufs_hba_ops));
	dev->sunxi_pltfm_hba_ops.init = sunxi_ufs_host_init;
	dev->sunxi_pltfm_hba_ops.exit = sunxi_ufs_host_exit;
	dev->sunxi_pltfm_hba_ops.hce_enable_notify = sunxi_ufs_hce_enable_notify;
	dev->sunxi_pltfm_hba_ops.link_startup_notify = sunxi_ufs_link_startup_notify;
	dev->sunxi_pltfm_hba_ops.pwr_change_notify = sunxi_ufs_pwr_change_notify;
	dev->sunxi_pltfm_hba_ops.phy_initialization = sunxi_ufs_phy_config;
	dev->sunxi_pltfm_hba_ops.device_reset = sunxi_ufs_device_reset;
}

void ufs_hda_ops_unbind(struct udevice *dev)
{
	memset(&dev->sunxi_pltfm_hba_ops, 0, sizeof(struct ufs_hba_ops));
}


static int sunxi_ufs_pltfm_probe(struct udevice *dev)
{
	int err = 0;
	err = ufshcd_probe(dev, &dev->sunxi_pltfm_hba_ops);
	//if (err)
	//	dev_err(dev, "ufshcd_probe() failed %d\n", err);

	return err;
}

/*/
static int cdns_ufs_pltfm_bind(struct udevice *dev)
{
	struct udevice *scsi_dev;

	return ufs_scsi_bind(dev, &scsi_dev);
}

static const struct udevice_id cdns_ufs_pltfm_ids[] = {
	{
		.compatible = "cdns,ufshc-m31-16nm",
	},
	{},
};

U_BOOT_DRIVER(cdns_ufs_pltfm) = {
	.name		= "cdns-ufs-pltfm",
	.id		=  UCLASS_UFS,
	.of_match	= cdns_ufs_pltfm_ids,
	.probe		= cdns_ufs_pltfm_probe,
	.bind		= cdns_ufs_pltfm_bind,
};
*/


int ufs_probe(struct udevice *dev)
{
	dev_info(dev, "Driver version %s\n", SUNXI_UFS_DRIVER_VESION);
	return sunxi_ufs_pltfm_probe(dev);
}
