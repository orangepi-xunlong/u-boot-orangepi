/*
 * (C) Copyright 2007-2012
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * Description: MMC driver for winner's mmc controller
 * Author: Aaron <leafy.myeh@allwinnertech.com>
 * Date: 2012-2-3 14:18:18
 */
#include "common.h"
#include "mmc_def.h"
#include "mmc_bsp.h"
#include "mmc.h"
#include "mmc_host_0_3_v4px.h"
#include "mmc_host_2_v5p1.h"

#include <asm/arch/gpio.h>
#include <private_boot0.h>
#include <private_toc.h>
#include <private_uboot.h>


#ifdef SUNXI_MMCDBG
//#define MMCDBG(fmt...)	printf("[mmc]: "fmt)

void dumphex32(char* name, char* base, int len)
{
	__u32 i;

	mmcmsg("dump %s registers:", name);
	for (i=0; i<len; i+=4)
	{
		if (!(i&0xf))
			mmcmsg("\n0x%x : ", base + i);
		mmcmsg("%x ", readl(base + i));
	}
	mmcmsg("\n");
}

void dumpmmcreg(u32 smc_no, void *reg)
{
	mmcmsg("gctrl     0x%x\n", reg->gctrl     );
	mmcmsg("clkcr     0x%x\n", reg->clkcr     );
	mmcmsg("timeout   0x%x\n", reg->timeout   );
	mmcmsg("width     0x%x\n", reg->width     );
	mmcmsg("blksz     0x%x\n", reg->blksz     );
	mmcmsg("bytecnt   0x%x\n", reg->bytecnt   );
	mmcmsg("cmd       0x%x\n", reg->cmd       );
	mmcmsg("arg       0x%x\n", reg->arg       );
	mmcmsg("resp0     0x%x\n", reg->resp0     );
	mmcmsg("resp1     0x%x\n", reg->resp1     );
	mmcmsg("resp2     0x%x\n", reg->resp2     );
	mmcmsg("resp3     0x%x\n", reg->resp3     );
	mmcmsg("imask     0x%x\n", reg->imask     );
	mmcmsg("mint      0x%x\n", reg->mint      );
	mmcmsg("rint      0x%x\n", reg->rint      );
	mmcmsg("status    0x%x\n", reg->status    );
	mmcmsg("ftrglevel 0x%x\n", reg->ftrglevel );
	mmcmsg("funcsel   0x%x\n", reg->funcsel   );
	mmcmsg("dmac      0x%x\n", reg->dmac      );
	mmcmsg("dlba      0x%x\n", reg->dlba      );
	mmcmsg("idst      0x%x\n", reg->idst      );
	mmcmsg("idie      0x%x\n", reg->idie      );
}
#else
//#define MMCDBG(fmt...)

//#define dumpmmcreg(fmt...)
//#define dumphex32(fmt...)
void dumphex32(char* name, char* base, int len) {};
void dumpmmcreg(u32 smc_no, void *reg) {};


#endif /* SUNXI_MMCDBG */


extern int mmc_register(int dev_num, struct mmc *mmc);
extern int mmc_unregister(int dev_num);

/* support 4 mmc hosts */
struct mmc mmc_dev[MAX_MMC_NUM];
struct sunxi_mmc_host mmc_host[MAX_MMC_NUM];

void set_mmc_para(int smc_no, void *addr)
{
	struct spare_boot_head_t  *uboot_buf = (struct spare_boot_head_t *)CONFIG_SYS_TEXT_BASE;
	u32 *p = NULL;
	int i = 0;

	memcpy((void *)(uboot_buf->boot_data.sdcard_spare_data), (addr+SDMMC_PRIV_INFO_ADDR_OFFSET), sizeof(struct boot_sdmmc_private_info_t));

	p = (u32 *)(uboot_buf->boot_data.sdcard_spare_data);
	for (i=0; i<6; i++)
		printf("0x%x 0x%x\n", p[i*2 + 0], p[i*2 + 1]);

	return;
}


int mmc_clk_io_onoff(int sdc_no, int onoff, const normal_gpio_cfg *gpio_info, int offset)
{
	unsigned int rval;
	struct sunxi_mmc_host* mmchost = &mmc_host[sdc_no];

	if(sdc_no == 0)
	{
		boot_set_gpio((void *)gpio_info, 8, 1);
	}
	else // if(sdc_no == 2)
	{
		boot_set_gpio((void *)(gpio_info + offset), 10, 1);
	}
	
	/* config ahb clock */
	rval = readl(mmchost->hclkbase);
	rval |= (1 << (8 + sdc_no));
	writel(rval, mmchost->hclkbase);

	rval = readl(mmchost->hclkrst);
	rval |= (1 << (8 + sdc_no));
	writel(rval, mmchost->hclkrst);
	/* config mod clock */
	writel(0x80000000, mmchost->mclkbase);
	mmchost->mod_clk = 24000000;
//	dumphex32("ccmu", (char*)SUNXI_CCM_BASE, 0x100);
//	dumphex32("gpio", (char*)SUNXI_PIO_BASE, 0x100);
//	dumphex32("mmc", (char*)mmchost->reg, 0x100);

	return 0;
}

