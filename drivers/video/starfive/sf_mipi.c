#include <asm/gpio.h>
#include <asm/io.h>
#include <common.h>
#include <clk.h>
#include <display.h>
#include <dm.h>
#include <log.h>
#include <panel.h>
#include <regmap.h>
#include <syscon.h>

#include <dm/uclass-internal.h>
#include <linux/err.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/iopoll.h>
#include <linux/err.h>
#include <power/regulator.h>

#include <video_bridge.h>
#include <dsi_host.h>
#include <mipi_dsi.h>
#include <reset.h>
#include <video.h>

#include "sf_mipi.h"

static inline u32 sf_dphy_get_reg(void __iomem * io_addr, u32 shift, u32 mask)
{
	return (readl(io_addr) & mask) >> shift;
}

static inline void sf_dphy_set_reg(void __iomem * io_addr, u32 data, u32 shift, u32 mask)
{
    u32 tmp;
    tmp = readl(io_addr);
    tmp &= ~mask;
    tmp |= (data << shift) & mask;
    writel(tmp, io_addr);
}

static int dsi_phy_init(void *priv_data)
{
	struct mipi_dsi_device *device = priv_data;
	struct udevice *dev = device->dev;
	struct dsi_sf_priv *dsi = dev_get_priv(dev);

	uint32_t temp;

	temp = sf_dphy_get_reg(dsi->sys_reg, AON_GP_REG_SHIFT,AON_GP_REG_MASK);

	if (!(temp & DPHY_TX_PSW_EN_MASK)) {
		temp |= DPHY_TX_PSW_EN_MASK;
		sf_dphy_set_reg(dsi->sys_reg, temp,AON_GP_REG_SHIFT,AON_GP_REG_MASK);
	}

	return 0;
}

static int is_pll_locked(void __iomem *phy_reg)
{
	return !sf_dphy_get_reg(phy_reg + 0x8,
				RGS_CDTX_PLL_UNLOCK_SHIFT, RGS_CDTX_PLL_UNLOCK_MASK);
}

static void reset(int assert, void __iomem *phy_reg)
{
	sf_dphy_set_reg(phy_reg + 0x64, (!assert), RESETB_SHIFT, RESETB_MASK);

	if (!assert) {
		while(!is_pll_locked(phy_reg));
	}
}

int sys_mipi_dsi_set_ppi_txbyte_hs(int enable, void *priv_data)
{
	struct mipi_dsi_device *device = priv_data;
	struct udevice *dev = device->dev;
	struct dsi_sf_priv *priv = dev_get_priv(dev);
	static int status = 0;
	int ret;
	if (!enable && status) {
		status = 0;
		ret = reset_assert(&priv->txbytehs_rst);
		if (ret < 0) {
			return ret;
		}
	} else if (enable && !status) {
		status = 1;
		ret = reset_deassert(&priv->txbytehs_rst);
		if (ret < 0) {
			return ret;
		}
	}
	mdelay(100);
	return 0;
}

