// SPDX-License-Identifier: GPL-2.0+
#ifndef SUNXI_MMC_H
#define SUNXI_MMC_H

#include <mmc.h>
#include <asm/arch/mmc.h>
#include <asm-generic/gpio.h>
#include <common.h>
#include <sys_config.h>

/* speed mode */
#define DS26_SDR12            (0)
#define HSSDR52_SDR25         (1)
#define HSDDR52_DDR50         (2)
#define HS200_SDR104          (3)
#define HS400                 (4)
#define MAX_SPD_MD_NUM        (5)

/* frequency point */
#define CLK_400K         (0)
#define CLK_25M          (1)
#define CLK_50M          (2)
#define CLK_100M         (3)
#define CLK_150M         (4)
#define CLK_200M         (5)
#define MAX_CLK_FREQ_NUM (8)

/*
timing mode
0: output and input are both based on [0,1,...,7] pll delay.
1: output and input are both based on phase.
2: output is based on phase, input is based on delay chain except hs400.
	input of hs400 is based on delay chain.
3: output is based on phase, input is based on delay chain.
4: output is based on phase, input is based on delay chain.
    it also support to use delay chain on data strobe signal.
*/
#define SUNXI_MMC_TIMING_MODE_0 0U
#define SUNXI_MMC_TIMING_MODE_1 1U
#define SUNXI_MMC_TIMING_MODE_2 2U
#define SUNXI_MMC_TIMING_MODE_3 3U
#define SUNXI_MMC_TIMING_MODE_4 4U

#define MMC_CLK_SAMPLE_POINIT_MODE_0 8U
#define MMC_CLK_SAMPLE_POINIT_MODE_1 3U
#define MMC_CLK_SAMPLE_POINIT_MODE_2 2U
#define MMC_CLK_SAMPLE_POINIT_MODE_2_HS400 64U
#define MMC_CLK_SAMPLE_POINIT_MODE_3 64U
#define MMC_CLK_SAMPLE_POINIT_MODE_4 64U

#define TM1_OUT_PH90   (0)
#define TM1_OUT_PH180  (1)
#define TM1_IN_PH90    (0)
#define TM1_IN_PH180   (1)
#define TM1_IN_PH270   (2)

#define TM3_OUT_PH90   (0)
#define TM3_OUT_PH180  (1)

#define TM4_OUT_PH90   (0)
#define TM4_OUT_PH180  (1)

/* error number defination */
#define ERR_NO_BEST_DLY (2)

/* need malloc low len when flush unaligned addr cache */
#if defined (SUNXI_MACH_SUN8IW18) || defined (SUNXI_MACH_SUN8IW19)
#define SUNXI_MMC_MALLOC_LOW_LEN	(4 << 20)
#else
#define SUNXI_MMC_MALLOC_LOW_LEN        (16 << 20)
#endif

struct sunxi_mmc_timing_mode0 {
	u32 cur_spd_md;
	u32 cur_freq;
	u8 odly[MAX_SPD_MD_NUM*MAX_CLK_FREQ_NUM];
	u8 sdly[MAX_SPD_MD_NUM*MAX_CLK_FREQ_NUM];
	u8 def_odly[MAX_SPD_MD_NUM*MAX_CLK_FREQ_NUM];
	u8 def_sdly[MAX_SPD_MD_NUM*MAX_CLK_FREQ_NUM];
	u32 sample_point_cnt;
	u8 cur_odly;
	u8 cur_sdly;
};

/* for smhc v4.1x*/
struct sunxi_mmc_timing_mode1 {
	u32 cur_spd_md;
	u32 cur_freq;
	u8 odly[MAX_SPD_MD_NUM*MAX_CLK_FREQ_NUM];
	u8 sdly[MAX_SPD_MD_NUM*MAX_CLK_FREQ_NUM];
	u8 def_odly[MAX_SPD_MD_NUM*MAX_CLK_FREQ_NUM];
	u8 def_sdly[MAX_SPD_MD_NUM*MAX_CLK_FREQ_NUM];
	u32 sample_point_cnt;
	u8 cur_odly;
	u8 cur_sdly;
};

