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
#include <asm/arch/gpio.h>
#include <private_boot0.h>
#include <private_toc.h>


#ifdef MMC_DEBUG
/* #define MMCDBG(fmt...)	printf("[mmc]: "fmt) */

static void dumphex32(char *name, char *base, int len)
{
	__u32 i;

	mmcmsg("dump %s registers:", name);
	for (i = 0; i < len; i += 4) {
		if (!(i & 0xf))
			mmcmsg("\n%x : ", base + i);
		mmcmsg("%x ", readl(base + i));
	}
	mmcmsg("\n");
}

static void dumpmmcreg(struct sunxi_mmc *reg)
{
	mmcmsg("gctrl     0x%x\n", reg->gctrl);
	mmcmsg("clkcr     0x%x\n", reg->clkcr);
	mmcmsg("timeout   0x%x\n", reg->timeout);
	mmcmsg("width     0x%x\n", reg->width);
	mmcmsg("blksz     0x%x\n", reg->blksz);
	mmcmsg("bytecnt   0x%x\n", reg->bytecnt);
	mmcmsg("cmd       0x%x\n", reg->cmd);
	mmcmsg("arg       0x%x\n", reg->arg);
	mmcmsg("resp0     0x%x\n", reg->resp0);
	mmcmsg("resp1     0x%x\n", reg->resp1);
	mmcmsg("resp2     0x%x\n", reg->resp2);
	mmcmsg("resp3     0x%x\n", reg->resp3);
	mmcmsg("imask     0x%x\n", reg->imask);
	mmcmsg("mint      0x%x\n", reg->mint);
	mmcmsg("rint      0x%x\n", reg->rint);
	mmcmsg("status    0x%x\n", reg->status);
	mmcmsg("ftrglevel 0x%x\n", reg->ftrglevel);
	mmcmsg("funcsel   0x%x\n", reg->funcsel);
	mmcmsg("dmac      0x%x\n", reg->dmac);
	mmcmsg("dlba      0x%x\n", reg->dlba);
	mmcmsg("idst      0x%x\n", reg->idst);
	mmcmsg("idie      0x%x\n", reg->idie);
}
#else
/* #define MMCDBG(fmt...) */
#define dumpmmcreg(fmt...)
#define  dumphex32(fmt...)
#endif /* SUNXI_MMCDBG */

extern int mmc_register(int dev_num, struct mmc *mmc);
extern int mmc_unregister(int dev_num);

/* support 4 mmc hosts */
struct mmc mmc_dev[MAX_MMC_NUM];
struct sunxi_mmc_host mmc_host[MAX_MMC_NUM];

int sunxi_clock_get_pll6(void)
{
     unsigned int reg_val;
     int factor_n, factor_k, pll6;
     reg_val = readl(CCMU_PLL_PERIPH0_CTRL_REG);
     factor_n = ((reg_val >> 8) & 0x1f) + 1;
     factor_k = ((reg_val >> 4) & 0x03) + 1;
     pll6 = 24 * factor_n * factor_k/2;
     return pll6;
}
void set_mmc_para(int smc_no, void *sdly_addr)
{
	struct spare_boot_head_t  *uboot_buf = (struct spare_boot_head_t *)CONFIG_SYS_TEXT_BASE;
	struct tuning_sdly *p = NULL;

	memcpy((void *)uboot_buf->boot_data.sdcard_spare_data, sdly_addr, sizeof(struct tuning_sdly));
	p = (struct tuning_sdly *)(uboot_buf->boot_data.sdcard_spare_data);
	printf("%s, sdly 50M %d\n", __func__, p->sdly_50M);
	printf("%s, sdly 25M %d\n", __func__, p->sdly_25M);
	return;
}

