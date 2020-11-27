/**
 * spi_hal.h
 * date:    2012/2/12 22:34:41
 * author:  Aaron<leafy.myeh@allwinnertech.com>
 * history: V0.1
 */

#ifndef __SPI_HAL_H
#define __SPI_HAL_H



#define RET_OK                               (0)
#define RET_FAIL                             (-1)

#define RET_TRUE                             (0)
#define RET_FALSE                            (-1)

#define DMA_TYPE_SPI0                        (23)      /* port 23 */
#define DMA_TYPE_SPI1                        (24)      /* port 24 */
#define DMA_CFG_DST_DRQ_SPI0                 (DMA_TYPE_SPI0 << 16)
#define DMA_CFG_DST_DRQ_SPI1                 (DMA_TYPE_SPI1 << 16)
#define DMA_CFG_SRC_DRQ_SPI0                 (DMA_TYPE_SPI0 << 0)
#define DMA_CFG_SRC_DRQ_SPI1                 (DMA_TYPE_SPI1 << 0)
#define SPI_TRANS_MODE                       (0)
#define SPI_CLK_SRC                          (1)       //0-24M, 1-PLL6
#define SPI_MCLK                             (40000000)
#define MAX_SPI_NUM                          (2)

#define set_wbit(addr, v)                    (*((volatile unsigned long  *)(addr)) |=  (unsigned long)(v))
#define clr_wbit(addr, v)                    (*((volatile unsigned long  *)(addr)) &= ~(unsigned long)(v))


#define AXI_BUS                              (0)
#define AHB1_BUS0                            (1)
#define AHB1_BUS1                            (2)
#define AHB1_LVDS                            (3)
#define APB1_BUS0                            (4)
#define APB2_BUS0                            (5)
#define CCM_BASE                             (0x01c20000)
#define SPI0_CKID                            (20)


#define CCMU_PLL_CPUX_CTRL_REG               (CCM_BASE + 0x00)
#define CCMU_PLL_PERIPH_CTRL_REG             (CCM_BASE + 0x28)
#define CCMU_AHB1_APB_HCLKC_CFG_REG          (AHB1_BUS0 + 0x54)

#define CCMU_BUS_SOFT_RST_REG0               (CCM_BASE+0x02C0)
#define CCMU_BUS_SOFT_RST_REG1               (CCM_BASE+0x02C4)
#define CCMU_BUS_SOFT_RST_REG2               (CCM_BASE+0x02D0)
#define CCMU_BUS_CLK_GATING_REG0             (CCM_BASE+0x060)
#define CCM_PLL6_MOD_CTRL                    (CCM_BASE+0x028)


#define DMA_PTY_CFG_REG                      (SUNXI_DMA_BASE + 0x08)

#define DMA_CFG_SRC_BST1_WIDTH8              (( DMA_CFG_BST1 << 7)  | ( DMA_CFG_WID8 << 9 ))
#define DMA_CFG_DST_BST1_WIDTH8              (( DMA_CFG_BST1 << 23) | ( DMA_CFG_WID8 << 25))
#define DRAM_MEM_BASE                        (0x40000000)
#define DMA_IRQ_PEND0_REG                    DMA_IRQ_PEND_REG


#define DMA_OFFSET                  0x20
#define DMA_N_OFFSET                0x100
#define DMA_D_OFFSET                0x300

/* offset */
#define DMAC_REG_o_IRQ_EN           0x00
#define DMAC_REG_o_IRQ_PENDING      0x04
#define DMAC_REG_o_SYS_PRI          0x08
#define DMAC_REG_o_CFG              0x00
#define DMAC_REG_o_SRC_ADDR         0x04
#define DMAC_REG_o_DST_ADDR         0x08
#define DMAC_REG_o_BYTE_CNT         0x0C
#define DMAC_REG_o_PAGE_SIZE        0x10
#define DMAC_REG_o_PAGE_STEP        0x14
#define DMAC_REG_o_CMT_BLK          0x18

