/*
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include "common.h"
#include "asm/io.h"
#include "asm/arch/ccmu.h"
#include "asm/arch/ss.h"
#include "asm/arch/mmu.h"
#include "ss_op.h"

#define ALG_SHA256  (0x13)
#define ALG_RSA     (0x20)
#define ALG_MD5		(0x10)
#define ALG_TRANG	(0x1C)

static u32 __aw_endian4(u32 data)
{
	u32 d1, d2, d3, d4;
	d1= (data&0xff)<<24;
	d2= (data&0xff00)<<8;
	d3= (data&0xff0000)>>8;
	d4= (data&0xff000000)>>24;

	return (d1|d2|d3|d4);
}

static u32 __sha_padding(u32 data_size, u8* text, u32 hash_mode)
{
	u32 i;
	u32 k, q;
	u32 size;
	u32 padding_buf[16];
    u8 *ptext;

	k = data_size/64;
	q = data_size%64;

	ptext = (u8*)padding_buf;
	memset(padding_buf, 0, 16 * sizeof(u32));
	if(q==0){
		padding_buf[0] = 0x00000080;

		if(hash_mode)
		{
			padding_buf[14] = data_size>>29;
			padding_buf[15] = data_size<<3;
			padding_buf[14] = __aw_endian4(padding_buf[14]);
			padding_buf[15] = __aw_endian4(padding_buf[15]);
		}
		else
		{
			padding_buf[14] = data_size<<3;
			padding_buf[15] = data_size>>29;
		}

		for(i=0; i<64; i++){
			text[k*64 + i] = ptext[i];
		}
		size = (k + 1)*64;
	}else if(q<56)
	{
		for(i=0; i<q; i++){
			ptext[i] = text[k*64 + i];
		}
		ptext[q] = 0x80;

		if(hash_mode)
		{
			padding_buf[14] = data_size>>29;
			padding_buf[15] = data_size<<3;
			padding_buf[14] = __aw_endian4(padding_buf[14]);
			padding_buf[15] = __aw_endian4(padding_buf[15]);
		}
		else
		{
			padding_buf[14] = data_size<<3;
			padding_buf[15] = data_size>>29;
		}

		for(i=0; i<64; i++){
			text[k*64 + i] = ptext[i];
		}
		size = (k + 1)*64;
	}else{
		for(i=0; i<q; i++){
			ptext[i] = text[k*64 + i];
		}
		ptext[q] = 0x80;
		for(i=0; i<64; i++){
			text[k*64 + i] = ptext[i];
		}

		//send last 512-bits text to SHA1/MD5
		for(i=0; i<16; i++){
			padding_buf[i] = 0x0;
		}

		if(hash_mode)
		{
			padding_buf[14] = data_size>>29;
			padding_buf[15] = data_size<<3;
			padding_buf[14] = __aw_endian4(padding_buf[14]);
			padding_buf[15] = __aw_endian4(padding_buf[15]);
		}
		else
		{
			padding_buf[14] = data_size<<3;
			padding_buf[15] = data_size>>29;
		}

		for(i=0; i<64; i++){
			text[(k + 1)*64 + i] = ptext[i];
		}
		size = (k + 2)*64;
	}

	return size;
}



static void __rsa_padding(u8 *dst_buf, u8 *src_buf, u32 data_len, u32 group_len)
{
	int i = 0;

	memset(dst_buf, 0, group_len);
	for(i = group_len - data_len; i < group_len; i++)
	{
		dst_buf[i] = src_buf[group_len - 1 - i];
	}
}

void sunxi_ss_open(void)
{
	//config all clk resource for ce,reset hw.
	ss_open();
}

void sunxi_ss_close(void)
{
	ss_close();
}

int  sunxi_md5_calc(u8 *dst_addr, u32 dst_len, u8 *src_addr, u32 src_len)
{
	u32 word_len = 0, src_align_len = 0;
	u32 total_bit_len = 0;
	task_queue task0  __aligned(CACHE_LINE_SIZE) = {0};
	/* sha256  2word, sha512 4word*/
	ALLOC_CACHE_ALIGN_BUFFER(u32, total_package_len, 4);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_sign, CACHE_LINE_SIZE);
	ss_open();
	memset(p_sign, 0, sizeof(p_sign));

	if (ss_get_ver() < 2) {
		/* CE1.0 */
		src_align_len = __sha_padding(src_len, (u8 *)src_addr, 1);
		word_len = src_align_len>>2;

		task0.task_id = 0;
		task0.common_ctl = (ALG_MD5)|(1U << 31);
		task0.data_len = word_len;

	} else {
		/* CE2.0 */
		src_align_len = ALIGN(src_len, 4);
		word_len  = src_align_len>>2;

		total_bit_len = src_len<<3;
		total_package_len[0] = total_bit_len;
		total_package_len[1] = 0;

		task0.task_id = 0;
		task0.common_ctl = (ALG_MD5)|(1<<15)|(1U << 31);
		task0.key_descriptor = (u32)total_package_len;
		task0.data_len = total_bit_len;
	}

	task0.source[0].addr = (uint)src_addr;
	task0.source[0].length = word_len;
	task0.destination[0].addr = (uint)p_sign;
	task0.destination[0].length = dst_len>>2;
	task0.next_descriptor = 0;

	flush_cache((u32)&task0, sizeof(task0));
	flush_cache((u32)p_sign, CACHE_LINE_SIZE);
	flush_cache((u32)src_addr, src_align_len);
	flush_cache((u32)total_package_len, sizeof(total_package_len));

	ss_set_drq((u32)&task0);
	ss_irq_enable(task0.task_id);
	ss_ctrl_start(ALG_MD5);
	ss_wait_finish(task0.task_id);
	ss_pending_clear(task0.task_id);
	ss_ctrl_stop();
	ss_irq_disable(task0.task_id);
	if (ss_check_err()) {
		printf("SS %s fail 0x%x\n", __func__, ss_check_err());
	}

	invalidate_dcache_range((ulong)p_sign, (ulong)p_sign + CACHE_LINE_SIZE);
	/*copy data*/
	memcpy(dst_addr, p_sign, dst_len);
	return 0;
}