static int _get_pll_periph0(void)
{
	unsigned int reg_val;
	int factor_n, factor_k, pll6;

	reg_val = readl(CCMU_PLL_PERIPH0_CTRL_REG);
	factor_n = ((reg_val >> 8) & 0x1f) + 1;
	factor_k = ((reg_val >> 4) & 0x03) + 1;
	pll6 = 24 * factor_n * factor_k/2;
	return pll6;
}

int mmc_set_mclk(struct sunxi_mmc_host* mmchost, u32 clk_hz)
{
	unsigned n, m, div, src, sclk_hz = 0;
	unsigned rval;

	mmcdbg("%s: mod_clk %d\n", __FUNCTION__, clk_hz);
	if (clk_hz <= 4000000) { //400000
		src = 0;
		sclk_hz = 24000000;
	} else {
		src = 1;
		sclk_hz = _get_pll_periph0()*2*1000000; /*use 2x pll-per0 clock */
	}

	div = (2 * sclk_hz + clk_hz) / (2 * clk_hz);
	div = (div==0) ? 1 : div;
	if (div > 128) {
		m = 1;
		n = 0;
		mmcinfo("%s: source clock is too high, clk %d, src %d!!!\n",
			__FUNCTION__, clk_hz, sclk_hz);
	} else if (div > 64) {
		n = 3;
		m = div >> 3;
	} else if (div > 32) {
		n = 2;
		m = div >> 2;
	} else if (div > 16) {
		n = 1;
		m = div >> 1;
	} else {
		n = 0;
		m = div;
	}

	//rval = (1U << 31) | (src << 24) | (n << 16) | (m - 1);
	rval = (src << 24) | (n << 16) | (m - 1);
	writel(rval, mmchost->mclkbase);
	return 0;
}

unsigned mmc_get_mclk(struct sunxi_mmc_host* mmchost)
{
	unsigned n, m, src, sclk_hz = 0;
	unsigned rval = readl(mmchost->mclkbase);

	m = rval & 0xf;
	n = (rval>>16) & 0x3;
	src = (rval>>24) & 0x3;

	if (src == 0)
		sclk_hz = 24000000;
	else if (src == 1)
		sclk_hz = _get_pll_periph0()*2*1000000; /* use 2x pll6 */
	else if (src == 2) {
		/*todo*/
	} else {
		mmcinfo("%s: wrong clock source %d\n",__func__, src);
	}

	return (sclk_hz / (1<<n) / (m+1) );
}


int mmc_resource_init(int sdc_no)
{
	struct sunxi_mmc_host* mmchost = &mmc_host[sdc_no];

	mmcdbg("init mmc %d resource\n", sdc_no);
	mmchost->reg = (struct sunxi_mmc *)(MMC_REG_BASE + sdc_no * 0x1000);
	mmchost->database = (u32)mmchost->reg + MMC_REG_FIFO_OS;

	mmchost->hclkbase = CCMU_HCLKGATE0_BASE;
	mmchost->hclkrst  = CCMU_BUS_SOFT_RST_REG0;
	if (sdc_no == 0)
		mmchost->mclkbase = CCMU_MMC0_CLK_BASE;
	else if (sdc_no == 2)
	{
		mmchost->mclkbase = CCMU_MMC2_CLK_BASE;
		mmchost->database = (u32)mmchost->reg + SMC_2_FIFO_OFFS; //added by Leson 20150721
	}
	else if (sdc_no == 3)
		mmchost->mclkbase = CCMU_MMC3_CLK_BASE;
	else {
		mmcinfo("Wrong mmc NO.: %d\n", sdc_no);
		return -1;
	}
	mmchost->mmc_no = sdc_no;

	return 0;
}

int sunxi_mmc_init(int sdc_no, unsigned bus_width, const normal_gpio_cfg *gpio_info, int offset ,void *extra_data)
{
	if ((sdc_no == 0) || (sdc_no == 3))
		return mmc_v4px_init(sdc_no, bus_width, gpio_info, offset, extra_data);
	else if (sdc_no == 2)
		return mmc_v5p1_init(sdc_no, bus_width, gpio_info, offset, extra_data);
	else {
		mmcinfo("wrong mmc no.: %d\n", sdc_no);
		return -1;
	}

	return 0;
}

int sunxi_mmc_exit(int sdc_no, const normal_gpio_cfg *gpio_info, int offset)
{
	if ((sdc_no == 0) || (sdc_no == 3))
		return mmc_v4px_exit(sdc_no,gpio_info,offset);
	else if (sdc_no == 2)
		return mmc_v5p1_exit(sdc_no,gpio_info,offset);
	else {
		mmcinfo("wrong mmc no.: %d\n", sdc_no);
		return -1;
	}

	return 0;
}
