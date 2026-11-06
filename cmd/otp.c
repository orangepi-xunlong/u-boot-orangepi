// SPDX-License-Identifier: GPL-2.0+

#include <common.h>
#include <command.h>
#include <stdlib.h>
#include <stdarg.h>
#include <malloc.h>
#include <mmio.h>
#include <linux/arm-smccc.h>
#include <linux/ctype.h>
#include <cpu_func.h>

#include "cvitek/cvi_otp_defines.h"

#define OTP_DEBUG 0

#define _cc_trace(fmt, ...) __trace("", __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define _cc_error(fmt, ...) __trace("ERROR:", __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)

#define ERROR(fmt, ...) __trace("ERROR:", __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)

#if OTP_DEBUG

#define VERBOSE(fmt, ...) __trace("VERBOSE:", __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)

static int __trace(const char *prefix, const char *path, const char *func, int lineno, const char *fmt, ...)
{
	va_list ap;
	int ret;

	printf("[%s%s:%s:%d] ", prefix, path, func, lineno);
	if (!fmt || fmt[0] == '\0') {
		ret = printf("\n");
	} else {
		va_start(ap, fmt);
		ret = vprintf(fmt, ap);
		va_end(ap);
	}

	return ret;
}
#else

#define VERBOSE(fmt, ...)

static int __trace(const char *prefix, const char *path, const char *func, int lineno, const char *fmt, ...)
{
	return 0;
}
#endif

static int hex2bytes(const char *hex, unsigned char *buf, int buf_size)
{
	int i, total = 0;
	char tmp[3];

	memset(buf, 0, buf_size);

	for (i = 0; i < buf_size; i++) {
		if (!hex[0] || !hex[1])
			break;

		tmp[0] = hex[0];
		tmp[1] = hex[1];
		tmp[2] = '\0';

		buf[i] = simple_strtoul(tmp, NULL, 16);
		hex += 2;
		total += 1;
	}

	return total;
}

/*
 * Base address
 */
#define SYSTEM_OTP_BASE			0x27100000
#define SYSTEM_OTP2_BS		  0x0

#define PIF_BS				  0x1100
#define PIF_OTP2_LOCK		   (PIF_BS + 0x10)
#define PIF_OTP2_SECRP_PROT_EN  (PIF_BS + 0x20)
#define PIF_OTP2_CDE_SECRP	  (PIF_BS + 0x24)
#define PIF_OTP2_LVL2LCK		(PIF_BS + 0x5C)

#define CFG_BS				  0x1280
#define CFG_INTERRUPT		   (CFG_BS + 0x10)
#define CFG_BUSY				(CFG_BS + 0x20)
#define CFG_PDSTB			   (CFG_BS + 0x24)
#define CFG_WAKE_UP_TIME		(CFG_BS + 0x28)
#define CFG_CDE_PSMSK_0		 (CFG_BS + 0x60)
#define CFG_CDE_PSMSK_1		 (CFG_BS + 0x64)
#define CFG_SEC_RANGE		   (CFG_BS + 0x78)
#define CFG_LOCK_WRITE		  (CFG_BS + 0x7c)

#define OPTEE_SMC_CALL_CV_OTP3_READ		0x03000006
#define OPTEE_SMC_CALL_CV_OTP3_WRITE	0x03000007

// ===========================================================================
// OTP driver implementation
// ===========================================================================
static inline void cvi_otp_enter_sleep_mode(void)
{
	uint32_t value;

	value = mmio_read_32(SYSTEM_OTP_BASE + CFG_PDSTB);
	if ((value & 0x1) == 1) {
		mmio_write_32(SYSTEM_OTP_BASE + CFG_PDSTB, 0);
		while ((value = mmio_read_32(SYSTEM_OTP_BASE + CFG_BUSY)) & 0xC);
	} else {
		pr_info("otp is already sleep mode\n");
	}
}

static inline void cvi_otp_enter_active_mode(void)
{
	uint32_t value;

	value = mmio_read_32(SYSTEM_OTP_BASE + CFG_PDSTB);
	if ((value & 0x1) == 0) {
		mmio_write_32(SYSTEM_OTP_BASE + CFG_PDSTB, 1);
		while (((value = mmio_read_32(SYSTEM_OTP_BASE + CFG_BUSY)) & 0x1));
	} else {
		pr_info("otp is already active mode\n");
	}
}