int  sunxi_sha_calc(u8 *dst_addr, u32 dst_len,
					u8 *src_addr, u32 src_len)
{
	u32 word_len = 0,src_align_len = 0;
	u32 total_bit_len = 0;
	task_queue task0  __aligned(CACHE_LINE_SIZE) = {0};
	/* sha256  2word, sha512 4word*/
	ALLOC_CACHE_ALIGN_BUFFER(u32, total_package_len, 4);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_sign, CACHE_LINE_SIZE);

	memset(p_sign, 0, sizeof(p_sign));

	if(ss_get_ver() < 2)
	{
		/* CE1.0 */
		src_align_len = __sha_padding(src_len, (u8 *)src_addr, 1);
		word_len = src_align_len>>2;

		task0.task_id = 0;
		task0.common_ctl = (ALG_SHA256)|(1U << 31);
		task0.data_len = word_len;

	}
	else
	{
		/* CE2.0 */
		src_align_len = ALIGN(src_len,4);
		word_len  = src_align_len>>2;

		total_bit_len = src_len<<3;
		total_package_len[0] = total_bit_len;
		total_package_len[1] = 0;

		task0.task_id = 0;
		task0.common_ctl = (ALG_SHA256)|(1<<15)|(1U << 31);
		task0.key_descriptor = (u32)total_package_len;
		task0.data_len = total_bit_len;
	}

	task0.source[0].addr = (uint)src_addr;
	task0.source[0].length = word_len;
	task0.destination[0].addr = (uint)p_sign;
	task0.destination[0].length = 32>>2;
	task0.next_descriptor = 0;

	flush_cache((u32)&task0, sizeof(task0));
	flush_cache((u32)p_sign, CACHE_LINE_SIZE);
	flush_cache((u32)src_addr, src_align_len);
	flush_cache((u32)total_package_len, sizeof(total_package_len));

	ss_set_drq((u32)&task0);
	ss_irq_enable(task0.task_id);
	ss_ctrl_start(ALG_SHA256);
	ss_wait_finish(task0.task_id);
	ss_pending_clear(task0.task_id);
	ss_ctrl_stop();
	ss_irq_disable(task0.task_id);
	if(ss_check_err())
	{
		printf("SS %s fail 0x%x\n",__func__,ss_check_err());
	}

	invalidate_dcache_range((ulong)p_sign,(ulong)p_sign+CACHE_LINE_SIZE);
	//copy data
	memcpy(dst_addr, p_sign, 32);
	return 0;
}

