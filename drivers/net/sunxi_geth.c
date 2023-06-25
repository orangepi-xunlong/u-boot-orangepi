/*
 * sunxi_geth.c: Allwinnertech Gigabit Ethernet u-boot driver
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *	Author: Sugar <shuge@allwinnertech.com>
 *		fuzhaoke <fuzhaoke@allwinnertech.com>
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <linux/types.h>
#include <common.h>
#include <asm/io.h>
#include <memalign.h>
#include <net.h>
#include <malloc.h>
#include <linux/mii.h>
#include <netdev.h>
#include <errno.h>
#include <sys_config.h>
#include <linux/string.h>
#include <fdt_support.h>
#include <miiphy.h>
#include <phy.h>
#if CONFIG_RTL8363_NB
#include <rtk8363.h>
#include <smi.h>
#include <rtk_types.h>
#endif

#if defined(CONFIG_SUN50IW2P1)
#define IOBASE			0x01c30000
#define PHY_CLK_REG		(0x01c00000 + 0x30)
#define CCMU_BASE		0x01c20000
#define AHB1_GATING		(0x60)
#define AHB1_RESET		(0x02c0)
#define AHB1_GATING_BIT         (1 << 17)
#define AHB1_RESET_BIT          (1 << 17)
#elif defined(CONFIG_SUN8IW11P1)
#define IOBASE			0x01c50000
#define PHY_CLK_REG		(0x01c20000 + 0x164)
#define CCMU_BASE		0x01c20000
#define AHB1_GATING		(0x64)
#define AHB1_RESET		(0x02c4)
#define AHB1_GATING_BIT         (1 << 17)
#define AHB1_RESET_BIT          (1 << 17)
#elif defined(CONFIG_SUN50IW6P1)
#define IOBASE                  0x05020000
#define PHY_CLK_REG             (0x03000000 + 0x30)
#define CCMU_BASE               0x03001000
#define CCMU_GMAC_CLK_REG       0x097c
#define CCMU_GMAC_RST_BIT       16
#define CCMU_GMAC_GATING_BIT    0
#elif defined(CONFIG_MACH_SUN50IW10)
#define IOBASE			0x05020000
#define PHY_CLK_REG		(0x03000000 + 0x30)
#define CCMU_BASE		0x03001000
#define CCMU_GMAC_CLK_REG	0x097c
#define CCMU_GMAC_RST_BIT	16
#define CCMU_GMAC_GATING_BIT	0
#define CCMU_EPHY_CLK_REG	0x0970
#define CCMU_EPHY_GATING_BIT	31
#define CONFIG_SUNXI_EXT_PHY
#elif defined(CONFIG_SUN8IW12P1) || defined(CONFIG_SUN8IW12P1_NOR)
#define IOBASE                  0x05020000
#define PHY_CLK_REG             (0x03000000 + 0x30)
#define CCMU_BASE               0x03001000
#define CCMU_GMAC_CLK_REG       0x097c
#define CCMU_GMAC_RST_BIT       16
#define CCMU_GMAC_GATING_BIT    0
#define CCMU_EPHY_CLK_REG       0x0970
#define CCMU_EPHY_GATING_BIT    31
#define DISABLE_AUTONEG
#elif defined(CONFIG_MACH_SUN50IW9) || defined(CONFIG_MACH_SUN8IW19)
#define IOBASE                  0x05020000
#define PHY_CLK_REG             (0x03000000 + 0x30)
#define CCMU_BASE               0x03001000
#define CCMU_GMAC_CLK_REG       0x097c
#define CCMU_GMAC_RST_BIT       16
#define CCMU_GMAC_GATING_BIT    0
#define CCMU_EPHY_CLK_REG       0x0970
#define CCMU_EPHY_GATING_BIT    31
#define DISABLE_AUTONEG
#if defined(CONFIG_MACH_SUN50IW9)
#define CONFIG_HARD_CHECKSUM
#endif
#define CONFIG_SUNXI_EXT_PHY
#elif defined(CONFIG_MACH_SUN8IW20) || defined(CONFIG_MACH_SUN20IW1) || defined(CONFIG_MACH_SUN8IW21)
#define IOBASE                  0x04500000
#define PHY_CLK_REG             (0x03000000 + 0x30)  /* SYS_CFG_BASE + EMAC_EPHY_CLK_REG0 */
#define CCMU_BASE               0x02001000  /* CCMU Base Address */
#define CCMU_GMAC_CLK_REG       0x097c	/* GMAC_BGR_REG */
#define CCMU_GMAC_RST_BIT       16	/* GMAC_RST */
#define CCMU_GMAC_GATING_BIT    0	/* GMAC_GATING */
#define CCMU_EPHY_CLK_REG       0x0970	/* GMAC_25M_CLK_REG */
#define CCMU_EPHY_GATING_BIT    31	/* GMAC_25M_CLK_GATING */
#define DISABLE_AUTONEG
//#define CONFIG_HARD_CHECKSUM	/* EMAC_RX_CTL0 bit27: CHECK_CRC */
#define CONFIG_SUNXI_EXT_PHY

#ifdef CONFIG_MACH_SUN8IW21
#define NOT_SUPPORT_NOCACHED_ALLOC
#define CONFIG_HARD_CHECKSUM
#endif

#ifdef CONFIG_MACH_SUN20IW1
/*
 * D1 need restart DMA_EN.
 * If we only set DMA_START, the link-down err will happen.
 */
#define RESET_DMA_EN
/*
 * D1 does not have noncached_* API supported.
 * We will use flush_cache() to sync the cache.
 */
#define NOT_SUPPORT_NOCACHED_ALLOC
/*
 * D1 board actually only use the specific PHY
 */
#define PHY_RTL8211F
#endif
#else
#error "There is not any platform support SUNXI_GETH here!"
#endif

