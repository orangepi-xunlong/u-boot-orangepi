// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2019 STMicroelectronics - All Rights Reserved
 * Author(s): Yannick Fertre <yannick.fertre@st.com> for STMicroelectronics.
 *            Philippe Cornu <philippe.cornu@st.com> for STMicroelectronics.
 *
 * This rm68200 panel driver is inspired from the Linux Kernel driver
 * drivers/gpu/drm/panel/panel-raydium-rm68200.c.
 */
#include <common.h>
#include <backlight.h>
#include <dm.h>
#include <mipi_dsi.h>
#include <panel.h>
#include <asm/gpio.h>
#include <dm/device_compat.h>
#include <linux/delay.h>
#include <power/regulator.h>
#include <i2c.h>

/* I2C registers of the Atmel microcontroller. */
enum REG_ADDR {
	REG_ID = 0x80,
	REG_PORTA, /* BIT(2) for horizontal flip, BIT(3) for vertical flip */
	REG_PORTB,
	REG_PORTC,
	REG_PORTD,
	REG_POWERON,
	REG_PWM,
	REG_DDRA,
	REG_DDRB,
	REG_DDRC,
	REG_DDRD,
	REG_TEST,
	REG_WR_ADDRL,
	REG_WR_ADDRH,
	REG_READH,
	REG_READL,
	REG_WRITEH,
	REG_WRITEL,
	REG_ID2,
};

/* DSI D-PHY Layer Registers */
#define D0W_DPHYCONTTX 0x0004
#define CLW_DPHYCONTRX 0x0020
#define D0W_DPHYCONTRX 0x0024
#define D1W_DPHYCONTRX 0x0028
#define COM_DPHYCONTRX 0x0038
#define CLW_CNTRL 0x0040
#define D0W_CNTRL 0x0044
#define D1W_CNTRL 0x0048
#define DFTMODE_CNTRL 0x0054

/* DSI PPI Layer Registers */
#define PPI_STARTPPI 0x0104
#define PPI_BUSYPPI 0x0108
#define PPI_LINEINITCNT 0x0110
#define PPI_LPTXTIMECNT 0x0114
#define PPI_CLS_ATMR 0x0140
#define PPI_D0S_ATMR 0x0144
#define PPI_D1S_ATMR 0x0148
#define PPI_D0S_CLRSIPOCOUNT 0x0164
#define PPI_D1S_CLRSIPOCOUNT 0x0168
#define CLS_PRE 0x0180
#define D0S_PRE 0x0184
#define D1S_PRE 0x0188
#define CLS_PREP 0x01A0
#define D0S_PREP 0x01A4
#define D1S_PREP 0x01A8
#define CLS_ZERO 0x01C0
#define D0S_ZERO 0x01C4
#define D1S_ZERO 0x01C8
#define PPI_CLRFLG 0x01E0
#define PPI_CLRSIPO 0x01E4
#define HSTIMEOUT 0x01F0
#define HSTIMEOUTENABLE 0x01F4

/* DSI Protocol Layer Registers */
#define DSI_STARTDSI 0x0204
#define DSI_BUSYDSI 0x0208
#define DSI_LANEENABLE 0x0210
#define DSI_LANEENABLE_CLOCK BIT(0)
#define DSI_LANEENABLE_D0 BIT(1)
#define DSI_LANEENABLE_D1 BIT(2)

#define DSI_LANESTATUS0 0x0214
#define DSI_LANESTATUS1 0x0218
#define DSI_INTSTATUS 0x0220
#define DSI_INTMASK 0x0224
#define DSI_INTCLR 0x0228
#define DSI_LPTXTO 0x0230
#define DSI_MODE 0x0260
#define DSI_PAYLOAD0 0x0268
#define DSI_PAYLOAD1 0x026C
#define DSI_SHORTPKTDAT 0x0270
#define DSI_SHORTPKTREQ 0x0274
#define DSI_BTASTA 0x0278
#define DSI_BTACLR 0x027C

/* DSI General Registers */
#define DSIERRCNT 0x0300
#define DSISIGMOD 0x0304

/* DSI Application Layer Registers */
#define APLCTRL 0x0400
#define APLSTAT 0x0404
#define APLERR 0x0408
#define PWRMOD 0x040C
#define RDPKTLN 0x0410
#define PXLFMT 0x0414
#define MEMWRCMD 0x0418

/* LCDC/DPI Host Registers */
#define LCDCTRL 0x0420
#define HSR 0x0424
#define HDISPR 0x0428
#define VSR 0x042C
#define VDISPR 0x0430
#define VFUEN 0x0434

/* DBI-B Host Registers */
#define DBIBCTRL 0x0440

/* SPI Master Registers */
#define SPICMR 0x0450
#define SPITCR 0x0454

/* System Controller Registers */
#define SYSSTAT 0x0460
#define SYSCTRL 0x0464
#define SYSPLL1 0x0468
#define SYSPLL2 0x046C
#define SYSPLL3 0x0470
#define SYSPMCTRL 0x047C

/* GPIO Registers */
#define GPIOC 0x0480
#define GPIOO 0x0484
#define GPIOI 0x0488

/* I2C Registers */
#define I2CCLKCTRL 0x0490

/* Chip/Rev Registers */
#define IDREG 0x04A0

/* Debug Registers */
#define WCMDQUEUE 0x0500
#define RCMDQUEUE 0x0504


struct rm68200_panel_priv {
	struct udevice *reg;
	struct udevice *backlight;
	struct gpio_desc reset;
};

static const struct display_timing default_timing = {
	.pixelclock.typ		= 29700000,
	.hactive.typ		= 800,
	.hfront_porch.typ	= 90,
	.hback_porch.typ	= 5,
	.hsync_len.typ		= 5,
	.vactive.typ		= 480,
	.vfront_porch.typ	= 60,
	.vback_porch.typ	= 5,
	.vsync_len.typ		= 5,
};

