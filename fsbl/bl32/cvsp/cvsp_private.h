/*
 * Copyright (c) 2014, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __CVSP_PRIVATE_H__
#define __CVSP_PRIVATE_H__

#define CVSP_TRACE_ENABLE 1

/* Definitions to help the assembler access the SMC/ERET args structure */
#define BMSP_ARGS_SIZE 0x40
#define BMSP_ARG0 0x0
#define BMSP_ARG1 0x8
#define BMSP_ARG2 0x10
#define BMSP_ARG3 0x18
#define BMSP_ARG4 0x20
#define BMSP_ARG5 0x28
#define BMSP_ARG6 0x30
#define BMSP_ARG7 0x38
#define BMSP_ARGS_END 0x40

#ifndef __ASSEMBLY__

#include <stdarg.h>
#include <string.h>

#define _PRINTF_38A01C tf_printf
#define _SIZE_67A9DC (sizeof(stmp) - (x - stmp))
int __trace(const char *prefix, const char *path, const char *func, int lineno,
	    const char *fmt, ...) __printflike(5, 6);
#define _error(fmt, ...)                                                       \
	__trace("ERROR:", __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)

#if CVSP_TRACE_ENABLE
#define _trace(fmt, ...)                                                       \
	__trace("", __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define _trace(fmt, ...)
#endif /* CVSP_TRACE_ENABLE */

#include <cassert.h>
#include <platform_def.h> /* For CACHE_WRITEBACK_GRANULE */
#include <spinlock.h>
#include <stdint.h>
#include <cvsp.h>

typedef struct work_statistics {
	/* Number of s-el1 interrupts on this cpu */
	uint32_t sel1_intr_count;
	/* Number of non s-el1 interrupts on this cpu which preempted BMSP */
	uint32_t preempt_intr_count;
	/* Number of sync s-el1 interrupts on this cpu */
	uint32_t sync_sel1_intr_count;
	/* Number of s-el1 interrupts returns on this cpu */
	uint32_t sync_sel1_intr_ret_count;
	uint32_t smc_count; /* Number of returns on this cpu */
	uint32_t eret_count; /* Number of entries on this cpu */
	uint32_t cpu_on_count; /* Number of cpu on requests */
	uint32_t cpu_off_count; /* Number of cpu off requests */
	uint32_t cpu_suspend_count; /* Number of cpu suspend requests */
	uint32_t cpu_resume_count; /* Number of cpu resume requests */
} __aligned(CACHE_WRITEBACK_GRANULE) work_statistics_t;

typedef struct bmsp_args {
	uint64_t _regs[BMSP_ARGS_END >> 3];
} __aligned(CACHE_WRITEBACK_GRANULE) bmsp_args_t;

/* Macros to access members of the above structure using their offsets */
#define read_sp_arg(args, offset) ((args)->_regs[offset >> 3])
#define write_sp_arg(args, offset, val) (((args)->_regs[offset >> 3]) = val)
/*
 * Ensure that the assembler's view of the size of the bmsp_args is the
 * same as the compilers
 */
CASSERT(sizeof(bmsp_args_t) == BMSP_ARGS_SIZE, assert_sp_args_size_mismatch);

void bmsp_get_magic(uint64_t args[4]);

bmsp_args_t *bmsp_cpu_resume_main(uint64_t arg0, uint64_t arg1, uint64_t arg2,
				  uint64_t arg3, uint64_t arg4, uint64_t arg5,
				  uint64_t arg6, uint64_t arg7);
bmsp_args_t *bmsp_cpu_suspend_main(uint64_t arg0, uint64_t arg1, uint64_t arg2,
				   uint64_t arg3, uint64_t arg4, uint64_t arg5,
				   uint64_t arg6, uint64_t arg7);
bmsp_args_t *bmsp_cpu_on_main(void);
bmsp_args_t *bmsp_cpu_off_main(uint64_t arg0, uint64_t arg1, uint64_t arg2,
			       uint64_t arg3, uint64_t arg4, uint64_t arg5,
			       uint64_t arg6, uint64_t arg7);

/* Generic Timer functions */
void bmsp_generic_timer_start(void);
void bmsp_generic_timer_handler(void);
void bmsp_generic_timer_stop(void);
void bmsp_generic_timer_save(void);
void bmsp_generic_timer_restore(void);

/* S-EL1 interrupt management functions */
void bmsp_update_sync_sel1_intr_stats(uint32_t type, uint64_t elr_el3);

bmsp_args_t *bmsp_set_smc_args(uint64_t arg0, uint64_t arg1, uint64_t arg2,
			       uint64_t arg3, uint64_t arg4, uint64_t arg5,
			       uint64_t arg6, uint64_t arg7);

/* Data structure to keep track of BMSP statistics */
extern spinlock_t console_lock;
extern work_statistics_t bmsp_stats[PLATFORM_CORE_COUNT];

/* Vector table of jumps */
extern bmsp_vectors_t bmsp_vector_table;

bmsp_args_t *cvsp_private_smc_handler(uint64_t func, uint64_t arg1,
				     uint64_t arg2, uint64_t arg3,
				     uint64_t arg4, uint64_t arg5,
				     uint64_t arg6, uint64_t arg7);

bmsp_args_t *cvsp_smc_call_debug_handler(uint64_t func, uint64_t arg1,
					 uint64_t arg2, uint64_t arg3,
					 uint64_t arg4, uint64_t arg5,
					 uint64_t arg6, uint64_t arg7);

#endif /* __ASSEMBLY__ */

#endif /* __CVSP_PRIVATE_H__ */