#define GETH_BASIC_CTL0        0x00
#define GETH_BASIC_CTL1        0x04
#define GETH_INT_STA           0x08
#define GETH_INT_EN            0x0C
#define GETH_TX_CTL0           0x10
#define GETH_TX_CTL1           0x14
#define GETH_TX_FLOW_CTL       0x1C
#define GETH_TX_DESC_LIST      0x20
#define GETH_RX_CTL0           0x24
#define GETH_RX_CTL1           0x28
#define GETH_RX_DESC_LIST      0x34
#define GETH_RX_FRM_FLT        0x38
#define GETH_RX_HASH0          0x40
#define GETH_RX_HASH1          0x44
#define GETH_MDIO_ADDR         0x48
#define GETH_MDIO_DATA         0x4C
#define GETH_ADDR_HI(reg)      (0x50 + ((reg) << 3))
#define GETH_ADDR_LO(reg)      (0x54 + ((reg) << 3))
#define GETH_TX_DMA_STA        0xB0
#define GETH_TX_CUR_DESC       0xB4
#define GETH_TX_CUR_BUF        0xB8
#define GETH_RX_DMA_STA        0xC0
#define GETH_RX_CUR_DESC       0xC4
#define GETH_RX_CUR_BUF        0xC8
#define GETH_RGMII_STA         0xD0
#define MII_BUSY               0x00000001
#define MII_WRITE              0x00000002


/* PHY address */
#define PHY_DM                 0x0010
#define PHY_AUTO_NEG           0x0020
#define PHY_POWERDOWN          0x0080
#define PHY_NEG_EN             0x1000
#define PHY_

enum rx_frame_status { /* IPC status */
	good_frame = 0,
	discard_frame = 1,
	csum_none = 2,
	llc_snap = 4,
};

typedef union {
	struct {
		/* TDES0 */
		u32 deferred:1;         /* Deferred bit (only half-duplex) */
		u32 under_err:1;        /* Underflow error */
		u32 ex_deferral:1;      /* Excessive deferral */
		u32 coll_cnt:4;         /* Collision count */
		u32 vlan_tag:1;         /* VLAN Frame */

		u32 ex_coll:1;          /* Excessive collision */
		u32 late_coll:1;        /* Late collision */
		u32 no_carr:1;          /* No carrier */
		u32 loss_carr:1;        /* Loss of collision */

		u32 ipdat_err:1;        /* IP payload error */
		u32 frm_flu:1;          /* Frame flushed */
		u32 jab_timeout:1;      /* Jabber timeout */
		u32 err_sum:1;          /* Error summary */

		u32 iphead_err:1;       /* IP header error */
		u32 ttss:1;             /* Transmit time stamp status */
		u32 reserved0:13;
		u32 own:1;              /* Own bit. CPU:0, DMA:1 */
	} tx;

	struct {
		/* RDES0 */
		u32 chsum_err:1;        /* Payload checksum error */
		u32 crc_err:1;          /* CRC error */
		u32 dribbling:1;        /* Dribble bit error */
		u32 mii_err:1;          /* Received error (bit3) */

		u32 recv_wt:1;          /* Received watchdog timeout */
		u32 frm_type:1;         /* Frame type */
		u32 late_coll:1;        /* Late Collision */
		u32 ipch_err:1;         /* IPv header checksum error (bit7) */

		u32 last_desc:1;        /* Laset descriptor */
		u32 first_desc:1;       /* First descriptor */
		u32 vlan_tag:1;         /* VLAN Tag */
		u32 over_err:1;         /* Overflow error (bit11) */

		u32 len_err:1;          /* Length error */
		u32 sou_filter:1;       /* Source address filter fail */
		u32 desc_err:1;         /* Descriptor error */
		u32 err_sum:1;          /* Error summary (bit15) */

		u32 frm_len:14;         /* Frame length */
		u32 des_filter:1;       /* Destination address filter fail */
		u32 own:1;              /* Own bit. CPU:0, DMA:1 */
		#define RX_PKT_OK               0x7FFFB77C
		#define RX_LEN                  0x3FFF0000
	} rx;

	u32 all;
} desc0_u;

typedef union {
	struct {
		/* TDES1 */
		u32 buf1_size:11;       /* Transmit buffer1 size */
		u32 buf2_size:11;       /* Transmit buffer2 size */
		u32 ttse:1;             /* Transmit time stamp enable */
		u32 dis_pad:1;          /* Disable pad (bit23) */

		u32 adr_chain:1;        /* Second address chained */
		u32 end_ring:1;         /* Transmit end of ring */
		u32 crc_dis:1;          /* Disable CRC */
		u32 cic:2;              /* Checksum insertion control (bit27:28) */
		u32 first_sg:1;         /* First Segment */
		u32 last_seg:1;         /* Last Segment */
		u32 interrupt:1;        /* Interrupt on completion */
	} tx;

	struct {
		/* RDES1 */
		u32 buf1_size:11;       /* Received buffer1 size */
		u32 buf2_size:11;       /* Received buffer2 size */
		u32 reserved1:2;

		u32 adr_chain:1;        /* Second address chained */
		u32 end_ring:1;         /* Received end of ring */
		u32 reserved2:5;
		u32 dis_ic:1;           /* Disable interrupt on completion */
	} rx;

	u32 all;
} desc1_u;

typedef struct dma_desc {
       desc0_u desc0;	/* 1st: Status */
       desc1_u desc1;	/* 2nd: Buffer Size */
       u32 *desc2;	/* 3rd: Buffer Addr */
       u32 *desc3;	/* 4th: Next Desc */
} __attribute__((packed)) dma_desc_t;

#define EXT_PHY 0
#define INT_PHY 1
#if CONFIG_RTL8363_NB
struct eth_device *dev;
#endif
static struct dma_desc *dma_desc_tx;
static struct dma_desc *dma_desc_rx;
static char *rx_packet;
static char *rx_handle_buf;
static unsigned int used_type = INT_PHY;
static phy_interface_t phy_interface = PHY_INTERFACE_MODE_MII;
static unsigned int phy_addr = 0x1f;
//#define PKT_DEBUG
#ifdef PKT_DEBUG
static void pkt_hex_dump(char *prefix_str, void *pkt, unsigned int len)
{
	int i;
	unsigned char *data = (unsigned char *)pkt;
	for (i = 0; i < len; i++) {
		if (!(i % 16))
			printf("\n%s %08x:", prefix_str, i);

		printf(" %02x", data[i]);
	}
	printf("\n");
}
#else
#define pkt_hex_dump(a, b, c)  {}
#endif

