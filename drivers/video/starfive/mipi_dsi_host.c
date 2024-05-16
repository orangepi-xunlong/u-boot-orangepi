/*
 * Copyright 2016-2019 NXP
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <asm/io.h>
#include <common.h>
#include <div64.h>
#include <dm.h>
#include <dm/device-internal.h>
#include <dsi_host.h>
#include <linux/err.h>
#include <linux/string.h>
#include <malloc.h>
#include <panel.h>
#include <regmap.h>
#include <syscon.h>
#include <video_bridge.h>

#include "sf_mipi.h"
#include "mipi_dsi_northwest_regs.h"

#define MIPI_LCD_SLEEP_MODE_DELAY	(120)
#define MIPI_FIFO_TIMEOUT		250000 /* 250ms */
#define	PS2KHZ(ps)	(1000000000UL / (ps))
#define MSEC_PER_SEC			1000

#define DIV_ROUND_CLOSEST_ULL(x, divisor)(		\
{							\
	typeof(divisor) __d = divisor;			\
	unsigned long long _tmp = (x) + (__d) / 2;	\
	do_div(_tmp, __d);				\
	_tmp;						\
}							\
)

enum mipi_dsi_mode {
	DSI_COMMAND_MODE,
	DSI_VIDEO_MODE
};

#define DSI_LP_MODE	0
#define DSI_HS_MODE	1

enum mipi_dsi_payload {
	DSI_PAYLOAD_CMD,
	DSI_PAYLOAD_VIDEO,
};

/*
 * mipi-dsi northwest driver information structure, holds useful data for the driver.
 */
struct mipi_dsi_northwest_info {
	void __iomem *dsi_base;
	void __iomem *phy_base;
	void __iomem *mmio_base;

	struct mipi_dsi_device *device;
	struct mipi_dsi_host dsi_host;
	struct display_timing timings;
	struct regmap *sim;
	const struct mipi_dsi_phy_ops *phy_ops;

	uint32_t max_data_lanes;
	uint32_t max_data_rate;
	uint32_t pll_ref;
	bool link_initialized;
};

static void cdns_dsi_init_link(struct mipi_dsi_northwest_info *mipi_dsi, struct mipi_dsi_device *device)
{
	unsigned long  ulpout;
	u32 val;
	int i;
	struct udevice *dev = device->dev;
	struct dsi_sf_priv *priv = dev_get_priv(dev);

	if (mipi_dsi->link_initialized)
		return;

	val = WAIT_BURST_TIME(0xf);
	for (i = 1; i < mipi_dsi->max_data_lanes; i++)
		val |= DATA_LANE_EN(i);

	debug("%s,device->mode_flags = 0x%ld,mipi_dsi->max_data_lanes = %d\n", __func__,device->mode_flags,mipi_dsi->max_data_lanes);

	if (!(device->mode_flags & MIPI_DSI_CLOCK_NON_CONTINUOUS))
		val |= CLK_CONTINUOUS;

	writel(val, mipi_dsi->dsi_base + MCTL_MAIN_PHY_CTL);

	ulpout = DIV_ROUND_UP(clk_get_rate(&priv->dsi_sys_clk), MSEC_PER_SEC);
	debug("%s ulpout: 0x%ld\n", __func__, ulpout);

	writel(CLK_LANE_ULPOUT_TIME(ulpout) | DATA_LANE_ULPOUT_TIME(ulpout),
		mipi_dsi->dsi_base + MCTL_ULPOUT_TIME);

	writel(LINK_EN, mipi_dsi->dsi_base + MCTL_MAIN_DATA_CTL);

	val = CLK_LANE_EN | PLL_START ; // | PLL_START; unused bit
	for (i = 0; i < mipi_dsi->max_data_lanes; i++)
		val |= DATA_LANE_START(i);

	writel(val, mipi_dsi->dsi_base + MCTL_MAIN_EN);
	udelay(20);

	mipi_dsi->link_initialized = true;
}


static inline struct mipi_dsi_northwest_info *host_to_dsi(struct mipi_dsi_host *host)
{
	return container_of(host, struct mipi_dsi_northwest_info, dsi_host);
}

static int mipi_dsi_northwest_host_attach(struct mipi_dsi_host *host,
				   struct mipi_dsi_device *device)
{
	return 0;
}

