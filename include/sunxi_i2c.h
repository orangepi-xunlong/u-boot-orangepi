/*
 * Copyright (C) 2016 Allwinner.
 * zhouhuacai <zhouhuacai@allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 *
 * SUNXI TWI Controller Definition
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#ifndef _SUNXI_I2C_H_
#define _SUNXI_I2C_H_

/* status or interrupt source */
/*------------------------------------------------------------------------------
* Code   Status
* 00h    Bus error
* 08h    START condition transmitted
* 10h    Repeated START condition transmitted
* 18h    Address + Write bit transmitted, ACK received
* 20h    Address + Write bit transmitted, ACK not received
* 28h    Data byte transmitted in master mode, ACK received
* 30h    Data byte transmitted in master mode, ACK not received
* 38h    Arbitration lost in address or data byte
* 40h    Address + Read bit transmitted, ACK received
* 48h    Address + Read bit transmitted, ACK not received
* 50h    Data byte received in master mode, ACK transmitted
* 58h    Data byte received in master mode, not ACK transmitted
* 60h    Slave address + Write bit received, ACK transmitted
* 68h    Arbitration lost in address as master, slave address + Write bit received, ACK transmitted
* 70h    General Call address received, ACK transmitted
* 78h    Arbitration lost in address as master, General Call address received, ACK transmitted
* 80h    Data byte received after slave address received, ACK transmitted
* 88h    Data byte received after slave address received, not ACK transmitted
* 90h    Data byte received after General Call received, ACK transmitted
* 98h    Data byte received after General Call received, not ACK transmitted
* A0h    STOP or repeated START condition received in slave mode
* A8h    Slave address + Read bit received, ACK transmitted
* B0h    Arbitration lost in address as master, slave address + Read bit received, ACK transmitted
* B8h    Data byte transmitted in slave mode, ACK received
* C0h    Data byte transmitted in slave mode, ACK not received
* C8h    Last byte transmitted in slave mode, ACK received
* D0h    Second Address byte + Write bit transmitted, ACK received
* D8h    Second Address byte + Write bit transmitted, ACK not received
* F8h    No relevant status information or no interrupt
*-----------------------------------------------------------------------------*/

#define I2C_START_TRANSMIT     0x08
#define I2C_RESTART_TRANSMIT   0x10
#define I2C_ADDRWRITE_ACK      0x18
#define I2C_ADDRREAD_ACK       0x40
#define I2C_DATAWRITE_ACK      0x28
#define I2C_READY              0xf8
#define I2C_DATAREAD_NACK      0x58
#define I2C_DATAREAD_ACK       0x50

#define SUNXI_I2C_OK      0
#define SUNXI_I2C_FAIL   -1
#define SUNXI_I2C_RETRY  -2
#define SUNXI_I2C_SFAIL  -3
#define SUNXI_I2C_TFAIL  -4
#define SUNXI_I2C_TOUT   -5

#define I2C_WRITE         0
#define I2C_READ          1

/* TWI Soft Reset Register Bit Fields & Masks  */
#define TWI_SRST_SRST   (0x1<<0)

/* TWI Control Register Bit Fields & Masks, default value: 0x0000_0000*/
#define TWI_CTL_ACK     (0x1<<2)
#define TWI_CTL_INTFLG  (0x1<<3)
#define TWI_CTL_STP     (0x1<<4)
#define TWI_CTL_STA     (0x1<<5)
#define TWI_CTL_BUSEN   (0x1<<6)
#define TWI_CTL_INTEN   (0x1<<7)
/* 31:8 bit reserved */

/* twi line control register -default value: 0x0000_003a */
#define TWI_LCR_SDA_EN          (0x01<<0)
#define TWI_LCR_SDA_CTL         (0x01<<1)
#define TWI_LCR_SCL_EN          (0x01<<2)
#define TWI_LCR_SCL_CTL         (0x01<<3)
#define TWI_LCR_SDA_STATE_MASK  (0x01<<4)
#define TWI_LCR_SCL_STATE_MASK  (0x01<<5)

#define TWI_STAT_IDLE           (0xF8)
#define TWI_LCR_NORM_STATUS     (0x30)




#define TWI_CONTROL_OFFSET             0x400
#define SUNXI_I2C_CONTROLLER             5

struct sunxi_twi_reg
{
    volatile unsigned int addr;        /* slave address     */
    volatile unsigned int xaddr;       /* extend address    */
	volatile unsigned int data;        /* data              */
    volatile unsigned int ctl;         /* control           */
    volatile unsigned int status;      /* status            */
    volatile unsigned int clk;         /* clock             */
    volatile unsigned int srst;        /* soft reset        */
    volatile unsigned int eft;         /* enhanced future   */
    volatile unsigned int lcr;         /* line control      */
    volatile unsigned int dvfs;        /* dvfs control      */
};

enum {
	SUNXI_PHY_I2C0 = 0,

	SUNXI_PHY_I2C1,

	SUNXI_PHY_I2C2,

	SUNXI_PHY_I2C3,

	SUNXI_PHY_I2C4,

	SUNXI_PHY_I2C5,

	SUNXI_PHY_R_I2C0,

	SUNXI_PHY_R_I2C1,

	/*The new i2c bus must be added before SUNXI_PHY_I2C_BUS_MAX*/
	SUNXI_PHY_I2C_BUS_MAX,

};

enum {
#ifdef CONFIG_I2C0_ENABLE
	SUNXI_VIR_I2C0,
#endif
#ifdef CONFIG_I2C1_ENABLE
	SUNXI_VIR_I2C1,
#endif
#ifdef CONFIG_I2C2_ENABLE
	SUNXI_VIR_I2C2,
#endif
#ifdef CONFIG_I2C3_ENABLE
	SUNXI_VIR_I2C3,
#endif
#ifdef CONFIG_I2C4_ENABLE
	SUNXI_VIR_I2C4,
#endif
#ifdef CONFIG_I2C5_ENABLE
	SUNXI_VIR_I2C5,
#endif
#ifdef CONFIG_R_I2C0_ENABLE
	SUNXI_VIR_R_I2C0,
#endif
#ifdef CONFIG_R_I2C1_ENABLE
	SUNXI_VIR_R_I2C1,
#endif
	/*The new i2c bus must be added before SUNXI_VIR_I2C_BUS_MAX*/
	SUNXI_VIR_I2C_BUS_MAX,
};


#endif /* _SUNXI_I2C_H_ */