#if DEBUG
static void geth_regs_show(struct eth_device *dev)
{
	void *reg;
	u32 val;
	/* Shows in the same format as linux command. Makes it easier to compare text:
	 * `cd /sys/class/sunxi_dump; echo 0x04500000, 0x045000d0 > dump; cat dump`
	 */
	for (u32 i = 0; i <= 0x0d0; i += 4) {
		reg = (void *)(dev->iobase + i);
		val = readl(reg);
		if ((i != 0) && (i % 16 == 0))
			printf("\n");
		if (i % 16 == 0)
			printf("0x%p: ", reg);
		printf("0x%08x ", val);
	}
	printf("\n");
}

static void phy_regs_show(struct eth_device *dev)
{
	u16 reg;
	u16 val;
	/*
	 * dump phy regs
	 */
	for (u16 i = 0; i <= 0xf; i += 1) {
		reg = (0x00 + i);
		val = geth_phy_read(dev, phy_addr, reg);
		if ((i != 0) && (i % 4 == 0))
			printf("\n");
		if (i % 4 == 0)
			printf("0x%02x: ", reg);
		printf("0x%04x ", val);
	}
	printf("\n");
}
#endif

static void random_ether_addr(u8 *addr)
{
#if 0
	int i;
	unsigned long long rand;

	for (i = 0; i < 6; i++) {
		rand = get_timer(0) * 0xfedf4fd;
		rand = rand * 0xd2634f967 + 0xea6f22ad8235;
		addr[i] = (uchar)(rand % 0xFF);
	}

	addr[0] &= 0xfe;
	addr[0] |= 0x02;
#endif
}

static u16 geth_phy_read(struct eth_device *dev, int phy_adr, u16 reg)
{
	unsigned long reg_val = 0;

	reg_val |= (0x06 << 20);
	reg_val |= (((phy_adr << 12) & (0x0001F000)) |
					((reg << 4) & (0x000007F0)) |
					MII_BUSY);

	writel(reg_val, (void *)(unsigned long)(dev->iobase + GETH_MDIO_ADDR));
	while (readl((void *)(unsigned long)(dev->iobase + GETH_MDIO_ADDR)) & MII_BUSY)
		;

	return  readl((void *)(unsigned long)(dev->iobase + GETH_MDIO_DATA));
}

static void geth_phy_write(struct eth_device *dev, u8 phy_adr, u8 reg, u16 data)
{
	int reg_val = 0;

	reg_val |= (0x06 << 20);
	reg_val |= (((phy_adr << 12) & (0x0001F000)) |
					((reg << 4) & (0x000007F0)) |
					MII_WRITE | MII_BUSY);

	writel(data, (void *)(unsigned long)(dev->iobase + GETH_MDIO_DATA));
	writel(reg_val, (void *)(unsigned long)(dev->iobase + GETH_MDIO_ADDR));

	/* Until operation is complete and exiting */
	while (readl((void *)(unsigned long)(dev->iobase + GETH_MDIO_ADDR)) & MII_BUSY)
		;
}

#if CONFIG_RTL8363_NB
int rtk_mdio_read(u32 len, u8 phy_adr, u8 reg, u32 *value)
{

	*value = geth_phy_read(dev, phy_adr, reg);

	return 0;
}

int rtk_mdio_write(u32 len, u8 phy_adr, u8 reg, u32 data)
{

	geth_phy_write(dev, phy_adr, reg, data);

	return 0;
}
#endif

int geth_miiphy_read(const char *devname, u8 phy_adr, u8 reg, u16 *value)
{
	struct eth_device *dev;

	dev = eth_get_dev_by_name(devname);
	*value = geth_phy_read(dev, phy_adr, reg);

	return 0;
}

int geth_miiphy_write(const char *devname, u8 phy_adr, u8 reg, u16 data)
{
	struct eth_device *dev;

	dev = eth_get_dev_by_name(devname);
	geth_phy_write(dev, phy_adr, reg, data);

	return 0;
}

#ifdef RESET_DMA_EN
static void dma_tx_enable(struct eth_device *dev, bool en)
{
	u32 reg_val;
	reg_val = readl((void *)(unsigned long)(dev->iobase + GETH_TX_CTL1));
	if (en)
		reg_val |= 0x40000000;
	else
		reg_val &= ~0x40000000;

	writel(reg_val, (void *)(unsigned long)(dev->iobase + GETH_TX_CTL1));
}
#endif
static int geth_xmit(struct eth_device *dev, void *packet, int length)
{
	u32 reg_val, xmit_stat;
	dma_desc_t *tx_p = dma_desc_tx;
	int tmo;

	/* Get transmit status */
	xmit_stat = readl((void *)(unsigned long)(dev->iobase + GETH_TX_DMA_STA)) & 0x7;

	/* Wait the Prev packet compled and timeout flush it */
	tmo = get_timer(0) + 5 * CONFIG_SYS_HZ;
	while (tx_p->desc0.tx.own || (xmit_stat != 0x00
			&& xmit_stat != 0x06)) {
		if (get_timer(0) > tmo)
			break;
	}
#ifdef RESET_DMA_EN
	dma_tx_enable(dev, false);
	/* clear the tx interrupt */
	reg_val = readl((void *)(unsigned long)(dev->iobase + GETH_INT_STA));
	reg_val |= 0x3F;
	writel(reg_val, (void *)(unsigned long)(dev->iobase + GETH_INT_STA));
#endif
	/* configure transmit dma descriptor */
	tx_p->desc0.all = 0x80000000;   /* Set Own */
	tx_p->desc1.all = 0x61000000;
#ifdef CONFIG_HARD_CHECKSUM
	tx_p->desc1.all |= (0x3 << 27); /* CIC Full */
#endif
	tx_p->desc1.all |= (((1 << 11) - 1) & length);
	tx_p->desc2 = (u32 *)packet;

#ifdef NOT_SUPPORT_NOCACHED_ALLOC
	flush_cache((unsigned long)tx_p, sizeof(*tx_p));
#endif
	flush_cache((unsigned long)packet, length);
	/* flush Transmit FIFO */
	reg_val = readl((void *)(unsigned long)(dev->iobase + GETH_TX_CTL1));
	reg_val |= 0x00000001;
	writel(reg_val, (void *)(unsigned long)(dev->iobase + GETH_TX_CTL1));

	pkt_hex_dump("TX", (void *)packet, 64);
#ifdef RESET_DMA_EN
	writel((ulong)tx_p, (void *)(unsigned long)(dev->iobase + GETH_TX_DESC_LIST));
	dma_tx_enable(dev, true);
#else
	/* Enable transmit and Poll transmit */
	reg_val = readl((void *)(unsigned long)(dev->iobase + GETH_TX_CTL1));
	if (xmit_stat == 0x00)
		reg_val |= 0x40000000;
	else
		reg_val |= 0x80000000;
	writel(reg_val, (void *)(unsigned long)(dev->iobase + GETH_TX_CTL1));
#endif
	return 0;
}

