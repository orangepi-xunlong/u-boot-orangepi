 /*
 * Copyright (c) 2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef HPNFC_IF_H
#define HPNFC_IF_H

#include "cdn_stdtypes.h"

/** @defgroup ConfigInfo  Configuration and Hardware Operation Information
 *  The following definitions specify the driver operation environment that
 *  is defined by hardware configuration or client code. These defines are
 *  located in the header file of the core driver.
 *  @{
 */

/**********************************************************************
* Defines
**********************************************************************/
/** size of descriptor in bytes */
#define	HPNFC_CDMA_DESC_SIZE 56U

/** size of descriptor containing dummy bytes. Size is in bytes */
#define	HPNFC_CDMA_DESC_WITH_DUMMY_SIZE 64U

/** current operation not finished yet */
#define	HPNFC_STATUS_PENDING 255U

/**
 *  @}
 */


/** @defgroup DataStructure Dynamic Data Structures
 *  This section defines the data structures used by the driver to provide
 *  hardware information, modification and dynamic operation of the driver.
 *  These data structures are defined in the header file of the core driver
 *  and utilized by the API.
 *  @{
 */

/**********************************************************************
 * Forward declarations
 **********************************************************************/
typedef struct HPNFC_ControllerInfo_s HPNFC_ControllerInfo;
typedef struct HPNFC_BchConfigInfo_s HPNFC_BchConfigInfo;
typedef struct HPNFC_Config_s HPNFC_Config;
typedef struct HPNFC_LunInterleavedConfig_s HPNFC_LunInterleavedConfig;
typedef struct HPNFC_DeviceSubLayout_s HPNFC_DeviceSubLayout;
typedef struct HPNFC_DeviceLayout_s HPNFC_DeviceLayout;
typedef struct HPNFC_StatusPollingConfig_s HPNFC_StatusPollingConfig;
typedef struct HPNFC_AutoStatusConfig_s HPNFC_AutoStatusConfig;
typedef struct HPNFC_TransferConfig_s HPNFC_TransferConfig;
typedef struct HPNFC_MultiplaneConfig_s HPNFC_MultiplaneConfig;
typedef struct HPNFC_CacheConfig_s HPNFC_CacheConfig;
typedef struct HPNFC_EccConfiguration_s HPNFC_EccConfiguration;
typedef struct HPNFC_EraseDetectConfig_s HPNFC_EraseDetectConfig;
typedef struct HPNFC_PhyConfig_s HPNFC_PhyConfig;
typedef struct HPNFC_GenReqInputAddress_s HPNFC_GenReqInputAddress;
typedef struct HPNFC_GenReqInputData_s HPNFC_GenReqInputData;
typedef union HPNFC_GenReqInput_s HPNFC_GenReqInput;
typedef struct HPNFC_OperationResult_s HPNFC_OperationResult;
typedef struct HPNFC_GenReq_s HPNFC_GenReq;
typedef struct HPNFC_PioReq_s HPNFC_PioReq;
typedef struct HPNFC_RstReq_s HPNFC_RstReq;
typedef struct HPNFC_CdmaReq_s HPNFC_CdmaReq;
typedef struct HPNFC_CdmaSyncConfig_s HPNFC_CdmaSyncConfig;
typedef struct HPNFC_CdmaCmd_s HPNFC_CdmaCmd;
typedef struct HPNFC_SysReq_s HPNFC_SysReq;
typedef union HPNFC_WorkModeSpecificRequest_s HPNFC_WorkModeSpecificRequest;
typedef struct HPNFC_CmdReq_s HPNFC_CmdReq;
typedef struct HPNFC_DebugReq_s HPNFC_DebugReq;
typedef struct HPNFC_DiConfig_s HPNFC_DiConfig;
typedef struct HPNFC_DiErrInjConfig_s HPNFC_DiErrInjConfig;
typedef struct HPNFC_DmaConfig_s HPNFC_DmaConfig;
typedef struct HPNFC_RemapRecord_s HPNFC_RemapRecord;
typedef struct HPNFC_PSlcConfig_s HPNFC_PSlcConfig;
typedef struct HPNFC_TimeoutConfig_s HPNFC_TimeoutConfig;
typedef struct HPNFC_DevInfo_s HPNFC_DevInfo;


/**********************************************************************
 * Enumerations
 **********************************************************************/
/** it defines CDMA command type */
typedef enum
{
	/** Read data transfer */
	HPNFC_DATA_TRANSFER_READ = 0U,
	/** Write data transfer */
	HPNFC_DATA_TRANSFER_WRITE = 1U,
} HPNFC_TransferDirection;

/** it defines CDMA command type */
typedef enum
{
	/** block erase command */
	HPNFC_CDMA_CMD_TYPE_ERASE = 1U,
	/** page program command */
	HPNFC_CDMA_CMD_TYPE_PROGRAM = 2U,
	/** page read command */
	HPNFC_CDMA_CMD_TYPE_READ = 3U,
	/** copy back command */
	HPNFC_CDMA_CMD_TYPE_COPY_BACK = 4U,
	/** reset command */
	HPNFC_CDMA_CMD_TYPE_RESET = 5U,
	/** nop command */
	HPNFC_CDMA_CMD_TYPE_NOP = 6U,
} HPNFC_CdmaCmdType;

/** it defines PIO command type */
typedef enum
{
	/** block erase command */
	HPNFC_PIO_CMD_TYPE_ERASE = 1U,
	/** page program command */
	HPNFC_PIO_CMD_TYPE_PROGRAM = 2U,
	/** page read command */
	HPNFC_PIO_CMD_TYPE_READ = 3U,
	/** copy back command */
	HPNFC_PIO_CMD_TYPE_COPY_BACK = 4U,
	/** reset command */
	HPNFC_PIO_CMD_TYPE_RESET = 5U,
	/** nop command */
	HPNFC_PIO_CMD_TYPE_SET_FEATURES = 7U,
} HPNFC_PioCmdType;

