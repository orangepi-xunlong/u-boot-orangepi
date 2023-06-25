/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __SPI_HAL_H
#define __SPI_HAL_H

/* run time control */
#define TEST_SPI_NO		(0)
#define SPI_DEFAULT_CLK		(40000000)
#define SPI_TX_WL		(32)
#define SPI_RX_WL		(32)
#define SPI_FIFO_SIZE		(64)
#define SPI_CLK_SRC		(1)
#define SPI_MCLK		(40000000)

#define SPI_BASE                        SUNXI_SPI0_BASE
#define SPI_TX_IO_DATA                  (SPI_BASE + 0x200)
#define SPI_RX_IO_DATA                  (SPI_BASE + 0x300)

#define SPI_VAR		(SPI_BASE + 0x00)
#define SPI_GCR		(SPI_BASE + 0x04)
#define SPI_TCR		(SPI_BASE + 0x08)
#define SPI_IER		(SPI_BASE + 0x10)
#define SPI_ISR		(SPI_BASE + 0x14)
#define SPI_FCR		(SPI_BASE + 0x18)
#define SPI_FSR		(SPI_BASE + 0x1c)
#define SPI_WCR		(SPI_BASE + 0x20)
#define SPI_CCR		(SPI_BASE + 0x24)
#define SPI_SDC		(SPI_BASE + 0x28)
#define SPI_MBC		(SPI_BASE + 0x30)
#define SPI_MTC		(SPI_BASE + 0x34)
#define SPI_BCC		(SPI_BASE + 0x38)
#define SPI_DMA		(SPI_BASE + 0x88)
#define SPI_TXD		(SPI_BASE + 0x200)
#define SPI_RXD		(SPI_BASE + 0x300)

/* bit field of registers */
#define SPI_SOFT_RST	(1U << 31)
#define SPI_TXPAUSE_EN	(1U << 7)
#define SPI_MASTER	(1U << 1)
#define SPI_ENABLE	(1U << 0)

#define SPI_EXCHANGE	(1U << 31)
#define SPI_SAMPLE_MODE	(1U << 13)
#define SPI_LSB_MODE	(1U << 12)
#define SPI_SAMPLE_CTRL	(1U << 11)
#define SPI_RAPIDS_MODE	(1U << 10)
#define SPI_DUMMY_1	(1U << 9)
#define SPI_DHB		(1U << 8)
#define SPI_SET_SS_1	(1U << 7)
#define SPI_SS_MANUAL	(1U << 6)
#define SPI_SEL_SS0	(0U << 4)
#define SPI_SEL_SS1	(1U << 4)
#define SPI_SEL_SS2	(2U << 4)
#define SPI_SEL_SS3	(3U << 4)
#define SPI_SS_N_INBST	(1U << 3)
#define SPI_SS_ACTIVE0	(1U << 2)
#define SPI_MODE0	(0U << 0)
#define SPI_MODE1	(1U << 0)
#define SPI_MODE2	(2U << 0)
#define SPI_MODE3	(3U << 0)

#define SPI_SS_INT	(1U << 13)
#define SPI_TC_INT	(1U << 12)
#define SPI_TXUR_INT	(1U << 11)
#define SPI_TXOF_INT	(1U << 10)
#define SPI_RXUR_INT	(1U << 9)
#define SPI_RXOF_INT	(1U << 8)
#define SPI_TXFULL_INT	(1U << 6)
#define SPI_TXEMPT_INT	(1U << 5)
#define SPI_TXREQ_INT	(1U << 4)
#define SPI_RXFULL_INT	(1U << 2)
#define SPI_RXEMPT_INT	(1U << 1)
#define SPI_RXREQ_INT	(1U << 0)
#define SPI_ERROR_INT	(SPI_TXUR_INT|SPI_TXOF_INT|SPI_RXUR_INT|SPI_RXOF_INT)