/* frm_len is sometimes wrong, but ip->ip_len is not wrong */
void frm_len_check(uchar *in_packet, u32 *p)
{
	struct ethernet_hdr *et;
	struct ip_udp_hdr *ip;

	int eth_proto;

	et = (struct ethernet_hdr *)in_packet;

	if (*p < ETHER_HDR_SIZE)
		return;

	eth_proto = ntohs(et->et_protlen);

	if ((eth_proto != PROT_VLAN) && !(eth_proto < 1514))	/* normal packet */
		ip = (struct ip_udp_hdr *)(in_packet + ETHER_HDR_SIZE);
	else
		return;

	if (eth_proto == PROT_IP)
		*p = ntohs(ip->ip_len) + ETHER_HDR_SIZE;

	return;
}

static int rx_status(dma_desc_t *p)
{
	int ret = good_frame;

	if (p->desc0.rx.last_desc == 0)
		ret = discard_frame;

	if (p->desc0.rx.frm_type && (p->desc0.rx.chsum_err
			|| p->desc0.rx.ipch_err))
		ret = discard_frame;

	if (p->desc0.rx.err_sum)
		ret = discard_frame;

	if (p->desc0.rx.len_err)
		ret = discard_frame;

	if (p->desc0.rx.mii_err)
		ret = discard_frame;

	return ret;
}
#ifdef RESET_DMA_EN
static void dma_rx_enable(struct eth_device *dev, bool en)
{
	u32 reg_val;
	reg_val = readl((void *)(unsigned long)(dev->iobase + GETH_RX_CTL1));
	if (en)
		reg_val |= 0x40000000;
	else
		reg_val &= ~0x40000000;

	writel(reg_val, (void *)(unsigned long)(dev->iobase + GETH_RX_CTL1));
}
#endif
static int geth_recv(struct eth_device *dev)
{
	u32 len, recv_stat;
	dma_desc_t *rx_p = (void *)dma_desc_rx;

#ifdef NOT_SUPPORT_NOCACHED_ALLOC
	invalidate_dcache_range((ulong)rx_p, (ulong)((ulong)rx_p + ALIGN(sizeof(*rx_p), CACHE_LINE_SIZE)));
#endif

	if (rx_p->desc0.rx.own)
		return 0;

#ifdef NOT_SUPPORT_NOCACHED_ALLOC
	invalidate_dcache_range((ulong)rx_packet, (ulong)((ulong)rx_packet + ALIGN(2048, CACHE_LINE_SIZE)));
#endif
	/*
	 * FIXME: to prevent the Rx buffer that we are handling
	 * from being overwrited.
	 */
	memcpy(rx_handle_buf, rx_packet, 2048);

	recv_stat = readl((void *)(unsigned long)(dev->iobase + GETH_INT_STA));
	if (!(recv_stat & 0x2300))
		goto fill;

#ifdef RESET_DMA_EN
	dma_rx_enable(dev, false);
#endif
	writel(0x3F00, (void *)(unsigned long)(dev->iobase + GETH_INT_STA));

	recv_stat = rx_status(rx_p);
	if (recv_stat != discard_frame) {
		if (recv_stat != llc_snap)
			len = (rx_p->desc0.rx.frm_len - 4);
		else
			len = rx_p->desc0.rx.frm_len;

		pkt_hex_dump("RX", (void *)rx_handle_buf, 64);

		flush_cache((long unsigned int)rx_packet, 2048);

		frm_len_check((uchar *)rx_handle_buf, &len);

		net_process_received_packet((uchar *)rx_handle_buf, len);
	} else {
		/* Just need to clear 64 bits header */
		memset(rx_packet, 0, 64);
#ifdef NOT_SUPPORT_NOCACHED_ALLOC
		flush_cache((unsigned long)rx_packet, 64);
#endif
	}

fill:
	rx_p->desc0.all = 0x80000000;
	rx_p->desc1.all = 0x81000000;
	rx_p->desc1.all |= ((1 << 11) - 1);
	rx_p->desc2 = (void *)rx_packet;
#ifdef NOT_SUPPORT_NOCACHED_ALLOC
	flush_cache((unsigned long)rx_p, ALIGN(sizeof(*rx_p), CACHE_LINE_SIZE));
#endif

#ifdef RESET_DMA_EN
	writel((ulong)rx_p, (void *)(unsigned long)(dev->iobase + GETH_RX_DESC_LIST));
	dma_rx_enable(dev, true);
#else
	u32 reg_val;
	recv_stat = readl((void *)(unsigned long)(dev->iobase + GETH_RX_DMA_STA)) & 0x07;
	/* Enable receive and poll it */
	reg_val = readl((void *)(unsigned long)(dev->iobase + GETH_RX_CTL1));
	if (recv_stat == 0x00)
		reg_val |= 0x40000000;
	else
		reg_val |= 0x80000000;
	writel(reg_val, (void *)(unsigned long)(dev->iobase + GETH_RX_CTL1));
#endif

	return 0;
}