#define __REG(x)                    (*(volatile unsigned int   *)(x))
/* registers */
#define DMAC_REG_IRQ_EN             __REG( SUNXI_DMA_BASE + DMAC_REG_o_IRQ_EN                                    )
#define DMAC_REG_IRQ_PENDING        __REG( SUNXI_DMA_BASE + DMAC_REG_o_IRQ_PENDING                               )
#define DMAC_REG_SYS_PRI            __REG( SUNXI_DMA_BASE + DMAC_REG_o_SYS_PRI                                   )
#define DMAC_REG_N0_CFG             __REG( SUNXI_DMA_BASE + 0 * DMA_OFFSET + DMA_N_OFFSET + DMAC_REG_o_CFG       )
#define DMAC_REG_N0_SRC_ADDR        __REG( SUNXI_DMA_BASE + 0 * DMA_OFFSET + DMA_N_OFFSET + DMAC_REG_o_SRC_ADDR  )
#define DMAC_REG_N0_DST_ADDR        __REG( SUNXI_DMA_BASE + 0 * DMA_OFFSET + DMA_N_OFFSET + DMAC_REG_o_DST_ADDR  )
#define DMAC_REG_N0_BYTE_CNT        __REG( SUNXI_DMA_BASE + 0 * DMA_OFFSET + DMA_N_OFFSET + DMAC_REG_o_BYTE_CNT  )
#define DMAC_REG_N1_CFG             __REG( SUNXI_DMA_BASE + 1 * DMA_OFFSET + DMA_N_OFFSET + DMAC_REG_o_CFG       )
#define DMAC_REG_N1_SRC_ADDR        __REG( SUNXI_DMA_BASE + 1 * DMA_OFFSET + DMA_N_OFFSET + DMAC_REG_o_SRC_ADDR  )
#define DMAC_REG_N1_DST_ADDR        __REG( SUNXI_DMA_BASE + 1 * DMA_OFFSET + DMA_N_OFFSET + DMAC_REG_o_DST_ADDR  )
#define DMAC_REG_N1_BYTE_CNT        __REG( SUNXI_DMA_BASE + 1 * DMA_OFFSET + DMA_N_OFFSET + DMAC_REG_o_BYTE_CNT  )
#define DMAC_REG_N2_CFG             __REG( SUNXI_DMA_BASE + 2 * DMA_OFFSET + DMA_N_OFFSET + DMAC_REG_o_CFG       )
#define DMAC_REG_N2_SRC_ADDR        __REG( SUNXI_DMA_BASE + 2 * DMA_OFFSET + DMA_N_OFFSET + DMAC_REG_o_SRC_ADDR  )
#define DMAC_REG_N2_DST_ADDR        __REG( SUNXI_DMA_BASE + 2 * DMA_OFFSET + DMA_N_OFFSET + DMAC_REG_o_DST_ADDR  )
#define DMAC_REG_N2_BYTE_CNT        __REG( SUNXI_DMA_BASE + 2 * DMA_OFFSET + DMA_N_OFFSET + DMAC_REG_o_BYTE_CNT  )
#define DMAC_REG_N3_CFG             __REG( SUNXI_DMA_BASE + 3 * DMA_OFFSET + DMA_N_OFFSET + DMAC_REG_o_CFG       )
#define DMAC_REG_N3_SRC_ADDR        __REG( SUNXI_DMA_BASE + 3 * DMA_OFFSET + DMA_N_OFFSET + DMAC_REG_o_SRC_ADDR  )
#define DMAC_REG_N3_DST_ADDR        __REG( SUNXI_DMA_BASE + 3 * DMA_OFFSET + DMA_N_OFFSET + DMAC_REG_o_DST_ADDR  )
#define DMAC_REG_N3_BYTE_CNT        __REG( SUNXI_DMA_BASE + 3 * DMA_OFFSET + DMA_N_OFFSET + DMAC_REG_o_BYTE_CNT  )

