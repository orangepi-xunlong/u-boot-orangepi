#include <delay_timer.h>
#include <debug.h>
#include <string.h>
#include <aarch64/arch_helpers.h>
#include "a2_ddr_test.h"
#include "tempsen.h"
#include "bm_ddr.h"

/*cdma register*/
#define CDMA_CHL_CTRL         0x800
#define CDMA_INT_MASK         0x808
#define CDMA_INT_STAT         0x80C
#define CDMA_CMD_FIFO_STAT    0x810
#define CDMA_CMD_ACCP0        0x878
#define CDMA_CMD_ACCP1        0x87C
#define CDMA_CMD_ACCP2        0x880
#define CDMA_CMD_ACCP3        0x884
#define CDMA_CMD_ACCP4        0x888
#define CDMA_CMD_ACCP5        0x88C
#define CDMA_CMD_ACCP6        0x890
#define CDMA_CMD_ACCP7        0x894
#define CDMA_MAX_PAYLOAD      0x9F8
#define CDMA_INT_RAW          0x998

/* CDMA bit definition */
/* CHL_CTRL REG */
#define CDMA_CHL_CTRL_WrQOS_VALUE (0xF<<24)
#define CDMA_CHL_CTRL_RdQOS_VALUE (0xF<<20)
#define CDMA_CHL_CTRL_RdQOS_VALUE_LOW (0xF<<16)
#define CDMA_CHL_CTRL_RdQOS_VALUE_NORMAL (0xF<<12)
#define CDMA_CHL_CTRL_RdQOS_VALUE_HIGH (0xF<<8)
#define CDMA_CHL_CTRL_RdQOS_MODE (0x1<<7)
#define CDMA_CHL_CTRL_IRQ_EN (0x1<<3)  // interrupt output
#define CDMA_CHL_CTRL_DMA_EN (0x1)

/* INT_MAST REG */
#define CDMA_INT_RF_CMD_EPT_MASK (0x1<<3)
#define CDMA_INT_RF_DES_MASK   (0x1)

/* INT_STATUS REG */
#define CDMA_INT_RF_CMD_EPT (0x1<<3)
#define CDMA_INT_RF_DES   (0x1)

/* CMD_FIFO_STATUS REG */
#define CDMA_CMD_FIFO_STATUS_MASK 0xFF

/* MAX_PAYLOAD REG */
#define CDMA_MAX_READ_PAYLOAD (0x7<<3)
#define CDMA_MAX_WRITE_PAYLOAD 0x7

/* CDMA Descriptors */
#define CDMA_CMD_2D_FORMAT_MASK (0x1<<7)
#define CDMA_CMD_2D_FORMAT_BYTE (0x0<<7)
#define CDMA_CMD_2D_FORMAT_WORD (0x1<<7)

#define CDMA_CMD_2D_TRANS_MODE_MASK (0x3<<5)
#define CDMA_CMD_TRANS_DATA_ONLY (0x0<<5)
#define CDMA_CMD_TRANS_WITH_FLUSH (0x1<<5)

#define CDMA_CMD_MODE_STRIDE (0x1<<4)
#define CDMA_CMD_MODE_NORMAL (0x0<<4)
#define CDMA_CMD_IRQ_EN (0x1<<3)
#define CDMA_CMD_DESC_EOD (0x1<<2)
#define CDMA_CMD_DESC_VALID 0x1


#define CDMA_CMD_NORMAL_CFG (CDMA_CMD_MODE_NORMAL | CDMA_CMD_IRQ_EN | \
         CDMA_CMD_DESC_EOD | CDMA_CMD_DESC_VALID)
#define CDMA_CMD_STRIDE_CFG (CDMA_CMD_MODE_STRIDE | CDMA_CMD_IRQ_EN | \
         CDMA_CMD_DESC_EOD | CDMA_CMD_DESC_VALID)

#define CDMA_ADDR_WORD_ALIGN 0xFFFFFFFE //for 2-bytes align

#ifndef bool
#define bool char
#define true 1
#define false 0
#endif

#define TEST_ARRAY_SIZE 512
#define START_ADDRESS 0x110000000

#define CDMA0_INTR 85
#define CDMA1_INTR 86

#define CDMA_NUM 0

#define CDMA_BASE_ADDR  0x20BE8000

#define CDMA_BASE(c) (CDMA_BASE_ADDR + 0x1000 * c)
#define CDMA_REG(c, r) (CDMA_BASE(c) + r)