/** it defines Generic command type */
typedef enum
{
	/** execute command sequence */
	HPNFC_GEN_CMD_TYPE_CMD_SEQ = 0U,
	/** execute address sequence */
	HPNFC_GEN_CMD_TYPE_ADDR_SEQ = 1U,
	/** execute data sequence */
	HPNFC_GEN_CMD_TYPE_DATA_SEQ = 2U,
	/** execute read command */
	HPNFC_GEN_CMD_TYPE_READ = 3U,
	/** execute write command */
	HPNFC_GEN_CMD_TYPE_WRITE = 4U,
	/** execute reset command */
	HPNFC_GEN_CMD_TYPE_RESET = 5U,
	/** execute erase command */
	HPNFC_GEN_CMD_TYPE_ERASE = 6U,
	/** execute read status command */
	HPNFC_GEN_CMD_TYPE_STATUS = 7U,
	/** execute read enhanced status command */
	HPNFC_GEN_CMD_TYPE_ENHANCED_STATUS = 8U,
	/** execute read cache random command */
	HPNFC_GEN_CMD_TYPE_READ_CACHE = 9U,
	/** execute copy back read command */
	HPNFC_GEN_CMD_TYPE_COPYBACK_READ = 10U,
	/** execute copy back program command */
	HPNFC_GEN_CMD_TYPE_COPYBACK_WRITE = 11U,
	/** execute change read column command */
	HPNFC_GEN_CMD_TYPE_READ_COLUMN = 12U,
	/** execute change read column command enhanced */
	HPNFC_GEN_CMD_TYPE_READ_COLUMN_ENHANCED = 13U,
	/** execute change read column command JEDEC */
	HPNFC_GEN_CMD_TYPE_READ_COLUMN_JEDEC = 14U,
	/** execute multiplane read command */
	HPNFC_GEN_CMD_TYPE_MULTIPLANE_READ = 15U,
	/** execute multiplane erase command */
	HPNFC_GEN_CMD_TYPE_MULTIPLANE_ERASE = 16U,
	/** execute multiplane erase command ONFI JEDEC */
	HPNFC_GEN_CMD_TYPE_MULTIPLANE_ERASE2 = 17U,
	/** execute change write collumn command */
	HPNFC_GEN_CMD_TYPE_WRITE_COLUMN = 18U,
	/** execute small data move command */
	HPNFC_GEN_CMD_TYPE_SMALL_DATA_MOVE = 19U,
	/** execute synchronous reset */
	HPNFC_GEN_CMD_TYPE_SYNCHRONOUS_RESET = 20U,
	/** execute volume select */
	HPNFC_GEN_CMD_TYPE_VOLUME_SELECT = 21U,
	/** execute ODT configure */
	HPNFC_GEN_CMD_TYPE_ODT_CONFIGURE = 22U,
	/** execute set features command */
	HPNFC_GEN_CMD_TYPE_SET_FEATURES = 23U,
	/** execute get features command */
	HPNFC_GEN_CMD_TYPE_GET_FEATURES = 24U,
	/** execute LUN get features command */
	HPNFC_GEN_CMD_TYPE_LUN_GET_FEATURES = 25U,
	/** execute LUN set features command */
	HPNFC_GEN_CMD_TYPE_LUN_SET_FEATURES = 26U,
	/** execute read ID */
	HPNFC_GEN_CMD_TYPE_READ_ID = 27U,
	/** execute read parameter page */
	HPNFC_GEN_CMD_TYPE_READ_PARAM_PAGE = 28U,
	/** execute ZQ calibration short */
	HPNFC_GEN_CMD_TYPE_ZQ_CALIB_SHORT = 29U,
	/** execute ZQ calibration long */
	HPNFC_GEN_CMD_TYPE_ZQ_CALIB_LONG = 30U,
	/** execute LUN reset */
	HPNFC_GEN_CMD_LUN_RESET = 31U,
	/** execute Seed Initial */
	HPNFC_GEN_CMD_SEED_INIT = 32U,
} HPNFC_GenericCmdType;

/** it defines timing work mode */
typedef enum
{
	/** sdr mode (asynchronous) */
	HPNFC_WORK_MODE_SDR = 1U,
	/** NV DDR mode */
	HPNFC_WORK_MODE_NV_DDR = 2U,
	/** NV DDR2 mode */
	HPNFC_WORK_MODE_NV_DDR2 = 3U,
	/** NV DDR3 command */
	HPNFC_WORK_MODE_NV_DDR3 = 4U,
	/** toggle mode */
	HPNFC_WORK_MODE_TOGG = 5U,
} HPNFC_WorkMode;

/** it defines DMA mode */
typedef enum
{
	/** Slave DMA mode */
	HPNFC_DMA_MODE_SLAVE = 0U,
	/** Master DMA mode */
	HPNFC_DMA_MODE_MASTER = 1U,
} HPNFC_DmaMode;

/** it defines operation status */
typedef enum
{
	/** operation complete without error */
	HPNFC_STATUS_NO_ERROR = 0U,
	/** device error detected during read status operation */
	HPNFC_STATUS_DEV_ERR = 1U,
	/** operation failed to complete successfully because of a time out scenario */
	HPNFC_STATUS_TIMEOUT_ERR = 2U,
	/** correctable ECC error was detected. */
	HPNFC_STATUS_ECC_CORR_ERR = 3U,
	/** uncorrectable ECC error was detected. */
	HPNFC_STATUS_ECC_UNCORR_ERR = 4U,
	/** invalid command/descriptor was detected */
	HPNFC_STATUS_CMD_ERR = 5U,
	/** command was not executed at all or it was interrupted */
	HPNFC_STATUS_CMD_UNFINISHED = 6U,
	/** Incorrect number of the DQS pulses */
	HPNFC_STATUS_DQS_ERR = 7U,
	/** Error response on the system bus */
	HPNFC_STATUS_BUS_ERR = 8U,
	/** Parity error during reading from context memory */
	HPNFC_STATUS_PARITY_CTX_ERR = 9U,
	/** Parity error during reading the descriptor */
	HPNFC_STATUS_PARITY_DSC_ERR = 10U,
	/** Parity or CRC error during data transmission */
	HPNFC_STATUS_CRC_DAT_ERR = 11U,
} HPNFC_OperationStatus;

/** command request type */
typedef enum
{
	/** CDMA request */
	HPNFC_REQ_TYPE_CDMA = 0U,
	/** PIO request */
	HPNFC_REQ_TYPE_PIO = 1U,
	/** Generic request */
	HPNFC_REQ_TYPE_GEN = 2U,
	/** Thread reset request */
	HPNFC_REQ_TYPE_RST = 3U,
} HPNFC_ReqType;

/** LUN status select */
typedef enum
{
	/** the 78-addr sequence will be use for both first and following LUN */
	HPNFC_LUN_STAT_SELECT_78 = 1U,
	/** the F1 command will be used for a first LUN and F2 will be used for second one. */
	HPNFC_LUN_STAT_SELECT_F1_F2 = 2U,
} HPNFC_LunStatSelect;

