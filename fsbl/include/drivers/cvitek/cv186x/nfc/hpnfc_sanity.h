/**
 * This file contains sanity API functions. The purpose of sanity functions
 * is to check input parameters validity. They take the same parameters as
 * original API functions and return 0 on success or EINVAL on wrong parameter
 * value(s).
 */

#ifndef HPNFC_SANITY_H
#define HPNFC_SANITY_H

#include <hpnfc_if.h>
#include <cdn_stdtypes.h>
#include <cdn_errno.h>


#define	HPNFC_ProbeSF HPNFC_SanityFunction1
#define	HPNFC_InitSF HPNFC_SanityFunction2
#define	HPNFC_IsrSF HPNFC_SanityFunction3
#define	HPNFC_StartSF HPNFC_SanityFunction3
#define	HPNFC_StopSF HPNFC_SanityFunction3
#define	HPNFC_DestroySF HPNFC_SanityFunction3
#define	HPNFC_ExecCmdReqSF HPNFC_SanityFunction8
#define	HPNFC_SetWorkModeSF HPNFC_SanityFunction9
#define	HPNFC_SetEccConfigSF HPNFC_SanityFunction10
#define	HPNFC_GetEccConfigSF HPNFC_SanityFunction10
#define	HPNFC_SetTransferConfigSF HPNFC_SanityFunction12
#define	HPNFC_GetTransferConfigSF HPNFC_SanityFunction12
#define	HPNFC_CdmaDescGetOperationReSF HPNFC_SanityFunction16
#define	HPNFC_CdmaParseDescSF HPNFC_SanityFunction7
#define	HPNFC_SetStatusPollingConfigSF HPNFC_SanityFunction19
#define	HPNFC_GetStatusPollingConfigSF HPNFC_SanityFunction19
#define	HPNFC_SetAutoStatusConfigSF HPNFC_SanityFunction21
#define	HPNFC_GetAutoStatusConfigSF HPNFC_SanityFunction21
#define	HPNFC_GetDeviceLayoutSF HPNFC_SanityFunction23
#define	HPNFC_SetDeviceSubLayoutSF HPNFC_SanityFunction24
#define	HPNFC_GetRnbSF HPNFC_SanityFunction27
#define	HPNFC_GetBootStatusSF HPNFC_SanityFunction28
#define	HPNFC_GetBchConfigInfoSF HPNFC_SanityFunction33
#define	HPNFC_GetControllerInfoSF HPNFC_SanityFunction34
#define	HPNFC_GetCdmaDescSizeSF HPNFC_SanityFunction35
#define	HPNFC_SetDmaConfigSF HPNFC_SanityFunction43
#define	HPNFC_GetDmaConfigSF HPNFC_SanityFunction43
#define	HPNFC_SetTimeoutConfigSF HPNFC_SanityFunction61
#define	HPNFC_GetTimeoutConfigSF HPNFC_SanityFunction61
#define	HPNFC_GetDeviceInfoSF HPNFC_SanityFunction65
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] registerBase base address of controller registers
 * @param[out] req on success, filled with the number of bytes the driver requires for private data.
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction1(uintptr_t registerBase, HPNFC_SysReq* req)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (req == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData pointer to memory allocated for the driver use of sufficient size to hold the number of bytes returned by probe in HPNFC_SysReq
 * @param[in] config contains driver and hardware configuration as determined by probe call(s)
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction2(void* privateData, HPNFC_Config* config)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
ret = EINVAL;
	}
	if (config == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction3(void* privateData)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[in] descPtr descriptor pointer
 * @param[in] cdmaCmd CDMA command used to assemble one CDMA descriptor
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction7(void* privateData, void* descPtr, HPNFC_CdmaCmd* cdmaCmd)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}
	if (descPtr == NULL)
	{
		ret = EINVAL;
	}
	if (cdmaCmd == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[in] cmdReq Request to execute
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction8(void* privateData, HPNFC_CmdReq* cmdReq)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}
	if (cmdReq == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[in] workMode work mode to set up
 * @param[in] timingMode timing mode of work mode
 * @param[in] phyConfig phy configuration for given work mode
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction9(void* privateData, HPNFC_WorkMode workMode, uint8_t timingMode, HPNFC_PhyConfig* phyConfig)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}
	if (phyConfig == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[in] eccConfig new ECC configuration
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction10(void* privateData, HPNFC_EccConfiguration* eccConfig)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}
	if (eccConfig == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[in] transferCfg new transfer configuration configuration
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction12(void* privateData, HPNFC_TransferConfig* transferCfg)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}
if (transferCfg == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] cdmaDesc first descriptor in the chain
 * @param[out] fail a pointer to the descriptor whose execution failed
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction14(void* cdmaDesc, void** fail)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (cdmaDesc == NULL)
		{
		ret = EINVAL;
	}
	if (fail == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[in] cdmaDesc a descriptor to check
 * @param[out] operationResult status of operation defined in descriptor
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction16(void* privateData, void* cdmaDesc, HPNFC_OperationResult* operationResult)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}
	if (cdmaDesc == NULL)
	{
		ret = EINVAL;
	}
	if (operationResult == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[in] cmdReq a request to check
 * @param[out] operationResult status of operation defined in descriptor
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction17(void* privateData, HPNFC_CmdReq* cmdReq, HPNFC_OperationResult* operationResult)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}
	if (cmdReq == NULL)
	{
		ret = EINVAL;
	}
	if (operationResult == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[in] statusPollingConfig new status polling configuration
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction19(void* privateData, HPNFC_StatusPollingConfig* statusPollingConfig)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
		{
		ret = EINVAL;
	}
	if (statusPollingConfig == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[in] autoStatusConfig new status configuration
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction21(void* privateData, HPNFC_AutoStatusConfig* autoStatusConfig)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}
	if (autoStatusConfig == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[out] deviceLayout current device layout
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction23(void* privateData, HPNFC_DeviceLayout* deviceLayout)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}
	if (deviceLayout == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[in] deviceSubLayout new device sublayout to set
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction24(void* privateData, HPNFC_DeviceSubLayout* deviceSubLayout)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}
	if (deviceSubLayout == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[in] lunInterleavedConfig new interleaved configuration to set
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction25(void* privateData, HPNFC_LunInterleavedConfig* lunInterleavedConfig)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}
	if (lunInterleavedConfig == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[in] chip chip number to check rnb signal
 * @param[out] rnb current chip Ready/Busy# signal state
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction27(void* privateData, uint8_t chip, uint8_t* rnb)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}
	if (rnb == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[out] bootStatus boot status
 * @param[out] cpyId boot block used in the latest boot sequence run
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction28(void* privateData, HPNFC_BootStatus* bootStatus, uint8_t* cpyId)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}
	if (bootStatus == NULL)
	{
		ret = EINVAL;
	}
	if (cpyId == NULL)
	{
				ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[out] multiplaneConfig current multiplane configuration
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction29(void* privateData, HPNFC_MultiplaneConfig* multiplaneConfig)
{
		/* Declaring return variable */
int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}
	if (multiplaneConfig == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[out] cacheConfig current cache configuration
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction31(void* privateData, HPNFC_CacheConfig* cacheConfig)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}
	if (cacheConfig == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[out] bchConfigInfo BCH supported configurations
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction33(void* privateData, HPNFC_BchConfigInfo* bchConfigInfo)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}
	if (bchConfigInfo == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[out] controllerInfo Controller supported features
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction34(void* privateData, HPNFC_ControllerInfo* controllerInfo)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}
	if (controllerInfo == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[out] descSize descriptor size in bytes
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction35(void* privateData, uint8_t* descSize)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}
	if (descSize == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[inout] dbgReq debug request to be executed on Debug Interface
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction36(void* privateData, HPNFC_DebugReq* dbgReq)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}
	if (dbgReq == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[in] ifType debug nterface type
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction37(void* privateData, HPNFC_DebugIfType ifType)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}
	if (
		ifType != HPNFC_DBG_IF_TYPE_REGISTER &&
		ifType != HPNFC_DBG_IF_TYPE_HARDWARE
	)
	{
		ret = EINVAL;
	}

	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[out] ifType debug interface type
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction38(void* privateData, HPNFC_DebugIfType* ifType)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}
	if (ifType == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[in] diConfig data integrity configuration
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction39(void* privateData, HPNFC_DiConfig* diConfig)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}
	if (diConfig == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[in] diErrInjConfig error injection configuration to set
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction41(void* privateData, HPNFC_DiErrInjConfig* diErrInjConfig)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}
	if (diErrInjConfig == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[in] dmaConfig DMA configuration
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction43(void* privateData, HPNFC_DmaConfig* dmaConfig)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}
	if (dmaConfig == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[in] remapRecord remap record to write
 * @param[in determines which memory target is bind to current write] memTrg
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction45(void* privateData, HPNFC_RemapRecord* remapRecord, uint8_t memTrg)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}
	if (remapRecord == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[in] recordIdx index of remap record
 * @param[out] remapRecord read remap record
 * @param[in determines which memory target is bind to current read] memTrg
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction46(void* privateData, uint16_t recordIdx, HPNFC_RemapRecord* remapRecord, uint8_t memTrg)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}
	if (remapRecord == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[out] recordCount number of records stored in remap table
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction48(void* privateData, uint16_t* recordCount)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}
	if (recordCount == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[in] remapMask remap mask to be set
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction49(void* privateData, uint32_t remapMask)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[out] remapMask remap mask to be set
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction50(void* privateData, uint32_t* remapMask)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}
	if (remapMask == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[in] remapEnable configuration of remap, 0 - disabled, 1 - enabled
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction51(void* privateData, uint8_t remapEnable)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[in] pSlcConfig configuration pSLC to set
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction53(void* privateData, HPNFC_PSlcConfig* pSlcConfig)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}
	if (pSlcConfig == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}

/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[int] calStatToClear Each bit of calStat points ZQ recalibration status to clear for
 *      selected target/device Bits are encoded as following
 *      b0 - do not clear error
 *      b1 - clear re-calibration error
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction60(void* privateData, uint16_t calStatToClear)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[in timeout configuration to set] timeoutConfig
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction61(void* privateData, HPNFC_TimeoutConfig* timeoutConfig)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}
	if (timeoutConfig == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}
/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] privateData driver instance data filled by init
 * @param[out] deviceInfo current device layout
 * @return 0 success
 * @return EINVAL invalid parameters
 */
static inline int32_t HPNFC_SanityFunction65(void* privateData, HPNFC_DevInfo* deviceInfo)
{
	/* Declaring return variable */
	int32_t ret = 0;

	if (privateData == NULL)
	{
		ret = EINVAL;
	}
	if (deviceInfo == NULL)
	{
		ret = EINVAL;
	}


	return ret;
}

#endif	/* HPNFC_SANITY_H */