s32 sunxi_rsa_calc(u8 * n_addr,   u32 n_len,
				   u8 * e_addr,   u32 e_len,
				   u8 * dst_addr, u32 dst_len,
				   u8 * src_addr, u32 src_len)
{
	const u32	TEMP_BUFF_LEN =	((2048>>3) + CACHE_LINE_SIZE);

	u32 mod_bit_size = 2048;
	u32 mod_size_len_inbytes = mod_bit_size/8;
	u32 data_word_len = mod_size_len_inbytes/4;

	task_queue task0		__aligned(CACHE_LINE_SIZE) = {0};
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_n, TEMP_BUFF_LEN);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_e, TEMP_BUFF_LEN);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_src, TEMP_BUFF_LEN);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_dst, TEMP_BUFF_LEN);

	__rsa_padding(p_src, src_addr, src_len, mod_size_len_inbytes);
	__rsa_padding(p_n, n_addr, n_len, mod_size_len_inbytes);
	memset(p_e, 0, mod_size_len_inbytes);
	memcpy(p_e, e_addr, e_len);

	if(ss_get_ver() < 2)
	{
		/* CE1.0 */
		task0.task_id = 0;
		task0.common_ctl = (ALG_RSA | (1U<<31));
		task0.symmetric_ctl = 0;
		task0.asymmetric_ctl = (2<<28); /* rsa2048 */
		task0.key_descriptor = (uint)p_e;
		task0.iv_descriptor = (uint)p_n;
		task0.ctr_descriptor = 0;
		task0.data_len = data_word_len;
		task0.source[0].addr= (uint)p_src;
		task0.source[0].length = data_word_len;
		task0.destination[0].addr= (uint)p_dst;
		task0.destination[0].length = data_word_len;
		task0.next_descriptor = 0;
	}
	else
	{
		/* CE2.0 */
		task0.task_id = 0;
		task0.common_ctl = (ALG_RSA | (1U<<31));
		task0.symmetric_ctl = 0;
		task0.asymmetric_ctl = (2048/32); /* rsa2048 */
		task0.ctr_descriptor = 0;

		task0.source[0].addr= (uint)p_e;
		task0.source[0].length = data_word_len;
		task0.source[1].addr= (uint)p_n;
		task0.source[1].length = data_word_len;
		task0.source[2].addr= (uint)p_src;
		task0.source[2].length = data_word_len;

		task0.data_len += task0.source[0].length;
		task0.data_len += task0.source[1].length;
		task0.data_len += task0.source[2].length;
		task0.data_len *= 4; /* byte len */

		task0.destination[0].addr= (uint)p_dst;
		task0.destination[0].length = data_word_len;
	}

	flush_cache((u32)&task0, sizeof(task0));
	flush_cache((u32)p_n, mod_size_len_inbytes);
	flush_cache((u32)p_e, mod_size_len_inbytes);
	flush_cache((u32)p_src, mod_size_len_inbytes);
	flush_cache((u32)p_dst, mod_size_len_inbytes);

	ss_set_drq((u32)&task0);
	ss_irq_enable(task0.task_id);
	ss_ctrl_start(ALG_RSA);
	ss_wait_finish(task0.task_id);
	ss_pending_clear(task0.task_id);
	ss_ctrl_stop();
	ss_irq_disable(task0.task_id);
	if(ss_check_err())
	{
		printf("SS %s fail 0x%x\n",__func__,ss_check_err());
	}

	invalidate_dcache_range((ulong)p_dst,(ulong)p_dst+mod_size_len_inbytes);
	__rsa_padding(dst_addr, p_dst, mod_bit_size/64, mod_bit_size/64);

	return 0;
}

