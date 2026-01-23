// SPDX-License-Identifier: GPL-2.0+
/**
 * ufs.c - Universal Flash Subsystem (UFS) driver
 *
 * Taken from Linux Kernel v5.2 (drivers/scsi/ufs/ufshcd.c) and ported
 * to u-boot.
 *
 * Copyright (C) 2019 Texas Instruments Incorporated - http://www.ti.com
 * Change by lixiang <lixiang@allwinnertech.com> at 2022/11/15/15:24
*/

#include <common.h>
#include "errno.h"
#include <types.h>
#include <log.h>
#include <malloc.h>
#include <./uinclude/umalloc.h>
#include <linux/stddef.h>
#include <dma-direction.h>
#include <compat.h>
#include <device_compat.h>
#include <linux/kernel.h>
#include <string.h>
#include <delay.h>
#include "./uinclude/scsi.h"
#include <read.h>
#include <util.h>
#include <./uinclude/blk.h>
#include <./uinclude/scsi_proto.h>
#include <./uinclude/scsi_status.h>
#include <time.h>
#include <sunxi-ufs.h>
#include "sunxi-ufs-trace.h"
#include "charset.h"
#include "hexdump.h"

#include "ufs.h"
#include <device.h>
#include <linux/unaligned/be_byteshift.h>


#define  DES_READ_SUP
#define PWR_MODE_CHANGE
#define PWR_HS_MODE
//#define PWR_G3

#define SUNXI_UFS_DES_SIZE  (4096*2)
u8 g_ufs_des_base[SUNXI_UFS_DES_SIZE];

#define UTRDL_SIZE  (sizeof(struct utp_transfer_req_desc)*4)
#define ALIGN_1K_UTRDL_ADDR  ((((u32)g_ufs_des_base + 1024 - 1)/1024)*1024)
#define ALIGN_128_UCDL_ADDR  (((ALIGN_1K_UTRDL_ADDR+ UTRDL_SIZE + 128 - 1)/128)*128)


/*********configurable parameters*********/
/*software reserved for future use,such as boot part**/
/*change to 64M for some ufs has boot part already**/
//#define CONFIG_RESERVED_SPACE_SECTOR 	(16*1024*1024/512)
#define CONFIG_RESERVED_SPACE_SECTOR 	(4*16*1024*1024/512)

#define CONFIG_HEADER_OFFSET 0x16
#define CONFIG_LUN_OFFSET 0x1A

/* Config desc. offsets for UFS 2.0 - 3.0 spec */
#define CONFIG_HEADER_OFFSET_3_0 0x10
#define CONFIG_LUN_OFFSET_3_0 0x10

/*bLUEnable*/
#define B_LU_ENABLE_OFFSET 					0x0

/*bProvisioningType*/
#define  BPROVISIONINGTYPE_OFFSET 0xa
#define  BPROVISIONINGTYPE_TYPE_ERASE 0x3

/*dNumAllocUnits*/
#define D_NUM_ALLOC_UNITS_OFFSET			0x4

/*bdatareliability*/
#define B_DATA_RELIABILITY_OFFSET  0x8
#define B_DATA_RELIABILITY_ENABLED 0x1

/*bLogicalBlockSize*/
#define B_LOGICAL_BLOCKSIZE_OFFSET 			0x9

/*****geometry descriptor******/
/*qTotalRawDeviceCapacity*/
#define Q_TOTAL_RAW_DEVICE_CAPACITY_OFFSET  0x4
/*dSegmentSize*/
#define D_SEGMENT_SIZE_OFFSET 				0xd
/*bAllocationUnitSize*/
#define B_ALLOCATION_UNIT_SIZE_OFFSET 		0x11

/*****health descriptor******/
/**bPreEOLInfo*/
#define B_PREEOLINFO_OFFSET 				0x2
/*bDeviceLifeTimeEstA**/
#define B_DEVICE_LIFETIME_EST_A_OFFSET		0x3
/*bDeviceLifeTimeEstB**/
#define B_DEVICE_LIFETIME_EST_B_OFFSET		0x3

/*****unit descriptor******/
/*bBootLunID**/
#define B_BOOT_LUN_ID							0x4
/*qLogicalBlockCount**/
#define Q_LOGICAL_BLOCK_COUNT_OFFSET			0xb
/*qPhyMemResourceCount**/
#define Q_PHY_MEM_RESOURCE_COUNT_OFFSET			0x18


/**device descriptor**/
/*bUD0BaseOffset*/
#define B_UD0_BASE_OFFSET_OFFSET				0x1a
/**bUDConfigPLength**/
#define B_UD_CONFIGP_LENGTH_OFFSET				0x1b

//extern unsigned long long get_timer(unsigned long long  base);
extern void ufs_hda_ops_bind(struct udevice *dev);
extern void ufs_hda_ops_unbind(struct udevice *dev);

#define UFSHCD_ENABLE_INTRS	(UTP_TRANSFER_REQ_COMPL |\
				 UTP_TASK_REQ_COMPL |\
				 UFSHCD_ERROR_MASK)
/* maximum number of link-startup retries */
#define DME_LINKSTARTUP_RETRIES 3

/* maximum number of retries for a general UIC command  */
#define UFS_UIC_COMMAND_RETRIES 3

/* Query request retries */
#define QUERY_REQ_RETRIES 3
/* Query request timeout */
#if 0
#define QUERY_REQ_TIMEOUT 400
#else
//#define QUERY_REQ_TIMEOUT 1500 /* 1.5 seconds */
/*Because this timeout also use in data in upiu,so according to
 *bBackgroundOpsTermLati max value 2550ms,here use round to 3000ms
 */
#define QUERY_REQ_TIMEOUT 3000000 /* 1.5 seconds */
#endif

/* maximum timeout in ms for a general UIC command */
#define UFS_UIC_CMD_TIMEOUT	1000
/* NOP OUT retries waiting for NOP IN response */
#define NOP_OUT_RETRIES    10
/* Timeout after 30 msecs if NOP OUT hangs without response */
#define NOP_OUT_TIMEOUT    30 /* msecs */

/* Only use one Task Tag for all requests */
#define TASK_TAG	0

/* Expose the flag value from utp_upiu_query.value */
#define MASK_QUERY_UPIU_FLAG_LOC 0xFF

#define MAX_PRDT_ENTRY	262144

/* maximum bytes per request */
#define UFS_MAX_BYTES	(MAX_BUFF * 256 * 1024)

/*
 * Peer Initiated Boot Procedure
 * From:
 * JESD223D______Universal Flash Storage Host Controller Interface (UFSHCI), Version 3.0:7.1.1 Host Controller Initialization step 9
 * mipi_UniPro_specification_v1-8 Figure 107 Peer Initiated Boot Procedure
 */
#define SUPPORT_PEER_INITED_BOOT
#ifdef SUPPORT_PEER_INITED_BOOT
//#define PEER_INITED_BOOT_TEST
#endif


static inline bool ufshcd_is_hba_active(struct ufs_hba *hba);
static inline void ufshcd_hba_stop(struct ufs_hba *hba);
static int ufshcd_hba_enable(struct ufs_hba *hba);


/*
 * ufshcd_wait_for_register - wait for register value to change
 */
int ufshcd_wait_for_register(struct ufs_hba *hba, u32 reg, u32 mask,
				    u32 val, unsigned long long timeout_ms)
{
	int err = 0;
	unsigned long long start = get_timer(0);
	/* ignore bits that we don't intend to wait on */
	val = val & mask;

	while ((ufshcd_readl(hba, reg) & mask) != val) {
		if (get_timer(start) > timeout_ms) {
			if ((ufshcd_readl(hba, reg) & mask) != val) {
				err = -ETIMEDOUT;
				sunxi_ufs_trace_point(SUNXI_UFS_TOUT);
			}
			break;
		}
	}

	return err;
}

inline void ufshcd_print_uic_info(struct ufs_hba *hba)
{
	return ;
	dev_err(hba->dev, "uic c %x,uic a %x,%x,%x\n",\
			ufshcd_readl(hba, REG_UIC_COMMAND), \
			ufshcd_readl(hba, REG_UIC_COMMAND_ARG_1),\
			ufshcd_readl(hba, REG_UIC_COMMAND_ARG_2),\
			ufshcd_readl(hba, REG_UIC_COMMAND_ARG_3));

	dev_err(hba->dev, "e %x,%x,%x,%x,%x\n",\
			ufshcd_readl(hba, REG_UIC_ERROR_CODE_PHY_ADAPTER_LAYER),\
			ufshcd_readl(hba, REG_UIC_ERROR_CODE_DATA_LINK_LAYER),\
			ufshcd_readl(hba, REG_UIC_ERROR_CODE_NETWORK_LAYER),\
			ufshcd_readl(hba, REG_UIC_ERROR_CODE_TRANSPORT_LAYER),\
			ufshcd_readl(hba, REG_UIC_ERROR_CODE_DME));
}

inline void ufshcd_print_utp_info(struct ufs_hba *hba)
{
	dev_err(hba->dev, "utp s %x\n",\
			ufshcd_readl(hba, REG_CONTROLLER_STATUS));
}


inline void ufshcd_print_int_info(struct ufs_hba *hba, u32 int_status)
{
	dev_err(hba->dev, "int s %x,%x\n",\
			ufshcd_readl(hba, REG_INTERRUPT_STATUS), int_status);
}


/**
 * ufshcd_init_pwr_info - setting the POR (power on reset)
 * values in hba power info
 */
static void ufshcd_init_pwr_info(struct ufs_hba *hba)
{
	hba->pwr_info.gear_rx = UFS_PWM_G1;
	hba->pwr_info.gear_tx = UFS_PWM_G1;
	hba->pwr_info.lane_rx = 1;
	hba->pwr_info.lane_tx = 1;
	hba->pwr_info.pwr_rx = SLOWAUTO_MODE;
	hba->pwr_info.pwr_tx = SLOWAUTO_MODE;
	hba->pwr_info.hs_rate = 0;
}

#ifdef PWR_MODE_CHANGE
/**
 * ufshcd_print_pwr_info - print power params as saved in hba
 * power info
 */
static void ufshcd_print_pwr_info(struct ufs_hba *hba)
{
	static const char * const names[] = {
		"INVALID MODE",
		"FAST MODE",
		"SLOW_MODE",
		"INVALID MODE",
		"FASTAUTO_MODE",
		"SLOWAUTO_MODE",
		"INVALID MODE",
	};

	dev_info(hba->dev, "[RX, TX]: gear=[%d, %d], lane[%d, %d], pwr[%s, %s], rate = %d\n",
		hba->pwr_info.gear_rx, hba->pwr_info.gear_tx,
		hba->pwr_info.lane_rx, hba->pwr_info.lane_tx,
		names[hba->pwr_info.pwr_rx],
		names[hba->pwr_info.pwr_tx],
		hba->pwr_info.hs_rate);
}
#endif

/**
 * ufshcd_ready_for_uic_cmd - Check if controller is ready
 *                            to accept UIC commands
 */
static inline bool ufshcd_ready_for_uic_cmd(struct ufs_hba *hba)
{
	if (ufshcd_readl(hba, REG_CONTROLLER_STATUS) & UIC_COMMAND_READY)
		return true;
	else
		return false;
}

/**
 * ufshcd_get_uic_cmd_result - Get the UIC command result
 */
static inline int ufshcd_get_uic_cmd_result(struct ufs_hba *hba)
{
	return ufshcd_readl(hba, REG_UIC_COMMAND_ARG_2) &
	       MASK_UIC_COMMAND_RESULT;
}

/**
 * ufshcd_get_dme_attr_val - Get the value of attribute returned by UIC command
 */
static inline u32 ufshcd_get_dme_attr_val(struct ufs_hba *hba)
{
	return ufshcd_readl(hba, REG_UIC_COMMAND_ARG_3);
}

/**
 * ufshcd_is_device_present - Check if any device connected to
 *			      the host controller
 */
static inline bool ufshcd_is_device_present(struct ufs_hba *hba)
{
	return (ufshcd_readl(hba, REG_CONTROLLER_STATUS) &
						DEVICE_PRESENT) ? true : false;
}

/**
 * ufshcd_is_device_linkstartup - Check if Link start-up process
 * has been initiated by the remote end of the Link
 */
/*
static inline bool ufshcd_is_device_linkstartup(struct ufs_hba *hba)
{
	u32 ulss = ufshcd_readl(hba, REG_INTERRUPT_STATUS) &
						UIC_LINK_STARTUP;
	return ulss ? true : false;
}
*/

/**
 * ufshcd_clear_device_linkstartup - clear Link start-up status(ULSS)
 *
 */
static inline void ufshcd_clear_device_linkstartup(struct ufs_hba *hba)
{
	ufshcd_writel(hba, UIC_LINK_STARTUP, REG_INTERRUPT_STATUS);
}

/**
 * ufshcd_hba_wait_device_linkstartup - wait device link startup
 */
static inline int ufshcd_wait_device_linkstartup(struct ufs_hba *hba)
{
	int err;

	err = ufshcd_wait_for_register(hba, REG_INTERRUPT_STATUS,
				       UIC_LINK_STARTUP, UIC_LINK_STARTUP,
				       100);
	//if (err)
	//	dev_err(hba->dev, "wait device link startup failed\n");
	return err;
}


/**
 * ufshcd_send_uic_cmd - UFS Interconnect layer command API
 *
 */
static int ufshcd_send_uic_cmd(struct ufs_hba *hba, struct uic_command *uic_cmd)
{
	unsigned long start = 0;
	u32 intr_status;
	u32 enabled_intr_status;

	if (!ufshcd_ready_for_uic_cmd(hba)) {
		dev_err(hba->dev,
			"Controller not ready to accept UIC commands\n");
		return -EIO;
	}

	debug("sending uic command:%d\n", uic_cmd->command);

	/* Write Args */
	ufshcd_writel(hba, uic_cmd->argument1, REG_UIC_COMMAND_ARG_1);
	ufshcd_writel(hba, uic_cmd->argument2, REG_UIC_COMMAND_ARG_2);
	ufshcd_writel(hba, uic_cmd->argument3, REG_UIC_COMMAND_ARG_3);

	/* Write UIC Cmd */
	ufshcd_writel(hba, uic_cmd->command & COMMAND_OPCODE_MASK,
		      REG_UIC_COMMAND);

	start = get_timer(0);
	do {
		intr_status = ufshcd_readl(hba, REG_INTERRUPT_STATUS);
		enabled_intr_status = intr_status & hba->intr_mask;
		ufshcd_writel(hba, intr_status, REG_INTERRUPT_STATUS);

		if (get_timer(start) > UFS_UIC_CMD_TIMEOUT) {
			dev_err(hba->dev,
				"Timedout waiting for UIC response\n");
			ufshcd_print_uic_info(hba);
			ufshcd_print_int_info(hba, intr_status);
			sunxi_ufs_trace_point(SUNXI_UFS_TOUT);
			return -ETIMEDOUT;
		}

		if (enabled_intr_status & UFSHCD_ERROR_MASK) {
			ufshcd_print_uic_info(hba);
			ufshcd_print_int_info(hba, intr_status);
			sunxi_ufs_trace_is_point(enabled_intr_status);
			dev_err(hba->dev, "Error in status:%08x\n",
				enabled_intr_status);
			return -1;
		}
	} while (!(enabled_intr_status & UFSHCD_UIC_MASK));

	uic_cmd->argument2 = ufshcd_get_uic_cmd_result(hba);
	uic_cmd->argument3 = ufshcd_get_dme_attr_val(hba);

	if (uic_cmd->argument2) {
		//ufshcd_print_uic_info(hba);
		//ufshcd_print_int_info(hba, intr_status);
	}

	debug("Sent successfully\n");

	return 0;
}

