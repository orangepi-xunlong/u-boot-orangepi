/*
 * Allwinner SoCs hdmi2.0 driver.
 *
 * Copyright (C) 2016 Allwinner.
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include "cec.h"
#include "core/irq.h"
#if defined(__LINUX_PLAT__)
#include <linux/delay.h>



/******************************************************************************
 *
 *****************************************************************************/
/**
 * Wait for message (quit after a given period)
 * @param dev:     address and device information
 * @param buf:     buffer to hold data
 * @param size:    maximum data length [byte]
 * @param timeout: time out period [ms]
 * @return error code or received bytes
 */
static int cec_msgRx(hdmi_tx_dev_t *dev, char *buf, unsigned size,
		 int timeout)
{
	int i, retval = -1;

	if (timeout < 0) {
		/* TO BE USED only by Receptor */
		while (cec_GetLocked(dev) == 0)
			;
	} else {
		for (i = 0; i < (timeout/100); i++) {
			if (cec_GetLocked(dev) != 0)
				break;
			snps_sleep(100);
		}
	}
	if (cec_GetLocked(dev) != 0) {
		retval = cec_CfgRxBuf(dev, buf, size);
		cec_IntStatusClear(dev, CEC_MASK_EOM_MASK | CEC_MASK_ERROR_FLOW_MASK);
		cec_SetLocked(dev);
	}
	return retval;
}

/**
 * Send a message (retry in case of failure)
 * @param dev:   address and device information
 * @param buf:   data to send
 * @param size:  data length [byte]
 * @param retry: maximum number of retries
 * @return error code or transmitted bytes
 */
static int cec_msgTx(hdmi_tx_dev_t *dev, const char *buf, unsigned size,
		 unsigned retry)
{
	int timeout = 2;
	int i, retval = -1;

	if (size > 0) {
		if (cec_CfgTxBuf(dev, buf, size) == (int)size) {
			for (i = 0; i < (int)retry; i++) {
				if (cec_CfgSignalFreeTime(dev, i ? 3 : 5))
					break;

				cec_IntEnable(dev, CEC_MASK_DONE_MASK
						| CEC_MASK_NACK_MASK
						| CEC_MASK_ARB_LOST_MASK
						| CEC_MASK_ERROR_INITIATOR_MASK);
				cec_SetSend(dev);
				while (cec_GetSend(dev) != 0 && timeout) {
					msleep(200);
					timeout--;
				}
				if (cec_IntStatus(dev,
					CEC_MASK_DONE_MASK) != 0) {
					cec_IntStatusClear(dev, CEC_MASK_DONE_MASK);
					HDMI_INFO_MSG("cec send %d bytes, retry %d\n", size, i);
					retval = size;
					break;
				}
			}
		}
	}
	return retval;
}

/**
 * Open a CEC controller
 * @warning Execute before start using a CEC controller
 * @param dev:  address and device information
 * @return error code
 */
int cec_Init(hdmi_tx_dev_t *dev)
{
	/*
	 * This function is also called after wake up,
	 * so it must NOT reset logical address allocation
	 */
	cec_CfgStandbyMode(dev, 0);
	cec_IntDisable(dev, CEC_MASK_WAKEUP_MASK);
	cec_IntClear(dev, CEC_MASK_WAKEUP_MASK);
	cec_IntEnable(dev, CEC_MASK_DONE_MASK | CEC_MASK_EOM_MASK
					| CEC_MASK_NACK_MASK
					| CEC_MASK_ARB_LOST_MASK
					| CEC_MASK_ERROR_FLOW_MASK
					| CEC_MASK_ERROR_INITIATOR_MASK);

	return 0;
}

/**
 * Close a CEC controller
 * @warning Execute before stop using a CEC controller
 * @param dev:    address and device information
 * @param wakeup: enable wake up mode (don't enable it in TX side)
 * @return error code
 */
int cec_Disable(hdmi_tx_dev_t *dev, int wakeup)
{
	cec_IntDisable(dev, CEC_MASK_DONE_MASK | CEC_MASK_EOM_MASK |
		CEC_MASK_NACK_MASK | CEC_MASK_ARB_LOST_MASK
		| CEC_MASK_ERROR_FLOW_MASK
		| CEC_MASK_ERROR_INITIATOR_MASK);
	if (wakeup) {
		cec_IntClear(dev, CEC_MASK_WAKEUP_MASK);
		cec_IntEnable(dev, CEC_MASK_WAKEUP_MASK);
		cec_CfgStandbyMode(dev, 1);
	} else
		cec_CfgLogicAddr(dev, BCST_ADDR, 1);
	return 0;
}

