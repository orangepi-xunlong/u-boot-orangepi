/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2018-2020 <wangwei@allwinnertech.com>
 */

#ifndef _SUNXI_CPU_H
#define _SUNXI_CPU_H

#if defined(CONFIG_SUNXI_NCAT)
#include <asm/arch/cpu_ncat.h>
#elif defined(CONFIG_SUNXI_NCAT_V2)
#include <asm/arch/cpu_ncat_v2.h>
#elif defined(CONFIG_SUNXI_VERSION1)
#include <asm/arch/cpu_version1.h>
#else
#include <asm/arch/cpu_sun4i.h>
#endif

#define SOCID_A64	0x1689
#define SOCID_H3	0x1680
#define SOCID_H5	0x1718
#define SOCID_R40	0x1701

#endif /* _SUNXI_CPU_H */