/**
 * ufshcd_dme_set_attr - UIC command for DME_SET, DME_PEER_SET
 *
 */
int ufshcd_dme_set_attr(struct ufs_hba *hba, u32 attr_sel, u8 attr_set,
			u32 mib_val, u8 peer)
{
	struct uic_command uic_cmd = {0};
	static const char *const action[] = {
		"dme-set",
		"dme-peer-set"
	};
	const char *set = action[!!peer];
	int ret;
	int retries = UFS_UIC_COMMAND_RETRIES;

	memset(&uic_cmd, 0, sizeof(struct uic_command));

	uic_cmd.command = peer ?
		UIC_CMD_DME_PEER_SET : UIC_CMD_DME_SET;
	uic_cmd.argument1 = attr_sel;
	uic_cmd.argument2 = UIC_ARG_ATTR_TYPE(attr_set);
	uic_cmd.argument3 = mib_val;

	do {
		/* for peer attributes we retry upon failure */
		ret = ufshcd_send_uic_cmd(hba, &uic_cmd);
		if (ret)
			dev_dbg(hba->dev, "%s: attr-id 0x%x val 0x%x error code %d\n",
				set, UIC_GET_ATTR_ID(attr_sel), mib_val, ret);
	} while (ret && peer && --retries);

	if (ret)
		dev_err(hba->dev, "%s: attr-id 0x%x val 0x%x failed %d retries\n",
			set, UIC_GET_ATTR_ID(attr_sel), mib_val,
			UFS_UIC_COMMAND_RETRIES - retries);

	return ret;
}

/**
 * ufshcd_dme_get_attr - UIC command for DME_GET, DME_PEER_GET
 *
 */
int ufshcd_dme_get_attr(struct ufs_hba *hba, u32 attr_sel,
			u32 *mib_val, u8 peer)
{
	struct uic_command uic_cmd = {0};
	static const char *const action[] = {
		"dme-get",
		"dme-peer-get"
	};
	const char *get = action[!!peer];
	int ret;
	int retries = UFS_UIC_COMMAND_RETRIES;

	memset(&uic_cmd, 0, sizeof(struct uic_command));

	uic_cmd.command = peer ?
		UIC_CMD_DME_PEER_GET : UIC_CMD_DME_GET;
	uic_cmd.argument1 = attr_sel;

	do {
		/* for peer attributes we retry upon failure */
		ret = ufshcd_send_uic_cmd(hba, &uic_cmd);
		if (ret)
			dev_dbg(hba->dev, "%s: attr-id 0x%x error code %d\n",
				get, UIC_GET_ATTR_ID(attr_sel), ret);
	} while (ret && peer && --retries);

	if (ret)
		dev_err(hba->dev, "%s: attr-id 0x%x failed %d retries\n",
			get, UIC_GET_ATTR_ID(attr_sel),
			UFS_UIC_COMMAND_RETRIES - retries);

	if (mib_val && !ret)
		*mib_val = uic_cmd.argument3;

	return ret;
}

static int ufshcd_disable_tx_lcc(struct ufs_hba *hba, bool peer)
{
	u32 tx_lanes, i, err = 0;

	if (!peer)
		ufshcd_dme_get(hba, UIC_ARG_MIB(PA_CONNECTEDTXDATALANES),
			       &tx_lanes);
	else
		ufshcd_dme_peer_get(hba, UIC_ARG_MIB(PA_CONNECTEDTXDATALANES),
				    &tx_lanes);
	for (i = 0; i < tx_lanes; i++) {
		if (!peer)
			err = ufshcd_dme_set(hba,
					     UIC_ARG_MIB_SEL(TX_LCC_ENABLE,
					     UIC_ARG_MPHY_TX_GEN_SEL_INDEX(i)),
					     0);
		else
			err = ufshcd_dme_peer_set(hba,
					UIC_ARG_MIB_SEL(TX_LCC_ENABLE,
					UIC_ARG_MPHY_TX_GEN_SEL_INDEX(i)),
					0);
		if (err) {
			dev_err(hba->dev, "%s: TX LCC Disable failed, peer = %d, lane = %d, err = %d",
				__func__, peer, i, err);
			break;
		}
	}

	return err;
}

static inline int ufshcd_disable_device_tx_lcc(struct ufs_hba *hba)
{
	return ufshcd_disable_tx_lcc(hba, true);
}

/**
 * ufshcd_dme_link_startup - Notify Unipro to perform link startup
 *
 */
static int ufshcd_dme_link_startup(struct ufs_hba *hba)
{
	struct uic_command uic_cmd = {0};
	int ret;

	memset(&uic_cmd, 0, sizeof(struct uic_command));

	uic_cmd.command = UIC_CMD_DME_LINK_STARTUP;

	ret = ufshcd_send_uic_cmd(hba, &uic_cmd);
	if (ret)
		dev_err(hba->dev,
			"dme-link-startup: error code %d\n", ret);
	return ret;
}

int ufshcd_dme_configure_adapt(struct ufs_hba *hba,
			       int agreed_gear,
			       int adapt_val)
{
	int ret;

	if (agreed_gear < UFS_HS_G4)
		adapt_val = PA_NO_ADAPT;

	dev_dbg(NULL, "set %d adapt\n", adapt_val);
	ret = ufshcd_dme_set(hba,
				UIC_ARG_MIB(PA_TXHSADAPTTYPE),
				adapt_val);
	return ret;
}

/**
 * ufshcd_disable_intr_aggr - Disables interrupt aggregation.
 *
 */
static inline void ufshcd_disable_intr_aggr(struct ufs_hba *hba)
{
	ufshcd_writel(hba, 0, REG_UTP_TRANSFER_REQ_INT_AGG_CONTROL);
}

/**
 * ufshcd_get_lists_status - Check UCRDY, UTRLRDY and UTMRLRDY
 */
static inline int ufshcd_get_lists_status(u32 reg)
{
	return !((reg & UFSHCD_STATUS_READY) == UFSHCD_STATUS_READY);
}

/**
 * ufshcd_enable_run_stop_reg - Enable run-stop registers,
 *			When run-stop registers are set to 1, it indicates the
 *			host controller that it can process the requests
 */
static void ufshcd_enable_run_stop_reg(struct ufs_hba *hba)
{
	ufshcd_writel(hba, UTP_TASK_REQ_LIST_RUN_STOP_BIT,
		      REG_UTP_TASK_REQ_LIST_RUN_STOP);
	ufshcd_writel(hba, UTP_TRANSFER_REQ_LIST_RUN_STOP_BIT,
		      REG_UTP_TRANSFER_REQ_LIST_RUN_STOP);
}

/**
 * ufshcd_enable_intr - enable interrupts
 */
static void ufshcd_enable_intr(struct ufs_hba *hba, u32 intrs)
{
	u32 set = ufshcd_readl(hba, REG_INTERRUPT_ENABLE);
	u32 rw;

	if (hba->version == UFSHCI_VERSION_10) {
		rw = set & INTERRUPT_MASK_RW_VER_10;
		set = rw | ((set ^ intrs) & intrs);
	} else {
		set |= intrs;
	}

	ufshcd_writel(hba, set, REG_INTERRUPT_ENABLE);

	hba->intr_mask = set;
}

/**
 * ufshcd_make_hba_operational - Make UFS controller operational
 *
 * To bring UFS host controller to operational state,
 * 1. Enable required interrupts
 * 2. Configure interrupt aggregation
 * 3. Program UTRL and UTMRL base address
 * 4. Configure run-stop-registers
 *
 */
static int ufshcd_make_hba_operational(struct ufs_hba *hba)
{
	int err = 0;
	u32 reg;

	/* Enable required interrupts */
	ufshcd_enable_intr(hba, UFSHCD_ENABLE_INTRS);

	/* Disable interrupt aggregation */
	ufshcd_disable_intr_aggr(hba);

	/* Configure UTRL and UTMRL base address registers */
	ufshcd_writel(hba, lower_32_bits((dma_addr_t)hba->utrdl),
		      REG_UTP_TRANSFER_REQ_LIST_BASE_L);
	ufshcd_writel(hba, upper_32_bits((dma_addr_t)hba->utrdl),
		      REG_UTP_TRANSFER_REQ_LIST_BASE_H);
	ufshcd_writel(hba, lower_32_bits((dma_addr_t)hba->utmrdl),
		      REG_UTP_TASK_REQ_LIST_BASE_L);
	ufshcd_writel(hba, upper_32_bits((dma_addr_t)hba->utmrdl),
		      REG_UTP_TASK_REQ_LIST_BASE_H);

	/*
	 * UCRDY, UTMRLDY and UTRLRDY bits must be 1
	 */
	reg = ufshcd_readl(hba, REG_CONTROLLER_STATUS);
	if (!(ufshcd_get_lists_status(reg))) {
		ufshcd_enable_run_stop_reg(hba);
	} else {
		dev_err(hba->dev,
			"Host controller not ready to process requests");
		err = -EIO;
		goto out;
	}

out:
	return err;
}

/**
 * ufshcd_link_startup - Initialize unipro link startup
 */
static int ufshcd_link_startup(struct ufs_hba *hba)
{
	int ret = 0;
	int retries = DME_LINKSTARTUP_RETRIES;
	bool link_startup_again = false;
	int skip_no = 0;
#ifdef PEER_INITED_BOOT_TEST
	int skip_lk = 1;
#endif


link_startup:
	do {

		if (!skip_no) {
			ret = ufshcd_ops_link_startup_notify(hba, PRE_CHANGE);
			if (ret) {
				dev_err(hba->dev, "Phy init failed\n");
				sunxi_ufs_trace_point(SUNXI_UFS_PHYINIT_ERR);
				ufshcd_hba_enable(hba);
				continue;
			}
		} else
			 skip_no = 0;

#ifdef PEER_INITED_BOOT_TEST
	if (!skip_lk) {
		ret = ufshcd_dme_link_startup(hba);
	} else {
		skip_lk = 0;
		dev_info(hba->dev, "skip first host link start\n");
	}
#else
		ret = ufshcd_dme_link_startup(hba);
#endif
		/* check if device is detected by inter-connect layer */
		if (!ret && !ufshcd_is_device_present(hba)) {
			//dev_err(hba->dev, "%s: Device not present\n", __func__);
			//dev_err(hba->dev, "Device not present\n");
			ret = -ENXIO;
#ifdef SUPPORT_PEER_INITED_BOOT
			if (ufshcd_wait_device_linkstartup(hba)) {
				//dev_info(hba->dev, "peer link startup timeout\n");
				if (retries == 1)
					ufshcd_ops_device_reset(hba);
				/*
				 *Beginning from UniPro Version 1.41, DME software driver designers
				 *should answer a DME_LINKSTARTUP.cnf_L(FAILURE) with the sequence
				 *DME_RESET.req followed by  a DME_ENABLE.req, to ensure the M-PHY
				 *is set back to Hibernate.
				 * */
				ufshcd_hba_enable(hba);
			} else {
				/* Although JESD223D______Universal Flash Storage Host Controller Interface (UFSHCI), Version 3.0
				 * 7.1.1 Host Controller Initialization does not say that user should clear ulss after ulss has
				 * been set to 1,we should clear it because ulss is a interrupt status bit,it should be clear
				 * commonly
				 * */
				ufshcd_clear_device_linkstartup(hba);
				if (retries == 1) {
					ufshcd_ops_device_reset(hba);
					ufshcd_hba_enable(hba);
				} else {
					skip_no = 1;
				}
				dev_info(hba->dev, "peer link startup\n");
			}
			continue;
#else
			goto out;
#endif
		}

		/*
		 * DME link lost indication is only received when link is up,
		 * but we can't be sure if the link is up until link startup
		 * succeeds. So reset the local Uni-Pro and try again.
		 */
		if (ret && ufshcd_hba_enable(hba)) {
			dev_err(hba->dev, "link start failed and reinit host failed\n");
			goto out;
		}
	} while (ret && retries--);

	if (ret) {
		/* failed to get the link up... retire */
		goto out;
	}

	if (link_startup_again) {
		link_startup_again = false;
		retries = DME_LINKSTARTUP_RETRIES;
		goto link_startup;
	}

	/* Mark that link is up in PWM-G1, 1-lane, SLOW-AUTO mode */
	ufshcd_init_pwr_info(hba);

	if (hba->quirks & UFSHCD_QUIRK_BROKEN_LCC) {
		ret = ufshcd_disable_device_tx_lcc(hba);
		if (ret)
			goto out;
	}

	/* Include any host controller configuration via UIC commands */
	ret = ufshcd_ops_link_startup_notify(hba, POST_CHANGE);
	if (ret) {
		sunxi_ufs_trace_point(SUNXI_UFS_UNIPRO_INIT_ERR);
		goto out;
	}

	ret = ufshcd_make_hba_operational(hba);
out:
	if (ret) {
		sunxi_ufs_trace_point(SUNXI_UFS_LINK_ERR);
		//dev_err(hba->dev, "link startup failed %d\n", ret);
	}
	return ret;
}

/**
 * ufshcd_hba_stop - Send controller to reset state
 */
static inline void ufshcd_hba_stop(struct ufs_hba *hba)
{
	int err;

	ufshcd_writel(hba, CONTROLLER_DISABLE,  REG_CONTROLLER_ENABLE);
	err = ufshcd_wait_for_register(hba, REG_CONTROLLER_ENABLE,
				       CONTROLLER_ENABLE, CONTROLLER_DISABLE,
				       10);
	if (err)
		//dev_err(hba->dev, "%s: Controller disable failed\n", __func__);
		dev_err(hba->dev, "Controller disable failed\n");
}





/**
 * ufshcd_is_hba_active - Get controller state
 */
static inline bool ufshcd_is_hba_active(struct ufs_hba *hba)
{
	return (ufshcd_readl(hba, REG_CONTROLLER_ENABLE) & CONTROLLER_ENABLE)
		? false : true;
}

/**
 * ufshcd_hba_start - Start controller initialization sequence
 */
static inline void ufshcd_hba_start(struct ufs_hba *hba)
{
	ufshcd_writel(hba, CONTROLLER_ENABLE, REG_CONTROLLER_ENABLE);
}

/**
 * ufshcd_hba_enable - initialize the controller
 */