static int geth_sys_init(void)
{
	u32 reg_val;
	int value;
	char *phy_mode = NULL;
	int geth_nodeoffset;
	unsigned char i;
	u32 tx_delay = 0;
	u32 rx_delay = 0;
	u32 use_ephy_clk = 0;

/* it should be defined in uboot/include/configs/sun?iw?p?.h */
#ifdef CONFIG_SUNXI_EXT_PHY
	used_type = EXT_PHY;
#else
	used_type = INT_PHY;
#endif

	/* Enable clk for ephy */
	value = readl((void *)(unsigned long)PHY_CLK_REG);
	if (used_type == INT_PHY) {
		reg_val = readl((void *)(unsigned long)(CCMU_BASE + 0x0070));
		reg_val |= (1 << 0);
		writel(reg_val, (void *)(unsigned long)(CCMU_BASE + 0x0070));

		reg_val = readl((void *)(unsigned long)(CCMU_BASE + 0x02c8));
		reg_val |= (1 << 2);
		writel(reg_val, (void *)(unsigned long)(CCMU_BASE + 0x02c8));

		value |= (1 << 15);
		value &= ~(1 << 16);
		value |= (3 << 17);
	} else {  /* EXT_PHY */
		/* config PIO for gmac */
		geth_nodeoffset = fdt_path_offset(working_fdt, "gmac0");
		if (geth_nodeoffset < 0) {
			printf("%s:%d: get node 'gmac0' error\n", __func__, __LINE__);
			return -1;
		}

		if (0 != fdt_set_all_pin_by_offset(geth_nodeoffset, "pinctrl-0")) {
			printf("set pin for sunxi_geth error\n");
			return -1;
		}

		/* config PHY mode */
		if (fdt_getprop_string(working_fdt, geth_nodeoffset, "phy-mode", &phy_mode) < 0) {
			printf("get phy-mode fail!");
			return -1;
		}
		for (i = 0; i < ARRAY_SIZE(phy_interface_strings); i++) {
			if (!strcmp(phy_interface_strings[i], (const char *)phy_mode))
				break;
		}
		if (i == PHY_INTERFACE_MODE_NONE) {
			printf("there is no phy interface support!\n");
			return -1;
		} else {
			phy_interface = i;
			printf("phy_mode=%s, phy_interface=%d\n", phy_mode, i);
		}

		value &= ~(1 << 15);  /* PHY_SELECT = 0 (External PHY) */
		value |= (1 << 16);   /* 0:PowerUp; 1:Shutdown */

		/* config delay */
		if (fdt_getprop_u32(working_fdt, geth_nodeoffset, "tx-delay", &tx_delay) < 0) {
			printf("get tx-delay fail!");
			return -1;
		}

		if (fdt_getprop_u32(working_fdt, geth_nodeoffset, "rx-delay", &rx_delay) < 0) {
			printf("get rx-delay fail!");
			return -1;
		}

		if (fdt_getprop_u32(working_fdt, geth_nodeoffset,
					"use_ephy25m", &use_ephy_clk) < 0)
			printf("use_ephy25m is unkown.\n");
	}

	/* Set PHY clock, depend on phy mode */
	if (phy_interface == PHY_INTERFACE_MODE_RGMII) {
		value |= 0x00000004;
	} else {
		value &= ~0x00000004;
	}

	value &= ~0x00000003;
	if (phy_interface == PHY_INTERFACE_MODE_RGMII
	 || phy_interface == PHY_INTERFACE_MODE_GMII)
		value |= 0x00000002;
	else if (phy_interface == PHY_INTERFACE_MODE_RMII)
		value |= 0x00002001;

	/* Adjust Tx/Rx clock delay */
	value &= ~(0x07 << 10);
	value |= ((tx_delay & 0x07) << 10);
	value &= ~(0x1f << 5);
	value |= ((rx_delay & 0x1f) << 5);

	writel(value, (void *)(unsigned long)PHY_CLK_REG);

	/* enable clk for gmac */
#if defined(CONFIG_SUN50IW6P1) || defined(CONFIG_SUN8IW12P1) \
    || defined(CONFIG_SUN8IW12P1_NOR) || defined(CONFIG_MACH_SUN50IW9) \
    || defined(CONFIG_MACH_SUN8IW19) \
    || defined(CONFIG_MACH_SUN8IW20) || defined(CONFIG_MACH_SUN20IW1) \
    || defined(CONFIG_MACH_SUN50IW10) || defined(CONFIG_MACH_SUN8IW21)
	reg_val = readl((void *)(unsigned long)(CCMU_BASE + CCMU_GMAC_CLK_REG));
	reg_val |= (1 << CCMU_GMAC_RST_BIT);   /* Reset Deassert */
	reg_val |= (1 << CCMU_GMAC_GATING_BIT);
	writel(reg_val, (void *)(unsigned long)(CCMU_BASE + CCMU_GMAC_CLK_REG));
#else
	reg_val = readl((void *)(unsigned long)(CCMU_BASE + AHB1_GATING));
	reg_val |= AHB1_GATING_BIT;
	writel(reg_val, (void *)(unsigned long)(CCMU_BASE + AHB1_GATING));

	reg_val = readl((void *)(unsigned long)(CCMU_BASE + AHB1_RESET));
	reg_val |= AHB1_RESET_BIT;
	writel(reg_val, (void *)(unsigned long)(CCMU_BASE + AHB1_RESET));
#endif

	/* enable ephy clk */
	if (use_ephy_clk == 1) {
		printf("gmac: *** using ephy_clk ***\n");
		reg_val = readl((void *)(unsigned long)(CCMU_BASE + CCMU_EPHY_CLK_REG));
#if defined(CONFIG_MACH_SUN50IW9) || defined(CONFIG_MACH_SUN8IW19) || \
    defined(CONFIG_MACH_SUN8IW20) || defined(CONFIG_MACH_SUN20IW1) || \
    defined(CONFIG_MACH_SUN50IW10) || defined(CONFIG_MACH_SUN8IW21)
		/* set CCMU_EPHY_CLK_REG's bit30 and bit31 to 1 */
		reg_val |= (3 << (CCMU_EPHY_GATING_BIT - 1));
#else
		reg_val |= (1 << CCMU_EPHY_GATING_BIT);
#endif
		writel(reg_val, (void *)(unsigned long)(CCMU_BASE + CCMU_EPHY_CLK_REG));
	}

	return 0;
}

