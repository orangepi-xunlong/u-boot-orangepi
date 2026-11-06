/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2021. All rights reserved.
 *
 * File Name: cv_spinor.h
 *
 * Description: Cvitek SPI NOR flash driver header
 */

#ifndef __CV_SPINOR_H__
#define __CV_SPINOR_H__

#define BASE_SPI0           0x10000000
#define BASE_SPI1           0x20000000
#define BASE_SPI            BASE_SPI0

/* spi register definitions */
#define REG_SPI_CTRL        (BASE_SPI + 0x00)
#define REG_SPI_CE_CTRL     (BASE_SPI + 0x04)
#define REG_SPI_DLY_CTRL    (BASE_SPI + 0x08)
#define REG_SPI_DMMR        (BASE_SPI + 0x0C)
#define REG_SPI_TRAN_CSR    (BASE_SPI + 0x10)
#define REG_SPI_TRAN_NUM    (BASE_SPI + 0x14)
#define REG_SPI_FIFO_PORT   (BASE_SPI + 0x18)
#define REG_SPI_FIFO_PT     (BASE_SPI + 0x20)
#define REG_SPI_INT_STS     (BASE_SPI + 0x28)
#define REG_SPI_INT_EN      (BASE_SPI + 0x2C)

/* spi-nor commands */
#define CMD_EN4B            0xB7
#define CMD_EX4B            0xE9    // XM25QH*: 29h
#define CMD_WREN            0x06
#define CMD_WRDI            0x04
#define CMD_RDID            0x9F
#define CMD_RDSR            0x05
#define CMD_WRSR            0x01
#define CMD_READ            0x03
#define CMD_READ4B          0x13
#define CMD_PP              0x02
#define CMD_PP4B            0x12
#define CMD_SE              0x20
#define CMD_SE4B            0x21
#define CMD_BE              0xD8
#define CMD_BE4B            0xDC

/* spi-nor status register */
#define SPI_STATUS_WIP          (1 << 0)
#define SPI_STATUS_WEL          (1 << 1)
#define SPI_STATUS_BP0          (1 << 2)
#define SPI_STATUS_BP1          (1 << 3)
#define SPI_STATUS_BP2          (1 << 4)
#define SPI_STATUS_BP3          (1 << 5)
#define SPI_STATUS_TP           (1 << 6)
#define SPI_STATUS_SRWD         (1 << 7)

/* register bit definition */
#define BIT_CTRL_CPHA                   (1 << 12)
#define BIT_CTRL_CPOL                   (1 << 13)
#define BIT_CTRL_HOLD_OL                (1 << 14)
#define BIT_CTRL_WP_OL                  (1 << 15)
#define BIT_CTRL_FL                     (1 << 16)
#define BIT_CTRL_LSBF                   (1 << 20)
#define BIT_CTRL_SRST                   (1 << 21)

#define BIT_CE_CTRL_CEMANUAL            (1 << 0)
#define BIT_CE_CTRL_CEMANUAL_EN         (1 << 1)

#define BIT_DMMR_DIS                    (0 << 0)
#define BIT_DMMR_EN                     (1 << 0)

#define BIT_TRAN_CSR_TRAN_MODE_RX       (1 << 0)
#define BIT_TRAN_CSR_TRAN_MODE_TX       (1 << 1)
#define BIT_TRAN_CSR_CNTNS_READ         (1 << 2)
#define BIT_TRAN_CSR_FAST_MODE          (1 << 3)
#define BIT_TRAN_CSR_BUSWIDTH_1_BIT     (0 << 4)
#define BIT_TRAN_CSR_BUSWIDTH_2_BIT     (1 << 4)
#define BIT_TRAN_CSR_BUSWIDTH_4_BIT     (2 << 4)
#define BIT_TRAN_CSR_DMA_EN             (1 << 6)
#define BIT_TRAN_CSR_MISO_LEVEL         (1 << 7)
#define BIT_TRAN_CSR_ADDR_BN            (1 << 8)
#define BIT_TRAN_CSR_WITH_CMD           (1 << 11)
#define BIT_TRAN_CSR_FIFO_TRGLVL_1_BYTE (0 << 12)
#define BIT_TRAN_CSR_FIFO_TRGLVL_2_BYTE (1 << 12)
#define BIT_TRAN_CSR_FIFO_TRGLVL_4_BYTE (2 << 12)
#define BIT_TRAN_CSR_FIFO_TRGLVL_8_BYTE (3 << 12)
#define BIT_TRAN_CSR_GO_BUSY            (1 << 15)
#define BIT_TRAN_CSR_TRAN_MODE_MASK     (0x0003)
#define BIT_TRAN_CSR_ADDR_BYTES_MASK    (0x0700)
#define BIT_TRAN_CSR_FIFO_TRGLVL_MASK   (0x3000)
#define BIT_TRAN_CSR_BUS_WIDTH_MASK	(0x30)
#define BIT_DLY_CTRL_CET                (3 << 8)
#define BIT_DLY_CTRL_NEG_SAMPLE         (1 << 14)

