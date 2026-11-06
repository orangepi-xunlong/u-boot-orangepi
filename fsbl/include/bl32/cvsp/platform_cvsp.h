/*
 * Copyright (c) 2014, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __PLATFORM_CVSP_H__


/*******************************************************************************
 * Mandatory BMSP functions (only if platform contains a BMSP)
 ******************************************************************************/
void bmsp_early_platform_setup(void);
void bmsp_plat_arch_setup(void);
void bmsp_platform_setup(void);


#define __PLATFORM_CVSP_H__

#endif
