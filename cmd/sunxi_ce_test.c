/*
 * (C) Copyright 2018 allwinnertech  <wangwei@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <securestorage.h>
#include <sunxi_board.h>
#include <asm/arch/ce.h>
#include <sunxi_board.h>
#include <asm/arch/timer.h>

int do_hash_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	u8 hash[32] = { 0 };
	u8 __aligned(64) src[3] = {0x61, 0x62, 0x63};
	u8 sha256[32] = {0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea,
					 0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
					 0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c,
					 0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad};
	u8 sm3[32] =	{0x66, 0xc7, 0xf0, 0xf4, 0x62, 0xee, 0xed, 0xd9,
					 0xd1, 0xf2, 0xd4, 0x6b, 0xdc, 0x10, 0xe4, 0xe2,
					 0x41, 0x67, 0xc4, 0x87, 0x5c, 0xf2, 0xf7, 0xa2,
					 0x29, 0x7d, 0xa0, 0x2b, 0x8f, 0x4b, 0xa8, 0xe0};
	u32 sha_flag = 0;
	u32 src_len = 3;
	u32 md_size = 32;
	int ret;

	if (strcmp(argv[1], "sm3") == 0) {
		pr_err("sm3\n");
		sha_flag = SUNXI_SM3;
	} else {
		pr_err("sha256\n");
		sha_flag = SUNXI_SHA256;
	}

	tick_printf("test start 0\n");
	sunxi_ss_open();

	if (sha_flag == SUNXI_SHA256) {
		sunxi_sha_calc(hash, md_size, (u8 *)(IOMEM_ADDR(src)), src_len);
		ret = memcmp(hash, sha256, md_size);
	} else {
		sunxi_hash_test(hash, md_size, (u8 *)(IOMEM_ADDR(src)), src_len, sha_flag);
		ret = memcmp(hash, sm3, md_size);
	}

	if (ret == 0) {
		tick_printf("test sucess\n");
	} else {
		tick_printf("test fail\n");
	}

	tick_printf("src data\n");
	if (src_len > 512) {
		src_len = 512;
	}
	sunxi_dump((u8 *)(IOMEM_ADDR(src)), src_len);
	tick_printf("hash\n");
	sunxi_dump(hash, 32);

	return CMD_RET_SUCCESS;
}

static s32 sm2_kdf(u8 *src, u32 src_len, u8 *dst, u32 dst_len)
{
	u32 ct = 0x1;
	u32 hash_len = 32;
	u8 hash[32];
	u8 *tmp = NULL;
	u32 counter = 0;
	u32 i = 0;
	int ret;

	ALLOC_CACHE_ALIGN_BUFFER(u8, data, ((SM2_SIZE_BYTE * 2) + 4));

	if ((src == NULL) || (dst == NULL)) {
		pr_err("input buf is NULL\n");
		ret = -1;
		goto err;
	}

	if ((src_len == 0x0) || (dst_len == 0x0)) {
		pr_err("input buf len is 0\n");
		ret = -2;
		goto err;
	}

	tmp = malloc(dst_len);
	if (tmp == NULL) {
		pr_err("malloc fail\n");
		ret = -3;
		goto err;
	}

	memset(tmp, 0x0, dst_len);
	memset(hash, 0x0, sizeof(hash));
	counter = (dst_len + hash_len -1) / hash_len;

	for (i = 0; i < counter; i++) {
		src[src_len + 3] = ct & 0xff;
		src[src_len + 2] = (ct >> 8) & 0xff;
		src[src_len + 1] = (ct >> 16) & 0xff;
		src[src_len + 0] = (ct >> 24) & 0xff;

		ret = sunxi_hash_test(hash, 32, data, (src_len + 4), SUNXI_SM3);
		if (ret < 0) {
			pr_err("%s\n", __func__);
			goto err;
		}
		if (i == counter -1) {
			if (dst_len % 32 != 0) {
				hash_len = dst_len % 32;
			}
		}
		memcpy(tmp + (32 * i), hash, hash_len);

		ct++;
	}

	memcpy(dst, tmp, dst_len);
	ret = 0;

err:
	if (!tmp)
		free(tmp);
	return ret;
}


static s32 sunxi_sm2_crypto_test(struct sunxi_sm2_ctx_t *sm2_ctx)
{
	u32 demod_cx[8] = {0x04EBFC71, 0x8E8D1798, 0x62043226, 0x8E77FEB6,
						0x415E2EDE, 0x0E073C0F, 0x4F640ECD, 0x2E149A73};
	u32 demod_cy[8] = {0xE858F9D8, 0x1E5430A5, 0x7B36DAAB, 0x8F950A3C,
						0x64E6EE6A, 0x63094D99, 0x283AFF76, 0x7E124DF0};
	u32 demod_kx[8] = {0x335E18D7, 0x51E51F04, 0x0E27D468, 0x138B7AB1,
						0xDC86AD7F, 0x981D7D41, 0x6222FD6A, 0xB3ED230D};
	u32 demod_ky[8] = {0xAB743EBC, 0xFB22D64F, 0x7B6AB791, 0xF70658F2,
						0x5B48FA93, 0xE54064FD, 0xBFBED3F0, 0xBD847AC9};
	int ret1, ret2;
	u32 m_len = 19;
	u32 sm2_size_byte = sm2_ctx->sm2_size >> 3;

	ALLOC_CACHE_ALIGN_BUFFER(u8, src, ((SM2_SIZE_BYTE * 2) + 4));
	ALLOC_CACHE_ALIGN_BUFFER(u8, t, ((SM2_SIZE_BYTE * 2) + 4));

	//-1-gen c1(cx || cy || kx || ky)
	ret1 = sm2_crypto_gen_cxy_kxy(sm2_ctx);
	if (ret1) {
		pr_err("sm2_crypto_gen_cxy_kxy fail\n");
		return -1;
	}

	ret1 = memcmp(demod_cx, sm2_ctx->cx, sm2_size_byte);
	ret2 = memcmp(demod_cy, sm2_ctx->cy, sm2_size_byte);
	if ((ret1 || ret2)) {
		pr_err("compare is fail\n");
		pr_err("cx(ce)\n");
		sunxi_dump(sm2_ctx->cx, sm2_size_byte);
		pr_err("cx(demod)\n");
		sunxi_dump(demod_cx, sm2_size_byte);
		pr_err("cy(ce)\n");
		sunxi_dump(sm2_ctx->cy, sm2_size_byte);
		pr_err("cy(demod)\n");
		sunxi_dump(demod_cy, sm2_size_byte);
		return -1;
	} else {
		ret1 = memcmp(demod_kx, sm2_ctx->kx, sm2_size_byte);
		ret2 = memcmp(demod_ky, sm2_ctx->ky, sm2_size_byte);
		if ((ret1 || ret2)) {
			pr_err("compare is fail\n");
			pr_err("kx(ce)\n");
			sunxi_dump(sm2_ctx->kx, sm2_size_byte);
			pr_err("kx(demod)\n");
			sunxi_dump(demod_kx, sm2_size_byte);
			pr_err("ky(ce)\n");
			sunxi_dump(sm2_ctx->ky, sm2_size_byte);
			pr_err("ky(demod)\n");
			sunxi_dump(demod_ky, sm2_size_byte);
			return -1;
		} else {
			pr_err("gen cx/cy/kx/ky is sucess\n");
		}
	}

	//-2-gen c2(M||t)
	memset(t, 0x00, m_len);
	//kx||ky||ct
	memset(src, 0x00, ((sm2_size_byte * 2) + 4));
	memcpy(src, sm2_ctx->kx, sm2_size_byte);
	memcpy((src + sm2_size_byte), sm2_ctx->ky, sm2_size_byte);

	ret1 = sm2_kdf(src, (sm2_size_byte * 2), t, m_len);
	if (ret1) {
		pr_err("sm2: calu t error\n");
		return -2;
	}

	return 0;
}

static s32 sunxi_sm2_ecdh_test(struct sunxi_sm2_ctx_t *sm2_ctx)
{
	int ret;

	//-1-gen key_a rax || ray
	ret = sm2_ecdh_gen_key_rx_ry(sm2_ctx, ECDH_USER_A);
	if (ret) {
		pr_err("ra: gen_curve_rx_ry fail\n");
		return -1;
	}

	//-2-gen key_b rbx || rby
	ret = sm2_ecdh_gen_key_rx_ry(sm2_ctx, ECDH_USER_B);
	if (ret) {
		pr_err("rb: gen_curve_rx_ry fail\n");
		return -1;
	}

	//-3-gen ux || uy
	ret = sm2_ecdh_gen_ux_uy(sm2_ctx);
	if (ret) {
		pr_err("sm2_ecdh_gen_ux_uy fail\n");
		return -1;
	}

	return 0;
}

int do_sm2_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	u32 sm2_Fp_256_k[8] = {0x59276e27, 0xd506861a, 0x16680f3a, 0xd9c02dcc,
						0xef3cc1fa, 0x3cdbe4ce, 0x6d54b80d, 0xeac1bc21};
	u32 sm2_Fp_256_p[8] = {0xFFFFFFFE, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
						0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF};
	u32 sm2_Fp_256_a[8] = {0xFFFFFFFE, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
						0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFC};
	u32 sm2_Fp_256_b[8] = {0x28E9FA9E, 0x9D9F5E34, 0x4D5A9E4B, 0xCF6509A7,
						0xF39789F5, 0x15AB8F92, 0xDDBCBD41, 0x4D940E93};
	u32 sm2_Fp_256_gx[8] = {0x32C4AE2C, 0x1F198119, 0x5F990446, 0x6A39C994,
						0x8FE30BBF, 0xF2660BE1, 0x715A4589, 0x334C74C7};
	u32 sm2_Fp_256_gy[8] = {0xBC3736A2, 0xF4F6779C, 0x59BDCEE3, 0x6B692153,
						0xD0A9877C, 0xC62A4740, 0x02DF32E5, 0x2139F0A0};
	u32 sm2_Fp_256_n[8] = {0xFFFFFFFE, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
						0x7203DF6B, 0x21C6052B, 0x53BBF409, 0x39D54123};
	u32 sm2_Fp_256_h[8] = {0x00000001, 0x00000000, 0x00000000, 0x00000000,
						0x00000000, 0x00000000, 0x00000000, 0x00000000};
	u32 sm2_Fp_256_m[8] = {0x656E63, 0x72797074, 0x696F6E20, 0x7374616E, 0x64617264};
	u32 sm2_Fp_256_d[8] = {0x3945208F, 0x7B2144B1, 0x3F36E38A, 0xC6D39F95,
							0x88939369, 0x2860B51A, 0x42FB81EF, 0x4DF7C5B8};
	u32 sm2_Fp_256_px[8] = {0x09F9DF31, 0x1E5421A1, 0x50DD7D16, 0x1E4BC5C6,
							0x72179FAD, 0x1833FC07, 0x6BB08FF3, 0x56F35020};
	u32 sm2_Fp_256_py[8] = {0xCCEA490C, 0xE26775A5, 0x2DC6EA71, 0x8CC1AA60,
							0x0AED05FB, 0xF35E084A, 0x6632F607, 0x2DA9AD13};
	u32 sm2_Fp_256_e[8] = {0xF0B43E94, 0xBA45ACCA, 0xACE692ED, 0x534382EB,
							0x17E6AB5A, 0x19CE7B31, 0xF4486FDF, 0xC0D28640};
	u32 sm2_Fp_256_cx[8] = {0x0};
	u32 sm2_Fp_256_cy[8] = {0x0};
	u32 sm2_Fp_256_kx[8] = {0x0};
	u32 sm2_Fp_256_ky[8] = {0x0};
	u32 sm2_Fp_256_out[8] = {0x0};
	u32 sm2_Fp_256_r[8] = {0x0};
	u32 sm2_Fp_256_s[8] = {0x0};
	u32 sm2_Fp_256_rax[8] = {0x0};
	u32 sm2_Fp_256_ray[8] = {0x0};
	u32 sm2_Fp_256_rbx[8] = {0x0};
	u32 sm2_Fp_256_rby[8] = {0x0};
	u32 sm2_Fp_256_ra[8] = {0xD4DE1547, 0x4DB74D06, 0x491C440D, 0x305E0124,
							0x00990F3E, 0x390C7E87, 0x153C12DB, 0x2EA60BB3};
	u32 sm2_Fp_256_pxa[8] = {0x160E1289, 0x7DF4EDB6, 0x1DD812FE, 0xB96748FB,
							0xD3CCF4FF, 0xE26AA6F6, 0xDB9540AF, 0x49C94232};
	u32 sm2_Fp_256_pya[8] = {0x4A7DAD08, 0xBB9A4595, 0x31694BEB, 0x20AA489D,
							0x6649975E, 0x1BFCF8C4, 0x741B78B4, 0xB223007F};
	u32 sm2_Fp_256_da[8] = {0x81EB26E9, 0x41BB5AF1, 0x6DF11649, 0x5F906952,
							0x72AE2CD6, 0x3D6C4AE1, 0x678418BE, 0x48230029};
	u32 sm2_Fp_256_rb[8] = {0x7E071248, 0x14B30948, 0x9125EAED, 0x10111316,
							0x4EBF0F34, 0x58C5BD88, 0x335C1F9D, 0x596243D6};
	u32 sm2_Fp_256_pxb[8] = {0x6AE848C5, 0x7C53C7B1, 0xB5FA99EB, 0x2286AF07,
							0x8BA64C64, 0x591B8B56, 0x6F7357D5, 0x76F16DFB};
	u32 sm2_Fp_256_pyb[8] = {0xEE489D77, 0x1621A27B, 0x36C5C799, 0x2062E9CD,
							0x09A92643, 0x86F3FBEA, 0x54DFF693, 0x05621C4D};
	u32 sm2_Fp_256_db[8] = {0x78512991, 0x7D45A9EA, 0x5437A593, 0x56B82338,
							0xEAADDA6C, 0xEB199088, 0xF14AE10D, 0xEFA229B5};
	u32 demod_r[8] = {0xF5A03B06, 0x48D2C463, 0x0EEAC513, 0xE1BB81A1,
							0x5944DA38, 0x27D5B741, 0x43AC7EAC, 0xEEE720B3};
	u32 demod_s[8] = {0xB1B6AA29, 0xDF212FD8, 0x763182BC, 0x0D421CA1,
							0xBB9038FD, 0x1F7F42D4, 0x840B69C4, 0x85BBC1AA};
	struct sunxi_sm2_ctx_t sm2_ctx;
	int ret1, ret2;

	memset(&sm2_ctx, 0x0, sizeof(struct sunxi_sm2_ctx_t));

	sm2_ctx.sm2_size = 256;
	sm2_ctx.k_len = sizeof(sm2_Fp_256_k);
	sm2_ctx.k = (void *)sm2_Fp_256_k;
	sm2_ctx.n = (void *)sm2_Fp_256_n;
	sm2_ctx.p = (void *)sm2_Fp_256_p;
	sm2_ctx.a = (void *)sm2_Fp_256_a;
	sm2_ctx.b = (void *)sm2_Fp_256_b;
	sm2_ctx.gx = (void *)sm2_Fp_256_gx;
	sm2_ctx.gy = (void *)sm2_Fp_256_gy;
	sm2_ctx.m = (void *)sm2_Fp_256_m;
	sm2_ctx.m_len = sizeof(sm2_Fp_256_m);
	sm2_ctx.e = (void *)sm2_Fp_256_e;
	sm2_ctx.e_len = 256;
	sm2_ctx.d = (void *)sm2_Fp_256_d;
	sm2_ctx.px = (void *)sm2_Fp_256_px;
	sm2_ctx.py = (void *)sm2_Fp_256_py;
	sm2_ctx.h = (void *)sm2_Fp_256_h;
	sm2_ctx.cx = (void *)sm2_Fp_256_cx;
	sm2_ctx.cy = (void *)sm2_Fp_256_cy;
	sm2_ctx.kx = (void *)sm2_Fp_256_kx;
	sm2_ctx.ky = (void *)sm2_Fp_256_ky;
	sm2_ctx.out = (void *)sm2_Fp_256_out;
	sm2_ctx.r = (void *)sm2_Fp_256_r;
	sm2_ctx.s = (void *)sm2_Fp_256_s;
	sm2_ctx.ra = (void *)sm2_Fp_256_ra;
	sm2_ctx.pxa = (void *)sm2_Fp_256_pxa;
	sm2_ctx.pya = (void *)sm2_Fp_256_pya;
	sm2_ctx.da = (void *)sm2_Fp_256_da;
	sm2_ctx.rb = (void *)sm2_Fp_256_rb;
	sm2_ctx.pxb = (void *)sm2_Fp_256_pxb;
	sm2_ctx.pyb = (void *)sm2_Fp_256_pyb;
	sm2_ctx.db = (void *)sm2_Fp_256_db;
	sm2_ctx.rax = (void *)sm2_Fp_256_rax;
	sm2_ctx.ray = (void *)sm2_Fp_256_ray;
	sm2_ctx.rbx = (void *)sm2_Fp_256_rbx;
	sm2_ctx.rby = (void *)sm2_Fp_256_rby;

	sunxi_ss_open();

	sm2_ctx.mode = 0x0;
	tick_printf("sm2 encrypt start\n");
	ret1 = sunxi_sm2_crypto_test(&sm2_ctx);
	if (ret1 == 0) {
		pr_err("ce caclu sucess\n");
	} else {
		pr_err("ce caclu fail\n");
		return -1;
	}

	sm2_ctx.mode = 2;
	tick_printf("sm2 signature start\n");
	ret1 = sunxi_sm2_sign_verify_test(&sm2_ctx);
	if (ret1 == 0) {
		pr_err("ce caclu sucess\n");
	} else {
		pr_err("ce caclu fail\n");
		return -1;
	}
	ret1 = memcmp(demod_r, sm2_ctx.r, 32);
	ret2 = memcmp(demod_s, sm2_ctx.s, 32);
	if ((ret1 || ret2)) {
		pr_err("compare is fail\n");
		pr_err("r(ce)\n");
		sunxi_dump(sm2_ctx.r, 32);
		pr_err("r(demod)\n");
		sunxi_dump(demod_r, 32);
		pr_err("s(ce)\n");
		sunxi_dump(sm2_ctx.s, 32);
		pr_err("s(demod)\n");
		sunxi_dump(demod_s, 32);
		return -1;
	} else {
		pr_err("signature is sucess\n");
	}

	sm2_ctx.mode = 3;
	if (sm2_ctx.mode == 3) {
		tick_printf("sm2 signature verify start\n");
		ret1 = sunxi_sm2_sign_verify_test(&sm2_ctx);
		if (ret1 == 0) {
			pr_err("signature verify sucess\n");
		} else {
			pr_err("signature verify fail\n");
			return -1;
		}
	}

	sm2_ctx.mode = 0x4;
	tick_printf("sm2 ecdh start\n");
	ret1 = sunxi_sm2_ecdh_test(&sm2_ctx);
	if (ret1 == 0) {
		pr_err("ce ecdh sucess\n");
	} else {
		pr_err("ce ecdh fail\n");
		return -1;
	}

	return CMD_RET_SUCCESS;
}

s32 sunxi_normal_rsa(u8 *n_addr, u32 n_len, u8 *e_addr, u32 e_len, u8 *dst_addr,
		     u32 dst_len, u8 *src_addr, u32 src_len);
int do_rsa_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	/* 2048 bit */
	u8 rsa[256]  = { 0 };
	phys_addr_t n_addr   = simple_strtol(argv[1], NULL, 16);
	u32 n_len    = simple_strtol(argv[2], NULL, 16);
	phys_addr_t e_addr   = simple_strtol(argv[3], NULL, 16);
	u32 e_len    = simple_strtol(argv[4], NULL, 16);
	phys_addr_t src_addr = simple_strtol(argv[5], NULL, 16);
	u32 src_len  = simple_strtol(argv[6], NULL, 16);

	if ((n_len > 256) || (e_len > 256) || (src_len > 256)) {
		pr_err("len invalid, n:0x%x e:0x%x src:0x%x\n", n_len, e_len,
		       src_len);
	}
	tick_printf("rsa test start 0\n");
	sunxi_ss_open();
	sunxi_normal_rsa((u8 *)IOMEM_ADDR(n_addr), n_len, (u8 *)IOMEM_ADDR(e_addr), e_len, rsa, 256,
			 (u8 *)IOMEM_ADDR(src_addr), src_len);
	tick_printf("rsa test end\n");
	tick_printf("key n\n");
	sunxi_dump((u8 *)IOMEM_ADDR(n_addr), n_len);
	tick_printf("key e\n");
	sunxi_dump((u8 *)IOMEM_ADDR(e_addr), e_len);
	tick_printf("src data\n");
	sunxi_dump((u8 *)IOMEM_ADDR(src_addr), src_len);
	tick_printf("rsa\n");
	sunxi_dump(rsa, 256);
	return CMD_RET_SUCCESS;
}

