// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2023 keith.zhao@starfivetech.com
 */

#include <asm/gpio.h>
#include <asm/io.h>
#include <common.h>
#include <clk.h>
#include <display.h>
#include <dm.h>
#include <dw_hdmi.h>
#include <edid.h>
#include <regmap.h>
#include <syscon.h>

#include <power/regulator.h>
#include <linux/delay.h>

#include "sf_hdmi.h"

static int hdmi_read(struct sf_hdmi_priv *priv,uint32_t addr)
{
	return readl(priv->base + (addr) * 0x04);

}
static void hdmi_write(struct sf_hdmi_priv *priv,int val, uint32_t addr)
{
	writel(val, priv->base + (addr) * 0x04);
}

static void inno_hdmi_detect(struct sf_hdmi_priv *priv)
{
	int val;
	val = hdmi_read(priv,0x1b0);
	val |= 0x4;
	hdmi_write(priv,val, 0x1b0); //set 0x1b0[2] to 1'b1
	hdmi_write(priv,0xf, 0x1cc); //set 0x1cc[3:0] to 4'b1111
	//while(!(hdmi_read(0x1cd)  == 0x55));

	/*turn on pre-PLL*/
	val = hdmi_read(priv,0x1a0);
	val &= ~(0x1);
	hdmi_write(priv,val, 0x1a0);
	/*turn on post-PLL*/
	val = hdmi_read(priv,0x1aa);
	val &= ~(0x1);
	hdmi_write(priv,val, 0x1aa);

	/*wait for pre-PLL and post-PLL lock*/
	while(!(hdmi_read(priv,0x1a9) & 0x1));
	while(!(hdmi_read(priv,0x1af) & 0x1));

	/*turn on LDO*/
	hdmi_write(priv,0x7, 0x1b4);
	/*turn on serializer*/
	hdmi_write(priv,0x70, 0x1be);
}

static void inno_hdmi_tx_phy_power_down(struct sf_hdmi_priv *priv)
{
	hdmi_write(priv,0x63, 0x00);
}

static void inno_hdmi_config_1440x480i60(struct sf_hdmi_priv *priv)
{
#ifdef REF_CLK_27M
	const reg_value_t cfg_pll_data[] = {
		/* config pll: 1440x480i, 60hz*/
		{0x1a0, 0x01},
		{0x1aa, 0x0f},
		{0x1a1, 0x01},
		{0x1a2, 0xf0},
		{0x1a3, 0x28},
		{0x1a4, 0x35},
		{0x1a5, 0x61},
		{0x1a6, 0x64},
		{0x1ab, 0x01},
		{0x1ac, 0x28},
		{0x1ad, 0x03},
		{0x1aa, 0x0e},
		{0x1a0, 0x00},
	};
#else
	const reg_value_t cfg_pll_data[] = {
		/* config pll: 1440x480i, 60hz*/
		{0x1a0, 0x01},
		{0x1aa, 0x0f},
		{0x1a1, 0x01},
		{0x1a2, 0xf0},
		{0x1a3, 0x64},
		{0x1a4, 0x2f},
		{0x1a5, 0x6c},
		{0x1a6, 0x64},
		{0x1ab, 0x01},
		{0x1ac, 0x50},
		{0x1ad, 0x07},
		{0x1aa, 0x0e},
		{0x1a0, 0x00},
	};
#endif
	for (int i = 0; i < sizeof(cfg_pll_data)/sizeof(reg_value_t); i++) {
		hdmi_write(priv, cfg_pll_data[i].value, cfg_pll_data[i].reg);
	}
	return;
}

