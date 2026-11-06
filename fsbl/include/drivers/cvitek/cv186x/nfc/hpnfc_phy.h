 /*
 * Copyright (c) 2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef HPNFC_PHY_H
#define HPNFC_PHY_H
#include <hpnfc_if.h>

void HPNFC_PhyPrintPhyConfig(HPNFC_PhyConfig *phyConfig);
void HPNFC_PhyGetPhyConfig(HPNFC_State *hpnfc, HPNFC_PhyConfig *phyConfig);
void HPNFC_PhyWriteDllReset(HPNFC_State *hpnfc, uint8_t dllReset);
void HPNFC_PhySetPhyConfig(HPNFC_State *hpnfc, HPNFC_PhyConfig *phyConfig);

#endif
