// SPDX-License-Identifier: GPL-2.0+
/*
 * RealTek PHY drivers
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * Copyright 2010-2011 Freescale Semiconductor, Inc.
 * author Andy Fleming
 *
 */

#include <config.h>
#include <common.h>
#include <phy.h>
#include <bitfield.h>

#define REG_PHY_SPEC_STATUS	0x11
#define REG_DEBUG_ADDR_OFFSET	0x1e
#define REG_DEBUG_DATA		0x1f
#define EXTREG_SLEEP_CONTROL	0x27

#define YTPHY_EXTREG_CHIP_CONFIG	0xa001
#define YTPHY_EXTREG_RGMII_CONFIG1	0xa003
#define YTPHY_PAD_DRIVES_STRENGTH_CFG	0xa010
#define YTPHY_DUPLEX		0x2000
#define YTPHY_DUPLEX_BIT	13
#define YTPHY_SPEED_MODE	0xc000
#define YTPHY_SPEED_MODE_BIT	14
#define YTPHY_RGMII_SW_DR_MASK	GENMASK(5, 4)
#define YTPHY_RGMII_RXC_DR_MASK	GENMASK(15, 13)

#define YT8521_EXT_CLK_GATE	0xc
#define YT8521_EN_SLEEP_SW_BIT	15

#define SPEED_UNKNOWN		-1

#define PHY_ID_YT8531                   0x4f51e91b
#define MOTORCOMM_PHY_ID_MASK           0x00000fff

#define YT8512_EXTREG_LED0              0x40c0
#define YT8512_EXTREG_LED1              0x40c3

#define YT8512_EXTREG_SLEEP_CONTROL1    0x2027

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

struct ytphy_reg_field {
	const char	*name;
	const u8	size;	/* Size of the bitfield, in bits */
	const u8	off;	/* Offset from bit 0 */
	const u8	dflt;	/* Default value */
};

static const struct ytphy_reg_field ytphy_dr_grp[] = {
	{ "rgmii_sw_dr", 2, 4, 0x3},
	{ "rgmii_sw_dr_2", 1, 12, 0x0},
	{ "rgmii_sw_dr_rxc", 3, 13, 0x3}
};

static const struct ytphy_reg_field ytphy_rxtxd_grp[] = {
	{ "rx_delay_sel", 4, 10, 0x0 },
	{ "tx_delay_sel_fe", 4, 4, 0xf },
	{ "tx_delay_sel", 4, 0, 0x1 }
};

static const struct ytphy_reg_field ytphy_txinver_grp[] = {
	{ "tx_inverted_1000", 1, 14, 0x0 },
	{ "tx_inverted_100", 1, 14, 0x0 },
	{ "tx_inverted_10", 1, 14, 0x0 }
};

static const struct ytphy_reg_field ytphy_rxden_grp[] = {
	{ "rxc_dly_en", 1, 8, 0x1 }
};

static int ytphy_read_ext(struct phy_device *phydev, u32 regnum)
{
	int ret;

	ret = phy_write(phydev, MDIO_DEVAD_NONE, REG_DEBUG_ADDR_OFFSET, regnum);
	if (ret < 0)
		return ret;

	return phy_read(phydev, MDIO_DEVAD_NONE, REG_DEBUG_DATA);
}

static int ytphy_write_ext(struct phy_device *phydev, u32 regnum, u16 val)
{
	int ret;

	ret = phy_write(phydev, MDIO_DEVAD_NONE, REG_DEBUG_ADDR_OFFSET, regnum);
	if (ret < 0)
		return ret;

	return phy_write(phydev, MDIO_DEVAD_NONE, REG_DEBUG_DATA, val);
}