int sunxi_trng_gen(u8 *rng_buf, u32 rng_byte);
int do_rng_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	phys_addr_t x1 = simple_strtol(argv[1], NULL, 16);
	u32 x2 = simple_strtol(argv[2], NULL, 16);

	if (x2 != 32) {
		pr_err("only 32 bytes len supported only\n");
		return CMD_RET_FAILURE;
	}
	tick_printf("rng test start 0\n");
	sunxi_ss_open();
	sunxi_trng_gen((u8 *)IOMEM_ADDR(x1), x2);
	tick_printf("rng test end\n");
	tick_printf("rng\n");
	sunxi_dump((u8 *)IOMEM_ADDR(x1), x2);

	return CMD_RET_SUCCESS;
}

#define SUNXI_SYMMETRIC_CTRL							\
	((SS_AES_KEY_128BIT << 0) | (SS_AES_MODE_CBC << 8) | \
	 (SS_KEY_SELECT_INPUT << 20))
#define SSK_NAME	"ssk"
#define	HUK_NAME	"huk"
#define RSSK_NAME	"rssk"
int sunxi_aes_with_hardware(uint8_t *dst_addr, uint8_t *src_addr, int len,
			    uint8_t *key, uint32_t key_len,
			    uint32_t symmetric_ctl, uint32_t com_ctl);
