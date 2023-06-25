/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2015 Roy Spliet <rspliet@ultimaker.com>
 */

#ifndef _SUNXI_DMA_H
#define _SUNXI_DMA_H

#define SUNXI_DMA_CHANNAL_BASE (SUNXI_DMA_BASE + 0x100)
#define DMA_AUTO_GATE_REG (SUNXI_DMA_BASE + 0x28)

#define SUNXI_DMA_CHANANL_SIZE (0x40)
#define SUNXI_DMA_LINK_NULL (0xfffff800)

#define DMAC_DMATYPE_NORMAL 0
#define DMAC_CFG_TYPE_DRAM (1)
#define DMAC_CFG_TYPE_SRAM (0)

#if defined(CONFIG_MACH_SUN8IW18) ||\
    defined(CONFIG_MACH_SUN50IW9) ||\
    defined(CONFIG_MACH_SUN50IW10) ||\
    defined(CONFIG_MACH_SUN8IW19) ||\
	defined(CONFIG_MACH_SUN20IW1) ||\
	defined(CONFIG_MACH_SUN8IW20) ||\
	defined(CONFIG_MACH_SUN8IW21)
#define DMAC_CFG_TYPE_SPI0 (22)
#define DMAC_CFG_SRC_TYPE_NAND (5)
#elif defined(CONFIG_MACH_SUN8IW15)
#define DMAC_CFG_TYPE_SPI0 (23)
#define DMAC_CFG_SRC_TYPE_NAND (5)
#elif defined(CONFIG_MACH_SUN8IW7) ||\
      defined(CONFIG_MACH_SUN8IW11)
#define DMAC_CFG_TYPE_SPI0 (24)
#define DMAC_CFG_SRC_TYPE_NAND (5)
#elif defined(CONFIG_MACH_SUN50IW11) ||\
	defined(CONFIG_MACH_SUN50IW12) ||\
       	defined(CONFIG_MACH_SUN55IW3)

#define DMAC_CFG_TYPE_SPI0 (22)
#define DMAC_CFG_SRC_TYPE_NAND (10)

#endif

/* DMA base config  */
#define DMAC_CFG_CONTINUOUS_ENABLE (0x01)
#define DMAC_CFG_CONTINUOUS_DISABLE (0x00)

/* ----------DMA dest config-------------------- */
/* DMA dest width config */
#define DMAC_CFG_DEST_DATA_WIDTH_8BIT (0x00)
#define DMAC_CFG_DEST_DATA_WIDTH_16BIT (0x01)
#define DMAC_CFG_DEST_DATA_WIDTH_32BIT (0x02)

/* DMA dest bust config */
#define DMAC_CFG_DEST_1_BURST (0x00)
#define DMAC_CFG_DEST_4_BURST (0x01)
#define DMAC_CFG_DEST_8_BURST (0x02)

#define DMAC_CFG_DEST_ADDR_TYPE_LINEAR_MODE (0x00)
#define DMAC_CFG_DEST_ADDR_TYPE_IO_MODE (0x01)

/* ----------DMA src config -------------------*/
#define DMAC_CFG_SRC_DATA_WIDTH_8BIT (0x00)
#define DMAC_CFG_SRC_DATA_WIDTH_16BIT (0x01)
#define DMAC_CFG_SRC_DATA_WIDTH_32BIT (0x02)

#define DMAC_CFG_SRC_1_BURST (0x00)
#define DMAC_CFG_SRC_4_BURST (0x01)
#define DMAC_CFG_SRC_8_BURST (0x02)

#define DMAC_CFG_SRC_ADDR_TYPE_LINEAR_MODE (0x00)
#define DMAC_CFG_SRC_ADDR_TYPE_IO_MODE (0x01)

/*dma int config*/
#define DMA_PKG_HALF_INT (1 << 0)
#define DMA_PKG_END_INT (1 << 1)
#define DMA_QUEUE_END_INT (1 << 2)

typedef struct {
	unsigned int config;
	unsigned int source_addr;
	unsigned int dest_addr;
	unsigned int byte_count;
	unsigned int commit_para;
	unsigned int link;
	unsigned int reserved[2];
} sunxi_dma_desc;

