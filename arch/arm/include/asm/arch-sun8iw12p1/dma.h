/*
 * dma.h
 *
 * Copyright (C) 2017-2020 Allwinnertech Co., Ltd
 *
 * Author        : zhouhuacai
 *
 * Description   : sunxi dma driver header
 *
 * SPDX-License-Identifier:	GPL-2.0+
 *
 *
 * History       :
 *  1.Date        : 2017/5/10
 *    Author      : zhouhuacai
 *    Modification: Created file
 */

#ifndef	_DMA_H_
#define	_DMA_H_

#include <asm/arch/intc.h>

//#define SUNXI_DMA_MAX     8
#define SYS_ENABLE_DCACHE 1
#define SUNXI_DMA_CHANNAL_BASE    (SUNXI_DMA_BASE + 0x100)
#define SUNXI_DMA_CHANANL_SIZE    (0x40)
#define SUNXI_DMA_LINK_NULL       (0xfffff800)

#define  DMAC_DMATYPE_NORMAL      0
#define DMAC_CFG_TYPE_SPI0                      (22)
#define	DMAC_CFG_TYPE_DRAM                      (1)
#define DMAC_CFG_TYPE_SRAM                      (0)

#define DMAC_CFG_CONTINUOUS_ENABLE              (0x01)
#define DMAC_CFG_CONTINUOUS_DISABLE             (0x00)

#define	DMAC_CFG_DEST_DATA_WIDTH_8BIT                   (0x00)
#define	DMAC_CFG_DEST_DATA_WIDTH_16BIT                  (0x01)
#define	DMAC_CFG_DEST_DATA_WIDTH_32BIT                  (0x02)

#define	DMAC_CFG_DEST_1_BURST                           (0x00)
#define	DMAC_CFG_DEST_4_BURST                           (0x01)
#define	DMAC_CFG_DEST_8_BURST                           (0x02)

#define	DMAC_CFG_DEST_ADDR_TYPE_LINEAR_MODE		(0x00)
#define	DMAC_CFG_DEST_ADDR_TYPE_IO_MODE 		(0x01)

#define	DMAC_CFG_SRC_DATA_WIDTH_8BIT			(0x00)
#define	DMAC_CFG_SRC_DATA_WIDTH_16BIT			(0x01)
#define	DMAC_CFG_SRC_DATA_WIDTH_32BIT			(0x02)

#define	DMAC_CFG_SRC_1_BURST 				(0x00)
#define	DMAC_CFG_SRC_4_BURST		    		(0x01)
#define	DMAC_CFG_SRC_8_BURST		    		(0x02)

#define	DMAC_CFG_SRC_ADDR_TYPE_LINEAR_MODE		(0x00)
#define	DMAC_CFG_SRC_ADDR_TYPE_IO_MODE 			(0x01)

#define	DMAC_CFG_DEST_TYPE_SRAM				(0x00)
#define	DMAC_CFG_DEST_TYPE_DRAM		    		(0x01)

#define DMAC_CFG_DEST_TYPE_NAND				(5)

#define	DMAC_CFG_DEST_TYPE_CODEC	    		(15)

#define	DMAC_CFG_DEST_TYPE_OTG_EP1	    		(17)
#define	DMAC_CFG_DEST_TYPE_OTG_EP2	    		(18)
#define	DMAC_CFG_DEST_TYPE_OTG_EP3	    		(19)
#define	DMAC_CFG_DEST_TYPE_OTG_EP4	    		(20)
#define	DMAC_CFG_DEST_TYPE_OTG_EP5	    		(21)

#define	DMAC_CFG_SRC_TYPE_SRAM				(0x00)
#define	DMAC_CFG_SRC_TYPE_DRAM		    	   	(0x01)

#define DMAC_CFG_SRC_TYPE_NAND				(5)

#define	DMAC_CFG_SRC_TYPE_CODEC	    			(15)

#define	DMAC_CFG_SRC_TYPE_OTG_EP1	    		(17)
#define	DMAC_CFG_SRC_TYPE_OTG_EP2	    		(18)
#define	DMAC_CFG_SRC_TYPE_OTG_EP3	    		(19)
#define	DMAC_CFG_SRC_TYPE_OTG_EP4	    		(20)
#define	DMAC_CFG_SRC_TYPE_OTG_EP5	    		(21)