static void dsi_phy_post_set_mode(void *priv_data, unsigned long mode_flags)
{
	struct mipi_dsi_device *device = priv_data;
	struct udevice *dev = device->dev;
	struct dsi_sf_priv *priv = dev_get_priv(dev);
	uint32_t bitrate;
	unsigned long alignment;
	int i;
	int ret;
	const struct m31_dphy_config *p;
	const uint32_t AON_POWER_READY_N_active = 0;

	debug("Set mode enable %ld\n",
		mode_flags & MIPI_DSI_MODE_VIDEO);

	if (!priv)
		return;

	bitrate = 750000000;//1188M 60fps

	sf_dphy_set_reg(priv->phy_reg + 0x8, 0x10,
					RG_CDTX_L0N_HSTX_RES_SHIFT, RG_CDTX_L0N_HSTX_RES_MASK);
	sf_dphy_set_reg(priv->phy_reg + 0xC, 0x10,
					RG_CDTX_L0N_HSTX_RES_SHIFT, RG_CDTX_L0N_HSTX_RES_MASK);
	sf_dphy_set_reg(priv->phy_reg + 0xC, 0x10,
					RG_CDTX_L2N_HSTX_RES_SHIFT, RG_CDTX_L2N_HSTX_RES_MASK);
	sf_dphy_set_reg(priv->phy_reg + 0xC, 0x10,
					RG_CDTX_L3N_HSTX_RES_SHIFT, RG_CDTX_L3N_HSTX_RES_MASK);
	sf_dphy_set_reg(priv->phy_reg + 0x10, 0x10,
					RG_CDTX_L4N_HSTX_RES_SHIFT, RG_CDTX_L4N_HSTX_RES_MASK);
	sf_dphy_set_reg(priv->phy_reg + 0x8, 0x10,
					RG_CDTX_L0P_HSTX_RES_SHIFT, RG_CDTX_L0P_HSTX_RES_MASK);
	sf_dphy_set_reg(priv->phy_reg + 0xC, 0x10,
					RG_CDTX_L1P_HSTX_RES_SHIFT, RG_CDTX_L1P_HSTX_RES_MASK);
	sf_dphy_set_reg(priv->phy_reg + 0xC, 0x10,
					RG_CDTX_L2P_HSTX_RES_SHIFT, RG_CDTX_L2P_HSTX_RES_MASK);
	sf_dphy_set_reg(priv->phy_reg + 0xC, 0x10,
					RG_CDTX_L3P_HSTX_RES_SHIFT, RG_CDTX_L3P_HSTX_RES_MASK);
	sf_dphy_set_reg(priv->phy_reg + 0x10, 0x10,
					RG_CDTX_L4P_HSTX_RES_SHIFT, RG_CDTX_L4P_HSTX_RES_MASK);

	if (is_pll_locked(priv->phy_reg))
		debug("Error: MIPI dphy-tx # PLL is not supposed to be LOCKED\n");
	else
		debug("MIPI dphy-tx # PLL is not LOCKED\n");

	alignment = M31_DPHY_BITRATE_ALIGN;
	if (bitrate % alignment) {
		bitrate += alignment - (bitrate % alignment);
	}

	p = m31_dphy_configs;
	for (i = 0; i < ARRAY_SIZE(m31_dphy_configs); i++, p++) {
		if (p->bitrate == bitrate) {
			sf_dphy_set_reg(priv->phy_reg + 0x64, M31_DPHY_REFCLK, REFCLK_IN_SEL_SHIFT, REFCLK_IN_SEL_MASK);

			sf_dphy_set_reg(priv->phy_reg, AON_POWER_READY_N_active,
							AON_POWER_READY_N_SHIFT, AON_POWER_READY_N_MASK);

			sf_dphy_set_reg(priv->phy_reg, 0x0,
							CFG_L0_SWAP_SEL_SHIFT, CFG_L0_SWAP_SEL_MASK);//Lane setting
			sf_dphy_set_reg(priv->phy_reg, 0x1,
							CFG_L1_SWAP_SEL_SHIFT, CFG_L1_SWAP_SEL_MASK);
			sf_dphy_set_reg(priv->phy_reg, 0x2,
							CFG_L2_SWAP_SEL_SHIFT, CFG_L2_SWAP_SEL_MASK);
			sf_dphy_set_reg(priv->phy_reg, 0x3,
							CFG_L3_SWAP_SEL_SHIFT, CFG_L3_SWAP_SEL_MASK);
			sf_dphy_set_reg(priv->phy_reg, 0x4,
							CFG_L4_SWAP_SEL_SHIFT, CFG_L4_SWAP_SEL_MASK);
			//PLL setting
			sf_dphy_set_reg(priv->phy_reg + 0x1c, 0x0,
							RG_CDTX_PLL_SSC_EN_SHIFT, RG_CDTX_PLL_SSC_EN_MASK);
			sf_dphy_set_reg(priv->phy_reg + 0x18, 0x1,
							RG_CDTX_PLL_LDO_STB_X2_EN_SHIFT, RG_CDTX_PLL_LDO_STB_X2_EN_MASK);
			sf_dphy_set_reg(priv->phy_reg + 0x18, 0x1,
							RG_CDTX_PLL_FM_EN_SHIFT, RG_CDTX_PLL_FM_EN_MASK);

			sf_dphy_set_reg(priv->phy_reg + 0x18, p->pll_prev_div,
							RG_CDTX_PLL_PRE_DIV_SHIFT, RG_CDTX_PLL_PRE_DIV_MASK);
			sf_dphy_set_reg(priv->phy_reg + 0x18, p->pll_fbk_int,
							RG_CDTX_PLL_FBK_INT_SHIFT, RG_CDTX_PLL_FBK_INT_MASK);
			sf_dphy_set_reg(priv->phy_reg + 0x14, p->pll_fbk_fra,
							RG_CDTX_PLL_FBK_FRA_SHIFT, RG_CDTX_PLL_FBK_FRA_MASK);
			sf_dphy_set_reg(priv->phy_reg + 0x28, p->extd_cycle_sel,
							RG_EXTD_CYCLE_SEL_SHIFT, RG_EXTD_CYCLE_SEL_MASK);
			sf_dphy_set_reg(priv->phy_reg + 0x24, p->dlane_hs_pre_time,
							RG_DLANE_HS_PRE_TIME_SHIFT, RG_DLANE_HS_PRE_TIME_MASK);
			sf_dphy_set_reg(priv->phy_reg + 0x24, p->dlane_hs_pre_time,
							RG_DLANE_HS_PRE_TIME_SHIFT, RG_DLANE_HS_PRE_TIME_MASK);
			sf_dphy_set_reg(priv->phy_reg + 0x24, p->dlane_hs_zero_time,
							RG_DLANE_HS_ZERO_TIME_SHIFT, RG_DLANE_HS_ZERO_TIME_MASK);
			sf_dphy_set_reg(priv->phy_reg + 0x24, p->dlane_hs_trail_time,
							RG_DLANE_HS_TRAIL_TIME_SHIFT, RG_DLANE_HS_TRAIL_TIME_MASK);
			sf_dphy_set_reg(priv->phy_reg + 0x20, p->clane_hs_pre_time,
							RG_CLANE_HS_PRE_TIME_SHIFT, RG_CLANE_HS_PRE_TIME_MASK);
			sf_dphy_set_reg(priv->phy_reg + 0x24, p->clane_hs_zero_time,
							RG_CLANE_HS_ZERO_TIME_SHIFT, RG_CLANE_HS_ZERO_TIME_MASK);
			sf_dphy_set_reg(priv->phy_reg + 0x20, p->clane_hs_trail_time,
							RG_CLANE_HS_TRAIL_TIME_SHIFT, RG_CLANE_HS_TRAIL_TIME_MASK);
			sf_dphy_set_reg(priv->phy_reg + 0x20, p->clane_hs_clk_pre_time,
							RG_CLANE_HS_CLK_PRE_TIME_SHIFT, RG_CLANE_HS_CLK_PRE_TIME_MASK);
			sf_dphy_set_reg(priv->phy_reg + 0x20, p->clane_hs_clk_post_time,
							RG_CLANE_HS_CLK_POST_TIME_SHIFT, RG_CLANE_HS_CLK_POST_TIME_MASK);
			break;
		}
	}
	
	reset(0, priv->phy_reg);
	sf_dphy_set_reg(priv->phy_reg + 0x30, 0,
					SCFG_PPI_C_READY_SEL_SHIFT, SCFG_PPI_C_READY_SEL_MASK);
	sf_dphy_set_reg(priv->phy_reg + 0x30, 0,
					SCFG_DSI_TXREADY_ESC_SEL_SHIFT, SCFG_DSI_TXREADY_ESC_SEL_MASK);
	sf_dphy_set_reg(priv->phy_reg + 0x2c, 0x30,
					SCFG_C_HS_PRE_ZERO_TIME_SHIFT, SCFG_C_HS_PRE_ZERO_TIME_MASK);
	ret = clk_enable(&priv->dphy_txesc_clk);
	if (ret) {
		pr_err("failed to prepare/enable dphy_txesc_clk\n");
		return;
	}

	ret = reset_deassert(&priv->dphy_sys);
	if (ret < 0) {
		pr_err("failed to deassert dphy_sys\n");
		return;
	}

	ret = sys_mipi_dsi_set_ppi_txbyte_hs(1, priv_data);
	return;
}

