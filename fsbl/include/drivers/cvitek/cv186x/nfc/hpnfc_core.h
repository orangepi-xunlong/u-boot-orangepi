 /*
 * Copyright (c) 2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef HPNFC_CORE_H
#define HPNFC_CORE_H

#include <hpnfc_if.h>
#include <hpnfc_regs.h>
#include "hpnfc_regs_macro.h"
#include "nfc9_minictrl_regs.h"
#include "nfc9_minictrl_regs_macro.h"

#include "dll_phy_addr_map.h"
#include "dll_phy_addr_map_macro.h"


/* max supported thread count */
#define HPNFC_MAX_THREAD_COUNT          1
/* max supported command count for PIO and CMDA mode */
#define HPNFC_MAX_CMD_COUNT             0x100
/* max supported extended command count for PIO and CMDA mode */
#define HPNFC_MAX_EXT_CMD_COUNT         0x10000
#define HPNFC_MAX_CMD_COUNT_MASK        (HPNFC_MAX_CMD_COUNT - 1)
#define HPNFC_MAX_EXT_CMD_COUNT_MASK    (HPNFC_MAX_EXT_CMD_COUNT - 1)


#define LogAndExit(status) \
	do{\
		DbgMsg(DBG_GEN_MSG, 0,\
			"Error nr:%d in %s:%d\n",(unsigned int)status,\
			__FILE__, __LINE__);\
		return status;\
	}while(0)


/* command type CDMA */
#define HPNFC_CMD_REG0_CT_CDMA                  (0uL << 30)
/* command type PIO */
#define HPNFC_CMD_REG0_CT_PIO                   (1uL << 30)
/* command type reset */
#define HPNFC_CMD_REG0_CT_RST                   (2uL << 30)
/* command type generic */
#define HPNFC_CMD_REG0_CT_GEN                   (3uL << 30)
#define HPNFC_CMD_REG0_THREAD_WRITE(thread)     ((uint32_t)thread << 24)
/* interrupt enabled mask */
#define HPNFC_CMD_REG0_INT_MASK                 (1uL << 20)

#define HPNFC_RST_INSTR_RESET_TYPE_0            0x200
#define HPNFC_RST_INSTR_RESET_TYPE_1            0x201

/* Operation work mode - SDR mode */
#define HPNFC_COMMON_SETT_OPR_MODE_SDR          (0 << RF_MINICTRL_REGS__RT_COMMON_SETTINGS__OPR_MODE__SHIFT)
/* Operation work mode - NV_DDR mode */
#define HPNFC_COMMON_SETT_OPR_MODE_NV_DDR       (1 << RF_MINICTRL_REGS__RT_COMMON_SETTINGS__OPR_MODE__SHIFT)
/* Operation work mode - ToggleMode/NV-DDR2/NV-DDR3 mode */
#define HPNFC_COMMON_SETT_OPR_MODE_TOGGLE       (2 << RF_MINICTRL_REGS__RT_COMMON_SETTINGS__OPR_MODE__SHIFT)


/* plane ECC error - shift*/
#define HPNFC_OPER_STATUS_EXT_PL_ECC_ERR_SHIFT   24
/* plane ECC error - mask*/
#define HPNFC_OPER_STATUS_EXT_PL_ECC_ERR_MASK   (0xFFuL << 24)
/* plane device error - shift*/
#define HPNFC_OPER_STATUS_EXT_PL_DEV_ERR_SHIFT   16
/* plane device error - mask*/
#define HPNFC_OPER_STATUS_EXT_PL_DEV_ERR_MASK   (0xFFuL << 16)
/* command extended status  - the operation number
 * where the first error was detected - shift*/
#define HPNFC_OPER_STATUS_EXT_ERR_IDX_SHIFT     0
/* command extended status  - the operation number
 * where the first error was detected - mask*/
#define HPNFC_OPER_STATUS_EXT_ERR_IDX_MASK      (0xFFuL)


/* command  status  - the operation number
 * where the first error was detected - shift*/
#define HPNFC_OPER_STATUS_ERR_IDX_SHIFT     24
/* command  status  - the operation number
 * where the first error was detected - mask*/
#define HPNFC_OPER_STATUS_ERR_IDX_MASK      (0xFFuL << 24)
/* parity error was detected on the data bus during access to the
 * context memory */
#define HPNFC_OPER_STATUS_DI_CTX_ERR_MASK   (1uL << 19)
/* parity error was detected on the data bus during access to the
 * system bus */
#define HPNFC_OPER_STATUS_DI_DSC_ERR_MASK   (1uL << 18)
/* CRC error was detected in the data path  */
#define HPNFC_OPER_STATUS_DI_DAT_ERR_MASK   (1uL << 17)
/* controller got an error response on the system bus */
#define HPNFC_OPER_STATUS_BUS_ERR_MASK      (1uL << 16)
/* command  status  - operation complete - mask*/
#define HPNFC_OPER_STATUS_COMP_MASK         (1uL << 15)
/* command  status  - operation fail - mask*/
#define HPNFC_OPER_STATUS_FAIL_MASK         (1uL << 14)
/* Incorrect DQS pulses number was detected during the data read operation */
#define HPNFC_OPER_STATUS_DQS_ERR_MASK      (1uL << 13)
/* Device error was detected in any of the device planes. - mask*/
#define HPNFC_OPER_STATUS_DEV_ERR_MASK      (1uL << 12)
/* command  status  - page erased - mask*/
#define HPNFC_OPER_STATUS_ERP_MASK          (1uL << 11)
/* command  status  - timeout occured - mask*/
#define HPNFC_OPER_STATUS_TOUT_MASK         (1uL << 10)
/* command  status - maximum amount of correction
 * applied to one ECC sector - shift */
