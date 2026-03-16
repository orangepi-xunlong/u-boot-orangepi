// SPDX-License-Identifier: GPL-2.0+
/*
 * drivers/net/phy/motorcomm.c
 *
 * Driver for Motorcomm PHYs
 *
 * Author: Frank <Frank.Sae@motor-comm.com>
 *
 * Copyright (c) 2019 Motorcomm, Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the  License, or (at your
 * option) any later version.
 *
 * Support : Motorcomm Phys:
 *        Giga phys: yt8511, yt8521, yt8531, yt8614, yt8618
 *        100/10 Phys : yt8512, yt8512b, yt8510
 *        Automotive 100Mb Phys : yt8010
 *        Automotive 100/10 hyper range Phys: yt8510
 */
#include <common.h>
#include <phy.h>
#include <linux/compat.h>
#include <malloc.h>

#define YT_UBOOT_MAJOR                  2
#define YT_UBOOT_MINOR  	            2
#define YT_UBOOT_SUBVERSION             9918
#define YT_UBOOT_VERSIONID              "2.2.9918"


#define PHY_ID_YT8010                   0x00000309
#define PHY_ID_YT8010AS                 0x4f51eb19
#define PHY_ID_YT8510                   0x00000109
#define PHY_ID_YT8511                   0x0000010a
#define PHY_ID_YT8512                   0x00000118
#define PHY_ID_YT8512B                  0x00000128
#define PHY_ID_YT8521S                  0x0000011a
#define PHY_ID_YT8531                   0x4f51e91b
#define PHY_ID_YT8531S                  0x4f51e91a
#define PHY_ID_YT8614                   0x4F51E899
#define PHY_ID_YT8618                   0x0000e889
#define PHY_ID_YT8821                   0x4f51ea19
#define PHY_ID_MASK                     0xffffffff

/* for YT8531 package A xtal init config */
#define YT8531A_XTAL_INIT               0
/* some GMAC need clock input from PHY, for eg., 125M,
 * please enable this macro
 * by degault, it is set to 0
 * NOTE: this macro will need macro SYS_WAKEUP_BASED_ON_ETH_PKT to set to 1
 */
#define GMAC_CLOCK_INPUT_NEEDED         0
#define YT_861X_AB_VER              	0
#if (YT_861X_AB_VER)
static int yt8614_get_port_from_phydev(struct phy_device *phydev);
#endif

#define REG_PHY_SPEC_STATUS             0x11

#define REG_DEBUG_ADDR_OFFSET           0x1e
#define REG_DEBUG_DATA                  0x1f
#define YT_SPEC_STATUS               	0x11
#define YT_UTP_INTR_REG              	0x12
#define YT_SOFT_RESET                	0x8000
#define YT_SPEED_MODE                	0xc000
#define YT_SPEED_MODE_BIT            	14
#define YT_DUPLEX                    	0x2000
#define YT_DUPLEX_BIT                	13
#define YT_LINK_STATUS_BIT           	10
#define YT_REG_SPACE_UTP             	0
#define YT_REG_SPACE_FIBER           	2

#define YT8512_CLOCK_INIT_NEED          0
#define YT8512_EXTREG_AFE_PLL           0x50
#define YT8512_EXTREG_EXTEND_COMBO      0x4000
#define YT8512_EXTREG_LED0              0x40c0
#define YT8512_EXTREG_LED1              0x40c3
#define YT8512_EXTREG_SLEEP_CONTROL1    0x2027
#define YT8512_CONFIG_PLL_REFCLK_SEL_EN 0x0040
#define YT8512_CONTROL1_RMII_EN         0x0001
#define YT8512_LED0_ACT_BLK_IND         0x1000
#define YT8512_LED0_DIS_LED_AN_TRY      0x0001
#define YT8512_LED0_BT_BLK_EN           0x0002
#define YT8512_LED0_HT_BLK_EN           0x0004
#define YT8512_LED0_COL_BLK_EN          0x0008
#define YT8512_LED0_BT_ON_EN            0x0010
#define YT8512_LED1_BT_ON_EN            0x0010
#define YT8512_LED1_TXACT_BLK_EN        0x0100
#define YT8512_LED1_RXACT_BLK_EN        0x0200
#define YT8512_EN_SLEEP_SW_BIT          15
#define YT8521_EXTREG_SLEEP_CONTROL1    0x27
#define YT8521_EN_SLEEP_SW_BIT          15

/* to enable system WOL feature of phy, please define this macro to 1
 * otherwise, define it to 0.
 */
#define YT_WOL_FEATURE_ENABLE        	0
/* WOL Feature Event Interrupt Enable */
#define YT_WOL_FEATURE_INTR          	BIT(6)

/* Magic Packet MAC address registers */
#define YT_WOL_FEATURE_MACADDR2_4_MAGIC_PACKET    0xa007
#define YT_WOL_FEATURE_MACADDR1_4_MAGIC_PACKET    0xa008
#define YT_WOL_FEATURE_MACADDR0_4_MAGIC_PACKET    0xa009

#define YT_WOL_FEATURE_REG_CFG       0xa00a
/* WOL TYPE Config */
#define YT_WOL_FEATURE_TYPE_CFG      BIT(0)
/* WOL Enable Config */
#define YT_WOL_FEATURE_ENABLE_CFG    BIT(3)
/* WOL Event Interrupt Enable Config */
#define YT_WOL_FEATURE_INTR_SEL_CFG  BIT(6)
/* WOL Pulse Width Config */
#define YT_WOL_FEATURE_WIDTH1_CFG    BIT(1)
/* WOL Pulse Width Config */
#define YT_WOL_FEATURE_WIDTH2_CFG    BIT(2)    

enum yt_wol_feature_trigger_type_e {
	YT_WOL_FEATURE_PULSE_TRIGGER,
	YT_WOL_FEATURE_LEVEL_TRIGGER,
	YT_WOL_FEATURE_TRIGGER_TYPE_MAX
};

enum yt_wol_feature_pulse_width_e {
	YT_WOL_FEATURE_672MS_PULSE_WIDTH,
	YT_WOL_FEATURE_336MS_PULSE_WIDTH,
	YT_WOL_FEATURE_168MS_PULSE_WIDTH,
	YT_WOL_FEATURE_84MS_PULSE_WIDTH,
	YT_WOL_FEATURE_PULSE_WIDTH_MAX
};

