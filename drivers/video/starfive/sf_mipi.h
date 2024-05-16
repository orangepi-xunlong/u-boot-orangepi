/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2017, Fuzhou Rockchip Electronics Co., Ltd
 * Author: Eric Gao <eric.gao@rock-chips.com>
 */

#ifndef __RK_MIPI_H
#define __RK_MIPI_H

#include <clk.h>
#include <reset.h>
#include <mipi_dsi.h>

//sysrst registers
#define SRST_ASSERT0		0x00
#define SRST_STATUS0		0x04

#define IP_CONF				0x0
#define SP_HS_FIFO_DEPTH(x)		(((x) & GENMASK(30, 26)) >> 26)
#define SP_LP_FIFO_DEPTH(x)		(((x) & GENMASK(25, 21)) >> 21)
#define VRS_FIFO_DEPTH(x)		(((x) & GENMASK(20, 16)) >> 16)
#define DIRCMD_FIFO_DEPTH(x)		(((x) & GENMASK(15, 13)) >> 13)
#define SDI_IFACE_32			BIT(12)
#define INTERNAL_DATAPATH_32		(0 << 10)
#define INTERNAL_DATAPATH_16		(1 << 10)
#define INTERNAL_DATAPATH_8		(3 << 10)
#define INTERNAL_DATAPATH_SIZE		((x) & GENMASK(11, 10))
#define NUM_IFACE(x)			((((x) & GENMASK(9, 8)) >> 8) + 1)
#define MAX_LANE_NB(x)			(((x) & GENMASK(7, 6)) >> 6)
#define RX_FIFO_DEPTH(x)		((x) & GENMASK(5, 0))

#define MCTL_MAIN_DATA_CTL		0x4
#define TE_MIPI_POLLING_EN		BIT(25)
#define TE_HW_POLLING_EN		BIT(24)
#define DISP_EOT_GEN			BIT(18)
#define HOST_EOT_GEN			BIT(17)
#define DISP_GEN_CHECKSUM		BIT(16)
#define DISP_GEN_ECC			BIT(15)
#define BTA_EN				BIT(14)
#define READ_EN				BIT(13)
#define REG_TE_EN			BIT(12)
#define IF_TE_EN(x)			BIT(8 + (x))
#define TVG_SEL				BIT(6)
#define VID_EN				BIT(5)
#define IF_VID_SELECT(x)		((x) << 2)
#define IF_VID_SELECT_MASK		GENMASK(3, 2)
#define IF_VID_MODE			BIT(1)
#define LINK_EN				BIT(0)

#define MCTL_MAIN_PHY_CTL		0x8
#define HS_INVERT_DAT(x)		BIT(19 + ((x) * 2))
#define SWAP_PINS_DAT(x)		BIT(18 + ((x) * 2))
#define HS_INVERT_CLK			BIT(17)
#define SWAP_PINS_CLK			BIT(16)
#define HS_SKEWCAL_EN			BIT(15)
#define WAIT_BURST_TIME(x)		((x) << 10)
#define DATA_ULPM_EN(x)			BIT(6 + (x))
#define CLK_ULPM_EN			BIT(5)
#define CLK_CONTINUOUS			BIT(4)
#define DATA_LANE_EN(x)			BIT((x) - 1)

#define MCTL_MAIN_EN			0xc
#define DATA_FORCE_STOP			BIT(17)
#define CLK_FORCE_STOP			BIT(16)
#define IF_EN(x)			BIT(13 + (x))
#define DATA_LANE_ULPM_REQ(l)		BIT(9 + (l))
#define CLK_LANE_ULPM_REQ		BIT(8)
#define DATA_LANE_START(x)		BIT(4 + (x))
#define CLK_LANE_EN			BIT(3)
#define PLL_START			BIT(0)

#define MCTL_DPHY_CFG0			0x10
#define DPHY_C_RSTB			BIT(20)
#define DPHY_D_RSTB(x)			GENMASK(15 + (x), 16)
#define DPHY_PLL_PDN			BIT(10)
#define DPHY_CMN_PDN			BIT(9)
#define DPHY_C_PDN			BIT(8)
#define DPHY_D_PDN(x)			GENMASK(3 + (x), 4)
#define DPHY_ALL_D_PDN			GENMASK(7, 4)
#define DPHY_PLL_PSO			BIT(1)
#define DPHY_CMN_PSO			BIT(0)

#define MCTL_DPHY_TIMEOUT1		0x14
#define HSTX_TIMEOUT(x)			((x) << 4)
#define HSTX_TIMEOUT_MAX		GENMASK(17, 0)
#define CLK_DIV(x)			(x)
#define CLK_DIV_MAX			GENMASK(3, 0)

#define MCTL_DPHY_TIMEOUT2		0x18
#define LPRX_TIMEOUT(x)			(x)

#define MCTL_ULPOUT_TIME		0x1c
#define DATA_LANE_ULPOUT_TIME(x)	((x) << 9)
#define CLK_LANE_ULPOUT_TIME(x)		(x)

#define MCTL_3DVIDEO_CTL		0x20
#define VID_VSYNC_3D_EN			BIT(7)
#define VID_VSYNC_3D_LR			BIT(5)
#define VID_VSYNC_3D_SECOND_EN		BIT(4)
#define VID_VSYNC_3DFORMAT_LINE		(0 << 2)
#define VID_VSYNC_3DFORMAT_FRAME	(1 << 2)
#define VID_VSYNC_3DFORMAT_PIXEL	(2 << 2)
#define VID_VSYNC_3DMODE_OFF		0
#define VID_VSYNC_3DMODE_PORTRAIT	1
#define VID_VSYNC_3DMODE_LANDSCAPE	2

#define MCTL_MAIN_STS			0x24
#define MCTL_MAIN_STS_CTL		0x130
#define MCTL_MAIN_STS_CLR		0x150
#define MCTL_MAIN_STS_FLAG		0x170
#define HS_SKEWCAL_DONE			BIT(11)
#define IF_UNTERM_PKT_ERR(x)		BIT(8 + (x))
#define LPRX_TIMEOUT_ERR		BIT(7)
#define HSTX_TIMEOUT_ERR		BIT(6)
#define DATA_LANE_RDY(l)		BIT(2 + (l))
#define CLK_LANE_RDY			BIT(1)
#define PLL_LOCKED			BIT(0)

#define MCTL_DPHY_ERR			0x28
#define MCTL_DPHY_ERR_CTL1		0x148
#define MCTL_DPHY_ERR_CLR		0x168
#define MCTL_DPHY_ERR_FLAG		0x188
#define ERR_CONT_LP(x, l)		BIT(18 + ((x) * 4) + (l))
#define ERR_CONTROL(l)			BIT(14 + (l))
#define ERR_SYNESC(l)			BIT(10 + (l))
#define ERR_ESC(l)			BIT(6 + (l))

#define MCTL_DPHY_ERR_CTL2		0x14c
#define ERR_CONT_LP_EDGE(x, l)		BIT(12 + ((x) * 4) + (l))
#define ERR_CONTROL_EDGE(l)		BIT(8 + (l))
#define ERR_SYN_ESC_EDGE(l)		BIT(4 + (l))
#define ERR_ESC_EDGE(l)			BIT(0 + (l))