int do_aes_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	phys_addr_t src_addr = simple_strtol(argv[1], NULL, 16);
	u32 src_len  = simple_strtol(argv[2], NULL, 16);
	phys_addr_t enc_addr = simple_strtol(argv[3], NULL, 16);
	phys_addr_t dec_addr = simple_strtol(argv[4], NULL, 16);
	phys_addr_t key_addr = 0;
	u32 key_len = 16;
	u32 c_ctl = ALG_AES << 0;
	u32 s_ctl, byte_algin = 0;
	s32 ret;

	if ((strcmp(argv[5], SSK_NAME) == 0) &&
				(strlen(argv[5]) == strlen(SSK_NAME))) {
		s_ctl = ((SS_AES_KEY_256BIT << 0) |
				(SS_AES_MODE_CBC << 8) | (SS_KEY_SELECT_SSK << 20));
		key_len = 32;
	} else if ((strcmp(argv[5], HUK_NAME) == 0) &&
				(strlen(argv[5]) == strlen(HUK_NAME))) {
		s_ctl = ((SS_AES_KEY_192BIT << 0) |
				(SS_AES_MODE_CBC << 8) | (SS_KEY_SELECT_HUK << 20));
		key_len = 24;
	} else if ((strcmp(argv[5], RSSK_NAME) == 0) &&
				(strlen(argv[5]) == strlen(RSSK_NAME))) {
		s_ctl = ((SS_AES_KEY_128BIT << 0) |
				(SS_AES_MODE_CBC << 8) | (SS_KEY_SELECT_RSSK << 20));
		key_len = 16;
	} else {
		s_ctl = ((SS_AES_KEY_128BIT << 0) |
				(SS_AES_MODE_CBC << 8) | (SS_KEY_SELECT_INPUT << 20));
		key_addr = simple_strtol(argv[5], NULL, 16);
	}

	if (argc == 7) {
		if (strcmp(argv[6], "sm4") == 0) {
			c_ctl = ALG_SM4 << 0;
			pr_err("test sm4\n");
		}
	}

	if (argc == 8) {
		byte_algin = simple_strtol(argv[7], NULL, 16);
	}

	if (!byte_algin) {
		if ((src_addr & (CACHE_LINE_SIZE - 1)) != 0) {
			pr_err("start addr 0x%x not aligned with cache line size 0x%x\n",
					src_addr, CACHE_LINE_SIZE);
			return CMD_RET_FAILURE;
		}

		if ((src_len & (CACHE_LINE_SIZE - 1)) != 0) {
			pr_err("len 0x%x not aligned with cache line size 0x%x\n",
					src_len, CACHE_LINE_SIZE);
			return CMD_RET_FAILURE;
		}

		if ((enc_addr & (CACHE_LINE_SIZE - 1)) != 0) {
			pr_err("enc addr 0x%x not aligned with cache line size 0x%x\n",
					enc_addr, CACHE_LINE_SIZE);
			return CMD_RET_FAILURE;
		}

		if ((dec_addr & (CACHE_LINE_SIZE - 1)) != 0) {
			pr_err("dec addr 0x%x not aligned with cache line size 0x%x\n",
					dec_addr, CACHE_LINE_SIZE);
			return CMD_RET_FAILURE;
		}
	}

	tick_printf("aes test start 0\n");
	sunxi_ss_open();
	c_ctl |= SS_DIR_ENCRYPT << 8;
	sunxi_aes_with_hardware((u8 *)IOMEM_ADDR(enc_addr), (u8 *)IOMEM_ADDR(src_addr), src_len,
				(u8 *)IOMEM_ADDR(key_addr), key_len, s_ctl, c_ctl);

	c_ctl |= SS_DIR_DECRYPT << 8;
	sunxi_aes_with_hardware((u8 *)IOMEM_ADDR(dec_addr), (u8 *)IOMEM_ADDR(enc_addr), src_len,
				(u8 *)IOMEM_ADDR(key_addr), key_len, s_ctl, c_ctl);

	ret = memcmp((u8 *)IOMEM_ADDR(src_addr), (u8 *)IOMEM_ADDR(dec_addr), src_len);
	if (ret != 0) {
		tick_printf("aes test fail\n");
		if (src_len > 256) {
			src_len = 256;
		}
		pr_err("raw\n");
		sunxi_dump((u8 *)IOMEM_ADDR(src_addr), src_len);
		pr_err("enc\n");
		sunxi_dump((u8 *)IOMEM_ADDR(enc_addr), src_len);
		pr_err("dec\n");
		sunxi_dump((u8 *)IOMEM_ADDR(dec_addr), src_len);
	} else {
		tick_printf("aes test sucess\n");
	}

	return CMD_RET_SUCCESS;
}