#define MAX_PAYLOAD  0x9 // 256Bytes

static unsigned int cdma_reg_read(u32 reg)
{
	return mmio_read_32(CDMA_REG(0, reg));
}

static void cdma_reg_write(u32 reg, u32 val)
{
	mmio_write_32(CDMA_REG(0, reg), val);
}

static int cdma_transfer(u64 src, u64 dst, u64 size)
{
	u32 reg_val;
	u32 src_addr_hi = 0;
	u32 src_addr_lo = 0;
	u32 dst_addr_hi = 0;
	u32 dst_addr_lo = 0;
	u32 timeout = 300000;

	/*Enable CDMA & interrupt*/
	reg_val = cdma_reg_read(CDMA_CHL_CTRL);
	reg_val |= (CDMA_CHL_CTRL_DMA_EN | CDMA_CHL_CTRL_IRQ_EN);
	
	reg_val &= ~CDMA_CHL_CTRL_IRQ_EN;
	
	cdma_reg_write(CDMA_CHL_CTRL, reg_val);
	cdma_reg_write(CDMA_MAX_PAYLOAD, MAX_PAYLOAD);

	if (cdma_reg_read(CDMA_CMD_FIFO_STAT) > 7) {
		NOTICE("cdma fifo no available entry\n");
		return -1;
	}

	/* clear rf_des_int_mask */
	reg_val = cdma_reg_read(CDMA_INT_MASK);
	reg_val &= ~(CDMA_INT_RF_DES_MASK);
	cdma_reg_write(CDMA_INT_MASK, reg_val);

	src_addr_hi = (u64)src >> 32;
	src_addr_lo = (u64)src & 0xffffffff;
	dst_addr_hi = (u64)dst >> 32;
	dst_addr_lo = (u64)dst & 0xffffffff;

	cdma_reg_write(CDMA_CMD_ACCP3, src_addr_lo);
	cdma_reg_write(CDMA_CMD_ACCP4, src_addr_hi);
	cdma_reg_write(CDMA_CMD_ACCP5, dst_addr_lo);
	cdma_reg_write(CDMA_CMD_ACCP6, dst_addr_hi);
	/* 1-D descriptor */
	cdma_reg_write(CDMA_CMD_ACCP7, size);
	reg_val = CDMA_CMD_NORMAL_CFG;

	cdma_reg_write(CDMA_CMD_ACCP0, reg_val);

	/* Using interrupt(10s timeout) or polling for detect cmda done */
	while (((cdma_reg_read(CDMA_INT_STAT)) & 0x1) != 0x1) {
		udelay(1);
		if (--timeout == 0) {
			NOTICE("cdma polling wait timeout\n");
			return -1;
		}
	}
	reg_val = cdma_reg_read(CDMA_INT_STAT);
	reg_val |= (1 << 0x0);
	cdma_reg_write(CDMA_INT_STAT, reg_val);
	// NOTICE("cdma transfer using polling mode end\n");
	
	return 0;
}

void cdma_case(u64 src, u64 dst, u32 len)
{
	u32 cdma_len = len;   // 64KByte
	// u64 src = 0x5200000; // rtc ahbsram size: 32k 0x8000
	// u64 dst = 0x25400000; // tpu1 sram size: 4M 

	u32 tpu_clk_reg = 0x26050000;
	u32 tpu1_regmem_region0 = 0x26010100;

	mmio_setbits_32(tpu_clk_reg, 0x2);
	mmio_setbits_32(tpu1_regmem_region0, 0x1);

	for (long i = 0; i < cdma_len; i+=4) {
		if (i%8)
			mmio_write_32(src + i, 0xa5a55a5a);
		else
			mmio_write_32(src + i, 0xffffffff);
	}
	// NOTICE("disable mmu\n");
	// disable_mmu_el3();
	flush_dcache_range(src, cdma_len);
	inv_dcache_range(src, cdma_len);
	for (long i = 0; i < cdma_len; i+=4) {
		if (i%8) {
			if (mmio_read_32(src + i) != 0xa5a55a5a)
				NOTICE("dst val: 0xa5a55a5a, read data from 0x%llx, val: 0x%x\n", src+i, mmio_read_32(src + i));
		} else {
			if (mmio_read_32(src + i) != 0xffffffff)
				NOTICE("dst val: 0xffffffff, read data from 0x%llx, val: 0x%x\n", src+i, mmio_read_32(src + i));
		}
	}
	for (long i = 0; i < cdma_len; i+=4) {
		mmio_write_32(dst + i, 0x33333333);
	}
	flush_dcache_range(dst, cdma_len);
	cdma_transfer(src, dst, cdma_len);
	// memcpy((void *)dst, (void *)src, cdma_len);
	NOTICE("cdma copy from 0x%llx to 0x%llx len: 0x%x done\n", src, dst, len);
	inv_dcache_range(dst, cdma_len);
	inv_dcache_range(src, cdma_len);
	for (int i = 0; i < cdma_len; i+=4) {
		if ((mmio_read_32(src + i) != mmio_read_32(dst + i)) || ((mmio_read_32(src + i) != 0xffffffff) && (mmio_read_32(src + i) != 0xa5a55a5a))) {
			NOTICE("ERROR: i is 0x%x, src is 0x%llx, val: 0x%x, dst is 0x%llx, val: 0x%x\n",
					i, src+i, mmio_read_32(src + i), dst+i, mmio_read_32(dst + i));
		}
	}
	NOTICE("test cdma len 0x%x done\n", cdma_len);
}