struct yt_wol_feature_cfg {
	bool enable;
	int type;
	int width;
};

/* polling mode */
#define PHY_MODE_FIBER           1 //fiber mode only
#define PHY_MODE_UTP             2 //utp mode only
#define PHY_MODE_POLL            3 //fiber and utp, poll mode

#define msleep(n)                udelay(n * 1000)

struct yt8xxx_private {
	int strap_polling;
	int reserve;
};

static int yt_read_ext(struct phy_device *phydev, u32 regnum)
{
	int ret;
	int val;

	ret = phy_write(phydev, MDIO_DEVAD_NONE, REG_DEBUG_ADDR_OFFSET, regnum);
	if (ret < 0)
		return ret;

	val = phy_read(phydev, MDIO_DEVAD_NONE, REG_DEBUG_DATA);

	return val;
}

static int yt_write_ext(struct phy_device *phydev, u32 regnum, u16 val)
{
	int ret;

	ret = phy_write(phydev, MDIO_DEVAD_NONE, REG_DEBUG_ADDR_OFFSET, regnum);
	if (ret < 0)
		return ret;

	ret = phy_write(phydev, MDIO_DEVAD_NONE, REG_DEBUG_DATA, val);

	return ret;
}

static int yt_soft_reset(struct phy_device *phydev)
{
	int ret = 0, val = 0;

	val = phy_read(phydev, MDIO_DEVAD_NONE, MII_BMCR);
	if (val < 0)
		return val;

	ret = phy_write(phydev, MDIO_DEVAD_NONE, MII_BMCR, val | BMCR_RESET);
	if (ret < 0)
		return ret;

	return ret;
}

#if YT8512_CLOCK_INIT_NEED
static int yt8512_clk_init(struct phy_device *phydev)
{
	int ret;
	int val;

	val = yt_read_ext(phydev, YT8512_EXTREG_AFE_PLL);
	if (val < 0)
		return val;

	val |= YT8512_CONFIG_PLL_REFCLK_SEL_EN;

	ret = yt_write_ext(phydev, YT8512_EXTREG_AFE_PLL, val);
	if (ret < 0)
		return ret;

	val = yt_read_ext(phydev, YT8512_EXTREG_EXTEND_COMBO);
	if (val < 0)
		return val;

	val |= YT8512_CONTROL1_RMII_EN;

	ret = yt_write_ext(phydev, YT8512_EXTREG_EXTEND_COMBO, val);
	if (ret < 0)
		return ret;

	val = phy_read(phydev, MDIO_DEVAD_NONE, MII_BMCR);
	if (val < 0)
		return val;

	val |= YT_SOFT_RESET;
	ret = phy_write(phydev, MII_BMCR, val);

	return ret;
}
#endif

static int yt8512_led_init(struct phy_device *phydev)
{
	int ret;
	int val;
	int mask;

	val = yt_read_ext(phydev, YT8512_EXTREG_LED0);
	if (val < 0)
		return val;

	val |= YT8512_LED0_ACT_BLK_IND;

	mask = YT8512_LED0_DIS_LED_AN_TRY | YT8512_LED0_BT_BLK_EN |
	YT8512_LED0_HT_BLK_EN | YT8512_LED0_COL_BLK_EN |
	YT8512_LED0_BT_ON_EN;
	val &= ~mask;

	ret = yt_write_ext(phydev, YT8512_EXTREG_LED0, val);
	if (ret < 0)
		return ret;

	val = yt_read_ext(phydev, YT8512_EXTREG_LED1);
	if (val < 0)
		return val;

	val |= YT8512_LED1_BT_ON_EN;

	mask = YT8512_LED1_TXACT_BLK_EN | YT8512_LED1_RXACT_BLK_EN;
	val &= ~mask;

	ret = yt_write_ext(phydev, YT8512_LED1_BT_ON_EN, val);

	return ret;
}

#if (YT8531A_XTAL_INIT)
static int yt8531a_xtal_init(struct phy_device *phydev)
{
	int ret = 0;
	int val = 0;

	udelay(50000);		/* delay 50ms */

	do {
		ret = yt_write_ext(phydev, 0xa012, 0x88);
		if (ret < 0)
			return ret;

		udelay(100000);	/* delay 100ms */

		val = yt_read_ext(phydev, 0xa012);
		if (val < 0)
			return val;
	} while (val != 0x88);

	ret = yt_write_ext(phydev, 0xa012, 0xc8);
	if (ret < 0)
		return ret;

	return ret;
}
#endif

#if (YT_WOL_FEATURE_ENABLE)
static int yt_switch_reg_space(struct phy_device *phydev, int space)
{
	int ret;

	if (space == YT_REG_SPACE_UTP)
		ret = yt_write_ext(phydev, 0xa000, 0);
	else
		ret = yt_write_ext(phydev, 0xa000, 2);

	return ret;
}

static int yt_wol_feature_enable_cfg(struct phy_device *phydev,
			struct yt_wol_feature_cfg wol_cfg)
{
	int ret = 0;
	int val = 0;

	val = yt_read_ext(phydev, YT_WOL_FEATURE_REG_CFG);
	if (val < 0)
		return val;

