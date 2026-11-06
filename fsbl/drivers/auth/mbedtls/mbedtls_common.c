/*
 * Copyright (c) 2015-2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <debug.h>

/* mbed TLS headers */
#include <mbedtls/memory_buffer_alloc.h>
#include <mbedtls/platform.h>
#include <mbedtls_config.h>
#include <arch_helpers.h>

/*
 * mbed TLS heap
 */
#if defined(IMAGE_BL32)
#define MBEDTLS_HEAP_SIZE		(128*1024)
#else
#if (TF_MBEDTLS_KEY_ALG_ID == TF_MBEDTLS_ECDSA)
#define MBEDTLS_HEAP_SIZE		(14*1024)
#elif (TF_MBEDTLS_KEY_ALG_ID == TF_MBEDTLS_RSA)
#define MBEDTLS_HEAP_SIZE		(7*1024)
#endif
#endif /* IMAGE_BLX */

static unsigned char heap[MBEDTLS_HEAP_SIZE] __aligned(16);

/*
 * mbed TLS initialization function
 */

#if defined(MBEDTLS_ARMV8CE_AES_C)
/* Enable FP/SIMD */
static void mbedtls_setup_armv8ce(void)
{
	if (IS_IN_EL1()) {
		write_cpacr_el1(read_cpacr_el1() |
				CPACR_EL1_FPEN(CPACR_EL1_FP_TRAP_EL0));
	}
}
#endif

void mbedtls_init(void)
{
	static int ready;

	if (!ready) {
#if defined(MBEDTLS_ARMV8CE_AES_C)
		mbedtls_setup_armv8ce();
#endif

		/* Initialize the mbed TLS heap */
		mbedtls_memory_buffer_alloc_init(heap, MBEDTLS_HEAP_SIZE);

#ifdef MBEDTLS_PLATFORM_SNPRINTF_ALT
		/* Use reduced version of snprintf to save space. */
		mbedtls_platform_set_snprintf(tf_snprintf);
#endif

		ready = 1;
	}
}