typedef struct {
	unsigned int config;
	unsigned int source_addr;
	unsigned int dest_addr;
	unsigned int byte_count;
	unsigned int commit_para;
	unsigned int link;
	unsigned int reserved[2];
}
sunxi_dma_start_t;

typedef struct {
	unsigned int      src_drq_type     :6;
	unsigned int      src_burst_length :2;
	unsigned int      src_addr_mode    :1;
	unsigned int      src_data_width   :2;
	unsigned int      reserved0        :5;
	unsigned int      dst_drq_type     :6;
	unsigned int      dst_burst_length :2;
	unsigned int      dst_addr_mode    :1;
	unsigned int      dst_data_width   :2;
	unsigned int      reserved1        :5;
}
sunxi_dma_channal_config;
//for user request
typedef struct {
	sunxi_dma_channal_config  cfg;
	unsigned int	loop_mode;
	unsigned int	data_block_size;
	unsigned int	wait_cyc;
}
sunxi_dma_setting_t;

extern    void sunxi_dma_init(void);
extern    void sunxi_dma_exit(void);

extern    unsigned long sunxi_dma_request(unsigned int dmatype);
extern    int sunxi_dma_release(unsigned long hdma);
extern    int sunxi_dma_setting(unsigned long hdma, sunxi_dma_setting_t *cfg);
extern    int sunxi_dma_start(unsigned long hdma, unsigned int saddr, unsigned int daddr, unsigned int bytes);
extern    int sunxi_dma_stop(unsigned long hdma);
extern    int sunxi_dma_querystatus(unsigned long hdma);

extern    int sunxi_dma_install_int(ulong hdma, interrupt_handler_t dma_int_func, void *p);
extern    int sunxi_dma_disable_int(ulong hdma);

extern    int sunxi_dma_enable_int(ulong hdma);
extern    int sunxi_dma_free_int(ulong hdma);


/* register difine */
#define DMA_IRQ_EN_REG		(SUNXI_DMA_BASE + 0x00)

#define DMA_IRQ_PEND_REG	(SUNXI_DMA_BASE + 0x10)
#define DMA_AUTO_GATE_REG	(SUNXI_DMA_BASE + 0x28)
#define DMA_CHAN_STA_REG	(SUNXI_DMA_BASE + 0x30)

#define DMA_ENABLE_REG(ch)	(SUNXI_DMA_BASE + 0x0100 + ((ch) << 6) + 0x00)
#define DMA_PAUSE_REG(ch)	(SUNXI_DMA_BASE + 0x0100 + ((ch) << 6) + 0x04)
#define DMA_DESADDR_REG(ch)	(SUNXI_DMA_BASE + 0x0100 + ((ch) << 6) + 0x08)
#define DMA_CONFIGR_REG(ch)	(SUNXI_DMA_BASE + 0x0100 + ((ch) << 6) + 0x0C)
#define DMA_CUR_SADDR_REG(ch)	(SUNXI_DMA_BASE + 0x0100 + ((ch) << 6) + 0x10)
#define DMA_CUR_DADDR_REG(ch)	(SUNXI_DMA_BASE + 0x0100 + ((ch) << 6) + 0x14)
#define DMA_LEFTCNT_REG(ch)	(SUNXI_DMA_BASE + 0x0100 + ((ch) << 6) + 0x18)
#define DMA_PARAM_REG(ch)	(SUNXI_DMA_BASE + 0x0100 + ((ch) << 6) + 0x1C)

#define DMA_CHANN_NUM				(2)
/* dma descriptor */
#define STATIC_ALLOC_DMA_DES
struct dma_des {
	u32 config;
	u32 saddr;
	u32 daddr;
	u32 bcnt;
	u32 param;
	struct dma_des *next;
};

enum {DMA_CFG_BST1, DMA_CFG_BST4, DMA_CFG_BST8};
enum {DMA_CFG_WID8, DMA_CFG_WID16, DMA_CFG_WID32};
enum {DMA_LINEAR, DMA_IO};
enum {DMA_SECURITY, DMA_NONSECURITY};
#define DMA_DES_NULL			(0xfffff800)