static int wait_for_send_done(struct mipi_dsi_northwest_info *mipi_dsi, unsigned long timeout)
{
   uint32_t irq_status;
   uint32_t ctl;

   do {
	   irq_status = readl(mipi_dsi->dsi_base + DIRECT_CMD_STS_FLAG);
	   if (irq_status)
	   {
			ctl = readl(mipi_dsi->dsi_base + DIRECT_CMD_STS_CTL);
			ctl &= ~irq_status;
			writel(ctl, mipi_dsi->dsi_base + DIRECT_CMD_STS_CTL);
		    return timeout;
	   }
	   udelay(1);
   } while (--timeout);

   return 0;
}

static ssize_t cdns_dsi_transfer(struct mipi_dsi_host *host,
				const struct mipi_dsi_msg *msg)
{
	struct mipi_dsi_northwest_info *dsi = host_to_dsi(host);
	u32 cmd, sts, val, wait = WRITE_COMPLETED, ctl = 0;
	struct mipi_dsi_packet packet;
	int ret, i, tx_len, rx_len;
	struct udevice *dev = dsi->device->dev;
	struct dsi_sf_priv *priv = dev_get_priv(dev);

	cdns_dsi_init_link(dsi, dsi->device);

	ret = mipi_dsi_create_packet(&packet, msg);
	if (ret)
	   goto out;

	tx_len = msg->tx_buf ? msg->tx_len : 0;
	rx_len = msg->rx_buf ? msg->rx_len : 0;

	/* For read operations, the maximum TX len is 2. */
	if (rx_len && tx_len > 2) {
		ret = -ENOTSUPP;
		goto out;
	}

	/* TX len is limited by the CMD FIFO depth. */
	if (tx_len > priv->direct_cmd_fifo_depth) {
		ret = -ENOTSUPP;
		goto out;
	}

	/* RX len is limited by the RX FIFO depth. */
	if (rx_len > priv->rx_fifo_depth) {
		ret = -ENOTSUPP;
		goto out;
	}

	cmd = CMD_SIZE(tx_len) | CMD_VCHAN_ID(msg->channel) |
		 CMD_DATATYPE(msg->type);

	if (msg->flags & MIPI_DSI_MSG_USE_LPM)
	   cmd |= CMD_LP_EN;

	if (mipi_dsi_packet_format_is_long(msg->type))
	   cmd |= CMD_LONG;

	if (rx_len) {
		cmd |= READ_CMD;
		wait = READ_COMPLETED_WITH_ERR | READ_COMPLETED;
		ctl = READ_EN | BTA_EN;
	} else if (msg->flags & MIPI_DSI_MSG_REQ_ACK) {
		cmd |= BTA_REQ;
		wait = ACK_WITH_ERR_RCVD | ACK_RCVD;
		ctl = BTA_EN;
	}
	udelay(10);
	writel(readl(dsi->dsi_base + MCTL_MAIN_DATA_CTL) | ctl,
		  dsi->dsi_base + MCTL_MAIN_DATA_CTL);

	writel(cmd, dsi->dsi_base + DIRECT_CMD_MAIN_SETTINGS);

	for (i = 0; i < tx_len; i += 4) {
	   const u8 *buf = msg->tx_buf;
	   int j;

	   val = 0;
	   for (j = 0; j < 4 && j + i < tx_len; j++)
		   val |= (u32)buf[i + j] << (8 * j);

	   writel(val, dsi->dsi_base + DIRECT_CMD_WRDATA);
	}

	/* Clear status flags before sending the command. */
	writel(wait, dsi->dsi_base + DIRECT_CMD_STS_CLR);
	writel(wait, dsi->dsi_base + DIRECT_CMD_STS_CTL);

	writel(0, dsi->dsi_base + DIRECT_CMD_SEND);

	ret = wait_for_send_done(dsi, MIPI_FIFO_TIMEOUT);
	if (!ret) {
		printf("wait tx done timeout!\n");
		return -ETIMEDOUT;
	}
	udelay(10);
	sts = readl(dsi->dsi_base + DIRECT_CMD_STS);
	writel(wait, dsi->dsi_base + DIRECT_CMD_STS_CLR);
	writel(0, dsi->dsi_base + DIRECT_CMD_STS_CTL);

	writel(readl(dsi->dsi_base + MCTL_MAIN_DATA_CTL) & ~ctl,
		  dsi->dsi_base + MCTL_MAIN_DATA_CTL);

	/* We did not receive the events we were waiting for. */
	if (!(sts & wait)) {
	   ret = -ETIMEDOUT;
	   goto out;
	}

	/* 'READ' or 'WRITE with ACK' failed. */
	if (sts & (READ_COMPLETED_WITH_ERR | ACK_WITH_ERR_RCVD)) {
	   ret = -EIO;
	   goto out;
	}

	for (i = 0; i < rx_len; i += 4) {
	   u8 *buf = msg->rx_buf;
	   int j;

	   val = readl(dsi->dsi_base + DIRECT_CMD_RDDATA);
	   for (j = 0; j < 4 && j + i < rx_len; j++)
		   buf[i + j] = val >> (8 * j);
	}

	out:

	return ret;
}