#define MCTL_LANE_STS			0x2c
#define PPI_C_TX_READY_HS		BIT(18)
#define DPHY_PLL_LOCK			BIT(17)
#define PPI_D_RX_ULPS_ESC(x)		(((x) & GENMASK(15, 12)) >> 12)
#define LANE_STATE_START		0
#define LANE_STATE_IDLE			1
#define LANE_STATE_WRITE		2
#define LANE_STATE_ULPM			3
#define LANE_STATE_READ			4
#define DATA_LANE_STATE(l, val)		\
	(((val) >> (2 + 2 * (l) + ((l) ? 1 : 0))) & GENMASK((l) ? 1 : 2, 0))
#define CLK_LANE_STATE_HS		2
#define CLK_LANE_STATE(val)		((val) & GENMASK(1, 0))

#define DSC_MODE_CTL			0x30
#define DSC_MODE_EN			BIT(0)

#define DSC_CMD_SEND			0x34
#define DSC_SEND_PPS			BIT(0)
#define DSC_EXECUTE_QUEUE		BIT(1)

#define DSC_PPS_WRDAT			0x38

#define DSC_MODE_STS			0x3c
#define DSC_PPS_DONE			BIT(1)
#define DSC_EXEC_DONE			BIT(2)

#define CMD_MODE_CTL			0x70
#define IF_LP_EN(x)			BIT(9 + (x))
#define IF_VCHAN_ID(x, c)		((c) << ((x) * 2))

#define CMD_MODE_CTL2			0x74
#define TE_TIMEOUT(x)			((x) << 11)
#define FILL_VALUE(x)			((x) << 3)
#define ARB_IF_WITH_HIGHEST_PRIORITY(x)	((x) << 1)
#define ARB_ROUND_ROBIN_MODE		BIT(0)

#define CMD_MODE_STS			0x78
#define CMD_MODE_STS_CTL		0x134
#define CMD_MODE_STS_CLR		0x154
#define CMD_MODE_STS_FLAG		0x174
#define ERR_IF_UNDERRUN(x)		BIT(4 + (x))
#define ERR_UNWANTED_READ		BIT(3)
#define ERR_TE_MISS			BIT(2)
#define ERR_NO_TE			BIT(1)
#define CSM_RUNNING			BIT(0)

#define DIRECT_CMD_SEND			0x80

#define DIRECT_CMD_MAIN_SETTINGS	0x84
#define TRIGGER_VAL(x)			((x) << 25)
#define CMD_LP_EN			BIT(24)
#define CMD_SIZE(x)			((x) << 16)
#define CMD_VCHAN_ID(x)			((x) << 14)
#define CMD_DATATYPE(x)			((x) << 8)
#define CMD_LONG			BIT(3)
#define WRITE_CMD			0
#define READ_CMD			1
#define TE_REQ				4
#define TRIGGER_REQ			5
#define BTA_REQ				6

#define DIRECT_CMD_STS			0x88
#define DIRECT_CMD_STS_CTL		0x138
#define DIRECT_CMD_STS_CLR		0x158
#define DIRECT_CMD_STS_FLAG		0x178
#define RCVD_ACK_VAL(val)		((val) >> 16)
#define RCVD_TRIGGER_VAL(val)		(((val) & GENMASK(14, 11)) >> 11)
#define READ_COMPLETED_WITH_ERR		BIT(10)
#define BTA_FINISHED			BIT(9)
#define BTA_COMPLETED			BIT(8)
#define TE_RCVD				BIT(7)
#define TRIGGER_RCVD			BIT(6)
#define ACK_WITH_ERR_RCVD		BIT(5)
#define ACK_RCVD			BIT(4)
#define READ_COMPLETED			BIT(3)
#define TRIGGER_COMPLETED		BIT(2)
#define WRITE_COMPLETED			BIT(1)

#define SENDING_CMD			BIT(0)

#define DIRECT_CMD_STOP_READ		0x8c

#define DIRECT_CMD_WRDATA		0x90

#define DIRECT_CMD_FIFO_RST		0x94

#define DIRECT_CMD_RDDATA		0xa0

#define DIRECT_CMD_RD_PROPS		0xa4
#define RD_DCS				BIT(18)
#define RD_VCHAN_ID(val)		(((val) >> 16) & GENMASK(1, 0))
#define RD_SIZE(val)			((val) & GENMASK(15, 0))

#define DIRECT_CMD_RD_STS		0xa8
#define DIRECT_CMD_RD_STS_CTL		0x13c
#define DIRECT_CMD_RD_STS_CLR		0x15c
#define DIRECT_CMD_RD_STS_FLAG		0x17c
#define ERR_EOT_WITH_ERR		BIT(8)
#define ERR_MISSING_EOT			BIT(7)
#define ERR_WRONG_LENGTH		BIT(6)
#define ERR_OVERSIZE			BIT(5)
#define ERR_RECEIVE			BIT(4)
#define ERR_UNDECODABLE			BIT(3)
#define ERR_CHECKSUM			BIT(2)
#define ERR_UNCORRECTABLE		BIT(1)
#define ERR_FIXED			BIT(0)

#define VID_MAIN_CTL			0xb0
#define VID_IGNORE_MISS_VSYNC		BIT(31)
#define VID_FIELD_SW			BIT(28)
#define VID_INTERLACED_EN		BIT(27)
#define RECOVERY_MODE(x)		((x) << 25)
#define RECOVERY_MODE_NEXT_HSYNC	0
#define RECOVERY_MODE_NEXT_STOP_POINT	2
#define RECOVERY_MODE_NEXT_VSYNC	3
#define REG_BLKEOL_MODE(x)		((x) << 23)
#define REG_BLKLINE_MODE(x)		((x) << 21)
#define REG_BLK_MODE_NULL_PKT		0
#define REG_BLK_MODE_BLANKING_PKT	1
#define REG_BLK_MODE_LP			2
#define SYNC_PULSE_HORIZONTAL		BIT(20)
#define SYNC_PULSE_ACTIVE		BIT(19)
#define BURST_MODE			BIT(18)
#define VID_PIXEL_MODE_MASK		GENMASK(17, 14)
#define VID_PIXEL_MODE_RGB565		(0 << 14)
#define VID_PIXEL_MODE_RGB666_PACKED	(1 << 14)
#define VID_PIXEL_MODE_RGB666		(2 << 14)
#define VID_PIXEL_MODE_RGB888		(3 << 14)
#define VID_PIXEL_MODE_RGB101010	(4 << 14)
#define VID_PIXEL_MODE_RGB121212	(5 << 14)
#define VID_PIXEL_MODE_YUV420		(8 << 14)
#define VID_PIXEL_MODE_YUV422_PACKED	(9 << 14)
#define VID_PIXEL_MODE_YUV422		(10 << 14)
#define VID_PIXEL_MODE_YUV422_24B	(11 << 14)
#define VID_PIXEL_MODE_DSC_COMP		(12 << 14)
#define VID_DATATYPE(x)			((x) << 8)
#define VID_VIRTCHAN_ID(iface, x)	((x) << (4 + (iface) * 2))
#define STOP_MODE(x)			((x) << 2)
#define START_MODE(x)			(x)

#define VID_VSIZE1			0xb4
#define VFP_LEN(x)			((x) << 12)
#define VBP_LEN(x)			((x) << 6)
#define VSA_LEN(x)			(x)

#define VID_VSIZE2			0xb8
#define VACT_LEN(x)			(x)

#define VID_HSIZE1			0xc0
#define HBP_LEN(x)			((x) << 16)
#define HSA_LEN(x)			(x)

#define VID_HSIZE2			0xc4
#define HFP_LEN(x)			((x) << 16)
#define HACT_LEN(x)			(x)

#define VID_BLKSIZE1			0xcc
#define BLK_EOL_PKT_LEN(x)		((x) << 15)
#define BLK_LINE_EVENT_PKT_LEN(x)	(x)