static inline void cvi_otp_power_switch(int on)
{
	if (on)
		cvi_otp_enter_active_mode();
	else
		cvi_otp_enter_sleep_mode();
}

static inline uint32_t otp2_segment_read(uint32_t segment, uint32_t addr)
{
	VERBOSE("otp2 read 0x%lx\n", (SYSTEM_OTP_BASE + SYSTEM_OTP2_BS + (((segment << 5) + addr) << 2)));
	return mmio_read_32(SYSTEM_OTP_BASE + SYSTEM_OTP2_BS + (((segment << 5) + addr) << 2));
}

static inline void otp2_segment_addr_program(uint32_t segment, uint32_t addr, uint32_t value)
{
	VERBOSE("otp2 program 0x%x : 0x%x\n", (SYSTEM_OTP_BASE + SYSTEM_OTP2_BS + (((segment << 5) + addr) << 2)), value);
	mmio_write_32(SYSTEM_OTP_BASE + SYSTEM_OTP2_BS + (((segment << 5) + addr) << 2), value);
}

static inline void otp2_segment_lock(uint32_t segment)
{
	uint32_t index = segment/8;
	uint32_t shfit = (segment%8)*4;

	mmio_write_32(SYSTEM_OTP_BASE + PIF_OTP2_LOCK + index*4, (0xF << shfit));
}

static inline uint32_t otp2_segment_isLocked(uint32_t segment)
{
	uint32_t index = segment/8;
	uint32_t shfit = (segment%8)*4;

	return mmio_read_32(SYSTEM_OTP_BASE + PIF_OTP2_LOCK + index*4) & (0xF << shfit);
}

static void otp2_segment_dump(uint32_t segment)
{
	const uint32_t size = 32; // one segment size
	uint32_t buf[size];
	uint32_t i = 0;

	for (; i < size; i++)
		buf[i] = otp2_segment_read(segment, i);

	print_buffer(0, (unsigned char *)buf, 1, (size << 2), 0);
}

static void otp2_segment_program(uint32_t segment, uint32_t *value, uint32_t size)
{
	uint32_t i = 0;

	if (size > 32)
		size = 32;

	for (; i < size; i++)
		otp2_segment_addr_program(segment, i, value[i]);
}

static int otp3_segment_read(uint32_t segment, uint32_t addr, uint32_t *value)
{
	struct arm_smccc_res res = { 0 };

	flush_dcache_all();
	arm_smccc_smc(OPTEE_SMC_CALL_CV_OTP3_READ, segment, addr, 1, (unsigned long)value, 0, 0, 0, &res);
	if (res.a0 < 0) {
		ERROR("smc OPTEE_SMC_CALL_CV_OTP3_READ failed\n");
		return -1;
	}

	invalidate_dcache_all();
	return 0;
}

static int otp3_segment_addr_program(uint32_t segment, uint32_t addr, uint32_t value)
{
	struct arm_smccc_res res = { 0 };

	flush_dcache_all();
	arm_smccc_smc(OPTEE_SMC_CALL_CV_OTP3_WRITE, segment, addr, 1, (unsigned long)&value, 0, 0, 0, &res);
	if (res.a0 < 0) {
		ERROR("smc OPTEE_SMC_CALL_CV_OTP3_WRITE failed\n");
		return -1;
	}

	return 0;
}

static void otp3_segment_dump(uint32_t segment)
{
	struct arm_smccc_res res = { 0 };
	uint32_t value[32] = {0};

	flush_dcache_all();
	arm_smccc_smc(OPTEE_SMC_CALL_CV_OTP3_READ, segment, 0, 32, (unsigned long)value, 0, 0, 0, &res);
	if (res.a0 < 0) {
		ERROR("smc OPTEE_SMC_CALL_CV_OTP3_READ failed\n");
		return;
	}

	invalidate_dcache_all();
	print_buffer(0, (unsigned char *)value, 1, (32 << 2), 0);
}

static int otp3_segment_program(uint32_t segment, void *value, uint32_t size)
{
	struct arm_smccc_res res = { 0 };

	flush_dcache_all();
	arm_smccc_smc(OPTEE_SMC_CALL_CV_OTP3_WRITE, segment, 0, (size >> 2), (unsigned long)value, 0, 0, 0, &res);
	if (res.a0 < 0) {
		ERROR("smc OPTEE_SMC_CALL_CV_OTP3_WRITE failed\n");
		return -1;
	}

	return 0;
}