int sunxi_create_rssk(u8 *rssk_buf, u32 rssk_byte)
{
	u32 total_bit_len = 0;
	task_queue task0  __aligned(CACHE_LINE_SIZE) = {0};

	ALLOC_CACHE_ALIGN_BUFFER(u32, total_package_len, 4);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_sign, CACHE_LINE_SIZE);

	memset(p_sign, 0, sizeof(p_sign));
	sunxi_ss_open();
	if (rssk_buf == NULL) {
		return -1;
	}

	total_bit_len = rssk_byte << 3;
	total_package_len[0] = total_bit_len;
	total_package_len[1] = 0;

	task0.task_id = 0;
	task0.common_ctl = (ALG_TRANG)|(0x1U<<31);
	task0.key_descriptor = (uint)total_package_len;
	task0.data_len = total_bit_len;
	task0.source[0].addr = 0;
	task0.source[0].length = 0;
	task0.destination[0].addr = (uint)p_sign;
	task0.destination[0].length = (rssk_byte>>2);
	task0.next_descriptor = 0;

	flush_cache((u32)&task0, sizeof(task0));
	flush_cache((u32)p_sign, CACHE_LINE_SIZE);
	flush_cache((u32)total_package_len, sizeof(total_package_len));

	ss_set_drq((u32)&task0);
	ss_irq_enable(task0.task_id);
	ss_ctrl_start(ALG_TRANG);
	ss_wait_finish(task0.task_id);
	ss_pending_clear(task0.task_id);
	ss_ctrl_stop();
	ss_irq_disable(task0.task_id);

	if (ss_check_err()) {
		printf("RSSK %s fail 0x%x\n", __func__, ss_check_err());
	}

	invalidate_dcache_range((ulong)p_sign, (ulong)p_sign+CACHE_LINE_SIZE);
	/*copy data*/
	memcpy(rssk_buf, p_sign, rssk_byte);

	return 0;


}

//new add begin
//do_work:
static uint32_t ss_do_work(task_queue * ptask)
{
	uint32_t task_id = ptask->task_id;
	uint32_t alg_type = ptask->common_ctl&0xff;

	ss_set_drq((uint32_t)ptask);
	ss_irq_enable(task_id);
	ss_ctrl_start(alg_type);
	ss_wait_finish(task_id);
	ss_pending_clear(task_id);
	ss_ctrl_stop();
	ss_irq_disable(task_id);
	if(ss_check_err())
	{
		printf("SS %s fail 0x%x\n",__func__,ss_check_err());
		return -1;
	}
	return 0;
}

#define ECB_D_MSK 1
#define ECB_E_MSK 2
#define CBC_D_MSK 4
#define CBC_E_MSK 8
#define IV_MAX_BYTES 64

static inline unsigned int get_com_ctl_msk(unsigned int msk)
{
	switch (msk)
	{
		case ECB_E_MSK:
		case CBC_E_MSK:
			return (1U << SS_SYM_OFFSET) | SS_METHOD_AES;
		case ECB_D_MSK:
		case CBC_D_MSK:
			return ((1U << SS_SYM_OFFSET) | (1 << SS_MODE_OFFSET) | SS_METHOD_AES);
	}
	return ((1U << SS_SYM_OFFSET) | (1 << SS_MODE_OFFSET) | SS_METHOD_AES);/*dlt use decrypt*/
}

static inline unsigned int key_len_map(int key_len)
{
	if (key_len == 16)
		return SS_AES_KEY_128BIT;
	else if (key_len == 32)
		return SS_AES_KEY_256BIT;
	return SS_AES_KEY_192BIT;
}

static unsigned int get_sym_ctl_msk(unsigned int msk, int key_len)
{
	unsigned int kl = key_len_map(key_len);

	switch (msk)
	{
		case CBC_E_MSK:
		case CBC_D_MSK:
			return (SS_AES_MODE_CBC << SS_MODE_OFFSET) | kl;
		case ECB_E_MSK:
		case ECB_D_MSK:
			return (SS_AES_MODE_ECB << SS_MODE_OFFSET) | kl;
	}

	printf("%s unknown issue\n", __func__);
	return (SS_AES_MODE_ECB << SS_MODE_OFFSET);/*aes128ecb*/
}

