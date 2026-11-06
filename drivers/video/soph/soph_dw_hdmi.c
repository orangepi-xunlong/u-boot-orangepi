#include <common.h>
#include <clk.h>
#include <display.h>
#include <dm.h>
#include <drm_modes.h>
#include <edid.h>
#include <log.h>
#include <regmap.h>
#include <syscon.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include "dw_hdmi.h"
#include "soph_dw_hdmi.h"
#include "soph_connector.h"


int soph_hdmi_read_edid(struct udevice *dev, u8 *buf, int buf_size)
{
	return 0;
}

int soph_hdmi_of_to_plat(struct udevice *dev)
{
	return 0;
}

static const struct soph_connector_funcs soph_dw_hdmi_funcs = {
	.init = soph_dw_hdmi_init,
	.deinit = soph_dw_hdmi_deinit,
	.prepare = soph_dw_hdmi_prepare,
	.enable = soph_dw_hdmi_enable,
	.disable = soph_dw_hdmi_disable,
	.get_timing = soph_dw_hdmi_get_timing,
	.detect = soph_dw_hdmi_detect,
	.get_edid = soph_dw_hdmi_get_edid,
};

const struct dw_hdmi_plat_data soph_hdmi_drv_data = {
};

static int soph_dw_hdmi_probe(struct udevice *dev)
{
	int id = 0;
	struct soph_connector *conn = dev_get_priv(dev);

	soph_connector_bind(conn, dev, id, &soph_dw_hdmi_funcs, NULL,
				DRM_MODE_CONNECTOR_HDMIA);

	return 0;
}

static const struct udevice_id soph_dw_hdmi_ids[] = {
	{
	 .compatible = "cvitek,dw_hdmi",
	 .data = (ulong)&soph_hdmi_drv_data,
	}
};

U_BOOT_DRIVER(soph_dw_hdmi) = {
	.name = "soph_dw_hdmi",
	.id = UCLASS_DISPLAY,
	.of_match = soph_dw_hdmi_ids,
	.probe	= soph_dw_hdmi_probe,
	.priv_auto = sizeof(struct soph_hdmi_priv),
};