#define VID_BLKSIZE2			0xd0
#define BLK_LINE_PULSE_PKT_LEN(x)	(x)

#define VID_PKT_TIME			0xd8
#define BLK_EOL_DURATION(x)		(x)

#define VID_DPHY_TIME			0xdc
#define REG_WAKEUP_TIME(x)		((x) << 17)
#define REG_LINE_DURATION(x)		(x)

#define VID_ERR_COLOR1			0xe0
#define COL_GREEN(x)			((x) << 12)
#define COL_RED(x)			(x)

#define VID_ERR_COLOR2			0xe4
#define PAD_VAL(x)			((x) << 12)
#define COL_BLUE(x)			(x)

#define VID_VPOS			0xe8
#define LINE_VAL(val)			(((val) & GENMASK(14, 2)) >> 2)
#define LINE_POS(val)			((val) & GENMASK(1, 0))

#define VID_HPOS			0xec
#define HORIZ_VAL(val)			(((val) & GENMASK(17, 3)) >> 3)
#define HORIZ_POS(val)			((val) & GENMASK(2, 0))

#define VID_MODE_STS			0xf0
#define VID_MODE_STS_CTL		0x140
#define VID_MODE_STS_CLR		0x160
#define VID_MODE_STS_FLAG		0x180
#define VSG_RECOVERY			BIT(10)
#define ERR_VRS_WRONG_LEN		BIT(9)
#define ERR_LONG_READ			BIT(8)
#define ERR_LINE_WRITE			BIT(7)
#define ERR_BURST_WRITE			BIT(6)
#define ERR_SMALL_HEIGHT		BIT(5)
#define ERR_SMALL_LEN			BIT(4)
#define ERR_MISSING_VSYNC		BIT(3)
#define ERR_MISSING_HSYNC		BIT(2)
#define ERR_MISSING_DATA		BIT(1)
#define VSG_RUNNING			BIT(0)

#define VID_VCA_SETTING1		0xf4
#define BURST_LP			BIT(16)
#define MAX_BURST_LIMIT(x)		(x)

#define VID_VCA_SETTING2		0xf8
#define MAX_LINE_LIMIT(x)		((x) << 16)
#define EXACT_BURST_LIMIT(x)		(x)

#define TVG_CTL				0xfc
#define TVG_STRIPE_SIZE(x)		((x) << 5)
#define TVG_MODE_MASK			GENMASK(4, 3)
#define TVG_MODE_SINGLE_COLOR		(0 << 3)
#define TVG_MODE_VSTRIPES		(2 << 3)
#define TVG_MODE_HSTRIPES		(3 << 3)
#define TVG_STOPMODE_MASK		GENMASK(2, 1)
#define TVG_STOPMODE_EOF		(0 << 1)
#define TVG_STOPMODE_EOL		(1 << 1)
#define TVG_STOPMODE_NOW		(2 << 1)
#define TVG_RUN				BIT(0)

#define TVG_IMG_SIZE			0x100
#define TVG_NBLINES(x)			((x) << 16)
#define TVG_LINE_SIZE(x)		(x)

#define TVG_COLOR1			0x104
#define TVG_COL1_GREEN(x)		((x) << 12)
#define TVG_COL1_RED(x)			(x)

#define TVG_COLOR1_BIS			0x108
#define TVG_COL1_BLUE(x)		(x)

#define TVG_COLOR2			0x10c
#define TVG_COL2_GREEN(x)		((x) << 12)
#define TVG_COL2_RED(x)			(x)

#define TVG_COLOR2_BIS			0x110
#define TVG_COL2_BLUE(x)		(x)

#define TVG_STS				0x114
#define TVG_STS_CTL			0x144
#define TVG_STS_CLR			0x164
#define TVG_STS_FLAG			0x184
#define TVG_STS_RUNNING			BIT(0)

#define STS_CTL_EDGE(e)			((e) << 16)

#define DPHY_LANES_MAP			0x198
#define DAT_REMAP_CFG(b, l)		((l) << ((b) * 8))

#define DPI_IRQ_EN			0x1a0
#define DPI_IRQ_CLR			0x1a4
#define DPI_IRQ_STS			0x1a8
#define PIXEL_BUF_OVERFLOW		BIT(0)

#define DPI_CFG				0x1ac
#define DPI_CFG_FIFO_DEPTH(x)		((x) >> 16)
#define DPI_CFG_FIFO_LEVEL(x)		((x) & GENMASK(15, 0))

#define TEST_GENERIC			0x1f0
#define TEST_STATUS(x)			((x) >> 16)
#define TEST_CTRL(x)			(x)

#define ID_REG				0x1fc
#define REV_VENDOR_ID(x)		(((x) & GENMASK(31, 20)) >> 20)
#define REV_PRODUCT_ID(x)		(((x) & GENMASK(19, 12)) >> 12)
#define REV_HW(x)			(((x) & GENMASK(11, 8)) >> 8)
#define REV_MAJOR(x)			(((x) & GENMASK(7, 4)) >> 4)
#define REV_MINOR(x)			((x) & GENMASK(3, 0))

#define DSI_OUTPUT_PORT			0
#define DSI_INPUT_PORT(inputid)		(1 + (inputid))

#define DSI_HBP_FRAME_OVERHEAD		12
#define DSI_HSA_FRAME_OVERHEAD		14
#define DSI_HFP_FRAME_OVERHEAD		6
#define DSI_HSS_VSS_VSE_FRAME_OVERHEAD	4
#define DSI_BLANKING_FRAME_OVERHEAD	6
#define DSI_NULL_FRAME_OVERHEAD		6
#define DSI_EOT_PKT_SIZE		4

#define ARRAY_SIZE_DSI(x) (sizeof(x) / sizeof((x)[0]))
#define TEST_CTRL(x)                    (x)