	if (wol_cfg.enable) {
		val |= YT_WOL_FEATURE_ENABLE_CFG;

	if (wol_cfg.type == YT_WOL_FEATURE_LEVEL_TRIGGER) {
		val &= ~YT_WOL_FEATURE_TYPE_CFG;
		val &= ~YT_WOL_FEATURE_INTR_SEL_CFG;
	} else if (wol_cfg.type == YT_WOL_FEATURE_PULSE_TRIGGER) {
		val |= YT_WOL_FEATURE_TYPE_CFG;
		val |= YT_WOL_FEATURE_INTR_SEL_CFG;

		if (wol_cfg.width == YT_WOL_FEATURE_84MS_PULSE_WIDTH) {
			val &= ~YT_WOL_FEATURE_WIDTH1_CFG;
			val &= ~YT_WOL_FEATURE_WIDTH2_CFG;
		} else if (wol_cfg.width == YT_WOL_FEATURE_168MS_PULSE_WIDTH) {
			val |= YT_WOL_FEATURE_WIDTH1_CFG;
			val &= ~YT_WOL_FEATURE_WIDTH2_CFG;
		} else if (wol_cfg.width == YT_WOL_FEATURE_336MS_PULSE_WIDTH) {
			val &= ~YT_WOL_FEATURE_WIDTH1_CFG;
			val |= YT_WOL_FEATURE_WIDTH2_CFG;
		} else if (wol_cfg.width == YT_WOL_FEATURE_672MS_PULSE_WIDTH) {
			val |= YT_WOL_FEATURE_WIDTH1_CFG;
			val |= YT_WOL_FEATURE_WIDTH2_CFG;
		}
	}
	} else {
		val &= ~YT_WOL_FEATURE_ENABLE_CFG;
		val &= ~YT_WOL_FEATURE_INTR_SEL_CFG;
	}

	ret = yt_write_ext(phydev, YT_WOL_FEATURE_REG_CFG, val);
	if (ret < 0)
		return ret;

	return 0;
}

static void yt_wol_feature_get(struct phy_device *phydev,
				struct ethtool_wolinfo *wol)
{
	int val = 0;

	wol->supported = WAKE_MAGIC;
	wol->wolopts = 0;

	val = yt_read_ext(phydev, YT_WOL_FEATURE_REG_CFG);
	if (val < 0)
		return;

	if (val & YT_WOL_FEATURE_ENABLE_CFG)
		wol->wolopts |= WAKE_MAGIC;
}

static int yt_wol_feature_set(struct phy_device *phydev,
				struct ethtool_wolinfo *wol)
{
	int ret, curr_reg_space, val;
	struct yt_wol_feature_cfg wol_cfg;
	struct eth_device *p_attached_dev = phydev->dev;

	memset(&wol_cfg, 0, sizeof(struct yt_wol_feature_cfg));
	curr_reg_space = yt_read_ext(phydev, 0xa000);
	if (curr_reg_space < 0)
		return curr_reg_space;

	/* Switch to phy UTP page */
	ret = yt_switch_reg_space(phydev, YT_REG_SPACE_UTP);
	if (ret < 0)
		return ret;

	if (wol->wolopts & WAKE_MAGIC) {
		/* Enable the WOL feature interrupt */
		val = phy_read(phydev, MDIO_DEVAD_NONE, YT_UTP_INTR_REG);
		val |= YT_WOL_FEATURE_INTR;
		ret = phy_write(phydev, YT_UTP_INTR_REG, val);
		if (ret < 0)
			return ret;

		/* Set the WOL feature config */
		wol_cfg.enable = true;
		wol_cfg.type = YT_WOL_FEATURE_PULSE_TRIGGER;
		wol_cfg.width = YT_WOL_FEATURE_672MS_PULSE_WIDTH;
		ret = yt_wol_feature_enable_cfg(phydev, wol_cfg);
		if (ret < 0)
			return ret;

		/* Store the device address for the magic packet */
		ret = yt_write_ext(phydev, YT_WOL_FEATURE_MACADDR2_4_MAGIC_PACKET,
			((p_attached_dev->enetaddr[0] << 8) |
			p_attached_dev->enetaddr[1]));
		if (ret < 0)
			return ret;
		ret = yt_write_ext(phydev, YT_WOL_FEATURE_MACADDR1_4_MAGIC_PACKET,
			((p_attached_dev->enetaddr[2] << 8) |
			p_attached_dev->enetaddr[3]));
		if (ret < 0)
			return ret;
		ret = yt_write_ext(phydev, YT_WOL_FEATURE_MACADDR0_4_MAGIC_PACKET,
			((p_attached_dev->enetaddr[4] << 8) |
			p_attached_dev->enetaddr[5]));
		if (ret < 0)
			return ret;
	} else {
		wol_cfg.enable = false;
		wol_cfg.type = YT_WOL_FEATURE_TRIGGER_TYPE_MAX;
		wol_cfg.width = YT_WOL_FEATURE_PULSE_WIDTH_MAX;
		ret = yt_wol_feature_enable_cfg(phydev, wol_cfg);
		if (ret < 0)
			return ret;
	}

	/* Recover to previous register space page */
	ret = yt_switch_reg_space(phydev, curr_reg_space);
	if (ret < 0)
		return ret;

	return 0;
}
#endif /*(YT_WOL_FEATURE_ENABLE)*/

static int yt_parse_status(struct phy_device *phydev)
{
	int val;
	int speed, speed_mode, duplex;
	
	val = phy_read(phydev, MDIO_DEVAD_NONE, YT_SPEC_STATUS);
	if (val < 0)
		return val;

	duplex = (val & YT_DUPLEX) >> YT_DUPLEX_BIT;
	speed_mode = (val & YT_SPEED_MODE) >> YT_SPEED_MODE_BIT;
	switch (speed_mode) {
	case 2:
		speed = SPEED_1000;
		break;
	case 1:
		speed = SPEED_100;
		break;
	default:
		speed = SPEED_10;
		break;
	}

	phydev->speed = speed;
	phydev->duplex = duplex;

	return 0;
}

static int yt8821_parse_status(struct phy_device *phydev)
{
	int val;
	int speed, speed_mode, duplex;
	int speed_mode_bit15_14, speed_mode_bit9;
	
	val = phy_read(phydev, MDIO_DEVAD_NONE, YT_SPEC_STATUS);
	if (val < 0)
		return val;

	duplex = (val & YT_DUPLEX) >> YT_DUPLEX_BIT;

	/* Bit9-Bit15-Bit14 speed mode 100---2.5G; 010---1000M; 
	 *                             001---100M; 000---10M 
	 */
	speed_mode_bit15_14 = (val & YT_SPEED_MODE) >> YT_SPEED_MODE_BIT;
	speed_mode_bit9 = (val & BIT(9)) >> 9;
	speed_mode = (speed_mode_bit9 << 2) | speed_mode_bit15_14;

	switch (speed_mode) {
	case 4:
		speed = SPEED_2500;
		break;
	case 2:
		speed = SPEED_1000;
		break;
	case 1:
		speed = SPEED_100;
		break;
	default:
		speed = SPEED_10;
		break;
	}

	phydev->speed = speed;
	phydev->duplex = duplex;

	return 0;
}