static int dsi_get_lane_mbps(void *priv_data, struct display_timing *timings,
			     u32 lanes, u32 format, unsigned int *lane_mbps)
{
	return 0;
}

static const struct mipi_dsi_phy_ops dsi_stm_phy_ops = {
	.init = dsi_phy_init,
	.get_lane_mbps = dsi_get_lane_mbps,
	.post_set_mode = dsi_phy_post_set_mode,
};

static int dsi_sf_attach(struct udevice *dev)
{
	struct dsi_sf_priv *priv = dev_get_priv(dev);
	struct mipi_dsi_device *device = &priv->device;
	struct mipi_dsi_panel_plat *mplat;
	struct display_timing timings;
	int ret;

	ret = uclass_first_device(UCLASS_PANEL, &priv->panel);
	if (ret) {
		debug("panel device error %d\n", ret);
		return ret;
	}
	debug("%s,priv->panel->name = %s\n", __func__,priv->panel->name);

	mplat = dev_get_plat(priv->panel);
	mplat->device = &priv->device;
	device->lanes = mplat->lanes;
	device->format = mplat->format;
	device->mode_flags = mplat->mode_flags;

	ret = panel_get_display_timing(priv->panel, &timings);
	if (ret) {
		ret = ofnode_decode_display_timing(dev_ofnode(priv->panel),
						   0, &timings);
		if (ret) {
			printf("decode display timing error %d\n", ret);
			return ret;
		}
	}

	ret = uclass_get_device(UCLASS_DSI_HOST, 0, &priv->dsi_host);
	if (ret) {
		printf("No video dsi host detected %d\n", ret);
		return ret;
	}

	ret = dsi_host_init(priv->dsi_host, device, &timings,
			mplat->lanes,
			&dsi_stm_phy_ops);
	if (ret) {
		printf("failed to initialize mipi dsi host\n");
		return ret;
	}

	return 0;
}