/*pass ball to caller which consider data align issue, not use malloc, because it is just boot*/
/*flow: in uboot stage, there is no complex shareability control between normal,secure world,
*multitask,para flow select,etc .for H6 secure boot design, Now the cpu is in normal world,so use normal regs of ce
*config task descritor
*flag_issym,sub_sym(aes,des,??),aes_sub_mode(ecb,cbc,ofb,cfb,xts??),key_len(128,192,256??)
*key type(hw or sw),key address(if hw, it is string to descripte hw key info),key_len, iv_addr,
*iv_len(if set to 0,no iv), out addr.
*cache flush(all data input from mbus memory field)
*do work func(set der, interrupt enable ,check err or compeletion...)
*cache invalidate(out)
*/
#define DCACHE_ALIGN_LEN 64
static int ss_aes_element(u8* in, u32 in_len, u8* key, u32 key_len,
	u8* iv, u32 iv_len, u8* out, u32* out_len, u32 msk_i)
{

	u8 *src_align;
	unsigned long dst_align;
	task_queue task0 __aligned(64) = {0};

	uint32_t size_4B;

	ALLOC_CACHE_ALIGN_BUFFER(uint8_t, key_map, 64);
	ALLOC_CACHE_ALIGN_BUFFER(uint8_t, iv_map, IV_MAX_BYTES);

	src_align = in;
	dst_align = (unsigned long)out -  ((unsigned long)out % 64);

	if (((u32)src_align % 4) || ((u32)out % 4))
	{
		printf("must pass 4B aligned addr\n");
		return -1;
	}
	ss_open(); /*it is optional, rst hw in case of dirty state*/
	memset(key_map, 0, IV_MAX_BYTES);

	/*extend hw key later if necessary use builtin rule, such as bonding name. i.e.: SUNXI_RSSK*/
	memcpy(key_map, key, key_len);/*key must be non-null because of arg check*/

	if(iv)
		memcpy(iv_map, iv, iv_len);

	memset((void *)&task0, 0x00, sizeof(task_queue));
	size_4B = (ALIGN(in_len, 4)) >> 2;

	task0.task_id = 0;
	task0.common_ctl = get_com_ctl_msk(msk_i);
	task0.symmetric_ctl = get_sym_ctl_msk(msk_i, key_len);
	task0.key_descriptor = (uint32_t)key_map;
	task0.iv_descriptor = iv ? (uint32_t)iv_map : 0;
	task0.data_len = in_len;
	task0.source[0].addr= (uint32_t)src_align;
	task0.source[0].length = size_4B;
	task0.destination[0].addr= (uint32_t)out;
	task0.destination[0].length = size_4B;
	task0.next_descriptor = 0;

	printf("task0.common_ctl is 0x%x, task0.symmetric_ctl is 0x%x", task0.common_ctl, task0.symmetric_ctl);
	//flush&clean cache
	flush_cache((unsigned long)iv_map, sizeof(iv_map));
	flush_cache((unsigned long)key_map, sizeof(key_map));
	flush_cache((unsigned long)src_align, in_len);
	flush_cache((unsigned long)out, in_len);
	flush_cache((unsigned long)&task0, sizeof(task0));

	//start work
	if(ss_do_work(&task0))
	{
		printf("ce hardware err!!\n");
		if (out_len)
		*out_len = 0;
		return -1;
	}
	//invlaid l1 cache
	invalidate_dcache_range((unsigned long)dst_align, (unsigned long)dst_align + in_len + DCACHE_ALIGN_LEN);

	if (out_len)
	*out_len = in_len;
	return 0;
}

int sunxi_ss_aes_ecb_decrypt(u8* in, u32 in_len, u8* key, u32 key_len, u8* out, u32* out_len)
{
	if (!(in && key && in_len && out && key_len))
	{
		printf("%s arg\n", __func__);
		return -1;
	}

	return ss_aes_element(in, in_len, key, key_len, NULL, 0, out,out_len, ECB_D_MSK);
}

int sunxi_ss_aes_ecb_encrypt(u8* in, u32 in_len, u8* key, u32 key_len, u8* out, u32* out_len)
{
	if (!(in && key && in_len && out && key_len))
	{
		printf("%s arg\n", __func__);
		return -1;
	}
	return ss_aes_element(in, in_len, key, key_len, NULL, 0, out, out_len, ECB_E_MSK);
}

int sunxi_ss_aes_cbc_decrypt(u8* in, u32 in_len, u8* key, u32 key_len,
	u8* iv, u32 iv_len, u8* out, u32* out_len)
{
	if (!(in && key && in_len && out && key_len && iv && iv_len))
	{
		printf("%s arg\n", __func__);
		return -1;
	}
	return ss_aes_element(in, in_len, key, key_len, iv, iv_len, out,out_len, CBC_D_MSK);
}