static void inno_hdmi_config_640x480p60(struct sf_hdmi_priv *priv)
{
#ifdef REF_CLK_27M
	const reg_value_t cfg_pll_data[] = {
		/* config pll: 640x480p, 60hz*/
		{0x1a0, 0x01},
		{0x1aa, 0x0f},
		{0x1a1, 0x01},
		{0x1a2, 0xc0},
		{0x1a3, 0x25},
		{0x1a4, 0x35},
		{0x1a5, 0x61},
		{0x1a6, 0x64},
		{0x1ab, 0x01},
		{0x1ac, 0x28},
		{0x1ad, 0x03},
		{0x1aa, 0x0e},
		{0x1a0, 0x00},
		{0x1d1, 0x55},
		{0x1d2, 0x55},
		{0x1d3, 0x55},
	};
#else
	const reg_value_t cfg_pll_data[] = {
		/* config pll: 640x480p, 60hz*/
		{0x1a0, 0x01},
		//{0x1aa, 0x0f},
		{0x1a1, 0x01},
		{0x1a2, 0xf0},
		{0x1a3, 0x64},
		{0x1a4, 0x2f},
		//{0x1a4, 0x2a},
		{0x1a5, 0x6c},
		{0x1a6, 0x64},
		{0x1ab, 0x01},
		{0x1ac, 0x50},
		//{0x1ad, 0x07},
		{0x1ad, 0x0d},
		{0x1aa, 0x0e},
		{0x1a0, 0x00},
	};
#endif
	for (int i = 0; i < sizeof(cfg_pll_data)/sizeof(reg_value_t); i++) {
		hdmi_write(priv, cfg_pll_data[i].value, cfg_pll_data[i].reg);
	}
	return;
}

static void inno_hdmi_config_720x480p60(struct sf_hdmi_priv *priv)
{
#ifdef REF_CLK_27M
	const reg_value_t cfg_pll_data[] = {
		/* config pll: 720x480p, 60hz*/
		{0x1a0, 0x01},
		{0x1aa, 0x0f},
		{0x1a1, 0x01},
		{0x1a2, 0xf0},
		{0x1a3, 0x28},
		{0x1a4, 0x35},
		{0x1a5, 0x61},
		{0x1a6, 0x64},
		{0x1ab, 0x01},
		{0x1ac, 0x28},
		{0x1ad, 0x03},
		{0x1aa, 0x0e},
		{0x1a0, 0x00},
	};
#else
	const reg_value_t cfg_pll_data[] = {
		/* config pll: 640x480p, 60hz*/
		{0x1a0, 0x01},
		{0x1aa, 0x0f},
		{0x1a1, 0x01},
		{0x1a2, 0xf0},
		{0x1a3, 0x64},
		{0x1a4, 0x2f},
		{0x1a5, 0x6c},
		{0x1a6, 0x64},
		{0x1ab, 0x01},
		{0x1ac, 0x50},
		{0x1ad, 0x07},
		{0x1aa, 0x0e},
		{0x1a0, 0x00},
	};
#endif
	for (int i = 0; i < sizeof(cfg_pll_data)/sizeof(reg_value_t); i++) {
		hdmi_write(priv,cfg_pll_data[i].value, cfg_pll_data[i].reg);
	}
	return;
}


static void inno_hdmi_config_1280x720p60(struct sf_hdmi_priv *priv)
{
#ifdef REF_CLK_27M
	const reg_value_t cfg_pll_data[] = {
		/* config pll: 720p, 60hz*/
		{0x1a0, 0x01},
		{0x1aa, 0x0f},
		{0x1a1, 0x01},
		{0x1a2, 0xf0},
		{0x1a3, 0x37},
		{0x1a4, 0x30},
		{0x1a5, 0x61},
		{0x1a6, 0x42},
		{0x1ab, 0x01},
		{0x1ac, 0x14},
		{0x1ad, 0x01},
		{0x1aa, 0x0e},
		{0x1a0, 0x00},
	};
#else
	const reg_value_t cfg_pll_data[] = {
		/* config pll: 720p, 60hz*/
		{0x1a0, 0x01},
		{0x1aa, 0x0f},
		{0x1a1, 0x01},
		{0x1a2, 0xf0},
		{0x1a3, 0x63},
		//{0x1a4, 0x1f},
		{0x1a4, 0x1a},
		//{0x1a5, 0x48},
		{0x1a5, 0x41},
		{0x1a6, 0x64},
		{0x1ab, 0x01},
		{0x1ac, 0x14},
		{0x1ad, 0x01},
		{0x1aa, 0x0e},
		{0x1a0, 0x00},
	};
#endif
	for (int i = 0; i < sizeof(cfg_pll_data)/sizeof(reg_value_t); i++) {
		hdmi_write(priv, cfg_pll_data[i].value, cfg_pll_data[i].reg);
	}
	return;
}