static void otp3_read_hash_public(void)
{
	struct arm_smccc_res res = { 0 };
	uint32_t value[8] = {0};

	flush_dcache_all();
	arm_smccc_smc(OPTEE_SMC_CALL_CV_OTP3_READ, 3, 10, 8, (unsigned long)value, 0, 0, 0, &res);
	if (res.a0 < 0) {
		ERROR("smc OPTEE_SMC_CALL_CV_OTP3_READ failed\n");
		return;
	}

	invalidate_dcache_all();
	print_buffer(0, (unsigned char *)value, 1, (8 << 2), 0);
}

static void otp3_program_hash_public(void *value, uint32_t size)
{
	struct arm_smccc_res res = { 0 };

	flush_dcache_all();
	arm_smccc_smc(OPTEE_SMC_CALL_CV_OTP3_WRITE, 3, 10, (size >> 2), (unsigned long)value, 0, 0, 0, &res);
	if (res.a0 < 0) {
		ERROR("smc OPTEE_SMC_CALL_CV_OTP3_WRITE failed\n");
		return;
	}
}

static void otp3_read_loader_ek(void)
{
	struct arm_smccc_res res = { 0 };
	uint32_t value[4] = {0};

	flush_dcache_all();
	arm_smccc_smc(OPTEE_SMC_CALL_CV_OTP3_READ, 3, 22, 4, (unsigned long)value, 0, 0, 0, &res);
	if (res.a0 < 0) {
		ERROR("smc OPTEE_SMC_CALL_CV_OTP3_READ failed\n");
		return;
	}

	invalidate_dcache_all();
	print_buffer(0, (unsigned char *)value, 1, (4 << 2), 0);
}

static void otp3_program_loader_ek(void *value, uint32_t size)
{
	struct arm_smccc_res res = { 0 };

	flush_dcache_all();
	arm_smccc_smc(OPTEE_SMC_CALL_CV_OTP3_WRITE, 3, 22, (size >> 2), (unsigned long)value, 0, 0, 0, &res);
	if (res.a0 < 0) {
		ERROR("smc OPTEE_SMC_CALL_CV_OTP3_WRITE failed\n");
		return;
	}
}

static inline void cvi_otp_version(void)
{
	printf("version: %x\n", mmio_read_32(SYSTEM_OTP_BASE + CFG_BS));
}

static int find_otp_area_by_name(const char *name)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(otp_areas); i++) {
		if (!otp_areas[i])
			continue;

		if (!strcmp(name, otp_areas[i]))
			return i;
	}

	return -1;
}

static int CVI_OTP_EnableSecureBoot(CVI_OTP_SECUREBOOT_E sel)
{
	uint32_t value = 0;

	value |= (0x3 << CVI_OTP_TEE_SCS_ENABLE_SHIFT);
	value |= (0x4 << CVI_OTP_ROOT_PUBLIC_KEY_SELECTION_SHIFT);

	if (sel == CVI_OTP_SECUREBOOT_SIGN_ENCRYPT) {
		value |= (0x3 << CVI_OTP_BOOT_LOADER_ENCRYPTION);
		value |= (0x4 << CVI_OTP_LDR_KEY_SELECTION_SHIFT);
	}

	return otp3_segment_addr_program(3, 8, value);
}

static int CVI_OTP_IsSecureBootEnabled(void)
{
	uint32_t value = 0;
	int ret = -1;

	ret = otp3_segment_read(3, 8, &value);
	if (ret < 0)
		return ret;

	if (value & (0x3 << CVI_OTP_TEE_SCS_ENABLE_SHIFT)) {
		if (value & (0x3 << CVI_OTP_BOOT_LOADER_ENCRYPTION))
			return CVI_OTP_SECUREBOOT_SIGN_ENCRYPT;
		else
			return CVI_OTP_SECUREBOOT_SIGN;
	}

	return CVI_OTP_SECUREBOOT_DISABLE;
}