static int mmc_resource_init(int sdc_no)
{
	struct sunxi_mmc_host *mmchost = &mmc_host[sdc_no];

	mmcdbg("init mmc %d resource\n", sdc_no);
	mmchost->reg = (struct sunxi_mmc *)(MMC_REG_BASE + sdc_no * 0x1000);
	mmchost->database = (u32)mmchost->reg + MMC_REG_FIFO_OS;

	mmchost->hclkbase = CCMU_HCLKGATE0_BASE;
	mmchost->hclkrst  = CCM_AHB1_RST_REG0;
	if (sdc_no == 0)
		mmchost->mclkbase = CCMU_MMC0_CLK_BASE;
	else if (sdc_no == 2)
		mmchost->mclkbase = CCMU_MMC2_CLK_BASE;
	else {
		mmcinfo("Wrong mmc NO.: %d\n", sdc_no);
		return -1;
	}
	mmchost->mmc_no = sdc_no;

	return 0;
}

static int mmc_clk_io_onoff(int sdc_no, int onoff, const normal_gpio_cfg *gpio_info, int offset)
{
	unsigned int rval;
	struct sunxi_mmc_host *mmchost = &mmc_host[sdc_no];

	if (sdc_no == 0) {
		boot_set_gpio((void *)gpio_info, 8, 1);
	} else {
		/* if (sdc_no == 2) */
		boot_set_gpio((void *)(gpio_info + offset), 11, 1);
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
	mmchost->mclk = 24000000;
	dumphex32("ccmu", (char *)SUNXI_CCM_BASE, 0x100);
	dumphex32("gpio", (char *)SUNXI_PIO_BASE, 0x100);
	dumphex32("mmc", (char *)mmchost->reg, 0x100);

	return 0;
}

static int mmc_update_clk(struct mmc *mmc)
{
	struct sunxi_mmc_host *mmchost = (struct sunxi_mmc_host *)mmc->priv;
	unsigned int cmd;
	int timeout = 0xfffff;

	cmd = (1U << 31) | (1 << 21) | (1 << 13);
	writel(cmd, &mmchost->reg->cmd);
	while ((readl(&mmchost->reg->cmd) & 0x80000000) && timeout--)
		;
	if (timeout < 0) {
		mmcinfo("mmc %d update clk failed\n", mmchost->mmc_no);
		dumphex32("mmc", (char *)mmchost->reg, 0x100);
		return -1;
	}

	writel(readl(&mmchost->reg->rint), &mmchost->reg->rint);
	return 0;
}


static int mmc_update_phase(struct mmc *mmc)
{
	struct sunxi_mmc_host *mmchost = (struct sunxi_mmc_host *)mmc->priv;

	if ((mmchost->mmc_no == 2) &&
	    (mmchost->host_func & MMC_HOST_2XMODE_FUNC)) {
		mmcinfo("mmc re-update_phase\n");
		return mmc_update_clk(mmc);
	}
	return 0;
}

static int mmc_config_clock(struct mmc *mmc, unsigned clk)
{
	struct sunxi_mmc_host *mmchost = (struct sunxi_mmc_host *)mmc->priv;
	unsigned rval = readl(&mmchost->reg->clkcr);
/* 	unsigned int clkdiv = 0; */

	/* Disable Clock */
	rval &= ~(1 << 16);
	writel(rval, &mmchost->reg->clkcr);
	if (mmc_update_clk(mmc)) {
		mmcinfo("mmc %d disable clock failed\n", mmchost->mmc_no);
		return -1;
	}

	/* 	clkdiv = mmchost->mclk/clk/2; */
	/* disable mclk first */
	writel(0, mmchost->mclkbase);
	mmcdbg("mmc %d mclkbase %x\n", mmchost->mmc_no, readl(mmchost->mclkbase));
	if (clk <= 400000) {
		mmchost->mclk = 400000;
		writel(0x0002000f, mmchost->mclkbase);
		mmcdbg("mmc %d mclkbase%x\n", mmchost->mmc_no,
		       readl(mmchost->mclkbase));
	} else {
		mmchost->mclk = 12000000;
		writel(0x00000001, mmchost->mclkbase);
		mmcdbg("mmc %d mclkbase%x\n", mmchost->mmc_no,
		       readl(mmchost->mclkbase));
	}
	/* re-enable mclk */
	writel(readl(mmchost->mclkbase)|(1<<31), mmchost->mclkbase);
	mmcdbg("mmc %d mclkbase%x\n", mmchost->mmc_no, readl(mmchost->mclkbase));
	/*
	 * CLKCREG[7:0]: divider
	 * CLKCREG[16]:  on/off
	 * CLKCREG[17]:  power save
	 */
	/* Change Divider Factor */
	rval &= ~(0xFF);
	writel(rval, &mmchost->reg->clkcr);
	if (mmc_update_clk(mmc)) {
		mmcinfo("mmc %d Change Divider Factor failed\n",
			mmchost->mmc_no);
		return -1;
	}
	/* Re-enable Clock */
	rval |= (3 << 16);
	writel(rval, &mmchost->reg->clkcr);
	if (mmc_update_clk(mmc)) {
		mmcinfo("mmc %d re-enable clock failed\n", mmchost->mmc_no);
		return -1;
	}
	return 0;
}



static int mmc_2xmode_config_clock(struct mmc *mmc, unsigned clk)
{
	struct sunxi_mmc_host *mmchost = (struct sunxi_mmc_host *)mmc->priv;
	unsigned rval = readl(&mmchost->reg->clkcr);
	unsigned int clkdiv = 0;
	unsigned int rntsr = readl(&mmchost->reg->ntsr);
	/* unsigned int rgctrl = readl(&mmchost->reg->gctrl); */

	/* Disable Clock */
	rval &= ~(1 << 16);
	writel(rval, &mmchost->reg->clkcr);
	if (mmc_update_clk(mmc))
		return -1;

	/* disable mclk first */
	writel(0x4000000, mmchost->mclkbase);
	mmcdbg("mmc %d mclkbase 0x%x\n", mmchost->mmc_no, readl(mmchost->mclkbase));

	/*NTSR*/
	rntsr |= (1<<31);
	mmcdbg("mmc %d rntsr 0x%x\n", mmchost->mmc_no, rntsr);
	writel(rntsr, &mmchost->reg->ntsr);


	/*set ddr mode*/
	/* if (mmc->io_mode) {
		mmcdbg("first %d rgctrl 0x%x\n", mmchost->mmc_no, rgctrl);
		rgctrl |= 1 << 10;
		writel(rgctrl, &mmchost->reg->gctrl);
		mmcdbg("after %d rgctrl 0x%x\n", mmchost->mmc_no, readl(&mmchost->reg->gctrl));
	} else{
		mmcdbg("mmc not set ddr mmc->io_mode = %x\n", mmc->io_mode);
	} */

	if (clk <= 400000) {
		mmchost->mclk = 400000;
		writel(0x4001000e, mmchost->mclkbase);
		mmcdbg("mmc %d mclkbase 0x%x\n", mmchost->mmc_no,
		       readl(mmchost->mclkbase));
	} else {
		u32 pllclk;
		u32 n, m;

		pllclk = sunxi_clock_get_pll6() * 1000000;
		/*set ddr mode clock*/
		if (mmc->io_mode) {
			clkdiv = pllclk / (clk * 4) - 1;
		} else {
			clkdiv = pllclk / (clk * 2) - 1;
		}

		if (clkdiv < 16) {
			n = 0;
			m = clkdiv;
		} else if (clkdiv < 32) {
			n = 1;
			m = clkdiv >> 1;
		} else {
			n = 2;
			m = clkdiv >> 2;
		}
		mmchost->mclk = clk;

		if (clk <= 26000000) {
			writel(0x41000000 | (n << 16) | m, mmchost->mclkbase);
		} else {
			writel(0x41000000 | (n << 16) | m, mmchost->mclkbase);
		}
		mmcdbg("init mmc %d pllclk %d, clk %d, mclkbase %x\n",
		       mmchost->mmc_no, pllclk, mmchost->mclk,
		       readl(mmchost->mclkbase));
		mmcdbg("Get round clk %d\n", pllclk / (1 << n) / (m + 1) / 2);
		if (mmc->io_mode)
			mmc->clock = pllclk / (1 << n) / (m + 1) / 2 / 2;
		else
			mmc->clock = pllclk / (1 << n) / (m + 1) / 2;
	}
	/* re-enable mclk */
	writel(readl(mmchost->mclkbase)|(1<<31), mmchost->mclkbase);
	mmcdbg("mmc %d mclkbase 0x%x\n", mmchost->mmc_no, readl(mmchost->mclkbase));
	/*
	 * CLKCREG[7:0]: divider
	 * CLKCREG[16]:  on/off
	 * CLKCREG[17]:  power save
	 */
	/* Change Divider Factor */
	rval &= ~(0xFF);
	if (mmc->io_mode)
		rval |= 0x1;
	writel(rval, &mmchost->reg->clkcr);
	if (mmc_update_clk(mmc)) {
		mmcdbg("mmc %d disable clock failed\n", mmchost->mmc_no);
		return -1;
	}
	/* Re-enable Clock */
	rval |= (3 << 16);
	writel(rval, &mmchost->reg->clkcr);
	if (mmc_update_clk(mmc)) {
		mmcinfo("mmc %d re-enable clock failed\n", mmchost->mmc_no);
		return -1;
	}

	dumphex32("ccmu", (char *)SUNXI_CCM_BASE, 0x100);
	dumphex32("gpio", (char *)SUNXI_PIO_BASE, 0x100);
	dumphex32("mmc", (char *)mmchost->reg, 0x100);

	mmcdbg("mmc %d ntsr 0x%x\n", mmchost->mmc_no, readl(&mmchost->reg->ntsr));
	return 0;

}





static void mmc_set_ios(struct mmc *mmc)
{
	struct sunxi_mmc_host *mmchost = (struct sunxi_mmc_host *)mmc->priv;


	mmcdbg("mmc %d ios: bus: %d, clock: %d\n", mmchost->mmc_no, mmc->bus_width, mmc->clock);

	if (mmchost->host_func & MMC_HOST_2XMODE_FUNC) {
		if (mmc->clock && mmc_2xmode_config_clock(mmc, mmc->clock)) {
			printf("[mmc]: "
			       "*** update clock failed\n");
			mmchost->fatal_err = 1;
			return;
		}

	} else {
		/* to compatible with previous boot0, use 12m when in old mode
		 */
		if (mmc->clock && mmc_config_clock(mmc, mmc->clock)) {
			mmcinfo("[mmc]: "
				"*** update clock failed\n");
			mmchost->fatal_err = 1;
			return;
		}
	}
	/* Change bus width */
	if (mmc->bus_width == 8)
		writel(2, &mmchost->reg->width);
	else if (mmc->bus_width == 4)
		writel(1, &mmchost->reg->width);
	else
		writel(0, &mmchost->reg->width);

	if (mmc->io_mode) {
		uint tmp = readl(&mmchost->reg->gctrl);
		tmp |= (1 << 10);
		writel(tmp, &mmchost->reg->gctrl);
		mmcdbg("enable ddr %x\n", readl(&mmchost->reg->gctrl));
	}
}

static int mmc_core_init(struct mmc *mmc)
{
	struct sunxi_mmc_host *mmchost = (struct sunxi_mmc_host *)mmc->priv;
	/* Reset controller */
	writel(0x7, &mmchost->reg->gctrl);
	while (readl(&mmchost->reg->gctrl) & 0x7)
		;
	/* release eMMC reset signal */
	writel(1, &mmchost->reg->hwrst);
	writel(0, &mmchost->reg->hwrst);
	__msdelay(1);
	writel(1, &mmchost->reg->hwrst);
	return 0;
}
static int mmc_trans_data_by_cpu(struct mmc *mmc, struct mmc_data *data)
{
	struct sunxi_mmc_host *mmchost = (struct sunxi_mmc_host *)mmc->priv;
	unsigned i;
	unsigned byte_cnt = data->blocksize * data->blocks;
	unsigned *buff;
	unsigned timeout = 0xffffff;

	if (data->flags & MMC_DATA_READ) {
		buff = (unsigned int *)data->b.dest;
		for (i = 0; i < (byte_cnt >> 2); i++) {
			while (--timeout &&
			       (readl(&mmchost->reg->status) & (1 << 2)))
				;
			if (timeout <= 0)
				goto out;
			buff[i] = readl(mmchost->database);
			timeout = 0xffffff;
		}
	} else {
		buff = (unsigned int *)data->b.src;
		for (i = 0; i < (byte_cnt >> 2); i++) {
			while (--timeout &&
			       (readl(&mmchost->reg->status) & (1 << 3)))
				;
			if (timeout <= 0)
				goto out;
			writel(buff[i], mmchost->database);
			timeout = 0xffffff;
		}
	}

out:
	if (timeout <= 0) {
		mmcinfo("mmc %d transfer by cpu failed\n", mmchost->mmc_no);
		return -1;
	}

	return 0;
}

static int mmc_trans_data_by_dma(struct mmc *mmc, struct mmc_data *data)
{
	struct sunxi_mmc_host *mmchost = (struct sunxi_mmc_host *)mmc->priv;
	struct sunxi_mmc_des *pdes = NULL;
	unsigned byte_cnt = data->blocksize * data->blocks;
	unsigned char *buff;
	unsigned des_idx = 0;
	unsigned buff_frag_num = 0;
	unsigned remain;
	unsigned i, rval;

	buff = data->flags & MMC_DATA_READ ?
			(unsigned char *)data->b.dest : (unsigned char *)data->b.src;
	buff_frag_num = byte_cnt >> SDXC_DES_NUM_SHIFT;
	remain = byte_cnt & (SDXC_DES_BUFFER_MAX_LEN-1);
	if (remain)
		buff_frag_num++;
	else
		remain = SDXC_DES_BUFFER_MAX_LEN;

	pdes = mmchost->pdes;

	/* OSAL_CacheRangeFlush(buff, (unsigned long)byte_cnt, CACHE_CLEAN_FLUSH_D_CACHE_REGION); */
	for (i = 0; i < buff_frag_num; i++, des_idx++) {
		memset((void *)&pdes[des_idx], 0, sizeof(struct sunxi_mmc_des));
		pdes[des_idx].des_chain = 1;
		pdes[des_idx].own = 1;
		pdes[des_idx].dic = 1;
		if (buff_frag_num > 1 && i != buff_frag_num - 1) {
			pdes[des_idx].data_buf1_sz = SDXC_DES_BUFFER_MAX_LEN;
		} else
			pdes[des_idx].data_buf1_sz = remain;

		pdes[des_idx].buf_addr_ptr1 =
		    (u32)buff + i * SDXC_DES_BUFFER_MAX_LEN;
		if (i == 0)
			pdes[des_idx].first_des = 1;

		if (i == buff_frag_num - 1) {
			pdes[des_idx].dic = 0;
			pdes[des_idx].last_des = 1;
			pdes[des_idx].end_of_ring = 1;
			pdes[des_idx].buf_addr_ptr2 = 0;
		} else {
			pdes[des_idx].buf_addr_ptr2 = (u32)&pdes[des_idx + 1];
		}
		mmcdbg("mmc %d frag %d, remain %d, des[%d](%x): "
		       "[0] = %x, [1] = %x, [2] = %x, [3] = %x\n",
		       mmchost->mmc_no, i, remain, des_idx, (u32)&pdes[des_idx],
		       (u32)((u32 *)&pdes[des_idx])[0],
		       (u32)((u32 *)&pdes[des_idx])[1],
		       (u32)((u32 *)&pdes[des_idx])[2],
		       (u32)((u32 *)&pdes[des_idx])[3]);
	}
	/* OSAL_CacheRangeFlush(pdes, sizeof(struct sunxi_mmc_des) * (des_idx+1), CACHE_CLEAN_FLUSH_D_CACHE_REGION); */
	__asm("DSB");
	__asm("ISB");

	/*
	 * GCTRLREG
	 * GCTRL[2]	: DMA reset
	 * GCTRL[5]	: DMA enable
	 *
	 * IDMACREG
	 * IDMAC[0]	: IDMA soft reset
	 * IDMAC[1]	: IDMA fix burst flag
	 * IDMAC[7]	: IDMA on
	 *
	 * IDIECREG
	 * IDIE[0]	: IDMA transmit interrupt flag
	 * IDIE[1]	: IDMA receive interrupt flag
	 */
	rval = readl(&mmchost->reg->gctrl);
	writel(rval|(1 << 5)|(1 << 2), &mmchost->reg->gctrl);	/* dma enable */
	writel((1 << 0), &mmchost->reg->dmac); /* idma reset */
	writel((1 << 1) | (1 << 7), &mmchost->reg->dmac); /* idma on */
	rval = readl(&mmchost->reg->idie) & (~3);
	if (data->flags & MMC_DATA_WRITE)
		rval |= (1 << 0);
	else
		rval |= (1 << 1);
	writel(rval, &mmchost->reg->idie);
	writel((u32)pdes, &mmchost->reg->dlba);
	writel((2U<<28)|(7<<16)|8, &mmchost->reg->ftrglevel);

	return 0;
}

static int mmc_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd,
			struct mmc_data *data)
{
	struct sunxi_mmc_host *mmchost = (struct sunxi_mmc_host *)mmc->priv;
	unsigned int cmdval = 0x80000000;
	unsigned int timeout = 0;
	int error = 0;
	unsigned int status = 0;
	unsigned int usedma = 0;
	unsigned int bytecnt = 0;