#define DMAC_REG_N_CFG(port)        __REG( SUNXI_DMA_BASE + ( 1 + port ) * DMA_OFFSET + DMAC_REG_o_CFG           )
#define DMAC_REG_N_SRC_ADDR(port)   __REG( SUNXI_DMA_BASE + ( 1 + port ) * DMA_OFFSET + DMAC_REG_o_SRC_ADDR      )
#define DMAC_REG_N_DST_ADDR(port)   __REG( SUNXI_DMA_BASE + ( 1 + port ) * DMA_OFFSET + DMAC_REG_o_DST_ADDR      )
#define DMAC_REG_N_BYTE_CNT(port)   __REG( SUNXI_DMA_BASE + ( 1 + port ) * DMA_OFFSET + DMAC_REG_o_BYTE_CNT      )
#define DMAC_REG_D_CFG(port)        __REG( SUNXI_DMA_BASE + ( 5 + port ) * DMA_OFFSET + DMAC_REG_o_CFG           )
#define DMAC_REG_D_SRC_ADDR(port)   __REG( SUNXI_DMA_BASE + ( 5 + port ) * DMA_OFFSET + DMAC_REG_o_SRC_ADDR      )
#define DMAC_REG_D_DST_ADDR(port)   __REG( SUNXI_DMA_BASE + ( 5 + port ) * DMA_OFFSET + DMAC_REG_o_DST_ADDR      )
#define DMAC_REG_D_BYTE_CNT(port)   __REG( SUNXI_DMA_BASE + ( 5 + port ) * DMA_OFFSET + DMAC_REG_o_BYTE_CNT      )
#define DMAC_REG_D_PAGE_SIZE(port)  __REG( SUNXI_DMA_BASE + ( 5 + port ) * DMA_OFFSET + DMAC_REG_o_PAGE_SIZE     )
#define DMAC_REG_D_PAGE_STEP(port)  __REG( SUNXI_DMA_BASE + ( 5 + port ) * DMA_OFFSET + DMAC_REG_o_PAGE_STEP     )
#define DMAC_REG_D_CMT_BLK(port)    __REG( SUNXI_DMA_BASE + ( 5 + port ) * DMA_OFFSET + DMAC_REG_o_CMT_BLK       )







/* run time control */
#define TEST_SPI_NO     (0)
#define SPI_DEFAULT_CLK (40000000)
#define SPI_TX_WL       (0x20)
#define SPI_RX_WL       (0x20)
#define SPI_FIFO_SIZE   (64)
#define SPI_CLK_SRC     (1) //0-24M, 1-PLL6
#define SPI_MCLK        (40000000)

#define SPIC_BASE_OS    (0x1000)
#define SPI0_BASE       (0x01c05000)
#define SPI1_BASE       (0x01c06000)

#define SPI_VAR         (SPI0_BASE + 0x00)
#define SPI_GCR         (SPI0_BASE + 0x04)
#define SPI_TCR         (SPI0_BASE + 0x08)
#define SPI_IER         (SPI0_BASE + 0x10)
#define SPI_ISR         (SPI0_BASE + 0x14)
#define SPI_FCR         (SPI0_BASE + 0x18)
#define SPI_FSR         (SPI0_BASE + 0x1c)
#define SPI_WCR         (SPI0_BASE + 0x20)
#define SPI_CCR         (SPI0_BASE + 0x24)
#define SPI_MBC         (SPI0_BASE + 0x30)
#define SPI_MTC         (SPI0_BASE + 0x34)
#define SPI_BCC         (SPI0_BASE + 0x38)
#define SPI_TXD         (SPI0_BASE + 0x200)
#define SPI_RXD         (SPI0_BASE + 0x300)

#define SPI_TXD_OFFSET  (0X200)
#define SPI_RXD_OFFSET  (0X300)



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
#define SPI_RXDMA_DMODE (1U << 9)
#define SPI_MASTER_DUAL (1U << 28)



#define cache_line_size (32)

#define NDMA_SRAM       (16)
#define NDMA_SDRAM      (17)

#define NDMA_SPI0       (4)
#define NDMA_SPI1       (5)

enum DMADWidth
{
    WID8  = 0,
    WID16 = 1,
    WID32 = 2
};

enum NDMAAddrType
{
    INCR     = 0,
    NOCHANGE = 1
};

enum DDMAAddrType
{
    LINEAR = 0,
    IO,
    PAGE_HMOD,
    PAGE_VMOD
};

enum DMABurstLen
{
    SINGLE = 0,
    BLEN4  = 1
};



typedef struct __SPI_REGS
{
    __u32   ver_num;            //0x00
    __u32   global_control;     //0x04
    __u32   transfer_control;   //0x08
    __u32   resv0;              //0x0c
    __u32   interrupt_control;  //0x10
    __u32   interrupt_status;   //0x14
    __u32   fifo_control;       //0x18
    __u32   fifo_status;        //0x1c
    __u32   wait_clock_counter; //0x20
    __u32   clock_rate_control; //0x24
    __u32   resv1;              //0x28
    __u32   resv2;              //0x2c
    __u32   burst_counter;      //0x30
    __u32   transmit_counter;   //0x34
    __u32   burst_control_counter;  //0x38
} __spi_reg_t;



extern int   spic_init(unsigned int spi_no);
extern int   spic_exit(unsigned int spi_no);
extern int   spic_rw( u32 txlen, void* txbuff, u32 rxlen, void* rxbuff);

extern void  spic_config_dual_mode(u32 spi_no, u32 rxdual, u32 dbc, u32 stc);
#endif
