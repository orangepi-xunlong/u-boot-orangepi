/*
 * Copyright (c) 2015-2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <string.h>
#include <platform.h>
#include <cv_private.h>
#include <tbbr_oid.h>
#include <mmio.h>
#include <firmware_image_package.h>
#include <io_fip.h>
#include "cv_efuse.h"

#if defined(IMG_ENC)
#include "mbedtls/aes.h"
#endif

/* SHA256 algorithm */
#define SHA256_BYTES			32

//DER header of SHA256 digest.
//See RFC3447 Page 43.
static const unsigned char rotpk_hash_hdr[] =
		"\x30\x31\x30\x0D\x06\x09\x60\x86\x48"
		"\x01\x65\x03\x04\x02\x01\x05\x00\x04\x20";
static const unsigned int rotpk_hash_hdr_len = sizeof(rotpk_hash_hdr) - 1;
static unsigned char rotpk_hash_der[sizeof(rotpk_hash_hdr) - 1 + SHA256_BYTES];

// Key are used by BL1
extern unsigned char cv_rotpk_hash[];
extern unsigned char cv_rotpk2_hash[];
extern unsigned char cv_rotpk3_hash[];
extern unsigned char cv_ldrkey[];
extern unsigned char cv_ldrkey2[];
extern unsigned char cv_ldrkey3[];

// Key are used by BL2
extern unsigned char cv_tkcpk_hash[];
extern unsigned char cv_tkc_ek[];

#if defined(IMG_ENC)
static void get_ldr_dec_key_from_bootrom(void *ldr_key);
static int plat_is_boot_loader_encrypted(void);
#endif

#if defined(IMAGE_BL2)
#else
static void get_rotpk_dgst_from_bootrom(void *dst);
#endif


void plat_scs_info(void)
{
	if (plat_is_trusted_boot() == 1) {
		#if defined(IMG_ENC)
		if (plat_is_boot_loader_encrypted() == 1)
			NOTICE("Secure boot 1\n");
		else
			NOTICE("Secure boot 0\n");
		#else
			NOTICE("Secure boot 0\n");
		#endif
	} else {
		NOTICE("Non-secure boot\n");
	}
}

/*
 * @retval 0 none trusted boot
 * @retval others trusted boot
 */
int plat_is_trusted_boot(void)
{
#if defined(BL1_INIT_EFUSE)
	int value; //default working at none security mode

	value = (mmio_read_32(EFUSE_SCS_CONFIG_REG) &
					(0x3 << BIT_TEE_SCS_ENABLE)) >> BIT_TEE_SCS_ENABLE;
	if (value == 0)
		return 0;
	else
		return 1;
#else
	return 1;
#endif
}

/*
 * @retval 0 none trusted boot
 * @retval others trusted boot
 */
int plat_ree_is_trusted_boot(void)
{
	int value; //default working at none security mode

	value = (get_fip_header()->flags >> FIP_REE_SCS_SHIFT) & FIP_REE_SCS_MASK;
	if (value)
		return 1;

	value = (mmio_read_32(EFUSE_SCS_CONFIG_REG) &
		 (0x3 << BIT_SCS_ENABLE)) >>
		BIT_SCS_ENABLE;
	if (value == 0)
		return 0;
	else
		return 1;
}
/*
 * @retval 0 means no encryption
 */
#if defined(IMG_ENC)
static int plat_is_boot_loader_encrypted(void)
{
#if defined(BL1_INIT_EFUSE)
	int value;

	value = (mmio_read_32(EFUSE_SCS_CONFIG_REG) &
							(0x3 << BIT_BOOT_LOADER_ENCRYPTION)) >>
							BIT_BOOT_LOADER_ENCRYPTION;
	if (value == 0)
		return 0;
	else
		return 1;
#else
	return 1;
#endif
}
#endif

static void get_rotpk_dgst_from_efuse(void *dst)
{
#if defined(BL1_INIT_EFUSE)
	unsigned int n, i;
	uint32_t tmp;

	NOTICE("Check booting info\n");
	n = SHA256_BYTES / sizeof(uint32_t);
	for (i = 0; i < n; i++) {
		tmp = mmio_read_32(EFUSE_KPUB_HASH_REG + i*4);
		memcpy((char *)dst + i * sizeof(uint32_t), &tmp, sizeof(tmp));
	}
#else
	get_rotpk_dgst_from_bootrom(dst);
#endif
}

#if defined(IMAGE_BL2)
void get_tkc_ek_dec_key(void *key)
{
	INFO("Get CV_TKC_EK from BL2\n");
	memcpy(key, cv_tkc_ek, 16);
}
#else /* IMAGE_BL2 */
static void get_rotpk_dgst_from_bootrom(void *dst)
{
	NOTICE("Check booting info 1\n");
	memcpy(dst, cv_rotpk_hash, SHA256_BYTES);
}

static void get_rotpk2_dgst_from_bootrom(void *dst)
{
	NOTICE("Check booting info 2\n");
	memcpy(dst, cv_rotpk2_hash, SHA256_BYTES);
}