void cdma_random(u64 src, u64 dst, u32 len)
{
	u32 cdma_len = len;   // 64KByte

	u32 tpu_clk_reg = 0x26050000;
	u32 tpu1_regmem_region0 = 0x26010100;

	mmio_setbits_32(tpu_clk_reg, 0x2);
	mmio_setbits_32(tpu1_regmem_region0, 0x1);

	for (long i = 0; i < cdma_len; i+=4) {
		mmio_write_32(src + i, (src + i)&0xffffffff);
	}
	for (long i = 0; i < cdma_len; i+=4) {
		mmio_write_32(dst + i, ((dst + i)&0xffff) | 0x33330000);
	}
	
	flush_dcache_range(src, cdma_len);
	flush_dcache_range(dst, cdma_len);
	flush_dcache_range(0x110000000, 0x1000000);
	flush_dcache_range(0x180000000, 0x1000000);
	inv_dcache_range(src, cdma_len);
	for (long i = 0; i < cdma_len; i+=4) {
		if (mmio_read_32(src + i) != ((src + i)&0xffffffff))
			NOTICE("read data from 0x%llx, val: 0x%x\n", src+i, mmio_read_32(src + i));
	}

	cdma_transfer(src, dst, cdma_len);
	NOTICE("cdma copy from 0x%llx to 0x%llx len: 0x%x done\n", src, dst, len);
	inv_dcache_range(dst, cdma_len);
	inv_dcache_range(src, cdma_len);
	for (int i = 0; i < cdma_len; i+=4) {
		if ((mmio_read_32(src + i) != mmio_read_32(dst + i))) {
			NOTICE("ERROR: offset is 0x%x, src is 0x%llx, val: 0x%x, dst is 0x%llx, val: 0x%x\n",
					i, src+i, mmio_read_32(src + i), dst+i, mmio_read_32(dst + i));
		}
	}
	NOTICE("test cdma len 0x%x done\n", cdma_len);
}