/**
 * Configure logical address
 * @param dev:    address and device information
 * @param addr:   logical address
 * @param enable: alloc/free address
 * @return error code
 */
int cec_CfgLogicAddr(hdmi_tx_dev_t *dev, unsigned addr, int enable)
{
	unsigned int regs;

	if (addr > BCST_ADDR) {
		HDMI_ERROR_MSG("invalid parameter\n");
		return -1;
	}
	if (addr == BCST_ADDR) {
		if (enable) {
			dev_write(dev, CEC_ADDR_H, 0x80);
			dev_write(dev, CEC_ADDR_L, 0x00);
			return 0;
		} else {
			HDMI_ERROR_MSG("cannot de-allocate broadcast logical address\n");
			return -1;
		}
	}
	regs = (addr > 8) ? CEC_ADDR_H : CEC_ADDR_L;
	addr = (addr > 8) ? (addr - 8) : addr;
	if (enable) {
		dev_write(dev, CEC_ADDR_H, 0x00);
		dev_write(dev, regs, dev_read(dev, regs) |  (1 << addr));
	} else
		dev_write(dev, regs, dev_read(dev, regs) & ~(1 << addr));
	return 0;
}

/**
 * Configure standby mode
 * @param dev:    address and device information
 * @param enable: if true enable standby mode
 * @return error code
 */
int cec_CfgStandbyMode(hdmi_tx_dev_t *dev, int enable)
{
	if (enable)
		dev_write(dev, CEC_CTRL, dev_read(dev, CEC_CTRL)
						| CEC_CTRL_STANDBY_MASK);
	else
		dev_write(dev, CEC_CTRL, dev_read(dev, CEC_CTRL)
					& ~CEC_CTRL_STANDBY_MASK);
	return 0;
}



/**
 * Configure signal free time
 * @param dev:  address and device information
 * @param time: time between attempts [nr of frames]
 * @return error code
 */
int cec_CfgSignalFreeTime(hdmi_tx_dev_t *dev, int time)
{
	unsigned char data;

	HDMI_ERROR_MSG("%i", time);
	data = dev_read(dev, CEC_CTRL) & ~(CEC_CTRL_FRAME_TYP_MASK);
	if (time == 3)
		;                                  /* 0 */
	else if (time == 5)
		data |= FRAME_TYP0;                /* 1 */
	else if (time == 7)
		data |= FRAME_TYP1 | FRAME_TYP0;   /* 2 */
	else
		return -1;
	dev_write(dev, CEC_CTRL, data);
	return 0;
}

/**
 * Read send status
 * @param dev:  address and device information
 * @return SEND status
 */
int cec_GetSend(hdmi_tx_dev_t *dev)
{
	return (dev_read(dev, CEC_CTRL) & CEC_CTRL_SEND_MASK) != 0;
}

/**
 * Set send status
 * @param dev: address and device information
 * @return error code
 */
int cec_SetSend(hdmi_tx_dev_t *dev)
{
	dev_write(dev, CEC_CTRL, dev_read(dev, CEC_CTRL) | CEC_CTRL_SEND_MASK);
	return 0;
}

/**
 * Clear interrupts
 * @param dev:  address and device information
 * @param mask: interrupt mask to clear
 * @return error code
 */
int cec_IntClear(hdmi_tx_dev_t *dev, unsigned char mask)
{
	dev_write(dev, CEC_MASK, mask);
	return 0;
}

/**
 * Disable interrupts
 * @param dev:  address and device information
 * @param mask: interrupt mask to disable
 * @return error code
 */
int cec_IntDisable(hdmi_tx_dev_t *dev, unsigned char mask)
{
	HDMI_INFO_MSG("0x%02X\n", mask);
	dev_write(dev, CEC_MASK, dev_read(dev, CEC_MASK) | mask);
	return 0;
}

/**
 * Enable interrupts
 * @param dev:  address and device information
 * @param mask: interrupt mask to enable
 * @return error code
 */
int cec_IntEnable(hdmi_tx_dev_t *dev, unsigned char mask)
{
	dev_write(dev, CEC_MASK, dev_read(dev, CEC_MASK) & ~mask);
	return 0;
}

/**
 * Read interrupts
 * @param dev:  address and device information
 * @param mask: interrupt mask to read
 * @return INT content
 */
int cec_IntStatus(hdmi_tx_dev_t *dev, unsigned char mask)
{
	return dev_read(dev, IH_CEC_STAT0) & mask;
}