static int ufshcd_hba_enable(struct ufs_hba *hba)
{
	int retry;

	if (!ufshcd_is_hba_active(hba))
		/* change controller state to "reset state" */
		ufshcd_hba_stop(hba);

	ufshcd_ops_hce_enable_notify(hba, PRE_CHANGE);

	/* start controller initialization sequence */
	ufshcd_hba_start(hba);

	/*
	 * To initialize a UFS host controller HCE bit must be set to 1.
	 * During initialization the HCE bit value changes from 1->0->1.
	 * When the host controller completes initialization sequence
	 * it sets the value of HCE bit to 1. The same HCE bit is read back
	 * to check if the controller has completed initialization sequence.
	 * So without this delay the value HCE = 1, set in the previous
	 * instruction might be read back.
	 * This delay can be changed based on the controller.
	 */
	/*For our contrller,from sim wave,we make sure that we don't need wait,so skip
	 * this wait
	 * */
	//dev_dbg(NULL, "%s,%d\n", __FUNCTION__, __LINE__);
#if 0
#if (BROM_MODE == BROM_MODE_ALL_SIMU)
	_udelay(2);
#else
	_mdelay(1);
#endif
#endif

	/* wait for the host controller to complete initialization */
	retry = 50000;
	while (ufshcd_is_hba_active(hba)) {
		if (retry) {
	//		dev_dbg(NULL, "%s,%d\n", __FUNCTION__, __LINE__);
			retry--;
		} else {
			dev_err(hba->dev, "Controller enable failed\n");
			sunxi_ufs_trace_point(SUNXI_UFS_HOST_EN_ERR);
			return -EIO;
		}
		udelay(1);
	}

	/* enable UIC related interrupts */
	ufshcd_enable_intr(hba, UFSHCD_UIC_MASK);

	ufshcd_ops_hce_enable_notify(hba, POST_CHANGE);

	return 0;
}

/**
 * ufshcd_host_memory_configure - configure local reference block with
 *				memory offsets
 */
static void ufshcd_host_memory_configure(struct ufs_hba *hba)
{
	struct utp_transfer_req_desc *utrdlp;
	dma_addr_t cmd_desc_dma_addr;
	u16 response_offset;
	u16 prdt_offset;

	utrdlp = hba->utrdl;
	cmd_desc_dma_addr = (dma_addr_t)hba->ucdl;

	utrdlp->command_desc_base_addr_lo =
				cpu_to_le32(lower_32_bits(cmd_desc_dma_addr));
	utrdlp->command_desc_base_addr_hi =
				cpu_to_le32(upper_32_bits(cmd_desc_dma_addr));

	response_offset = offsetof(struct utp_transfer_cmd_desc, response_upiu);
	prdt_offset = offsetof(struct utp_transfer_cmd_desc, prd_table);

	utrdlp->response_upiu_offset = cpu_to_le16(response_offset >> 2);
	utrdlp->prd_table_offset = cpu_to_le16(prdt_offset >> 2);
	utrdlp->response_upiu_length = cpu_to_le16(ALIGNED_UPIU_SIZE >> 2);

	hba->ucd_req_ptr = (struct utp_upiu_req *)hba->ucdl;
	hba->ucd_rsp_ptr =
		(struct utp_upiu_rsp *)&hba->ucdl->response_upiu;
	hba->ucd_prdt_ptr =
		(struct ufshcd_sg_entry *)&hba->ucdl->prd_table;
}

/**
 * ufshcd_memory_alloc - allocate memory for host memory space data structures
 */
static int ufshcd_memory_alloc(struct ufs_hba *hba)
{
	/* Allocate one Transfer Request Descriptor
	 * Should be aligned to 1k boundary.
	 */
//	hba->utrdl = memalign(1024, sizeof(struct utp_transfer_req_desc));
	hba->utrdl = (void *)ALIGN_1K_UTRDL_ADDR;
	if (!hba->utrdl) {
		dev_err(hba->dev, "Transfer Descriptor memory allocation failed\n");
		return -ENOMEM;
	}

	/* Allocate one Command Descriptor
	 * Should be aligned to 1k boundary.
	 */
	//hba->ucdl = memalign(1024, sizeof(struct utp_transfer_cmd_desc));
	hba->ucdl = (void *)ALIGN_128_UCDL_ADDR;
	if (!hba->ucdl) {
		dev_err(hba->dev, "Command descriptor memory allocation failed\n");
		return -ENOMEM;
	}

	return 0;
}

/**
 * ufshcd_get_intr_mask - Get the interrupt bit mask
 */
static inline u32 ufshcd_get_intr_mask(struct ufs_hba *hba)
{
	u32 intr_mask = 0;

	switch (hba->version) {
	case UFSHCI_VERSION_10:
		intr_mask = INTERRUPT_MASK_ALL_VER_10;
		break;
	case UFSHCI_VERSION_11:
	case UFSHCI_VERSION_20:
		intr_mask = INTERRUPT_MASK_ALL_VER_11;
		break;
	case UFSHCI_VERSION_21:
	default:
		intr_mask = INTERRUPT_MASK_ALL_VER_21;
		break;
	}

	return intr_mask;
}

/**
 * ufshcd_get_ufs_version - Get the UFS version supported by the HBA
 */
static inline u32 ufshcd_get_ufs_version(struct ufs_hba *hba)
{
	return ufshcd_readl(hba, REG_UFS_VERSION);
}

/**
 * ufshcd_get_upmcrs - Get the power mode change request status
 */
static inline u8 ufshcd_get_upmcrs(struct ufs_hba *hba)
{
	return (ufshcd_readl(hba, REG_CONTROLLER_STATUS) >> 8) & 0x7;
}

/**
 * ufshcd_prepare_req_desc_hdr() - Fills the requests header
 * descriptor according to request
 */
static void ufshcd_prepare_req_desc_hdr(struct utp_transfer_req_desc *req_desc,
					u32 *upiu_flags,
					enum dma_data_direction cmd_dir)
{
	u32 data_direction;
	u32 dword_0;

	if (cmd_dir == DMA_FROM_DEVICE) {
		data_direction = UTP_DEVICE_TO_HOST;
		*upiu_flags = UPIU_CMD_FLAGS_READ;
	} else if (cmd_dir == DMA_TO_DEVICE) {
		data_direction = UTP_HOST_TO_DEVICE;
		*upiu_flags = UPIU_CMD_FLAGS_WRITE;
	} else {
		data_direction = UTP_NO_DATA_TRANSFER;
		*upiu_flags = UPIU_CMD_FLAGS_NONE;
	}

	//printf("upiu flags %x\n", *upiu_flags);

	dword_0 = data_direction | (0x1 << UPIU_COMMAND_TYPE_OFFSET);

	/* Enable Interrupt for command */
	dword_0 |= UTP_REQ_DESC_INT_CMD;

	/* Transfer request descriptor header fields */
	req_desc->header.dword_0 = cpu_to_le32(dword_0);
	/* dword_1 is reserved, hence it is set to 0 */
	req_desc->header.dword_1 = 0;
	/*
	 * assigning invalid value for command status. Controller
	 * updates OCS on command completion, with the command
	 * status
	 */
	req_desc->header.dword_2 =
		cpu_to_le32(OCS_INVALID_COMMAND_STATUS);
	/* dword_3 is reserved, hence it is set to 0 */
	req_desc->header.dword_3 = 0;

	req_desc->prd_table_length = 0;
}

static void ufshcd_prepare_utp_query_req_upiu(struct ufs_hba *hba,
					      u32 upiu_flags)
{
	struct utp_upiu_req *ucd_req_ptr = hba->ucd_req_ptr;
	struct ufs_query *query = &hba->dev_cmd.query;
	u16 len = be16_to_cpu(query->request.upiu_req.length);

	/* Query request header */
	ucd_req_ptr->header.dword_0 =
				UPIU_HEADER_DWORD(UPIU_TRANSACTION_QUERY_REQ,
						  upiu_flags, 0, TASK_TAG);
	ucd_req_ptr->header.dword_1 =
				UPIU_HEADER_DWORD(0, query->request.query_func,
						  0, 0);

	/* Data segment length only need for WRITE_DESC */
	if (query->request.upiu_req.opcode == UPIU_QUERY_OPCODE_WRITE_DESC)
		ucd_req_ptr->header.dword_2 =
				UPIU_HEADER_DWORD(0, 0, (len >> 8), (u8)len);
	else
		ucd_req_ptr->header.dword_2 = 0;

	/* Copy the Query Request buffer as is */
	memcpy(&ucd_req_ptr->qr, &query->request.upiu_req, QUERY_OSF_SIZE);

	/* Copy the Descriptor */
	if (query->request.upiu_req.opcode == UPIU_QUERY_OPCODE_WRITE_DESC)
		memcpy(ucd_req_ptr + 1, query->descriptor, len);

	memset(hba->ucd_rsp_ptr, 0, sizeof(struct utp_upiu_rsp));
}

static inline void ufshcd_prepare_utp_nop_upiu(struct ufs_hba *hba)
{
	struct utp_upiu_req *ucd_req_ptr = hba->ucd_req_ptr;

	memset(ucd_req_ptr, 0, sizeof(struct utp_upiu_req));

	/* command descriptor fields */
	ucd_req_ptr->header.dword_0 =
			UPIU_HEADER_DWORD(UPIU_TRANSACTION_NOP_OUT, 0, 0, 0x1f);
	/* clear rest of the fields of basic header */
	ucd_req_ptr->header.dword_1 = 0;
	ucd_req_ptr->header.dword_2 = 0;

	memset(hba->ucd_rsp_ptr, 0, sizeof(struct utp_upiu_rsp));
}

/**
 * ufshcd_comp_devman_upiu - UFS Protocol Information Unit(UPIU)
 *			     for Device Management Purposes
 */
static int ufshcd_comp_devman_upiu(struct ufs_hba *hba,
				   enum dev_cmd_type cmd_type)
{
	u32 upiu_flags;
	int ret = 0;
	struct utp_transfer_req_desc *req_desc = hba->utrdl;

	hba->dev_cmd.type = cmd_type;

	ufshcd_prepare_req_desc_hdr(req_desc, &upiu_flags, DMA_NONE);
	switch (cmd_type) {
	case DEV_CMD_TYPE_QUERY:
		ufshcd_prepare_utp_query_req_upiu(hba, upiu_flags);
		break;
	case DEV_CMD_TYPE_NOP:
		ufshcd_prepare_utp_nop_upiu(hba);
		break;
	default:
		ret = -EINVAL;
	}

	return ret;
}

static int ufshcd_send_command(struct ufs_hba *hba, unsigned int task_tag)
{
	unsigned long start;
	u32 intr_status;
	u32 enabled_intr_status;

	flush_dcache_all();
	ufshcd_writel(hba, 1 << task_tag, REG_UTP_TRANSFER_REQ_DOOR_BELL);

	start = get_timer(0);
	do {
		intr_status = ufshcd_readl(hba, REG_INTERRUPT_STATUS);
		enabled_intr_status = intr_status & hba->intr_mask;
		ufshcd_writel(hba, intr_status, REG_INTERRUPT_STATUS);

		if (get_timer(start) > QUERY_REQ_TIMEOUT) {
			dev_err(hba->dev,
				"Timedout waiting for UTP response\n");
			ufshcd_print_uic_info(hba);
			ufshcd_print_utp_info(hba);
			ufshcd_print_int_info(hba, intr_status);
			sunxi_ufs_trace_point(SUNXI_UFS_TOUT);
			return -ETIMEDOUT;
		}

		if (enabled_intr_status & UFSHCD_ERROR_MASK) {
			ufshcd_print_uic_info(hba);
			ufshcd_print_utp_info(hba);
			ufshcd_print_int_info(hba, intr_status);
			dev_err(hba->dev, "Error in status:%08x\n",
				enabled_intr_status);
			sunxi_ufs_trace_is_point(enabled_intr_status);
			return -1;
		}
	} while (!(enabled_intr_status & UTP_TRANSFER_REQ_COMPL));

	invalidate_dcache_all();
	return 0;
}

/**
 * ufshcd_get_req_rsp - returns the TR response transaction type
 */
static inline int ufshcd_get_req_rsp(struct utp_upiu_rsp *ucd_rsp_ptr)
{
	return be32_to_cpu(ucd_rsp_ptr->header.dword_0) >> 24;
}

/**
 * ufshcd_get_tr_ocs - Get the UTRD Overall Command Status
 *
 */
static inline int ufshcd_get_tr_ocs(struct ufs_hba *hba)
{
	return le32_to_cpu(hba->utrdl->header.dword_2) & MASK_OCS;
}

static inline int ufshcd_get_rsp_upiu_result(struct utp_upiu_rsp *ucd_rsp_ptr)
{
	return be32_to_cpu(ucd_rsp_ptr->header.dword_1) & MASK_RSP_UPIU_RESULT;
}

static inline int ufshcd_get_rsp_upiu_seg_len(struct utp_upiu_rsp *ucd_rsp_ptr)
{
	return be32_to_cpu(ucd_rsp_ptr->header.dword_2) & MASK_RSP_UPIU_DATA_SEG_LEN;
}

static int ufshcd_check_query_response(struct ufs_hba *hba)
{
	struct ufs_query_res *query_res = &hba->dev_cmd.query.response;

	/* Get the UPIU response */
	query_res->response = ufshcd_get_rsp_upiu_result(hba->ucd_rsp_ptr) >>
				UPIU_RSP_CODE_OFFSET;
	return query_res->response;
}

/**
 * ufshcd_copy_query_response() - Copy the Query Response and the data
 * descriptor
 */
static int ufshcd_copy_query_response(struct ufs_hba *hba)
{
	struct ufs_query_res *query_res = &hba->dev_cmd.query.response;

	memcpy(&query_res->upiu_res, &hba->ucd_rsp_ptr->qr, QUERY_OSF_SIZE);

	/* Get the descriptor */
	if (hba->dev_cmd.query.descriptor &&
	    hba->ucd_rsp_ptr->qr.opcode == UPIU_QUERY_OPCODE_READ_DESC) {
		u8 *descp = (u8 *)hba->ucd_rsp_ptr +
				GENERAL_UPIU_REQUEST_SIZE;
		u16 resp_len;
		u16 buf_len;

		/* data segment length */
		resp_len = be32_to_cpu(hba->ucd_rsp_ptr->header.dword_2) &
						MASK_QUERY_DATA_SEG_LEN;
		buf_len =
			be16_to_cpu(hba->dev_cmd.query.request.upiu_req.length);
		if (buf_len >= resp_len) {
			memcpy(hba->dev_cmd.query.descriptor, descp, resp_len);
		} else {
/*
			dev_warn(hba->dev,
				 "%s: Response size is bigger than buffer",
				 __func__);
*/
			dev_warn(hba->dev,
				 "Response size is bigger than buffer");
			return -EINVAL;
		}
	}

	return 0;
}

/**
 * ufshcd_exec_dev_cmd - API for sending device management requests
 */
