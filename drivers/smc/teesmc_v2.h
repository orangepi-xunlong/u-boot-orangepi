/*
 * (C) Copyright 2018 allwinnertech  <wangwei@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef TEESMC25_H
#define TEESMC25_H

/* for optee v2.5 */
#define ARM_SMCCC_STD_CALL		0
#define ARM_SMCCC_FAST_CALL		1u
#define ARM_SMCCC_TYPE_SHIFT		31

#define ARM_SMCCC_SMC_32		0
#define ARM_SMCCC_SMC_64		1
#define ARM_SMCCC_CALL_CONV_SHIFT	30

#define ARM_SMCCC_OWNER_MASK		0x3F
#define ARM_SMCCC_OWNER_SHIFT		24

#define ARM_SMCCC_FUNC_MASK		0xFFFF

#define ARM_SMCCC_IS_FAST_CALL(smc_val)	\
	((smc_val) & (ARM_SMCCC_FAST_CALL << ARM_SMCCC_TYPE_SHIFT))
#define ARM_SMCCC_IS_64(smc_val) \
	((smc_val) & (ARM_SMCCC_SMC_64 << ARM_SMCCC_CALL_CONV_SHIFT))
#define ARM_SMCCC_FUNC_NUM(smc_val)	((smc_val) & ARM_SMCCC_FUNC_MASK)
#define ARM_SMCCC_OWNER_NUM(smc_val) \
	(((smc_val) >> ARM_SMCCC_OWNER_SHIFT) & ARM_SMCCC_OWNER_MASK)

#define ARM_SMCCC_CALL_VAL(type, calling_convention, owner, func_num) \
	(((type) << ARM_SMCCC_TYPE_SHIFT) | \
	((calling_convention) << ARM_SMCCC_CALL_CONV_SHIFT) | \
	(((owner) & ARM_SMCCC_OWNER_MASK) << ARM_SMCCC_OWNER_SHIFT) | \
	((func_num) & ARM_SMCCC_FUNC_MASK))

#define ARM_SMCCC_OWNER_ARCH		0
#define ARM_SMCCC_OWNER_CPU		1
#define ARM_SMCCC_OWNER_SIP		2
#define ARM_SMCCC_OWNER_OEM		3
#define ARM_SMCCC_OWNER_STANDARD	4
#define ARM_SMCCC_OWNER_TRUSTED_APP	48
#define ARM_SMCCC_OWNER_TRUSTED_APP_END	49
#define ARM_SMCCC_OWNER_TRUSTED_OS	50
#define ARM_SMCCC_OWNER_TRUSTED_OS_END	63


#define OPTEE_SMC_FAST_CALL_VAL(func_num) \
	ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL, ARM_SMCCC_SMC_32, \
			   ARM_SMCCC_OWNER_TRUSTED_OS, (func_num))

/*
 * Get revision of Trusted OS.
 *
 * Used by non-secure world to figure out which version of the Trusted OS
 * is installed. Note that the returned revision is the revision of the
 * Trusted OS, not of the API.
 *
 * Returns revision in 2 32-bit words in the same way as
 * OPTEE_MSG_CALLS_REVISION described above.
 */
#define OPTEE_MSG_FUNCID_GET_OS_REVISION	0x0001

 /*
  * Get revision of Trusted OS.
  *
  * Used by non-secure world to figure out which version of the Trusted OS
  * is installed. Note that the returned revision is the revision of the
  * Trusted OS, not of the API.
  *
  * Returns revision in a0-1 in the same way as OPTEE_SMC_CALLS_REVISION
  * described above. May optionally return a 32-bit build identifier in a2,
  * with zero meaning unspecified.
  */
 #define OPTEE_SMC_FUNCID_GET_OS_REVISION OPTEE_MSG_FUNCID_GET_OS_REVISION
 #define OPTEE_SMC_CALL_GET_OS_REVISION \
		OPTEE_SMC_FAST_CALL_VAL(OPTEE_SMC_FUNCID_GET_OS_REVISION)

#define SUNXI_OPTEE_SMC_OFFSET (0x200)
extern uint32_t sunxi_smc_call_offset;

#define OPTEE_SMC_FUNCID_GET_DRMINFO	(15 | sunxi_smc_call_offset)
#define OPTEE_SMC_GET_DRM_INFO \
	OPTEE_SMC_FAST_CALL_VAL(OPTEE_SMC_FUNCID_GET_DRMINFO)