/** sync start condition for CDMA operation */
typedef enum
{
	/** the start condition is the sync buffer flag must be equal or greater than start value */
	HPNFC_CDMA_SYNC_CFG_SC_EQUAL = 1U,
	/** the start condition is the sync buffer flag must be equal to the start value */
	HPNFC_CDMA_SYNC_CFG_SC_GREATER = 2U,
} HPNFC_CdmaSyncStartCond;

/** operation to be made after operation described by descriptor finished */
typedef enum
{
	/** increment the sync buffer flag at the end cdma operation */
	HPNFC_CDMA_SYNC_CFG_EO_INCREMENT = 1U,
	/** store the end value to the sync buffer flag at the end cdma operation */
	HPNFC_CDMA_SYNC_CFG_EO_STORE = 2U,
} HPNFC_CdmaSyncEndOp;

/** operation to be made after operation described by descriptor finished */
typedef enum
{
	/** no error detected on boot operation */
	HPNFC_BOOT_STATUS_OK = 0U,
	/** correctable ECC error was detected */
	HPNFC_BOOT_STATUS_ECC_CORR_ERR = 1U,
	/** uncorrectable ECC error was detected */
	HPNFC_BOOT_STATUS_ECC_UNCORR_ERR = 2U,
	/** DMA master AXI interface receiving an error response from the target */
	HPNFC_BOOT_STATUS_AXI_ERR = 3U,
	/** the boot process fails due to dfi_dqs_underrun error */
	HPNFC_BOOT_STATUS_PHY_ERR = 4U,
	/** the boot module finished work without data copy */
	HPNFC_BOOT_STATUS_INIT_ERR = 5U,
} HPNFC_BootStatus;

/** It defines mode in which debug data are read */
typedef enum
{
	/** debug data are non synchronized */
	HPNFC_DBG_SYNCH_MODE_NON_SYNCH = 1U,
	/** debug data are synchronized by NF_CLK */
	HPNFC_DBG_SYNCH_MODE_SYNCH_BY_NF_CLK = 2U,
	/** debug data are synchronized by BCH_CLK */
	HPNFC_DBG_SYNCH_MODE_SYNCH_BY_BCH_CLK = 3U,
} HPNFC_DebugSynchMode;

/** It defines Debug access interface type */
typedef enum
{
	/** debug interface is accessible by registers */
	HPNFC_DBG_IF_TYPE_REGISTER = 1U,
	/** debug interface is accessible by hardware */
	HPNFC_DBG_IF_TYPE_HARDWARE = 0U,
} HPNFC_DebugIfType;

/** It defines parity type for host and SRAM interfaces */
typedef enum
{
	/** even parity type */
	HPNFC_DI_PARITY_TYPE_EVEN = 1U,
	/** odd parity type */
	HPNFC_DI_PARITY_TYPE_ODD = 2U,
} HPNFC_DiParityType;

/** It defines error injection type */
typedef enum
{
	/** one time injection */
	HPNFC_DI_ERR_INJ_TYPE_ONE_TIME = 1U,
	/** continuous injection */
	HPNFC_DI_ERR_INJ_TYPE_CONT = 2U,
} HPNFC_DiErrInjType;

/** It defines number of planes */
typedef enum
{
	/** one plane only */
	HPNFC_PLANES_NUMBER_1 = 1U,
	/** two planes */
	HPNFC_PLANES_NUMBER_2 = 2U,
	/** four planes */
	HPNFC_PLANES_NUMBER_4 = 4U,
	/** eight planes */
	HPNFC_PLANES_NUMBER_8 = 8U,
} HPNFC_PlanesNumber;

/** It defines multiplane program sequence */
typedef enum
{
	/** 80-Addr5-Data-11 repeated for planes 0 to N-2 ... 80-Addr5-Data-10 for plane N-1 */
	HPNFC_MPL_PRG_SEQ_0 = 0U,
	/** 80-Addr5-Data-11 repeat for planes 0 to N-2... 81-Addr5-Data-10 for plane N-1 */
	HPNFC_MPL_PRG_SEQ_1 = 1U,
	/** 80-Addr5-Data-11 or plane 0 ... 81-Addr5-Data-11 for planes 1 to N-2... 81-Addr5-Data-10 for plane N-1 */
	HPNFC_MPL_PRG_SEQ_2 = 2U,
} HPNFC_MplPrgSeq;

/** It defines multiplane read sequence */
typedef enum
{
	/** 00-Addr5_0-32, 00-Addr5_1-32, ... ,00-Addr5_n-30, 06-Addr5_0-E0-Data ... */
	HPNFC_MPL_RD_SEQ_0 = 0U,
	/** 00-Addr5/2_0-32, 00-Addr5_1-32, ... ,00-Addr5_n-30, 05-Addr5/2_0-E0-Data */
	HPNFC_MPL_RD_SEQ_1 = 1U,
	/** 60-Addr3_0-60-Addr3_1 , ..., 60-Addr3_n-30, 00-Addr5_0-05-Addr2-E0-DATA ... */
	HPNFC_MPL_RD_SEQ_2 = 2U,
	/** 00-Add5_0-32, 00-Addr5_1-32, ... ,00-Addr5_n-30, 00-Addr5_0-05-Addr2-E0 ... */
	HPNFC_MPL_RD_SEQ_4 = 4U,
	/** 60-Addr3_0-60-Addr3_1 , ..., 60-Addr3_n-C30/C33, 00-Addr5_0-05-Addr2_0-E0-DATA ... 30 */
	HPNFC_MPL_RD_SEQ_6 = 6U,
} HPNFC_MplRdSeq;

/** It defines multiplane erase sequence */
typedef enum
{
	/** ONFI sequence 60-Addr3-d1 60-Addr3-d0 */
	HPNFC_MPL_ERASE_SEQ_0 = 0U,
	/** JEDEC sequence 60-Addr3-60-Addr3-d0 */
	HPNFC_MPL_ERASE_SEQ_1 = 1U,
} HPNFC_MplEraseSeq;

/** It defines multiplane copyback write operation sequence */
typedef enum
{
	/** used sequence is 85-Addr5-11 ... 85-Addr5-11 ... 85-Addr5-10 */
	HPNFC_LAST_WR_CMD_SEQ_0 = 0U,
	/** 85-Addr5-11 ... 81-Addr5-11 ... 81-Addr5-10 */
	HPNFC_LAST_WR_CMD_SEQ_1 = 1U,
} HPNFC_LastWrCmdSeq;

