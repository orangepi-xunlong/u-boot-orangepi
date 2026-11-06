/*
 * Copyright (c) 2016-2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __CV_I2C_H__
#define __CV_I2C_H__

#include <cv_private.h>

enum {
	I2C_DL_SLAVE_READ = 0,
	I2C_DL_SLAVE_END,
	I2C_DL_HOST_ASK,
	I2C_DL_HOST_WRITE,
};
struct i2c_dl_msg {
	uint8_t slave_addr;
	uint8_t op_id;
	uint16_t seq_no;
	uint64_t io_addr;
	uint32_t length; // including header size
	uint8_t data[0];
} __attribute__((packed));

/* register definitions */
#define REG_I2C_CON                      0x000
#define REG_I2C_TAR                      0x004
#define REG_I2C_SAR                      0x008
#define REG_I2C_HS_MADDR                 0x00C
#define REG_I2C_DATA_CMD                 0x010
#define REG_I2C_SS_SCL_HCNT              0x014
#define REG_I2C_SS_SCL_LCNT              0x018
#define REG_I2C_FS_SCL_HCNT              0x01C
#define REG_I2C_FS_SCL_LCNT              0x020
#define REG_I2C_HS_SCL_HCNT              0x024
#define REG_I2C_HS_SCL_LCNT              0x028
#define REG_I2C_INT_STAT                 0x02C
#define REG_I2C_INT_MASK                 0x030
#define REG_I2C_RAW_INT_STAT             0x034
#define REG_I2C_RX_TL                    0x038
#define REG_I2C_TX_TL                    0x03C
#define REG_I2C_CLR_INTR                 0x040
#define REG_I2C_CLR_RX_UNDER             0x044
#define REG_I2C_CLR_RX_OVER              0x048
#define REG_I2C_CLR_TX_OVER              0x04C
#define REG_I2C_CLR_RD_REQ               0x050
#define REG_I2C_CLR_TX_ABRT              0x054
#define REG_I2C_CLR_RX_DONE              0x058
#define REG_I2C_CLR_ACTIVITY             0x05C
#define REG_I2C_CLR_STOP_DET             0x060
#define REG_I2C_CLR_START_DET            0x064
#define REG_I2C_CLR_GEN_ALL              0x068
#define REG_I2C_ENABLE                   0x06C
#define REG_I2C_STATUS                   0x070
#define REG_I2C_TXFLR                    0x074
#define REG_I2C_RXFLR                    0x078
#define REG_I2C_SDA_HOLD                 0x07C
#define REG_I2C_TX_ABRT_SOURCE           0x080
#define REG_I2C_SLV_DATA_NACK_ONLY       0x084
#define REG_I2C_DMA_CR                   0x088
#define REG_I2C_DMA_TDLR                 0x08C
#define REG_I2C_DMA_RDLR                 0x090
#define REG_I2C_SDA_SETUP                0x094
#define REG_I2C_ACK_GENERAL_CALL         0x098
#define REG_I2C_ENABLE_STATUS            0x09C
#define REG_I2C_FS_SPKLEN                0x0A0
#define REG_I2C_HS_SPKLEN                0x0A4
#define REG_I2C_COMP_PARAM_1             0x0F4
#define REG_I2C_COMP_VERSION             0x0F8
#define REG_I2C_COMP_TYPE                0x0FC

/* bit definition */
#define BIT_I2C_CON_MASTER_MODE              (0x01 << 0)
#define BIT_I2C_CON_STANDARD_SPEED           (0x01 << 1)
#define BIT_I2C_CON_FULL_SPEED               (0x02 << 1)
#define BIT_I2C_CON_HIGH_SPEED               (0x03 << 1)
#define BIT_I2C_CON_10B_ADDR_SLAVE           (0x01 << 3)
#define BIT_I2C_CON_10B_ADDR_MASTER          (0x01 << 4)
#define BIT_I2C_CON_RESTART_EN               (0x01 << 5)
#define BIT_I2C_CON_SLAVE_DIS                (0x01 << 6)