/* DMA drq type */
#define DMA_TYPE_SRAM		(0 )      /* port 00 */
#define DMA_TYPE_SDRAM          (1 )      /* port 01 */
#define DMA_TYPE_SPDIF          (2 )      /* port 02 */
#define DMA_TYPE_I2S0        	(3 )      /* port 03 */
#define DMA_TYPE_I2S1        	(4 )      /* port 04 */
#define DMA_TYPE_I2S2         	(5 )      /* port 05 */
#define DMA_TYPE_I2S3    	(6 )      /* port 06 */
#define DMA_TYPE_DMIC         	(7 )      /* port 07 */
#define DMA_TYPE_DSD         	(8 )      /* port 08 */
#define DMA_TYPE_CE          	(9 )      /* port 09 */
#define DMA_TYPE_NAND0          (10)      /* port 10 */
#define DMA_TYPE_NAND1        	(11)      /* port 11 */
#define DMA_TYPE_GPADC          (12)      /* port 12 */
#define DMA_TYPE_IR         	(13)      /* port 13 */
#define DMA_TYPE_UART0        	(14)      /* port 14 */
#define DMA_TYPE_UART1        	(15)      /* port 15 */
#define DMA_TYPE_UART2        	(16)      /* port 16 */
#define DMA_TYPE_UART3        	(17)      /* port 17 */
#define DMA_TYPE_UART4          (18)      /* port 18 */
#define DMA_TYPE_UART5       	(19)      /* port 19 */
#define DMA_TYPE_UART6        	(20)      /* port 20 */
#define DMA_TYPE_UART7        	(21)      /* port 21 */
#define DMA_TYPE_SPI0         	(22)      /* port 22 */
#define DMA_TYPE_SPI1           (23)      /* port 23 */
#define DMA_TYPE_SPI2        	(24)      /* port 24 */
#define DMA_TYPE_SPI3        	(25)      /* port 25 */
#define DMA_TYPE_SPI4        	(26)      /* port 26 */
#define DMA_TYPE_SPI5        	(27)      /* port 27 */
#define DMA_TYPE_SPI6        	(28)      /* port 28 */
#define DMA_TYPE_SPI7        	(29)      /* port 29 */
#define DMA_TYPE_OTG_EP1        (30)      /* port 30 */
#define DMA_TYPE_OTG_EP2        (31)      /* port 31 */
#define DMA_TYPE_OTG_EP3        (32)      /* port 32 */
#define DMA_TYPE_OTG_EP4        (33)      /* port 33 */
#define DMA_TYPE_OTG_EP5        (34)      /* port 34 */


#define DMA_TYPE_AUDIO_HUB0		(43)	  /* port 43 */
#define DMA_TYPE_AUDIO_HUB1		(44)	  /* port 44 */
#define DMA_TYPE_AUDIO_HUB3		(45)	  /* port 45 */