#define SPI_TXFIFO_RST	(1U << 31)
#define SPI_TXFIFO_TST	(1U << 30)
#define SPI_TXDMAREQ_EN	(1U << 24)
#define SPI_RXFIFO_RST	(1U << 15)
#define SPI_RXFIFO_TST	(1U << 14)
#define SPI_RXDMAREQ_EN	(1U << 8)

#define SPI_MASTER_DUAL	(1U << 28)

/* sample delay mode */
#define SPI_SAMP_MODE_EN	(1U << 2)
#define SPI_SAMP_DL_SW_EN	(1U << 7)
#define DELAY_NORMAL_SAMPLE	(0x100)
#define DELAY_0_5_CYCLE_SAMPLE	(0x000)
#define DELAY_1_CYCLE_SAMPLE	(0x010)
#define DELAY_1_5_CYCLE_SAMPLE	(0x110)
#define DELAY_2_CYCLE_SAMPLE	(0x101)
#define DELAY_2_5_CYCLE_SAMPLE	(0x001)
#define DELAY_3_CYCLE_SAMPLE	(0x011)

#define SPI_NAND_READY		(1U << 0)
#define SPI_NAND_ERASE_FAIL	(1U << 2)
#define SPI_NAND_WRITE_FAIL	(1U << 3)
#define SPI_NAND_ECC_FIRST_BIT	(4)
#define SPI_NAND_ECC_BITMAP	(0x3)
#define SPI_NAND_QE		(1U << 0)
#define SPI_NAND_ECC_ENABLE	(1U << 4)
#define SPI_NAND_BUF_MODE	(1U << 3)


#define SPI_NAND_INT_ECCSR_BITMAP	(0xf)

#define SPI_NAND_WREN  		        0x06
#define SPI_NAND_WRDI  		        0x04
#define SPI_NAND_GETSR			0x0f
#define SPI_NAND_SETSR			0x1f
#define SPI_NAND_PAGE_READ		0x13
#define SPI_NAND_FAST_READ_X1		0x0b
#define SPI_NAND_READ_X1		0x03
#define SPI_NAND_READ_X2		0x3b
#define SPI_NAND_READ_X4            	0x6b
#define SPI_NAND_READ_DUAL_IO 	    	0xbb
#define SPI_NAND_READ_QUAD_IO 	    	0xeb
#define SPI_NAND_RDID  		        0x9f
#define SPI_NAND_PP    		        0x02
#define SPI_NAND_PP_X4    	        0x32
#define SPI_NAND_RANDOM_PP    		0x84
#define SPI_NAND_RANDOM_PP_X4    	0x34
#define SPI_NAND_PE			0x10
#define SPI_NAND_BE    		        0xd8
#define SPI_NAND_RESET 		        0xff
#define SPI_NAND_READ_INT_ECCSTATUS	0x7c

//#define readb(addr)		(*((volatile unsigned char  *)(addr)))
//#define readw(addr)		(*((volatile unsigned int *)(addr)))
//#define writeb(v, addr)	(*((volatile unsigned char  *)(addr)) = (unsigned char)(v))
//#define writew(v, addr)	(*((volatile unsigned int *)(addr)) = (unsigned int)(v))

int spic0_init(void);
int spic0_exit(void);
int spic0_change_mode(unsigned int clk);
void spic0_samp_dl_sw_status(unsigned int status);
int update_right_delay_para(struct mtd_info *mtd);
int set_right_delay_para(struct mtd_info *mtd);

#define SPI0_MODE_NOTSET 0
#define SPI0_MODE_AUTOSET 1
extern int spi0_write(void *txbuf, unsigned int txnum, int mode);
extern int spi0_write_then_read(void *txbuf, unsigned int txnum,
		void *rxbuf, unsigned int rxnum, int mode);
extern void spic0_config_io_mode(unsigned int rxmode, unsigned int dbc,
		unsigned int stc);
#endif