static int do_otp3_read(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	uint32_t segment;

	if (argc < 2)
		return CMD_RET_USAGE;

	segment = simple_strtoul(argv[1], NULL, 0);
	if (segment > 3) {
		printf("segment %d out of range\n", segment);
		return CMD_RET_USAGE;
	} else if (segment > 1) {
		printf("segment %d is reserved\n", segment);
		return CMD_RET_USAGE;
	}

	cvi_otp_power_switch(1);
	if (argc == 3) {
		uint32_t addr, value;
		int ret = -1;

		addr = simple_strtoul(argv[2], NULL, 0);
		if (addr > 31) {
			printf("addr overflow\n");
			goto ret_usage;
		}

		ret = otp3_segment_read(segment, addr, &value);
		if (ret < 0) {
			printf("otp3 read failed\n");
			goto ret_usage;
		}

		printf("Read otp3 segment[0x%x] addr[0x%04x] : 0x%08x\n", segment, addr, value);
	} else if (argc == 2) {
		otp3_segment_dump(segment);
	}
	cvi_otp_power_switch(0);

	return CMD_RET_SUCCESS;

ret_usage:
	cvi_otp_power_switch(0);
	return CMD_RET_USAGE;
}

static int do_otp3_program(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	uint32_t segment;
	int ret = -1;

	if (argc < 2)
		return CMD_RET_USAGE;

	segment = simple_strtoul(argv[1], NULL, 0);
	if (segment > 3) {
		printf("segment %d out of range\n", segment);
		return CMD_RET_USAGE;
	} else if (segment > 1) {
		printf("segment %d is reserved\n", segment);
		return CMD_RET_USAGE;
	}

	cvi_otp_power_switch(1);
	if (argc == 4) {
		uint32_t addr = simple_strtoul(argv[2], NULL, 0);

		if (addr > 31) {
			printf("addr %d overflow\n", addr);
			goto ret_usage;
		}

		uint32_t value = simple_strtoul(argv[3], NULL, 16);

		ret = otp3_segment_addr_program(segment, addr, value);
		if (ret < 0) {
			printf("otp3_segment_addr_program failed\n");
			goto ret_usage;
		}
	} else if (argc == 3) {
		unsigned char buf[128] = { 0 };

		uint32_t size = hex2bytes(argv[2], buf, sizeof(buf));

		if ((size == 0) || (size & 0x3)) {
			printf("Invailed string len\n");
			goto ret_usage;
		}

		print_buffer(0, buf, 1, size, 0);

		ret = otp3_segment_program(segment, buf, size);
		if (ret < 0) {
			printf("otp3_segment_addr_program failed\n");
			goto ret_usage;
		}
	}
	cvi_otp_power_switch(0);

	return CMD_RET_SUCCESS;

ret_usage:
	cvi_otp_power_switch(0);
	return CMD_RET_USAGE;
}

static int do_otp2_read(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	uint32_t segment, addr;
	uint32_t value;

	if (argc < 2)
		return CMD_RET_USAGE;

	segment = simple_strtoul(argv[1], NULL, 0);
	if (segment == 0) {
		printf("segment 0 is reserved\n");
		return CMD_RET_USAGE;
	} else if (segment > 27) {
		printf("segment %d out of range\n", segment);
		return CMD_RET_USAGE;
	}

	cvi_otp_power_switch(1);
	if (argc == 3) {
		addr = simple_strtoul(argv[2], NULL, 0);
		if (addr > 31) {
			printf("addr %d overflow\n", addr);
			goto ret_usage;
		}

		value = otp2_segment_read(segment, addr);
		printf("Read otp2 segment[0x%x] addr[0x%04x] : 0x%08x\n", segment, addr, value);
	} else if (argc == 2) {
		otp2_segment_dump(segment);
	}
	cvi_otp_power_switch(0);

	return CMD_RET_SUCCESS;
ret_usage:
	cvi_otp_power_switch(0);
	return CMD_RET_USAGE;
}

static int do_otp2_program(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	uint32_t segment;

	if (argc < 2)
		return CMD_RET_USAGE;

	segment = simple_strtoul(argv[1], NULL, 0);
	if (segment == 0) {
		printf("segment 0 is reserved\n");
		return CMD_RET_USAGE;
	} else if (segment > 27) {
		printf("segment %d out of range\n", segment);
		return CMD_RET_USAGE;
	}

	cvi_otp_power_switch(1);
	if (argc == 4) {
		uint32_t addr = simple_strtoul(argv[2], NULL, 0);

		if (addr > 31) {
			printf("addr %d overflow\n", addr);
			goto ret_usage;
		}

		uint32_t value = simple_strtoul(argv[3], NULL, 16);

		otp2_segment_addr_program(segment, addr, value);
	} else if (argc == 3) {
		unsigned char buf[128] = { 0 };

		uint32_t size = hex2bytes(argv[2], buf, sizeof(buf));

		if ((size == 0) || (size & 0x3)) {
			printf("Invailed hex string len\n");
			goto ret_usage;
		}

		print_buffer(0, buf, 1, size, 0);

		otp2_segment_program(segment, (uint32_t *)buf, (size >> 2));
	}
	cvi_otp_power_switch(0);

	return CMD_RET_SUCCESS;

ret_usage:
	cvi_otp_power_switch(0);
	return CMD_RET_USAGE;
}

