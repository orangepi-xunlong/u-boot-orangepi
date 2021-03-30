#ifndef _MMC_BSP_H_
#define _MMC_BSP_H_

#include <asm/arch/mmc.h>

struct sunxi_mmc_host {
	void *reg; //struct sunxi_mmc *reg;
	u32  mmc_no;
	//u32  mclk;
	u32  hclkrst;
	u32  hclkbase;
	u32  mclkbase;
	u32  database;
	u32	 commreg;
	u32  fatal_err;
	void *pdes; //struct sunxi_mmc_des *pdes;

	/*sample delay and output deley setting*/
	u32 timing_mode;
	struct mmc *mmc;
	u32 mod_clk;
	u32 clock;

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

struct tuning_sdly{
	//u8 sdly_400k;
	u8 sdly_25M;
	u8 sdly_50M;
	u8 sdly_100M;
	u8 sdly_200M;
};//size can not over 256 now



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
	1: output and input are both based on phase
	2: output and input are both based on phase except hs400 mode.
	    output is based on phase, input is based on delay chain on hs400 mode.
	3: output is based on phase, input is based on delay chain
	4: output is based on phase, input is based on delay chain.
	    it also support to use delay chain on data strobe signal.
*/
#define SUNXI_MMC_TIMING_MODE_1 1U
#define SUNXI_MMC_TIMING_MODE_2 2U
#define SUNXI_MMC_TIMING_MODE_3 3U
#define SUNXI_MMC_TIMING_MODE_4 4U


void dumphex32(char* name, char* base, int len);
void dumpmmcreg(u32 smc_no, void *reg);

int mmc_clk_io_onoff(int sdc_no, int onoff, const normal_gpio_cfg *gpio_info, int offset);
int mmc_set_mclk(struct sunxi_mmc_host* mmchost, u32 clk_hz);
unsigned mmc_get_mclk(struct sunxi_mmc_host* mmchost);
int mmc_resource_init(int sdc_no);	


#endif /* _MMC_BSP_H_ */