#if defined(CONFIG_MACH_SUN50IW3) || defined(CONFIG_MACH_SUN8IW18) ||          \
	defined(CONFIG_MACH_SUN50IW9) || defined(CONFIG_MACH_SUN8IW16) ||      \
	defined(CONFIG_MACH_SUN8IW19) || defined(CONFIG_MACH_SUN50IW10) ||     \
	defined(CONFIG_MACH_SUN8IW15) || defined(CONFIG_MACH_SUN8IW7) ||	\
	defined(CONFIG_MACH_SUN50IW11) || defined(CONFIG_MACH_SUN50IW12) ||	\
	defined(CONFIG_MACH_SUN20IW1) || defined(CONFIG_MACH_SUN8IW20) ||	\
	defined(CONFIG_MACH_SUN8IW21) || defined(CONFIG_MACH_SUN50IW5) ||  \
        defined(CONFIG_MACH_SUN55IW3)
typedef struct {
	unsigned int src_drq_type : 6;
	unsigned int src_burst_length : 2;
	unsigned int src_addr_mode : 1;
	unsigned int src_data_width : 2;
	unsigned int reserved0 : 5;
	unsigned int dst_drq_type : 6;
	unsigned int dst_burst_length : 2;
	unsigned int dst_addr_mode : 1;
	unsigned int dst_data_width : 2;
	unsigned int reserved1 : 5;
} sunxi_dma_channal_config;

#elif defined(CONFIG_MACH_SUN8IW11)
typedef struct {
	unsigned int src_drq_type : 5;
	unsigned int src_addr_mode : 2;
	unsigned int src_burst_length : 2;
	unsigned int src_data_width : 2;
	unsigned int reserved0 : 5;
	unsigned int dst_drq_type : 5;
	unsigned int dst_addr_mode : 2;
	unsigned int dst_burst_length : 2;
	unsigned int dst_data_width : 2;
	unsigned int reserved1 : 5;
} sunxi_dma_channal_config;

#else
#error "DMA definition not available for this architecture"
#endif

typedef struct {
	sunxi_dma_channal_config channal_cfg;
	unsigned int loop_mode;
	unsigned int data_block_size;
	unsigned int wait_cyc;
} sunxi_dma_set;

struct dma_irq_handler {
	void *m_data;
	void (*m_func)(void *data);
};

typedef struct {
	unsigned int enable;
	unsigned int pause;
	unsigned int desc_addr;
	unsigned int config;
	unsigned int cur_src_addr;
	unsigned int cur_dst_addr;
	unsigned int left_bytes;
	unsigned int parameters;
	unsigned int mode;
	unsigned int fdesc_addr;
	unsigned int pkg_num;
	unsigned int res[5];
} sunxi_dma_channal_reg;

typedef struct {
	unsigned int irq_en0; /* 0x0 dma irq enable register 0 */
	unsigned int irq_en1; /* 0x4 dma irq enable register 1 */
	unsigned int reserved0[2];
	unsigned int irq_pending0; /* 0x10 dma irq pending register 0 */
	unsigned int irq_pending1; /* 0x14 dma irq pending register 1 */
	unsigned int reserved1[2];
	unsigned int security; /* 0x20 dma security register */
	unsigned int reserved3[1];
	unsigned int auto_gate; /* 0x28 dma auto gating register */
	unsigned int reserved4[1];
	unsigned int status; /* 0x30 dma status register */
	unsigned int reserved5[3];
	unsigned int version; /* 0x40 dma Version register */
	unsigned int reserved6[47];
	sunxi_dma_channal_reg channal[16]; /* 0x100 dma channal register */
} sunxi_dma_reg;

typedef struct sunxi_dma_source_t {
	unsigned int used;
	unsigned int channal_count;
	sunxi_dma_channal_reg *channal;
	unsigned int reserved;
	sunxi_dma_desc *desc;
	struct dma_irq_handler dma_func;
} sunxi_dma_source;

#define DMA_RST_OFS 16
#define DMA_GATING_OFS 0

void sunxi_dma_init(void);
void sunxi_dma_exit(void);

ulong sunxi_dma_request(unsigned int dmatype);
ulong sunxi_dma_request_from_last(unsigned int dmatype);
int sunxi_dma_release(unsigned long hdma);
int sunxi_dma_setting(unsigned long hdma, sunxi_dma_set *cfg);
int sunxi_dma_start(unsigned long hdma, unsigned int saddr, unsigned int daddr,
		    unsigned int bytes);
int sunxi_dma_stop(unsigned long hdma);
int sunxi_dma_querystatus(unsigned long hdma);

int sunxi_dma_install_int(ulong hdma, interrupt_handler_t dma_int_func,
			  void *p);
int sunxi_dma_disable_int(ulong hdma);

int sunxi_dma_enable_int(ulong hdma);
int sunxi_dma_free_int(ulong hdma);

#endif /* _SUNXI_DMA_H */