static int do_otp_version(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	cvi_otp_power_switch(1);
	cvi_otp_version();
	cvi_otp_power_switch(0);

	return CMD_RET_SUCCESS;
}

static int do_otp2_lock(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	uint32_t segment;

	if (argc < 2)
		return CMD_RET_USAGE;

	segment = simple_strtoul(argv[1], NULL, 0);
	if (segment == 0) {
		printf("segment 0 is reserved\n");
		return CMD_RET_FAILURE;
	} else if (segment > 27) {
		printf("segment %d out of range\n", segment);
		return CMD_RET_USAGE;
	}

	cvi_otp_power_switch(1);

	otp2_segment_lock(segment);

	cvi_otp_power_switch(0);

	return CMD_RET_SUCCESS;
}

static int do_otp2_isLocked(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	uint32_t segment;
	uint32_t ret;

	if (argc < 2)
		return CMD_RET_USAGE;

	segment = simple_strtoul(argv[1], NULL, 0);
	if (segment == 0) {
		printf("segment 0 is reserved\n");
		return CMD_RET_FAILURE;
	} else if (segment > 27) {
		printf("segment %d out of range\n", segment);
		return CMD_RET_USAGE;
	}

	cvi_otp_power_switch(1);

	ret = otp2_segment_isLocked(segment);
	if (ret)
		printf("segment %d is locked\n", segment);
	else
		printf("segment %d unLock\n", segment);

	cvi_otp_power_switch(0);

	return CMD_RET_SUCCESS;
}

static int do_config_read(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	int area_id;

	if (argc < 2)
		return CMD_RET_USAGE;

	area_id = find_otp_area_by_name(argv[1]);
	if (area_id == -1) {
		printf("can't find otp area\n");
		return CMD_RET_FAILURE;
	}

	cvi_otp_power_switch(1);
	switch (area_id) {
	case CVI_OTP_AREA_HASH0_PUBLIC:
		otp3_read_hash_public();
		break;
	case CVI_OTP_AREA_LOADER_EK:
		otp3_read_loader_ek();
		break;
	case CVI_OTP_CONFIG_SECUREBOOT: {
		int ret = CVI_OTP_IsSecureBootEnabled();

		if (ret > 0)
			printf("secure boot is enable\n");
		else
			printf("secure boot is disable\n");
		break;
	}
	case CVI_OTP_CONFIG_RESERVE_SEGMENT: {
		uint32_t segment;

		if (argc < 3)
			goto ret_usage;

		segment = simple_strtoul(argv[2], NULL, 0);

		switch (segment) {
		case 0: {
			uint32_t addr, value;

			if (argc == 4) {
				addr = simple_strtoul(argv[3], NULL, 0);
				if (addr > 31) {
					printf("addr %d overflow\n", addr);
					goto ret_usage;
				}

				value = otp2_segment_read(segment, addr);
				printf("Read otp2 segment[0x%x] addr[0x%04x] : 0x%08x\n", segment, addr, value);
			} else if (argc == 3) {
				otp2_segment_dump(segment);
			}
			break;
		}
		case 2:
		case 3: {
			if (argc == 4) {
				uint32_t addr, value;

				addr = simple_strtoul(argv[3], NULL, 0);
				if (addr > 31) {
					printf("addr overflow\n");
					goto ret_usage;
				}

				otp3_segment_read(segment, addr, &value);
				printf("Read otp3 segment[0x%x] addr[0x%04x] : 0x%08x\n", segment, addr, value);
			} else if (argc == 3) {
				otp3_segment_dump(segment);
			}
			break;
		}
		default:
			printf("segment %d overflow\n", segment);
			goto ret_usage;
		};
		break;
	}
	default:
		break;
	};
	cvi_otp_power_switch(0);

	return CMD_RET_SUCCESS;
ret_usage:
	cvi_otp_power_switch(0);
	return CMD_RET_USAGE;
}