#define  AON_POWER_READY_N_WIDTH             0x1U
#define  AON_POWER_READY_N_SHIFT             0x0U
#define  AON_POWER_READY_N_MASK              0x1U
#define  CFG_CKLANE_SET_WIDTH                0x5U
#define  CFG_CKLANE_SET_SHIFT                0x1U
#define  CFG_CKLANE_SET_MASK                 0x3EU
#define  CFG_DATABUS16_SEL_WIDTH             0x1U
#define  CFG_DATABUS16_SEL_SHIFT             0x6U
#define  CFG_DATABUS16_SEL_MASK              0x40U
#define  CFG_DPDN_SWAP_WIDTH                 0x5U
#define  CFG_DPDN_SWAP_SHIFT                 0x7U
#define  CFG_DPDN_SWAP_MASK                  0xF80U
#define  CFG_L0_SWAP_SEL_WIDTH               0x3U
#define  CFG_L0_SWAP_SEL_SHIFT               0xCU
#define  CFG_L0_SWAP_SEL_MASK                0x7000U
#define  CFG_L1_SWAP_SEL_WIDTH               0x3U
#define  CFG_L1_SWAP_SEL_SHIFT               0xFU
#define  CFG_L1_SWAP_SEL_MASK                0x38000U
#define  CFG_L2_SWAP_SEL_WIDTH               0x3U
#define  CFG_L2_SWAP_SEL_SHIFT               0x12U
#define  CFG_L2_SWAP_SEL_MASK                0x1C0000U
#define  CFG_L3_SWAP_SEL_WIDTH               0x3U
#define  CFG_L3_SWAP_SEL_SHIFT               0x15U
#define  CFG_L3_SWAP_SEL_MASK                0xE00000U
#define  CFG_L4_SWAP_SEL_WIDTH               0x3U
#define  CFG_L4_SWAP_SEL_SHIFT               0x18U
#define  CFG_L4_SWAP_SEL_MASK                0x7000000U
#define  MPOSV_31_0__WIDTH                   0x20U
#define  MPOSV_31_0__SHIFT                   0x0U
#define  MPOSV_31_0__MASK                    0xFFFFFFFFU
#define  MPOSV_46_32__WIDTH                  0xFU
#define  MPOSV_46_32__SHIFT                  0x0U
#define  MPOSV_46_32__MASK                   0x7FFFU
#define  RGS_CDTX_PLL_FM_CPLT_WIDTH          0x1U
#define  RGS_CDTX_PLL_FM_CPLT_SHIFT          0xFU
#define  RGS_CDTX_PLL_FM_CPLT_MASK           0x8000U
#define  RGS_CDTX_PLL_FM_OVER_WIDTH          0x1U
#define  RGS_CDTX_PLL_FM_OVER_SHIFT          0x10U
#define  RGS_CDTX_PLL_FM_OVER_MASK           0x10000U
#define  RGS_CDTX_PLL_FM_UNDER_WIDTH         0x1U
#define  RGS_CDTX_PLL_FM_UNDER_SHIFT         0x11U
#define  RGS_CDTX_PLL_FM_UNDER_MASK          0x20000U
#define  RGS_CDTX_PLL_UNLOCK_WIDTH           0x1U
#define  RGS_CDTX_PLL_UNLOCK_SHIFT           0x12U
#define  RGS_CDTX_PLL_UNLOCK_MASK            0x40000U
#define  RG_CDTX_L0N_HSTX_RES_WIDTH          0x5U
#define  RG_CDTX_L0N_HSTX_RES_SHIFT          0x13U
#define  RG_CDTX_L0N_HSTX_RES_MASK           0xF80000U
#define  RG_CDTX_L0P_HSTX_RES_WIDTH          0x5U
#define  RG_CDTX_L0P_HSTX_RES_SHIFT          0x18U
#define  RG_CDTX_L0P_HSTX_RES_MASK           0x1F000000U

#define  RG_CDTX_L1N_HSTX_RES_WIDTH          0x5U
#define  RG_CDTX_L1N_HSTX_RES_SHIFT          0x0U
#define  RG_CDTX_L1N_HSTX_RES_MASK           0x1FU
#define  RG_CDTX_L1P_HSTX_RES_WIDTH          0x5U
#define  RG_CDTX_L1P_HSTX_RES_SHIFT          0x5U
#define  RG_CDTX_L1P_HSTX_RES_MASK           0x3E0U
#define  RG_CDTX_L2N_HSTX_RES_WIDTH          0x5U
#define  RG_CDTX_L2N_HSTX_RES_SHIFT          0xAU
#define  RG_CDTX_L2N_HSTX_RES_MASK           0x7C00U
#define  RG_CDTX_L2P_HSTX_RES_WIDTH          0x5U
#define  RG_CDTX_L2P_HSTX_RES_SHIFT          0xFU
#define  RG_CDTX_L2P_HSTX_RES_MASK           0xF8000U
#define  RG_CDTX_L3N_HSTX_RES_WIDTH          0x5U
#define  RG_CDTX_L3N_HSTX_RES_SHIFT          0x14U
#define  RG_CDTX_L3N_HSTX_RES_MASK           0x1F00000U
#define  RG_CDTX_L3P_HSTX_RES_WIDTH          0x5U
#define  RG_CDTX_L3P_HSTX_RES_SHIFT          0x19U
#define  RG_CDTX_L3P_HSTX_RES_MASK           0x3E000000U

#define  RG_CDTX_L4N_HSTX_RES_WIDTH          0x5U
#define  RG_CDTX_L4N_HSTX_RES_SHIFT          0x0U
#define  RG_CDTX_L4N_HSTX_RES_MASK           0x1FU
#define  RG_CDTX_L4P_HSTX_RES_WIDTH          0x5U
#define  RG_CDTX_L4P_HSTX_RES_SHIFT          0x5U
#define  RG_CDTX_L4P_HSTX_RES_MASK           0x3E0U

#define  RG_CDTX_PLL_FBK_FRA_WIDTH           0x18U
#define  RG_CDTX_PLL_FBK_FRA_SHIFT           0x0U
#define  RG_CDTX_PLL_FBK_FRA_MASK            0xFFFFFFU

#define  RG_CDTX_PLL_FBK_INT_WIDTH           0x9U
#define  RG_CDTX_PLL_FBK_INT_SHIFT           0x0U
#define  RG_CDTX_PLL_FBK_INT_MASK            0x1FFU
#define  RG_CDTX_PLL_FM_EN_WIDTH             0x1U
#define  RG_CDTX_PLL_FM_EN_SHIFT             0x9U
#define  RG_CDTX_PLL_FM_EN_MASK              0x200U
#define  RG_CDTX_PLL_LDO_STB_X2_EN_WIDTH     0x1U
#define  RG_CDTX_PLL_LDO_STB_X2_EN_SHIFT     0xAU
#define  RG_CDTX_PLL_LDO_STB_X2_EN_MASK      0x400U
#define  RG_CDTX_PLL_PRE_DIV_WIDTH           0x2U
#define  RG_CDTX_PLL_PRE_DIV_SHIFT           0xBU
#define  RG_CDTX_PLL_PRE_DIV_MASK            0x1800U
#define  RG_CDTX_PLL_SSC_DELTA_WIDTH         0x12U
#define  RG_CDTX_PLL_SSC_DELTA_SHIFT         0xDU
#define  RG_CDTX_PLL_SSC_DELTA_MASK          0x7FFFE000U

#define  RG_CDTX_PLL_SSC_DELTA_INIT_WIDTH    0x12U
#define  RG_CDTX_PLL_SSC_DELTA_INIT_SHIFT    0x0U
#define  RG_CDTX_PLL_SSC_DELTA_INIT_MASK     0x3FFFFU
#define  RG_CDTX_PLL_SSC_EN_WIDTH            0x1U
#define  RG_CDTX_PLL_SSC_EN_SHIFT            0x12U
#define  RG_CDTX_PLL_SSC_EN_MASK             0x40000U
#define  RG_CDTX_PLL_SSC_PRD_WIDTH           0xAU
#define  RG_CDTX_PLL_SSC_PRD_SHIFT           0x13U
#define  RG_CDTX_PLL_SSC_PRD_MASK            0x1FF80000U

#define  RG_CLANE_HS_CLK_POST_TIME_WIDTH     0x8U
#define  RG_CLANE_HS_CLK_POST_TIME_SHIFT     0x0U
#define  RG_CLANE_HS_CLK_POST_TIME_MASK      0xFFU
#define  RG_CLANE_HS_CLK_PRE_TIME_WIDTH      0x8U
#define  RG_CLANE_HS_CLK_PRE_TIME_SHIFT      0x8U
#define  RG_CLANE_HS_CLK_PRE_TIME_MASK       0xFF00U
#define  RG_CLANE_HS_PRE_TIME_WIDTH          0x8U
#define  RG_CLANE_HS_PRE_TIME_SHIFT          0x10U
#define  RG_CLANE_HS_PRE_TIME_MASK           0xFF0000U
#define  RG_CLANE_HS_TRAIL_TIME_WIDTH        0x8U
#define  RG_CLANE_HS_TRAIL_TIME_SHIFT        0x18U
#define  RG_CLANE_HS_TRAIL_TIME_MASK         0xFF000000U

