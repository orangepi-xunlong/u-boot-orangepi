/* cbc_mode.c - TinyCrypt implementation of CBC mode encryption & decryption */

/*
 *  Copyright (C) 2017 by Intel Corporation, All Rights Reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *    - Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *    - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 *    - Neither the name of Intel Corporation nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#include <crypto/ecb_mode.h>
#include <crypto/constants.h>
#include <crypto/utils.h>
#ifdef CONFIG_TZ_ROM
#include "rom/compiler.h"
#endif

__xip_text
int tc_ecb_mode_encrypt(uint8_t *out, unsigned int outlen, const uint8_t *in,
			    unsigned int inlen, const TCAesKeySched_t sched)
{
	unsigned int n;

	/* input sanity check: */
	if (out == (uint8_t *) 0 ||
	    in == (const uint8_t *) 0 ||
	    sched == (TCAesKeySched_t) 0 ||
	    inlen == 0 ||
	    outlen == 0 ||
	    (inlen % TC_AES_BLOCK_SIZE) != 0 ||
	    (outlen % TC_AES_BLOCK_SIZE) != 0 ||
	    outlen != inlen /*+ TC_AES_BLOCK_SIZE */) {
		return TC_CRYPTO_FAIL;
	}

	for (n = 0; n < inlen; ++n) {
		if ((n % TC_AES_BLOCK_SIZE) == 0) {
			tc_aes_encrypt(out, in, sched);
			in += TC_AES_BLOCK_SIZE;
			out += TC_AES_BLOCK_SIZE;
		}
	}

	return TC_CRYPTO_SUCCESS;
}

__xip_text
int tc_ecb_mode_decrypt(uint8_t *out, unsigned int outlen, const uint8_t *in,
			    unsigned int inlen, const TCAesKeySched_t sched)
{
	unsigned int n;

	/* sanity check the inputs */
	if (out == (uint8_t *) 0 ||
	    in == (const uint8_t *) 0 ||
	    sched == (TCAesKeySched_t) 0 ||
	    inlen == 0 ||
	    outlen == 0 ||
	    (inlen % TC_AES_BLOCK_SIZE) != 0 ||
	    (outlen % TC_AES_BLOCK_SIZE) != 0 ||
	    outlen != inlen) {
		return TC_CRYPTO_FAIL;
	}

	for (n = 0; n < inlen; ++n) {
		if ((n % TC_AES_BLOCK_SIZE) == 0) {
			tc_aes_decrypt(out, in, sched);
			in += TC_AES_BLOCK_SIZE;
			out += TC_AES_BLOCK_SIZE;
		}
	}

	return TC_CRYPTO_SUCCESS;
}
