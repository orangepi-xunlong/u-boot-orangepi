// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2015 Andreas Bießmann <andreas@biessmann.org>
 *
 * Copyright (c) 2011 The Chromium OS Authors.
 * (C) Copyright 2002-2006
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 */
#include <common.h>

DECLARE_GLOBAL_DATA_PTR;

int arch_reserve_stacks(void)
{
#ifdef CONFIG_SPL_BUILD
	gd->start_addr_sp -= 128;	/* leave 32 words for abort-stack */
	gd->irq_sp = gd->start_addr_sp;
#else
	/* setup stack pointer for exceptions */
	gd->irq_sp = gd->start_addr_sp;
#ifdef CONFIG_ARCH_SUNXI
	gd->start_addr_sp -= (SUNXI_STACKSIZE_IRQ + SUNXI_STACKSIZE_FIQ);
	debug("Reserving %zu Bytes for IRQ stack at: %08lx\n",
			SUNXI_STACKSIZE_IRQ + SUNXI_STACKSIZE_FIQ, gd->start_addr_sp);

	/* 8-byte alignment for ARM ABI compliance */
	gd->start_addr_sp &= ~0x07;
#endif

# if !defined(CONFIG_ARM64)
	/* leave 3 words for abort-stack, plus 1 for alignment */
	gd->start_addr_sp -= 16;
# endif
#endif
	return 0;
}
