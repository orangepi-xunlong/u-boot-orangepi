/*
 * Copyright (c) 2013-2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <arch_helpers.h>
#include <bl_common.h>
#include <debug.h>
#include <platform.h>
#include <platform_def.h>
#include <platform_cvsp.h>
#include <spinlock.h>
#include <console.h>
#include <opteed_private.h>
#include <teesmc_opteed_macros.h>
#include <teesmc_opteed.h>
#include <optee_msg.h>
#include <optee_smc.h>
#include <cvsp.h>
#include "delay_timer.h"
#include "mbedtls/aes.h"
#include "cvsp_private.h"

struct _time_records *time_records = (void *)TIME_RECORDS_ADDR;

/*******************************************************************************
 * Lock to control access to the console
 ******************************************************************************/
spinlock_t console_lock;

/*******************************************************************************
 * Per cpu data structure to populate parameters for an SMC in C code and use
 * a pointer to this structure in assembler code to populate x0-x7
 ******************************************************************************/
static bmsp_args_t bmsp_smc_args[PLATFORM_CORE_COUNT];

/*******************************************************************************
 * Per cpu data structure to keep track of BMSP activity
 ******************************************************************************/
work_statistics_t bmsp_stats[PLATFORM_CORE_COUNT];

/*******************************************************************************
 * The BMSP memory footprint starts at address BL32_BASE and ends with the
 * linker symbol __BL32_END__. Use these addresses to compute the BMSP image
 * size.
 ******************************************************************************/
#define BL32_TOTAL_LIMIT (unsigned long)(&__BL32_END__)
#define BL32_TOTAL_SIZE (BL32_TOTAL_LIMIT - (unsigned long) BL32_BASE)