static int ufshcd_exec_dev_cmd(struct ufs_hba *hba, enum dev_cmd_type cmd_type,
			       int timeout)
{
	int err;
	int resp;

	err = ufshcd_comp_devman_upiu(hba, cmd_type);
	if (err)
		return err;

	err = ufshcd_send_command(hba, TASK_TAG);
	if (err)
		return err;

	err = ufshcd_get_tr_ocs(hba);
	if (err) {
		dev_err(hba->dev, "Error in OCS:%d\n", err);
		sunxi_ufs_trace_point(SUNXI_UFS_OCS_ERR);
		return -EINVAL;
	}

	resp = ufshcd_get_req_rsp(hba->ucd_rsp_ptr);
	switch (resp) {
	case UPIU_TRANSACTION_NOP_IN:
		break;
	case UPIU_TRANSACTION_QUERY_RSP:
		err = ufshcd_check_query_response(hba);
		if (!err)
			err = ufshcd_copy_query_response(hba);
		break;
	case UPIU_TRANSACTION_REJECT_UPIU:
		/* TODO: handle Reject UPIU Response */
		err = -EPERM;
/*
		dev_err(hba->dev, "%s: Reject UPIU not fully implemented\n",
			__func__);
*/
		dev_err(hba->dev, "Reject UPIU not fully implemented\n");
		break;
	default:
		err = -EINVAL;
/*
		dev_err(hba->dev, "%s: Invalid device management cmd response: %x\n",
			__func__, resp);
*/
		dev_err(hba->dev, "Invalid device management cmd response: %x\n", resp);
	}

	return err;
}

/**
 * ufshcd_init_query() - init the query response and request parameters
 */
static inline void ufshcd_init_query(struct ufs_hba *hba,
				     struct ufs_query_req **request,
				     struct ufs_query_res **response,
				     enum query_opcode opcode,
				     u8 idn, u8 index, u8 selector)
{
	*request = &hba->dev_cmd.query.request;
	*response = &hba->dev_cmd.query.response;
	memset(*request, 0, sizeof(struct ufs_query_req));
	memset(*response, 0, sizeof(struct ufs_query_res));
	(*request)->upiu_req.opcode = opcode;
	(*request)->upiu_req.idn = idn;
	(*request)->upiu_req.index = index;
	(*request)->upiu_req.selector = selector;
}

/**
 * ufshcd_query_flag() - API function for sending flag query requests
 */
int ufshcd_query_flag(struct ufs_hba *hba, enum query_opcode opcode,
		      enum flag_idn idn, bool *flag_res)
{
	struct ufs_query_req *request = NULL;
	struct ufs_query_res *response = NULL;
	int err, index = 0, selector = 0;
	int timeout = QUERY_REQ_TIMEOUT;

	ufshcd_init_query(hba, &request, &response, opcode, idn, index,
			  selector);

	switch (opcode) {
	case UPIU_QUERY_OPCODE_SET_FLAG:
	case UPIU_QUERY_OPCODE_CLEAR_FLAG:
	case UPIU_QUERY_OPCODE_TOGGLE_FLAG:
		request->query_func = UPIU_QUERY_FUNC_STANDARD_WRITE_REQUEST;
		break;
	case UPIU_QUERY_OPCODE_READ_FLAG:
		request->query_func = UPIU_QUERY_FUNC_STANDARD_READ_REQUEST;
		if (!flag_res) {
			/* No dummy reads */
/*
			dev_err(hba->dev, "%s: Invalid argument for read request\n",
				__func__);
*/
			dev_err(hba->dev, "Invalid argument for read request\n");
			err = -EINVAL;
			goto out;
		}
		break;
	default:
/*
		dev_err(hba->dev,
			"%s: Expected query flag opcode but got = %d\n",
			__func__, opcode);
*/
		dev_err(hba->dev,
			"Expected query flag opcode but got = %d\n", opcode);
		err = -EINVAL;
		goto out;
	}

	err = ufshcd_exec_dev_cmd(hba, DEV_CMD_TYPE_QUERY, timeout);

	if (err) {
/*
		dev_err(hba->dev,
			"%s: Sending flag query for idn %d failed, err = %d\n",
			__func__, idn, err);
*/
		dev_err(hba->dev,
			"Sending flag query for idn %d failed, err = %d\n", idn, err);

		goto out;
	}

	if (flag_res)
		*flag_res = ((be32_to_cpu(response->upiu_res.value) &
				MASK_QUERY_UPIU_FLAG_LOC) & 0x1) ? true:false;

out:
	return err;
}

static int ufshcd_query_flag_retry(struct ufs_hba *hba,
				   enum query_opcode opcode,
				   enum flag_idn idn, bool *flag_res)
{
	int ret;
	int retries;

	for (retries = 0; retries < QUERY_REQ_RETRIES; retries++) {
		ret = ufshcd_query_flag(hba, opcode, idn, flag_res);
		if (ret) {
			dev_dbg(hba->dev,
				"%s: failed with error %d, retries %d\n",
				__func__, ret, retries);
		} else
			break;
	}

	if (ret)
/*
		dev_err(hba->dev,
			"%s: query attribute, opcode %d, idn %d, failed with error %d after %d retires\n",
			__func__, opcode, idn, ret, retries);
*/
		dev_err(hba->dev,
			"query attribute, opcode %d, idn %d, failed with error %d after %d retires\n",
			opcode, idn, ret, retries);

	return ret;
}

/**
 * ufshcd_query_attr - API function for sending attribute requests
 * @hba: per-adapter instance
 * @opcode: attribute opcode
 * @idn: attribute idn to access
 * @index: index field
 * @selector: selector field
 * @attr_val: the attribute value after the query request completes
 *
 * Return: 0 for success, non-zero in case of failure.
*/
int ufshcd_query_attr(struct ufs_hba *hba, enum query_opcode opcode,
		      enum attr_idn idn, u8 index, u8 selector, u32 *attr_val)
{
	struct ufs_query_req *request = NULL;
	struct ufs_query_res *response = NULL;
	int err;

	BUG_ON(!hba);

	if (!attr_val) {
		dev_err(hba->dev, "%s: attribute value required for opcode 0x%x\n",
				__func__, opcode);
		return -EINVAL;
	}

	ufshcd_init_query(hba, &request, &response, opcode, idn, index,
			selector);

	switch (opcode) {
	case UPIU_QUERY_OPCODE_WRITE_ATTR:
		request->query_func = UPIU_QUERY_FUNC_STANDARD_WRITE_REQUEST;
		request->upiu_req.value = cpu_to_be32(*attr_val);
		break;
	case UPIU_QUERY_OPCODE_READ_ATTR:
		request->query_func = UPIU_QUERY_FUNC_STANDARD_READ_REQUEST;
		break;
	default:
		dev_err(hba->dev, "%s: Expected query attr opcode but got = 0x%.2x\n",
				__func__, opcode);
		err = -EINVAL;
		goto out_unlock;
	}

	err = ufshcd_exec_dev_cmd(hba, DEV_CMD_TYPE_QUERY, QUERY_REQ_TIMEOUT);

	if (err) {
		dev_err(hba->dev, "%s: opcode 0x%.2x for idn %d failed, index %d, err = %d\n",
				__func__, opcode, idn, index, err);
		goto out_unlock;
	}

	*attr_val = be32_to_cpu(response->upiu_res.value);

out_unlock:
	return err;
}


/**
 * ufshcd_query_attr_retry() - API function for sending query
 * attribute with retries
 * @hba: per-adapter instance
 * @opcode: attribute opcode
 * @idn: attribute idn to access
 * @index: index field
 * @selector: selector field
 * @attr_val: the attribute value after the query request
 * completes
 *
 * Return: 0 for success, non-zero in case of failure.
*/
int ufshcd_query_attr_retry(struct ufs_hba *hba,
	enum query_opcode opcode, enum attr_idn idn, u8 index, u8 selector,
	u32 *attr_val)
{
	int ret = 0;
	u32 retries;

	for (retries = QUERY_REQ_RETRIES; retries > 0; retries--) {
		ret = ufshcd_query_attr(hba, opcode, idn, index,
						selector, attr_val);
		if (ret)
			dev_dbg(hba->dev, "%s: failed with error %d, retries %d\n",
				__func__, ret, retries);
		else
			break;
	}

	if (ret)
		dev_err(hba->dev,
			"%s: query attribute, idn %d, failed with error %d after %d retries\n",
			__func__, idn, ret, QUERY_REQ_RETRIES);
	return ret;
}



#ifdef DES_READ_SUP

static int __ufshcd_query_descriptor(struct ufs_hba *hba,
				     enum query_opcode opcode,
				     enum desc_idn idn, u8 index, u8 selector,
				     u8 *desc_buf, int *buf_len)
{
	struct ufs_query_req *request = NULL;
	struct ufs_query_res *response = NULL;
	int err;

	if (!desc_buf) {
		dev_err(hba->dev, "%s: descriptor buffer required for opcode 0x%x\n",
			__func__, opcode);
		err = -EINVAL;
		goto out;
	}

	if (*buf_len < QUERY_DESC_MIN_SIZE || *buf_len > QUERY_DESC_MAX_SIZE) {
		dev_err(hba->dev, "%s: descriptor buffer size (%d) is out of range\n",
			__func__, *buf_len);
		err = -EINVAL;
		goto out;
	}

	ufshcd_init_query(hba, &request, &response, opcode, idn, index,
			  selector);
	hba->dev_cmd.query.descriptor = desc_buf;
	request->upiu_req.length = cpu_to_be16(*buf_len);

	switch (opcode) {
	case UPIU_QUERY_OPCODE_WRITE_DESC:
		request->query_func = UPIU_QUERY_FUNC_STANDARD_WRITE_REQUEST;
		break;
	case UPIU_QUERY_OPCODE_READ_DESC:
		request->query_func = UPIU_QUERY_FUNC_STANDARD_READ_REQUEST;
		break;
	default:
		dev_err(hba->dev, "%s: Expected query descriptor opcode but got = 0x%.2x\n",
			__func__, opcode);
		err = -EINVAL;
		goto out;
	}

	err = ufshcd_exec_dev_cmd(hba, DEV_CMD_TYPE_QUERY, QUERY_REQ_TIMEOUT);

	if (err) {
		dev_err(hba->dev, "%s: opcode 0x%.2x for idn %d failed, index %d, err = %d\n",
			__func__, opcode, idn, index, err);
		goto out;
	}

	hba->dev_cmd.query.descriptor = NULL;
	*buf_len = be16_to_cpu(response->upiu_res.length);

out:
	return err;
}

/**
 * ufshcd_query_descriptor_retry - API function for sending descriptor requests
 */
int ufshcd_query_descriptor_retry(struct ufs_hba *hba, enum query_opcode opcode,
				  enum desc_idn idn, u8 index, u8 selector,
				  u8 *desc_buf, int *buf_len)
{
	int err;
	int retries;

	for (retries = QUERY_REQ_RETRIES; retries > 0; retries--) {
		err = __ufshcd_query_descriptor(hba, opcode, idn, index,
						selector, desc_buf, buf_len);
		if (!err || err == -EINVAL)
			break;
	}

	return err;
}

/**
 * ufshcd_read_desc_length - read the specified descriptor length from header
 */
static int ufshcd_read_desc_length(struct ufs_hba *hba, enum desc_idn desc_id,
				   int desc_index, int *desc_length)
{
	int ret;
	u8 header[QUERY_DESC_HDR_SIZE];
	int header_len = QUERY_DESC_HDR_SIZE;

	if (desc_id >= QUERY_DESC_IDN_MAX)
		return -EINVAL;

	ret = ufshcd_query_descriptor_retry(hba, UPIU_QUERY_OPCODE_READ_DESC,
					    desc_id, desc_index, 0, header,
					    &header_len);

	if (ret) {
		dev_err(hba->dev, "%s: Failed to get descriptor header id %d",
			__func__, desc_id);
		return ret;
	} else if (desc_id != header[QUERY_DESC_DESC_TYPE_OFFSET]) {
		dev_warn(hba->dev, "%s: descriptor header id %d and desc_id %d mismatch",
			 __func__, header[QUERY_DESC_DESC_TYPE_OFFSET],
			 desc_id);
		ret = -EINVAL;
	}

	*desc_length = header[QUERY_DESC_LENGTH_OFFSET];

	return ret;
}

static void ufshcd_init_desc_sizes(struct ufs_hba *hba)
{
	int err;

	err = ufshcd_read_desc_length(hba, QUERY_DESC_IDN_DEVICE, 0,
				      &hba->desc_size.dev_desc);
	if (err)
		hba->desc_size.dev_desc = QUERY_DESC_DEVICE_DEF_SIZE;

	err = ufshcd_read_desc_length(hba, QUERY_DESC_IDN_POWER, 0,
				      &hba->desc_size.pwr_desc);
	if (err)
		hba->desc_size.pwr_desc = QUERY_DESC_POWER_DEF_SIZE;

	err = ufshcd_read_desc_length(hba, QUERY_DESC_IDN_INTERCONNECT, 0,
				      &hba->desc_size.interc_desc);
	if (err)
		hba->desc_size.interc_desc = QUERY_DESC_INTERCONNECT_DEF_SIZE;

	err = ufshcd_read_desc_length(hba, QUERY_DESC_IDN_CONFIGURATION, 0,
				      &hba->desc_size.conf_desc);
	if (err)
		hba->desc_size.conf_desc = QUERY_DESC_CONFIGURATION_DEF_SIZE;

	err = ufshcd_read_desc_length(hba, QUERY_DESC_IDN_UNIT, 0,
				      &hba->desc_size.unit_desc);
	if (err)
		hba->desc_size.unit_desc = QUERY_DESC_UNIT_DEF_SIZE;

	err = ufshcd_read_desc_length(hba, QUERY_DESC_IDN_GEOMETRY, 0,
				      &hba->desc_size.geom_desc);
	if (err)
		hba->desc_size.geom_desc = QUERY_DESC_GEOMETRY_DEF_SIZE;

	err = ufshcd_read_desc_length(hba, QUERY_DESC_IDN_HEALTH, 0,
				      &hba->desc_size.hlth_desc);
	if (err)
		hba->desc_size.hlth_desc = QUERY_DESC_HEALTH_DEF_SIZE;
}

/**
 * ufshcd_map_desc_id_to_length - map descriptor IDN to its length
 *
 */
int ufshcd_map_desc_id_to_length(struct ufs_hba *hba, enum desc_idn desc_id,
				 int *desc_len)
{
	switch (desc_id) {
	case QUERY_DESC_IDN_DEVICE:
		*desc_len = hba->desc_size.dev_desc;
		break;
	case QUERY_DESC_IDN_POWER:
		*desc_len = hba->desc_size.pwr_desc;
		break;
	case QUERY_DESC_IDN_GEOMETRY:
		*desc_len = hba->desc_size.geom_desc;
		break;
	case QUERY_DESC_IDN_CONFIGURATION:
		*desc_len = hba->desc_size.conf_desc;
		break;
	case QUERY_DESC_IDN_UNIT:
		*desc_len = hba->desc_size.unit_desc;
		break;
	case QUERY_DESC_IDN_INTERCONNECT:
		*desc_len = hba->desc_size.interc_desc;
		break;
	case QUERY_DESC_IDN_STRING:
		*desc_len = QUERY_DESC_MAX_SIZE;
		break;
	case QUERY_DESC_IDN_HEALTH:
		*desc_len = hba->desc_size.hlth_desc;
		break;
	case QUERY_DESC_IDN_RFU_0:
	case QUERY_DESC_IDN_RFU_1:
		*desc_len = 0;
		break;
	default:
		*desc_len = 0;
		return -EINVAL;
	}
	return 0;
}

