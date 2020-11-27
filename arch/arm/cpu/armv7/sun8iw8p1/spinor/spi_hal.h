/**
 * spi_hal.h
 * date:    2012/2/12 22:34:41
 * author:  Aaron<leafy.myeh@allwinnertech.com>
 * history: V0.1
 */

#ifndef __SPI_HAL_H
#define __SPI_HAL_H


#define RET_OK      (0)
#define RET_FAIL    (-1)

#define RET_TRUE    (0)
#define RET_FALSE   (-1)

#define DMA_TYPE_SPI0               (23) /* port 23 */
#define DMA_TYPE_SPI1               (24) /* port 24 */
#define DMA_CFG_DST_DRQ_SPI0        (DMA_TYPE_SPI0 << 16)
#define DMA_CFG_DST_DRQ_SPI1        (DMA_TYPE_SPI1 << 16)
#define DMA_CFG_SRC_DRQ_SPI0        (DMA_TYPE_SPI0 << 0)
#define DMA_CFG_SRC_DRQ_SPI1        (DMA_TYPE_SPI1 << 0)
#define SPI_TRANS_MODE              0
#define SPI_CLK_SRC                 (1) //0-24M, 1-PLL6
#define SPI_MCLK                    (40000000)
#define MAX_SPI_NUM                 2

#define set_wbit(addr, v)   (*((volatile unsigned long  *)(addr)) |=  (unsigned long)(v))
#define clr_wbit(addr, v)   (*((volatile unsigned long  *)(addr)) &= ~(unsigned long)(v))
//#define writeb(value,addr)  (*((volatile __u8 *)(addr))  = (value))   /* byte output */


#define AXI_BUS                     (0)
#define AHB1_BUS0                   (1)
#define AHB1_BUS1                   (2)
#define AHB1_LVDS                   (3)
#define APB1_BUS0                   (4)
#define APB2_BUS0                   (5)
#define CCM_BASE                    (0x01c20000)
#define SPI0_CKID                   ((AHB1_BUS0 << 8) | 20)
#define SPI1_CKID                   ((AHB1_BUS0 << 8) | 21)
#define CCM_AHB1_RST_REG0           (CCM_BASE+0x02C0)
#define CCM_AHB1_RST_REG1           (CCM_BASE+0x02C4)
#define CCM_AHB1_RST_REG2           (CCM_BASE+0x02C8)
#define CCM_AXI_GATE_CTRL           (CCM_BASE+0x05c)
#define CCM_AHB1_GATE0_CTRL         (CCM_BASE+0x060)
#define CCM_PLL6_MOD_CTRL           (CCM_BASE+0x028)
#define CCM_SPI0_SCLK_CTRL          (CCM_BASE+0x0a0)



#define DMA_CFG_SRC_BST1_WIDTH8     ((DMA_CFG_BST1 << 7) | (DMA_CFG_WID8 << 9))
#define DMA_CFG_DST_BST1_WIDTH8     ((DMA_CFG_BST1 << 23) | (DMA_CFG_WID8 << 25))
#define DRAM_MEM_BASE               (0x40000000)
#define DMA_IRQ_PEND0_REG           DMA_IRQ_PEND_REG



/* run time control */
#define TEST_SPI_NO     (0)
#define SPI_DEFAULT_CLK (40000000)
#define SPI_TX_WL       (0x20)
#define SPI_RX_WL       (0x20)
#define SPI_FIFO_SIZE   (64)
#define SPI_CLK_SRC     (1) //0-24M, 1-PLL6
#define SPI_MCLK        (40000000)

#define SPIC_BASE_OS    (0x1000)
#define SPI_BASE        (0x01c68000)


//#define SPI_BASE      (SPI0_BASE + SPIC_BASE_OS * (spi_no))
//#define SPI_IRQNO(_n) (GIC_SRC_SPI0 + (_n))

#define SPI_VAR     (SPI_BASE + 0x00)
#define SPI_GCR     (SPI_BASE + 0x04)
#define SPI_TCR     (SPI_BASE + 0x08)
#define SPI_IER     (SPI_BASE + 0x10)
#define SPI_ISR     (SPI_BASE + 0x14)
#define SPI_FCR     (SPI_BASE + 0x18)
#define SPI_FSR     (SPI_BASE + 0x1c)
#define SPI_WCR     (SPI_BASE + 0x20)
#define SPI_CCR     (SPI_BASE + 0x24)
#define SPI_MBC     (SPI_BASE + 0x30)
#define SPI_MTC     (SPI_BASE + 0x34)
#define SPI_BCC     (SPI_BASE + 0x38)
#define SPI_TXD     (SPI_BASE + 0x200)
#define SPI_RXD     (SPI_BASE + 0x300)

/* bit field of registers */
#define SPI_SOFT_RST    (1U << 31)
#define SPI_TXPAUSE_EN  (1U << 7)
#define SPI_MASTER      (1U << 1)
#define SPI_ENABLE      (1U << 0)

#define SPI_EXCHANGE    (1U << 31)
#define SPI_SAMPLE_MODE (1U << 13)
#define SPI_LSB_MODE    (1U << 12)
#define SPI_SAMPLE_CTRL (1U << 11)
#define SPI_RAPIDS_MODE (1U << 10)
#define SPI_DUMMY_1     (1U << 9)
#define SPI_DHB         (1U << 8)
#define SPI_SET_SS_1    (1U << 7)
#define SPI_SS_MANUAL   (1U << 6)
#define SPI_SEL_SS0     (0U << 4)
#define SPI_SEL_SS1     (1U << 4)
#define SPI_SEL_SS2     (2U << 4)
#define SPI_SEL_SS3     (3U << 4)
#define SPI_SS_N_INBST  (1U << 3)
#define SPI_SS_ACTIVE0  (1U << 2)
#define SPI_MODE0       (0U << 0)
#define SPI_MODE1       (1U << 0)
#define SPI_MODE2       (2U << 0)
#define SPI_MODE3       (3U << 0)

#define SPI_SS_INT      (1U << 13)
#define SPI_TC_INT      (1U << 12)
#define SPI_TXUR_INT    (1U << 11)
#define SPI_TXOF_INT    (1U << 10)
#define SPI_RXUR_INT    (1U << 9)
#define SPI_RXOF_INT    (1U << 8)
#define SPI_TXFULL_INT  (1U << 6)
#define SPI_TXEMPT_INT  (1U << 5)
#define SPI_TXREQ_INT   (1U << 4)
#define SPI_RXFULL_INT  (1U << 2)
#define SPI_RXEMPT_INT  (1U << 1)
#define SPI_RXREQ_INT   (1U << 0)
#define SPI_ERROR_INT   (SPI_TXUR_INT|SPI_TXOF_INT|SPI_RXUR_INT|SPI_RXOF_INT)

#define SPI_TXFIFO_RST  (1U << 31)
#define SPI_TXFIFO_TST  (1U << 30)
#define SPI_TXDMAREQ_EN (1U << 24)
#define SPI_RXFIFO_RST  (1U << 15)
#define SPI_RXFIFO_TST  (1U << 14)
#define SPI_RXDMAREQ_EN (1U << 8)
#define SPI_MASTER_DUAL (1U << 28)


extern int   spic_init(unsigned int spi_no);
extern int   spic_exit(unsigned int spi_no);
extern int spic_rw( u32 txlen, void* txbuff, u32 rxlen, void* rxbuff);

#endif