/** It defines Multiplane copyback read command sequence */
typedef enum
{
	/** used sequence is 00-Addr5-32, ...., 00-Addr5-35 */
	HPNFC_COPY_BCK_CMD_SEQ_0 = 0U,
	/** used sequence is 60-Addr3-60-Addr3-35, 00-Addr5-05-Addr2-e0 */
	HPNFC_COPY_BCK_CMD_SEQ_1 = 1U,
} HPNFC_CopyBckCmdSeq;

/** It defines how many NAND flash chips are present in one package */
typedef enum
{
	/** single device per package */
	HPNFC_TARGET_PER_PACKAGE_ONE = 1U,
	/** two devices per package */
	HPNFC_TARGET_PER_PACKAGE_TWO = 2U,
	/** four devices per package */
	HPNFC_TARGET_PER_PACKAGE_FOUR = 4U,
	/** eight devices per package */
	HPNFC_TARGET_PER_PACKAGE_EIGHT = 8U,
} HPNFC_TargetPerPackage;

/** It defines NAND flash memory type */
typedef enum
{
	/** ONFI compliant device */
	HPNFC_DEV_TYPE_ONFI = 1U,
	/** JEDEC (Toggle) compliant device */
	HPNFC_DEV_TYPE_JEDEC = 2U,
} HPNFC_DevType;

/**********************************************************************
 * Callbacks
 **********************************************************************/
/** Callback for slave dma interrupt handling. */
typedef void (*HPNFC_SlaveDmaHandler)(void* privateData, void* dataPtr, HPNFC_TransferDirection transferDirection, uint32_t sdmaSize, uint32_t* transferredData);

/**********************************************************************
 * Structures and unions
 **********************************************************************/
/** Structure defining all configuration parameters applied on driver start-up */
struct HPNFC_ControllerInfo_s
{
	/** Controller revision number. */
	uint8_t ctrlRev;
	/** Fixed number (minor revision number). */
	uint8_t ctrlFix;
	/** Support for 16b NF interface. */
	uint32_t support16b:1;
	/** Support for NV-DDR2/3 work mode */
	uint32_t ddr2Support:1;
	/** Support for NV-DDR work mode */
	uint32_t ddrSupport:1;
	/** Support for asynchronous work mode */
	uint32_t asyncSupport:1;
	/** Maximum number of banks supported by hardware. This is an encoded value. */
	uint8_t banksNumber;
	/** Slave and Master DMA data width is 64 bit */
	uint32_t dmaData64bit:1;
	/** Slave and Master DMA address width is 64 bit */
	uint32_t dmaAddr64bit:1;
	/** Data ECC protection engine present. */
	uint32_t eccAvailable:1;
	/** Boot feature present. */
	uint32_t bootAvailable:1;
	/** Availability of pre-fetching mechanism */
	uint32_t preFetch:1;
	/** Availability of Data Integrity mechanism. */
	uint32_t dataIntergity:1;
	/** Availability Extended Command Feature. */
	uint32_t extCmdCnt:1;
	/** Availability of remap mechanism */
	uint32_t remap:1;
	/** Availability Extended Status feature. */
	uint32_t extStatus:1;
	/** Availability of dummy descriptor read feature. */
	uint32_t dummyDescRead:1;
	/** Number of threads available in the controller. */
	uint8_t threadsNumber;
};

/** Structure defines bch configuration */
struct HPNFC_BchConfigInfo_s
{
	/** Supported BCH correction capabilities. Is sorted from smallest to biggest correction capabilities. */
	uint8_t corrCaps[8];
	/** Supported BCH correction capabilities count */
	uint8_t numberOfCorrCaps;
	/** Supported BCH sector sizes */
	uint32_t sectorSizes[2];
};

/** Structure defining all configuration parameters applied on driver start-up */
struct HPNFC_Config_s
{
	/** base address of HPNFC registers */
	uint32_t regBase;
	/** Slave DMA handler, passes info about data size and thread number. */
	HPNFC_SlaveDmaHandler slaveDmaHandler;
	/** NAND flash device number connected to controller */
	uint8_t devNum;
	/**
	 * HW configuration of BCH, it is used only if driver cannot obtain this information
	 * from controller registers.
	*/
	HPNFC_BchConfigInfo* bchCfgInfo;
};

/** Structure defines interleaved operation configuration */
struct HPNFC_LunInterleavedConfig_s
{
	/** Bit in ROW address used for selecting of the LUN */
	uint8_t lunAddrOff;
	/** LUN status select */
	HPNFC_LunStatSelect lunStatSelect;
	/**
	 * This bit informs the controller if the device supports a
	 *  program operation on a LUN while a Read operation is
	 *  already ongoing in the other LUN.
	*/
	uint8_t programAferRead:1;
	/** enable multi LUN interleaved operations */
	uint8_t multiLunEn:1;
};

/** Structure defines NAND flash memory sublayout. Only this part of layout which is writable. */
struct HPNFC_DeviceSubLayout_s
{
	/** The number of LUN presents in the device. Up to 8 LUNs are supported. */
	uint8_t lunNumber;
	/** pages per block - number of pages in a block */
	uint16_t pagesPerBlock;
};

/** Structure defines NAND flash memory layout */
struct HPNFC_DeviceLayout_s
{
	/** Main area size in bytes for the NF device page */
	uint16_t mainSize;
	/** Spare area size in bytes for the NF device page */
	uint16_t spareSize;
	/** The number of LUN presents in the device. Up to 8 LUNs are supported. */
	uint8_t lunNumber;
	/** pages per block - number of pages in a block */
	uint16_t pagesPerBlock;
	/** number of blocks per LUN */
	uint32_t blocksPerLun;
	/** number of planes */
	uint8_t numberOfPlanes;

	uint16_t blockShift;
};

/** Status polling configuration. */
struct HPNFC_StatusPollingConfig_s
{
	/**
	 * Number of system clock cycles after issue of erase/write/
	 * read operation before the controller starts to poll for status.
	 * This values is valid only in the status polling mode.
	*/
	uint16_t longPolling;
	/**
	 * In the status polling mode, sets the number of cycles the
	 * controller must wait before checking the status register.
	*/
	uint16_t shortPolling;
	/**
	 * the mask used to comparison of response from the NF memory to the
	 * device status command and readyValue.
	*/
	uint8_t readyMask;
	/**
	 * value which will be compared to response from the NF memory to the
	 * device status command
	*/
	uint8_t readyValue;
	/** Selects R/B pin checks (if 1) or status polling mode (if 0). */
	uint8_t disableStatusPolling;
};

