/*
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include "common.h"
#include "asm/io.h"
#include "asm/armv7.h"
#include "asm/arch/ccmu.h"
#include "asm/arch/ss.h"
#include "asm/arch/mmu.h"

#define ALG_SHA256 (0x13)
#define ALG_RSA    (0x20)

static int ss_base_mode = 1;

static u32 __aw_endian4(u32 data)
{
	u32 d1, d2, d3, d4;
	d1 = (data & 0xff) << 24;
	d2 = (data & 0xff00) << 8;
	d3 = (data & 0xff0000) >> 8;
	d4 = (data & 0xff000000) >> 24;

	return (d1 | d2 | d3 | d4);
}

u32 __sha256_padding(u32 data_size, u8* text)
{
	u32 i;
	u32 k, q;
	u32 size;
	u32 padding_buf[16];
	u8 *ptext;

	k = data_size / 64;
	q = data_size % 64;

	ptext = (u8*)padding_buf;
	if (q == 0) {
		for (i = 0; i < 16; i++) {
			padding_buf[i] = 0x0;
		}
		padding_buf[0] = 0x00000080;

		padding_buf[14] = data_size >> 29;
		padding_buf[15] = data_size << 3;
		padding_buf[14] = __aw_endian4(padding_buf[14]);
		padding_buf[15] = __aw_endian4(padding_buf[15]);

		for (i = 0; i < 64; i++) {
			text[k*64 + i] = ptext[i];
		}
		size = (k + 1) * 64;
	} else if (q < 56) {
		for (i = 0; i < 16; i++) {
			padding_buf[i] = 0x0;
		}
		for (i = 0; i < q; i++) {
			ptext[i] = text[k*64 + i];
		}
		ptext[q] = 0x80;


		padding_buf[14] = data_size >> 29;
		padding_buf[15] = data_size << 3;
		padding_buf[14] = __aw_endian4(padding_buf[14]);
		padding_buf[15] = __aw_endian4(padding_buf[15]);

		for (i = 0; i < 64; i++) {
			text[k*64 + i] = ptext[i];
		}
		size = (k + 1) * 64;
	} else {
		for (i = 0; i < 16; i++) {
			padding_buf[i] = 0x0;
		}
		for (i = 0; i < q; i++) {
			ptext[i] = text[k*64 + i];
		}
		ptext[q] = 0x80;
		for (i = 0; i < 64; i++) {
			text[k*64 + i] = ptext[i];
		}

		/*send last 512-bits text to SHA1/MD5*/
		for (i = 0; i < 16; i++) {
			padding_buf[i] = 0x0;
		}
		padding_buf[14] = data_size >> 29;
		padding_buf[15] = data_size << 3;
		padding_buf[14] = __aw_endian4(padding_buf[14]);
		padding_buf[15] = __aw_endian4(padding_buf[15]);

		for (i = 0; i < 64; i++) {
			text[(k + 1)*64 + i] = ptext[i];
		}
		size = (k + 2) * 64;
	}

	return size;
}
static void __ss_encry_decry_end(uint task_id)
{
	uint int_en;

	int_en = readl(SS_S_ICR) & 0xf;
	int_en = int_en & (0x01 << task_id);
	if (int_en != 0) {
		while ((readl(SS_S_ISR)&(0x01 << task_id)) == 0) {};
	}
}
static void __rsa_padding(u8 *dst_buf, u8 *src_buf, u32 data_len, u32 group_len)
{
	int i = 0;

	memset(dst_buf, 0, group_len);
	for (i = group_len - data_len; i < group_len; i++) {
		dst_buf[i] = src_buf[group_len - 1 - i];
	}
}