/* for smhc v4.1x*/
struct sunxi_mmc_timing_mode3 {
	u32 cur_spd_md;
	u32 cur_freq;
	u8 odly[MAX_SPD_MD_NUM*MAX_CLK_FREQ_NUM];
	u8 sdly[MAX_SPD_MD_NUM*MAX_CLK_FREQ_NUM];
	u8 def_odly[MAX_SPD_MD_NUM*MAX_CLK_FREQ_NUM];
	u8 def_sdly[MAX_SPD_MD_NUM*MAX_CLK_FREQ_NUM];
	u32 sample_point_cnt;
	u32 sdly_unit_ps;
	u8 dly_calibrate_done;
	u8 cur_odly;
	u8 cur_sdly;
};

/* for smhc v4.5x*/
struct sunxi_mmc_timing_mode4 {
	u32 cur_spd_md;
	u32 cur_freq;
	u8 odly[MAX_SPD_MD_NUM*MAX_CLK_FREQ_NUM];
	u8 sdly[MAX_SPD_MD_NUM*MAX_CLK_FREQ_NUM];
	u8 dsdly[MAX_CLK_FREQ_NUM];
	u8 def_odly[MAX_SPD_MD_NUM*MAX_CLK_FREQ_NUM];
	u8 def_sdly[MAX_SPD_MD_NUM*MAX_CLK_FREQ_NUM];
	u8 def_dsdly[MAX_CLK_FREQ_NUM];
	u32 sample_point_cnt;
	u32 sdly_unit_ps;
	u32 dsdly_unit_ps;
	u8 dly_calibrate_done;
	u8 cur_odly;
	u8 cur_sdly;
	u8 cur_dsdly;
};

/* for smhc v5.1x*/
struct sunxi_mmc_timing_mode2 {
	u32 cur_spd_md;
	u32 cur_freq;
	u8 odly[MAX_SPD_MD_NUM*MAX_CLK_FREQ_NUM];
	u8 sdly[MAX_SPD_MD_NUM*MAX_CLK_FREQ_NUM];
	u8 dsdly[MAX_CLK_FREQ_NUM];
	u8 def_odly[MAX_SPD_MD_NUM*MAX_CLK_FREQ_NUM];
	u8 def_sdly[MAX_SPD_MD_NUM*MAX_CLK_FREQ_NUM];
	u8 def_dsdly[MAX_CLK_FREQ_NUM];
	u32 sample_point_cnt;
	//u32 sdly_unit_ps;
	u32 sample_point_cnt_hs400;
	u32 dsdly_unit_ps;
	u8 dly_calibrate_done;
	u8 cur_odly;
	u8 cur_sdly;
	u8 cur_dsdly;
};