/** Auto status configurations. It is used to automatic read and interpret a device status by controller */
struct HPNFC_AutoStatusConfig_s
{
	/**
	 * the mask used to comparison of response from the NF memory to the operation status
	 * command and status_value
	*/
	uint8_t errorMask;
	/**
	 * value which will be compared to response from the NF memory to the operation status
	 * command
	*/
	uint8_t errorValue;
};

/** Structure defines transfer configuration for program and read pages operations. */
struct HPNFC_TransferConfig_s
{
	/** Offset value from the beginning of the page, used for data  transfer. */
	uint16_t offset;
	/** Numbers of sectors which will be transferred within single NF device page. */
	uint8_t sectorCount;
	/** Size of not-last data sector */
	uint16_t sectorSize;
	/** Size of last data sector */
	uint16_t lastSectorSize;
};

/** Structure contains multiplane operations configuration */
struct HPNFC_MultiplaneConfig_s
{
	/** multiplane write enable */
	uint8_t mplWrEn;
	/** multiplane read enable */
	uint8_t mplRdEn;
	/** multiplane program sequence, it can be 0, 1, 2 */
	HPNFC_MplPrgSeq mplPrgSeq;
	/** multiplane read sequence, it can be 0, 1, 2, 4, 6 */
	HPNFC_MplRdSeq mplRdSeq;
	/** multiplane erase sequence, it can be 0, 1 */
	HPNFC_MplEraseSeq mplEraseSeq;
	/** multiplane last write command sequence, it can be 0, 1 */
	HPNFC_LastWrCmdSeq lastWrCmd;
	/** multiplane copy back command sequence, it can be 0, 1 */
	HPNFC_CopyBckCmdSeq copyBckCmd;
	/**
	 * select status probing mechanism,
	 * 0 - summary status for all planes
	 * 1 - separate status per each plane
	*/
	uint8_t extStatusEn;
	/** number of planes per device it can be 1, 2, 4, 8 */
	HPNFC_PlanesNumber planesNumber;
};

/** Structure contains cache operation configuration */
struct HPNFC_CacheConfig_s
{
	/** cache read command sequences support */
	uint8_t cacheRdEn:1;
	/** cache write command sequences support */
	uint8_t cacheWrEn:1;
};

/** Structure defines ECC configuration */
struct HPNFC_EccConfiguration_s
{
	/** ECC enabled. Used for enabling and disabling ECC check bits generation and correction. */
	uint8_t eccEnabled:1;
	/** scrambler enabled. Used for enabling/disabling scrambler logic in the controller. */
	uint8_t scramblerEnabled:1;
	/** ECC correction capability */
	uint8_t corrCap;
	/** bad block marker - skip bytes offset */
	uint32_t skipBytesOffset;
	/** bad block marker - number of bytes to skip, 0 means no bytes will be skipped */
	uint8_t skipBytes;
	/** bad block marker - 16bit value that will be written in the spare area skip bytes. */
	uint16_t marker;
};

/** Structure defines erase detection configuration */
struct HPNFC_EraseDetectConfig_s
{
	/** erase detection enable */
	uint8_t eraseDetectEnable:1;
	/** erase detection level */
	uint16_t eraseDetectLevel;
};

/** Structure defining PHY configuration */
struct HPNFC_PhyConfig_s
{
	/** global control settings for the PHY. */
	uint32_t phyCtrl;
	/** global control settings for the termination selects for reads. */
	uint32_t phyTsel;
	/** the DQ related timing configuration */
	uint32_t phyDqTiming;
	/** the DQS related timing configuration */
	uint32_t phyDqsTiming;
	/** gate and loopback control related timing configuration */
	uint32_t phyGateLpbkCtrl;
	/** master DLL logic configuration */
	uint32_t phyDllMasterCtrl;
	/** slave DLL logic configuration */
	uint32_t phyDllSlaveCtrl;
	/** NV-DDR/ToggleMode/NV-DDR2/NV-DDR3 timings configuration */
	uint32_t toggleTimings0;
	/** NV-DDR/ToggleMode/NV-DDR2/NV-DDR3 timings configuration */
	uint32_t toggleTimings1;
	/** NV-DDR/ToggleMode/NV-DDR2/NV-DDR3 and SDR timings configuration */
	uint32_t asyncToggleTimings;
	/** Global timings configuration - register 0. */
	uint32_t timings0;
	/** Global timings configuration - register 1. */
	uint32_t timings1;
	/** Global timings configuration - register 2. */
	uint32_t timings2;
	/** Configuration of the resynchronization of slave DLL of PHY. */
	uint32_t dllPhyCtrl;
};

/** input for address sequence */
struct HPNFC_GenReqInputAddress_s
{
	/** address bytes to send */
	uint64_t address;
	/** number of addres bytes to send */
	uint8_t addrSize;
};

/** input for data sequence */
struct HPNFC_GenReqInputData_s
{
	/** correction capability */
	uint8_t corrCap;
	/** number of data to transfer in the last sector. */
	uint16_t lastSectorSize;
	/** number of data to transfer of the n-1 sectors except the last one */
	uint16_t sectorSize;
	/** defines the number of sectors to transfer within a single sequence */
	uint8_t sectorCount;
	/** enable erase page detection */
	uint8_t erasePageDetectEn:1;
	/** enable ECC support */
	uint8_t eccEn:1;
	/** enable ECC scrambler */
	uint8_t scramblerEnabled:1;
	/** data tranfser direction */
	HPNFC_TransferDirection transferDirection;
	/** Data pointer. Used for slave DMA. */
	void* dataPtr;
	/** Number of bytes transferred. */
	uint32_t transferredData;
};

/** Generic request input field */
union HPNFC_GenReqInput_s
{
	/** input for command sequence */
	uint16_t cmdSeq;
	/** input for address sequence */
	HPNFC_GenReqInputAddress addrSeq;
	/** input for data sequence */
	HPNFC_GenReqInputData dataSeq;
	/** input for commands which need only address parameter. */
	uint64_t addr;
	/** input for read status command it F2 enable */
	uint8_t readStatusF2En;
	/** input value for Seed Initial command */
	uint32_t seedValue;
};