static int yt_startup(struct phy_device *phydev)
{
	msleep(1000);
	genphy_update_link(phydev);
	yt_parse_status(phydev);
	
	return 0;
}

static int yt8010_config(struct phy_device *phydev)
{
	yt_soft_reset(phydev);

	phydev->autoneg = AUTONEG_DISABLE;

	return 0;
}

int yt8010AS_soft_reset(struct phy_device *phydev)
{
	int ret = 0;

	/* sgmii */
	yt_write_ext(phydev, 0xe, 1);
	ret = yt_soft_reset(phydev);
	if (ret < 0) {
		yt_write_ext(phydev, 0xe, 0);
		return ret;
	}

	/* utp */
	yt_write_ext(phydev, 0xe, 0);
	ret = yt_soft_reset(phydev);
	if (ret < 0)
		return ret;

	return 0;
}

static int yt8010AS_config(struct phy_device *phydev)
{
	
	yt8010AS_soft_reset(phydev);
	
	phydev->autoneg = AUTONEG_DISABLE;

	return 0;
}

static int yt8010_startup(struct phy_device *phydev)
{
	msleep(1000);

	genphy_update_link(phydev);
	
	phydev->speed = SPEED_100;
	phydev->duplex = DUPLEX_FULL;
	
	return 0;
}

static int yt8510_config(struct phy_device *phydev)
{
	yt_soft_reset(phydev);
	
	return 0;
}


static int yt8511_config(struct phy_device *phydev)
{
	yt_soft_reset(phydev);
	
	genphy_config_aneg(phydev);
	
	return 0;
}

static int yt8512_config(struct phy_device *phydev)
{
	int ret = 0, val = 0;
	
	yt_soft_reset(phydev);

#if YT8512_CLOCK_INIT_NEED
	ret = yt8512_clk_init(phydev);
	if (ret < 0)
		return ret;
#endif

	ret = yt8512_led_init(phydev);

	/* disable auto sleep */
	val = yt_read_ext(phydev, YT8512_EXTREG_SLEEP_CONTROL1);
	if (val < 0)
		return val;

	val &= (~BIT(YT8512_EN_SLEEP_SW_BIT));

	ret = yt_write_ext(phydev, YT8512_EXTREG_SLEEP_CONTROL1, val);
	if (ret < 0)
		return ret;
	
	genphy_config_aneg(phydev);

	return 0;
}

static int yt8521_hw_strap_polling(struct phy_device *phydev)
{
	int val = 0;

	val = yt_read_ext(phydev, 0xa001) & 0x7;
	switch (val) {
	case 1:
	case 4:
	case 5:
		return PHY_MODE_FIBER;
	case 2:
	case 6:
	case 7:
		return PHY_MODE_POLL;
	case 3:
	case 0:
	default:
		return PHY_MODE_UTP;
	}
}

static int yt8521S_probe(struct phy_device *phydev)
{
	struct yt8xxx_private *yt8521S;
	
	if (!phydev->priv) {
		yt8521S = kzalloc(sizeof(*yt8521S), GFP_KERNEL);
		if (!yt8521S)
			return -ENOMEM;	//ref: net/phy/ti.c

		phydev->priv = yt8521S;
		yt8521S->strap_polling = yt8521_hw_strap_polling(phydev);
	} else {
		yt8521S = (struct yt8xxx_private *)phydev->priv;
	}


	return 0;
}

static int yt8521S_config(struct phy_device *phydev)
{
	int ret;
	int val, hw_strap_mode;

#if (YT_WOL_FEATURE_ENABLE)
	struct ethtool_wolinfo wol;

	/* set phy wol enable */
	memset(&wol, 0x0, sizeof(struct ethtool_wolinfo));
	wol.wolopts |= WAKE_MAGIC;
	yt_wol_feature_set(phydev, &wol);
#endif

	/* soft reset */
	if (phydev->priv) {
		struct yt8xxx_private *yt8521S = phydev->priv;
		switch (yt8521S->strap_polling) {
		case PHY_MODE_POLL:
			yt_write_ext(phydev, 0xa000, 0);
			yt_soft_reset(phydev);
			yt_write_ext(phydev, 0xa000, 2);
			yt_soft_reset(phydev);
			break;
		case PHY_MODE_FIBER:
			yt_write_ext(phydev, 0xa000, 2);
			yt_soft_reset(phydev);
			break;
		case PHY_MODE_UTP:
		default:
			yt_write_ext(phydev, 0xa000, 0);
			yt_soft_reset(phydev);
			break;
		}
	}

	hw_strap_mode = yt_read_ext(phydev, 0xa001) & 0x7;
	printf("hw_strap_mode: 0x%x\n", hw_strap_mode);

	yt_write_ext(phydev, 0xa000, 0);

	/* disable auto sleep */
	val = yt_read_ext(phydev, YT8521_EXTREG_SLEEP_CONTROL1);
	if (val < 0)
		return val;

	val &= (~BIT(YT8521_EN_SLEEP_SW_BIT));

	ret = yt_write_ext(phydev, YT8521_EXTREG_SLEEP_CONTROL1, val);
	if (ret < 0)
		return ret;

	/* enable RXC clock when no wire plug */
	val = yt_read_ext(phydev, 0xc);
	if (val < 0)
		return val;
	val &= ~(1 << 12);
	ret = yt_write_ext(phydev, 0xc, val);
	if (ret < 0)
		return ret;

	genphy_config_aneg(phydev);

	return ret;
}

/* for fiber mode, there is no 10M speed mode and
 * this function is for this purpose.
 */
static int yt8521_adjust_status(struct phy_device *phydev, int val, int is_utp)
{
	int speed_mode, duplex, speed;

	if (is_utp)
		duplex = (val & YT_DUPLEX) >> YT_DUPLEX_BIT;
	else
		duplex = 1;
	speed_mode = (val & YT_SPEED_MODE) >> YT_SPEED_MODE_BIT;
	switch (speed_mode) {
	case 2:
		speed = SPEED_1000;
		break;
	case 1:
		speed = SPEED_100;
		break;
	default:
		speed = SPEED_10;
		break;
	}

	if (!is_utp) {
		if (speed == SPEED_10)
			speed = SPEED_100;
	}
		
	phydev->speed = speed;
	phydev->duplex = duplex;

	return 0;
}