/**
 * ufshcd_read_desc_param - read the specified descriptor parameter
 *
 */
//#ifdef DES_READ_SUP
int ufshcd_read_desc_param(struct ufs_hba *hba, enum desc_idn desc_id,
			   int desc_index, u8 param_offset, u8 *param_read_buf,
			   u8 param_size)
{
	int ret;
	u8 *desc_buf;
	int buff_len;
	bool is_kmalloc = true;

	/* Safety check */
	if (desc_id >= QUERY_DESC_IDN_MAX || !param_size)
		return -EINVAL;

	/* Get the max length of descriptor from structure filled up at probe
	 * time.
	 */
	ret = ufshcd_map_desc_id_to_length(hba, desc_id, &buff_len);

	/* Sanity checks */
	if (ret || !buff_len) {
		dev_err(hba->dev, "%s: Failed to get full descriptor length",
			__func__);
		return ret;
	}

	/* Check whether we need temp memory */
	if (param_offset != 0 || param_size < buff_len) {
		desc_buf = kmalloc(buff_len, GFP_KERNEL);
		if (!desc_buf)
			return -ENOMEM;
	} else {
		desc_buf = param_read_buf;
		is_kmalloc = false;
	}

	/* Request for full descriptor */
	ret = ufshcd_query_descriptor_retry(hba, UPIU_QUERY_OPCODE_READ_DESC,
					    desc_id, desc_index, 0, desc_buf,
					    &buff_len);

	if (ret) {
		dev_err(hba->dev, "%s: Failed reading descriptor. desc_id %d, desc_index %d, param_offset %d, ret %d",
			__func__, desc_id, desc_index, param_offset, ret);
		goto out;
	}

	/* Sanity check */
	if (desc_buf[QUERY_DESC_DESC_TYPE_OFFSET] != desc_id) {
		dev_err(hba->dev, "%s: invalid desc_id %d in descriptor header",
			__func__, desc_buf[QUERY_DESC_DESC_TYPE_OFFSET]);
		ret = -EINVAL;
		goto out;
	}

	/* Check wherher we will not copy more data, than available */
	if (is_kmalloc && param_size > buff_len)
		param_size = buff_len;

	if (is_kmalloc)
		memcpy(param_read_buf, &desc_buf[param_offset], param_size);
out:
	if (is_kmalloc)
		kfree(desc_buf);
	return ret;
}

int ufshcd_write_desc_param(struct ufs_hba *hba, enum desc_idn desc_id,
			   int desc_index, u8 param_offset, u8 *param_read_buf,
			   u8 param_size)
{
	int ret;
	u8 *desc_buf;
	int buff_len;

	/* Safety check */
	if (desc_id >= QUERY_DESC_IDN_MAX || !param_size)
		return -EINVAL;

	/* Get the max length of descriptor from structure filled up at probe
	 * time.
	 */
	ret = ufshcd_map_desc_id_to_length(hba, desc_id, &buff_len);

	/* Sanity checks */
	if (ret || !buff_len) {
		dev_err(hba->dev, "%s: Failed to get full descriptor length",
			__func__);
		return ret;
	}

	/* Check whether we need temp memory */
	if (param_offset != 0) {
		dev_err(hba->dev, "%s: write descriptor is unsupport when param_offset is not 0",
			__func__);
		ret = -EINVAL;
		goto out;
	} else {
		desc_buf = param_read_buf;
	}

	if (param_size > buff_len) {
		dev_info(hba->dev, "%s: param size is over buff len,only write buff_len\n", __func__);
	}

	/* Sanity check */
	if (desc_buf[QUERY_DESC_DESC_TYPE_OFFSET] != desc_id) {
		dev_err(hba->dev, "%s: invalid desc_id %d in descriptor header",
			__func__, desc_buf[QUERY_DESC_DESC_TYPE_OFFSET]);
		ret = -EINVAL;
		goto out;
	}


	/* Request for full descriptor */
	ret = ufshcd_query_descriptor_retry(hba, UPIU_QUERY_OPCODE_WRITE_DESC,
					    desc_id, desc_index, 0, desc_buf,
					    &buff_len);

	if (ret) {
		dev_err(hba->dev, "%s: Failed writing descriptor. desc_id %d, desc_index %d, param_offset %d, ret %d",
			__func__, desc_id, desc_index, param_offset, ret);
		goto out;
	}


out:
	return ret;
}


#endif
/* replace non-printable or non-ASCII characters with spaces */
static inline void ufshcd_remove_non_printable(uint8_t *val)
{
	if (!val)
		return;

	if (*val < 0x20 || *val > 0x7e)
		*val = ' ';
}

#ifdef PWR_MODE_CHANGE
/**
 * ufshcd_uic_pwr_ctrl - executes UIC commands (which affects the link power
 * state) and waits for it to take effect.
 *
 */
static int ufshcd_uic_pwr_ctrl(struct ufs_hba *hba, struct uic_command *cmd)
{
	unsigned long start = 0;
	u8 status;
	int ret;

	ret = ufshcd_send_uic_cmd(hba, cmd);
	if (ret) {
		dev_err(hba->dev,
			"pwr ctrl cmd 0x%x with mode 0x%x uic error %d\n",
			cmd->command, cmd->argument3, ret);

		return ret;
	}

	start = get_timer(0);
	do {
		status = ufshcd_get_upmcrs(hba);
		if (get_timer(start) > UFS_UIC_CMD_TIMEOUT) {
			dev_err(hba->dev,
				"pwr ctrl cmd 0x%x failed, host upmcrs:0x%x\n",
				cmd->command, status);
			ret = (status != PWR_OK) ? status : -1;
			break;
		}
	} while (status != PWR_LOCAL);

	return ret;
}

/**
 * ufshcd_uic_change_pwr_mode - Perform the UIC power mode change
 *				using DME_SET primitives.
 */
static int ufshcd_uic_change_pwr_mode(struct ufs_hba *hba, u8 mode)
{
	struct uic_command uic_cmd = {0};
	int ret;

	memset(&uic_cmd, 0, sizeof(struct uic_command));
	uic_cmd.command = UIC_CMD_DME_SET;
	uic_cmd.argument1 = UIC_ARG_MIB(PA_PWRMODE);
	uic_cmd.argument3 = mode;
	ret = ufshcd_uic_pwr_ctrl(hba, &uic_cmd);

	return ret;
}
#endif

static
void ufshcd_prepare_utp_scsi_cmd_upiu(struct ufs_hba *hba,
				      struct scsi_cmd *pccb, u32 upiu_flags)
{
	struct utp_upiu_req *ucd_req_ptr = hba->ucd_req_ptr;
	unsigned int cdb_len;

	/* command descriptor fields */
	ucd_req_ptr->header.dword_0 =
			UPIU_HEADER_DWORD(UPIU_TRANSACTION_COMMAND, upiu_flags,
					  pccb->lun, TASK_TAG);
	ucd_req_ptr->header.dword_1 =
			UPIU_HEADER_DWORD(UPIU_COMMAND_SET_TYPE_SCSI, 0, 0, 0);

	/* Total EHS length and Data segment length will be zero */
	ucd_req_ptr->header.dword_2 = 0;

	ucd_req_ptr->sc.exp_data_transfer_len = cpu_to_be32(pccb->datalen);

	cdb_len = min_t(unsigned short, pccb->cmdlen, UFS_CDB_SIZE);
	//cdb_len = min(pccb->cmdlen, UFS_CDB_SIZE);
	memset(ucd_req_ptr->sc.cdb, 0, UFS_CDB_SIZE);
	memcpy(ucd_req_ptr->sc.cdb, pccb->cmd, cdb_len);
	memset(hba->ucd_rsp_ptr, 0, sizeof(struct utp_upiu_rsp));
}

static inline void prepare_prdt_desc(struct ufshcd_sg_entry *entry,
				     unsigned char *buf, ulong len)
{
	entry->size = cpu_to_le32(len) | 0x3;
	entry->base_addr = cpu_to_le32(lower_32_bits((unsigned long)buf));
	entry->upper_addr = cpu_to_le32(upper_32_bits((unsigned long)buf));
}

static void prepare_prdt_table(struct ufs_hba *hba, struct scsi_cmd *pccb)
{
	struct utp_transfer_req_desc *req_desc = hba->utrdl;
	struct ufshcd_sg_entry *prd_table = hba->ucd_prdt_ptr;
	ulong datalen = pccb->datalen;
	int table_length;
	u8 *buf;
	int i;

	if (!datalen) {
		req_desc->prd_table_length = 0;
		return;
	}

	table_length = DIV_ROUND_UP(pccb->datalen, MAX_PRDT_ENTRY);
	buf = pccb->pdata;
	i = table_length;
	while (--i) {
		prepare_prdt_desc(&prd_table[table_length - i - 1], buf,
				  MAX_PRDT_ENTRY - 1);
		buf += MAX_PRDT_ENTRY;
		datalen -= MAX_PRDT_ENTRY;
	}

	prepare_prdt_desc(&prd_table[table_length - i - 1], buf, datalen - 1);

	req_desc->prd_table_length = table_length;
}

/**
 * ufshcd_copy_sense_data - Copy sense data in case of check condition
 * @lrbp: pointer to local reference block
 */
static inline void ufshcd_copy_sense_data(struct ufs_hba *hba, struct scsi_cmd *pccb)
{
	u8 *const sense_buffer = pccb->sense_buf;
	u16 resp_len;
	int len;

	resp_len = ufshcd_get_rsp_upiu_seg_len(hba->ucd_rsp_ptr);
	if (sense_buffer && resp_len) {
		int len_to_copy;

		len = be16_to_cpu(hba->ucd_rsp_ptr->sr.sense_data_len);
		len_to_copy = min_t(int, RESPONSE_UPIU_SENSE_DATA_LENGTH, len);

		memcpy(sense_buffer, hba->ucd_rsp_ptr->sr.sense_data,
		       len_to_copy);
		dev_dbg(hba->dev, "copy sense data\n");
	}
}


/**
 * ufshcd_scsi_cmd_status - Update SCSI command result based on SCSI status
 * @lrbp: pointer to local reference block of completed command
 * @scsi_status: SCSI command status
 *
 * Return: value base on SCSI command status.
 */
static inline int
ufshcd_scsi_cmd_status(struct ufs_hba *hba, struct scsi_cmd *pccb, int scsi_status)
{
	int result = 0;

	switch (scsi_status) {
	case SAM_STAT_CHECK_CONDITION:
		dev_err(hba->dev, "scsi status:Check conditon\n");
		ufshcd_copy_sense_data(hba, pccb);
		//fallthrough;
	case SAM_STAT_GOOD:
		result |= DID_OK << 16 | scsi_status;
		break;
	case SAM_STAT_TASK_SET_FULL:
		dev_err(hba->dev, "scsi status:task set full\n");
	case SAM_STAT_BUSY:
		dev_err(hba->dev, "scsi status:busy\n");
	case SAM_STAT_TASK_ABORTED:
		dev_err(hba->dev, "scsi status:task abort\n");
		ufshcd_copy_sense_data(hba, pccb);
		result |= scsi_status;
		break;
	default:
		dev_err(hba->dev, "scsi status:unknow %x\n", scsi_status);
		result |= DID_ERROR << 16;
		break;
	} /* end of switch */

	return result;
}


static int ufs_scsi_exec(struct udevice *scsi_dev, struct scsi_cmd *pccb)
{
	struct ufs_hba *hba = &(scsi_dev->uhba);
	struct utp_transfer_req_desc *req_desc = hba->utrdl;
	u32 upiu_flags;
	int ocs, result = 0;
	u8 scsi_status;

	ufshcd_prepare_req_desc_hdr(req_desc, &upiu_flags, pccb->dma_dir);
	ufshcd_prepare_utp_scsi_cmd_upiu(hba, pccb, upiu_flags);
	prepare_prdt_table(hba, pccb);

	ufshcd_send_command(hba, TASK_TAG);

	ocs = ufshcd_get_tr_ocs(hba);
	switch (ocs) {
	case OCS_SUCCESS:
		result = ufshcd_get_req_rsp(hba->ucd_rsp_ptr);
		switch (result) {
		case UPIU_TRANSACTION_RESPONSE:
			result = ufshcd_get_rsp_upiu_result(hba->ucd_rsp_ptr);

			scsi_status = result & MASK_SCSI_STATUS;
			if (scsi_status) {
				dev_err(hba->dev, "sc st %x\n", scsi_status);

				ufshcd_scsi_cmd_status(hba, pccb, scsi_status);
				print_hex_dump("[UFS]:sense buf: ", DUMP_PREFIX_ADDRESS, 16, 1, pccb->sense_buf, 16, true);
				dev_dump(hba->dev, pccb->sense_buf, 16);
				sunxi_ufs_trace_point(SUNXI_UFS_SCI_ST_ERR);
				return -EINVAL;
			}

			break;
		case UPIU_TRANSACTION_REJECT_UPIU:
			/* TODO: handle Reject UPIU Response */
			dev_err(hba->dev,
				"Reject UPIU not fully implemented\n");
			return -EINVAL;
		default:
			dev_err(hba->dev,
				"Unexpected request response code = %x\n",
				result);
			return -EINVAL;
		}
		break;
	default:
		sunxi_ufs_trace_point(SUNXI_UFS_OCS_ERR);
		dev_err(hba->dev, "OCS error from controller = %x\n", ocs);
		return -EINVAL;
	}

	return 0;
}

#ifdef DES_READ_SUP
static inline int ufshcd_read_desc(struct ufs_hba *hba, enum desc_idn desc_id,
				   int desc_index, u8 *buf, u32 size)
{
	return ufshcd_read_desc_param(hba, desc_id, desc_index, 0, buf, size);
}

static inline int ufshcd_write_desc(struct ufs_hba *hba, enum desc_idn desc_id,
				   int desc_index, u8 *buf, u32 size)
{
	return ufshcd_write_desc_param(hba, desc_id, desc_index, 0, buf, size);
}

#if 1
static int ufshcd_read_device_desc(struct ufs_hba *hba, u8 *buf, u32 size)
{
	return ufshcd_read_desc(hba, QUERY_DESC_IDN_DEVICE, 0, buf, size);
}
#endif
static int ufshcd_read_geometry_desc(struct ufs_hba *hba, u8 *buf, u32 size)
{
	return ufshcd_read_desc(hba, QUERY_DESC_IDN_GEOMETRY, 0, buf, size);
}

static int ufshcd_read_config_desc(struct ufs_hba *hba, u8 *buf, u32 size)
{
	return ufshcd_read_desc(hba, QUERY_DESC_IDN_CONFIGURATION, 0, buf, size);
}

static int ufshcd_read_unit_desc(struct ufs_hba *hba, u8 lun, u8 *buf, u32 size)
{
	return ufshcd_read_desc(hba, QUERY_DESC_IDN_UNIT, lun, buf, size);
}