static int mii_phy_init(struct eth_device *dev)
{
	int reg_val;
#if CONFIG_RTL8363_NB
	u32 phy_val;
#else
	u16 phy_val;
#endif
	int i = 0;

	for (i = 0; i < 32; i++) {
		reg_val = (int)(geth_phy_read(dev, i, MII_PHYSID1)
							& 0xffff) << 16;
		reg_val |= (int)(geth_phy_read(dev, i, MII_PHYSID2) & 0xffff);

		if ((reg_val & 0x1fffffff) == 0x1fffffff)
			continue;

		phy_addr = i;
		break;
	}

	if (used_type == INT_PHY) {
		geth_phy_write(dev, phy_addr, 0x1f, 0x013d);
		geth_phy_write(dev, phy_addr, 0x10, 0x3ffe);
		geth_phy_write(dev, phy_addr, 0x1f, 0x063d);
		geth_phy_write(dev, phy_addr, 0x13, 0x8000);
		geth_phy_write(dev, phy_addr, 0x1f, 0x023d);
		geth_phy_write(dev, phy_addr, 0x18, 0x1000);
		geth_phy_write(dev, phy_addr, 0x1f, 0x063d);
		geth_phy_write(dev, phy_addr, 0x15, 0x132c);
		geth_phy_write(dev, phy_addr, 0x1f, 0x013d);
		geth_phy_write(dev, phy_addr, 0x13, 0xd602);
		geth_phy_write(dev, phy_addr, 0x17, 0x003b);
		geth_phy_write(dev, phy_addr, 0x1f, 0x063d);
		geth_phy_write(dev, phy_addr, 0x14, 0x7088);
		geth_phy_write(dev, phy_addr, 0x1f, 0x033d);
		geth_phy_write(dev, phy_addr, 0x11, 0x8530);
		geth_phy_write(dev, phy_addr, 0x1f, 0x003d);
		//phy_val = geth_phy_read(dev, phy_addr, 0x3c);
		//geth_phy_write(dev, phy_addr, 0x3c, phy_val & 0x02);
	} else {
#ifdef CONFIG_PHY_SUNXI_ACX00
		/* Iint ephy */
		geth_phy_write(dev, phy_addr, 0x1f, 0x0100);	/* Switch to Page 1 */
		geth_phy_write(dev, phy_addr, 0x12, 0x4824);	/* Disable APS */

		geth_phy_write(dev, phy_addr, 0x1f, 0x0200);	/* Switch to Page 2 */
		geth_phy_write(dev, phy_addr, 0x18, 0x0000);	/* PHYAFE TRX optimization */

		geth_phy_write(dev, phy_addr, 0x1f, 0x0600);	/* Switch to Page 6 */
		geth_phy_write(dev, phy_addr, 0x14, 0x708f);	/* PHYAFE TX optimization */
		geth_phy_write(dev, phy_addr, 0x13, 0xF000);	/* PHYAFE RX optimization */
		geth_phy_write(dev, phy_addr, 0x15, 0x1530);

		geth_phy_write(dev, phy_addr, 0x1f, 0x0800);	/* Switch to Page 6 */
		geth_phy_write(dev, phy_addr, 0x18, 0x00bc);	/* PHYAFE TRX optimization */

		/* Disable Auto Power Saving mode */
		geth_phy_write(dev, phy_addr, 0x1f, 0x0100);	/* Switch to Page 1 */
		phy_val = geth_phy_read(dev, phy_addr, 0x17);
		phy_val &= (~(1 << 13));
		geth_phy_write(dev, phy_addr, 0x17, phy_val);
		geth_phy_write(dev, phy_addr, 0x1f, 0x0000);	/* Switch to Page 0 */
#endif
	}
#if CONFIG_RTL8363_NB
	smi_read(MII_BMCR, &phy_val);
	smi_write(MII_BMCR, phy_val | BMCR_RESET);
	while(1){
		smi_read(MII_BMCR, &phy_val);
		if (!(phy_val & BMCR_RESET))
			break;
	}
#else
	phy_val = geth_phy_read(dev, phy_addr, MII_BMCR);
	geth_phy_write(dev, phy_addr, MII_BMCR, phy_val | BMCR_RESET);
	while (geth_phy_read(dev, phy_addr, MII_BMCR) & BMCR_RESET)
		;
#endif
	/* Reset phy chip */
#if CONFIG_RTL8363_NB
	smi_read(MII_BMCR, &phy_val);
	smi_write(MII_BMCR, phy_val & (~BMCR_PDOWN));
	while(1){
		smi_read(MII_BMCR, &phy_val);
		if (!(phy_val & BMCR_PDOWN))
			break;
	}
#else
	phy_val = geth_phy_read(dev, phy_addr, MII_BMCR);
	geth_phy_write(dev, phy_addr, MII_BMCR, (phy_val & ~BMCR_PDOWN));
	while (geth_phy_read(dev, phy_addr, MII_BMCR) & BMCR_PDOWN)
		;
#endif
#ifdef PHY_RTL8211F
	/* @TODO:
	 * In the past, we just disabled auto-negotiation and
	 * forced the ethernet to work on 100Mbps mode as we
	 * didn't recommend to run 1Gbps mode in uboot when
	 * using a gigabit phy.
	 * Due to the above solution, we came across some
	 * link-down problems when connecting to routers
	 * nowadays.
	 *
	 * Here comes the new solution:
	 * 1.Do not advertise '1000FULL';
	 * 2.Do not disable auto-negotiation;
	 *
	 * So the link will automatically negotiate to 100Mbps mode.
	 */

	/*
	 * It doesn't really disable auto-negotiation any more,
	 * just for compatibility.
	 */
#ifdef DISABLE_AUTONEG
	phy_val = geth_phy_read(dev, phy_addr, MII_CTRL1000);
	phy_val &= ~ADVERTISE_1000FULL;
	geth_phy_write(dev, phy_addr, MII_CTRL1000, phy_val);

	phy_val = geth_phy_read(dev, phy_addr, MII_BMCR);
	phy_val &= ~BMCR_SPEED1000;
	phy_val |= BMCR_SPEED100;
	phy_val |= BMCR_FULLDPLX;
	geth_phy_write(dev, phy_addr, MII_BMCR, phy_val);
#endif

	/* Wait BMSR_ANEGCOMPLETE be set */
	while (!(geth_phy_read(dev, phy_addr, MII_BMSR) & BMSR_ANEGCOMPLETE)) {
		if (i > 4) {
			printf("Warning: Auto negotiation timeout!\n");
		}
		udelay(500*1000);
		i++;
	}
#else /* PHY_RTL8211F */
#ifdef DISABLE_AUTONEG
	phy_val = geth_phy_read(dev, phy_addr, MII_BMCR);
	phy_val &= ~BMCR_ANENABLE;
	phy_val &= ~BMCR_SPEED1000;
	phy_val |= BMCR_SPEED100;
	phy_val |= BMCR_FULLDPLX;
	geth_phy_write(dev, phy_addr, MII_BMCR, phy_val);
#endif
#endif /* PHY_RTL8211F */
	phy_val = geth_phy_read(dev, phy_addr, MII_BMCR);
	reg_val = readl((void *)(unsigned long)(dev->iobase + GETH_BASIC_CTL0));
	if (phy_val & BMCR_FULLDPLX)
		reg_val |= 0x01;
	else
		reg_val &= ~0x01;

	/* Default is 1000Mbps */
	reg_val &= ~0xc;
	if (phy_val & BMCR_SPEED100)
		reg_val |= 0x0c;
	else if (!(phy_val & BMCR_SPEED1000))
		reg_val |= 0x08;
	writel(reg_val, (void *)(unsigned long)(dev->iobase + GETH_BASIC_CTL0));

#if 0
	/* just for debug loopback */
	phy_val = geth_phy_read(dev, phy_addr, MII_BMCR);
	phy_val |= BMCR_LOOPBACK;
	geth_phy_write(dev, phy_addr, MII_BMCR, phy_val);
	phy_val = geth_phy_read(dev, phy_addr, MII_BMCR);
	printf("REG0: %08x\n", phy_val);

	/* MAC layer Loopback */
	reg_val = readl((dev->iobase + GETH_BASIC_CTL0));
	reg_val |= 0x02;
	writel(reg_val, (dev->iobase + GETH_BASIC_CTL0));
#endif

#if defined(CONFIG_SUN50IW6P1)
	phy_val = geth_phy_read(dev, phy_addr, 0x13);
	phy_val |= 1 << 12;    /* XMII RXCLK Inversed */
	geth_phy_write(dev, phy_addr, 0x13, phy_val);
#endif

	phy_val = geth_phy_read(dev, phy_addr, MII_BMCR);
	printf("%s   Speed: %dMbps, Mode: %s, Loopback: %s\n",
				dev->name, ((phy_val & BMCR_SPEED1000) ? 1000 :
				((phy_val & BMCR_SPEED100) ? 100 : 10)),
				(phy_val & BMCR_FULLDPLX) ? "Full duplex" : "Half duplex",
				(phy_val & BMCR_LOOPBACK) ? "YES" : "NO");

	return 0;
}