static int yt8521S_startup(struct phy_device *phydev)
{
	struct yt8xxx_private *yt8521S = phydev->priv;

	int ret = 0;
	int val = 0;
	int yt8521_fiber_latch_val;
	int yt8521_fiber_curr_val;
	int link;
	int link_utp = 0, link_fiber = 0;

	msleep(1000);
	if (yt8521S->strap_polling != PHY_MODE_FIBER) {
		/* reading UTP */
		ret = yt_write_ext(phydev, 0xa000, 0);
		if (ret < 0)
			return ret;

		val = phy_read(phydev, MDIO_DEVAD_NONE, REG_PHY_SPEC_STATUS);
		if (val < 0)
			return val;

		link = val & (BIT(YT_LINK_STATUS_BIT));
		if (link) {
			link_utp = 1;
			yt8521_adjust_status(phydev, val, 1);
		} else {
			link_utp = 0;
		}
	}

	if (yt8521S->strap_polling != PHY_MODE_UTP) {
		/* reading Fiber */
		ret = yt_write_ext(phydev, 0xa000, 2);
		if (ret < 0)
			return ret;

		val = phy_read(phydev, MDIO_DEVAD_NONE, REG_PHY_SPEC_STATUS);
		if (val < 0)
			return val;

		/* for fiber, from 1000m to 100m, there is not link down from 0x11,
		 * and check reg 1 to identify such case this is important for Linux
		 * kernel for that, missing linkdown event will cause problem.
		 */
		yt8521_fiber_latch_val = phy_read(phydev, MDIO_DEVAD_NONE, MII_BMSR);
		yt8521_fiber_curr_val = phy_read(phydev, MDIO_DEVAD_NONE, MII_BMSR);
		link = val & (BIT(YT_LINK_STATUS_BIT));
		if (link && yt8521_fiber_latch_val != yt8521_fiber_curr_val) {
			link = 0;
			printf("%s, phy addr: %d, fiber link down detect, latch = %04x, curr = %04x\n",
				__func__, phydev->addr, yt8521_fiber_latch_val, yt8521_fiber_curr_val);
		}

		if (link) {
			link_fiber = 1;
			yt8521_adjust_status(phydev, val, 0);
		} else {
			link_fiber = 0;
		}
	}

	if (link_utp || link_fiber) {
		if (phydev->link == 0)
			printf("%s, phy addr: %d, link up, media: %s, mii reg 0x11 = 0x%x\n",
				__func__, phydev->addr, (link_utp && link_fiber) ? "UNKONWN MEDIA" : 
				(link_utp ? "UTP" : "Fiber"), (unsigned int)val);
		phydev->link = 1;
	} else {
		if (phydev->link == 1)
			printf("%s, phy addr: %d, link down\n", __func__, phydev->addr);
		phydev->link = 0;
	}

	if (yt8521S->strap_polling != PHY_MODE_FIBER) {	//utp or combo
		if (link_fiber)
			yt_write_ext(phydev, 0xa000, 2);
		if (link_utp)
			yt_write_ext(phydev, 0xa000, 0);
	}
	return 0;
}

static int yt8531_config(struct phy_device *phydev)
{
#if (YT8531A_XTAL_INIT)
	int ret = 0;
	ret = yt8531a_xtal_init(phydev);
	if (ret < 0)
		return ret;
#endif

	yt_soft_reset(phydev);

#if (YT_WOL_FEATURE_ENABLE)
	struct ethtool_wolinfo wol;

	/* set phy wol enable */
	memset(&wol, 0x0, sizeof(struct ethtool_wolinfo));
	wol.wolopts |= WAKE_MAGIC;
	yt_wol_feature_set(phydev, &wol);
#endif

	genphy_config_aneg(phydev);

	return 0;
}

static int yt8531S_config(struct phy_device *phydev)
{
#if (YT8531A_XTAL_INIT)
	int ret = 0;
	ret = yt8531a_xtal_init(phydev);
	if (ret < 0)
		return ret;
#endif

	yt8521S_config(phydev);

	return 0;
}

static int yt8614_hw_strap_polling(struct phy_device *phydev)
{
	int val = 0;

	/* b3:0 are work mode, but b3 is always 1 */
	val = yt_read_ext(phydev, 0xa007) & 0xf;
	switch (val) {
	case 8:
	case 12:
	case 13:
	case 15:
		return (PHY_MODE_FIBER | PHY_MODE_UTP);
	case 14:
	case 11:
		return PHY_MODE_FIBER;
	case 9:
	case 10:
	default:
		return PHY_MODE_UTP;
	}
}

static int yt8614_probe(struct phy_device *phydev)
{
	struct yt8xxx_private *yt8614;
	
	if (!phydev->priv) {
		yt8614 = kzalloc(sizeof(*yt8614), GFP_KERNEL);
		if (!yt8614)
			return -ENOMEM;

		phydev->priv = yt8614;
		yt8614->strap_polling = yt8614_hw_strap_polling(phydev);
	} else {
		yt8614 = (struct yt8xxx_private *)phydev->priv;
	}

	return 0;
}

int yt8614_soft_reset(struct phy_device *phydev)
{
	int ret;

	/* qsgmii */
	yt_write_ext(phydev, 0xa000, 2);
	ret = yt_soft_reset(phydev);
	if (ret < 0) {
		yt_write_ext(phydev, 0xa000, 0);
		return ret;
	}

	/* sgmii */
	yt_write_ext(phydev, 0xa000, 3);
	ret = yt_soft_reset(phydev);
	if (ret < 0) {
		yt_write_ext(phydev, 0xa000, 0);
		return ret;
	}

	/* utp */
	yt_write_ext(phydev, 0xa000, 0);
	ret = yt_soft_reset(phydev);
	if (ret < 0)
		return ret;

	return 0;
}
#if (YT_861X_AB_VER)
static int yt8614_get_port_from_phydev(struct phy_device *phydev)
{
	int tmp = yt_read_ext(phydev, 0xa0ff);
	int phy_addr = (unsigned int)phydev->addr;

	if ((phy_addr - tmp) < 0) {
		yt_write_ext(phydev, 0xa0ff, phy_addr);
		tmp = phy_addr;
	}

	return (phy_addr - tmp);
}
#endif