#define AES_128_ECB_USERKEY_CFG                                              \
	((SS_AES_KEY_128BIT << 0) | (SS_AES_MODE_ECB << 8) |  \
	 (SS_KEY_SELECT_INPUT << 20))

#define AES_256_ECB_USERKEY_CFG                                              \
	((SS_AES_KEY_256BIT << 0) | (SS_AES_MODE_ECB << 8) |  \
	 (SS_KEY_SELECT_INPUT << 20))


int do_aes_perf_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	phys_addr_t src_addr = simple_strtol(argv[1], NULL, 16);
	u32 src_len  = simple_strtol(argv[2], NULL, 16);
	phys_addr_t enc_addr = simple_strtol(argv[3], NULL, 16);
	phys_addr_t dec_addr = simple_strtol(argv[4], NULL, 16);
	phys_addr_t key_addr = 0;
	u32 key_bit = 128;
	u32 key_len = 16;
	u32 c_ctl = ALG_AES << 0;
	u32 s_ctl = AES_128_ECB_USERKEY_CFG;
	int i, ret;
	u64 start, end, tmp;

	key_addr = simple_strtol(argv[5], NULL, 16);
	key_bit = simple_strtol(argv[6], NULL, 10);
	if (key_bit == 128) {
		s_ctl = AES_128_ECB_USERKEY_CFG;
		key_len = key_bit >> 3;
	} else if (key_bit == 256) {
		s_ctl = AES_256_ECB_USERKEY_CFG;
		key_len = key_bit >> 3;
	}
	pr_err("key_len = %d\n", key_len);

	if ((src_addr & (CACHE_LINE_SIZE - 1)) != 0) {
		pr_err("start addr 0x%x not aligned with cache line size 0x%x\n",
		       src_addr, CACHE_LINE_SIZE);
		return CMD_RET_FAILURE;
	}

	if ((src_len & (CACHE_LINE_SIZE - 1)) != 0) {
		pr_err("len 0x%x not aligned with cache line size 0x%x\n",
		       src_len, CACHE_LINE_SIZE);
		return CMD_RET_FAILURE;
	}

	if ((enc_addr & (CACHE_LINE_SIZE - 1)) != 0) {
		pr_err("enc addr 0x%x not aligned with cache line size 0x%x\n",
		       enc_addr, CACHE_LINE_SIZE);
		return CMD_RET_FAILURE;
	}
	if ((dec_addr & (CACHE_LINE_SIZE - 1)) != 0) {
		pr_err("dec addr 0x%x not aligned with cache line size 0x%x\n",
		       dec_addr, CACHE_LINE_SIZE);
		return CMD_RET_FAILURE;
	}

	sunxi_ss_open();
	tick_printf("test 100 times start\n");
	start = read_timer();
	for (i = 0; i < 50; i++) {
		c_ctl &= ~(1 << 8);
		c_ctl |= SS_DIR_ENCRYPT << 8;
		sunxi_aes_with_hardware((u8 *)IOMEM_ADDR(enc_addr),
				(u8 *)IOMEM_ADDR(src_addr), src_len,
				(u8 *)IOMEM_ADDR(key_addr), key_len, s_ctl, c_ctl);

		c_ctl &= ~(1 << 8);
		c_ctl |= SS_DIR_DECRYPT << 8;
		sunxi_aes_with_hardware((u8 *)IOMEM_ADDR(dec_addr),
				(u8 *)IOMEM_ADDR(enc_addr), src_len,
				(u8 *)IOMEM_ADDR(key_addr), key_len, s_ctl, c_ctl);
	}

	end = read_timer();
	tmp = end - start;
	pr_err("100 times need counter:%lld\n", tmp);

	ret = memcmp((u8 *)IOMEM_ADDR(src_addr), (u8 *)IOMEM_ADDR(dec_addr), src_len);
	if (ret != 0) {
		tick_printf("aes test fail\n");
		if (src_len > 256) {
			src_len = 256;
		}
		tick_printf("raw\n");
		sunxi_dump((u8 *)IOMEM_ADDR(src_addr), src_len);
		tick_printf("enc\n");
		sunxi_dump((u8 *)IOMEM_ADDR(enc_addr), src_len);
		tick_printf("dec\n");
		sunxi_dump((u8 *)IOMEM_ADDR(dec_addr), src_len);
	} else {
		tick_printf("aes test sucess\n");
	}

	return CMD_RET_SUCCESS;
}