static int geth_init(struct eth_device *dev, bd_t *bis)
{
	u32 reg_val;

	/* Reset all components */

	reg_val = readl((void *)(unsigned long)(dev->iobase + GETH_BASIC_CTL1));
	reg_val |= 0x01;
	writel(reg_val, (void *)(unsigned long)(dev->iobase + GETH_BASIC_CTL1));
	while (readl((void *)(unsigned long)(dev->iobase + GETH_BASIC_CTL1)) & (0x01))
		;

	/* init phy */
	mii_phy_init(dev);

	/* Initialize core */
	reg_val = readl((void *)(unsigned long)(dev->iobase + GETH_TX_CTL0));
	reg_val |= (3 << 30);   /* Enable transmit component &  Jabber Disable */
	writel(reg_val, (void *)(unsigned long)(dev->iobase + GETH_TX_CTL0));

	reg_val = readl((void *)(unsigned long)(dev->iobase + GETH_TX_CTL1));
	/* Transmit COE type 2 cannot be done in cut-through mode. */
	reg_val |= 0x02;
	writel(reg_val, (void *)(unsigned long)(dev->iobase + GETH_TX_CTL1));

	reg_val = readl((void *)(unsigned long)(dev->iobase + GETH_RX_CTL0));
#ifdef CONFIG_HARD_CHECKSUM
	reg_val |= (1 << 27);                           /* Enable CRC & IPv4 Header Checksum */
#endif
	reg_val |= (0x0F << 28);                        /* Automatic Pad/CRC Stripping */
	writel(reg_val, (void *)(unsigned long)(dev->iobase + GETH_RX_CTL0));

	reg_val = readl((void *)(unsigned long)(dev->iobase + GETH_RX_CTL1));
	reg_val |= 0x02;
	writel(reg_val, (void *)(unsigned long)(dev->iobase + GETH_RX_CTL1));

	/* GMAC frame filter */
	reg_val = readl((void *)(unsigned long)(dev->iobase + GETH_RX_FRM_FLT));
	reg_val |= 0x00000001;
	writel(reg_val, (void *)(unsigned long)(dev->iobase + GETH_RX_FRM_FLT));

	/* Burst should be 8 */
	reg_val = readl((void *)(unsigned long)(dev->iobase + GETH_BASIC_CTL1));
	reg_val |= (8 << 24);
	writel(reg_val, (void *)(unsigned long)(dev->iobase + GETH_BASIC_CTL1));

	/* Seth hardware address */
	if (dev->write_hwaddr)
		dev->write_hwaddr(dev);

	/* Disable all interrupt of dma */
	writel(0x00UL, (void *)(unsigned long)(dev->iobase + GETH_INT_EN));

	memset((void *)dma_desc_tx, 0, sizeof(dma_desc_t));
	memset((void *)dma_desc_rx, 0, sizeof(dma_desc_t));

	dma_desc_tx->desc3 = (void *)dma_desc_tx;  /* There is only one TX-DMA-Descriptor on the linked-list */
	dma_desc_rx->desc3 = (void *)dma_desc_rx;  /* There is only one RX-DMA-Descriptor on the linked-list */

#ifdef NOT_SUPPORT_NOCACHED_ALLOC
	flush_cache((unsigned long)dma_desc_tx, ALIGN(sizeof(*dma_desc_tx), CACHE_LINE_SIZE));
	flush_cache((unsigned long)dma_desc_rx, ALIGN(sizeof(*dma_desc_rx), CACHE_LINE_SIZE));
#endif

	writel((ulong)dma_desc_tx, (void *)(unsigned long)(dev->iobase + GETH_TX_DESC_LIST));
	writel((ulong)dma_desc_rx, (void *)(unsigned long)(dev->iobase + GETH_RX_DESC_LIST));

	return 0;
}