// 0000000: 8860 0b30 a575 ed88 9c46 ec83 500d d07d  .`.0.u...F..P..}
// 00000010: 0a
unsigned char encrypt_text[] = {
	0x60, 0x88, 0x30, 0x0b, 0x75, 0xa5, 0x88, 0xed, 0x46, 0x9c, 0x83, 0xec, 0x0d, 0x50, 0x7d, 0xd0,
	0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 00000000: 6865 6c6c 6f20 776f 726c 640a            hello world.
unsigned char plain_text[] = {
	0x65, 0x68, 0x6c, 0x6c, 0x20, 0x6f, 0x6f, 0x77, 0x6c, 0x72, 0x0a, 0x64,
};

unsigned char decrypt_buf[1024];

volatile int dbg_wait = 1;

static bmsp_args_t *set_smc_args(uint64_t arg0,
			     uint64_t arg1,
			     uint64_t arg2,
			     uint64_t arg3,
			     uint64_t arg4,
			     uint64_t arg5,
			     uint64_t arg6,
			     uint64_t arg7)
{
	uint32_t linear_id;
	bmsp_args_t *pcpu_smc_args;

	/*
	 * Return to Secure Monitor by raising an SMC. The results of the
	 * service are passed as an arguments to the SMC
	 */
	linear_id = plat_my_core_pos();
	pcpu_smc_args = &bmsp_smc_args[linear_id];
	write_sp_arg(pcpu_smc_args, BMSP_ARG0, arg0);
	write_sp_arg(pcpu_smc_args, BMSP_ARG1, arg1);
	write_sp_arg(pcpu_smc_args, BMSP_ARG2, arg2);
	write_sp_arg(pcpu_smc_args, BMSP_ARG3, arg3);
	write_sp_arg(pcpu_smc_args, BMSP_ARG4, arg4);
	write_sp_arg(pcpu_smc_args, BMSP_ARG5, arg5);
	write_sp_arg(pcpu_smc_args, BMSP_ARG6, arg6);
	write_sp_arg(pcpu_smc_args, BMSP_ARG7, arg7);

	return pcpu_smc_args;
}

bmsp_args_t *bmsp_set_smc_args(uint64_t arg0, uint64_t arg1, uint64_t arg2,
			       uint64_t arg3, uint64_t arg4, uint64_t arg5,
			       uint64_t arg6, uint64_t arg7)
{
	return set_smc_args(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}

/*******************************************************************************
 * BMSP main entry point where it gets the opportunity to initialize its secure
 * state/applications. Once the state is initialized, it must return to the
 * SPD with a pointer to the 'bmsp_vector_table' jump table.
 ******************************************************************************/
uint64_t bmsp_main(void)
{
	time_records->bl32_start = read_time_ms();
	VERBOSE("\n#bl32_start at %dms#\n", time_records->bl32_start);
	INFO("%s\n", version_string);
	INFO("%s\n", build_message);
	NOTICE("B%lx-%lx.", (unsigned long) BL32_BASE, BL32_TOTAL_SIZE);
	// dw_timer_start(TIMER_ID3);

	uint32_t linear_id = plat_my_core_pos();

	/* Initialize the platform */
	bmsp_platform_setup();

	/* Initialize secure/applications state here */
	//bmsp_generic_timer_start();

	/* Update this cpu's statistics */
	bmsp_stats[linear_id].smc_count++;
	bmsp_stats[linear_id].eret_count++;
	bmsp_stats[linear_id].cpu_on_count++;

#if LOG_LEVEL >= LOG_LEVEL_INFO
	spin_lock(&console_lock);
	INFO("BMSP: cpu 0x%lx: %d smcs, %d erets %d cpu on requests\n",
	     read_mpidr(),
	     bmsp_stats[linear_id].smc_count,
	     bmsp_stats[linear_id].eret_count,
	     bmsp_stats[linear_id].cpu_on_count);
	spin_unlock(&console_lock);
#endif
	NOTICE("start BL32 \n");
	// dw_timer_stop(TIMER_ID3, (uint32_t *)BL32_TIME_ADDR);

	// Disable BL32 message in default
	console_uninit();

	return (uint64_t) &bmsp_vector_table;
}

/*******************************************************************************
 * This function performs any remaining book keeping in the test secure payload
 * after this cpu's architectural state has been setup in response to an earlier
 * psci cpu_on request.
 ******************************************************************************/
bmsp_args_t *bmsp_cpu_on_main(void)
{
	uint32_t linear_id = plat_my_core_pos();

	VERBOSE("bmsp_cpu_on_main: TEESMC_OPTEED_RETURN_ENTRY_DONE=0x%llx\n",
		TEESMC_OPTEED_RETURN_ENTRY_DONE);

	/* Initialize secure/applications state here */
	// bmsp_generic_timer_start();

	/* Update this cpu's statistics */
	bmsp_stats[linear_id].smc_count++;
	bmsp_stats[linear_id].eret_count++;
	bmsp_stats[linear_id].cpu_on_count++;

#if LOG_LEVEL >= LOG_LEVEL_INFO
	spin_lock(&console_lock);
	INFO("BMSP: cpu 0x%lx turned on\n", read_mpidr());
	INFO("BMSP: cpu 0x%lx: %d smcs, %d erets %d cpu on requests\n",
		read_mpidr(),
		bmsp_stats[linear_id].smc_count,
		bmsp_stats[linear_id].eret_count,
		bmsp_stats[linear_id].cpu_on_count);
	spin_unlock(&console_lock);
#endif
	/* Indicate to the SPD that we have completed turned ourselves on */
	return set_smc_args(TEESMC_OPTEED_RETURN_ON_DONE, 0, 0, 0, 0, 0, 0, 0);
}

/*******************************************************************************
 * This function performs any remaining book keeping in the test secure payload
 * before this cpu is turned off in response to a psci cpu_off request.
 ******************************************************************************/
bmsp_args_t *bmsp_cpu_off_main(uint64_t arg0,
			   uint64_t arg1,
			   uint64_t arg2,
			   uint64_t arg3,
			   uint64_t arg4,
			   uint64_t arg5,
			   uint64_t arg6,
			   uint64_t arg7)
{
	uint32_t linear_id = plat_my_core_pos();

	/*
	 * This cpu is being turned off, so disable the timer to prevent the
	 * secure timer interrupt from interfering with power down. A pending
	 * interrupt will be lost but we do not care as we are turning off.
	 */
	// bmsp_generic_timer_stop();

	/* Update this cpu's statistics */
	bmsp_stats[linear_id].smc_count++;
	bmsp_stats[linear_id].eret_count++;
	bmsp_stats[linear_id].cpu_off_count++;

#if LOG_LEVEL >= LOG_LEVEL_INFO
	spin_lock(&console_lock);
	INFO("BMSP: cpu 0x%lx off request\n", read_mpidr());
	INFO("BMSP: cpu 0x%lx: %d smcs, %d erets %d cpu off requests\n",
		read_mpidr(),
		bmsp_stats[linear_id].smc_count,
		bmsp_stats[linear_id].eret_count,
		bmsp_stats[linear_id].cpu_off_count);
	spin_unlock(&console_lock);
#endif

	/* Indicate to the SPD that we have completed this request */
	return set_smc_args(TEESMC_OPTEED_RETURN_OFF_DONE, 0, 0, 0, 0, 0, 0, 0);
}


/*******************************************************************************
 * This function performs any book keeping in the test secure payload before
 * this cpu's architectural state is saved in response to an earlier psci
 * cpu_suspend request.
 ******************************************************************************/
bmsp_args_t *bmsp_cpu_suspend_main(uint64_t arg0,
			       uint64_t arg1,
			       uint64_t arg2,
			       uint64_t arg3,
			       uint64_t arg4,
			       uint64_t arg5,
			       uint64_t arg6,
			       uint64_t arg7)
{
	uint32_t linear_id = plat_my_core_pos();

	/*
	 * Save the time context and disable it to prevent the secure timer
	 * interrupt from interfering with wakeup from the suspend state.
	 */
	// bmsp_generic_timer_save();
	// bmsp_generic_timer_stop();

	/* Update this cpu's statistics */
	bmsp_stats[linear_id].smc_count++;
	bmsp_stats[linear_id].eret_count++;
	bmsp_stats[linear_id].cpu_suspend_count++;

#if LOG_LEVEL >= LOG_LEVEL_INFO
	spin_lock(&console_lock);
	INFO("BMSP: cpu 0x%lx: %d smcs, %d erets %d cpu suspend requests\n",
		read_mpidr(),
		bmsp_stats[linear_id].smc_count,
		bmsp_stats[linear_id].eret_count,
		bmsp_stats[linear_id].cpu_suspend_count);
	spin_unlock(&console_lock);
#endif

	/* Indicate to the SPD that we have completed this request */
	return set_smc_args(TEESMC_OPTEED_RETURN_SUSPEND_DONE, 0, 0, 0, 0, 0, 0, 0);
}

/*******************************************************************************
 * This function performs any book keeping in the test secure payload after this
 * cpu's architectural state has been restored after wakeup from an earlier psci
 * cpu_suspend request.
 ******************************************************************************/
bmsp_args_t *bmsp_cpu_resume_main(uint64_t max_off_pwrlvl,
			      uint64_t arg1,
			      uint64_t arg2,
			      uint64_t arg3,
			      uint64_t arg4,
			      uint64_t arg5,
			      uint64_t arg6,
			      uint64_t arg7)
{
	uint32_t linear_id = plat_my_core_pos();

	/* Restore the generic timer context */
	// bmsp_generic_timer_restore();

	/* Update this cpu's statistics */
	bmsp_stats[linear_id].smc_count++;
	bmsp_stats[linear_id].eret_count++;
	bmsp_stats[linear_id].cpu_resume_count++;

#if LOG_LEVEL >= LOG_LEVEL_INFO
	spin_lock(&console_lock);
	INFO("BMSP: cpu 0x%lx resumed. maximum off power level %ld\n",
	     read_mpidr(), max_off_pwrlvl);
	INFO("BMSP: cpu 0x%lx: %d smcs, %d erets %d cpu suspend requests\n",
		read_mpidr(),
		bmsp_stats[linear_id].smc_count,
		bmsp_stats[linear_id].eret_count,
		bmsp_stats[linear_id].cpu_suspend_count);
	spin_unlock(&console_lock);
#endif
	/* Indicate to the SPD that we have completed this request */
	return set_smc_args(TEESMC_OPTEED_RETURN_RESUME_DONE, 0, 0, 0, 0, 0, 0, 0);
}

/*******************************************************************************
 * This function performs any remaining bookkeeping in the test secure payload
 * before the system is switched off (in response to a psci SYSTEM_OFF request)
 ******************************************************************************/
bmsp_args_t *bmsp_system_off_main(uint64_t arg0,
				uint64_t arg1,
				uint64_t arg2,
				uint64_t arg3,
				uint64_t arg4,
				uint64_t arg5,
				uint64_t arg6,
				uint64_t arg7)
{
	uint32_t linear_id = plat_my_core_pos();

	/* Update this cpu's statistics */
	bmsp_stats[linear_id].smc_count++;
	bmsp_stats[linear_id].eret_count++;

#if LOG_LEVEL >= LOG_LEVEL_INFO
	spin_lock(&console_lock);
	INFO("BMSP: cpu 0x%lx SYSTEM_OFF request\n", read_mpidr());
	INFO("BMSP: cpu 0x%lx: %d smcs, %d erets requests\n", read_mpidr(),
	     bmsp_stats[linear_id].smc_count,
	     bmsp_stats[linear_id].eret_count);
	spin_unlock(&console_lock);
#endif

	/* Indicate to the SPD that we have completed this request */
	return set_smc_args(TEESMC_OPTEED_RETURN_SYSTEM_OFF_DONE, 0, 0, 0, 0, 0, 0, 0);
}

/*******************************************************************************
 * This function performs any remaining bookkeeping in the test secure payload
 * before the system is reset (in response to a psci SYSTEM_RESET request)
 ******************************************************************************/
bmsp_args_t *bmsp_system_reset_main(uint64_t arg0,
				uint64_t arg1,
				uint64_t arg2,
				uint64_t arg3,
				uint64_t arg4,
				uint64_t arg5,
				uint64_t arg6,
				uint64_t arg7)
{
	uint32_t linear_id = plat_my_core_pos();

	/* Update this cpu's statistics */
	bmsp_stats[linear_id].smc_count++;
	bmsp_stats[linear_id].eret_count++;

#if LOG_LEVEL >= LOG_LEVEL_INFO
	spin_lock(&console_lock);
	INFO("BMSP: cpu 0x%lx SYSTEM_RESET request\n", read_mpidr());
	INFO("BMSP: cpu 0x%lx: %d smcs, %d erets requests\n", read_mpidr(),
	     bmsp_stats[linear_id].smc_count,
	     bmsp_stats[linear_id].eret_count);
	spin_unlock(&console_lock);
#endif

	/* Indicate to the SPD that we have completed this request */
	return set_smc_args(TEESMC_OPTEED_RETURN_SYSTEM_RESET_DONE, 0, 0, 0, 0, 0, 0, 0);
}

volatile uint32_t dbg_block = 1;

/*******************************************************************************
 * BMSP fast smc handler. The secure monitor jumps to this function by
 * doing the ERET after populating X0-X7 registers. The arguments are received
 * in the function arguments in order. Once the service is rendered, this
 * function returns to Secure Monitor by raising SMC.
 ******************************************************************************/
bmsp_args_t *bmsp_smc_handler(uint64_t func,
			       uint64_t arg1,
			       uint64_t arg2,
			       uint64_t arg3,
			       uint64_t arg4,
			       uint64_t arg5,
			       uint64_t arg6,
			       uint64_t arg7)
{
	//uint64_t results[2];
	//uint64_t service_args[2];
	uint32_t linear_id = plat_my_core_pos();

	/* Update this cpu's statistics */
	bmsp_stats[linear_id].smc_count++;
	bmsp_stats[linear_id].eret_count++;

	INFO("BMSP: func=0x%x, arg1=0x%x, arg2=0x%x, arg3=0x%x, arg4=0x%x\n",
	       (uint32_t)func, (uint32_t)arg1, (uint32_t)arg2, (uint32_t)arg3, (uint32_t)arg4);

	INFO("BMSP: cpu 0x%lx received %s smc 0x%lx\n", read_mpidr(),
		((func >> 31) & 1) == 1 ? "fast" : "yielding",
		func);
	INFO("BMSP: cpu 0x%lx: %d smcs, %d erets\n", read_mpidr(),
		bmsp_stats[linear_id].smc_count,
		bmsp_stats[linear_id].eret_count);

	/* Render secure services and obtain results here */
	//results[0] = arg1;
	//results[1] = arg2;

	/*
	 * Request a service back from dispatcher/secure monitor. This call
	 * return and thereafter resume execution
	 */
	if (OPTEE_SMC_OWNER_NUM(func) == OPTEE_SMC_OWNER_OEM) {
		return cvsp_private_smc_handler(func, arg1, arg2, arg3, arg4,
					       arg5, arg6, arg7);
	}

	switch (func) {
	case OPTEE_SMC_CALLS_UID:
	{
		VERBOSE("    OPTEE_SMC_CALLS_UID\n");
		/*
		 * Return the following UID if using API specified in this file without
		 * further extensions:
		 * 384fb3e0-e7f8-11e3-af63-0002a5d5c51b.
		 * Represented in 4 32-bit words in OPTEE_MSG_UID_0, OPTEE_MSG_UID_1,
		 * OPTEE_MSG_UID_2, OPTEE_MSG_UID_3.
		 */
		#define OPTEE_MSG_UID_0		0x384fb3e0
		#define OPTEE_MSG_UID_1		0xe7f811e3
		#define OPTEE_MSG_UID_2		0xaf630002
		#define OPTEE_MSG_UID_3		0xa5d5c51b

		return set_smc_args(TEESMC_OPTEED_RETURN_CALL_DONE,
				OPTEE_MSG_UID_0,
				OPTEE_MSG_UID_1,
				OPTEE_MSG_UID_2,
				OPTEE_MSG_UID_3,
				0, 0, 0);
	}
	break;

	case OPTEE_SMC_CALLS_REVISION:
	{
		return set_smc_args(TEESMC_OPTEED_RETURN_CALL_DONE,
				OPTEE_MSG_REVISION_MAJOR,
				OPTEE_MSG_REVISION_MINOR,
				0,
				0, 0, 0, 0);
	}
	break;

	case OPTEE_SMC_GET_SHM_CONFIG:
	{
		/*
		 * args->a0 = OPTEE_SMC_RETURN_OK;
		 * args->a1 = default_nsec_shm_paddr;
		 * args->a2 = default_nsec_shm_size;
		 * args->a3 = core_mmu_is_shm_cached();
		 */
		return set_smc_args(TEESMC_OPTEED_RETURN_CALL_DONE,
				0,
				0x100000000,	/* SRAM_BASE */
				512*1024,	/* 512kB */
				1,		/* must cached for linux */
				0, 0, 0);
	}
	break;

	case OPTEE_SMC_EXCHANGE_CAPABILITIES:
	{
		return set_smc_args(TEESMC_OPTEED_RETURN_CALL_DONE,
				0,
				1,
				0,
				0, 0, 0, 0);
	}
	break;

	case OPTEE_SMC_ENABLE_SHM_CACHE:
	{
		return set_smc_args(TEESMC_OPTEED_RETURN_CALL_DONE,
				0,
				0,
				0,
				0, 0, 0, 0);
	}
	break;

	/* 0x30000000 – 0x31000000 Trusted Application Calls */
	/* 0x32000000 – 0x3F000000 Trusted OS Calls */
	case OPTEE_SMC_CALL_WITH_ARG:
	{
		VERBOSE("  OPTEE_SMC_CALL_WITH_ARG\n");
		return set_smc_args(TEESMC_OPTEED_RETURN_CALL_DONE, 0, 0, 0, 0,
				    0, 0, 0);
	}
	break;

	default:
	break;
	}

	/* Always return valid argument, otherwise opteed will assert */
	return set_smc_args(TEESMC_OPTEED_RETURN_CALL_DONE, 0,
			    0/*results[0]*/,
			    0/*results[1]*/,
			    0, 0, 0, 0);
}

/*******************************************************************************
 * BMSP smc abort handler. This function is called when aborting a preemtped
 * yielding SMC request. It should cleanup all resources owned by the SMC
 * handler such as locks or dynamically allocated memory so following SMC
 * request are executed in a clean environment.
 ******************************************************************************/
bmsp_args_t *bmsp_abort_smc_handler(uint64_t func,
				  uint64_t arg1,
				  uint64_t arg2,
				  uint64_t arg3,
				  uint64_t arg4,
				  uint64_t arg5,
				  uint64_t arg6,
				  uint64_t arg7)
{
	VERBOSE("BMSP: abort, func 0x%x\n", (uint32_t)func);

	return set_smc_args(BMSP_ABORT_DONE, 0, 0, 0, 0, 0, 0, 0);
}
