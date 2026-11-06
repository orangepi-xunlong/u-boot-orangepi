// SPDX-License-Identifier: GPL-2.0+
/*
 * Advanced Crypto Engine - SHA Firmware
 * Copyright (c) 2012  Sophon
 */
#include <common.h>
#include <log.h>
#include <linux/errno.h>
#include <linux/arm-smccc.h>
#include <cpu_func.h>

#define OPTEE_SMC_CALL_CV_SHA256		0x0300000E

int sophon_spacc_sha256(const unsigned char *pbuf, unsigned int buf_len
				, unsigned char *pout)
{
	struct arm_smccc_res res = {0};

	flush_dcache_all();
	arm_smccc_smc(OPTEE_SMC_CALL_CV_SHA256, (unsigned long)pbuf, buf_len,
		      (unsigned long)pout, 0, 0, 0, 0, &res);

	invalidate_dcache_all();

	return 0;
}

void hw_sha256(const unsigned char *pbuf, unsigned int buf_len
				, unsigned char *pout, unsigned int chunk_size)
{
	if (sophon_spacc_sha256(pbuf, buf_len, pout))
		debug("ACE was not setup properly or it is faulty\n");
}