static void get_rotpk3_dgst_from_bootrom(void *dst)
{
	NOTICE("Check booting info 3\n");
	memcpy(dst, cv_rotpk3_hash, SHA256_BYTES);
}
#endif /* IMAGE_BL2 */

#if defined(IMG_ENC)
static void get_ldr_dec_key_from_efuse(void *ldr_key)
{
#if defined(BL1_INIT_EFUSE)
	unsigned int i;
	uint32_t tmp;

	NOTICE("Check boot loader info\n");
	for (i = 0; i < 4; i++) {
		tmp = mmio_read_32(EFUSE_LDR_DES_KEY_REG + i*4);
		memcpy((char *)ldr_key + i * sizeof(uint32_t), &tmp, sizeof(tmp));
	}

	//for (i=0; i<16; i++)
	//	NOTICE("ldr_key[%d] %x\n", i, ((unsigned char *)ldr_key)[i]);
#else
	get_ldr_dec_key_from_bootrom(ldr_key);
#endif
}
#endif

#if defined(IMG_ENC)
static void get_ldr_dec_key_from_bootrom(void *ldr_key)
{
	NOTICE("Check boot loader info 1\n");
	memcpy(ldr_key, cv_ldrkey, 16);
}

static void get_ldr_dec_key2_from_bootrom(void *ldr_key)
{
	NOTICE("Check boot loader info 2\n");
	memcpy(ldr_key, cv_ldrkey2, 16);
}

static void get_ldr_dec_key3_from_bootrom(void *ldr_key)
{
	NOTICE("Check boot loader info 3\n");
	memcpy(ldr_key, cv_ldrkey3, 16);
}

static void get_ldr_dec_key(void *ldr_key)
{
	int ldr_dec_key_selection = 0; // Default read from rom code

#if defined(BL1_INIT_EFUSE)
	ldr_dec_key_selection = (mmio_read_32(EFUSE_SCS_CONFIG_REG) &
							(0x7 << BIT_LDR_KEY_SELECTION)) >> BIT_LDR_KEY_SELECTION;
#else
	ldr_dec_key_selection = 0; // Forced read from rom code
#endif

	if (ldr_dec_key_selection == 0)
		get_ldr_dec_key_from_bootrom(ldr_key);
	else if (ldr_dec_key_selection == 1)
		get_ldr_dec_key2_from_bootrom(ldr_key);
	else if (ldr_dec_key_selection == 2)
		get_ldr_dec_key3_from_bootrom(ldr_key);
	else
		get_ldr_dec_key_from_efuse(ldr_key);
}

#if defined(IMAGE_BL2)
static void get_bl33_dec_key(void *key)
{
	extern unsigned char cv_non_trusted_ek_buf[];
	extern unsigned char bl33_ek_buf[];

	mbedtls_aes_context aes;

	unsigned char tkc_ek[16] __aligned(16) = { 0 };
	unsigned char ikey1[16] __aligned(16) = { 0 };

	get_ldr_dec_key_from_efuse(tkc_ek);

	mbedtls_aes_init(&aes);
	mbedtls_aes_setkey_dec(&aes, tkc_ek, 128);
	mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, cv_non_trusted_ek_buf, ikey1);

	mbedtls_aes_setkey_dec(&aes, ikey1, 128);
	mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, bl33_ek_buf, key);
}
#endif

int plat_blx_is_encrypted(unsigned int image_id)
{
	switch (image_id) {
	case BL2_IMAGE_ID:
	case BLD_IMAGE_ID:
		// Check eFuse
		return plat_is_boot_loader_encrypted();
	case BL31_IMAGE_ID:
	case BL32_IMAGE_ID:
		// TEE is always encrypted
		return 1;
	case BL33_IMAGE_ID:
		return ((get_fip_header()->flags >> FIP_REE_ENCRYPT_SHIFT) &
			FIP_REE_ENCRYPT_MASK) != 0;
	default:
		return 0;
	}
}

void get_blx_dec_key(unsigned int image_id, void *key)
{
	switch (image_id) {
	case BL2_IMAGE_ID:
	case BLD_IMAGE_ID:
		get_ldr_dec_key(key);
		break;
#if defined(IMAGE_BL2)
	case BL31_IMAGE_ID:
	case BL32_IMAGE_ID:
		get_tkc_ek_dec_key(key);
		break;
	case BL33_IMAGE_ID:
		get_bl33_dec_key(key);
		break;
#endif
	default:
		memset(key, 0, 16);
		break;
	}
}
#endif /* IMG_ENC */

static int current_rotpk_src;

void plat_set_rotpk_src(int src)
{
	current_rotpk_src = src;
}