#define BIT_I2C_INT_RX_UNDER                 (0x01 << 0)
#define BIT_I2C_INT_RX_OVER                  (0x01 << 1)
#define BIT_I2C_INT_RX_FULL                  (0x01 << 2)
#define BIT_I2C_INT_TX_OVER                  (0x01 << 3)
#define BIT_I2C_INT_TX_EMPTY                 (0x01 << 4)
#define BIT_I2C_INT_RD_REQ                   (0x01 << 5)
#define BIT_I2C_INT_TX_ABRT                  (0x01 << 6)
#define BIT_I2C_INT_RX_DONE                  (0x01 << 7)
#define BIT_I2C_INT_ACTIVITY                 (0x01 << 8)
#define BIT_I2C_INT_STOP_DET                 (0x01 << 9)
#define BIT_I2C_INT_START_DET                (0x01 << 10)
#define BIT_I2C_INT_GEN_ALL                  (0x01 << 11)

#define BIT_I2C_INT_RX_UNDER_MASK            (0x01 << 0)
#define BIT_I2C_INT_RX_OVER_MASK             (0x01 << 1)
#define BIT_I2C_INT_RX_FULL_MASK             (0x01 << 2)
#define BIT_I2C_INT_TX_OVER_MASK             (0x01 << 3)
#define BIT_I2C_INT_TX_EMPTY_MASK            (0x01 << 4)
#define BIT_I2C_INT_RD_REQ_MASK              (0x01 << 5)
#define BIT_I2C_INT_TX_ABRT_MASK             (0x01 << 6)
#define BIT_I2C_INT_RX_DONE_MASK             (0x01 << 7)
#define BIT_I2C_INT_ACTIVITY_MASK            (0x01 << 8)
#define BIT_I2C_INT_STOP_DET_MASK            (0x01 << 9)
#define BIT_I2C_INT_START_DET_MASK           (0x01 << 10)
#define BIT_I2C_INT_GEN_ALL_MASK             (0x01 << 11)

#define BIT_I2C_INT_RX_UNDER_RAW             (0x01 << 0)
#define BIT_I2C_INT_RX_OVER_RAW              (0x01 << 1)
#define BIT_I2C_INT_RX_FULL_RAW              (0x01 << 2)
#define BIT_I2C_INT_TX_OVER_RAW              (0x01 << 3)
#define BIT_I2C_INT_TX_EMPTY_RAW             (0x01 << 4)
#define BIT_I2C_INT_RD_REQ_RAW               (0x01 << 5)
#define BIT_I2C_INT_TX_ABRT_RAW              (0x01 << 6)
#define BIT_I2C_INT_RX_DONE_RAW              (0x01 << 7)
#define BIT_I2C_INT_ACTIVITY_RAW             (0x01 << 8)
#define BIT_I2C_INT_STOP_DET_RAW             (0x01 << 9)
#define BIT_I2C_INT_START_DET_RAW            (0x01 << 10)
#define BIT_I2C_INT_GEN_ALL_RAW              (0x01 << 11)

#define BIT_I2C_CMD_DATA_READ_BIT           (0x01 << 8)
#define BIT_I2C_CMD_DATA_STOP_BIT           (0x01 << 9)
#define BIT_I2C_CMD_DATA_RESTART_BIT        (0x01 << 10)

size_t i2c_dl_read_data(unsigned long lba, uintptr_t buf, size_t size);
void bm_i2c_init(unsigned long base, uint16_t addr);
void bm_i2c_disable(void);
void bm_i2c_master_init(unsigned long base, uint16_t addr);
void bm_i2c_master_write(uint8_t reg, uint8_t data);
int bm_i2c_master_read(uint8_t reg);

#endif	/* __CV_I2C_H__ */