/** Structure contains CDMA operation status */
struct HPNFC_OperationResult_s
{
	/** controller detected an erased page in the read transaction */
	uint8_t erased;
	/** maximum amount of correction applied to one ECC sector */
	uint8_t maxCorrErr;
	/** status of operation */
	HPNFC_OperationStatus status;
	/** operation number where the first error was detected */
	uint8_t errIdx;
	/**
	 * Each bit of this field inform about device error status
	 * for the corresponding plane
	*/
	uint8_t planesDeviceError;
	/**
	 * Each bit of this field inform about ECC error status
	 * for the corresponding plane.
	*/
	uint8_t planesEccError;
};

/** Structure contains PIO request */
struct HPNFC_GenReq_s
{
	/** Additional values required in command sequence. */
	HPNFC_GenReqInput input;
	/** ChipSelect. This field informs the minicotroller on which memory device the sequence need to be executed */
	uint8_t chipSelect;
	/** is command complaint with Jedec spec */
	uint8_t jedecSupp:1;
	/** wait for tWB after sending the last CMD/ADDR/DATA in the sequence. */
	uint8_t twbActive:1;
	/** Destination bank number. It used only for copy back operation. */
	HPNFC_GenericCmdType cmdType;
	/** Result of execution generic request. */
	HPNFC_OperationResult operationResult;
};

/** Structure contains PIO request */
struct HPNFC_PioReq_s
{
	/**
	 * DMA mode selection master or slave. If selected command
	 * does not require data transfer, this bit will be ignored
	*/
	HPNFC_DmaMode dmaMode;
	/** System/host memory address required for Data DMA commands - (page program and page read) */
	void* memPtr;
	/** Bank number. For copy back operation it is source bank number */
	uint8_t bank;
	/** It is device row address. For copy back operation it is row address */
	uint32_t rowAddr;
	/** Volume is an appointed address to a NAND Target. */
	uint8_t volume;
	/** Destination bank number. It used only for copy back operation. */
	uint8_t dstBank;
	/** It is destination device row address. It used only for copy back operation */
	uint32_t dstRowAddr;
	/** Command type */
	HPNFC_PioCmdType cmdType;
	/** It is feature address for set feature command */
	uint8_t featureAddr;
	/** It is feature value for set feature command */
	uint32_t featureValue;
	/** Result of execution PIO request. */
	HPNFC_OperationResult operationResult;
	/** Command count (for example for read command how many sequential pages should be read) */
	uint32_t cmdCount;
};

/** Structure contains PIO request */
struct HPNFC_RstReq_s
{
	/** reset type, can be 0 or 1. Reset type 0 is suitable only to reset CDMA */
	uint8_t resetType:1;
	/**
	 * request wich currently is executed on thread to reset
	 * it is internaly used field
	*/
	HPNFC_CmdReq* abortingRequest;
};

/** Structure contains CDMA chain request */
struct HPNFC_CdmaReq_s
{
	/** Pointer to first CDMA descriptor (address accessible by CPU) */
	void* descBuf;
	/** Pointer to first CDMA descriptor (address accessible by DMA engine) */
	uintptr_t descDmaAddr;
};

/** configuration of CDMA command sync functionality */
struct HPNFC_CdmaSyncConfig_s
{
	/** Address pointer to sync buffer location */
	uintptr_t syncBuffAddr;
	/** Start value of the sync buffer flag. The value required to start an operation */
	uint8_t startValue;
	/**
	 * Value to be stored to the sync buffer flag upon the successful completion of the
	 * operation unless the type is set to Increment
	*/
	uint8_t endValue;
	/**
	 * condition to meet before start executing operation described by descriptor
	 * HPNFC_CDMA_SYNC_CFG_SC_EQUAL - the start condition is the sync buffer flag must be equal to the startValue
	 * HPNFC_CDMA_SYNC_CFG_SC_GREATER - the start condition is the sync buffer flag must be equal or greater than the startValue
	*/
	HPNFC_CdmaSyncStartCond startCondition;
	/**
	 * operation to be made after operation described by descriptor finished
	 * HPNFC_CDMA_SYNC_CFG_EO_INCREMENT - the type is to increment the sync buffer flag and the endValue is ignored
	 * HPNFC_CDMA_SYNC_CFG_EO_STORE - the type is to store the endValue to the sync buffer flag
	*/
	HPNFC_CdmaSyncEndOp endOperation;
	/** enable or disable sync functionality. 1 - enable, 0 - disable */
	uint8_t syncEnable;
};

/** Structure contains one CDMA command used to assemble one CDMA descriptor */
struct HPNFC_CdmaCmd_s
{
	/** Next descriptor address. If there is no next descriptor address set to 0. */
	uintptr_t nextPtr;
	/**
	 * DMA mode selection master or slave. If selected command
	 * does not require data transfer, this bit will be ignored
	*/
	HPNFC_DmaMode dmaMode;
	/** System/host memory address required for Data DMA commands - (page program and page read) */
	void* memPtr;
	/** Bank number. For copy back operation it is source bank number */
	uint8_t bank;
	/** It is device row address. For copy back operation it is row address */
	uint32_t rowAddr;
	/** Volume is an appointed address to a NAND Target. */
	uint8_t volume;
	/** Destination bank number. It used only for copy back operation. */
	uint8_t dstBank;
	/** It is destination device row address. It used only for copy back operation */
	uint32_t dstRowAddr;
	/** Command count (for example for read command how many sequential pages should be read) */
	uint32_t cmdCount;
	/** Command type */
	HPNFC_CdmaCmdType cmdType;
	/**
	 * "If set continue addressing for the flash pointer  \n
	 * instead initialize it using value in the system memory."
	*/
	uint8_t flashPtrCont:1;
	/**
	 * "If set continue addressing for the memory pointer  \n
	 * instead initialize it using value in the system memory."
	*/
	uint8_t memPtrCont:1;
	/** sync configuration for this command */
	HPNFC_CdmaSyncConfig syncConfig;
};

/** Structure used for returning memory allocation size for private data and design configuration required by the driver */
struct HPNFC_SysReq_s
{
	/** size of memory required for HPNFC_PrivateData struct */
	uint32_t pDataSize;
};

/** Union groups all supported requests to one type */
union HPNFC_WorkModeSpecificRequest_s
{
	/** CDMA request */
	HPNFC_CdmaReq cdma;
	/** Pio request */
	HPNFC_PioReq pio;
	/** Generic request */
	HPNFC_GenReq generic;
	/** Reset thread request */
	HPNFC_RstReq reset;
};