static void inno_hdmi_config_1920x1080p60(struct sf_hdmi_priv *priv)
{
#ifdef REF_CLK_27M
	const reg_value_t cfg_pll_data[] = {
		/* config pll: 1080p, 60hz*/
		{0x1a0, 0x01},
		{0x1aa, 0x0f},
		{0x1a1, 0x01},
		{0x1a2, 0xf0},
		{0x1a3, 0x6e},
		{0x1a4, 0x30},
		{0x1a5, 0x60},
		{0x1a6, 0x42},
		{0x1ab, 0x04},
		{0x1ac, 0x50},
		{0x1ad, 0x01},
		{0x1aa, 0x0e},
		{0x1a0, 0x00},
	};
#else
	const reg_value_t cfg_pll_data[] = {
		/* config pll: 1080p, 60hz*/
		{0x1a0, 0x01},
		{0x1aa, 0x0f},
		{0x1a1, 0x01},
		{0x1a2, 0xf0},
		{0x1a3, 0x63},
		{0x1a4, 0x15},
		{0x1a5, 0x41},
		{0x1a6, 0x42},
		{0x1ab, 0x01},
		//{0x1ac, 0x0a},
		{0x1ac, 0x14},
		//{0x1ad, 0x00},
		{0x1ad, 0x01},
		{0x1aa, 0x0e},
		{0x1a0, 0x00},
	};
#endif
	for (int i = 0; i < sizeof(cfg_pll_data)/sizeof(reg_value_t); i++) {
		hdmi_write(priv, cfg_pll_data[i].value, cfg_pll_data[i].reg);
	}
	return;
}

static void inno_hdmi_config_3840x2160p60(struct sf_hdmi_priv *priv)
{
#ifdef REF_CLK_27M
	const reg_value_t cfg_pll_data[] = {
		/* config pll: 4K, 60hz*/
		{0x1a0, 0x01},
		{0x1aa, 0x0f},
		{0x1a1, 0x01},
		{0x1a2, 0xf0},
		{0x1a3, 0x63},
		{0x1a4, 0x08},
		{0x1a5, 0x01},
		{0x1a6, 0x21},
		{0x1ab, 0x04},
		{0x1ac, 0x14},
		{0x1ad, 0x00},
		{0x1aa, 0x02},
		{0x1a0, 0x00},
	};
#else
	const reg_value_t cfg_pll_data[] = {
		/* config pll: 4K, 60hz*/
		{0x1a0, 0x01},
		{0x1aa, 0x0f},
		{0x1a1, 0x01},
		{0x1a2, 0xf0},
		{0x1a3, 0x63},
		{0x1a4, 0x08},
		{0x1a5, 0x01},
		{0x1a6, 0x21},
		{0x1ab, 0x04},
		{0x1ac, 0x14},
		{0x1ad, 0x00},
		{0x1aa, 0x02},
		{0x1a0, 0x00},
	};
#endif
	for (int i = 0; i < sizeof(cfg_pll_data)/sizeof(reg_value_t); i++) {
		hdmi_write(priv, cfg_pll_data[i].value, cfg_pll_data[i].reg);
	}
	return;
}

static void inno_hdmi_config_3840x2160p30(struct sf_hdmi_priv *priv)
{
#ifdef REF_CLK_27M
	const reg_value_t cfg_pll_data[] = {
		/* config pll: 4K, 30hz*/
		{0x1a0, 0x01},
		{0x1aa, 0x03},
		{0x1a1, 0x01},
		{0x1a2, 0xf0},
		{0x1a3, 0x58},
		{0x1a4, 0x10},
		{0x1a5, 0x41},
		{0x1a6, 0x21},
		{0x1ab, 0x04},
		{0x1ac, 0x14},
		{0x1ad, 0x00},
		{0x1aa, 0x02},
		{0x1a0, 0x00},
	};
#else
	const reg_value_t cfg_pll_data[] = {
		/* config pll: 4K, 30hz*/
		{0x1a0, 0x01},
		{0x1aa, 0x0f},
		{0x1a1, 0x01},
		{0x1a2, 0xf0},
		{0x1a3, 0x63},
		{0x1a4, 0x10},
		{0x1a5, 0x41},
		{0x1a6, 0x21},
		{0x1ab, 0x04},
		{0x1ac, 0x14},
		{0x1ad, 0x00},
		{0x1aa, 0x02},
		{0x1a0, 0x00},
	};
#endif
	for (int i = 0; i < sizeof(cfg_pll_data)/sizeof(reg_value_t); i++) {
		hdmi_write(priv, cfg_pll_data[i].value, cfg_pll_data[i].reg);
	}
	return;
}

