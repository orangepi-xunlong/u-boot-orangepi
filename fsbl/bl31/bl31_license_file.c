/*
 * Copyright (c) 2013-2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <arch.h>
#include <arch_helpers.h>
#include <assert.h>
#include <bl_common.h>
#include <bl31.h>
#include <console.h>
#include <context_mgmt.h>
#include <debug.h>
#include <platform.h>
#include <pmf.h>
#include <runtime_instr.h>
#include <runtime_svc.h>
#include <string.h>
#include <mmio.h>
#include <license_file.h>
#include "cv_private.h"
#include "cv_security.h"

#if !defined(MBEDTLS_CONFIG_FILE)
#include "config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include <mbedtls/memory_buffer_alloc.h>
#include <mbedtls/platform.h>

#include "scheme-private.h"
#include "scheme.h"

static void *sc_malloc(size_t s)
{
	return mbedtls_calloc(1, s);
}

static void sc_free(void *p)
{
	mbedtls_free(p);
}

int bl31_run_license(const char *license, void *buf, size_t buf_size)
{
	scheme sc_ctx;
	pointer args;
	int ret = -1;

	VERBOSE("[%s:%d] license=%s\n", __func__, __LINE__, license);
	VERBOSE("------------\n");

	VERBOSE("[%s:%d] buf=%p(%lu)\n", __func__, __LINE__, buf,
	       buf_size);

	mbedtls_memory_buffer_alloc_init(buf, buf_size);

	if (!scheme_init_custom_alloc(&sc_ctx, sc_malloc, sc_free)) {
		ERROR("scheme_init_custom_alloc() failed\n");
		return -1;
	}

	args = mk_integer(&sc_ctx, 0x31);
	scheme_define(&sc_ctx, sc_ctx.global_env, mk_symbol(&sc_ctx, "args"),
		      args);

	NOTICE("K.");
	scheme_load_string(&sc_ctx, license);
	ret = sc_ctx.retcode;

	NOTICE("L%d\n", ret);
	if (ret < 1) {
		ERROR("Invalid License File\n");
		do_wdt_reset_direct(__FILE__, __LINE__, 0);
		while (1)
			;
	}

	mbedtls_memory_buffer_alloc_free();

	return ret;
}

void bl31_check_license(void)
{
#if MARS
	// No TEE
#else
	char *license_file = (char *)(LICENSE_FILE_BUF_BASE + LICENSE_FILE_DIGEST_SIZE);
	void *buf = (void *)BL32_LIMIT;

	mmio_setbits_32(SEC_FABFW_PERI_FW_S_TZ_S, SEC_FABFW_PERI_FW_S_TZ_S_EFUSE);

	license_file[LICENSE_FILE_MAX_SIZE - 1] = '\0';

	if (strcmp(license_file, "SKIP_LICENSE_CHECK") == 0) {
		NOTICE("No IMG_ENC. Skip license check.");
		return;
	}

	// Use LICENSE_FILE_BASE as the scheme runtime context
	bl31_run_license(license_file, buf, LICENSE_FILE_RUNTIME_SIZE);
#endif
}
