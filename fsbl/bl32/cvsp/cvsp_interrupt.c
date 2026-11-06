/*
 * Copyright (c) 2014-2015, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <arch_helpers.h>
#include <assert.h>
#include <debug.h>
#include <platform.h>
#include <platform_def.h>
#include <cvsp.h>
#include "cvsp_private.h"

/*******************************************************************************
 * This function updates the BMSP statistics for S-EL1 interrupts handled
 * synchronously i.e the ones that have been handed over by the BMSPD. It also
 * keeps count of the number of times control was passed back to the BMSPD
 * after handling the interrupt. In the future it will be possible that the
 * BMSPD hands over an S-EL1 interrupt to the BMSP but does not expect it to
 * return execution. This statistic will be useful to distinguish between these
 * two models of synchronous S-EL1 interrupt handling. The 'elr_el3' parameter
 * contains the address of the instruction in normal world where this S-EL1
 * interrupt was generated.
 ******************************************************************************/
void bmsp_update_sync_sel1_intr_stats(uint32_t type, uint64_t elr_el3)
{
	uint32_t linear_id = plat_my_core_pos();

	NOTICE("bmsp_update_sync_sel1_intr_stats\n");

	bmsp_stats[linear_id].sync_sel1_intr_count++;
	if (type == BMSP_HANDLE_SEL1_INTR_AND_RETURN)
		bmsp_stats[linear_id].sync_sel1_intr_ret_count++;

#if LOG_LEVEL >= LOG_LEVEL_VERBOSE
	spin_lock(&console_lock);
	VERBOSE("BMSP: cpu 0x%lx sync s-el1 interrupt request from 0x%lx\n",
		read_mpidr(), elr_el3);
	VERBOSE("BMSP: cpu 0x%lx: %d sync s-el1 interrupt requests, %d sync s-el1 interrupt returns\n",
		read_mpidr(),
		bmsp_stats[linear_id].sync_sel1_intr_count,
		bmsp_stats[linear_id].sync_sel1_intr_ret_count);
	spin_unlock(&console_lock);
#endif
}

/******************************************************************************
 * This function is invoked when a non S-EL1 interrupt is received and causes
 * the preemption of BMSP. This function returns BMSP_PREEMPTED and results
 * in the control being handed over to EL3 for handling the interrupt.
 *****************************************************************************/
int32_t bmsp_handle_preemption(void)
{
	uint32_t linear_id = plat_my_core_pos();

	NOTICE("bmsp_handle_preemption\n");

	bmsp_stats[linear_id].preempt_intr_count++;
#if LOG_LEVEL >= LOG_LEVEL_VERBOSE
	spin_lock(&console_lock);
	VERBOSE("BMSP: cpu 0x%lx: %d preempt interrupt requests\n",
		read_mpidr(), bmsp_stats[linear_id].preempt_intr_count);
	spin_unlock(&console_lock);
#endif
	return BMSP_PREEMPTED;
}

/*******************************************************************************
 * BMSP interrupt handler is called as a part of both synchronous and
 * asynchronous handling of BMSP interrupts. Currently the physical timer
 * interrupt is the only S-EL1 interrupt that this handler expects. It returns
 * 0 upon successfully handling the expected interrupt and all other
 * interrupts are treated as normal world or EL3 interrupts.
 ******************************************************************************/
int32_t bmsp_common_int_handler(void)
{
	uint32_t linear_id = plat_my_core_pos(), id;

	/*
	 * Get the highest priority pending interrupt id and see if it is the
	 * secure physical generic timer interrupt in which case, handle it.
	 * Otherwise throw this interrupt at the EL3 firmware.
	 *
	 * There is a small time window between reading the highest priority
	 * pending interrupt and acknowledging it during which another
	 * interrupt of higher priority could become the highest pending
	 * interrupt. This is not expected to happen currently for BMSP.
	 */
	id = plat_ic_get_pending_interrupt_id();

	NOTICE("BMSP: intrid=0x%x\n", id);

	/* BMSP can only handle the secure physical timer interrupt */
	if (id != BMSP_IRQ_SEC_PHY_TIMER)
		return bmsp_handle_preemption();

	/*
	 * Acknowledge and handle the secure timer interrupt. Also sanity check
	 * if it has been preempted by another interrupt through an assertion.
	 */
	id = plat_ic_acknowledge_interrupt();
	assert(id == BMSP_IRQ_SEC_PHY_TIMER);
	bmsp_generic_timer_handler();
	plat_ic_end_of_interrupt(id);

	/* Update the statistics and print some messages */
	bmsp_stats[linear_id].sel1_intr_count++;
#if LOG_LEVEL >= LOG_LEVEL_VERBOSE
	spin_lock(&console_lock);
	VERBOSE("BMSP: cpu 0x%lx handled S-EL1 interrupt %d\n",
	       read_mpidr(), id);
	VERBOSE("BMSP: cpu 0x%lx: %d S-EL1 requests\n",
	     read_mpidr(), bmsp_stats[linear_id].sel1_intr_count);
	spin_unlock(&console_lock);
#endif
	return 0;
}