static int ufshcd_write_config_desc(struct ufs_hba *hba, u8 *buf, u32 size)
{
	return ufshcd_write_desc(hba, QUERY_DESC_IDN_CONFIGURATION, 0, buf, size);
}

static int ufshcd_read_health_desc(struct ufs_hba *hba, u8 *buf, u32 size)
{
	return ufshcd_read_desc(hba, QUERY_DESC_IDN_HEALTH, 0, buf, size);
}



#if 0
/**
 * ufshcd_read_string_desc - read string descriptor
 *
 */
int ufshcd_read_string_desc(struct ufs_hba *hba, int desc_index,
			    u8 *buf, u32 size, bool ascii)
{
	int err = 0;

	err = ufshcd_read_desc(hba, QUERY_DESC_IDN_STRING, desc_index, buf,
			       size);

	if (err) {
		dev_err(hba->dev, "%s: reading String Desc failed after %d retries. err = %d\n",
			__func__, QUERY_REQ_RETRIES, err);
		goto out;
	}

	if (ascii) {
		int desc_len;
		int ascii_len;
		int i;
		u8 *buff_ascii;

		desc_len = buf[0];
		/* remove header and divide by 2 to move from UTF16 to UTF8 */
		ascii_len = (desc_len - QUERY_DESC_HDR_SIZE) / 2 + 1;
		if (size < ascii_len + QUERY_DESC_HDR_SIZE) {
			dev_err(hba->dev, "%s: buffer allocated size is too small\n",
				__func__);
			err = -ENOMEM;
			goto out;
		}

		buff_ascii = kmalloc(ascii_len, GFP_KERNEL);
		if (!buff_ascii) {
			err = -ENOMEM;
			goto out;
		}

		/*
		 * the descriptor contains string in UTF16 format
		 * we need to convert to utf-8 so it can be displayed
		 */
		utf16_to_utf8(buff_ascii,
			      (uint16_t *)&buf[QUERY_DESC_HDR_SIZE], ascii_len);

		/* replace non-printable or non-ASCII characters with spaces */
		for (i = 0; i < ascii_len; i++)
			ufshcd_remove_non_printable(&buff_ascii[i]);

		memset(buf + QUERY_DESC_HDR_SIZE, 0,
		       size - QUERY_DESC_HDR_SIZE);
		memcpy(buf + QUERY_DESC_HDR_SIZE, buff_ascii, ascii_len);
		buf[QUERY_DESC_LENGTH_OFFSET] = ascii_len + QUERY_DESC_HDR_SIZE;
		kfree(buff_ascii);
	}
out:
	return err;
}
#endif
#if 0
static int ufs_get_device_desc(struct ufs_hba *hba,
			       struct ufs_dev_desc *dev_desc)
{
	int err;
	size_t buff_len;
	u8 model_index;
	u8 *desc_buf;

	buff_len = max_t(size_t, hba->desc_size.dev_desc,
			 QUERY_DESC_MAX_SIZE + 1);
	desc_buf = kmalloc(buff_len, GFP_KERNEL);
	if (!desc_buf) {
		err = -ENOMEM;
		goto out;
	}

	err = ufshcd_read_device_desc(hba, desc_buf, hba->desc_size.dev_desc);
	if (err) {
		dev_err(hba->dev, "%s: Failed reading Device Desc. err = %d\n",
			__func__, err);
		goto out;
	}

	/*
	 * getting vendor (manufacturerID) and Bank Index in big endian
	 * format
	 */
	dev_desc->wmanufacturerid = desc_buf[DEVICE_DESC_PARAM_MANF_ID] << 8 |
				     desc_buf[DEVICE_DESC_PARAM_MANF_ID + 1];

	model_index = desc_buf[DEVICE_DESC_PARAM_PRDCT_NAME];

	/* Zero-pad entire buffer for string termination. */
	memset(desc_buf, 0, buff_len);

	err = ufshcd_read_string_desc(hba, model_index, desc_buf,
				      QUERY_DESC_MAX_SIZE, true/*ASCII*/);
	if (err) {
		dev_err(hba->dev, "%s: Failed reading Product Name. err = %d\n",
			__func__, err);
		goto out;
	}

	desc_buf[QUERY_DESC_MAX_SIZE] = '\0';
	strlcpy(dev_desc->model, (char *)(desc_buf + QUERY_DESC_HDR_SIZE),
		min_t(u8, desc_buf[QUERY_DESC_LENGTH_OFFSET],
		      MAX_MODEL_LEN));

	/* Null terminate the model string */
	dev_desc->model[MAX_MODEL_LEN] = '\0';

out:
	kfree(desc_buf);
	return err;
}
#endif

int ufshcd_device_health_check(struct ufs_hba *hba)
{
	u8 h_buf[QUERY_DESC_MAX_SIZE] = {0};
	int err = 0;
	int i = 0;
	memset(h_buf, 0, QUERY_DESC_MAX_SIZE);
	err = ufshcd_read_health_desc(hba, h_buf, QUERY_DESC_MAX_SIZE);
	if (err) {
		dev_err(hba->dev, "Get health status err = %d\n",
			err);
		goto out;
	}

	print_hex_dump("[UFS]: health desc: ", DUMP_PREFIX_ADDRESS, 16, 1, h_buf, QUERY_DESC_MAX_SIZE, true);
	if (h_buf[B_PREEOLINFO_OFFSET] > 1) {
		i = 10;
		while (i--) {
			dev_err(hba->dev, "Error!!!:eol info %x is over normal\n", h_buf[B_PREEOLINFO_OFFSET]);
		}
	} else {
		dev_info(hba->dev, "Eol info %x\n", h_buf[B_PREEOLINFO_OFFSET]);
	}

	if (h_buf[B_DEVICE_LIFETIME_EST_A_OFFSET] > 0xa) {
		i = 10;
		while (i--) {
			dev_err(hba->dev, "Error!!!:life time a %x is over 90\n", h_buf[B_DEVICE_LIFETIME_EST_A_OFFSET]);
		}
	} else if (h_buf[B_DEVICE_LIFETIME_EST_A_OFFSET] > 0x9) {
			dev_warn(hba->dev, "Warning!!!:life time a %x is over 80\n", h_buf[B_DEVICE_LIFETIME_EST_A_OFFSET]);
	} else {
		dev_info(hba->dev, "life time a %x\n", h_buf[B_DEVICE_LIFETIME_EST_A_OFFSET]);
	}

	if (h_buf[B_DEVICE_LIFETIME_EST_B_OFFSET] > 0xa) {
		i = 10;
		while (i--) {
			dev_err(hba->dev, "Error!!!:life time b %x is over 90\n", h_buf[B_DEVICE_LIFETIME_EST_B_OFFSET]);
		}
	} else if (h_buf[B_DEVICE_LIFETIME_EST_B_OFFSET] > 0x9) {
			dev_warn(hba->dev, "Warning!!!:life time b %x is over 80\n", h_buf[B_DEVICE_LIFETIME_EST_B_OFFSET]);
	} else {
		dev_info(hba->dev, "life time b %x\n", h_buf[B_DEVICE_LIFETIME_EST_B_OFFSET]);
	}

out:
	return err;
}

int ufshcd_device_exception_event_check(struct ufs_hba *hba)
{
	/*only enable low 3 bit,for ufs2.0,2.1,ony lower 3 bit
	 * can be enabled*
	 * To do:judge ufs version to enable diff QUERY_ATTR_IDN_EE_CONTROL
	 * */

/*
	u32 tmp = 0x7;
	//u32 dDynCapNeeded = 0;
	int err = 0;
	err = ufshcd_query_attr_retry(hba, UPIU_QUERY_OPCODE_WRITE_ATTR,
			QUERY_ATTR_IDN_EE_CONTROL, 0, 0, &tmp);

	if (err) {
		dev_err(hba->dev, "Exception Event Control enable failed,err=%x\n", err);
		goto out;
	}

	err = ufshcd_query_attr_retry(hba, UPIU_QUERY_OPCODE_READ_ATTR,
			QUERY_ATTR_IDN_EE_CONTROL, 0, 0, &tmp);

	if (err) {
		dev_err(hba->dev, "Exception Event Control read faile,err=%d\n",
			err);
		goto out;
	}
	dev_err(hba->dev, "ee ctrl %x\n", tmp);
*/
	u32 tmp = 0x0;
	int i = 0;
	int err = 0;
	err = ufshcd_query_attr_retry(hba, UPIU_QUERY_OPCODE_READ_ATTR,
			QUERY_ATTR_IDN_EE_STATUS, 0, 0, &tmp);

	if (err) {
		dev_err(hba->dev, "Get ExceptionEven status err = %d\n",
			err);
		goto out;
	}
	if (tmp) {
		i = 10;
		while (i--) {
			dev_err(hba->dev, "Warning wExceptionEventStatus %x !!!!!!!\n", tmp);
		}
	} else
		dev_dbg(hba->dev, "wExceptionEventStatus %x\n", tmp);
/*
	err = ufshcd_query_attr_retry(hba, UPIU_QUERY_OPCODE_READ_ATTR,
			QUERY_ATTR_IDN_DYN_CAP_NEEDED, lun, 0, &dDynCapNeeded);
	if (err) {
		dev_err(hba->dev, "failed reading dDynCapNeeded. err = %d\n",
			err);
	}
	if (dDynCapNeeded)
		dev_err(hba->dev, "dDynCapNeeded %x of lun %x\n", dDynCapNeeded, lun);
*/
out:
	return err;
}

int ufshcd_device_resource_check(struct ufs_hba *hba, int lun)
{
	u64 qLogicalBlockCount = 0;
	u64 qPhyMemResourceCount = 0;
	u8 unit_buf[QUERY_DESC_MAX_SIZE] = {0};
	int err = 0;
	int i = 0;
	err = ufshcd_read_unit_desc(hba, lun, unit_buf, QUERY_DESC_MAX_SIZE);
	if (err) {
		dev_err(hba->dev, "%s: Failed reading unit Desc. err = %d\n",
			__func__, err);
		goto out;
	}
	qLogicalBlockCount = get_unaligned_be64(unit_buf + Q_LOGICAL_BLOCK_COUNT_OFFSET);
	qPhyMemResourceCount = get_unaligned_be64(unit_buf + Q_PHY_MEM_RESOURCE_COUNT_OFFSET);
	dev_dbg(hba->dev, "qLogicalBlockCount 0x%016llX,qPhyMemResourceCount 0x%016llX, bBootLunID %d\n", qLogicalBlockCount, qPhyMemResourceCount, unit_buf[B_BOOT_LUN_ID]);

	if (qLogicalBlockCount != qPhyMemResourceCount) {
		i = 10;
		while (i--) {
			dev_err(hba->dev, "Error!!! qLogicalBlockCount 0x%016llX != qPhyMemResourceCount 0x%016llX, bBootLunID %d\n", qLogicalBlockCount, qPhyMemResourceCount, unit_buf[B_BOOT_LUN_ID]);
		}
		print_hex_dump("[UFS]:unit desc: ", DUMP_PREFIX_ADDRESS, 16, 1, unit_buf, QUERY_DESC_MAX_SIZE, true);
	}
out:
	return err;
}

int ufshcd_get_dev_ud0base_offset_udconfig_param_len(struct ufs_hba *hba, u8 *base_offset, u8 *cfg_len)
{
	u8 dev_buf[QUERY_DESC_MAX_SIZE] = {0};
	int err = 0;
	if (!base_offset && !cfg_len) {
		dev_err(hba->dev, "%s: input arg  err,%x,%x\n",
			__func__, (u32)base_offset, (u32)cfg_len);
		return err;
	}

	err = ufshcd_read_device_desc(hba, dev_buf, hba->desc_size.dev_desc);
	if (err) {
		dev_err(hba->dev, "%s: Failed reading Device Desc. err = %d\n",
			__func__, err);
		return err;
	}

	*base_offset = dev_buf[B_UD0_BASE_OFFSET_OFFSET];
	*cfg_len = dev_buf[B_UD_CONFIGP_LENGTH_OFFSET];
	dev_dbg(hba->dev, "bUD0BaseOffset = %x, bUDConfigPLength = %x\n", *base_offset, *cfg_len);
	print_hex_dump("[UFS]: device desc: ", DUMP_PREFIX_ADDRESS, 16, 1, dev_buf, QUERY_DESC_MAX_SIZE, true);
	return err;
}