struct mmc_reg_v4p1 {
	volatile u32 gctrl;              /* (0x00) SMC Global Control Register */
	volatile u32 clkcr;              /* (0x04) SMC Clock Control Register */
	volatile u32 timeout;        /* (0x08) SMC Time Out Register */
	volatile u32 width;           /* (0x0C) SMC Bus Width Register */
	volatile u32 blksz;            /* (0x10) SMC Block Size Register */
	volatile u32 bytecnt;       /* (0x14) SMC Byte Count Register */
	volatile u32 cmd;             /* (0x18) SMC Command Register */
	volatile u32 arg;              /* (0x1C) SMC Argument Register */
	volatile u32 resp0;          /* (0x20) SMC Response Register 0 */
	volatile u32 resp1;          /* (0x24) SMC Response Register 1 */
	volatile u32 resp2;          /* (0x28) SMC Response Register 2 */
	volatile u32 resp3;          /* (0x2C) SMC Response Register 3 */
	volatile u32 imask;          /* (0x30) SMC Interrupt Mask Register */
	volatile u32 mint;            /* (0x34) SMC Masked Interrupt Status Register */
	volatile u32 rint;              /* (0x38) SMC Raw Interrupt Status Register */
	volatile u32 status;         /* (0x3C) SMC Status Register */
	volatile u32 ftrglevel;     /* (0x40) SMC FIFO Threshold Watermark Register */
	volatile u32 funcsel;       /* (0x44) SMC Function Select Register */
	volatile u32 cbcr;            /* (0x48) SMC CIU Byte Count Register */
	volatile u32 bbcr;            /* (0x4C) SMC BIU Byte Count Register */
	volatile u32 dbgc;           /* (0x50) SMC Debug Enable Register */
	volatile u32 csdc;           /* (0x54) CRC status detect control register*/
	volatile u32 a12a;          /* (0x58)Auto command 12 argument*/
	volatile u32 ntsr;            /* (0x5c)SMC2 Newtiming Set Register */
	volatile u32 res1[6];     /* (0x54~0x74) */
	volatile u32 hwrst;        /* (0x78) SMC eMMC Hardware Reset Register */
	volatile u32 res2;          /*  (0x7c) */
	volatile u32 dmac;        /*  (0x80) SMC IDMAC Control Register */
	volatile u32 dlba;          /*  (0x84) SMC IDMAC Descriptor List Base Address Register */
	volatile u32 idst;           /*  (0x88) SMC IDMAC Status Register */
	volatile u32 idie;           /*  (0x8C) SMC IDMAC Interrupt Enable Register */
	volatile u32 chda;         /*  (0x90) */
	volatile u32 cbda;         /*  (0x94) */
	volatile u32 res3[26];  /*  (0x98~0xff) */
#if defined(CONFIG_SUNXI_GEN_SUN6I) || defined(CONFIG_MACH_SUN50I_H6) || defined(CONFIG_MACH_SUN8IW16) \
	|| defined(CONFIG_MACH_SUN8IW19) || defined(CONFIG_MACH_SUN50IW9) || defined(CONFIG_MACH_SUN50IW10) \
	|| defined(CONFIG_MACH_SUN8IW15) || defined(CONFIG_MACH_SUN8IW7) || defined(CONFIG_MACH_SUN50IW11)
	volatile u32 thldc;		/*  (0x100) Card Threshold Control Register */
	volatile u32 sfc;       /* (0x104) sample fifo control register */
	volatile u32 res4[1];    /*  (0x10b) */
	volatile u32 dsbd;		/* (0x10c) eMMC4.5 DDR Start Bit Detection Control */
	volatile u32 res5[12];  /* (0x110~0x13c) */
#if (!defined(CONFIG_MACH_SUN8IW7))
	volatile u32 drv_dl;    /* (0x140) drive delay control register*/
	volatile u32 samp_dl;   /* (0x144) sample delay control register*/
	volatile u32 ds_dl;     /* (0x148) data strobe delay control register */
#else
	volatile u32 res7[3];
#endif
	volatile u32 res6[45];  /* (0x110~0x1ff) */
#endif
	volatile u32 fifo;           /* (0x100 / 0x200) SMC FIFO Access Address */
};


struct mmc_des_v4p1 {
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

#define SDXC_DES_NUM_SHIFT 12  /* smhc2!! */
#define SDXC_DES_BUFFER_MAX_LEN	(1 << SDXC_DES_NUM_SHIFT)
	u32	data_buf1_sz:16,
		data_buf2_sz:16;

	u32	buf_addr_ptr1;
	u32	buf_addr_ptr2;

};

struct sunxi_mmc_pininfo {
	user_gpio_set_t pin_set[16];
	int pin_count;
};

struct sunxi_mmc_priv {
	u32 mmc_no;

	u32 hclkbase;
	u32 hclkrst;
	u32 mclkbase;
	u32 database;

	u32 fatal_err;
	u32 clock; /* @clock, bankup current clock at host,  is updated when configure clock over */
	u32 mod_clk;
	u32 *mclkreg;
	u32 time_pwroff;
	struct gpio_desc cd_gpio;	/* Change Detect GPIO */
	u32 pwr_handler;
	int cd_inverted;		/* Inverted Card Detect */
	/*void *reg;*/struct mmc_reg_v4p1 *reg;
	void *reg_bak;//struct sunxi_mmc *reg_bak;
	struct mmc_des_v4p1 *pdes;//struct sunxi_mmc_des* pdes;

	/*sample delay and output deley setting*/
	u32 timing_mode;
	struct sunxi_mmc_timing_mode0 tm0;
	struct sunxi_mmc_timing_mode1 tm1;
	struct sunxi_mmc_timing_mode2 tm2;
	struct sunxi_mmc_timing_mode3 tm3;
	struct sunxi_mmc_timing_mode4 tm4;
	struct sunxi_mmc_pininfo pin_default;
	struct sunxi_mmc_pininfo pin_disable;
	/* @retry_cnt used to count the retry times at a spcific speed mode and frequency during initial process or
		tuning process. it is always equal or less than the number of sample point.
	*/
	u32 retry_cnt;

	struct mmc *mmc;
	struct mmc_config cfg;

	/*sample delay and output deley setting*/
	u32 raw_int_bak;
	u32 acmd_err_bak;
	u32 sample_mode;

};

struct sunxi_mmc_plat {
	struct mmc_config cfg;
	struct mmc mmc;
};