void cdma_part(u64 src, u64 dst, u32 len)
{
	u32 cdma_len = len;   // 64KByte
	unsigned long long count = 0;

	u32 tpu_clk_reg = 0x26050000;
	u32 tpu1_regmem_region0 = 0x26010100;

	mmio_setbits_32(tpu_clk_reg, 0x2);
	mmio_setbits_32(tpu1_regmem_region0, 0x1);

	for (long i = 0; i < cdma_len; i+=4) {
		if (i%16 == 0)
			mmio_write_32(src + i, 0xa5a5a5a5);
		if (i%16 == 4)
			mmio_write_32(src + i, 0x5a5a5a5a);
		if (i%16 == 8)
			mmio_write_32(src + i, 0);
		if (i%16 == 0xc)
			mmio_write_32(src + i, 0xffffffff);
	}
	for (long i = 0; i < cdma_len; i+=4) {
		mmio_write_32(dst + i, ((dst + i)&0xffff) | 0x33330000);
	}
	
	flush_dcache_range(src, cdma_len);
	flush_dcache_range(dst, cdma_len);
	flush_dcache_range(0x110000000, 0x1000000);
	flush_dcache_range(0x180000000, 0x1000000);
	inv_dcache_range(src, cdma_len);
	// for (long i = 0; i < cdma_len; i+=4) {
	// 	// if (mmio_read_32(src + i) != ((src + i)&0xffffffff))
	// 	// 	NOTICE("read data from 0x%llx, val: 0x%x\n", src+i, mmio_read_32(src + i));
	// 	if (i%16 == 0)
	// 		mmio_write_32(src + i, 0xa5a5a5a5);
	// 	if (i%16 == 4)
	// 		mmio_write_32(src + i, 0x5a5a5a5a);
	// 	if (i%16 == 8)
	// 		mmio_write_32(src + i, 0);
	// 	if (i%16 == 0xc)
	// 		mmio_write_32(src + i, 0xffffffff);
	// }

	cdma_transfer(src, dst, cdma_len);
	// memcpy((void *)dst, (void *)src, cdma_len);
	inv_dcache_range(dst, cdma_len);
	inv_dcache_range(src, cdma_len);
	for (int i = 0; i < cdma_len; i+=4) {
		if ((mmio_read_32(src + i) != mmio_read_32(dst + i))) {
			if ((mmio_read_32(dst + i)&0xffff0000) == 0x33330000) {
				NOTICE(".");
				count++;
				if ((count%64) == 0)
					NOTICE("\n");
			} else {
				NOTICE("ERROR: offset is 0x%x, src is 0x%llx, val: 0x%x, dst is 0x%llx, val: 0x%x\n",
					i, src+i, mmio_read_32(src + i), dst+i, mmio_read_32(dst + i));
			}
		}
	}
}

void cpu_fill(u64 src, u32 len, u32 val)
{
    u64 i;

	for (i = src; i < (src + len); i = i+4) {
		mmio_write_32(i, val);
    }
    return ;
}

void cpu_cp(u64 dst, u64 src, u32 len)
{
    memcpy((void *) dst, (void *)src, len);
}

void cdma_cp(u64 dst, u64 src, u32 len)
{
    cdma_transfer(src, dst, len);
}

void gdma_cp(u64 dst, u64 src, u32 len)
{
    gdma_memcpy(dst, src, len);
}

u32 cpu_rd(u64 src, u32 len, u32 val, u32 cnt)
{
    u64 i;
    u32 rd_val;
    u32 ret = 0;
    int loop;

    for (loop = 0; loop < cnt; loop++) {
        for (i = src; i < (src + len); i+=4) {
            rd_val = mmio_read_32(i);
            if (rd_val != val) {
                ret = 1;
                NOTICE("error: loop: %d, src: 0x%llx, val: 0x%x, expect val: 0x%x\n",
                        loop, i, rd_val, val);
            }
        }
    }

    return ret;
}

u32 cpu_rd_gpio(u64 src, u32 len, u32 val, u32 cnt)
{
    u64 i;
    u32 rd_val;
    u32 ret = 0;
    int loop;

	mmio_write_32(0x27012004, 0xffffffff);
	mmio_write_32(0x27013004, 0xffffffff);
	mmio_write_32(0x27012008, 0);
	mmio_write_32(0x27013008, 0);
	mmio_write_32(0x27012000, 0);
	mmio_write_32(0x27013000, 0);
    for (loop = 0; loop < cnt; loop++) {
        for (i = src; i < (src + len); i+=4) {
            rd_val = mmio_read_32(i);
            if (rd_val != val) {
                ret = 1;
				mmio_write_32(0x27012000, 0xffffffff);
				mmio_write_32(0x27013000, 0xffffffff);
                NOTICE("error: loop: %d, src: 0x%llx, val: 0x%x, expect val: 0x%x\n",
                        loop, i, rd_val, val);
				mmio_write_32(0x27012000, 0);
				mmio_write_32(0x27013000, 0);
            }
        }
    }

    return ret;
}

int cpu_cmp(u64 src, u64 dst, u32 len)
{
    u64 i;
    u32 src_val;
    u32 dst_val;
    int ret = 0;

    for (i = src; i < (src + len); i+=4) {
        src_val = mmio_read_32(i);
        dst_val = mmio_read_32(dst+i-src);
        if (src_val != dst_val) {
            ret++;
            NOTICE("error %d: src: 0x%llx, val: 0x%x, dst: 0x%llx, val: 0x%x",
                    ret, i, src_val, dst-src+i, dst_val);
        }
    }

    return ret;
}