int cec_IntStatusClear(hdmi_tx_dev_t *dev, unsigned char mask)
{
	/* write 1 to clear */
	dev_write(dev, IH_CEC_STAT0, mask);
	return 0;
}

/**
 * Write transmission buffer
 * @param dev:  address and device information
 * @param buf:  data to transmit
 * @param size: data length [byte]
 * @return error code or bytes configured
 */
int cec_CfgTxBuf(hdmi_tx_dev_t *dev, const char *buf, unsigned size)
{
	unsigned i;
	if (size > CEC_TX_DATA_SIZE) {
		HDMI_ERROR_MSG("invalid parameter\n");
		return -1;
	}
	dev_write(dev, CEC_TX_CNT, size);   /* mask 7-5? */
	for (i = 0; i < size; i++)
		dev_write(dev, CEC_TX_DATA + (i * 4), *buf++);
	return size;
}

/**
 * Read reception buffer
 * @param dev:  address and device information
 * @param buf:  buffer to hold receive data
 * @param size: maximum data length [byte]
 * @return error code or received bytes
 */
int cec_CfgRxBuf(hdmi_tx_dev_t *dev, char *buf, unsigned size)
{
	unsigned i;
	unsigned char cnt;

	HDMI_INFO_MSG("%u\n", size);
	cnt = dev_read(dev, CEC_RX_CNT);   /* mask 7-5? */
	cnt = (cnt > size) ? size : cnt;
	for (i = 0; i < size; i++)
		*buf++ = dev_read(dev, CEC_RX_DATA + (i * 4));
	if (cnt > CEC_RX_DATA_SIZE) {
		HDMI_ERROR_MSG("wrong byte count\n");
		return -1;
	}
	return cnt;
}

/**
 * Read locked status
 * @param dev: address and device information
 * @return LOCKED status
 */
int cec_GetLocked(hdmi_tx_dev_t *dev)
{
	return (dev_read(dev, CEC_LOCK) & CEC_LOCK_LOCKED_BUFFER_MASK) != 0;
}

/**
 * Set locked status
 * @param dev: address and device information
 * @return error code
 */
int cec_SetLocked(hdmi_tx_dev_t *dev)
{
	dev_write(dev, CEC_LOCK, dev_read(dev, CEC_LOCK)
					& ~CEC_LOCK_LOCKED_BUFFER_MASK);
	return true;
}

/**
 * Configure broadcast rejection
 * @param dev:   address and device information
 * @param enable: if true enable broadcast rejection
 * @return error code
 */
int cec_CfgBroadcastNAK(hdmi_tx_dev_t *dev, int enable)
{
	if (enable)
		dev_write(dev, CEC_CTRL, dev_read(dev, CEC_CTRL)
						| CEC_CTRL_BC_NACK_MASK);
	else
		dev_write(dev, CEC_CTRL,
			dev_read(dev, CEC_CTRL) & ~CEC_CTRL_BC_NACK_MASK);
	return true;
}

/**
 * Attempt to receive data (quit after 1s)
 * @warning Caller context must allow sleeping
 * @param dev:  address and device information
 * @param buf:  container for incoming data (first byte will be header block)
 * @param size: maximum data size [byte]
 * @return error code or received message size [byte]
 */
int cec_ctrlReceiveFrame(hdmi_tx_dev_t *dev, char *buf, unsigned size)
{
	if (buf == NULL) {
		HDMI_ERROR_MSG("invalid parameter\n");
		return -1;
	}
	return cec_msgRx(dev, buf, size, 100);
}

/**
 * Attempt to send data (retry 5 times)
 * @warning Doesn't check if source address is allocated
 * @param dev:  address and device information
 * @param buf:  container of the outgoing data (without header block)
 * @param size: request size [byte] (must be less than 15)
 * @param src:  initiator logical address
 * @param dst:  destination logical address
 * @return error code or request size [byte]
 */
int cec_ctrlSendFrame(hdmi_tx_dev_t *dev, const char *buf, unsigned size,
		  unsigned src, unsigned dst)
{
	char tmp[100];
	int i;

	HDMI_INFO_MSG("%u, %u, %u\n", size, src, dst);
	if (buf == NULL || size >= CEC_TX_DATA_SIZE ||
	    src > BCST_ADDR || dst > BCST_ADDR) {
		HDMI_ERROR_MSG("invalid parameter\n");
		return -1;
	}

	tmp[0] = src << 4 | dst;
	for (i = 0; i < (int)size; i++)
		tmp[i + 1] = buf[i];

	if (cec_msgTx(dev, tmp, size + 1, 5) < 0)
		return -1;
	else
		return size;
}
#endif