static int seeed_panel_i2c_write(struct udevice *dev, uint addr, uint mask, uint data)
{
	uint8_t valb;
	int err = 0;

	if (mask != 0xff){
		err = dm_i2c_read(dev, addr, &valb, 1);
		if (err)
			return err;
	}
	valb &= ~mask;
	valb |= data;

	err = dm_i2c_write(dev, addr, &valb, 1);
	return err;
}

static int seeed_panel_i2c_read(struct udevice *dev, uint8_t addr, uint8_t *data)
{
	uint8_t valb;
	int err;

	err = dm_i2c_read(dev, addr, &valb, 1);
	if (err)
		return err;

	*data = (int)valb;
	return 0;
}

static void rpi_touchscreen_write(struct udevice *dev, u16 reg, u32 val)
{
	struct mipi_dsi_panel_plat *plat = dev_get_plat(dev);
	struct mipi_dsi_device *device = plat->device;
	int err;

    u8 msg[] = {
        reg,
        reg >> 8,
        val,
        val >> 8,
        val >> 16,
        val >> 24,
    };

	err = mipi_dsi_dcs_write_buffer(device, msg, sizeof(msg));
	if (err < 0)
		dev_err(dev, "MIPI DSI DCS write buffer failed: %d\n", err);

    return;
}

static int rm68200_panel_enable_backlight(struct udevice *dev)
{
	struct mipi_dsi_panel_plat *plat = dev_get_plat(dev);
	struct mipi_dsi_device *device = plat->device;
	int ret;
	int i;
	u8 reg_value = 0;

	ret = mipi_dsi_attach(device);
	if (ret < 0)
		return ret;

	seeed_panel_i2c_write(dev, REG_POWERON, 0xff, 1);

	mdelay(100);
	/* Wait for nPWRDWN to go low to indicate poweron is done. */
	for (i = 0; i < 100; i++) {
		seeed_panel_i2c_read(dev, REG_PORTB, &reg_value);
		if (reg_value & 1)
			break;
	}

	rpi_touchscreen_write(dev, DSI_LANEENABLE,
				DSI_LANEENABLE_CLOCK |
				DSI_LANEENABLE_D0);

	rpi_touchscreen_write(dev,PPI_D0S_CLRSIPOCOUNT, 0x05);
	rpi_touchscreen_write(dev,PPI_D1S_CLRSIPOCOUNT, 0x05);
	rpi_touchscreen_write(dev,PPI_D0S_ATMR, 0x00);
	rpi_touchscreen_write(dev,PPI_D1S_ATMR, 0x00);
	rpi_touchscreen_write(dev,PPI_LPTXTIMECNT, 0x03);

	rpi_touchscreen_write(dev,SPICMR, 0x00);
	rpi_touchscreen_write(dev,LCDCTRL, 0x00100150);
	rpi_touchscreen_write(dev,SYSCTRL, 0x040f);
	mdelay(100);

	rpi_touchscreen_write(dev,PPI_STARTPPI, 0x01);
	rpi_touchscreen_write(dev,DSI_STARTDSI, 0x01);
	mdelay(100);

	/* Turn on the backlight. */
	seeed_panel_i2c_write(dev,REG_PWM,	255, 255);
	mdelay(100);

	/* Default to the same orientation as the closed source
	* firmware used for the panel.  Runtime rotation
	* configuration will be supported using VC4's plane
	* orientation bits.
	*/
	seeed_panel_i2c_write(dev,REG_PORTA,255, BIT(2));
	mdelay(100);


	return 0;
}

static int rm68200_panel_get_display_timing(struct udevice *dev,
					    struct display_timing *timings)
{
	memcpy(timings, &default_timing, sizeof(*timings));
	return 0;
}

static int rm68200_panel_of_to_plat(struct udevice *dev)
{
	return 0;
}

static int rm68200_panel_probe(struct udevice *dev)
{
	struct mipi_dsi_panel_plat *plat = dev_get_plat(dev);

	u8 reg_value = 0;

	/* fill characteristics of DSI data link */
	plat->lanes = 1;
	plat->format = MIPI_DSI_FMT_RGB888;
	plat->mode_flags = MIPI_DSI_MODE_VIDEO |
			   MIPI_DSI_MODE_VIDEO_BURST |
			   MIPI_DSI_MODE_LPM;

	seeed_panel_i2c_read(dev, 0x80, &reg_value);

	debug("%s,reg_value = %d\n", __func__,reg_value);
	switch (reg_value) {
	   case 0xde: /* ver 1 */
	   case 0xc3: /* ver 2 */
	   break;

	   default:
		   debug("Unknown Atmel firmware revision: 0x%02x\n", reg_value);
		   return -ENODEV;
	}

	return 0;
}

static const struct panel_ops rm68200_panel_ops = {
	.enable_backlight = rm68200_panel_enable_backlight,
	.get_display_timing = rm68200_panel_get_display_timing,
};

static const struct udevice_id rm68200_panel_ids[] = {
	{ .compatible = "raydium,rm68200" },
	{ }
};

U_BOOT_DRIVER(rm68200_panel) = {
	.name			  = "rm68200_panel",
	.id			  = UCLASS_PANEL,
	.of_match		  = rm68200_panel_ids,
	.ops			  = &rm68200_panel_ops,
	.of_to_plat	  = rm68200_panel_of_to_plat,
	.probe			  = rm68200_panel_probe,
	.plat_auto	= sizeof(struct mipi_dsi_panel_plat),
	.priv_auto	= sizeof(struct rm68200_panel_priv),
};