static void inno_hdmi_tx_ctrl(struct sf_hdmi_priv *priv,vic_code_t vic)
{
	hdmi_write(priv, 0x06, 0x9f);
	hdmi_write(priv, 0x82, 0xa0);
	hdmi_write(priv, 0xd, 0xa2);
	hdmi_write(priv, 0x0, 0xa3);
	hdmi_write(priv, 0x0, 0xa4);
	hdmi_write(priv, 0x8, 0xa5);
	hdmi_write(priv, 0x70, 0xa6);
	hdmi_write(priv, vic, 0xa7);  //conifg video format Identification Code
	hdmi_write(priv, 0x10, 0xc9); //bist mode: 0x00, normal mode: 0x10, phy mode: 0x4
}

static void inno_hdmi_tx_phy_param_config(struct sf_hdmi_priv *priv,resolution_t type)
{
	vic_code_t vic;
	switch(type) {
    case RES_1440_480I_60HZ:
		vic = VIC_1440x480i60;
		inno_hdmi_config_1440x480i60(priv);
	    break;
	case RES_640_480P_60HZ:
		vic = VIC_640x480p60;
		inno_hdmi_config_640x480p60(priv);
		break;
	case RES_720_480P_60HZ:
		vic = VIC_720x480p60;
		inno_hdmi_config_720x480p60(priv);
		break;
	case RES_1280_720P_60HZ:
		vic = VIC_1280x720p60;
		inno_hdmi_config_1280x720p60(priv);
		break;
	case RES_1920_1080P_60HZ:
		vic = VIC_1920x1080p60;
		inno_hdmi_config_1920x1080p60(priv);
		break;
	case RES_3840_2160P_30HZ:
		vic = VIC_3840x2160p30;
		inno_hdmi_config_3840x2160p30(priv);
		break;
	case RES_3840_2160P_60HZ:
		vic = VIC_3840x2160p60;
		inno_hdmi_config_3840x2160p60(priv);
		break;
	}
	inno_hdmi_tx_ctrl(priv, vic);

    return;
}

static void inno_hdmi_tx_phy_power_on(struct sf_hdmi_priv *priv)
{
	hdmi_write(priv, 0x61, 0x00); //0x61: power 0n, 0x63: power off
}

static void inno_hdmi_data_sync(struct sf_hdmi_priv *priv)
{
	hdmi_write(priv, 0x00, 0xce);
	hdmi_write(priv, 0x01, 0xce);
}

void inno_hdmi_tmds_driver_on(struct sf_hdmi_priv *priv)
{
	hdmi_write(priv, 0x8f, 0x1b2);
	mdelay(50);
}

static int inno_hdmi_enable(struct udevice *dev, int panel_bpp,
			      const struct display_timing *edid)
{
	struct sf_hdmi_priv *priv = dev_get_priv(dev);
	debug("inno_hdmi_enable on\r\n");
	inno_hdmi_detect(priv);
	inno_hdmi_tx_phy_power_down(priv);
	inno_hdmi_tx_phy_param_config(priv,RES_1920_1080P_60HZ);
	inno_hdmi_tx_phy_power_on(priv);
	inno_hdmi_tmds_driver_on(priv);
	/*data sync*/
	inno_hdmi_data_sync(priv);
	return 0;
}

int rk_hdmi_read_edid(struct udevice *dev, u8 *buf, int buf_size)
{
	//need fix next
 	return 0;
}

