// SPDX-License-Identifier: GPL-2.0+
/**********************************************************************
 * main.c
 *
 * USB Core Driver
 * main component function
 ***********************************************************************/
#include "include/debug.h"
#include "include/cvi_usb.h"
#include "include/platform_def.h"
#include <common.h>
#include <linux/delay.h>
#include <mmio.h>
#include <cpu_func.h>

extern int acm_app(void);

void cvi_usb_hw_init(void)
{
	u32 value;
	u32 otp_usb_xtal_phy;
	u32 los_mask;

	value = mmio_read_32(REG_USB_SYS_REG_0114) | (REG_PHY0_PHY_RESET);
	mmio_write_32(REG_USB_SYS_REG_0114, value);
	value = mmio_read_32(SOFT_RSTN_1) & (~REG_SOFT_RESET_X_USB0);
	mmio_write_32(SOFT_RSTN_1, value);

	udelay(50);

	value = mmio_read_32(REG_USB_SYS_REG_010C);
	mmio_write_32(REG_USB_SYS_REG_010C, value | REG_PHY0_REF_SSP_EN);

	value = mmio_read_32(SOFT_RSTN_1) | REG_SOFT_RESET_X_USB0;
	mmio_write_32(SOFT_RSTN_1, value);
	value = mmio_read_32(REG_USB_SYS_REG_0114) & ~REG_PHY0_PHY_RESET;
	mmio_write_32(REG_USB_SYS_REG_0114, value);

	value = mmio_read_32(REG_USB_SYS_REG_0100) | REG_USB0_EN;
	mmio_write_32(REG_USB_SYS_REG_0100, value);

	otp_usb_xtal_phy = mmio_read_32(OTP_USB_XTAL_PHY) & OTP_USB_XTAL_PHY_MASK;
	value &= mmio_read_32(REG_USB_SYS_REG_010C)
				& ~(REG_PHY0_REF_CLKDIV2) & ~(REG_PHY0_FSEL_MSK)
				& ~(REG_PHY0_MPLL_MULTIPLIER_MSK) & ~(REG_PHY0_SSC_REF_CLK_SEL_MSK);
	switch (otp_usb_xtal_phy) {
	case 0x0:	// xtal = 24 MHz
		value |= (0x2A << REG_PHY0_FSEL_POS);
		los_mask = 240;
		break;
	case 0x1:	// xtal = 19.2 MHz
		value |= (0x38 << REG_PHY0_FSEL_POS);
		los_mask = 192;
		break;
	case 0x2:	// xtal = 20 MHz
		value |= (0x31 << REG_PHY0_FSEL_POS);
		los_mask = 200;
		break;
	case 0x3:	// xtal = 40 MHz
		value |= REG_PHY0_REF_CLKDIV2 | (0x31 << REG_PHY0_FSEL_POS);
		los_mask = 200;
		break;
	}
	mmio_write_32(REG_USB_SYS_REG_010C, value);

	value = mmio_read_32(USB_PHY0_TUNE_CTRL_REG1) & ~(REG_USB_PHY0_PCS_RX_LOS_MASK_VAL_MSK);
	value |= (los_mask << REG_USB_PHY0_PCS_RX_LOS_MASK_VAL_POS);
	mmio_write_32(USB_PHY0_TUNE_CTRL_REG1, value);

	NOTICE("%s\n", __func__);
}

/* program starts here */
int cvi_usb_polling(void)
{
	cvi_usb_hw_init();

	acm_app();

	return 0;
}