static void get_rotpk_dgst(void *dst)
{
#if defined(IMAGE_BL2)
	if (current_rotpk_src) {
		INFO("GPKE\n");
		get_rotpk_dgst_from_efuse(dst);
	} else {
		/* Get CV_TKC_PK hash from BL2 */
		INFO("GPKB\n");
		memcpy(dst, cv_tkcpk_hash, SHA256_BYTES);
	}
#else /* IMAGE_BL2 */
	int root_public_key_selection = 0; // Default read from rom code

#if defined(BL1_INIT_EFUSE)
	root_public_key_selection = (mmio_read_32(EFUSE_SCS_CONFIG_REG) &
								(0x7 << BIT_ROOT_PUBLIC_KEY_SELECTION)) >>
								BIT_ROOT_PUBLIC_KEY_SELECTION;
#else
	root_public_key_selection = 0; // Forced read from rom code
#endif

	/* Get ROTPK hash from ROM or eFuse */
	NOTICE("Get ROTPK hash from ROM or eFuse\n");
	if (root_public_key_selection == 0)
		get_rotpk_dgst_from_bootrom(dst);
	else if (root_public_key_selection == 1)
		get_rotpk2_dgst_from_bootrom(dst);
	else if (root_public_key_selection == 2)
		get_rotpk3_dgst_from_bootrom(dst);
	else
		get_rotpk_dgst_from_efuse(dst);
#endif /* IMAGE_BL2 */
}

/*
 * Return the ROTPK hash in the following ASN.1 structure in DER format:
 *
 * AlgorithmIdentifier  ::=  SEQUENCE  {
 *     algorithm         OBJECT IDENTIFIER,
 *     parameters        ANY DEFINED BY algorithm OPTIONAL
 * }
 *
 * DigestInfo ::= SEQUENCE {
 *     digestAlgorithm   AlgorithmIdentifier,
 *     digest            OCTET STRING
 * }
 */
int plat_get_rotpk_info(void *cookie, void **key_ptr, unsigned int *key_len,
			unsigned int *flags)
{
	void *dst;

	assert(key_ptr != NULL);
	assert(key_len != NULL);
	assert(flags != NULL);

	/* TODO: ROTPK_NOT_DEPLOYED */

	/* Copy the DER header */
	memcpy(rotpk_hash_der, rotpk_hash_hdr, rotpk_hash_hdr_len);
	dst = (void *)&rotpk_hash_der[rotpk_hash_hdr_len];

	get_rotpk_dgst(dst);

	*key_ptr = (void *)rotpk_hash_der;
	*key_len = (unsigned int)sizeof(rotpk_hash_der);
	*flags = ROTPK_IS_HASH;
	return 0;
}

int plat_get_market_segment(unsigned int *market_segment)
{
	uint32_t efuse_market_segment = 0;

#if defined(BL1_INIT_EFUSE)
	efuse_market_segment = mmio_read_32(EFUSE_MARKET_SEGMENT);
#endif
	INFO("efuse_market_segment %x\n", efuse_market_segment);

	*market_segment = efuse_market_segment;

	return 0;
}

/*
 * Return the non-volatile counter value stored in the platform. The cookie
 * will contain the OID of the counter in the certificate.
 *
 * Return: 0 = success, Otherwise = error
 */
int plat_get_nv_ctr(void *cookie, unsigned int *nv_ctr)
{
	/*
	 * The behaviour of this function is return a version number to
	 * authenticate a certificate if this certificate is suitable.
	 * The value it returned is fixed for a platform. It is not a dynamic
	 * value. I mean dynamic value is a system clock driven by crystal.
	 * eg. You can setup a limitation of lowest software version in some
	 * place on your chip and it canot be modified after chip tap out. Then
	 * arm-trusted-firmware(ATF) trusted board boot architecture will
	 * checkout this value in certificate then compared with this one,
	 * if this one is larger which means you are trying to run a legacy
	 * software on a modern platform(chip). Though you can pass the
	 * signature authtication (maybe the two serious of chip uses the same
	 * RSA private key), ATF will not let you run on it. ATF consider this
	 * as a "roll-back" attack.
	 *
	 * But for now no one impliments this authentication schema except one
	 * integrates ARM CryptoCell.
	 *
	 * ARM FVP(Fixed Virtual Platform) impliments this feature as fixed
	 * value.
	 * - For secure nv-counter, return 31
	 * - For none-secure, return 223
	 *
	 * Other platforms like qemu, hikey and raspberry pi, return 0.
	 * In other words, they donot use this feature.
	 * I will return 0 as well, if someone want to anti roll-back attack,
	 * changing RSA private key is an option.
	 */

	uint32_t efuse_nv_counter = 0;

#if defined(BL1_INIT_EFUSE)
	efuse_nv_counter = mmio_read_32(EFUSE_NV_COUNTER);
#endif
	INFO("efuse_nv_counter %x\n", efuse_nv_counter);

	*nv_ctr = efuse_nv_counter;

	return 0;
}

/*
 * Store a new non-volatile counter value. By default on ARM development
 * platforms, the non-volatile counters are RO and cannot be modified. We expect
 * the values in the certificates to always match the RO values so that this
 * function is never called.
 *
 * Return: 0 = success, Otherwise = error
 */
int plat_set_nv_ctr(void *cookie, unsigned int nv_ctr)
{
	return 0;
}

uint32_t get_mfr_secret_id(void)
{
	INFO("Get MFR_SECRET_ID from eFuse\n");
	return mmio_read_32(EFUSE_MFR_SECRET_ID_REG);
}