#define  RG_CLANE_HS_ZERO_TIME_WIDTH         0x8U
#define  RG_CLANE_HS_ZERO_TIME_SHIFT         0x0U
#define  RG_CLANE_HS_ZERO_TIME_MASK          0xFFU
#define  RG_DLANE_HS_PRE_TIME_WIDTH          0x8U
#define  RG_DLANE_HS_PRE_TIME_SHIFT          0x8U
#define  RG_DLANE_HS_PRE_TIME_MASK           0xFF00U
#define  RG_DLANE_HS_TRAIL_TIME_WIDTH        0x8U
#define  RG_DLANE_HS_TRAIL_TIME_SHIFT        0x10U
#define  RG_DLANE_HS_TRAIL_TIME_MASK         0xFF0000U
#define  RG_DLANE_HS_ZERO_TIME_WIDTH         0x8U
#define  RG_DLANE_HS_ZERO_TIME_SHIFT         0x18U
#define  RG_DLANE_HS_ZERO_TIME_MASK          0xFF000000U

#define  RG_EXTD_CYCLE_SEL_WIDTH             0x3U
#define  RG_EXTD_CYCLE_SEL_SHIFT             0x0U
#define  RG_EXTD_CYCLE_SEL_MASK              0x7U

#define  SCFG_C_HS_PRE_ZERO_TIME_WIDTH       0x20U
#define  SCFG_C_HS_PRE_ZERO_TIME_SHIFT       0x0U
#define  SCFG_C_HS_PRE_ZERO_TIME_MASK        0xFFFFFFFFU

#define  SCFG_DPHY_SRC_SEL_WIDTH             0x1U
#define  SCFG_DPHY_SRC_SEL_SHIFT             0x0U
#define  SCFG_DPHY_SRC_SEL_MASK              0x1U
#define  SCFG_DSI_TXREADY_ESC_SEL_WIDTH      0x2U
#define  SCFG_DSI_TXREADY_ESC_SEL_SHIFT      0x1U
#define  SCFG_DSI_TXREADY_ESC_SEL_MASK       0x6U
#define  SCFG_PPI_C_READY_SEL_WIDTH          0x2U
#define  SCFG_PPI_C_READY_SEL_SHIFT          0x3U
#define  SCFG_PPI_C_READY_SEL_MASK           0x18U
#define  VCONTROL_WIDTH                      0x5U
#define  VCONTROL_SHIFT                      0x5U
#define  VCONTROL_MASK                       0x3E0U

#define  XCFGI_DW00_WIDTH                    0x20U
#define  XCFGI_DW00_SHIFT                    0x0U
#define  XCFGI_DW00_MASK                     0xFFFFFFFFU

#define  XCFGI_DW01_WIDTH                    0x20U
#define  XCFGI_DW01_SHIFT                    0x0U
#define  XCFGI_DW01_MASK                     0xFFFFFFFFU

#define  XCFGI_DW02_WIDTH                    0x20U
#define  XCFGI_DW02_SHIFT                    0x0U
#define  XCFGI_DW02_MASK                     0xFFFFFFFFU

#define  XCFGI_DW03_WIDTH                    0x20U
#define  XCFGI_DW03_SHIFT                    0x0U
#define  XCFGI_DW03_MASK                     0xFFFFFFFFU

#define  XCFGI_DW04_WIDTH                    0x20U
#define  XCFGI_DW04_SHIFT                    0x0U
#define  XCFGI_DW04_MASK                     0xFFFFFFFFU

#define  XCFGI_DW05_WIDTH                    0x20U
#define  XCFGI_DW05_SHIFT                    0x0U
#define  XCFGI_DW05_MASK                     0xFFFFFFFFU

#define  XCFGI_DW06_WIDTH                    0x20U
#define  XCFGI_DW06_SHIFT                    0x0U
#define  XCFGI_DW06_MASK                     0xFFFFFFFFU

#define  XCFGI_DW07_WIDTH                    0x20U
#define  XCFGI_DW07_SHIFT                    0x0U
#define  XCFGI_DW07_MASK                     0xFFFFFFFFU

#define  XCFGI_DW08_WIDTH                    0x20U
#define  XCFGI_DW08_SHIFT                    0x0U
#define  XCFGI_DW08_MASK                     0xFFFFFFFFU

#define  XCFGI_DW09_WIDTH                    0x20U
#define  XCFGI_DW09_SHIFT                    0x0U
#define  XCFGI_DW09_MASK                     0xFFFFFFFFU

#define  XCFGI_DW0A_WIDTH                    0x20U
#define  XCFGI_DW0A_SHIFT                    0x0U
#define  XCFGI_DW0A_MASK                     0xFFFFFFFFU

#define  XCFGI_DW0B_WIDTH                    0x20U
#define  XCFGI_DW0B_SHIFT                    0x0U
#define  XCFGI_DW0B_MASK                     0xFFFFFFFFU

#define  DBG1_MUX_DOUT_WIDTH                 0x8U
#define  DBG1_MUX_DOUT_SHIFT                 0x0U
#define  DBG1_MUX_DOUT_MASK                  0xFFU
#define  DBG1_MUX_SEL_WIDTH                  0x5U
#define  DBG1_MUX_SEL_SHIFT                  0x8U
#define  DBG1_MUX_SEL_MASK                   0x1F00U
#define  DBG2_MUX_DOUT_WIDTH                 0x8U
#define  DBG2_MUX_DOUT_SHIFT                 0xDU
#define  DBG2_MUX_DOUT_MASK                  0x1FE000U
#define  DBG2_MUX_SEL_WIDTH                  0x5U
#define  DBG2_MUX_SEL_SHIFT                  0x15U
#define  DBG2_MUX_SEL_MASK                   0x3E00000U
#define  REFCLK_IN_SEL_WIDTH                 0x3U
#define  REFCLK_IN_SEL_SHIFT                 0x1AU
#define  REFCLK_IN_SEL_MASK                  0x1C000000U
#define  RESETB_WIDTH                        0x1U
#define  RESETB_SHIFT                        0x1DU
#define  RESETB_MASK                         0x20000000U

//aonsys con
#define AON_GP_REG_WIDTH                                   0x20U
#define AON_GP_REG_SHIFT                                   0x0U
#define AON_GP_REG_MASK                                    0xFFFFFFFFU


#define M31_DPHY_REFCLK_RESERVED	0
#define M31_DPHY_REFCLK_12M		1
#define M31_DPHY_REFCLK_19_2M		2
#define M31_DPHY_REFCLK_25M		3
#define M31_DPHY_REFCLK_26M		4
#define M31_DPHY_REFCLK_27M		5
#define M31_DPHY_REFCLK_38_4M		6
#define M31_DPHY_REFCLK_52M		7
#define M31_DPHY_REFCLK_BUTT		8

#define DPHY_TX_PSW_EN_MASK		(1<<30)

struct m31_dphy_config {
    int ref_clk;
    unsigned long bitrate;
    uint32_t pll_prev_div, pll_fbk_int, pll_fbk_fra, extd_cycle_sel;
    uint32_t dlane_hs_pre_time, dlane_hs_zero_time, dlane_hs_trail_time;
    uint32_t clane_hs_pre_time, clane_hs_zero_time, clane_hs_trail_time;
    uint32_t clane_hs_clk_pre_time, clane_hs_clk_post_time;
};

#define M31_DPHY_REFCLK         M31_DPHY_REFCLK_12M
#define M31_DPHY_BITRATE_ALIGN  10000000