static int yt8614_config(struct phy_device *phydev)
{
	int ret = 0;
	int val, hw_strap_mode;
	unsigned int retries = 12;
#if (YT_861X_AB_VER)
	int port = 0;
#endif

	yt8614_soft_reset(phydev);

	/* NOTE: this function should not be called more than one for each chip. */
	hw_strap_mode = yt_read_ext(phydev, 0xa007) & 0xf;

#if (YT_861X_AB_VER)
	port = yt8614_get_port_from_phydev(phydev);
#endif

	yt_write_ext(phydev, 0xa000, 0);

	/* for utp to optimize signal */
	ret = yt_write_ext(phydev, 0x41, 0x33);
	if (ret < 0)
		return ret;
	ret = yt_write_ext(phydev, 0x42, 0x66);
	if (ret < 0)
		return ret;
	ret = yt_write_ext(phydev, 0x43, 0xaa);
	if (ret < 0)
		return ret;
	ret = yt_write_ext(phydev, 0x44, 0xd0d);
	if (ret < 0)
		return ret;

#if (YT_861X_AB_VER)
	if (port == 2) {
		ret = yt_write_ext(phydev, 0x57, 0x2929);
		if (ret < 0)
			return ret;
	}
#endif

	/* soft reset to take config effect */
	val = phy_read(phydev, MDIO_DEVAD_NONE, MII_BMCR);
	phy_write(phydev, MDIO_DEVAD_NONE, MII_BMCR, val | BMCR_RESET);
	do {
		msleep(50);
		ret = phy_read(phydev, MDIO_DEVAD_NONE, MII_BMCR);
		if (ret < 0)
			return ret;
	} while ((ret & BMCR_RESET) && --retries);
	if (ret & BMCR_RESET)
		return -ETIMEDOUT;

	/* for QSGMII optimization */
	yt_write_ext(phydev, 0xa000, 0x02);
	ret = yt_write_ext(phydev, 0x3, 0x4F80);
	if (ret < 0) {
		yt_write_ext(phydev, 0xa000, 0);
		return ret;
	}
	ret = yt_write_ext(phydev, 0xe, 0x4F80);
	if (ret < 0) {
		yt_write_ext(phydev, 0xa000, 0);
		return ret;
	}

	/* for SGMII optimization */
	yt_write_ext(phydev, 0xa000, 0x03);
	ret = yt_write_ext(phydev, 0x3, 0x2420);
	if (ret < 0) {
		yt_write_ext(phydev, 0xa000, 0);
		return ret;
	}
	ret = yt_write_ext(phydev, 0xe, 0x24a0);
	if (ret < 0) {
		yt_write_ext(phydev, 0xa000, 0);
		return ret;
	}

	/* back up to utp*/
	yt_write_ext(phydev, 0xa000, 0);

	printf("%s done, phy addr: %d, chip mode: %d\n", 
		__func__, phydev->addr, hw_strap_mode);
	
	genphy_config_aneg(phydev);

	return ret;
}

static int yt8614_startup(struct phy_device *phydev)
{
	struct yt8xxx_private *yt8614 = phydev->priv;

	int ret;
	int val, yt8614_fiber_latch_val, yt8614_fiber_curr_val;
	int link;
	int link_utp = 0, link_fiber = 0;
	
	msleep(1000);

	if (yt8614->strap_polling & PHY_MODE_UTP) {
		/* switch to utp and reading regs  */
		ret = yt_write_ext(phydev, 0xa000, 0);
		if (ret < 0)
			return ret;

		val = phy_read(phydev, MDIO_DEVAD_NONE, REG_PHY_SPEC_STATUS);
		if (val < 0)
			return val;

		link = val & (BIT(YT_LINK_STATUS_BIT));
		if (link) {
			if (phydev->link == 0)
				printf("%s, phy addr: %d, link up, UTP, reg 0x11 = 0x%x\n",
					__func__, phydev->addr, (unsigned int)val);
			link_utp = 1;
			// here is same as 8521 and re-use the function;
			yt8521_adjust_status(phydev, val, 1);
		} else {
			link_utp = 0;
		}
	}

	if (yt8614->strap_polling & PHY_MODE_FIBER) {
		/* reading Fiber/sgmii */
		ret = yt_write_ext(phydev, 0xa000, 3);
		if (ret < 0)
			return ret;

		val = phy_read(phydev, MDIO_DEVAD_NONE, REG_PHY_SPEC_STATUS);
		if (val < 0)
			return val;

		/* for fiber, from 1000m to 100m, there is not link down from 0x11,
		 * and check reg 1 to identify such case
		 */
		yt8614_fiber_latch_val = phy_read(phydev, MDIO_DEVAD_NONE, MII_BMSR);
		yt8614_fiber_curr_val = phy_read(phydev, MDIO_DEVAD_NONE, MII_BMSR);
		link = val & (BIT(YT_LINK_STATUS_BIT));
		if (link && yt8614_fiber_latch_val != yt8614_fiber_curr_val) {
			link = 0;
			printf("%s, phy addr: %d, fiber link down detect, latch = %04x, curr = %04x\n",
				__func__, phydev->addr, yt8614_fiber_latch_val, yt8614_fiber_curr_val);
		}

		if (link) {
			if (phydev->link == 0)
				printf("%s, phy addr: %d, link up, Fiber, reg 0x11 = 0x%x\n",
					__func__, phydev->addr, (unsigned int)val);
			link_fiber = 1;
			yt8521_adjust_status(phydev, val, 0);
		} else {
			link_fiber = 0;
		}
	}

	if (link_utp || link_fiber) {
		if (phydev->link == 0)
			printf("%s, phy addr: %d, link up, media %s\n", __func__, 
				phydev->addr, (link_utp && link_fiber) ? "both UTP and Fiber" : 
				(link_utp ? "UTP" : "Fiber"));
		phydev->link = 1;
	} else {
		if (phydev->link == 1)
			printf("%s, phy addr: %d, link down\n", __func__, phydev->addr);
		phydev->link = 0;
	}

	if (yt8614->strap_polling & PHY_MODE_UTP) {
		if (link_utp)
			yt_write_ext(phydev, 0xa000, 0);
	}
	return 0;
}

