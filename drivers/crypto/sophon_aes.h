#ifndef _SOPHON_AES_H_
#define _SOPHON_AES_H_

#define OPTEE_SMC_CALL_CV_SPACC_EXEC 0x0300000A
#include <common.h>
#include <log.h>
#include <linux/errno.h>
#include <linux/arm-smccc.h>
#include <cpu_func.h>
enum algo {
	ALGO_BYPASS = 8,
	ALGO_AES = 9,
	ALGO_DES = 10,
	ALGO_SM4 = 11,
	ALGO_BASE64 = 13
};

enum mode {
	AES_ECB = 0,
	AES_CBC = 1,
	AES_CTR = 2,
	DES_DES = 3,
	DES_TDES = 4
};

enum key_mode {
	AES_128BIT = 4,
	AES_192BIT = 2,
	AES_256BIT = 1
};

enum action {
	DECRYPT = 0,
	ENCRYPTION = 1
};
enum otp {
	USE_DMA_KEY = 0,
	USE_OTP_KEY = 1
};
typedef struct _spacc_exec_config {
	enum algo algo;
	enum mode mode;
	enum key_mode key_mode;
	uintptr_t key;
	uintptr_t iv;
	enum action action;
	enum otp otp;
} spacc_exec_config;

int sophon_spacc_aes(spacc_exec_config *aes_config, const unsigned char *dst, const unsigned char *src, size_t size);

#endif // _SOPHON_AES_H_