static void geth_halt(struct eth_device *dev)
{
	int reg_val;

	/* Disable transmit component */
	reg_val = readl((void *)(unsigned long)(dev->iobase + GETH_TX_CTL0));
	reg_val &= ~0x80000000;
	writel(reg_val, (void *)(unsigned long)(dev->iobase + GETH_TX_CTL0));

	/* Disable received component */
	reg_val = readl((void *)(unsigned long)(dev->iobase + GETH_RX_CTL0));
	reg_val &= ~0x80000000;
	writel(reg_val, (void *)(unsigned long)(dev->iobase + GETH_RX_CTL0));
}

int geth_write_hwaddr(struct eth_device *dev)
{
	u32 mac;
	unsigned char *addr;

	addr = &(dev->enetaddr[0]);
	mac  = (addr[5] << 8) | addr[4];
	writel(mac, (void *)(unsigned long)(dev->iobase + GETH_ADDR_HI(0)));

	mac  = (addr[3] << 24) | (addr[2] << 16) | (addr[1] << 8) | addr[0];
	writel(mac, (void *)(unsigned long)(dev->iobase + GETH_ADDR_LO(0)));

	return 0;
}

#ifdef CONFIG_MCAST_TFTP  /*  This driver already accepts all b/mcast */
static int geth_bcast_addr (struct eth_device *dev, u32 bcast_mac, u8 set)
{
	return 0;
}
#endif

#ifdef NOT_SUPPORT_NOCACHED_ALLOC
/*
 * Some platform does not have noncached_* API supported, so lets fake it here...
 * And we will use flush_cache() to sync the cache.
 */
static phys_addr_t noncached_alloc(size_t size, size_t align)
{
	phys_addr_t addr = (phys_addr_t)memalign(64, size);
	printf("%s(): addr = 0x%x\n", __func__, (unsigned int)addr);
	return addr;
}
#endif

int geth_initialize(bd_t *bis)
{
#if CONFIG_RTL8363_NB
       // use eth_device as global variable.
#else
       struct eth_device *dev;
#endif

	u32 buf_addr;

	dev = (struct eth_device *)malloc(sizeof *dev);
	if (!dev)
		return -ENOMEM;

	memset(dev, 0, (size_t)sizeof(*dev));
	strcpy(dev->name, "eth0");

	buf_addr = (u32)noncached_alloc(sizeof(struct dma_desc), 64);
	dma_desc_tx = (struct dma_desc *)(unsigned long)buf_addr;
	if (dma_desc_tx == NULL)
		goto err;

	buf_addr = (u32)noncached_alloc(sizeof(struct dma_desc), 64);
	dma_desc_rx = (struct dma_desc *)(unsigned long)buf_addr;
	if (dma_desc_rx == NULL)
		goto err;

	buf_addr = (u32)noncached_alloc(2048, 64);
	rx_packet = (char *)(unsigned long)buf_addr;
	if (rx_packet == NULL)
		goto err;

	buf_addr = (u32)noncached_alloc(2048, 64);
	rx_handle_buf = (char *)(unsigned long)buf_addr;
	if (rx_handle_buf == NULL)
		goto err;
#if 0
	geth_phy_write(dev, 0, 31, 0x000E);
	geth_phy_write(dev, 0, 23, 0x1b00);
	geth_phy_write(dev, 0, 21, 0x1);
	geth_phy_read(dev, 0, 0x25);
        printf("%s->%d mdio_read: %x ===\n", __func__, __LINE__, geth_phy_read(dev, 0, 25));
#endif
	/* set random hwaddr for mac */
	random_ether_addr(dev->enetaddr);

	dev->iobase = IOBASE;
	dev->init = geth_init;
	dev->halt = geth_halt;
	dev->send = geth_xmit;
	dev->recv = geth_recv;
	dev->write_hwaddr = geth_write_hwaddr;
#ifdef CONFIG_MCAST_TFTP
	dev->mcast = geth_bcast_addr;
#endif

	if (geth_sys_init()) {
		printf("geth_sys_init fail!\n");
		return -ENOMEM;
	}

	eth_register(dev);
	miiphy_register(dev->name, geth_miiphy_read, geth_miiphy_write);

	return 0;

err:
	free(rx_packet);
	free(dma_desc_rx);
	free(dma_desc_tx);
	free(dev);

	return -ENOMEM;
}