void cpu_poll(u64 src, u32 len, u32 poll_num, u32 flag)
{
    u64 i;
    u64 cnt = 0;
    u32 val;
    u32 exp_val;
    u32 err_flag;
	u64 err_time = 0;
	u64 err_cnt = 0;

    for (i = src; i < src + len; i+=4) {
        if (flag == 0)
            cpu_fill(i, 4, (i-0x100000000)&0xffffffff);
        else
            cpu_fill(i, 4, ((i-0x100000000)&0xffffffff)|(1<<(flag-1)));
    }
    while (cnt < poll_num) {
        NOTICE("loop num: %lld\n", cnt);
        for (i = src; i < (src + len); i+=4) {
            val = mmio_read_32(i);
            if (flag == 0)
                exp_val = (u32)((i - 0x100000000)&0xffffffff);
            else
                exp_val = (u32)(((i-0x100000000)&0xffffffff)|(1<<(flag-1)));
            if (val != exp_val) {
                err_flag = 1;
                err_cnt++;
                NOTICE("read error: 0x%llx read val: 0x%x, except val: 0x%x\n",
                        i, val, exp_val);
            }
        }
        if (err_flag == 1) {
            err_flag = 0;
            err_time++;
        }
        NOTICE("loop %lld, total byte: %llu, error times: %lld, total error byte: %lld\n",
                cnt, (cnt+1)*len, err_time, err_cnt);
        NOTICE("tempture: %d\n", read_temp());
		cnt++;
		NOTICE("###################################\n");
    }
}

void cpu_poll_gpio(u64 src, u32 len, u32 poll_num, u32 flag)
{
    u64 i;
    u64 cnt = 0;
    u32 val;
    u32 exp_val;
    u32 err_flag;
	u64 err_time = 0;
	u64 err_cnt = 0;

	mmio_write_32(0x27012004, 0xffffffff);
	mmio_write_32(0x27013004, 0xffffffff);
	mmio_write_32(0x27012008, 0);
	mmio_write_32(0x27013008, 0);
	mmio_write_32(0x27012000, 0);
	mmio_write_32(0x27013000, 0);
    for (i = src; i < src + len; i+=4) {
        if (flag == 0)
            cpu_fill(i, 4, (i-0x100000000)&0xffffffff);
        else
            cpu_fill(i, 4, ((i-0x100000000)&0xffffffff)|(1<<(flag-1)));
    }
    while (cnt < poll_num) {
        NOTICE("loop num: %lld\n", cnt);
        for (i = src; i < (src + len); i+=4) {
            val = mmio_read_32(i);
            if (flag == 0)
                exp_val = (u32)((i - 0x100000000)&0xffffffff);
            else
                exp_val = (u32)(((i-0x100000000)&0xffffffff)|(1<<(flag-1)));
            if (val != exp_val) {
				mmio_write_32(0x27012000, 0xffffffff);
				mmio_write_32(0x27013000, 0xffffffff);
                err_flag = 1;
                err_cnt++;
                NOTICE("read error: 0x%llx read val: 0x%x, except val: 0x%x\n",
                        i, val, exp_val);
				mmio_write_32(0x27012000, 0);
				mmio_write_32(0x27013000, 0);
            }
        }
        if (err_flag == 1) {
            err_flag = 0;
            err_time++;
        }
        NOTICE("loop %lld, total byte: %llu, error times: %lld, total error byte: %lld\n",
                cnt, (cnt+1)*len, err_time, err_cnt);
        NOTICE("tempture: %d\n", read_temp());
		cnt++;
		NOTICE("###################################\n");
    }
}