static int ytphy_parse_status(struct phy_device *phydev)
{
	int val;
	int speed, speed_mode, duplex;

	val = phy_read(phydev, MDIO_DEVAD_NONE, REG_PHY_SPEC_STATUS);
	if (val < 0)
		return val;

	duplex = (val & YTPHY_DUPLEX) >> YTPHY_DUPLEX_BIT;
	speed_mode = (val & YTPHY_SPEED_MODE) >> YTPHY_SPEED_MODE_BIT;
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

static int ytphy_of_inverted(struct phy_device *phydev)
{
	ofnode node;
	u32 val;
	u32 inver_10;
	u32 inver_100;
	u32 inver_1000;

	node = phydev->node;
	if (!ofnode_valid(node)) {
		/* Look for a PHY node under the Ethernet node */
		node = dev_read_subnode(phydev->dev, "ethernet-phy");
	}

	if (!ofnode_valid(node)) /* No node found*/
		return 0;

	val = ytphy_read_ext(phydev, YTPHY_EXTREG_RGMII_CONFIG1);
	inver_10 = ofnode_read_u32_default(node, "tx_inverted_10", 0);
	inver_100 = ofnode_read_u32_default(node, "tx_inverted_100", 0);
	inver_1000 = ofnode_read_u32_default(node, "tx_inverted_1000", 0);

	switch (phydev->speed) {
	case SPEED_1000:
		val = bitfield_replace(val, ytphy_txinver_grp[0].off,
				ytphy_txinver_grp[0].size, inver_1000);
		break;
	case SPEED_100:
		val = bitfield_replace(val, ytphy_txinver_grp[1].off,
				ytphy_txinver_grp[1].size, inver_100);
		break;
	case SPEED_10:
		val = bitfield_replace(val, ytphy_txinver_grp[2].off,
				ytphy_txinver_grp[2].size, inver_10);
		break;
	default:
		printf("UNKOWN SPEED\n");
		break;
	}

	return ytphy_write_ext(phydev, YTPHY_EXTREG_RGMII_CONFIG1, val);
}

static int ytphy_startup(struct phy_device *phydev)
{
	int retval;

	retval = genphy_update_link(phydev);
	if (retval)
		return retval;
	ytphy_parse_status(phydev);

	ytphy_of_inverted(phydev);

	return 0;
}

static int ytphy_of_config(struct phy_device *phydev)
{
	ofnode node;
	u32 val;
	u32 cfg;
	int i;

	node = phydev->node;
	if (!ofnode_valid(node)) {
		/* Look for a PHY node under the Ethernet node */
		node = dev_read_subnode(phydev->dev, "ethernet-phy");
	}

	if (!ofnode_valid(node)) /* No node found*/
		return 0;

	/*read rxc_dly_en config*/
	cfg = ofnode_read_u32_default(node, ytphy_rxden_grp[0].name, ~0);
	if (cfg != -1) {

		val = ytphy_read_ext(phydev, YTPHY_EXTREG_CHIP_CONFIG);

		/*check the cfg overflow or not*/
		cfg = (cfg > ((1 << ytphy_rxden_grp[0].size) - 1)) ?
			((1 << ytphy_rxden_grp[0].size) - 1) : cfg;

		val = bitfield_replace(val, ytphy_rxden_grp[0].off,
			ytphy_rxden_grp[0].size, cfg);
		ytphy_write_ext(phydev, YTPHY_EXTREG_CHIP_CONFIG, val);
	}

	val = ytphy_read_ext(phydev, YTPHY_PAD_DRIVES_STRENGTH_CFG);
	for (i = 0; i < ARRAY_SIZE(ytphy_dr_grp); i++) {

		cfg = ofnode_read_u32_default(node,
			ytphy_dr_grp[i].name, ~0);
		cfg = (cfg != -1) ? cfg : ytphy_dr_grp[i].dflt;

		/*check the cfg overflow or not*/
		cfg = (cfg > ((1 << ytphy_dr_grp[i].size) - 1)) ?
			((1 << ytphy_dr_grp[i].size) - 1) : cfg;

		val = bitfield_replace(val, ytphy_dr_grp[i].off,
				ytphy_dr_grp[i].size, cfg);
	}
	ytphy_write_ext(phydev, YTPHY_PAD_DRIVES_STRENGTH_CFG, val);

	val = ytphy_read_ext(phydev, YTPHY_EXTREG_RGMII_CONFIG1);
	for (i = 0; i < ARRAY_SIZE(ytphy_rxtxd_grp); i++) {

		cfg = ofnode_read_u32_default(node,
			ytphy_rxtxd_grp[i].name, ~0);
		cfg = (cfg != -1) ? cfg : ytphy_rxtxd_grp[i].dflt;

		/*check the cfg overflow or not*/
		cfg = (cfg > ((1 << ytphy_rxtxd_grp[i].size) - 1)) ?
			((1 << ytphy_rxtxd_grp[i].size) - 1) : cfg;

		val = bitfield_replace(val, ytphy_rxtxd_grp[i].off,
				ytphy_rxtxd_grp[i].size, cfg);
	}

	return ytphy_write_ext(phydev, YTPHY_EXTREG_RGMII_CONFIG1, val);
}

static int yt8512_led_init(struct phy_device *phydev)
{
	int ret;
	int val;
	int mask;

	val = ytphy_read_ext(phydev, YT8512_EXTREG_LED0);
	if (val < 0)
		return val;

	val |= YT8512_LED0_ACT_BLK_IND;

	mask = YT8512_LED0_DIS_LED_AN_TRY | YT8512_LED0_BT_BLK_EN |
	YT8512_LED0_HT_BLK_EN | YT8512_LED0_COL_BLK_EN |
	YT8512_LED0_BT_ON_EN;
	val &= ~mask;

	ret = ytphy_write_ext(phydev, YT8512_EXTREG_LED0, val);
	if (ret < 0)
		return ret;

	val = ytphy_read_ext(phydev, YT8512_EXTREG_LED1);
	if (val < 0)
		return val;

	val |= YT8512_LED1_BT_ON_EN;

	mask = YT8512_LED1_TXACT_BLK_EN | YT8512_LED1_RXACT_BLK_EN;
	val &= ~mask;

	ret = ytphy_write_ext(phydev, YT8512_LED1_BT_ON_EN, val);

	return ret;
}

static int yt8512_config(struct phy_device *phydev)
{
	int ret;
	int val;

	ret = 0;
	genphy_config_aneg(phydev);

	ret = yt8512_led_init(phydev);

	/* disable auto sleep */
	val = ytphy_read_ext(phydev, YT8512_EXTREG_SLEEP_CONTROL1);
	if (val < 0)
		return val;

	val &= (~BIT(YT8512_EN_SLEEP_SW_BIT));

	ret = ytphy_write_ext(phydev, YT8512_EXTREG_SLEEP_CONTROL1, val);
	if (ret < 0)
		return ret;

	return ret;
}

static int yt8521_config(struct phy_device *phydev)
{
	int ret, val;

	ret = 0;
	genphy_config_aneg(phydev);

	/* disable auto sleep */
	val = ytphy_read_ext(phydev, EXTREG_SLEEP_CONTROL);
	if (val < 0)
		return val;

	val &= ~(1 << YT8521_EN_SLEEP_SW_BIT);
	ret = ytphy_write_ext(phydev, EXTREG_SLEEP_CONTROL, val);
	if (ret < 0)
		return ret;

	/*set delay config*/
	ret = ytphy_of_config(phydev);
	if (ret < 0)
		return ret;

	val = ytphy_read_ext(phydev, YT8521_EXT_CLK_GATE);
	if (val < 0)
		return val;

	val &= ~(1 << 12);
	ret = ytphy_write_ext(phydev, YT8521_EXT_CLK_GATE, val);
	if (ret < 0)
		return ret;

	return 0;
}

static int yt8531_config(struct phy_device *phydev)
{
	int ret;

	ret = 0;
	genphy_config_aneg(phydev);

	/*set delay config*/
	ret = ytphy_of_config(phydev);
	if (ret < 0)
		return ret;
	return 0;
}

static struct phy_driver YT8512_driver = {
	.name          = "YuTai YT8512",
	.uid           = 0x00000118,
	.mask          = 0x00000fff,
	.features      = PHY_GBIT_FEATURES,
	.config        = &yt8512_config,
	.startup       = &ytphy_startup,
	.shutdown      = &genphy_shutdown,
};

static struct phy_driver YT8521_driver = {
	.name = "YuTai YT8521",
	.uid = 0x0000011a,
	.mask = 0x00000fff,
	.features = PHY_GBIT_FEATURES,
	.config = &yt8521_config,
	.startup = &ytphy_startup,
	.shutdown = &genphy_shutdown,
};

static struct phy_driver YT8531_driver = {
	.name          = "YT8531 Gigabit Ethernet",
	.uid           = PHY_ID_YT8531,
	.mask          = MOTORCOMM_PHY_ID_MASK,
	.features      = PHY_GBIT_FEATURES,
	.config        = &yt8531_config,
	.startup       = &ytphy_startup,
	.shutdown      = &genphy_shutdown,
};

int phy_yutai_init(void)
{
	phy_register(&YT8512_driver);
	phy_register(&YT8521_driver);
	phy_register(&YT8531_driver);

	return 0;
}
