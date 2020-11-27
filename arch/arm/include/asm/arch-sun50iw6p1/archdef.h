/*
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */
#ifndef _ARCH_DEF_H_
#define _ARCH_DEF_H_

#define ARCHISB	asm volatile ("isb sy")
#define ARCHDSB	asm volatile ("dsb sy")
#define ARCHDMB	asm volatile ("dmb sy")

#endif