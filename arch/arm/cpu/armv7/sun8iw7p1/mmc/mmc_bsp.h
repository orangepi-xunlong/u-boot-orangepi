/*
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */
#ifndef _MMC_BSP_H_
#define _MMC_BSP_H_

/* winner's mmc controller definition */
typedef struct _boot_sdcard_info_t {
	__s32               card_ctrl_num;
	__s32				boot_offset;
	__s32 				card_no[4];
	__s32 				speed_mode[4];
	__s32				line_sel[4];
	__s32				line_count[4];
	__s32  	        	sdc_2xmode[4];
	__s32				ddrmode[4];
	__s32               sdc_f_max[4];

}
boot_sdcard_info_t;

/* winner's mmc controller definition */
struct sunxi_mmc {
	volatile    u32 gctrl;         /* (0x00) SMC Global Control Register */
	volatile    u32 clkcr;         /* (0x04) SMC Clock Control Register */
	volatile    u32 timeout;       /* (0x08) SMC Time Out Register */
	volatile    u32 width;         /* (0x0C) SMC Bus Width Register */
	volatile    u32 blksz;         /* (0x10) SMC Block Size Register */
	volatile    u32 bytecnt;       /* (0x14) SMC Byte Count Register */
	volatile    u32 cmd;           /* (0x18) SMC Command Register */
	volatile    u32 arg;           /* (0x1C) SMC Argument Register */
	volatile    u32 resp0;         /* (0x20) SMC Response Register 0 */
	volatile    u32 resp1;         /* (0x24) SMC Response Register 1 */
	volatile    u32 resp2;         /* (0x28) SMC Response Register 2 */
	volatile    u32 resp3;         /* (0x2C) SMC Response Register 3 */
	volatile    u32 imask;         /* (0x30) SMC Interrupt Mask Register */
	volatile    u32 mint;          /* (0x34) SMC Masked Interrupt Status Register */
	volatile    u32 rint;          /* (0x38) SMC Raw Interrupt Status Register */
	volatile    u32 status;        /* (0x3C) SMC Status Register */
	volatile    u32 ftrglevel;     /* (0x40) SMC FIFO Threshold Watermark Register */
	volatile    u32 funcsel;       /* (0x44) SMC Function Select Register */
	volatile    u32 cbcr;          /* (0x48) SMC CIU Byte Count Register */
	volatile    u32 bbcr;          /* (0x4C) SMC BIU Byte Count Register */
	volatile    u32 dbgc;          /* (0x50) SMC Debug Enable Register */
	volatile    u32 res;           /* (0x54)*/
	volatile    u32 a12a;          /* (0x58)Auto command 12 argument*/
	volatile    u32 ntsr;          /* (0x5c)SMC2 Newtiming Set Register */
	volatile    u32 res0[6];       /* (0x5d~0x74) */
	volatile    u32 hwrst;         /* (0x78) SMC eMMC Hardware Reset Register */
	volatile    u32 res1;          /* (0x7c) */
	volatile    u32 dmac;          /* (0x80) SMC IDMAC Control Register */
	volatile    u32 dlba;          /* (0x84) SMC IDMAC Descriptor List Base Address Register */
	volatile    u32 idst;          /* (0x88) SMC IDMAC Status Register */
	volatile    u32 idie;          /* (0x8C) SMC IDMAC Interrupt Enable Register */
	volatile    u32 chda;          /* (0x90) */
	volatile    u32 cbda;          /* (0x94) */
	volatile    u32 res2[26];      /* (0x98~0xff) */
	volatile    u32 thldc;		 /* (0x100) Card Threshold Control Register */
	volatile    u32 res3[2];		 /* (0x104~0x10b) */
	volatile    u32 dsbd;		 /* (0x10c) eMMC4.5 DDR Start Bit Detection Control */
	volatile    u32 res4[60];		/* (0x110~0x1ff) */
	volatile    u32 fifo;          /* (0x200) SMC FIFO Access Address */
};

struct sunxi_mmc_des {
	u32:1,
	dic:1, /* disable interrupt on completion */
	last_des:1, /* 1-this data buffer is the last buffer */
	first_des:1, /* 1-data buffer is the first buffer,
					   0-data buffer contained in the next descriptor is 1st buffer */
	des_chain:1, /* 1-the 2nd address in the descriptor is the next descriptor address */
	end_of_ring:1, /* 1-last descriptor flag when using dual data buffer in descriptor */
				: 24,
	card_err_sum:1, /* transfer error flag */
	own:1; /* des owner:1-idma owns it, 0-host owns it */
#if defined MMC_SUN4I
#define SDXC_DES_NUM_SHIFT 12
#define SDXC_DES_BUFFER_MAX_LEN	(1 << SDXC_DES_NUM_SHIFT)
	u32	data_buf1_sz:13,
	    data_buf2_sz:13,
					: 6;
#else
#define SDXC_DES_NUM_SHIFT 15
#define SDXC_DES_BUFFER_MAX_LEN	(1 << SDXC_DES_NUM_SHIFT)
	u32 data_buf1_sz:16,
	    data_buf2_sz:16;
#endif
	u32	buf_addr_ptr1;
	u32	buf_addr_ptr2;
};

struct sunxi_mmc_host {
	struct sunxi_mmc *reg;
	u32  mmc_no;
	u32  mclk;
	u32  hclkrst;
	u32  hclkbase;
	u32  mclkbase;
	u32  database;
	u32	 commreg;
	u32  fatal_err;
	struct sunxi_mmc_des *pdes;
	u32  host_func;
};


struct mmc_cmd {
	unsigned cmdidx;
	unsigned resp_type;
	unsigned cmdarg;
	unsigned response[4];
	unsigned flags;
};

struct mmc_data {
	union {
		char *dest;
		const char *src; /* src buffers don't get written to */
	} b;
	unsigned flags;
	unsigned blocks;
	unsigned blocksize;
};

struct tuning_sdly {
	/* u8 sdly_400k; */
	u8 sdly_25M;
	u8 sdly_50M;
	u8 sdly_100M;
	u8 sdly_200M;
};/* size can not over 256 now */

#endif /* _MMC_BSP_H_ */