#define OPTEE_SMC_FUNCID_CRYPT  (16 | sunxi_smc_call_offset)
#define OPTEE_SMC_CRYPT \
	OPTEE_SMC_FAST_CALL_VAL(OPTEE_SMC_FUNCID_CRYPT)


#define OPTEE_SMC_FUNCID_READ_REG  (17 | sunxi_smc_call_offset)
#define OPTEE_SMC_READ_REG \
	OPTEE_SMC_FAST_CALL_VAL(OPTEE_SMC_FUNCID_READ_REG)


#define OPTEE_SMC_FUNCID_WRITE_REG  (18 | sunxi_smc_call_offset)
#define OPTEE_SMC_WRITE_REG \
	OPTEE_SMC_FAST_CALL_VAL(OPTEE_SMC_FUNCID_WRITE_REG)

#define OPTEE_SMC_FUNCID_ARISC_COPY_PARAS  (19 | sunxi_smc_call_offset)
#define OPTEE_SMC_ARISC_COPY_PARAS \
	OPTEE_SMC_FAST_CALL_VAL(OPTEE_SMC_FUNCID_ARISC_COPY_PARAS)

#define OPTEE_SMC_FUNCID_GET_TEEADDR_PARAS  (20 | sunxi_smc_call_offset)
#define OPTEE_SMC_GET_TEEADDR_PARAS \
	OPTEE_SMC_FAST_CALL_VAL(OPTEE_SMC_FUNCID_GET_TEEADDR_PARAS)

#define OPTEE_SMC_FUNCID_EFUSE  (21 | sunxi_smc_call_offset)
#define OPTEE_SMC_EFUSE_OP \
	OPTEE_SMC_FAST_CALL_VAL(OPTEE_SMC_FUNCID_EFUSE)
#define EFUSE_OP_WR 1
#define EFUSE_OP_RD 0

#define OPTEE_SMC_FUNCIC_SUNXI_HASH (23 | sunxi_smc_call_offset)
#define OPTEE_SMC_SUNXI_HASH_OP \
		OPTEE_SMC_FAST_CALL_VAL(OPTEE_SMC_FUNCIC_SUNXI_HASH)

#define OPTEE_SMC_FUNCIC_SUNXI_SETUP_MIPS (24 + SUNXI_OPTEE_SMC_OFFSET)
#define OPTEE_SMC_SUNXI_SETUP_MIPS \
		OPTEE_SMC_FAST_CALL_VAL(OPTEE_SMC_FUNCIC_SUNXI_SETUP_MIPS)

#define OPTEE_SMC_FUNCID_SUNXI_INFORM_FDT (25 | SUNXI_OPTEE_SMC_OFFSET)
#define OPTEE_SMC_SUNXI_INFORM_FDT \
		OPTEE_SMC_FAST_CALL_VAL(OPTEE_SMC_FUNCID_SUNXI_INFORM_FDT)

/*PLATFORM_FASTCALL*/
#define OPTEE_SMC_FUNCID_PLATFORM  100
#define OPTEE_SMC_PLATFORM_CMD \
		OPTEE_SMC_FAST_CALL_VAL(OPTEE_SMC_FUNCID_PLATFORM)
#define TEE_SET_CPU_ENTRY 0xFFFF0001
#define TEE_SET_CPU_OFF 0xFFFF0002


#define TEESMC_SSK_ENCRYPT       0
#define TEESMC_KEYBOX_STORE      1
#define TEESMC_PROBE_SHM_BASE    2
#define TEESMC_SSK_DECRYPT       3
#define TEESMC_RSSK_ENCRYPT      4
#define TEESMC_RSSK_DECRYPT      5
#define TEESMC_AES_CBC           6

/**
 * struct arm_smccc_res - Result from SMC/HVC call
 * @a0-a3 result values from registers 0 to 3
 */
struct arm_smccc_res {
	unsigned long a0;
	unsigned long a1;
	unsigned long a2;
	unsigned long a3;
};

/**
 * arm_smccc_smc() - make SMC calls
 * @a0-a7: arguments passed in registers 0 to 7
 * @res: result values from registers 0 to 3
 *
 * This function is used to make SMC calls following SMC Calling Convention.
 * The content of the supplied param are copied to registers 0 to 7 prior
 * to the SMC instruction. The return values are updated with the content
 * from register 0 to 3 on return from the SMC instruction.
 */
asmlinkage void arm_smccc_smc(unsigned long a0, unsigned long a1,
			unsigned long a2, unsigned long a3, unsigned long a4,
			unsigned long a5, unsigned long a6, unsigned long a7,
			struct arm_smccc_res *res);


#endif /* TEESMC25_H */
