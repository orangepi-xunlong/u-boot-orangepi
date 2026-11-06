#ifndef __ROM_SPI_H__
#define __ROM_SPI_H__

enum rsa_size {
	RSA_2048_BITS,
	RSA_4096_BITS,
};

struct rom_api {
	int (*flash_init)(void);
	int (*get_number_of_retries)(void);
	int (*verify_rsa)(void *message, size_t n, void *sig, enum rsa_size rsa_size);
	int (*load_image)(void *buf, uint32_t offset, size_t image_size, int retry_num);
	int (*cryptodma_aes_decrypt)(void *plain, const void *encrypted, uint64_t len, uint8_t *key, uint8_t *iv);

	enum boot_src (*get_boot_src)(void);
	void (*set_boot_src)(enum boot_src src);
	uint32_t (*image_crc)(const void *buf, int len);
};

#endif /* __ROM_SPI_H__ */