void sunxi_ss_open(void)
{
	u32  reg_val;

	reg_val = readl(CCMU_CE_CLK_REG);

	/*set CE src clock*/
	reg_val &= ~(CE_CLK_SRC_MASK << CE_CLK_SRC_SEL_BIT);
#ifdef FPGA_PLATFORM
	/* OSC24M */
	reg_val |= 0 << CE_CLK_SRC_SEL_BIT;
#else
	/* PLL_PERI0(2X) */
	reg_val |= CE_CLK_SRC << CE_CLK_SRC_SEL_BIT;
	/*set div n*/
	reg_val &= ~(CE_CLK_DIV_RATION_N_MASK << CE_CLK_DIV_RATION_N_BIT);
	reg_val |= CE_CLK_DIV_RATION_N << CE_CLK_DIV_RATION_N_BIT;
	/*set div m*/
	reg_val &= ~(CE_CLK_DIV_RATION_M_MASK << CE_CLK_DIV_RATION_M_BIT);
	reg_val |= CE_CLK_DIV_RATION_M << CE_CLK_DIV_RATION_M_BIT;
#endif

	/*set src clock on*/
	reg_val |= CE_SCLK_ON << CE_SCLK_ONOFF_BIT;

	writel(reg_val, CCMU_CE_CLK_REG);

	/*open CE gating*/
	reg_val = readl(CE_GATING_BASE);
	reg_val |= CE_GATING_PASS << CE_GATING_BIT;
	writel(reg_val, CE_GATING_BASE);

	/*de-assert*/
	reg_val = readl(CE_RST_REG_BASE);
	reg_val |= CE_DEASSERT << CE_RST_BIT;
	writel(reg_val, CE_RST_REG_BASE);

}

void sunxi_ss_close(void)
{
}

void ss_set_drq(u32 addr)
{
	writel(addr, SS_TDQ);
}

void ss_ctrl_start(u8 alg_type)
{
	writel(alg_type << 8, SS_TLR);
	writel(readl(SS_TLR) | 0x1, SS_TLR);
}

void ss_ctrl_stop(void)
{
	writel(0x0, SS_TLR);
}

void ss_wait_finish(u32 task_id)
{
	uint int_en;
	int_en = readl(SS_ICR) & 0xf;
	int_en = int_en & (0x01 << task_id);
	if (int_en != 0) {
		while ((readl(SS_ISR)&(0x01 << task_id)) == 0) {};
	}
}
void ss_pending_clear(u32 task_id)
{
	u32 reg_val;
	reg_val = readl(SS_ISR);
	if ((reg_val&(0x01 << task_id)) == (0x01 << task_id)) {
		reg_val &= ~(0x0f);
		reg_val |= (0x01 << task_id);
	}
	writel(reg_val, SS_ISR);
}

void ss_irq_enable(u32 task_id)
{
	int val = readl(SS_ICR);

	val |= (0x1 << task_id);
	writel(val, SS_ICR);
}

void ss_irq_disable(u32 task_id)
{
	int val = readl(SS_ICR);

	val &= ~(1 << task_id);
	writel(val, SS_ICR);
}

u32 ss_check_err(void)
{
	return (readl(SS_ERR) & 0xffff);
}


int  sunxi_sha_calc(u8 *dst_addr, u32 dst_len,
                    u8 *src_addr, u32 src_len)
{
	u32 reg_val = 0;
	u32 total_len = 0;
	u32 md_size = 32;
	s32 i = 0;
	u8  sign_buff[32 + 32];
	task_queue task0;

	memset(sign_buff, 0, sizeof(sign_buff));
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_sign, CACHE_LINE_SIZE);

	total_len = __sha256_padding(src_len, (u8 *)src_addr) / 4;

	task0.task_id = 0;
	task0.common_ctl = (19) | (1U << 31);
	task0.symmetric_ctl = 0;
	task0.asymmetric_ctl = 0;
	task0.key_descriptor = 0;
	task0.iv_descriptor = 0;
	task0.ctr_descriptor = 0;
	task0.data_len = total_len;

	/*task0.source[0].addr = va2pa((uint)src_addr);*/
	task0.source[0].addr = (uint)src_addr;
	task0.source[0].length = total_len;

	for (i = 1;i < 8;i++)
		task0.source[i].length = 0;

	task0.destination[0].addr = (uint)p_sign;
	task0.destination[0].length = 32 / 4;
	for (i = 1;i < 8;i++)
		task0.destination[i].length = 0;
	task0.next_descriptor = 0;

	writel((uint)&task0, SS_S_TDQ); /*descriptor address*/
	/*enable SS end interrupt*/
	writel(0x1 << (task0.task_id), SS_S_ICR);
	/*make sure all data has been write to registers*/
	asm volatile ("isb");
	asm volatile ("dsb");
	/*start SS*/
	writel(0x1, SS_S_TLR);
	/*wait end*/
	__ss_encry_decry_end(task0.task_id);

	/*copy data*/
	memcpy(dst_addr, p_sign, md_size);
	/*clear pending*/
	reg_val = readl(SS_S_ISR);
	if ((reg_val&(0x01 << task0.task_id)) == (0x01 << task0.task_id)) {
		reg_val &= ~(0x0f);
		reg_val |= (0x01 << task0.task_id);
	}
	writel(reg_val, SS_S_ISR);
	/*SS engie exit*/
	writel(readl(SS_S_TLR) & (~0x1), SS_S_TLR);

	return 0;
}

