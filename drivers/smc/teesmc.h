/*
 * Copyright (c) 2014, Linaro Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */
#ifndef TEESMC_H
#define TEESMC_H

/*
 *******************************************************************************
 * Part 2 - low level SMC interaction
 *******************************************************************************
 */

#define TEESMC_32			0
#define TEESMC_64			0x40000000
#define TEESMC_FAST_CALL		0x80000000
#define TEESMC_STD_CALL			0

#define TEESMC_OWNER_MASK		0x3F
#define TEESMC_OWNER_SHIFT		24

#define TEESMC_FUNC_MASK		0xFFFF

#define TEESMC_IS_FAST_CALL(smc_val)	((smc_val) & TEESMC_FAST_CALL)
#define TEESMC_IS_64(smc_val)		((smc_val) & TEESMC_64)
#define TEESMC_FUNC_NUM(smc_val)	((smc_val) & TEESMC_FUNC_MASK)
#define TEESMC_OWNER_NUM(smc_val)	(((smc_val) >> TEESMC_OWNER_SHIFT) & \
					 TEESMC_OWNER_MASK)

#define TEESMC_CALL_VAL(type, calling_convention, owner, func_num) \
			((type) | (calling_convention) | \
			(((owner) & TEESMC_OWNER_MASK) << TEESMC_OWNER_SHIFT) |\
			((func_num) & TEESMC_FUNC_MASK))

#define TEESMC_OWNER_ARCH		0
#define TEESMC_OWNER_CPU		1
#define TEESMC_OWNER_SIP		2
#define TEESMC_OWNER_OEM		3
#define TEESMC_OWNER_STANDARD		4
#define TEESMC_OWNER_TRUSTED_APP	48
#define TEESMC_OWNER_TRUSTED_OS		50

#define TEESMC_OWNER_TRUSTED_OS_API	63


#define TEESMC_FUNCID_SSK_CRYP      16
#define TEESMC32_CALL_SSK_CRYP \
	TEESMC_CALL_VAL(TEESMC_32, TEESMC_FAST_CALL, TEESMC_OWNER_TRUSTED_OS, \
			TEESMC_FUNCID_SSK_CRYP)

#define TEESMC_SSK_ENCRYPT       0
#define TEESMC_KEYBOX_STORE      1
#define TEESMC_PROBE_SHM_BASE    2
#define TEESMC_SSK_DECRYPT       3
#define TEESMC_RSSK_ENCRYPT      4
#define TEESMC_RSSK_DECRYPT      5
#define TEESMC_AES_CBC           6



struct smc_param {
	uint32_t a0;
	uint32_t a1;
	uint32_t a2;
	uint32_t a3;
	uint32_t a4;
	uint32_t a5;
	uint32_t a6;
	uint32_t a7;
};

extern void tee_smc_call(struct smc_param *param);


#endif /* TEESMC_H */