int yt8618_soft_reset(struct phy_device *phydev)
{
	int ret;
	
	/* qsgmii */
	yt_write_ext(phydev, 0xa000, 2);
	ret = yt_soft_reset(phydev);
	if (ret < 0) {
		yt_write_ext(phydev, 0xa000, 0);
		return ret;
	}

	yt_write_ext(phydev, 0xa000, 0);
	ret = yt_soft_reset(phydev);
	if (ret < 0)
		return ret;

	return 0;
}

static int yt8618_config(struct phy_device *phydev)
{
	int ret;
	int val;
	unsigned int retries = 12;
#if (YT_861X_AB_VER)
	int port = 0;
#endif

	yt8618_soft_reset(phydev);

#if (YT_861X_AB_VER)
	port = yt8614_get_port_from_phydev(phydev);
#endif

	yt_write_ext(phydev, 0xa000, 0);

	/* for utp to optimize signal */
	ret = yt_write_ext(phydev, 0x41, 0x33);
	if (ret < 0)
		return ret;
	ret = yt_write_ext(phydev, 0x42, 0x66);
	if (ret < 0)
		return ret;
	ret = yt_write_ext(phydev, 0x43, 0xaa);
	if (ret < 0)
		return ret;
	ret = yt_write_ext(phydev, 0x44, 0xd0d);
	if (ret < 0)
		return ret;

#if (YT_861X_AB_VER)
	if ((port == 2) || (port == 5)) {
		ret = yt_write_ext(phydev, 0x57, 0x2929);
		if (ret < 0)
			return ret;
	}
#endif

	val = phy_read(phydev, MDIO_DEVAD_NONE, MII_BMCR);
	phy_write(phydev, MDIO_DEVAD_NONE, MII_BMCR, val | BMCR_RESET);
	do {
		msleep(50);
		ret = phy_read(phydev, MDIO_DEVAD_NONE, MII_BMCR);
		if (ret < 0)
			return ret;
	} while ((ret & BMCR_RESET) && --retries);
	if (ret & BMCR_RESET)
		return -ETIMEDOUT;

	/* for QSGMII optimization */
	yt_write_ext(phydev, 0xa000, 0x02);

	ret = yt_write_ext(phydev, 0x3, 0x4F80);
	if (ret < 0) {
		yt_write_ext(phydev, 0xa000, 0);
		return ret;
	}
	ret = yt_write_ext(phydev, 0xe, 0x4F80);
	if (ret < 0) {
		yt_write_ext(phydev, 0xa000, 0);
		return ret;
	}

	yt_write_ext(phydev, 0xa000, 0);

	printf("%s done, phy addr: %d\n", __func__, phydev->addr);

	genphy_config_aneg(phydev);

	return ret;
}

static int yt8618_startup(struct phy_device *phydev)
{
	int ret;
	int val;
	int link;
	int link_utp = 0;

	msleep(1000);
	
	/* switch to utp and reading regs  */
	ret = yt_write_ext(phydev, 0xa000, 0);
	if (ret < 0)
		return ret;

	val = phy_read(phydev, MDIO_DEVAD_NONE, REG_PHY_SPEC_STATUS);
	if (val < 0)
		return val;

	link = val & (BIT(YT_LINK_STATUS_BIT));
	if (link) {
		link_utp = 1;
		yt8521_adjust_status(phydev, val, 1);
	} else {
		link_utp = 0;
	}

	if (link_utp)
		phydev->link = 1;
	else
		phydev->link = 0;

	return 0;
}

static int yt8821_init(struct phy_device *phydev)
{
	int ret = 0, val = 0;

	ret = yt_write_ext(phydev, 0xa000, 0x0);
	if (ret < 0)
		return ret;

	ret = yt_write_ext(phydev, 0x34e, 0x8008);
	if (ret < 0)
		return ret;
	
	ret = yt_write_ext(phydev, 0x4d2, 0x5200);
	if (ret < 0)
		return ret;

	ret = yt_write_ext(phydev, 0x4d3, 0x5200);
	if (ret < 0)
		return ret;
	
	ret = yt_write_ext(phydev, 0x372, 0x5a3c);
	if (ret < 0)
		return ret;

	ret = yt_write_ext(phydev, 0x336, 0xaa0a);
	if (ret < 0)
		return ret;

	ret = yt_write_ext(phydev, 0x340, 0x3022);
	if (ret < 0)
		return ret;

	/* soft reset */
	val = phy_read(phydev, MDIO_DEVAD_NONE, MII_BMCR);
	if (val < 0)
		return val;
	ret = phy_write(phydev, MDIO_DEVAD_NONE, MII_BMCR, val | BMCR_RESET);

	return ret;
}

#define YT8821_CHIP_MODE_AUTO_BX2500_SGMII 	(1)
#define YT8821_CHIP_MODE_FORCE_BX2500 	    (0)
#define YT8821_CHIP_MODE_UTP_TO_FIBER_FORCE (0)
static int yt8821_config(struct phy_device *phydev)
{
	int ret;
	int val;

	/* NOTE: this function should not be called more than one for each chip. */
#if (YT8821_CHIP_MODE_AUTO_BX2500_SGMII)
	ret = yt_write_ext(phydev, 0xa001, 0x0);
	if (ret < 0)
		return ret;
#elif (YT8821_CHIP_MODE_FORCE_BX2500)
	ret = yt_write_ext(phydev, 0xa001, 0x1);
	if (ret < 0)
		return ret;
#elif (YT8821_CHIP_MODE_UTP_TO_FIBER_FORCE)
	ret = yt_write_ext(phydev, 0xa001, 0x5);
	if (ret < 0)
		return ret;
#endif

#if (YT_WOL_FEATURE_ENABLE)
	struct ethtool_wolinfo wol;

	/* set phy wol enable */
	memset(&wol, 0x0, sizeof(struct ethtool_wolinfo));
	wol.wolopts |= WAKE_MAGIC;
	yt_wol_feature_set(phydev, &wol);
#endif

	ret = yt8821_init(phydev);
	if (ret < 0)
		return ret;

	/* disable auto sleep */
	val = yt_read_ext(phydev, YT8521_EXTREG_SLEEP_CONTROL1);
	if (val < 0)
		return val;

	val &= (~BIT(YT8521_EN_SLEEP_SW_BIT));

	ret = yt_write_ext(phydev, YT8521_EXTREG_SLEEP_CONTROL1, val);
	if (ret < 0)
		return ret;
	
	printf("%s done, phy addr: %d\n", __func__, phydev->addr);
	
	genphy_config_aneg(phydev);

	return ret;
}