/**
 * Structure contains request to execute on HPNFC flash controller.
 *  It can be used to execute command in CDMA, PIO, and Generic work modes.
*/
struct HPNFC_CmdReq_s
{
	/** status of operation */
	int32_t status;
	/** thread number for command. */
	uint8_t thread;
	/** contains all supported requests types(CDMA, PIO and generic). */
	HPNFC_WorkModeSpecificRequest req;
	/** Request type */
	HPNFC_ReqType reqType;
};

/** Structure contains request to get debug data by HPNFC Debug Interface using register interface */
struct HPNFC_DebugReq_s
{
	/** Field select with word of the debug vector to read */
	uint8_t wordSel;
	/** Debug synchronize mode in which debug data are read */
	HPNFC_DebugSynchMode synchMode;
	/** read debug data */
	uint32_t data;
};

/** Configuration of data integrity feature */
struct HPNFC_DiConfig_s
{
	/** Field select parity type */
	HPNFC_DiParityType parityType;
	/**
	 * Enable error response generation on SFR interface when parity error detected.
	 * 1 - enabled, 0- disabled
	*/
	uint32_t errorRespEn:1;
	/** Enable CRC checking. */
	uint32_t crcEn:1;
	/** Enable parity checking. */
	uint32_t parityEn:1;
};

/** Configuration of error injection */
struct HPNFC_DiErrInjConfig_s
{
	/** Field select error injection type */
	HPNFC_DiErrInjType errInjType;
	/** Inject error for data of context memory. */
	uint8_t injCtxValue;
	/** Inject error for address of context memory */
	uint32_t injCtxAddr:1;
	/** Inject error on address for descriptor read */
	uint32_t injDescRdAddr:1;
	/** Inject error on address for descriptor status/sync flag write */
	uint32_t injDescWrAddr:1;
	/** Inject error on address for descriptor status/sync flag write. */
	uint32_t injDescVal:4;
	/** Inject error for register read value. */
	uint32_t injRegVal:4;
	/** Inject error on address for read data. */
	uint32_t injDataRdAddr:1;
	/** Inject error on address for write data. */
	uint32_t injDataWrAddr:1;
	/** Inject CRC error for write data path. */
	uint32_t injDataWrCrc:1;
	/** Inject parity error for read data path. */
	uint32_t injDataParityRd:4;
	/** Inject error for Remap SPRAM memory address */
	uint32_t injRmpAddr:1;
	/** Inject error for Remap SPRAM memory (write data). */
	uint32_t injRmpVal:7;
};

/** Configuration of DMA */
struct HPNFC_DmaConfig_s
{
	/**
	 * Number of 64 bit data words which will be transfered after
	 *  trig level condition is met
	*/
	uint16_t dmaPackSizeInWords;
	/** Trigger level in terms of 64 bit data words. */
	uint16_t trigLvlInWords;
};

/** single record of remap table */
struct HPNFC_RemapRecord_s
{
	/** logical address */
	uint32_t logicAddr;
	/** physical address */
	uint32_t physAddr;
};

/** configuration of pSLC */
struct HPNFC_PSlcConfig_s
{
	/** bit index at which block address starts inside the row address */
	uint8_t blkAddrIdx;
	/** enable 0xA2 prefix command sending for the TLC devices working in the pSLC mode */
	uint8_t pSlcPrefixEn;
};

/** configuration of ZQ calibration */
struct HPNFC_ZqCalConfig_s
{
	/**
	 * this bit informs controller what type of recalibration command needs to be sent after recalibration for
	 * selected target was enabled and next was disabled
	*/
	uint8_t toggImplLcal;
	/** defines how many NAND flash chips are present in single package conected to the controller */
	HPNFC_TargetPerPackage targetPerPackage;
	/** The cyclic re-calibration using the ZQ Calibration Short */
	uint8_t shortCalEnable;
	/** The cyclic re-calibration using the ZQ Calibration Long */
	uint8_t longCalEnable;
	/**
	 * A timing window defined for the ZQ Calibration Short in with
	 * the NAND flash device need to perform a full calibration and
	 * transfer status values.
	*/
	uint16_t tZqcs;
	/**
	 * A timing window defined for the ZQ Calibration Long in with
	 * the NAND flash device need to perform a full calibration and
	 * transfer status values.
	*/
	uint16_t tZqcl;
	/**
	 * Time value after with the ZQ Calibration Long operation will
	 * be repeated.
	*/
	uint32_t longCalPeriod;
	/**
	 * Time value after with the ZQ Calibration Short operation will
	 * be repeated.
	*/
	uint32_t shortCalPeriod;
};

/** timeout configuration feature */
struct HPNFC_TimeoutConfig_s
{
	/** timeout value */
	uint32_t timeout;
	/** enable timeout feature */
	uint8_t timeoutEn;
};

/** NAND flash memory basic information */
struct HPNFC_DevInfo_s
{
	/** NAND flash memory manufacturer ID */
	uint8_t manufacturerId;
	/** NAND flash memory device ID */
	uint8_t deviceId;
	/** NAND flash memory device ID */
	HPNFC_DevType devType;
	/** Number of bits per cell. */
	uint8_t bitsPerCell;
	/** NAND flash memory layout */
	HPNFC_DeviceLayout devLayout;
};

/**
 *  @}
 */

/** @defgroup DriverObject Driver API Object
 *  API listing for the driver. The API is contained in the object as
 *  function pointers in the object structure. As the actual functions
 *  resides in the Driver Object, the client software must first use the
 *  global GetInstance function to obtain the Driver Object Pointer.
 *  The actual APIs then can be invoked using obj->(api_name)() syntax.
 *  These functions are defined in the header file of the core driver
 *  and utilized by the API.
 *  @{
 */

/**********************************************************************
 * API methods
 **********************************************************************/
/**
 * "Get the driver's memory requirements to support the configuration
 * of the given controller."
 * @param[in] registerBase base address of controller registers
 * @param[out] req on success, filled with the number of bytes the driver requires for private data.
 * @return 0 on success (dataSize filled)
 * @return ENOTSUP if configuration cannot be supported due to driver/hardware constraints
 */
uint32_t HPNFC_Probe(uintptr_t registerBase, HPNFC_SysReq* req);
/**
 * The init function makes the following jobs: - initializes the
 * driver instance and state - configures the hardware as specified by
 * the client - initializes hardware
 * @param[in] privateData pointer to memory allocated for the driver use of sufficient size to hold the number of bytes returned by probe in HPNFC_SysReq
 * @param[in] config contains driver and hardware configuration as determined by probe call(s)
 * @return 0 on success
 * @return EINVAL if config is NULL or if illegal/inconsistent values in config
 */