	if (mmchost->fatal_err) {
		mmcinfo("mmc %d Found fatal err, so no send cmd\n",
			mmchost->mmc_no);
		return -1;
	}
	if (cmd->resp_type & MMC_RSP_BUSY)
		mmcdbg("mmc %d cmd %d check rsp busy\n", mmchost->mmc_no,
		       cmd->cmdidx);
	if (cmd->cmdidx == 12)
		return 0;
	/*
	 * CMDREG
	 * CMD[5:0]	: Command index
	 * CMD[6]	: Has response
	 * CMD[7]	: Long response
	 * CMD[8]	: Check response CRC
	 * CMD[9]	: Has data
	 * CMD[10]	: Write
	 * CMD[11]	: Steam mode
	 * CMD[12]	: Auto stop
	 * CMD[13]	: Wait previous over
	 * CMD[14]	: About cmd
	 * CMD[15]	: Send initialization
	 * CMD[21]	: Update clock
	 * CMD[31]	: Load cmd
	 */
	if (!cmd->cmdidx)
		cmdval |= (1 << 15);
	if (cmd->resp_type & MMC_RSP_PRESENT)
		cmdval |= (1 << 6);
	if (cmd->resp_type & MMC_RSP_136)
		cmdval |= (1 << 7);
	if (cmd->resp_type & MMC_RSP_CRC)
		cmdval |= (1 << 8);
	if (data) {
		if ((u32)data->b.dest & 0x3) {
			mmcinfo("mmc %d dest is not 4 byte align\n", mmchost->mmc_no);
			error = -1;
			goto out;
		}

		cmdval |= (1 << 9) | (1 << 13);
		if (data->flags & MMC_DATA_WRITE)
			cmdval |= (1 << 10);
		if (data->blocks > 1)
			cmdval |= (1 << 12);
		writel(data->blocksize, &mmchost->reg->blksz);
		writel(data->blocks * data->blocksize, &mmchost->reg->bytecnt);
	}