#define HPNFC_OPER_STATUS_MAXERR_SHIFT  2
/* command desciptor status - maximum amount of correction
 * applied to one ECC sector - mask */
#define HPNFC_OPER_STATUS_MAXERR_MASK       (0xFFuL << 2)
/* command  status - uncorrectable ECC error - mask*/
#define HPNFC_OPER_STATUS_UNCE_MASK         (1uL << 1)
/* command  status -  error - mask*/
#define HPNFC_OPER_STATUS_ERR_MASK          (1uL << 0)

/* accessing to remap table are possible by following access commands */
#define HPNFC_REMAP_ACCESS_TYPE_WRITE       0
#define HPNFC_REMAP_ACCESS_TYPE_READ        1
#define HPNFC_REMAP_ACCESS_TYPE_CLEAR_ALL   2

typedef struct HPNFC_SlaveDmaState {
	void *currMemPtr;
	void *currCdmaDesc;
} HPNFC_SlaveDmaState;


typedef struct HPNFC_State {
	uintptr_t regAddress;
	/* register access obejcts */
	struct hpnfc_regs *regs;
	struct dll_phy_addr_map *dll_phy_regs;
	struct nfc9_minictrl_regs *minictrl_regs;

	/* ONFi version of connected NAND flash memory */
	uint32_t onfiVersion;
	/* pointers to the currently executed requests. */
	HPNFC_CmdReq* cmdReq[HPNFC_MAX_THREAD_COUNT];
	/* it keeps information about current Slave DMA transmission */
	HPNFC_SlaveDmaState slvDmaState[HPNFC_MAX_THREAD_COUNT];
	/* if set interrupts are enabled */
	uint8_t intEnabled;
	/* default PHY config */
	HPNFC_PhyConfig defPhyConfig;
	/* ECC current configuration */
	HPNFC_EccConfiguration eccConfig;
	/* NAND flash memory information */
	HPNFC_DevInfo devInfo;
	/* Number of connected NAND flash memories */
	uint8_t devNum;
	/* pointer to function handling data transfer by slave DMA */
	HPNFC_SlaveDmaHandler slaveDmaHandler;
	/* NAND flash controller information (version, features ...) */
	HPNFC_ControllerInfo ctrlInfo;
	/* BCH supported configurations */
	HPNFC_BchConfigInfo bchCfgInfo;
	/* CDMA descriptor size */
	uint8_t cdmaDescSize;
} HPNFC_State;


void memSet(void *buff, uint32_t val, uint32_t size);


uint32_t HPNFC_GetReg(uintptr_t reg);
void HPNFC_SetReg(uintptr_t reg, uint32_t val);

int32_t HPNFC_PrintDevInfo(HPNFC_State *hpnfc);
int32_t HPNFC_Core_Init(HPNFC_State *hpnfc, HPNFC_Config *config);
void HPNFC_Core_Isr(HPNFC_State *hpnfc);
void HPNFC_ConfigInt(HPNFC_State *hpnfc, uint8_t enable);
int32_t HPNFC_CdmaSend(HPNFC_State *hpnfc, uint8_t thread, HPNFC_CdmaReq *req);
int32_t HPNFC_ReqAttach(HPNFC_State *hpnfc, HPNFC_CmdReq* cmdReq);
int32_t HPNFC_GetLastStatus(HPNFC_State *hpnfc, uint8_t thread, uint32_t status[2]);
int32_t HPNFC_ReqRelease(HPNFC_State *hpnfc, uint8_t thread, int32_t requestStatus);
int32_t HPNFC_PioSend(HPNFC_State *hpnfc, uint8_t thread, HPNFC_PioReq *req);
int32_t HPNFC_GenericSend(HPNFC_State *hpnfc, uint8_t thread, HPNFC_GenReq *req);
int32_t HPNFC_ResetSend(HPNFC_State *hpnfc, uint8_t thread, HPNFC_RstReq *req);
int32_t HPNFC_Core_GetEccConfig(struct HPNFC_State *hpnfc,
							HPNFC_EccConfiguration *eccConfig);
int32_t HPNFC_Core_SetEccConfig(struct HPNFC_State *hpnfc,
							HPNFC_EccConfiguration *eccConfig);

int32_t HPNFC_Core_SetTransferConfig(struct HPNFC_State *hpnfc,
								 HPNFC_TransferConfig *transferCfg);
void HPNFC_Core_GetTransferConfig(struct HPNFC_State *hpnfc,
								 HPNFC_TransferConfig *transferCfg);
void HPNFC_Core_GetDeviceLayout(struct HPNFC_State *hpnfc,
								HPNFC_DeviceLayout *devLayout);
int32_t HPNFC_Core_SetDeviceSubLayout(struct HPNFC_State *hpnfc,
								  HPNFC_DeviceSubLayout *devSubLayout);
HPNFC_OperationResult HPNFC_GetOperationResult(struct HPNFC_State *hpnfc,
											   uint32_t status[2], HPNFC_ReqType reqType);
int32_t HPNFC_Core_GetRnb(HPNFC_State *hpnfc, uint8_t chip);

int32_t HPNFC_Core_GetBootStatus(HPNFC_State *hpnfc, HPNFC_BootStatus *bootStatus,
							 uint8_t *cpyId);

int32_t HPNFC_Core_GetDevInfo(HPNFC_State *hpnfc, HPNFC_DevInfo *devInfo);

#endif
