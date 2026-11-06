// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
 *
 * (C) Copyright 2002
 * Gary Jennejohn, DENX Software Engineering, <garyj@denx.de>
 *
 * (C) Copyright 2004
 * DAVE Srl
 * http://www.dave-tech.it
 * http://www.wawnet.biz
 * mailto:info@wawnet.biz
 *
 * (C) Copyright 2004 Texas Insturments
 */

#include <common.h>
#include <command.h>
#include <cpu_func.h>
#include <irq_func.h>
#include <linux/delay.h>

__weak void reset_misc(void)
{
}
extern void cv_system_reset(void);
int do_reset(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	puts("resetting ...\n");
	mdelay(50);				/* wait 50 ms */
	disable_interrupts();
	cv_system_reset();
	//reset_misc();
	//reset_cpu();

	/*NOTREACHED*/
	return 0;
}

#include "mmio.h"
#define C906B_RVBA_H 0x28100374
#define C906B_RVBA_L 0x28100370
#define TC906B0_RVBA_H 0x2605021c
#define TC906B0_RVBA_L 0x26050218
#define TC906B1_RVBA_H 0x26050224
#define TC906B1_RVBA_L 0x26050220
#define C906B_RESET_REG 0x28103024
#define TC906B_RESET_REG 0x28103000
#define C906B_MAX_NUM 3

int do_reset_c906b(struct cmd_tbl *cmdtp,
				   int flag,
				   int argc,
				   char * const argv[])
{
		puts("reset c906b ...\n");
		ulong coreIndex = 0;
		ulong addr = 0;
		uint rvbaReg[3][2] = {{C906B_RVBA_H, C906B_RVBA_L},
							  {TC906B0_RVBA_H, TC906B0_RVBA_L},
							  {TC906B1_RVBA_H, TC906B1_RVBA_L}};

		if (argc == 2) {
			coreIndex = hextoul(argv[1], NULL);
			printf("reset C906B[%ld] ...\n", coreIndex);
			// reset AP c906B
			if (coreIndex == 0) {
				// bit10 1'b0
				mmio_clrbits_32(C906B_RESET_REG, (1 << 10));
				udelay(10);
				// bit10 1'b1
				mmio_setbits_32(C906B_RESET_REG, (1 << 10));
			} else if (coreIndex == 1 || coreIndex == 2) {
				// bit 27 for TC906B 0
				// bit 28 for TC906B 1
				mmio_clrbits_32(TC906B_RESET_REG, 1 << (26 + coreIndex));
				udelay(10);
				mmio_setbits_32(TC906B_RESET_REG, 1 << (26 + coreIndex));
			}
		} else if (argc == 3) {
			coreIndex = hextoul(argv[1], NULL);
			addr = hextoul(argv[2], NULL);
			printf("bring up C906B[%ld] from 0x%lx\n", coreIndex, addr);
			// set c906b rvba
			mmio_write_32(rvbaReg[coreIndex % C906B_MAX_NUM][0],
						  (addr >> 32) & 0xFF);  // bit32-39
			mmio_write_32(rvbaReg[coreIndex % C906B_MAX_NUM][1],
						  addr & 0xFFFFFFFF);

			if (coreIndex == 0) {
				// bit10 1'b0
				mmio_clrbits_32(C906B_RESET_REG, (1 << 10));
				udelay(10);
				// bit10 1'b1
				mmio_setbits_32(C906B_RESET_REG, (1 << 10));
			} else if (coreIndex == 1 || coreIndex == 2) {
				//set tpu c906 clk
				mmio_setbits_32(0x26050000, 1<< (4 + coreIndex));
				//set ddr tpu clk
				mmio_setbits_32(0x7000a014, 1 << 4);

				// bit 27 for TC906B 0
				// bit 28 for TC906B 1
				mmio_clrbits_32(TC906B_RESET_REG, 1 << (26 + coreIndex));
				udelay(10);
				mmio_setbits_32(TC906B_RESET_REG, 1 << (26 + coreIndex));
			}
		}

		return 0;
}

