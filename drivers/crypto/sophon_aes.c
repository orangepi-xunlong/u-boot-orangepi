
#include <sophon_aes.h>
int sophon_spacc_aes(spacc_exec_config *config,const unsigned char *dst, const unsigned char *src, size_t size)
{
	struct arm_smccc_res res = { 0 };
	uint64_t key_len;
	uint64_t arg7 = ((uint64_t)config->algo << 48) |
			((uint64_t)config->mode << 32) |
			((uint64_t)config->key_mode << 16) |
			((uint64_t)config->otp << 4 | (uint64_t)config->action);
	switch (config->key_mode) {
		case AES_128BIT:
			key_len = 16;
			break;
		case AES_192BIT:
			key_len = 24;
			break;
		case AES_256BIT:
			key_len = 32;
			break;
	}
	printf("key:%p, iv:%p, key_len:%llu\n", (void*)config->key, (void*)config->iv, key_len);
	printf("src:%p, dst:%p, size:%zu\n",  (void*)src,  (void*)dst, size);
	flush_dcache_all();
	arm_smccc_smc(OPTEE_SMC_CALL_CV_SPACC_EXEC, (unsigned long)src, (unsigned long)dst,
		      size, (unsigned long)config->key,
		      (unsigned long)config->iv, key_len, arg7, &res);
	flush_dcache_all();
	return res.a0;
}