int ufshcd_logical_unit_config(struct ufs_hba *hba, int force_reconfig)
{
	u8 cfg_buf[QUERY_DESC_MAX_SIZE] = {0};
	u8 *g_buf = cfg_buf;
	u8 head_off = CONFIG_HEADER_OFFSET;
	__u8 lun_off = CONFIG_LUN_OFFSET;
	/*Only support lun 0 now
	 *To do:support other lun
	 * **/
	int lun = 0;
	u64 raw_device_capacity = 0;
	u8 allocation_unit_size = 0;
	u32 segment_size = 0;
	/*The Capacity Adjustment Factor value for Normal memory type is one*/
	u32 capacity_adjfactor = 1;
	u32 max_num_alloc_units = 0;
	u32 res_num_alloc_units = 0;
	u8 bUD0BaseOffset = 0;
	u8 bUDConfigPLength = 0;
	int err = 0;


	err = ufshcd_device_exception_event_check(hba);
	if (err) {
		goto out;
	}

	err = ufshcd_device_health_check(hba);
	if (err) {
		goto out;
	}

	err = ufshcd_get_dev_ud0base_offset_udconfig_param_len(hba, &bUD0BaseOffset, &bUDConfigPLength);
	if (err) {
		dev_err(hba->dev, "failed reading bUD0BaseOffset,bUDConfigPLength . err = %d\n",
			err);
		goto out;
	}
	dev_dbg(hba->dev, "bUD0BaseOffset = %x, bUDConfigPLength = %x\n", bUD0BaseOffset, bUDConfigPLength);

	memset(g_buf, 0, QUERY_DESC_MAX_SIZE);
	err = ufshcd_read_geometry_desc(hba, g_buf, QUERY_DESC_MAX_SIZE);
	if (err) {
		dev_err(hba->dev, "%s: Failed reading geometry Desc. err = %d\n",
			__func__, err);
		return err;
	}

	print_hex_dump("[UFS]:geometry: ", DUMP_PREFIX_ADDRESS, 16, 1, g_buf, QUERY_DESC_MAX_SIZE, true);
	raw_device_capacity = get_unaligned_be64(g_buf + Q_TOTAL_RAW_DEVICE_CAPACITY_OFFSET);
	allocation_unit_size = g_buf[B_ALLOCATION_UNIT_SIZE_OFFSET];
	segment_size = get_unaligned_be32(g_buf + D_SEGMENT_SIZE_OFFSET);
	/**raw_device_capacity/CONFIG_RESERVED_SPACE_SECTOR  unit is 512 byte,so no div 512 when cal max alloc unit*/
	max_num_alloc_units = (raw_device_capacity * capacity_adjfactor)/allocation_unit_size/segment_size;
	res_num_alloc_units = (CONFIG_RESERVED_SPACE_SECTOR * capacity_adjfactor)/allocation_unit_size/segment_size;
/*
	dev_info(hba->dev, "qTotalRawDeviceCapacity 0x%016llX, CapacityAdjFactor 0x%08x, bAllocationUnitSize 0x%02x, dSegmentSize 0x%08x, max_num_alloc_units 0x%08x, res_num_alloc_units 0x%08x\n",\
			raw_device_capacity, capacity_adjfactor, allocation_unit_size, segment_size, max_num_alloc_units, res_num_alloc_units);
*/
	dev_info(hba->dev, "qTotalRawDeviceCapacity 0x%llX, max_num_alloc_units 0x%x\n", raw_device_capacity, max_num_alloc_units);

	memset(cfg_buf, 0, QUERY_DESC_MAX_SIZE);
	err = ufshcd_read_config_desc(hba, cfg_buf, QUERY_DESC_MAX_SIZE);
	if (err) {
		dev_err(hba->dev, "%s: Failed reading config Desc. err = %d\n",
			__func__, err);
		return err;
	}

	print_hex_dump("[UFS]:config desc: ", DUMP_PREFIX_ADDRESS, 16, 1, cfg_buf, QUERY_DESC_MAX_SIZE, true);


	if (cfg_buf[0] == QUERY_DESC_CONFIGURATION_DEF_SIZE) {
    //if (cfg_buf[0] == QUERY_DESC_CONFIGURAION_MAX_SIZE_3_0) {
		head_off = CONFIG_HEADER_OFFSET_3_0;
		lun_off = CONFIG_LUN_OFFSET_3_0;
	}
	if (head_off != bUD0BaseOffset)
		head_off = bUD0BaseOffset;
	if (lun_off != bUDConfigPLength)
		lun_off = bUDConfigPLength;

	dev_dbg(hba->dev, "cfg_buf[0] %x, head_off %x\n", cfg_buf[0], head_off);
	//dev_info(hba->dev, "bLUEnable %x\n ", cfg_buf[head_off + lun_off * lun +B_LU_ENABLE_OFFSET]);
	dev_info(hba->dev, "dNumAllocUnits:0x%x\n ", get_unaligned_be32(cfg_buf + head_off + lun_off * lun +  D_NUM_ALLOC_UNITS_OFFSET));
	dev_dbg(hba->dev, "bLogicalBlockSize:%x\n ", cfg_buf[head_off + lun_off * lun + B_LOGICAL_BLOCKSIZE_OFFSET]);
	dev_dbg(hba->dev, "bProvisioningType:%x\n ", cfg_buf[head_off + lun_off * lun + BPROVISIONINGTYPE_OFFSET]);
	dev_dbg(hba->dev, "bdatareliability:%x\n ", cfg_buf[head_off + lun_off * lun +  B_DATA_RELIABILITY_OFFSET]);

	if (force_reconfig || (cfg_buf[head_off + lun_off * lun + B_LU_ENABLE_OFFSET] != 1)) {
		if (force_reconfig) {
			dev_err(hba->dev, "force reconfig\n");
		}
		cfg_buf[head_off + lun_off * lun + B_LU_ENABLE_OFFSET] = 1;

		dev_info(hba->dev, "qTotalRawDeviceCapacity 0x%llX, CapacityAdjFactor 0x%x, bAllocationUnitSize 0x%x, dSegmentSize 0x%x, max_num_alloc_units 0x%x, res_num_alloc_units 0x%x\n",\
							raw_device_capacity, capacity_adjfactor, allocation_unit_size, segment_size, max_num_alloc_units, res_num_alloc_units);
		//dev_info(hba->dev, "dNumAllocUnits:0x%08x\n ", get_unaligned_be32(cfg_buf + head_off + lun_off * lun +  D_NUM_ALLOC_UNITS_OFFSET));
		dev_info(hba->dev, "bLogicalBlockSize:%x\n ", cfg_buf[head_off + lun_off * lun + B_LOGICAL_BLOCKSIZE_OFFSET]);
		dev_info(hba->dev, "bProvisioningType:%x\n ", cfg_buf[head_off + lun_off * lun + BPROVISIONINGTYPE_OFFSET]);
		dev_info(hba->dev, "bdatareliability:%x\n ", cfg_buf[head_off + lun_off * lun +  B_DATA_RELIABILITY_OFFSET]);

		put_unaligned_be32(max_num_alloc_units - res_num_alloc_units, &cfg_buf[head_off + lun_off * lun + D_NUM_ALLOC_UNITS_OFFSET]);
		cfg_buf[head_off + lun_off * lun + B_LOGICAL_BLOCKSIZE_OFFSET] = 0xc;//4k
		cfg_buf[head_off + lun_off * lun + BPROVISIONINGTYPE_OFFSET] = BPROVISIONINGTYPE_TYPE_ERASE;
		cfg_buf[head_off + lun_off * lun + B_DATA_RELIABILITY_OFFSET] = B_DATA_RELIABILITY_ENABLED;
//		cfg_buf[head_off + lun_off * lun + 0xb] = 0;
//		cfg_buf[head_off + lun_off * lun + 0xc] = 0;

		err = ufshcd_write_config_desc(hba, cfg_buf, QUERY_DESC_MAX_SIZE);
		if (err) {
			dev_err(hba->dev, "%s: Failed reading unit Desc. err = %d\n",
				__func__, err);
			return err;
		}
		dev_info(hba->dev, "write config des:config logical unit\n");
	}
	ufshcd_device_resource_check(hba, lun);
out:
	return err;
}



int ufshcd_enable_erase_by_lun(struct ufs_hba *hba, int lun)

{
	u8 cfg_buf[QUERY_DESC_MAX_SIZE] = {0};
    __u8 head_off = CONFIG_HEADER_OFFSET;
	__u8 lun_off = CONFIG_LUN_OFFSET;
	u8 bUD0BaseOffset = 0;
	u8 bUDConfigPLength = 0;
	int err = 0;

	if (lun != 0) {
		dev_err(hba->dev, "Enable ufs erase only support lun 0 now\n");
		return -EINVAL;
	}

	memset(cfg_buf, 0, QUERY_DESC_MAX_SIZE);

	ufshcd_read_config_desc(hba, cfg_buf, QUERY_DESC_MAX_SIZE);
	print_hex_dump("[UFS]:desc: ", DUMP_PREFIX_ADDRESS, 16, 1, cfg_buf, QUERY_DESC_MAX_SIZE, true);

	err = ufshcd_get_dev_ud0base_offset_udconfig_param_len(hba, &bUD0BaseOffset, &bUDConfigPLength);
	if (err) {
		dev_err(hba->dev, "failed reading bUD0BaseOffset,bUDConfigPLength . err = %d\n",
			err);
		goto out;
	}
	dev_dbg(hba->dev, "bUD0BaseOffset = %x, bUDConfigPLength = %x\n", bUD0BaseOffset, bUDConfigPLength);

	if (cfg_buf[0] == QUERY_DESC_CONFIGURATION_DEF_SIZE) {
    //if (cfg_buf[0] == QUERY_DESC_CONFIGURAION_MAX_SIZE_3_0) {
		head_off = CONFIG_HEADER_OFFSET_3_0;
		lun_off = CONFIG_LUN_OFFSET_3_0;
	}
	if (head_off != bUD0BaseOffset)
		head_off = bUD0BaseOffset;
	if (lun_off != bUDConfigPLength)
		lun_off = bUDConfigPLength;


	dev_dbg(hba->dev, "cfg_buf[0] %x, head_off %x\n", cfg_buf[0], head_off);
	dev_dbg(hba->dev, "dNumAllocUnits value %x %x %x %x\n ", cfg_buf[head_off + 0x4],\
			cfg_buf[head_off + 0x5], cfg_buf[head_off + 0x6], cfg_buf[head_off + 0x7]);
	dev_info(hba->dev, "bProvisioningType value %x\n ", cfg_buf[head_off + lun_off * lun + BPROVISIONINGTYPE_OFFSET]);

	if (cfg_buf[head_off + lun_off * lun +  BPROVISIONINGTYPE_OFFSET] != BPROVISIONINGTYPE_TYPE_ERASE) {
		cfg_buf[head_off + lun_off * lun + BPROVISIONINGTYPE_OFFSET] = BPROVISIONINGTYPE_TYPE_ERASE;
		ufshcd_write_config_desc(hba, cfg_buf, QUERY_DESC_MAX_SIZE);
		dev_info(hba->dev, "write config des:enable erase\n");
	}
out:
	return err;
}

int ufshcd_enable_data_reliability_by_lun(struct ufs_hba *hba, int lun)

{
	u8 cfg_buf[QUERY_DESC_MAX_SIZE] = {0};
    __u8 head_off = CONFIG_HEADER_OFFSET;
	__u8 lun_off = CONFIG_LUN_OFFSET;
	int err = 0;
	u8 bUD0BaseOffset = 0;
	u8 bUDConfigPLength = 0;


	if (lun != 0) {
		dev_err(hba->dev, "Enable ufs erase only support lun 0 now\n");
		err =  -EINVAL;
		goto out;
	}

	memset(cfg_buf, 0, QUERY_DESC_MAX_SIZE);

	ufshcd_read_config_desc(hba, cfg_buf, QUERY_DESC_MAX_SIZE);
	print_hex_dump("[UFS]:desc: ", DUMP_PREFIX_ADDRESS, 16, 1, cfg_buf, QUERY_DESC_MAX_SIZE, true);

	err = ufshcd_get_dev_ud0base_offset_udconfig_param_len(hba, &bUD0BaseOffset, &bUDConfigPLength);
	if (err) {
		dev_err(hba->dev, "failed reading bUD0BaseOffset,bUDConfigPLength . err = %d\n",
			err);
		goto out;
	}
	dev_dbg(hba->dev, "bUD0BaseOffset = %x, bUDConfigPLength = %x\n", bUD0BaseOffset, bUDConfigPLength);

	if (cfg_buf[0] == QUERY_DESC_CONFIGURATION_DEF_SIZE) {
    //if (cfg_buf[0] == QUERY_DESC_CONFIGURAION_MAX_SIZE_3_0) {
		head_off = CONFIG_HEADER_OFFSET_3_0;
		lun_off = CONFIG_LUN_OFFSET_3_0;
	}
	if (head_off != bUD0BaseOffset)
		head_off = bUD0BaseOffset;
	if (lun_off != bUDConfigPLength)
		lun_off = bUDConfigPLength;


	dev_dbg(hba->dev, "cfg_buf[0] %x, head_off %x\n", cfg_buf[0], head_off);
	dev_dbg(hba->dev, "dNumAllocUnits value %x %x %x %x\n ", cfg_buf[head_off + 0x4],\
			cfg_buf[head_off + 0x5], cfg_buf[head_off + 0x6], cfg_buf[head_off + 0x7]);
	dev_info(hba->dev, "bdatareliability value %x\n ", cfg_buf[head_off + lun_off * lun + B_DATA_RELIABILITY_OFFSET]);

	if (cfg_buf[head_off + lun_off * lun + B_DATA_RELIABILITY_OFFSET] != B_DATA_RELIABILITY_ENABLED) {
		cfg_buf[head_off + lun_off * lun + B_DATA_RELIABILITY_OFFSET] = B_DATA_RELIABILITY_ENABLED;
		ufshcd_write_config_desc(hba, cfg_buf, QUERY_DESC_MAX_SIZE);
		dev_info(hba->dev, "write config des:enable data reliability\n");
	}
out:
	return err;
}



#endif

#ifdef PWR_MODE_CHANGE
/**
 * ufshcd_get_max_pwr_mode - reads the max power mode negotiated with device
 */
static int ufshcd_get_max_pwr_mode(struct ufs_hba *hba)
{
	struct ufs_pa_layer_attr *pwr_info = &hba->max_pwr_info.info;

	if (hba->max_pwr_info.is_valid)
		return 0;

	pwr_info->pwr_tx = FAST_MODE;
	pwr_info->pwr_rx = FAST_MODE;
	pwr_info->hs_rate = PA_HS_MODE_B;

	/* Get the connected lane count */
	ufshcd_dme_get(hba, UIC_ARG_MIB(PA_CONNECTEDRXDATALANES),
		       &pwr_info->lane_rx);
	ufshcd_dme_get(hba, UIC_ARG_MIB(PA_CONNECTEDTXDATALANES),
		       &pwr_info->lane_tx);

	if (!pwr_info->lane_rx || !pwr_info->lane_tx) {
		dev_err(hba->dev, "%s: invalid connected lanes value. rx=%d, tx=%d\n",
			__func__, pwr_info->lane_rx, pwr_info->lane_tx);
		return -EINVAL;
	}

	/*
	 * First, get the maximum gears of HS speed.
	 * If a zero value, it means there is no HSGEAR capability.
	 * Then, get the maximum gears of PWM speed.
	 */
#ifdef PWR_HS_MODE
	ufshcd_dme_get(hba, UIC_ARG_MIB(PA_MAXRXHSGEAR), &pwr_info->gear_rx);
#else
	pwr_info->gear_rx = 0;
#endif
	if (!pwr_info->gear_rx) {
		ufshcd_dme_get(hba, UIC_ARG_MIB(PA_MAXRXPWMGEAR),
			       &pwr_info->gear_rx);
		if (!pwr_info->gear_rx) {
			dev_err(hba->dev, "%s: invalid max pwm rx gear read = %d\n",
				__func__, pwr_info->gear_rx);
			return -EINVAL;
		}
		pwr_info->pwr_rx = SLOW_MODE;
	}
#ifdef PWR_HS_MODE
	ufshcd_dme_peer_get(hba, UIC_ARG_MIB(PA_MAXRXHSGEAR),
			    &pwr_info->gear_tx);
#else
	pwr_info->gear_tx = 0;
#endif
	if (!pwr_info->gear_tx) {
		ufshcd_dme_peer_get(hba, UIC_ARG_MIB(PA_MAXRXPWMGEAR),
				    &pwr_info->gear_tx);
		if (!pwr_info->gear_tx) {
			dev_err(hba->dev, "%s: invalid max pwm tx gear read = %d\n",
				__func__, pwr_info->gear_tx);
			return -EINVAL;
		}
		pwr_info->pwr_tx = SLOW_MODE;
	}

#ifndef PWR_HS_MODE
	printf("force slow mode\n");
#endif

	hba->max_pwr_info.is_valid = true;
	return 0;
}

static int ufshcd_change_power_mode(struct ufs_hba *hba,
				    struct ufs_pa_layer_attr *pwr_mode)
{
	int ret;

	/* if already configured to the requested pwr_mode */
	if (pwr_mode->gear_rx == hba->pwr_info.gear_rx &&
	    pwr_mode->gear_tx == hba->pwr_info.gear_tx &&
	    pwr_mode->lane_rx == hba->pwr_info.lane_rx &&
	    pwr_mode->lane_tx == hba->pwr_info.lane_tx &&
	    pwr_mode->pwr_rx == hba->pwr_info.pwr_rx &&
	    pwr_mode->pwr_tx == hba->pwr_info.pwr_tx &&
	    pwr_mode->hs_rate == hba->pwr_info.hs_rate) {
		dev_dbg(hba->dev, "%s: power already configured\n", __func__);
		return 0;
	}

#ifdef PWR_G3
	pwr_mode->gear_tx = UFS_HS_G3;
	pwr_mode->gear_rx = UFS_HS_G3;
#endif