/* Struct for Intrrrupt Information */
#define SDXC_RespErr		BIT(1) //0x2
#define SDXC_CmdDone		BIT(2) //0x4
#define SDXC_DataOver		BIT(3) //0x8
#define SDXC_TxDataReq		BIT(4) //0x10
#define SDXC_RxDataReq		BIT(5) //0x20
#define SDXC_RespCRCErr		BIT(6) //0x40
#define SDXC_DataCRCErr		BIT(7) //0x80
#define SDXC_RespTimeout	BIT(8) //0x100
#define SDXC_ACKRcv		BIT(8)	//0x100
#define SDXC_DataTimeout	BIT(9)	//0x200
#define SDXC_BootStart		BIT(9)	//0x200
#define SDXC_DataStarve		BIT(10) //0x400
#define SDXC_VolChgDone		BIT(10) //0x400
#define SDXC_FIFORunErr		BIT(11) //0x800
#define SDXC_HardWLocked	BIT(12)	//0x1000
#define SDXC_StartBitErr		BIT(13)	//0x2000
#define SDXC_AutoCMDDone	BIT(14)	//0x4000
#define SDXC_EndBitErr		BIT(15)	//0x8000
#define SDXC_SDIOInt		BIT(16)	//0x10000
#define SDXC_CardInsert		BIT(30) //0x40000000
#define SDXC_CardRemove		BIT(31) //0x80000000
#define SDXC_IntErrBit		(SDXC_RespErr | SDXC_RespCRCErr | SDXC_DataCRCErr \
				| SDXC_RespTimeout | SDXC_DataTimeout | SDXC_FIFORunErr \
				| SDXC_HardWLocked | SDXC_StartBitErr | SDXC_EndBitErr)  //0xbfc2

//#define TUNING_LEN		(1)//The address which store the tuninng pattern
//#define TUNING_ADD		(38192-TUNING_LEN)//The address which store the tuninng pattern
#define TUNING_LEN		(60)//The length of the tuninng pattern
#define TUNING_ADD		(24576-4-TUNING_LEN)//The address which store the tuninng pattern
#define REPEAT_TIMES		(30)
#define SAMPLE_MODE 		(2)

//secure storage relate
#define MAX_SECURE_STORAGE_MAX_ITEM		32
#define SDMMC_SECURE_STORAGE_START_ADD		(6*1024*1024/512)//6M
#define SDMMC_ITEM_SIZE				(4*1024/512)//4K

/* IDMA status bit field */
#define SDXC_IDMACTransmitInt		BIT(0)
#define SDXC_IDMACReceiveInt		BIT(1)
#define SDXC_IDMACFatalBusErr		BIT(2)
#define SDXC_IDMACDesInvalid		BIT(4)
#define SDXC_IDMACCardErrSum		BIT(5)
#define SDXC_IDMACNormalIntSum		BIT(8)
#define SDXC_IDMACAbnormalIntSum 	BIT(9)
#define SDXC_IDMACHostAbtInTx		BIT(10)
#define SDXC_IDMACHostAbtInRx		BIT(10)
#define SDXC_IDMACIdle			(0U << 13)
#define SDXC_IDMACSuspend		(1U << 13)
#define SDXC_IDMACDESCRd		(2U << 13)
#define SDXC_IDMACDESCCheck		(3U << 13)
#define SDXC_IDMACRdReqWait		(4U << 13)
#define SDXC_IDMACWrReqWait		(5U << 13)
#define SDXC_IDMACRd			(6U << 13)
#define SDXC_IDMACWr			(7U << 13)
#define SDXC_IDMACDESCClose		(8U << 13)

/* delay control */
#define SDXC_StartCal        		(1<<15)
#define SDXC_CalDone         		(1<<14)
#define SDXC_CalDly          		(0x3F<<8)
#define SDXC_EnableDly       		(1<<7)
#define SDXC_CfgDly          		(0x3F<<0)

/* GPIO POWER MODE REGISTER */
#define GPIO_POW_MODE_REG		(0x0340)

extern void dumphex32(char *name, char *base, int len);
struct mmc *sunxi_mmc_init(int sdc_no);
int mmc_clk_io_onoff(int sdc_no, int onoff, int reset_clk);
void sunxi_mmc_pin_release(int sdc_no);

//#define SUPPORT_SUNXI_MMC_FFU

#endif /* SUNXI_MMC_H */