static const struct mipi_dsi_host_ops mipi_dsi_northwest_host_ops = {
	.attach = mipi_dsi_northwest_host_attach,
	.transfer = cdns_dsi_transfer,
};

static int mipi_dsi_northwest_init(struct udevice *dev,
			    struct mipi_dsi_device *device,
			    struct display_timing *timings,
			    unsigned int max_data_lanes,
			    const struct mipi_dsi_phy_ops *phy_ops)
{
	struct mipi_dsi_northwest_info *priv = dev_get_priv(dev);

	if (!phy_ops->init || !phy_ops->get_lane_mbps ||
	    !phy_ops->post_set_mode)
		return -5;

	priv->max_data_lanes = max_data_lanes;
	priv->device = device;
	priv->dsi_host.ops = &mipi_dsi_northwest_host_ops;
	device->host = &priv->dsi_host;

	priv->timings = *timings;
	priv->phy_ops = phy_ops;
	priv->dsi_base = (void *)dev_read_addr_name(device->dev, "dsi");
	priv->phy_base = (void *)dev_read_addr_name(device->dev, "phy");

	if ((fdt_addr_t)priv->dsi_base == FDT_ADDR_T_NONE || (fdt_addr_t)priv->phy_base == FDT_ADDR_T_NONE) {
		printf("dsi dt register address error\n");
		return -EINVAL;
	}
	priv->link_initialized = false;

	return 0;
}

static int mipi_dsi_enable(struct udevice *dev)
{
	struct mipi_dsi_northwest_info *priv = dev_get_priv(dev);

	priv->phy_ops->init(priv->device);
	priv->phy_ops->post_set_mode(priv->device, MIPI_DSI_MODE_VIDEO);
	cdns_dsi_init_link(priv, priv->device);

	writel(0x00670067, priv->dsi_base + 0x000000c0);
	writel(0x00cb0960, priv->dsi_base + 0x000000c4);
	writel(0x0003b145, priv->dsi_base + 0x000000b4);
	writel(0x000001e0, priv->dsi_base + 0x000000b8);
	writel(0x00000a9e, priv->dsi_base + 0x000000d0);
	writel(0x0a980000, priv->dsi_base + 0x000000f8);
	writel(0x00000b0f, priv->dsi_base + 0x000000cc);
	writel(0x7c3c0aae, priv->dsi_base + 0x000000dc);
	writel(0x0032dcd3, priv->dsi_base + 0x00000014);
	writel(0x00032dcd, priv->dsi_base + 0x00000018);
	writel(0x80b8fe00, priv->dsi_base + 0x000000b0);
	writel(0x00020027, priv->dsi_base + 0x00000004);
	writel(0x00004018, priv->dsi_base + 0x0000000c);

	return 0;
}

static int mipi_dsi_northwest_disable(struct udevice *dev)
{
	return 0;
}

struct dsi_host_ops mipi_dsi_northwest_ops = {
	.init = mipi_dsi_northwest_init,
	.enable = mipi_dsi_enable,
	.disable = mipi_dsi_northwest_disable,
};

static int mipi_dsi_northwest_probe(struct udevice *dev)
{
	return 0;
}

static const struct udevice_id mipi_dsi_northwest_ids[] = {
	{ .compatible = "starfive,mipi-dsi" },
	{ }
};

U_BOOT_DRIVER(mipi_dsi_host) = {
	.name			= "mipi_dsi_host",
	.id				= UCLASS_DSI_HOST,
	.of_match		= mipi_dsi_northwest_ids,
	.probe			= mipi_dsi_northwest_probe,
	.remove 		= mipi_dsi_northwest_disable,
	.ops			= &mipi_dsi_northwest_ops,
	.priv_auto	= sizeof(struct mipi_dsi_northwest_info),
};