static int yt8821_startup(struct phy_device *phydev)
{
	msleep(1000);
	genphy_update_link(phydev);
	yt8821_parse_status(phydev);
	
	return 0;
}

static struct phy_driver YT8010_driver = {
	.name          = "YT8010 Automotive Ethernet",
	.uid           = PHY_ID_YT8010,
	.mask          = PHY_ID_MASK,
	.features      = PHY_BASIC_FEATURES,
	.config        = &yt8010_config,
	.startup       = &yt8010_startup,
	.shutdown      = &genphy_shutdown,	
};

static struct phy_driver YT8010AS_driver = {
	.name          = "YT8010AS Automotive Ethernet",
	.uid           = PHY_ID_YT8010AS,
	.mask          = PHY_ID_MASK,
	.features      = PHY_BASIC_FEATURES,
	.config        = &yt8010AS_config,
	.startup       = &yt8010_startup,
	.shutdown      = &genphy_shutdown,	
};

static struct phy_driver YT8510_driver = {
	.name          = "8510 100/10Mb Ethernet",
	.uid           = PHY_ID_YT8510,
	.mask          = PHY_ID_MASK,
	.features      = PHY_BASIC_FEATURES,
	.config        = &yt8510_config,
	.startup       = &yt_startup,
	.shutdown      = &genphy_shutdown,	
};

static struct phy_driver YT8511_driver = {
	.name          = "YT8511 Gigabit Ethernet",
	.uid           = PHY_ID_YT8511,
	.mask          = PHY_ID_MASK,
	.features      = PHY_GBIT_FEATURES,
	.config        = &yt8511_config,
	.startup       = &yt_startup,
	.shutdown      = &genphy_shutdown,	
};

static struct phy_driver YT8512_driver = {
	.name          = "YT8512 Ethernet",
	.uid           = PHY_ID_YT8512,
	.mask          = PHY_ID_MASK,
	.features      = PHY_BASIC_FEATURES,
	.config        = &yt8512_config,
	.startup       = &yt_startup,
	.shutdown      = &genphy_shutdown,	
};

static struct phy_driver YT8512B_driver = {
	.name          = "YT8512B Ethernet",
	.uid           = PHY_ID_YT8512B,
	.mask          = PHY_ID_MASK,
	.features      = PHY_BASIC_FEATURES,
	.config        = &yt8512_config,
	.startup       = &yt_startup,
	.shutdown      = &genphy_shutdown,	
};

static struct phy_driver YT8521S_driver = {
	.name          = "YT8521S Gigabit Ethernet",
	.uid           = PHY_ID_YT8521S,
	.mask          = PHY_ID_MASK,
	.features      = PHY_GBIT_FEATURES,
	.probe         = &yt8521S_probe,
	.config        = &yt8521S_config,
	.startup       = &yt8521S_startup,
	.shutdown      = &genphy_shutdown,	
};

static struct phy_driver YT8531_driver = {
	.name          = "YT8531 Gigabit Ethernet",
	.uid           = PHY_ID_YT8531,
	.mask          = PHY_ID_MASK,
	.features      = PHY_GBIT_FEATURES,
	.config        = &yt8531_config,
	.startup       = &yt_startup,
	.shutdown      = &genphy_shutdown,	
};

static struct phy_driver YT8531S_driver = {
	.name          = "YT8531S Ethernet",
	.uid           = PHY_ID_YT8531S,
	.mask		   = PHY_ID_MASK,
	.features	   = PHY_GBIT_FEATURES,
	.probe		   = &yt8521S_probe,
	.config 	   = &yt8531S_config,
	.startup	   = &yt8521S_startup,
	.shutdown	   = &genphy_shutdown,	
};

static struct phy_driver YT8614_driver = {
	.name          = "YT8614 Ethernet",
	.uid           = PHY_ID_YT8614,
	.mask		   = PHY_ID_MASK,
	.features	   = PHY_GBIT_FEATURES,
	.probe		   = &yt8614_probe,
	.config 	   = &yt8614_config,
	.startup	   = &yt8614_startup,
	.shutdown	   = &genphy_shutdown,	
};

static struct phy_driver YT8618_driver = {
	.name          = "YT8618 Ethernet",
	.uid           = PHY_ID_YT8618,
	.mask		   = PHY_ID_MASK,
	.features	   = PHY_GBIT_FEATURES,
	.config 	   = &yt8618_config,
	.startup	   = &yt8618_startup,
	.shutdown	   = &genphy_shutdown,	
};

static struct phy_driver YT8821_driver = {
	.name          = "YT8821 2.5Gb Ethernet",
	.uid           = PHY_ID_YT8821,
	.mask		   = PHY_ID_MASK,
	.features	   = PHY_GBIT_FEATURES,
	.config 	   = &yt8821_config,
	.startup	   = &yt8821_startup,
	.shutdown	   = &genphy_shutdown,	
};

int phy_yt_init(void)
{
	phy_register(&YT8010_driver);
	phy_register(&YT8010AS_driver);
	phy_register(&YT8510_driver);
	phy_register(&YT8511_driver);
	phy_register(&YT8512_driver);
	phy_register(&YT8512B_driver);
	phy_register(&YT8521S_driver);
	phy_register(&YT8531_driver);
	phy_register(&YT8531S_driver);
	phy_register(&YT8614_driver);
	phy_register(&YT8618_driver);
	phy_register(&YT8821_driver);

	return 0;
}