int32_t HPNFC_Init(void* privateData, HPNFC_Config* config);
/**
 * "Driver ISR.  Platform-specific code is responsible for ensuring
 * this gets called when the controller's interrupt is asserted.
 * Registering the ISR should be done after calling init, and before
 * calling start."
 * @param[in] privateData driver instance data filled by init
 */
void HPNFC_Isr(void* privateData); // HPNFC_Isr
/**
 * "Start the driver. This enables any interrupts handled by the
 * driver. This is called after the client has successfully
 * initialized the driver and hooked the driver's ISR (the isr member
 * of this struct) to the IRQ."
 * @param[in] privateData driver instance data filled by init
 */
void HPNFC_Start(void* privateData);
/**
 * Stop the driver. This disables any interrupts being handled by the
 * driver.
 * @param[in] privateData driver instance data filled by init
 */
void HPNFC_Stop(void* privateData);
/**
 * Destroy the driver (automatically performs a stop)
 * @param[in] privateData driver instance data filled by init
 */
void HPNFC_Destroy(void* privateData);
/**
 * Function executes request
 * @param[in] privateData driver instance data filled by init
 * @param[in] cmdReq Request to execute
 * @return 0 on success
 * @return EINVAL if cmdReq is NULL or if illegal/inconsistent values in cmdReq
 * @return EBUSY if given thread defined in cmdReq is busy
 */
int32_t HPNFC_ExecCmdReq(void* privateData, HPNFC_CmdReq* cmdReq);
/**
 * Function configures ECC module of controller
 * @param[in] privateData driver instance data filled by init
 * @param[in] eccConfig new ECC configuration
 * @return 0 on success
 * @return EINVAL if eccConfig is NULL or if illegal/inconsistent values in eccConfig
 * @return ENOTSUP if ECC feature is not supported by controller
 * @return EBUSY if controller is busy
 */
int32_t HPNFC_SetEccConfig(void* privateData, HPNFC_EccConfiguration* eccConfig);
/**
 * Function get current ECC configuration
 * @param[in] privateData driver instance data filled by init
 * @param[out] eccConfig current ECC configuration
 * @return 0 on success
 * @return EINVAL if eccConfig is NULL
 * @return ENOTSUP if ECC feature is not supported by controller
 */
int32_t HPNFC_GetEccConfig(void* privateData, HPNFC_EccConfiguration* eccConfig);
/**
 * Function sets transfer configuration for program and read page
 * operations
 * @param[in] privateData driver instance data filled by init
 * @param[in] transferCfg new transfer configuration configuration
 * @return 0 on success
 * @return EINVAL if transferCfg is NULL or if illegal/inconsistent values in transferCfg
 * @return EBUSY if controller is busy
 */
int32_t HPNFC_SetTransferConfig(void* privateData, HPNFC_TransferConfig* transferCfg);
/**
 * Function gets transfer configuration
 * @param[in] privateData driver instance data filled by init
 * @param[out] transferCfg current transfer configuration configuration
 * @return 0 on success
 * @return EINVAL if transferCfg is NULL
 * @return EBUSY if controller is busy
 */
int32_t HPNFC_GetTransferConfig(void* privateData, HPNFC_TransferConfig* transferCfg);
/**
 * Function create cdmaCmd command base on descriptor.
 * @param[in] privateData driver instance data filled by init
 * @param[in] descPtr descriptor pointer to parse
 * @param[out] cdmaCmd CDMA command created base on descriptor pointer
 * @return 0 on success
 * @return EINVAL if cdmaCmd is NULL
 */
int32_t HPNFC_CdmaParseDesc(void* privateData, void* descPtr, HPNFC_CdmaCmd* cdmaCmd);
/**
 * Function gets device layout
 * @param[in] privateData driver instance data filled by init
 * @param[out] deviceLayout current device layout
 * @return 0 on success
 * @return EINVAL if deviceLayout is NULL
 */
int32_t HPNFC_GetDeviceLayout(void* privateData, HPNFC_DeviceLayout* deviceLayout);
/**
 * Function sets device sublayout
 * @param[in] privateData driver instance data filled by init
 * @param[in] deviceSubLayout new device sublayout to set
 * @return 0 on success
 * @return EINVAL if deviceSubLayout is NULL
 * @return EBUSY if controller is busy
 */
int32_t HPNFC_SetDeviceSubLayout(void* privateData, HPNFC_DeviceSubLayout* deviceSubLayout);
/**
 * Function gets current Ready/Busy# line status of one chip
 * @param[in] privateData driver instance data filled by init
 * @param[in] chip chip number to check rnb signal
 * @param[out] rnb current chip Ready/Busy# signal state
 * @return 0 on success
 * @return EINVAL if rnb is NULL
 */
int32_t HPNFC_GetRnb(void* privateData, uint8_t chip, uint8_t* rnb);
/**
 * Function gets status of boot operation
 * @param[in] privateData driver instance data filled by init
 * @param[out] bootStatus boot status
 * @param[out] cpyId boot block used in the latest boot sequence run
 * @return 0 on success
 * @return EINVAL if cpyId is NULL
 * @return ENOTSUP if boot feature is not supported by controller
 */
int32_t HPNFC_GetBootStatus(void* privateData, HPNFC_BootStatus* bootStatus, uint8_t* cpyId);
/**
 * Function gets ECC BCH configuration info (supported features)
 * @param[in] privateData driver instance data filled by init
 * @param[out] bchConfigInfo BCH supported configurations
 * @return 0 on success
 * @return EINVAL if bchConfigInfo is NULL
 */
int32_t HPNFC_GetBchConfigInfo(void* privateData, HPNFC_BchConfigInfo* bchConfigInfo);
/**
 * Function gets controller info (supported features)
 * @param[in] privateData driver instance data filled by init
 * @param[out] controllerInfo Controller supported features
 * @return 0 on success
 * @return EINVAL if controllerInfo is NULL
 */
int32_t HPNFC_GetControllerInfo(void* privateData, HPNFC_ControllerInfo* controllerInfo);
/**
 * Function gets information about NAND flash device
 * @param[in] privateData driver instance data filled by init
 * @param[out] deviceInfo current device layout
 * @return 0 on success
 * @return EINVAL if deviceInfo is NULL
 */
int32_t HPNFC_GetDeviceInfo(void* privateData, HPNFC_DevInfo* deviceInfo);

/**
 *  @}
 */


#endif	/* HPNFC_IF_H */