static const struct m31_dphy_config m31_dphy_configs[] = {
#if (M31_DPHY_REFCLK == M31_DPHY_REFCLK_25M)
	{25000000,  100000000, 0x1, 0x80, 0x000000, 0x4, 0x10, 0x21, 0x17, 0x07, 0x35, 0x0F, 0x0F, 0x73,},
	{25000000,  200000000, 0x1, 0x80, 0x000000, 0x3, 0x0C, 0x1B, 0x13, 0x07, 0x35, 0x0F, 0x07, 0x3F,},
	{25000000,  300000000, 0x1, 0xC0, 0x000000, 0x3, 0x11, 0x25, 0x19, 0x0A, 0x50, 0x15, 0x07, 0x45,},
	{25000000,  400000000, 0x1, 0x80, 0x000000, 0x2, 0x0A, 0x18, 0x11, 0x07, 0x35, 0x0F, 0x03, 0x25,},
	{25000000,  500000000, 0x1, 0xA0, 0x000000, 0x2, 0x0C, 0x1D, 0x14, 0x09, 0x42, 0x12, 0x03, 0x28,},
	{25000000,  600000000, 0x1, 0xC0, 0x000000, 0x2, 0x0E, 0x23, 0x17, 0x0A, 0x50, 0x15, 0x03, 0x2B,},
	{25000000,  700000000, 0x1, 0x70, 0x000000, 0x1, 0x08, 0x14, 0x0F, 0x06, 0x2F, 0x0E, 0x01, 0x16,},
	{25000000,  800000000, 0x1, 0x80, 0x000000, 0x1, 0x09, 0x17, 0x10, 0x07, 0x35, 0x0F, 0x01, 0x18,},
	{25000000,  900000000, 0x1, 0x90, 0x000000, 0x1, 0x0A, 0x19, 0x12, 0x08, 0x3C, 0x10, 0x01, 0x19,},
	{25000000, 1000000000, 0x1, 0xA0, 0x000000, 0x1, 0x0B, 0x1C, 0x13, 0x09, 0x42, 0x12, 0x01, 0x1B,},
	{25000000, 1100000000, 0x1, 0xB0, 0x000000, 0x1, 0x0C, 0x1E, 0x15, 0x09, 0x4A, 0x14, 0x01, 0x1D,},
	{25000000, 1200000000, 0x1, 0xC0, 0x000000, 0x1, 0x0E, 0x20, 0x16, 0x0A, 0x50, 0x15, 0x01, 0x1E,},
	{25000000, 1300000000, 0x1, 0x68, 0x000000, 0x0, 0x07, 0x12, 0x0D, 0x05, 0x2C, 0x0D, 0x00, 0x0F,},
	{25000000, 1400000000, 0x1, 0x70, 0x000000, 0x0, 0x07, 0x14, 0x0E, 0x06, 0x2F, 0x0E, 0x00, 0x10,},
	{25000000, 1500000000, 0x1, 0x78, 0x000000, 0x0, 0x08, 0x14, 0x0F, 0x06, 0x32, 0x0E, 0x00, 0x11,},
	{25000000, 1600000000, 0x1, 0x80, 0x000000, 0x0, 0x09, 0x15, 0x10, 0x07, 0x35, 0x0F, 0x00, 0x12,},
	{25000000, 1700000000, 0x1, 0x88, 0x000000, 0x0, 0x09, 0x17, 0x10, 0x07, 0x39, 0x10, 0x00, 0x12,},
	{25000000, 1800000000, 0x1, 0x90, 0x000000, 0x0, 0x0A, 0x18, 0x11, 0x08, 0x3C, 0x10, 0x00, 0x13,},
	{25000000, 1900000000, 0x1, 0x98, 0x000000, 0x0, 0x0A, 0x1A, 0x12, 0x08, 0x3F, 0x11, 0x00, 0x14,},
	{25000000, 2000000000, 0x1, 0xA0, 0x000000, 0x0, 0x0B, 0x1B, 0x13, 0x09, 0x42, 0x12, 0x00, 0x15,},
	{25000000, 2100000000, 0x1, 0xA8, 0x000000, 0x0, 0x0B, 0x1C, 0x13, 0x09, 0x46, 0x13, 0x00, 0x15,},
	{25000000, 2200000000, 0x1, 0xB0, 0x000000, 0x0, 0x0C, 0x1D, 0x14, 0x09, 0x4A, 0x14, 0x00, 0x16,},
	{25000000, 2300000000, 0x1, 0xB8, 0x000000, 0x0, 0x0C, 0x1F, 0x15, 0x0A, 0x4C, 0x14, 0x00, 0x17,},
	{25000000, 2400000000, 0x1, 0xC0, 0x000000, 0x0, 0x0D, 0x20, 0x16, 0x0A, 0x50, 0x15, 0x00, 0x18,},
	{25000000, 2500000000, 0x1, 0xC8, 0x000000, 0x0, 0x0E, 0x21, 0x16, 0x0B, 0x53, 0x16, 0x00, 0x18,},
#elif (M31_DPHY_REFCLK == M31_DPHY_REFCLK_12M)
     {12000000, 160000000, 0x0, 0x6a, 0xaa<<16|0xaa<<8|0xaa, 0x3, 0xa, 0x17, 0x11, 0x5, 0x2b, 0xd, 0x7, 0x3d,},
	{12000000, 170000000, 0x0, 0x71, 0x55<<16|0x55<<8|0x55, 0x3, 0xb, 0x18, 0x11, 0x5, 0x2e, 0xd, 0x7, 0x3d,},
	{12000000, 180000000, 0x0, 0x78, 0x0<<16|0x0<<8|0x0, 0x3, 0xb, 0x19, 0x12, 0x6, 0x30, 0xe, 0x7, 0x3e,},
	{12000000, 190000000, 0x0, 0x7e, 0xaa<<16|0xaa<<8|0xaa, 0x3, 0xc, 0x1a, 0x12, 0x6, 0x33, 0xe, 0x7, 0x3e,},
	{12000000, 200000000, 0x0, 0x85, 0x55<<16|0x55<<8|0x55, 0x3, 0xc, 0x1b, 0x13, 0x7, 0x35, 0xf, 0x7, 0x3f,},
	{12000000, 320000000, 0x0, 0x6a, 0xaa<<16|0xaa<<8|0xaa, 0x2, 0x8, 0x14, 0xf, 0x5, 0x2b, 0xd, 0x3, 0x23,},
	{12000000, 330000000, 0x0, 0x6e, 0x0<<16|0x0<<8|0x0, 0x2, 0x8, 0x15, 0xf, 0x5, 0x2d, 0xd, 0x3, 0x23,},
	{12000000, 340000000, 0x0, 0x71, 0x55<<16|0x55<<8|0x55, 0x2, 0x9, 0x15, 0xf, 0x5, 0x2e, 0xd, 0x3, 0x23,},
	{12000000, 350000000, 0x0, 0x74, 0xaa<<16|0xaa<<8|0xaa, 0x2, 0x9, 0x15, 0x10, 0x6, 0x2f, 0xe, 0x3, 0x24,},
	{12000000, 360000000, 0x0, 0x78, 0x0<<16|0x0<<8|0x0, 0x2, 0x9, 0x16, 0x10, 0x6, 0x30, 0xe, 0x3, 0x24,},
	{12000000, 370000000, 0x0, 0x7b, 0x55<<16|0x55<<8|0x55, 0x2, 0x9, 0x17, 0x10, 0x6, 0x32, 0xe, 0x3, 0x24,},
	{12000000, 380000000, 0x0, 0x7e, 0xaa<<16|0xaa<<8|0xaa, 0x2, 0xa, 0x17, 0x10, 0x6, 0x33, 0xe, 0x3, 0x24,},
	{12000000, 390000000, 0x0, 0x82, 0x0<<16|0x0<<8|0x0, 0x2, 0xa, 0x17, 0x11, 0x6, 0x35, 0xf, 0x3, 0x25,},
	{12000000, 400000000, 0x0, 0x85, 0x55<<16|0x55<<8|0x55, 0x2, 0xa, 0x18, 0x11, 0x7, 0x35, 0xf, 0x3, 0x25,},
	{12000000, 410000000, 0x0, 0x88, 0xaa<<16|0xaa<<8|0xaa, 0x2, 0xa, 0x19, 0x11, 0x7, 0x37, 0xf, 0x3, 0x25,},
	{12000000, 420000000, 0x0, 0x8c, 0x0<<16|0x0<<8|0x0, 0x2, 0xa, 0x19, 0x12, 0x7, 0x38, 0x10, 0x3, 0x26,},
	{12000000, 430000000, 0x0, 0x8f, 0x55<<16|0x55<<8|0x55, 0x2, 0xb, 0x19, 0x12, 0x7, 0x39, 0x10, 0x3, 0x26,},
	{12000000, 440000000, 0x0, 0x92, 0xaa<<16|0xaa<<8|0xaa, 0x2, 0xb, 0x1a, 0x12, 0x7, 0x3b, 0x10, 0x3, 0x26,},
	{12000000, 450000000, 0x0, 0x96, 0x0<<16|0x0<<8|0x0, 0x2, 0xb, 0x1b, 0x12, 0x8, 0x3c, 0x10, 0x3, 0x26,},
	{12000000, 460000000, 0x0, 0x99, 0x55<<16|0x55<<8|0x55, 0x2, 0xb, 0x1b, 0x13, 0x8, 0x3d, 0x11, 0x3, 0x27,},
	{12000000, 470000000, 0x0, 0x9c, 0xaa<<16|0xaa<<8|0xaa, 0x2, 0xc, 0x1b, 0x13, 0x8, 0x3e, 0x11, 0x3, 0x27,},
	{12000000, 480000000, 0x0, 0xa0, 0x0<<16|0x0<<8|0x0, 0x2, 0xc, 0x1c, 0x13, 0x8, 0x40, 0x11, 0x3, 0x27,},
	{12000000, 490000000, 0x0, 0xa3, 0x55<<16|0x55<<8|0x55, 0x2, 0xc, 0x1d, 0x14, 0x8, 0x42, 0x12, 0x3, 0x28,},
	{12000000, 500000000, 0x0, 0xa6, 0xaa<<16|0xaa<<8|0xaa, 0x2, 0xc, 0x1d, 0x14, 0x9, 0x42, 0x12, 0x3, 0x28,},
	{12000000, 510000000, 0x0, 0xaa, 0x0<<16|0x0<<8|0x0, 0x2, 0xc, 0x1e, 0x14, 0x9, 0x44, 0x12, 0x3, 0x28,},
	{12000000, 520000000, 0x0, 0xad, 0x55<<16|0x55<<8|0x55, 0x2, 0xd, 0x1e, 0x15, 0x9, 0x45, 0x13, 0x3, 0x29,},
	{12000000, 530000000, 0x0, 0xb0, 0xaa<<16|0xaa<<8|0xaa, 0x2, 0xd, 0x1e, 0x15, 0x9, 0x47, 0x13, 0x3, 0x29,},
	{12000000, 540000000, 0x0, 0xb4, 0x0<<16|0x0<<8|0x0, 0x2, 0xd, 0x1f, 0x15, 0x9, 0x48, 0x13, 0x3, 0x29,},
	{12000000, 550000000, 0x0, 0xb7, 0x55<<16|0x55<<8|0x55, 0x2, 0xd, 0x20, 0x16, 0x9, 0x4a, 0x14, 0x3, 0x2a,},
	{12000000, 560000000, 0x0, 0xba, 0xaa<<16|0xaa<<8|0xaa, 0x2, 0xe, 0x20, 0x16, 0xa, 0x4a, 0x14, 0x3, 0x2a,},
	{12000000, 570000000, 0x0, 0xbe, 0x0<<16|0x0<<8|0x0, 0x2, 0xe, 0x20, 0x16, 0xa, 0x4c, 0x14, 0x3, 0x2a,},
	{12000000, 580000000, 0x0, 0xc1, 0x55<<16|0x55<<8|0x55, 0x2, 0xe, 0x21, 0x16, 0xa, 0x4d, 0x14, 0x3, 0x2a,},
	{12000000, 590000000, 0x0, 0xc4, 0xaa<<16|0xaa<<8|0xaa, 0x2, 0xe, 0x22, 0x17, 0xa, 0x4f, 0x15, 0x3, 0x2b,},
	{12000000, 600000000, 0x0, 0xc8, 0x0<<16|0x0<<8|0x0, 0x2, 0xe, 0x23, 0x17, 0xa, 0x50, 0x15, 0x3, 0x2b,},
	{12000000, 610000000, 0x0, 0xcb, 0x55<<16|0x55<<8|0x55, 0x2, 0xf, 0x22, 0x17, 0xb, 0x50, 0x15, 0x3, 0x2b,},
	{12000000, 620000000, 0x0, 0xce, 0xaa<<16|0xaa<<8|0xaa, 0x2, 0xf, 0x23, 0x18, 0xb, 0x52, 0x16, 0x3, 0x2c,},
	{12000000, 630000000, 0x0, 0x69, 0x0<<16|0x0<<8|0x0, 0x1, 0x7, 0x12, 0xd, 0x5, 0x2a, 0xc, 0x1, 0x15,},
	{12000000, 640000000, 0x0, 0x6a, 0xaa<<16|0xaa<<8|0xaa, 0x1, 0x7, 0x13, 0xe, 0x5, 0x2b, 0xd, 0x1, 0x16,},
	{12000000, 650000000, 0x0, 0x6c, 0x55<<16|0x55<<8|0x55, 0x1, 0x7, 0x13, 0xe, 0x5, 0x2c, 0xd, 0x1, 0x16,},
	{12000000, 660000000, 0x0, 0x6e, 0x0<<16|0x0<<8|0x0, 0x1, 0x7, 0x13, 0xe, 0x5, 0x2d, 0xd, 0x1, 0x16,},
	{12000000, 670000000, 0x0, 0x6f, 0xaa<<16|0xaa<<8|0xaa, 0x1, 0x8, 0x13, 0xe, 0x5, 0x2d, 0xd, 0x1, 0x16,},
	{12000000, 680000000, 0x0, 0x71, 0x55<<16|0x55<<8|0x55, 0x1, 0x8, 0x13, 0xe, 0x5, 0x2e, 0xd, 0x1, 0x16,},
	{12000000, 690000000, 0x0, 0x73, 0x0<<16|0x0<<8|0x0, 0x1, 0x8, 0x14, 0xe, 0x6, 0x2e, 0xd, 0x1, 0x16,},
	{12000000, 700000000, 0x0, 0x74, 0xaa<<16|0xaa<<8|0xaa, 0x1, 0x8, 0x14, 0xf, 0x6, 0x2f, 0xe, 0x1, 0x16,},
	{12000000, 710000000, 0x0, 0x76, 0x55<<16|0x55<<8|0x55, 0x1, 0x8, 0x14, 0xf, 0x6, 0x2f, 0xe, 0x1, 0x17,},
	{12000000, 720000000, 0x0, 0x78, 0x0<<16|0x0<<8|0x0, 0x1, 0x8, 0x15, 0xf, 0x6, 0x30, 0xe, 0x1, 0x17,},
	{12000000, 730000000, 0x0, 0x79, 0xaa<<16|0xaa<<8|0xaa, 0x1, 0x8, 0x15, 0xf, 0x6, 0x31, 0xe, 0x1, 0x17,},
	{12000000, 740000000, 0x0, 0x7b, 0x55<<16|0x55<<8|0x55, 0x1, 0x8, 0x15, 0xf, 0x6, 0x32, 0xe, 0x1, 0x17,},
	{12000000, 750000000, 0x0, 0x7d, 0x0<<16|0x0<<8|0x0, 0x1, 0x8, 0x16, 0xf, 0x6, 0x32, 0xe, 0x1, 0x17,},
	{12000000, 760000000, 0x0, 0x7e, 0xaa<<16|0xaa<<8|0xaa, 0x1, 0x9, 0x15, 0xf, 0x6, 0x33, 0xe, 0x1, 0x17,},
	{12000000, 770000000, 0x0, 0x80, 0x55<<16|0x55<<8|0x55, 0x1, 0x9, 0x15, 0x10, 0x6, 0x34, 0xf, 0x1, 0x18,},
	{12000000, 780000000, 0x0, 0x82, 0x0<<16|0x0<<8|0x0, 0x1, 0x9, 0x16, 0x10, 0x6, 0x35, 0xf, 0x1, 0x18,},
	{12000000, 790000000, 0x0, 0x83, 0xaa<<16|0xaa<<8|0xaa, 0x1, 0x9, 0x16, 0x10, 0x7, 0x34, 0xf, 0x1, 0x18,},
	{12000000, 800000000, 0x0, 0x85, 0x55<<16|0x55<<8|0x55, 0x1, 0x9, 0x17, 0x10, 0x7, 0x35, 0xf, 0x1, 0x18,},
	{12000000, 810000000, 0x0, 0x87, 0x0<<16|0x0<<8|0x0, 0x1, 0x9, 0x17, 0x10, 0x7, 0x36, 0xf, 0x1, 0x18,},
	{12000000, 820000000, 0x0, 0x88, 0xaa<<16|0xaa<<8|0xaa, 0x1, 0x9, 0x17, 0x10, 0x7, 0x37, 0xf, 0x1, 0x18,},
	{12000000, 830000000, 0x0, 0x8a, 0x55<<16|0x55<<8|0x55, 0x1, 0x9, 0x18, 0x10, 0x7, 0x37, 0xf, 0x1, 0x18,},
	{12000000, 840000000, 0x0, 0x8c, 0x0<<16|0x0<<8|0x0, 0x1, 0x9, 0x18, 0x11, 0x7, 0x38, 0x10, 0x1, 0x19,},
	{12000000, 850000000, 0x0, 0x8d, 0xaa<<16|0xaa<<8|0xaa, 0x1, 0xa, 0x17, 0x11, 0x7, 0x39, 0x10, 0x1, 0x19,},
	{12000000, 860000000, 0x0, 0x8f, 0x55<<16|0x55<<8|0x55, 0x1, 0xa, 0x18, 0x11, 0x7, 0x39, 0x10, 0x1, 0x19,},
	{12000000, 870000000, 0x0, 0x91, 0x0<<16|0x0<<8|0x0, 0x1, 0xa, 0x18, 0x11, 0x7, 0x3a, 0x10, 0x1, 0x19,},
	{12000000, 880000000, 0x0, 0x92, 0xaa<<16|0xaa<<8|0xaa, 0x1, 0xa, 0x18, 0x11, 0x7, 0x3b, 0x10, 0x1, 0x19,},
	{12000000, 890000000, 0x0, 0x94, 0x55<<16|0x55<<8|0x55, 0x1, 0xa, 0x19, 0x11, 0x7, 0x3c, 0x10, 0x1, 0x19,},
	{12000000, 900000000, 0x0, 0x96, 0x0<<16|0x0<<8|0x0, 0x1, 0xa, 0x19, 0x12, 0x8, 0x3c, 0x10, 0x1, 0x19,},
	{12000000, 910000000, 0x0, 0x97, 0xaa<<16|0xaa<<8|0xaa, 0x1, 0xa, 0x19, 0x12, 0x8, 0x3c, 0x11, 0x1, 0x1a,},
	{12000000, 920000000, 0x0, 0x99, 0x55<<16|0x55<<8|0x55, 0x1, 0xa, 0x1a, 0x12, 0x8, 0x3d, 0x11, 0x1, 0x1a,},
	{12000000, 930000000, 0x0, 0x9b, 0x0<<16|0x0<<8|0x0, 0x1, 0xa, 0x1a, 0x12, 0x8, 0x3e, 0x11, 0x1, 0x1a,},
	{12000000, 940000000, 0x0, 0x9c, 0xaa<<16|0xaa<<8|0xaa, 0x1, 0xb, 0x1a, 0x12, 0x8, 0x3e, 0x11, 0x1, 0x1a,},
	{12000000, 950000000, 0x0, 0x9e, 0x55<<16|0x55<<8|0x55, 0x1, 0xb, 0x1a, 0x12, 0x8, 0x3f, 0x11, 0x1, 0x1a,},
	{12000000, 960000000, 0x0, 0xa0, 0x0<<16|0x0<<8|0x0, 0x1, 0xb, 0x1a, 0x12, 0x8, 0x40, 0x11, 0x1, 0x1a,},
	{12000000, 970000000, 0x0, 0xa1, 0xaa<<16|0xaa<<8|0xaa, 0x1, 0xb, 0x1b, 0x13, 0x8, 0x41, 0x12, 0x1, 0x1b,},
	{12000000, 980000000, 0x0, 0xa3, 0x55<<16|0x55<<8|0x55, 0x1, 0xb, 0x1b, 0x13, 0x8, 0x42, 0x12, 0x1, 0x1b,},
	{12000000, 990000000, 0x0, 0xa5, 0x0<<16|0x0<<8|0x0, 0x1, 0xb, 0x1b, 0x13, 0x8, 0x42, 0x12, 0x1, 0x1b,},
	{12000000, 1000000000, 0x0, 0xa6, 0xaa<<16|0xaa<<8|0xaa, 0x1, 0xb, 0x1c, 0x13, 0x9, 0x42, 0x12, 0x1, 0x1b,},

#endif
};


struct dsi_sf_priv {
	void __iomem *dsi_reg;
	void __iomem *phy_reg;//0x295e0000
	void __iomem *sys_reg;
	struct mipi_dsi_device device;
	struct udevice *panel;
	struct udevice *dsi_host;
	unsigned int data_lanes;

	struct clk dsi_sys_clk;
	struct clk apb_clk;
	struct clk txesc_clk;
	struct clk dpi_clk;
	struct clk dphy_txesc_clk;

	struct reset_ctl dpi_rst;
	struct reset_ctl apb_rst;
	struct reset_ctl rxesc_rst;
	struct reset_ctl sys_rst;
	struct reset_ctl txbytehs_rst;
	struct reset_ctl txesc_rst;
	struct reset_ctl dphy_sys;
	struct reset_ctl dphy_txbytehs;

    uint32_t direct_cmd_fifo_depth;
	uint32_t rx_fifo_depth;
    int direct_cmd_comp;
    bool link_initialized;
};

int rk_mipi_read_timing(struct udevice *dev,
			       struct display_timing *timing);

int rk_mipi_dsi_enable(struct udevice *dev,
			      const struct display_timing *timing);

int rk_mipi_phy_enable(struct udevice *dev);


#endif