static int inno_hdmi_of_to_plat(struct udevice *dev)
{
	struct sf_hdmi_priv *priv = dev_get_priv(dev);
	int ret;
	priv->base = dev_remap_addr(dev);
	if (!priv->base)
		return -EINVAL;
	debug("%s----priv->base = %px\n",__func__,priv->base);

	ret = clk_get_by_name(dev, "sysclk", &priv->sys_clk);
	if (ret) {
		pr_err("clk_get_by_name(sysclk) failed: %d", ret);
		return ret;
	}

	ret = clk_get_by_name(dev, "mclk", &priv->mclk);
	if (ret) {
		pr_err("clk_get_by_name(mclk) failed: %d\n", ret);
		return ret;
	}

	ret = clk_get_by_name(dev, "bclk", &priv->bclk);
	if (ret) {
		pr_err("clk_get_by_name(bclk) failed: %d\n", ret);
		return ret;
	}

	ret = reset_get_by_name(dev, "hdmi_tx", &priv->tx_rst);
	if (ret) {
		pr_err("failed to get hdmi_tx reset (ret=%d)\n", ret);
		return ret;
	}
	return 0;
}

static int inno_hdmi_probe(struct udevice *dev)
{
	struct sf_hdmi_priv *priv = dev_get_priv(dev);
	int ret;
	ret = clk_enable(&priv->sys_clk);
	if (ret < 0) {
		pr_err("clk_enable(sys_clk) failed: %d\n", ret);
		return ret;
	}

	ret = clk_enable(&priv->mclk);
	if (ret < 0) {
		pr_err("clk_enable(mclk) failed: %d\n", ret);
		goto free_clock_sys_clk;
	}

	ret = clk_enable(&priv->bclk);
	if (ret < 0) {
		pr_err("clk_enable(bclk) failed: %d\n", ret);
		goto free_clock_mclk_clk;
	}

	ret = reset_deassert(&priv->tx_rst);
	if (ret < 0) {
		pr_err("failed to deassert tx_rst\n");
		goto free_reset;
	}

	ret = (hdmi_read(priv, HDMI_STATUS) & m_HOTPLUG) ? 0 : 1; // 0 connected.. 1 disconnected
	debug("ret = %d\n",ret);
	return ret;

free_reset:
	clk_disable(&priv->bclk);
free_clock_mclk_clk:
	clk_disable(&priv->mclk);
free_clock_sys_clk:
	clk_disable(&priv->sys_clk);

	return ret;

}

static int sf_hdmi_remove(struct udevice *dev)
{
	struct sf_hdmi_priv *priv = dev_get_priv(dev);
	debug("sf_hdmi_remove  ---\n");
	hdmi_write(priv, 0x00,0x1b2);
	hdmi_write(priv, 0x00,0x1be);
	hdmi_write(priv, 0x00,0x1b4);
	hdmi_write(priv, 1,0x1a0);
	hdmi_write(priv, 1,0x1aa);
	hdmi_write(priv, 0x00,0x1cc);
	hdmi_write(priv, 0x00,0x1b0);

	clk_disable(&priv->bclk);
	clk_disable(&priv->mclk);
	clk_disable(&priv->sys_clk);
	reset_assert(&priv->tx_rst);
	return 0;
}

static const struct dm_display_ops inno_hdmi_ops = {
	.read_edid = rk_hdmi_read_edid,
	.enable = inno_hdmi_enable,
};

static const struct udevice_id inno_hdmi_ids[] = {
	{ .compatible = "starfive,inno-hdmi" },
	{ }
};

U_BOOT_DRIVER(inno_hdmi_starfive) = {
	.name = "inno_hdmi_starfive",
	.id = UCLASS_DISPLAY,
	.of_match = inno_hdmi_ids,
	.ops = &inno_hdmi_ops,
	.of_to_plat = inno_hdmi_of_to_plat,
	.probe = inno_hdmi_probe,
	.priv_auto	= sizeof(struct sf_hdmi_priv),
	.remove = sf_hdmi_remove,
	.flags = DM_FLAG_OS_PREPARE,
};