static int do_config_program(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	uint32_t area_id;

	if (argc < 3)
		return CMD_RET_USAGE;

	area_id = find_otp_area_by_name(argv[1]);
	if (area_id == -1) {
		printf("can't find otp area\n");
		return CMD_RET_USAGE;
	}

	cvi_otp_power_switch(1);
	switch (area_id) {
	case CVI_OTP_AREA_HASH0_PUBLIC: {
		unsigned char buf[32] = { 0 };
		uint32_t size;

		size = hex2bytes(argv[2], buf, sizeof(buf));
		if ((size <= 0) || (size & 0x3)) {
			printf("Invailed hex string len\n");
			goto ret_usage;
		}

		print_buffer(0, buf, 1, size, 0);
		otp3_program_hash_public(buf, size);
		break;
	}
	case CVI_OTP_AREA_LOADER_EK: {
		unsigned char buf[16] = { 0 };
		uint32_t size;

		size = hex2bytes(argv[2], buf, sizeof(buf));
		if ((size <= 0) || (size & 0x3)) {
			printf("Invailed hex string len\n");
			goto ret_usage;
		}

		print_buffer(0, buf, 1, size, 0);
		otp3_program_loader_ek(buf, size);
		break;
	}
	case CVI_OTP_CONFIG_SECUREBOOT: {
		uint32_t sel = simple_strtoul(argv[2], NULL, 0);

		if (sel > CVI_OTP_SECUREBOOT_SIGN_ENCRYPT) {
			printf("sel %d out of range\n", sel);
			goto ret_usage;
		}

		CVI_OTP_EnableSecureBoot(sel);
		break;
	}
	case CVI_OTP_CONFIG_RESERVE_SEGMENT: {
		uint32_t segment, addr, value;

		if (argc < 5)
			goto ret_usage;

		segment = simple_strtoul(argv[2], NULL, 0);
		addr = simple_strtoul(argv[3], NULL, 0);
		value = simple_strtoul(argv[4], NULL, 16);
		if (addr > 31) {
			printf("addr %d overflow\n", addr);
			goto ret_usage;
		}

		switch (segment) {
		case 0: {
			otp2_segment_addr_program(segment, addr, value);
			break;
		}
		case 2:
		case 3: {
			otp3_segment_addr_program(segment, addr, value);
			break;
		}
		default:
			printf("segment %d overflow\n", segment);
			goto ret_usage;
		};
		break;
	}
	default:
		break;
	};
	cvi_otp_power_switch(0);

	return CMD_RET_SUCCESS;

ret_usage:
	cvi_otp_power_switch(0);
	return CMD_RET_USAGE;
}

static char otp_help_text[] =
	"version - otp version\n"
	"otp otp2_r <segment> [line] - read otp2 segment[1~27] line[0~31], line is option\n"
	"otp otp2_w <segment> [line] <Hex value/Hex string> - program otp2 segment[1~27] line[0~31], line is option\n"
	"otp otp2_lock <segment> - lock otp2 segment[1~27], WARM: it can't unlock\n"
	"otp otp2_isLocked <segment> - is otp2 segment[1~27] Locked\n"
	"otp otp3_r <segment> [line] - read otp3 segment[0~1] line[0~31], line is option\n"
	"otp otp3_w <segment> [line] <Hex value/Hex string> - program otp3 segment[0~1] line[0~31], line is option\n"
	"otp config_r <area string> ... - read config\n"
	"otp config_w <area string> ... - program config\n";

U_BOOT_CMD_WITH_SUBCMDS(otp, "OTP sub-system", otp_help_text,
			U_BOOT_SUBCMD_MKENT(version, 1, 1, do_otp_version),
			U_BOOT_SUBCMD_MKENT(otp2_r, 3, 1, do_otp2_read),
			U_BOOT_SUBCMD_MKENT(otp2_w, 4, 1, do_otp2_program),
			U_BOOT_SUBCMD_MKENT(otp2_lock, 3, 1, do_otp2_lock),
			U_BOOT_SUBCMD_MKENT(otp2_isLocked, 3, 1, do_otp2_isLocked),
			U_BOOT_SUBCMD_MKENT(otp3_r, 3, 1, do_otp3_read),
			U_BOOT_SUBCMD_MKENT(otp3_w, 4, 1, do_otp3_program),
			U_BOOT_SUBCMD_MKENT(config_r, 4, 1, do_config_read),
			U_BOOT_SUBCMD_MKENT(config_w, 5, 1, do_config_program));