static int dsi_sf_set_backlight(struct udevice *dev, int percent)
{
	struct dsi_sf_priv *priv = dev_get_priv(dev);
	int ret;

	ret = dsi_host_enable(priv->dsi_host);
	if (ret) {
		printf("failed to enable mipi dsi host\n");
		return ret;
	}

	ret = panel_enable_backlight(priv->panel);
	if (ret) {
		printf("panel %s enable backlight error %d\n",
			priv->panel->name, ret);
		return ret;
	}

	return 0;
}

static int sf_mipi_of_to_plat(struct udevice *dev)
{
	struct dsi_sf_priv *priv = dev_get_priv(dev);
	int ret = 0;
	priv->dsi_reg = dev_remap_addr_name(dev, "dsi");
	if (!priv->dsi_reg)
		return -EINVAL;

	priv->phy_reg = dev_remap_addr_name(dev, "phy");
	if (!priv->phy_reg)
		return -EINVAL;

	priv->sys_reg = dev_remap_addr_name(dev, "syscon");
	if (!priv->phy_reg)
		return -EINVAL;

	ret = clk_get_by_name(dev, "sys", &priv->dsi_sys_clk);
	if (ret) {
		pr_err("clk_get_by_name(dsi_sys_clk) failed: %d", ret);
		return ret;
	}

	ret = clk_get_by_name(dev, "apb", &priv->apb_clk);
	if (ret) {
		pr_err("clk_get_by_name(apb_clk) failed: %d\n", ret);
		return ret;
	}

	ret = clk_get_by_name(dev, "txesc", &priv->txesc_clk);
	if (ret) {
		pr_err("clk_get_by_name(txesc_clk) failed: %d\n", ret);
		return ret;
	}

	ret = clk_get_by_name(dev, "dpi", &priv->dpi_clk);
	if (ret) {
		pr_err("clk_get_by_name(dpi_clk) failed: %d\n", ret);
		return ret;
	}

	ret = clk_get_by_name(dev, "dphy_txesc", &priv->dphy_txesc_clk);
	if (ret) {
		pr_err("clk_get_by_name(dphy_txesc_clk) failed: %d\n", ret);
		return ret;
	}

	ret = reset_get_by_name(dev, "dsi_dpi", &priv->dpi_rst);
	if (ret) {
		pr_err("failed to get dpi_rst reset (ret=%d)\n", ret);
		return ret;
	}

	ret = reset_get_by_name(dev, "dsi_apb", &priv->apb_rst);
	if (ret) {
		pr_err("failed to get apb_rst reset (ret=%d)\n", ret);
		return ret;
	}
	ret = reset_get_by_name(dev, "dsi_rxesc", &priv->rxesc_rst);
	if (ret) {
		pr_err("failed to get rxesc_rst reset (ret=%d)\n", ret);
		return ret;
	}
	ret = reset_get_by_name(dev, "dsi_sys", &priv->sys_rst);
	if (ret) {
		pr_err("failed to get sys_rst reset (ret=%d)\n", ret);
		return ret;
	}
	ret = reset_get_by_name(dev, "dsi_txbytehs", &priv->txbytehs_rst);
	if (ret) {
		pr_err("failed to get txbytehs_rst reset (ret=%d)\n", ret);
		return ret;
	}

	ret = reset_get_by_name(dev, "dsi_txesc", &priv->txesc_rst);
	if (ret) {
		pr_err("failed to get txesc_rst reset (ret=%d)\n", ret);
		return ret;
	}
	ret = reset_get_by_name(dev, "dphy_sys", &priv->dphy_sys);
	if (ret) {
		pr_err("failed to get dphy_sys reset (ret=%d)\n", ret);
		return ret;
	}
	ret = reset_get_by_name(dev, "dphy_txbytehs", &priv->dphy_txbytehs);
	if (ret) {
		pr_err("failed to get dphy_txbytehs reset (ret=%d)\n", ret);
		return ret;
	}

	return 0;
}