/* DMA source drq config */
#define DMA_CFG_SRC_DRQ_SRAM		(DMA_TYPE_SRAM      << 0)
#define DMA_CFG_SRC_DRQ_SDRAM		(DMA_TYPE_SDRAM     << 0)
#define DMA_CFG_SRC_DRQ_SPDIF		(DMA_TYPE_SPDIF     << 0)
#define DMA_CFG_SRC_DRQ_IIS0		(DMA_TYPE_IIS0      << 0)
#define DMA_CFG_SRC_DRQ_IIS1		(DMA_TYPE_IIS1      << 0)
#define DMA_CFG_SRC_DRQ_NAND0		(DMA_TYPE_NAND0     << 0)
#define DMA_CFG_SRC_DRQ_UART0		(DMA_TYPE_UART0     << 0)
#define DMA_CFG_SRC_DRQ_UART1		(DMA_TYPE_UART1     << 0)
#define DMA_CFG_SRC_DRQ_UART2		(DMA_TYPE_UART2     << 0)
#define DMA_CFG_SRC_DRQ_UART3		(DMA_TYPE_UART3     << 0)
#define DMA_CFG_SRC_DRQ_UART4		(DMA_TYPE_UART4     << 0)
#define DMA_CFG_SRC_DRQ_TCON0		(DMA_TYPE_TCON0     << 0)
#define DMA_CFG_SRC_DRQ_TCON1		(DMA_TYPE_TCON1     << 0)
#define DMA_CFG_SRC_DRQ_HDMIDDC		(DMA_TYPE_HDMIDDC   << 0)
#define DMA_CFG_SRC_DRQ_HDMIAUDIO	(DMA_TYPE_HDMIAUDIO << 0)
#define DMA_CFG_SRC_DRQ_CODEC		(DMA_TYPE_CODEC     << 0)
#define DMA_CFG_SRC_DRQ_SS		(DMA_TYPE_SS        << 0)
#define DMA_CFG_SRC_DRQ_OTG_EP1		(DMA_TYPE_OTG_EP1   << 0)
#define DMA_CFG_SRC_DRQ_OTG_EP2		(DMA_TYPE_OTG_EP2   << 0)
#define DMA_CFG_SRC_DRQ_OTG_EP3		(DMA_TYPE_OTG_EP3   << 0)
#define DMA_CFG_SRC_DRQ_OTG_EP4		(DMA_TYPE_OTG_EP4   << 0)
#define DMA_CFG_SRC_DRQ_OTG_EP5		(DMA_TYPE_OTG_EP5   << 0)
#define DMA_CFG_SRC_DRQ_UART5		(DMA_TYPE_UART5     << 0)
#define DMA_CFG_SRC_DRQ_SPI0 		(DMA_TYPE_SPI0      << 0)
#define DMA_CFG_SRC_DRQ_SPI1 		(DMA_TYPE_SPI1      << 0)
#define DMA_CFG_SRC_DRQ_SPI2 		(DMA_TYPE_SPI2      << 0)
#define DMA_CFG_SRC_DRQ_SPI3 		(DMA_TYPE_SPI3      << 0)
#define DMA_CFG_SRC_DRQ_TP   		(DMA_TYPE_TP        << 0)
#define DMA_CFG_SRC_DRQ_NAND1		(DMA_TYPE_NAND1     << 0)
#define DMA_CFG_SRC_DRQ_MTCACC		(DMA_TYPE_MTCACC    << 0)
#define DMA_CFG_SRC_DRQ_DIGMIC		(DMA_TYPE_DIGMIC    << 0)
/* DMA destination drq config */
#define DMA_CFG_DST_DRQ_SRAM		(DMA_TYPE_SRAM      << 16)
#define DMA_CFG_DST_DRQ_SDRAM		(DMA_TYPE_SDRAM     << 16)
#define DMA_CFG_DST_DRQ_SPDIF		(DMA_TYPE_SPDIF     << 16)
#define DMA_CFG_DST_DRQ_IIS0		(DMA_TYPE_IIS0      << 16)
#define DMA_CFG_DST_DRQ_IIS1		(DMA_TYPE_IIS1      << 16)
#define DMA_CFG_DST_DRQ_NAND0		(DMA_TYPE_NAND0     << 16)
#define DMA_CFG_DST_DRQ_UART0		(DMA_TYPE_UART0     << 16)
#define DMA_CFG_DST_DRQ_UART1		(DMA_TYPE_UART1     << 16)
#define DMA_CFG_DST_DRQ_UART2		(DMA_TYPE_UART2     << 16)
#define DMA_CFG_DST_DRQ_UART3		(DMA_TYPE_UART3     << 16)
#define DMA_CFG_DST_DRQ_UART4		(DMA_TYPE_UART4     << 16)
#define DMA_CFG_DST_DRQ_TCON0		(DMA_TYPE_TCON0     << 16)
#define DMA_CFG_DST_DRQ_TCON1		(DMA_TYPE_TCON1     << 16)
#define DMA_CFG_DST_DRQ_HDMIDDC		(DMA_TYPE_HDMIDDC   << 16)
#define DMA_CFG_DST_DRQ_HDMIAUDIO	(DMA_TYPE_HDMIAUDIO << 16)
#define DMA_CFG_DST_DRQ_CODEC		(DMA_TYPE_CODEC     << 16)
#define DMA_CFG_DST_DRQ_SS		(DMA_TYPE_SS        << 16)
#define DMA_CFG_DST_DRQ_OTG_EP1		(DMA_TYPE_OTG_EP1   << 16)
#define DMA_CFG_DST_DRQ_OTG_EP2		(DMA_TYPE_OTG_EP2   << 16)
#define DMA_CFG_DST_DRQ_OTG_EP3		(DMA_TYPE_OTG_EP3   << 16)
#define DMA_CFG_DST_DRQ_OTG_EP4		(DMA_TYPE_OTG_EP4   << 16)
#define DMA_CFG_DST_DRQ_OTG_EP5		(DMA_TYPE_OTG_EP5   << 16)
#define DMA_CFG_DST_DRQ_UART5		(DMA_TYPE_UART5     << 16)
#define DMA_CFG_DST_DRQ_SPI0 		(DMA_TYPE_SPI0      << 16)
#define DMA_CFG_DST_DRQ_SPI1 		(DMA_TYPE_SPI1      << 16)
#define DMA_CFG_DST_DRQ_SPI2 		(DMA_TYPE_SPI2      << 16)
#define DMA_CFG_DST_DRQ_SPI3 		(DMA_TYPE_SPI3      << 16)
#define DMA_CFG_DST_DRQ_TP   		(DMA_TYPE_TP        << 16)
#define DMA_CFG_DST_DRQ_NAND1		(DMA_TYPE_NAND1     << 16)
#define DMA_CFG_DST_DRQ_MTCACC		(DMA_TYPE_MTCACC    << 16)
#define DMA_CFG_DST_DRQ_DIGMIC		(DMA_TYPE_DIGMIC    << 16)
/* DMA source burst length and width */
#define DMA_CFG_SRC_BST1_WIDTH8     ((DMA_CFG_BST1 << 7) | (DMA_CFG_WID8 << 9))
#define DMA_CFG_SRC_BST1_WIDTH16    ((DMA_CFG_BST1 << 7) | (DMA_CFG_WID16<< 9))
#define DMA_CFG_SRC_BST1_WIDTH32    ((DMA_CFG_BST1 << 7) | (DMA_CFG_WID32<< 9))
#define DMA_CFG_SRC_BST4_WIDTH8     ((DMA_CFG_BST4 << 7) | (DMA_CFG_WID8 << 9))
#define DMA_CFG_SRC_BST4_WIDTH16    ((DMA_CFG_BST4 << 7) | (DMA_CFG_WID16<< 9))
#define DMA_CFG_SRC_BST4_WIDTH32    ((DMA_CFG_BST4 << 7) | (DMA_CFG_WID32<< 9))
#define DMA_CFG_SRC_BST8_WIDTH8     ((DMA_CFG_BST8 << 7) | (DMA_CFG_WID8 << 9))
#define DMA_CFG_SRC_BST8_WIDTH16    ((DMA_CFG_BST8 << 7) | (DMA_CFG_WID16<< 9))
#define DMA_CFG_SRC_BST8_WIDTH32    ((DMA_CFG_BST8 << 7) | (DMA_CFG_WID32<< 9))
/* DMA destination burst length and width */
#define DMA_CFG_DST_BST1_WIDTH8     ((DMA_CFG_BST1 << 23) | (DMA_CFG_WID8 << 25))
#define DMA_CFG_DST_BST1_WIDTH16    ((DMA_CFG_BST1 << 23) | (DMA_CFG_WID16<< 25))
#define DMA_CFG_DST_BST1_WIDTH32    ((DMA_CFG_BST1 << 23) | (DMA_CFG_WID32<< 25))
#define DMA_CFG_DST_BST4_WIDTH8     ((DMA_CFG_BST4 << 23) | (DMA_CFG_WID8 << 25))
#define DMA_CFG_DST_BST4_WIDTH16    ((DMA_CFG_BST4 << 23) | (DMA_CFG_WID16<< 25))
#define DMA_CFG_DST_BST4_WIDTH32    ((DMA_CFG_BST4 << 23) | (DMA_CFG_WID32<< 25))
#define DMA_CFG_DST_BST8_WIDTH8     ((DMA_CFG_BST8 << 23) | (DMA_CFG_WID8 << 25))
#define DMA_CFG_DST_BST8_WIDTH16    ((DMA_CFG_BST8 << 23) | (DMA_CFG_WID16<< 25))
#define DMA_CFG_DST_BST8_WIDTH32    ((DMA_CFG_BST8 << 23) | (DMA_CFG_WID32<< 25))
/* IO mode */
#define DMA_CFG_SRC_LINEAR			(DMA_LINEAR << 8)
#define DMA_CFG_SRC_IO				(DMA_IO 	<< 8)
#define DMA_CFG_DST_LINEAR			(DMA_LINEAR << 24)
#define DMA_CFG_DST_IO				(DMA_IO 	<< 24)

/* DMA irq flags */
#define DMA_HALF_PKG_IRQFLAG		(1 << 0)
#define DMA_FULL_PKG_IRQFLAG		(1 << 1)
#define DMA_QUEUE_END_IRQFLAG		(1 << 2)
#define DMA_IRQ_FLAG_MASK	(DMA_HALF_PKG_IRQFLAG|DMA_FULL_PKG_IRQFLAG|DMA_QUEUE_END_IRQFLAG)
#define DMA_USE_CONTINUE_MODE		(1 << 4)

typedef void (*DMAHdle)(u32 data);

#endif	//_DMA_H_

/* end of _DMA_H_ */

