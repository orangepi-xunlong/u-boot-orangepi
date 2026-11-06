 /*
 * Copyright (c) 2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef HPNFC_OBJ_IF_H
#define HPNFC_OBJ_IF_H

#include "hpnfc_if.h"

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
typedef struct HPNFC_OBJ_s
{
	/**
	 * "Get the driver's memory requirements to support the configuration
	 * of the given controller."
	 * @param[in] registerBase base address of controller registers
	 * @param[out] req on success, filled with the number of bytes the driver requires for private data.
	 * @return 0 on success (dataSize filled)
	 * @return ENOTSUP if configuration cannot be supported due to driver/hardware constraints
	 */
	uint32_t (*probe)(uintptr_t registerBase, HPNFC_SysReq* req); // HPNFC_Probe

	/**
	 * The init function makes the following jobs: - initializes the
	 * driver instance and state - configures the hardware as specified
	 * by the client - initializes hardware
	 * @param[in] privateData pointer to memory allocated for the driver use of sufficient size to hold the number of bytes returned by probe in HPNFC_SysReq
	 * @param[in] config contains driver and hardware configuration as determined by probe call(s)
	 * @return 0 on success
	 * @return EINVAL if config is NULL or if illegal/inconsistent values in config
	 */
	int32_t (*init)(void* privateData, HPNFC_Config* config); // HPNFC_Init

	/**
	 * "Driver ISR.  Platform-specific code is responsible for ensuring
	 * this gets called when the controller's interrupt is asserted.
	 * Registering the ISR should be done after calling init, and before
	 * calling start."
	 * @param[in] privateData driver instance data filled by init
	 */
	void (*isr)(void* privateData); // HPNFC_Isr

	/**
	 * "Start the driver. This enables any interrupts handled by the
	 * driver. This is called after the client has successfully
	 * initialized the driver and hooked the driver's ISR (the isr member
	 * of this struct) to the IRQ."
	 * @param[in] privateData driver instance data filled by init
	 */
	void (*start)(void* privateData);

	/**
	 * Stop the driver. This disables any interrupts being handled by the
	 * driver.
	 * @param[in] privateData driver instance data filled by init
	 */
	void (*stop)(void* privateData);

	/**
	 * Destroy the driver (automatically performs a stop)
	 * @param[in] privateData driver instance data filled by init
	 */
	void (*destroy)(void* privateData);


	/**
	 * Function executes request
	 * @param[in] privateData driver instance data filled by init
	 * @param[in] cmdReq Request to execute
	 * @return 0 on success
	 * @return EINVAL if cmdReq is NULL or if illegal/inconsistent values in cmdReq
	 * @return EBUSY if given thread defined in cmdReq is busy
	 */
	int32_t (*execCmdReq)(void* privateData, HPNFC_CmdReq* cmdReq); // HPNFC_ExecCmdReq

	/**
	 * Function change controller and device working mode.
	 * @param[in] privateData driver instance data filled by init
	 * @param[in] workMode work mode to set up
	 * @param[in] timingMode timing mode of work mode
	 * @param[in] phyConfig phy configuration for given work mode
	 * @return 0 on success
	 * @return EINVAL if phyConfig is NULL
	 * @return EBUSY if command send to memory device to change its mode cannot be finished
	 */
	int32_t (*setWorkMode)(void* privateData, HPNFC_WorkMode workMode, uint8_t timingMode, HPNFC_PhyConfig* phyConfig);

	/**
	 * Function configures ECC module of controller
	 * @param[in] privateData driver instance data filled by init
	 * @param[in] eccConfig new ECC configuration
	 * @return 0 on success
	 * @return EINVAL if eccConfig is NULL or if illegal/inconsistent values in eccConfig
	 * @return ENOTSUP if ECC feature is not supported by controller
	 * @return EBUSY if controller is busy
	 */
	int32_t (*setEccConfig)(void* privateData, HPNFC_EccConfiguration* eccConfig);

	/**
	 * Function get current ECC configuration
	 * @param[in] privateData driver instance data filled by init
	 * @param[out] eccConfig current ECC configuration
	 * @return 0 on success
	 * @return EINVAL if eccConfig is NULL
	 * @return ENOTSUP if ECC feature is not supported by controller
	 */
	int32_t (*getEccConfig)(void* privateData, HPNFC_EccConfiguration* eccConfig);

	/**
	 * Function sets transfer configuration for program and read page
	 * operations
	 * @param[in] privateData driver instance data filled by init
	 * @param[in] transferCfg new transfer configuration configuration
	 * @return 0 on success
	 * @return EINVAL if transferCfg is NULL or if illegal/inconsistent values in transferCfg
	 * @return EBUSY if controller is busy
	 */
	int32_t (*setTransferConfig)(void* privateData, HPNFC_TransferConfig* transferCfg);

	/**
	 * Function gets transfer configuration
	 * @param[in] privateData driver instance data filled by init
	 * @param[out] transferCfg current transfer configuration configuration
	 * @return 0 on success
	 * @return EINVAL if transferCfg is NULL
	 * @return EBUSY if controller is busy
	 */
	int32_t (*getTransferConfig)(void* privateData, HPNFC_TransferConfig* transferCfg);

	/**
	 * Function gets status polling configuration
	 * @param[in] privateData driver instance data filled by init
	 * @param[out] statusPollingConfig current status polling configuration
	 * @return 0 on success
	 * @return EINVAL if statusPollingConfig is NULL
	 */
	int32_t (*getStatusPollingConfig)(void* privateData, HPNFC_StatusPollingConfig* statusPollingConfig);

	/**
	 * Function gets device layout
	 * @param[in] privateData driver instance data filled by init
	 * @param[out] deviceLayout current device layout
	 * @return 0 on success
	 * @return EINVAL if deviceLayout is NULL
	 */
	int32_t (*getDeviceLayout)(void* privateData, HPNFC_DeviceLayout* deviceLayout); // HPNFC_GetDeviceLayout

	/**
	 * Function sets device sublayout
	 * @param[in] privateData driver instance data filled by init
	 * @param[in] deviceSubLayout new device sublayout to set
	 * @return 0 on success
	 * @return EINVAL if deviceSubLayout is NULL
	 * @return EBUSY if controller is busy
	 */
	int32_t (*setDeviceSubLayout)(void* privateData, HPNFC_DeviceSubLayout* deviceSubLayout);

	/**
	 * Function gets current Ready/Busy# line status of one chip
	 * @param[in] privateData driver instance data filled by init
	 * @param[in] chip chip number to check rnb signal
	 * @param[out] rnb current chip Ready/Busy# signal state
	 * @return 0 on success
	 * @return EINVAL if rnb is NULL
	 */
	int32_t (*getRnb)(void* privateData, uint8_t chip, uint8_t* rnb); // HPNFC_GetRnb

	/**
	 * Function gets status of boot operation
	 * @param[in] privateData driver instance data filled by init
	 * @param[out] bootStatus boot status
	 * @param[out] cpyId boot block used in the latest boot sequence run
	 * @return 0 on success
	 * @return EINVAL if cpyId is NULL
	 * @return ENOTSUP if boot feature is not supported by controller
	 */
	int32_t (*getBootStatus)(void* privateData, HPNFC_BootStatus* bootStatus, uint8_t* cpyId);

	/**
	 * Function gets multiplane configuration
	 * @param[in] privateData driver instance data filled by init
	 * @param[out] multiplaneConfig current multiplane configuration
	 * @return 0 on success
	 * @return EINVAL if multiplaneConfig is NULL
	 */
	int32_t (*getMultiplaneConfig)(void* privateData, HPNFC_MultiplaneConfig* multiplaneConfig);

	/**
	 * Function sets multiplane operations configuration
	 * @param[in] privateData driver instance data filled by init
	 * @param[in] multiplaneConfig new multiplane configuration to set
	 * @return 0 on success
	 * @return EINVAL if multiplaneConfig is NULL if illegal/inconsistent values in multiplaneConfig
	 * @return EBUSY if controller is busy
	 */
	int32_t (*setMultiplaneConfig)(void* privateData, HPNFC_MultiplaneConfig* multiplaneConfig);

	/**
	 * Function gets ECC BCH configuration info (supported features)
	 * @param[in] privateData driver instance data filled by init
	 * @param[out] bchConfigInfo BCH supported configurations
	 * @return 0 on success
	 * @return EINVAL if bchConfigInfo is NULL
	 */
	int32_t (*getBchConfigInfo)(void* privateData, HPNFC_BchConfigInfo* bchConfigInfo);

	/**
	 * Function gets controller info (supported features)
	 * @param[in] privateData driver instance data filled by init
	 * @param[out] controllerInfo Controller supported features
	 * @return 0 on success
	 * @return EINVAL if controllerInfo is NULL
	 */
	int32_t (*getControllerInfo)(void* privateData, HPNFC_ControllerInfo* controllerInfo); // HPNFC_GetControllerInfo

	/**
	 * Function gets CDMA descriptor size in bytes which is expected by
	 * hardware.
	 * @param[in] privateData driver instance data filled by init
	 * @param[out] descSize descriptor size in bytes
	 * @return 0 on success
	 * @return EINVAL if descSize is NULL
	 */
	int32_t (*getCdmaDescSize)(void* privateData, uint8_t* descSize);

	/**
	 * Function sets timeout configuration
	 * @param[in] privateData driver instance data filled by init
	 * @param[in timeout configuration to set] timeoutConfig
	 * @return 0 on success
	 * @return EINVAL if timeoutConfig is NULL
	 */
	int32_t (*setTimeoutConfig)(void* privateData, HPNFC_TimeoutConfig* timeoutConfig);

	/**
	 * Function gets timeout configuration
	 * @param[in] privateData driver instance data filled by init
	 * @param[out current timeout configuration] timeoutConfig
	 * @return 0 on success
	 * @return EINVAL if timeoutConfig is NULL
	 */
	int32_t (*getTimeoutConfig)(void* privateData, HPNFC_TimeoutConfig* timeoutConfig);

	/**
	 * Function gets information about NAND flash device
	 * @param[in] privateData driver instance data filled by init
	 * @param[out] deviceInfo current device layout
	 * @return 0 on success
	 * @return EINVAL if deviceInfo is NULL
	 */
	int32_t (*getDeviceInfo)(void* privateData, HPNFC_DevInfo* deviceInfo);

} HPNFC_OBJ;

/**
 * In order to access the HPNFC APIs, the upper layer software must call
 * this global function to obtain the pointer to the driver object.
 * @return HPNFC_OBJ* Driver Object Pointer
 */
extern HPNFC_OBJ *HPNFC_GetInstance(void);

/**
 *  @}
 */


#endif	/* HPNFC_OBJ_IF_H */