int sunxi_ss_aes_cbc_encrypt(u8* in, u32 in_len, u8* key, u32 key_len,
	u8* iv, u32 iv_len, u8* out, u32* out_len)
{
	if (!(in && key && in_len && out && key_len && iv && iv_len))
	{
		printf("%s arg\n", __func__);
		return -1;
	}
	return ss_aes_element(in, in_len, key, key_len, iv, iv_len, out, out_len, CBC_E_MSK);
}
//new add end

#ifdef SUNXI_HASH_TEST
extern void sunxi_dump(void *addr, unsigned int size);
int do_sha256_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	u8  hash[32] = {0};
	u32 x1 = simple_strtol(argv[1], NULL, 16);
	u32 x2 = simple_strtol(argv[2], NULL, 16);
	if(argc < 3)
	{
		return -1;
	}
	printf("src = 0x%x, len = 0x%x\n", x1,x2);

	tick_printf("sha256 test start 0\n");
	sunxi_ss_open();
	sunxi_sha_calc(hash, 32, (u8*)x1,x2);
	tick_printf("sha256 test end\n");
	sunxi_dump(hash,32);

	return 0;
}

U_BOOT_CMD(
	sha256_test, 3, 0, do_sha256_test,
	"do a sha256 test, arg1: src address, arg2: len(hex)",
	"NULL"
);
#endif

//#define SUNXI_AES_TST 1
#ifdef SUNXI_AES_TST
#define AES_STACK_DATA 1
#ifndef AES_STACK_DATA
u8 in[1024] __aligned(4) ;
u8 out[1024] __aligned(4);
u8 in_temp[1024] __aligned(4) ;
#endif
extern void sunxi_dump(void *addr, unsigned int size);
int do_aes_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{

	u32 out_len;
	int ret;
#ifdef AES_STACK_DATA
	u8 in[1024] __aligned(4) ;
	u8 out[1024] __aligned(4);
	u8 in_temp[1024] __aligned(4) ;
#endif
	u8 key[32] = {0x1,0x2,0x3,0x4};
	u8 iv[32] = {0x1,0x2,0x3,0x4};

	tick_printf("sunxi aes test begin\n");
	ret = sunxi_ss_aes_ecb_encrypt(in, sizeof(in), key, 16, out, &out_len);
	printf("out_len is %d ret is %d\n", out_len, ret);
	ret = sunxi_ss_aes_ecb_decrypt(out, sizeof(out), key, 16, in_temp, &out_len);
	printf("out_len is %d ret is %d\n", out_len, ret);
	if (memcmp(in_temp, in, sizeof(in)))
	{
		printf("sunxi_ss_aes_ecb_encrypt failed!!\n");
		printf("in_temp data:\n");
		sunxi_dump(in_temp, sizeof(in_temp));
		printf("in data:\n");
		sunxi_dump(in, sizeof(in));
		return -1;
	}

	printf("sunxi_ss_aes_ecb_encrypt success!!\n");

	ret = sunxi_ss_aes_cbc_encrypt(in, sizeof(in), key, 16, iv, 16, out, &out_len);
	printf("cbc out_len is %d ret is %d\n", out_len, ret);
	ret = sunxi_ss_aes_cbc_decrypt(out, sizeof(out), key, 16, iv, 16, in_temp, &out_len);
	printf("cbc out_len is %d ret is %d\n", out_len, ret);
	if (memcmp(in_temp, in, sizeof(in)))
	{
		printf("sunxi_ss_aes_ecb_encrypt failed!!\n");
		printf("in_temp data:\n");
		sunxi_dump(in_temp, sizeof(in_temp));
		printf("in data:\n");
		sunxi_dump(in, sizeof(in));
		return -1;
	}

	printf("sunxi_ss_aes_cbc_encrypt success!!\n");
	tick_printf("sunxi aes test end\n");
	return 0;
}

U_BOOT_CMD(
	aes_test, 3, 0, do_aes_test,
	"do aes test",
	"NULL"
);
#endif
#undef SUNXI_AES_TST