void cpu_byte3(u64 src, u32 len, u32 poll_num, u32 flag)
{
	u64 i;
    u64 cnt = 0;
    u32 val;
    u32 exp_val;
    u32 err_flag;
	u64 err_time = 0;
	u64 err_cnt = 0;

    for (i = src; i < src + len; i+=4) {
        if (flag == 0)
            cpu_fill(i, 4, (i<<16)&0xffff0000);
        else
            cpu_fill(i, 4, ((i<<16)&0xffff0000)|(1<<(flag-1)));
    }
    while (cnt < poll_num) {
        NOTICE("loop num: %lld\n", cnt);
        for (i = src; i < (src + len); i+=4) {
            val = mmio_read_32(i);
            if (flag == 0)
                exp_val = (u32)((i<<16)&0xffff0000);
            else
                exp_val = (u32)(((i<<16)&0xffff0000)|(1<<(flag-1)));
            if (val != exp_val) {
                err_flag = 1;
                err_cnt++;
                NOTICE("read error: 0x%llx read val: 0x%x, except val: 0x%x\n",
                        i, val, exp_val);
            }
        }
        if (err_flag == 1) {
            err_flag = 0;
            err_time++;
        }
        NOTICE("loop %lld, total byte: %llu, error times: %lld, total error byte: %lld\n",
                cnt, (cnt+1)*len, err_time, err_cnt);
        NOTICE("tempture: %d\n", read_temp());
		cnt++;
		NOTICE("###################################\n");
    }
}

void cpu_poll_z(u64 src, u32 len, u32 poll_num, u32 flag)
{
    u64 i;
    u64 cnt = 0;
    u32 val;
    u32 exp_val;
    u32 err_flag;
	u64 err_time = 0;
	u64 err_cnt = 0;

    for (i = src; i < src + len; i+=4) {
        if (flag == 0)
            cpu_fill(i, 4, (i-0x100000000)&0xffffffff);
        else
            cpu_fill(i, 4, ((i&0xffffffff)&(~(1<<(flag-1)))));
    }
    while (cnt < poll_num) {
        NOTICE("loop num: %lld\n", cnt);
        for (i = src; i < (src + len); i+=4) {
            val = mmio_read_32(i);
            if (flag == 0)
                exp_val = (u32)((i - 0x100000000)&0xffffffff);
            else
                exp_val = (u32)((i&0xffffffff)&(~(1<<(flag-1))));
            if (val != exp_val) {
                err_flag = 1;
                err_cnt++;
                NOTICE("read error: 0x%llx read val: 0x%x, except val: 0x%x\n",
                        i, val, exp_val);
            }
        }
        if (err_flag == 1) {
            err_flag = 0;
            err_time++;
        }
        NOTICE("loop %lld, total byte: %llu, error times: %lld, total error byte: %lld\n",
                cnt, (cnt+1)*len, err_time, err_cnt);
        NOTICE("tempture: %d\n", read_temp());
		cnt++;
		NOTICE("###################################\n");
    }
}

void cpu_2bit_w(u64 src, u32 len, u32 flag)
{
	u64 i;
	u32 tmp;
	u32 bit_f_1;

    for (i = src; i < src + len; i+=4) {
        if (flag == 0) {
            cpu_fill(i, 4, (i-0x100000000)&0xffffffff);
		} else {
			tmp = ((i&0xffffffff) & (~(1<<flag)));
			bit_f_1 = (((i&0xffffffff) >> (flag - 1)) & 0x1);
			tmp = tmp | (bit_f_1 << flag);
            cpu_fill(i, 4, tmp);
		}
    }
}

void cpu_2bit_r(u64 src, u32 len, u32 poll_num, u32 flag)
{
	u64 i;
	u64 cnt = 0;
	u32 val;
	u32 exp_val;
	u32 err_flag;
	u64 err_time = 0;
	u64 err_cnt = 0;
	u32 tmp;
	// u32 bit_f;
	u32 bit_f_1;

	while (cnt < poll_num) {
		NOTICE("loop num: %lld\n", cnt);
		for (i = src; i < (src + len); i+=4) {
			val = mmio_read_32(i);
			if (flag == 0) {
				exp_val = (u32)((i - 0x100000000)&0xffffffff);
			} else {
				tmp = ((i&0xffffffff) & (~(1<<flag)));
				bit_f_1 = (((i&0xffffffff) >> (flag - 1)) & 0x1);
				tmp = tmp | (bit_f_1 << flag);
				exp_val = tmp;
			}
			if (val != exp_val) {
				err_flag = 1;
				err_cnt++;
				NOTICE("read error: 0x%llx read val: 0x%x, except val: 0x%x\n",
						i, val, exp_val);
			}
		}
		if (err_flag == 1) {
			err_flag = 0;
			err_time++;
		}
		NOTICE("loop %lld, total byte: %llu, error times: %lld, total error byte: %lld\n",
				cnt, (cnt+1)*len, err_time, err_cnt);
		NOTICE("tempture: %d\n", read_temp());
		cnt++;
		NOTICE("###################################\n");
    }
}