	mmcdbg("mmc %d, cmd %d(0x%x), arg 0x%x\n", mmchost->mmc_no, cmd->cmdidx, cmdval|cmd->cmdidx, cmd->cmdarg);
	writel(cmd->cmdarg, &mmchost->reg->arg);
	if (!data)
		writel(cmdval|cmd->cmdidx, &mmchost->reg->cmd);

	/*
	 * transfer data and check status
	 * STATREG[2] : FIFO empty
	 * STATREG[3] : FIFO full
	 */
	if (data) {
		int ret = 0;
		bytecnt = data->blocksize * data->blocks;
		mmcdbg("mmc %d trans data %d bytes\n", mmchost->mmc_no, bytecnt);
#ifdef MMC_TRANS_BY_DMA
		if (bytecnt > 512) {
#else
		if (0) {
#endif
			usedma = 1;
			writel(readl(&mmchost->reg->gctrl)&(~0x80000000), &mmchost->reg->gctrl);
			ret = mmc_trans_data_by_dma(mmc, data);
			writel(cmdval|cmd->cmdidx, &mmchost->reg->cmd);
		} else {
			writel(readl(&mmchost->reg->gctrl)|0x80000000, &mmchost->reg->gctrl);
			writel(cmdval|cmd->cmdidx, &mmchost->reg->cmd);
			ret = mmc_trans_data_by_cpu(mmc, data);
		}
		if (ret) {
			mmcinfo("mmc %d Transfer failed\n", mmchost->mmc_no);
			error = readl(&mmchost->reg->rint) & 0xbbc2;
			if (!error)
				error = 0xffffffff;
			goto out;
		}
	}

	timeout = 0xffffff;
	do {
		status = readl(&mmchost->reg->rint);
		if (!timeout-- || (status & 0xbbc2)) {
			error = status & 0xbbc2;
			if (!error)
				error = 0xffffffff; /* represet software timeout */
			mmcinfo("mmc %d cmd %d err %x\n", mmchost->mmc_no,
				cmd->cmdidx, error);
			goto out;
		}
	} while (!(status&0x4));

	if (data) {
		unsigned done = 0;
		timeout = usedma ? 0xffff*bytecnt : 0xffff;

		do {
			status = readl(&mmchost->reg->rint);
			if (!timeout-- || (status & 0xbbc2)) {
				error = status & 0xbbc2;
				if (!error)
					error = 0xffffffff;/* represet software timeout */
				mmcinfo("mmc %d data err %x\n", mmchost->mmc_no, error);
				goto out;
			}
			if (data->blocks > 1)
				done = status & (1 << 14);
			else
				done = status & (1 << 3);
		} while (!done);

		if ((data->flags & MMC_DATA_READ) && usedma) {
			timeout = 0xffffff;
			done = 0;
			status = 0;
			mmcdbg("mmc %d cacl rd dma timeout %x\n", mmchost->mmc_no, timeout);
			do {
					status = readl(&mmchost->reg->idst);
					if (!timeout-- || (status & 0x234)) {
						error = status & 0x1E34;
						if (!error)
							error = 0xffffffff; /* represet software timeout */
						mmcinfo("mmc %d wait dma over err %x\n", mmchost->mmc_no, error);
						goto out;
					}
					done = status & (1 << 1);
					/* usdelay(1); */
			} while (!done);
			mmcdbg("idst *****0x%d******\n", readl(&mmchost->reg->idst));
		}
	}

	if (cmd->resp_type & MMC_RSP_BUSY) {
		timeout = 0x4ffffff;
		do {
			status = readl(&mmchost->reg->status);
			if (!timeout--) {
				error = -1;
				mmcinfo("mmc %d busy timeout\n", mmchost->mmc_no);
				goto out;
			}
		} while (status & (1 << 9));
	}
	if (cmd->resp_type & MMC_RSP_136) {
		cmd->response[0] = readl(&mmchost->reg->resp3);
		cmd->response[1] = readl(&mmchost->reg->resp2);
		cmd->response[2] = readl(&mmchost->reg->resp1);
		cmd->response[3] = readl(&mmchost->reg->resp0);
		mmcdbg("mmc %d resp 0x%x 0x%x 0x%x 0x%x\n", mmchost->mmc_no,
			cmd->response[3], cmd->response[2],
			cmd->response[1], cmd->response[0]);
	} else {
		cmd->response[0] = readl(&mmchost->reg->resp0);
		mmcdbg("mmc %d resp 0x%x\n", mmchost->mmc_no, cmd->response[0]);
	}
out:
	if (data && usedma) {
	/* IDMASTAREG
	 * IDST[0] : idma tx int
	 * IDST[1] : idma rx int
	 * IDST[2] : idma fatal bus error
	 * IDST[4] : idma descriptor invalid
	 * IDST[5] : idma error summary
	 * IDST[8] : idma normal interrupt sumary
	 * IDST[9] : idma abnormal interrupt sumary
	 */
	status = readl(&mmchost->reg->idst);
	writel(status, &mmchost->reg->idst);
	writel(0, &mmchost->reg->idie);
	writel(0, &mmchost->reg->dmac);
	writel(readl(&mmchost->reg->gctrl)&(~(1 << 5)), &mmchost->reg->gctrl);
    /* OSAL_CacheRangeFlush(data->b.dest, data->blocksize * data->blocks, 4); */
	}
	if (error) {
		dumphex32("mmc", (char *)mmchost->reg, 0x100);
		writel(0x7, &mmchost->reg->gctrl);
		while (readl(&mmchost->reg->gctrl) & 0x7)
			;
		mmc_update_clk(mmc);
		mmcinfo("mmc %d cmd %d err %x\n", mmchost->mmc_no, cmd->cmdidx, error);
	}
	writel(0xffffffff, &mmchost->reg->rint);

	if (error)
		return -1;
	else
		return 0;
}

int sunxi_mmc_init(int sdc_no, unsigned bus_width, const normal_gpio_cfg *gpio_info, int offset , void *extra_data)
{
	struct mmc *mmc;
	int ret;
	boot_sdcard_info_t  *sdcard_info = (boot_sdcard_info_t *)extra_data;

	mmcinfo("mmc driver ver %s\n", DRIVER_VER);

	memset(&mmc_dev[sdc_no], 0, sizeof(struct mmc));
	memset(&mmc_host[sdc_no], 0, sizeof(struct sunxi_mmc_host));
	mmc = &mmc_dev[sdc_no];

	strcpy(mmc->name, "SUNXI SD/MMC");
	mmc->priv = &mmc_host[sdc_no];
	mmc->send_cmd = mmc_send_cmd;
	mmc->set_ios = mmc_set_ios;
	mmc->init = mmc_core_init;
	mmc->update_phase = mmc_update_phase;

	mmc->voltages = MMC_VDD_29_30|MMC_VDD_30_31|MMC_VDD_31_32|MMC_VDD_32_33|
					MMC_VDD_33_34|MMC_VDD_34_35|MMC_VDD_35_36;
	mmc->host_caps = MMC_MODE_HS_52MHz|MMC_MODE_HS|MMC_MODE_HC;

	if ((sdcard_info->ddrmode[sdc_no]) && (sdc_no == 2) &&
	    (sdcard_info->sdc_2xmode[sdc_no])) {
		mmc->host_caps |= MMC_MODE_DDR_52MHz;
	}

	if (bus_width == 4)
		mmc->host_caps |= MMC_MODE_4BIT;

	if ((sdc_no == 2) && (sdcard_info->sdc_2xmode[sdc_no])) {
		if (bus_width == 8)
			mmc->host_caps |= MMC_MODE_8BIT;
	}

	mmc->f_min = 400000;
	if ((sdcard_info->sdc_2xmode[sdc_no]) && (sdc_no == 2)) {
		mmc_host[sdc_no].host_func |= MMC_HOST_2XMODE_FUNC;
		if ((sdcard_info->sdc_f_max[sdc_no]) &&
		    (sdcard_info->sdc_f_max[sdc_no] <= 50000000)) {
			mmc->f_max = sdcard_info->sdc_f_max[sdc_no];
		} else {
			mmc->f_max = 50000000;
		}
	} else {
		mmc->f_max = 25000000;
	}
	mmc->control_num = sdc_no;

	mmc_host[sdc_no].pdes = (struct sunxi_mmc_des *)DMAC_DES_BASE_IN_SDRAM;
	if (mmc_resource_init(sdc_no)) {
		mmcinfo("mmc %d resource init failed\n", sdc_no);
		return -1;
	}

	mmc_clk_io_onoff(sdc_no, 1, gpio_info, offset);
	ret = mmc_register(sdc_no, mmc);
	if (ret < 0) {
		mmcinfo("mmc %d register failed\n", sdc_no);
		return -1;
	}

	return mmc->lba;
}

int sunxi_mmc_exit(int sdc_no, const normal_gpio_cfg *gpio_info, int offset)
{
	mmc_clk_io_onoff(sdc_no, 0, gpio_info, offset);
	mmc_unregister(sdc_no);
	memset(&mmc_dev[sdc_no], 0, sizeof(struct mmc));
	memset(&mmc_host[sdc_no], 0, sizeof(struct sunxi_mmc_host));

	mmcdbg("sunxi mmc%d exit\n", sdc_no);
	return 0;
}