static int cdns_check_register_access(struct udevice *dev)
{
	struct dsi_sf_priv *priv = dev_get_priv(dev);
	const uint16_t ctrl_patterns[] = {0x0000, 0xffff, 0xa5a5, 0x5a5a};
	int i;
	uint32_t rd_val;

	for (i = 0; i < ARRAY_SIZE_DSI(ctrl_patterns); i++) {
		uint32_t temp = readl(priv->dsi_reg + TEST_GENERIC);
		temp &= ~0xffff;
		temp |= TEST_CTRL(ctrl_patterns[i]);
		writel(temp, priv->dsi_reg + TEST_GENERIC);

		rd_val = readl(priv->dsi_reg + TEST_GENERIC);
		if (rd_val != temp) {
			return 1;
		}
	}

    return 0;
}

static int dsi_sf_probe(struct udevice *dev)
{
	struct dsi_sf_priv *priv = dev_get_priv(dev);
	struct mipi_dsi_device *device = &priv->device;
	int ret;
	unsigned long rate;
	uint32_t val;

	device->dev = dev;

	ret = dev_read_u32(dev, "data-lanes-num", &priv->data_lanes);
	if (ret) {
		printf("fail to get data lanes property %d\n", ret);
		return 0;
	}

	ret = clk_enable(&priv->dsi_sys_clk);
	if (ret < 0) {
		pr_err("clk_enable(dsi_sys_clk) failed: %d\n", ret);
		return ret;
	}

	ret = clk_enable(&priv->apb_clk);
	if (ret < 0) {
		pr_err("clk_enable(apb_clk) failed: %d\n", ret);
		goto free_clock_apb_clk;
	}

	ret = clk_enable(&priv->txesc_clk);
	if (ret < 0) {
		pr_err("clk_enable(txesc_clk) failed: %d\n", ret);
		goto free_clock_txesc_clk;
	}

	ret = clk_enable(&priv->dpi_clk);
	if (ret < 0) {
		pr_err("clk_enable(dpi_clk) failed: %d\n", ret);
		goto free_clock_dpi_clk;
	}

	ret = reset_deassert(&priv->sys_rst);
	if (ret < 0) {
		pr_err("failed to deassert sys_rst\n");
		goto free_reset;
    }

	ret = reset_deassert(&priv->apb_rst);
	if (ret) {
		pr_err("failed to deassert apb_rst reset (ret=%d)\n", ret);
		goto free_reset;
	}

	ret = reset_deassert(&priv->txesc_rst);
	if (ret) {
		pr_err("failed to deassert txesc_rst reset (ret=%d)\n", ret);
		goto free_reset;
	}


	ret = reset_deassert(&priv->rxesc_rst);
	if (ret) {
		pr_err("failed to deassert rxesc_rst reset (ret=%d)\n", ret);
		goto free_reset;
	}

	ret = reset_deassert(&priv->dpi_rst);
	if (ret) {
		pr_err("failed to deassert dpi_rst reset (ret=%d)\n", ret);
		goto free_reset;
	}

	rate = clk_get_rate(&priv->dsi_sys_clk);
	debug("%s ok: dsi_sys_clk rate = %ld\n", __func__, rate);

	val = readl(priv->dsi_reg + ID_REG);

	debug("%s ok: ID_REG val = %08x\n", __func__, val);
	if (REV_VENDOR_ID(val) != 0xcad) {
		printf("invalid vendor id\n");
		ret = -EINVAL;
	}

	ret = cdns_check_register_access(dev);
    if (ret) {
        printf("error: r/w test generic reg failed\n");
    }

	val = readl(priv->dsi_reg + IP_CONF);
	priv->direct_cmd_fifo_depth = 1 << (DIRCMD_FIFO_DEPTH(val) + 2);
	priv->rx_fifo_depth = RX_FIFO_DEPTH(val);

	writel(0, priv->dsi_reg + MCTL_MAIN_DATA_CTL);
	writel(0, priv->dsi_reg + MCTL_MAIN_EN);
	writel(0, priv->dsi_reg + MCTL_MAIN_PHY_CTL);
	/* Mask all interrupts before registering the IRQ handler. */
	writel(0, priv->dsi_reg + MCTL_MAIN_STS_CTL);
	writel(0, priv->dsi_reg + MCTL_DPHY_ERR_CTL1);
	writel(0, priv->dsi_reg + CMD_MODE_STS_CTL);
	writel(0, priv->dsi_reg + DIRECT_CMD_STS_CTL);
	writel(0, priv->dsi_reg + DIRECT_CMD_RD_STS_CTL);
	writel(0, priv->dsi_reg + VID_MODE_STS_CTL);
	writel(0, priv->dsi_reg + TVG_STS_CTL);
	writel(0, priv->dsi_reg + DPI_IRQ_EN);

	return ret;

free_reset:
	clk_disable(&priv->dpi_clk);
free_clock_dpi_clk:
	clk_disable(&priv->txesc_clk);
free_clock_txesc_clk:
	clk_disable(&priv->apb_clk);
free_clock_apb_clk:
	clk_disable(&priv->dsi_sys_clk);

	return ret;
}

struct video_bridge_ops sf_dsi_bridge_ops = {
	.attach = dsi_sf_attach,
	.set_backlight = dsi_sf_set_backlight,
};

static const struct udevice_id sf_mipi_dsi_ids[] = {
	{ .compatible = "starfive,sf_mipi_dsi" },
	{ }
};

U_BOOT_DRIVER(starfive_mipi_dsi) = {
	.name	= "starfive_mipi_dsi",
	.id	= UCLASS_VIDEO_BRIDGE,
	.of_match = sf_mipi_dsi_ids,
	.bind	= dm_scan_fdt_dev,
	.of_to_plat = sf_mipi_of_to_plat,
	.probe	= dsi_sf_probe,
	.ops	= &sf_dsi_bridge_ops,
	.priv_auto	  = sizeof(struct dsi_sf_priv),
};