static cmd_tbl_t cmd_ce_test[] = {
	U_BOOT_CMD_MKENT(hash, 5, 0, do_hash_test, "", ""),
	U_BOOT_CMD_MKENT(rsa, 10, 0, do_rsa_test, "", ""),
	U_BOOT_CMD_MKENT(rng, 5, 0, do_rng_test, "", ""),
	U_BOOT_CMD_MKENT(aes, 10, 0, do_aes_test, "", ""),
	U_BOOT_CMD_MKENT(sm2, 10, 0, do_sm2_test, "", ""),
	U_BOOT_CMD_MKENT(aes_perf_test, 10, 0, do_aes_perf_test, "", ""),
};

int do_sunxi_ce_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	cmd_tbl_t *cp;

	cp = find_cmd_tbl(argv[1], cmd_ce_test, ARRAY_SIZE(cmd_ce_test));
	/* Drop the sunxi_ce_test command */
	argc--;
	argv++;

	if (cp)
		return cp->cmd(cmdtp, flag, argc, argv);
	else {
		pr_err("unknown sub command\n");
		return CMD_RET_USAGE;
	}
}

U_BOOT_CMD(sunxi_ce_test, CONFIG_SYS_MAXARGS, 0, do_sunxi_ce_test,
	   "sunxi ce test run", "NULL");