	/*
	 * Configure attributes for power mode change with below.
	 * - PA_RXGEAR, PA_ACTIVERXDATALANES, PA_RXTERMINATION,
	 * - PA_TXGEAR, PA_ACTIVETXDATALANES, PA_TXTERMINATION,
	 * - PA_HSSERIES
	 */
	ufshcd_dme_set(hba, UIC_ARG_MIB(PA_RXGEAR), pwr_mode->gear_rx);
	ufshcd_dme_set(hba, UIC_ARG_MIB(PA_ACTIVERXDATALANES),
		       pwr_mode->lane_rx);
	if (pwr_mode->pwr_rx == FASTAUTO_MODE || pwr_mode->pwr_rx == FAST_MODE)
		ufshcd_dme_set(hba, UIC_ARG_MIB(PA_RXTERMINATION), TRUE);
	else
		ufshcd_dme_set(hba, UIC_ARG_MIB(PA_RXTERMINATION), FALSE);

	ufshcd_dme_set(hba, UIC_ARG_MIB(PA_TXGEAR), pwr_mode->gear_tx);
	ufshcd_dme_set(hba, UIC_ARG_MIB(PA_ACTIVETXDATALANES),
		       pwr_mode->lane_tx);
	if (pwr_mode->pwr_tx == FASTAUTO_MODE || pwr_mode->pwr_tx == FAST_MODE)
		ufshcd_dme_set(hba, UIC_ARG_MIB(PA_TXTERMINATION), TRUE);
	else
		ufshcd_dme_set(hba, UIC_ARG_MIB(PA_TXTERMINATION), FALSE);

	if (pwr_mode->pwr_rx == FASTAUTO_MODE ||
	    pwr_mode->pwr_tx == FASTAUTO_MODE ||
	    pwr_mode->pwr_rx == FAST_MODE ||
	    pwr_mode->pwr_tx == FAST_MODE)
		ufshcd_dme_set(hba, UIC_ARG_MIB(PA_HSSERIES),
			       pwr_mode->hs_rate);

	ret = ufshcd_uic_change_pwr_mode(hba, pwr_mode->pwr_rx << 4 |
					 pwr_mode->pwr_tx);

	if (ret) {
		dev_err(hba->dev,
			"%s: power mode change failed %d\n", __func__, ret);

		return ret;
	}

	/* Copy new Power Mode to power info */
	memcpy(&hba->pwr_info, pwr_mode, sizeof(struct ufs_pa_layer_attr));

	return ret;
}
#endif

/**
 * ufshcd_verify_dev_init() - Verify device initialization
 *
 */
static int ufshcd_verify_dev_init(struct ufs_hba *hba)
{
	int retries;
	int err;

	for (retries = NOP_OUT_RETRIES; retries > 0; retries--) {
		err = ufshcd_exec_dev_cmd(hba, DEV_CMD_TYPE_NOP,
					  NOP_OUT_TIMEOUT);
		if (!err || err == -ETIMEDOUT)
			break;

		dev_dbg(hba->dev, "%s: error %d retrying\n", __func__, err);
	}

	if (err) {
		if (err == -ETIMEDOUT)
			sunxi_ufs_trace_point(SUNXI_UFS_TOUT);
		sunxi_ufs_trace_point(SUNXI_UFS_NOP_ERR);
		//dev_err(hba->dev, "%s: NOP OUT failed %d\n", __func__, err);
		dev_err(hba->dev, "NOP OUT failed %d\n", err);

	}
	return err;
}

/**
 * ufshcd_config_pwr_mode - configure a new power mode
 * @hba: per-adapter instance
 * @desired_pwr_mode: desired power configuration
 *
 * Return: 0 upon success; < 0 upon failure.
 */
int ufshcd_config_pwr_mode(struct ufs_hba *hba,
		struct ufs_pa_layer_attr *desired_pwr_mode)
{
	struct ufs_pa_layer_attr final_params = { 0 };
	int ret;

	ret = ufshcd_ops_pwr_change_notify(hba, PRE_CHANGE,
					desired_pwr_mode, &final_params);

	if (ret)
		memcpy(&final_params, desired_pwr_mode, sizeof(final_params));

	ret = ufshcd_change_power_mode(hba, &final_params);

	return ret;
}

/**
 * ufshcd_complete_dev_init() - checks device readiness
 */
static int ufshcd_complete_dev_init(struct ufs_hba *hba)
{
	int i;
	int err;
	bool flag_res = true;

	err = ufshcd_query_flag_retry(hba, UPIU_QUERY_OPCODE_SET_FLAG,
				      QUERY_FLAG_IDN_FDEVICEINIT, NULL);
	if (err) {
/*
		dev_err(hba->dev,
			"%s setting fDeviceInit flag failed with error %d\n",
			__func__, err);
*/
		dev_err(hba->dev,
			"setting fDeviceInit flag failed with error %d\n", err);
		goto out;
	}

	/* poll for max. 1000 iterations for fDeviceInit flag to clear */
	for (i = 0; i < 1000 && !err && flag_res; i++) {
		err = ufshcd_query_flag_retry(hba, UPIU_QUERY_OPCODE_READ_FLAG,
					      QUERY_FLAG_IDN_FDEVICEINIT,
					      &flag_res);
		if (!err && flag_res)
			mdelay(1);
	}

	if (err) {
/*
		dev_err(hba->dev,
			"%s reading fDeviceInit flag failed with error %d\n",
			__func__, err);
*/
		dev_err(hba->dev,
			"reading fDeviceInit flag failed with error %d\n", err);
	} else if (flag_res) {
		sunxi_ufs_trace_point(SUNXI_UFS_DEV_BUSY_ERR);
/*
		dev_err(hba->dev,
			"%s fDeviceInit was not cleared by the device\n",
			__func__);
*/
		dev_err(hba->dev,
			"fDeviceInit was not cleared by the device\n");
	}

out:
	return err;
}

static void ufshcd_def_desc_sizes(struct ufs_hba *hba)
{
	hba->desc_size.dev_desc = QUERY_DESC_DEVICE_DEF_SIZE;
	hba->desc_size.pwr_desc = QUERY_DESC_POWER_DEF_SIZE;
	hba->desc_size.interc_desc = QUERY_DESC_INTERCONNECT_DEF_SIZE;
	hba->desc_size.conf_desc = QUERY_DESC_CONFIGURATION_DEF_SIZE;
	hba->desc_size.unit_desc = QUERY_DESC_UNIT_DEF_SIZE;
	hba->desc_size.geom_desc = QUERY_DESC_GEOMETRY_DEF_SIZE;
	hba->desc_size.hlth_desc = QUERY_DESC_HEALTH_DEF_SIZE;
}

struct ufs_ref_clk {
	unsigned long freq_hz;
	enum ufs_ref_clk_freq val;
};

static const struct ufs_ref_clk ufs_ref_clk_freqs[] = {
	{19200000, REF_CLK_FREQ_19_2_MHZ},
	{26000000, REF_CLK_FREQ_26_MHZ},
	{38400000, REF_CLK_FREQ_38_4_MHZ},
	{52000000, REF_CLK_FREQ_52_MHZ},
	{0, REF_CLK_FREQ_INVAL},
};


static int ufshcd_set_dev_ref_clk(struct ufs_hba *hba)
{
	int err;
	u32 ref_clk;
	u32 freq = hba->dev_ref_clk_freq;


	err = ufshcd_query_attr_retry(hba, UPIU_QUERY_OPCODE_READ_ATTR,
			QUERY_ATTR_IDN_REF_CLK_FREQ, 0, 0, &ref_clk);

	if (err) {
		dev_err(hba->dev, "failed reading bRefClkFreq. err = %d\n",
			err);
		goto out;
	}

	if (ref_clk == freq) {
		dev_dbg(hba->dev, "bRefClkFreq in ufs is already to %lu Hz\n",
			ufs_ref_clk_freqs[freq].freq_hz);
		goto out; /* nothing to update */
	}

	err = ufshcd_query_attr_retry(hba, UPIU_QUERY_OPCODE_WRITE_ATTR,
			QUERY_ATTR_IDN_REF_CLK_FREQ, 0, 0, &freq);

	if (err) {
		dev_err(hba->dev, "bRefClkFreq setting to %lu Hz failed\n",
			ufs_ref_clk_freqs[freq].freq_hz);
		goto out;
	}

	dev_info(hba->dev, "bRefClkFreq setting to %lu Hz succeeded\n",
			ufs_ref_clk_freqs[freq].freq_hz);

out:
	return err;
}



int ufs_start(struct ufs_hba *hba)
{
#ifdef DES_READ_SUP
	struct ufs_dev_desc card = {0};
	memset(&card, 0, sizeof(struct ufs_dev_desc));
#endif
	int ret;

	ret = ufshcd_link_startup(hba);
	if (ret)
		return ret;

	ret = ufshcd_verify_dev_init(hba);
	if (ret)
		return ret;

	ret = ufshcd_complete_dev_init(hba);
	if (ret)
		return ret;
//#ifdef DES_READ_SUP
#if 0
	/* Init check for device descriptor sizes */
	ufshcd_init_desc_sizes(hba);

	ret = ufs_get_device_desc(hba, &card);
	if (ret) {
		dev_err(hba->dev, "%s: Failed getting device info. err = %d\n",
			__func__, ret);

		return ret;
	}
#endif
#ifdef PWR_MODE_CHANGE
	if (ufshcd_get_max_pwr_mode(hba)) {
		dev_err(hba->dev,
			"%s: Failed getting max supported power mode\n",
			__func__);
	} else {
		/*
		 * Set the right value to bRefClkFreq before attempting to
		 * switch to HS gears.
		 */
		if (hba->dev_ref_clk_freq != REF_CLK_FREQ_INVAL)
			ufshcd_set_dev_ref_clk(hba);

		ret = ufshcd_config_pwr_mode(hba, &hba->max_pwr_info.info);
		if (ret) {
			dev_err(hba->dev, "%s: Failed setting power mode, err = %d\n",
				__func__, ret);

			return ret;
		}

		//dev_info("Device at %s up at:", hba->dev->name);
		ufshcd_print_pwr_info(hba);
	}
#endif

#ifdef DES_READ_SUP
	/* Init check for device descriptor sizes */
	ufshcd_init_desc_sizes(hba);
#endif
	ret = ufshcd_logical_unit_config(hba, 0);
	if (ret)
		return ret;
	return 0;
}

int ufshcd_probe(struct udevice *ufs_dev, struct ufs_hba_ops *hba_ops)
{
	struct ufs_hba *hba = &(ufs_dev->uhba);
	int err;

	struct scsi_plat *scsi_plat;

	scsi_plat = &(ufs_dev->sc_plat);
	scsi_plat->max_id = UFSHCD_MAX_ID;
	scsi_plat->max_lun = UFS_MAX_LUNS;
	scsi_plat->max_bytes_per_req = UFS_MAX_BYTES;

	hba->dev = ufs_dev;
	hba->ops = hba_ops;
	hba->mmio_base = (void *)SUNXI_UFS_REGS_BASE;

	/* Set descriptor lengths to specification defaults */
	ufshcd_def_desc_sizes(hba);

	ufshcd_ops_init(hba);

	/* Read capabilties registers */
	hba->capabilities = ufshcd_readl(hba, REG_CONTROLLER_CAPABILITIES);

	/* Get UFS version supported by the controller */
	hba->version = ufshcd_get_ufs_version(hba);
	if (hba->version != UFSHCI_VERSION_10 &&
	    hba->version != UFSHCI_VERSION_11 &&
	    hba->version != UFSHCI_VERSION_20 &&
	    hba->version != UFSHCI_VERSION_21)
		dev_dbg(hba->dev, "invalid UFS version 0x%x\n",
			hba->version);

	/* Get Interrupt bit mask per version */
	hba->intr_mask = ufshcd_get_intr_mask(hba);

	/* Allocate memory for host memory space */
	err = ufshcd_memory_alloc(hba);
	if (err) {
		dev_err(hba->dev, "Memory allocation failed\n");
		return err;
	}

	/* Configure Local data structures */
	ufshcd_host_memory_configure(hba);

	/*
	 * In order to avoid any spurious interrupt immediately after
	 * registering UFS controller interrupt handler, clear any pending UFS
	 * interrupt status and disable all the UFS interrupts.
	 */
	ufshcd_writel(hba, ufshcd_readl(hba, REG_INTERRUPT_STATUS),
		      REG_INTERRUPT_STATUS);
	ufshcd_writel(hba, 0, REG_INTERRUPT_ENABLE);

	ufshcd_ops_device_reset(hba);

	err = ufshcd_hba_enable(hba);
	if (err) {
		dev_err(hba->dev, "Host controller enable failed\n");
		return err;
	}

	err = ufs_start(hba);
	if (err)
		return err;

	return 0;
}

void ufshcd_hba_exit(struct ufs_hba *hba)
{
	ufshcd_ops_device_exit(hba);
}

extern ulong scsi_read(struct udevice *dev, lbaint_t blknr, lbaint_t blkcnt,
		       void *buffer);
extern ulong scsi_write(struct udevice *dev, lbaint_t blknr, lbaint_t blkcnt,
			const void *buffer);


int  ufs_bind(struct udevice *udev)
{
	struct blk_ops *sc_bops =  udev->scsi_blk_ops;
	/*ufs hda*/
	ufs_hda_ops_bind(udev);
	/*ufs scsi*/
	udev->ufs_sc_ops.exec = ufs_scsi_exec;
	/*scsi blk*/
	//udev->bd = &gbd;
	sc_bops->read = scsi_read;
	sc_bops->write = scsi_write;
	//udev->scsi_blk_ops = &gscsi_blk_ops;
	return 0;
}

int  ufs_unbind(struct udevice *udev)
{
	/*ufs hda*/
	ufs_hda_ops_unbind(udev);
	/*ufs scsi*/
	udev->ufs_sc_ops.exec = NULL;
	/*scsi blk*/
	udev->bd = NULL;
	udev->scsi_blk_ops = NULL;
	return 0;
 }


/*
int ufs_scsi_bind(struct udevice *ufs_dev, struct udevice **scsi_devp)
{
	int ret = device_bind_driver(ufs_dev, "ufs_scsi", "ufs_scsi",
				     scsi_devp);

	return ret;
}

static struct scsi_ops ufs_ops = {
	.exec		= ufs_scsi_exec,
};

int ufs_probe_dev(int index)
{
	struct udevice *dev;

	return uclass_get_device(UCLASS_UFS, index, &dev);
}

int ufs_probe(void)
{
	struct udevice *dev;
	int ret, i;

	for (i = 0;; i++) {
		ret = uclass_get_device(UCLASS_UFS, i, &dev);
		if (ret == -ENODEV)
			break;
	}

	return 0;
}

U_BOOT_DRIVER(ufs_scsi) = {
	.id = UCLASS_SCSI,
	.name = "ufs_scsi",
	.ops = &ufs_ops,
};
*/