#define BIT_INT_TRAN_DONE               (1 << 0)
#define BIT_INT_RD_FIFO                 (1 << 2)
#define BIT_INT_WR_FIFO                 (1 << 3)
#define BIT_INT_RX_FRAME                (1 << 4)
#define BIT_INT_TX_FRAME                (1 << 5)

#define BIT_INT_TRAN_DONE_EN            (1 << 0)
#define BIT_INT_RD_FIFO_EN              (1 << 2)
#define BIT_INT_WR_FIFO_EN              (1 << 3)
#define BIT_INT_RX_FRAME_EN             (1 << 4)
#define BIT_INT_TX_FRAME_EN             (1 << 5)

/* general definition */
#define SPI_FLASH_BLOCK_SIZE            256
#define SPI_TRAN_CSR_ADDR_BYTES_SHIFT   8
#define SPI_TRAN_CSR_DUMMY_CYCLE_SHIFT  16
#define SPI_MAX_FIFO_DEPTH              8

#define SIZE_SECTOR_4K          0x1000
#define SIZE_SECTOR_64K         0x10000
#define SIZE_256B               256
#define SIZE_16MB               0x18
#define SPI_CLK_75M             1
#define SPI_CLK_50M             2
#define SPI_CLK_37M             3
#define SPI_CLK_30M             4
#define SPI_CLK_15M             9

#define CSR_DMMR_READ	CSR_DMMR_READ_6B

/* 03h: 0x3B01*/
#define CSR_DMMR_READ_03	(BIT_TRAN_CSR_TRAN_MODE_RX |\
	BIT_TRAN_CSR_BUSWIDTH_1_BIT |\
	(0x3 << SPI_TRAN_CSR_ADDR_BYTES_SHIFT) |\
	BIT_TRAN_CSR_WITH_CMD |\
	BIT_TRAN_CSR_FIFO_TRGLVL_8_BYTE|\
	0<<SPI_TRAN_CSR_DUMMY_CYCLE_SHIFT)

/* 6Bh: 0x83B21*/
#define CSR_DMMR_READ_6B	(BIT_TRAN_CSR_TRAN_MODE_RX |\
	BIT_TRAN_CSR_BUSWIDTH_4_BIT |\
	(0x3 << SPI_TRAN_CSR_ADDR_BYTES_SHIFT) |\
	BIT_TRAN_CSR_WITH_CMD |\
	BIT_TRAN_CSR_FIFO_TRGLVL_8_BYTE|\
	0x8<<SPI_TRAN_CSR_DUMMY_CYCLE_SHIFT)

/* EBh: 0x63B29*/
#define CSR_DMMR_READ_EB	(BIT_TRAN_CSR_TRAN_MODE_RX |\
	BIT_TRAN_CSR_FAST_MODE |\
	BIT_TRAN_CSR_BUSWIDTH_4_BIT |\
	(0x3 << SPI_TRAN_CSR_ADDR_BYTES_SHIFT) |\
	BIT_TRAN_CSR_WITH_CMD |\
	BIT_TRAN_CSR_FIFO_TRGLVL_8_BYTE|\
	0x6<<SPI_TRAN_CSR_DUMMY_CYCLE_SHIFT)

/* ECh: 0x363C29*/
#define CSR_DMMR_READ_4B	(BIT_TRAN_CSR_TRAN_MODE_RX |\
	BIT_TRAN_CSR_FAST_MODE |\
	BIT_TRAN_CSR_BUSWIDTH_4_BIT |\
	(0x4 << SPI_TRAN_CSR_ADDR_BYTES_SHIFT) |\
	BIT_TRAN_CSR_WITH_CMD |\
	BIT_TRAN_CSR_FIFO_TRGLVL_8_BYTE|\
	0x36<<SPI_TRAN_CSR_DUMMY_CYCLE_SHIFT)

#define SPI_ID_M25P128          0x00182020
#define SPI_ID_N25Q128          0x0018ba20
#define SPI_ID_GD25LQ128        0x001860c8

/* struct */
struct cmd_set {
	uint8_t rd;
	uint8_t pp;
	uint8_t se;
	uint8_t be;
	uint32_t csr;
};

/* functions */
void cv_spinor_init(uint32_t clk);
void cv_spinor_read(void *dest, uintptr_t addr, size_t len);
int cv_spinor_program(uint32_t addr, uintptr_t buf, size_t len);
int cv_spinor_erase(uint32_t addr, size_t len);

#endif	/* __CV_SPINOR_H__ */