s32 sunxi_rsa_calc(u8 * n_addr,   u32 n_len,
                   u8 * e_addr,   u32 e_len,
                   u8 * dst_addr, u32 dst_len,
                   u8 * src_addr, u32 src_len)
{
#define	TEMP_BUFF_LEN	((2048>>3) + 32)
	uint   i;
	/*task_queue task0;*/
	task_queue task0;
	u32 reg_val = 0;
	u32 mod_bit_size = 2048;
	u32 mod_size_len_inbytes = mod_bit_size / 8;

	ALLOC_CACHE_ALIGN_BUFFER(u8, p_n, TEMP_BUFF_LEN);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_e, TEMP_BUFF_LEN);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_src, TEMP_BUFF_LEN);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_dst, TEMP_BUFF_LEN);

	__rsa_padding(p_src, src_addr, src_len, mod_size_len_inbytes);
	__rsa_padding(p_n, n_addr, n_len, mod_size_len_inbytes);
	memset(p_e, 0, mod_size_len_inbytes);
	memcpy(p_e, e_addr, e_len);

	task0.task_id = 0;
	task0.common_ctl = (32 | (1U << 31));    /*ss method:rsa*/
	task0.symmetric_ctl = 0;
	task0.asymmetric_ctl = (2 << 28);
	task0.key_descriptor = (uint)p_e;
	task0.iv_descriptor = (uint)p_n;
	task0.ctr_descriptor = 0;
	task0.data_len = mod_size_len_inbytes / 4;   /*word in uint*/
	task0.source[0].addr = (uint)p_src;
	task0.source[0].length = mod_size_len_inbytes / 4;
	for (i = 1;i < 8;i++)
		task0.source[i].length = 0;
	task0.destination[0].addr = (uint)p_dst;
	task0.destination[0].length = mod_size_len_inbytes / 4;
	for (i = 1;i < 8;i++)
		task0.destination[i].length = 0;
	task0.next_descriptor = 0;

	writel((uint)&task0, SS_S_TDQ); /*descriptor address*/
	/*enable SS end interrupt*/
	writel(0x1 << (task0.task_id), SS_S_ICR);
	/*make sure all data has been write to registers*/
	asm volatile ("isb");
	asm volatile ("dsb");
	/*start SS*/
	writel(0x1, SS_S_TLR);
	/*wait end*/
	__ss_encry_decry_end(task0.task_id);

	__rsa_padding(dst_addr, p_dst, mod_bit_size / 64, mod_bit_size / 64);
	/*clear pending*/
	reg_val = readl(SS_S_ISR);
	if ((reg_val&(0x01 << task0.task_id)) == (0x01 << task0.task_id)) {
		reg_val &= ~(0x0f);
		reg_val |= (0x01 << task0.task_id);
	}
	writel(reg_val, SS_S_ISR);
	/*SS engie exit*/
	writel(readl(SS_S_TLR) & (~0x1), SS_S_TLR);

	return 0;
}
