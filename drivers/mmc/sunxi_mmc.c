// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Aaron <leafy.myeh@allwinnertech.com>
 *
 * MMC driver for allwinner sunxi platform.
 */

#include <common.h>
#include <dm.h>
#include <errno.h>
#include <malloc.h>
#include <mmc.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/timer.h>
#include <asm/arch/cpu.h>
#include <asm/arch/gpio.h>
#include <private_uboot.h>
#include "sunxi_mmc.h"
#include "host/sunxi_mmc_host_common.h"
#include "mmc_def.h"

#ifndef readl
#define  readl(a)   *(volatile uint *)(ulong)(a)
#endif
#ifndef writel
#define  writel(v, c) *(volatile uint *)(ulong)(c) = (v)
#endif

#define DTO_MAX 200

#if !CONFIG_IS_ENABLED(DM_MMC)
/* support 4 mmc hosts */
struct sunxi_mmc_priv mmc_host[4];
struct mmc_reg_v4p1 mmc_host_reg_bak[3];

//extern u8 ext_odly_spd_freq[];
//extern u8 ext_sdly_spd_freq[];
//extern u8 ext_odly_spd_freq_sdc0[];
//extern u8 ext_sdly_spd_freq_sdc0[];
static u8 ext_odly_spd_freq[MAX_SPD_MD_NUM*MAX_CLK_FREQ_NUM];
static u8 ext_sdly_spd_freq[MAX_SPD_MD_NUM*MAX_CLK_FREQ_NUM];
static u8 ext_odly_spd_freq_sdc0[MAX_SPD_MD_NUM*MAX_CLK_FREQ_NUM];
static u8 ext_sdly_spd_freq_sdc0[MAX_SPD_MD_NUM*MAX_CLK_FREQ_NUM];

void dumphex32(char *name, char *base, int len)
{
	__u32 i;

	printf("dump %s registers:", name);
	for (i = 0; i < len; i += 4) {
		if (!(i & 0xf))
			printf("\n0x%p : ", base + i);
		printf("0x%08x ", readl((ulong)base + i));
	}
	printf("\n");
}

void mmc_dump_errinfo(struct sunxi_mmc_priv *smc_priv, struct mmc_cmd *cmd)
{
	MMCMSG(smc_priv->mmc, "smc %d err, cmd %d, %s%s%s%s%s%s%s%s%s%s%s\n",
		smc_priv->mmc_no, cmd ? cmd->cmdidx : -1,
		smc_priv->raw_int_bak & SDXC_RespErr     ? " RE"     : "",
		smc_priv->raw_int_bak & SDXC_RespCRCErr  ? " RCE"    : "",
		smc_priv->raw_int_bak & SDXC_DataCRCErr  ? " DCE"    : "",
		smc_priv->raw_int_bak & SDXC_RespTimeout ? " RTO"    : "",
		smc_priv->raw_int_bak & SDXC_DataTimeout ? " DTO"    : "",
		smc_priv->raw_int_bak & SDXC_DataStarve  ? " DS"     : "",
		smc_priv->raw_int_bak & SDXC_FIFORunErr  ? " FE"     : "",
		smc_priv->raw_int_bak & SDXC_HardWLocked ? " HL"     : "",
		smc_priv->raw_int_bak & SDXC_StartBitErr ? " SBE"    : "",
		smc_priv->raw_int_bak & SDXC_EndBitErr   ? " EBE"    : "",
		smc_priv->raw_int_bak == 0 ? " STO"    : ""
		);
}

static int sunxi_mmc_getcd_gpio(int sdc_no)
{
/*
	switch (sdc_no) {
	case 0: return sunxi_name_to_gpio(CONFIG_MMC0_CD_PIN);
	case 1: return sunxi_name_to_gpio(CONFIG_MMC1_CD_PIN);
	case 2: return sunxi_name_to_gpio(CONFIG_MMC2_CD_PIN);
	case 3: return sunxi_name_to_gpio(CONFIG_MMC3_CD_PIN);
	}
*/
	return -EINVAL;
}

static int mmc_resource_init(int sdc_no)
{
	struct sunxi_mmc_priv *priv = &mmc_host[sdc_no];
	struct sunxi_ccm_reg *ccm = (struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	int cd_pin, ret = 0;

	MMCDBG("init mmc %d resource\n", sdc_no);

	switch (sdc_no) {
	case 0:
		priv->reg = (struct mmc_reg_v4p1 *)SUNXI_MMC0_BASE;
		priv->mclkreg = &ccm->sd0_clk_cfg;
		break;
	case 1:
		priv->reg = (struct mmc_reg_v4p1 *)SUNXI_MMC1_BASE;
		priv->mclkreg = &ccm->sd1_clk_cfg;
		break;
	case 2:
		priv->reg = (struct mmc_reg_v4p1 *)SUNXI_MMC2_BASE;
		priv->mclkreg = &ccm->sd2_clk_cfg;
		break;
#ifdef SUNXI_MMC3_BASE
	case 3:
		priv->reg = (struct mmc_reg_v4p1 *)SUNXI_MMC3_BASE;
		priv->mclkreg = &ccm->sd3_clk_cfg;
		break;
#endif
	default:
		MMCINFO("Wrong mmc number %d\n", sdc_no);
		return -1;
	}

#if !defined(CONFIG_MACH_SUN8IW5) && !defined(CONFIG_MACH_SUN8IW6) && !defined(CONFIG_MACH_SUN8IW7) && !defined(CONFIG_MACH_SUN8IW8)\
	&& !defined(CONFIG_MACH_SUN8IW10) && !defined(CONFIG_MACH_SUN8IW11)
	priv->hclkbase = IOMEM_ADDR(&ccm->sd_gate_reset);
	priv->hclkrst = IOMEM_ADDR(&ccm->sd_gate_reset);
#else
	priv->hclkbase = IOMEM_ADDR(&ccm->ahb_gate0);
	priv->hclkrst = IOMEM_ADDR(&ccm->ahb_reset0_cfg);
#endif
	priv->mmc_no = sdc_no;

	cd_pin = sunxi_mmc_getcd_gpio(sdc_no);
	if (cd_pin >= 0) {
		ret = gpio_request(cd_pin, "mmc_cd");
		if (!ret) {
			sunxi_gpio_set_pull(cd_pin, SUNXI_GPIO_PULL_UP);
			ret = gpio_direction_input(cd_pin);
		}
	}

	return ret;
}
#endif

static int sunxi_mmc_pin_set(int sdc_no)
{
	struct sunxi_mmc_priv *priv = &mmc_host[sdc_no];
	struct sunxi_mmc_pininfo *pin_default = &priv->pin_default;
	struct sunxi_mmc_pininfo *pin_disable = &priv->pin_disable;
	int ret = -1;

	if (priv->pwr_handler != 0 && pin_disable->pin_count > 0) {
		ret =  gpio_request_early(pin_disable->pin_set, pin_disable->pin_count, 1);
		gpio_write_one_pin_value(priv->pwr_handler, 1, "card-pwr-gpios");
		mdelay(priv->time_pwroff);
		gpio_write_one_pin_value(priv->pwr_handler, 0, "card-pwr-gpios");
		/*delay to ensure voltage stability*/
		mdelay(1);
	}

	if (pin_default->pin_count > 0) {
		ret =  gpio_request_early(pin_default->pin_set, pin_default->pin_count, 1);
	}
	return ret;
}

void sunxi_mmc_pin_release(int sdc_no)
{
	struct sunxi_mmc_priv *priv = &mmc_host[sdc_no];

	if (priv->pwr_handler != 0) {
		gpio_release(priv->pwr_handler, 0);
	}
}

int mmc_clk_io_onoff(int sdc_no, int onoff, int reset_clk)
{
	struct sunxi_mmc_priv *priv = &mmc_host[sdc_no];

	priv->sunxi_mmc_clk_io_onoff(sdc_no, onoff, reset_clk);

	return 0;
}

static int mmc_update_clk(struct sunxi_mmc_priv *priv)
{
	unsigned int cmd;
	unsigned timeout_msecs = 10000;
	unsigned long start = get_timer(0);

	writel(readl(&priv->reg->clkcr)|(0x1 << 31), &priv->reg->clkcr);
	cmd = SUNXI_MMC_CMD_START |
	      SUNXI_MMC_CMD_UPCLK_ONLY |
	      SUNXI_MMC_CMD_WAIT_PRE_OVER;

	writel(cmd, &priv->reg->cmd);
	while (readl(&priv->reg->cmd) & SUNXI_MMC_CMD_START) {
		if (get_timer(start) > timeout_msecs) {
				dumphex32("mmc", (char *)priv->reg, 0x200);
				return -1;
			}
	}
	writel(readl(&priv->reg->clkcr) & (~(0x1 << 31)), &priv->reg->clkcr);

	/* clock update sets various irq status bits, clear these */
	writel(readl(&priv->reg->rint), &priv->reg->rint);

	return 0;
}


static int mmc_set_mod_clk(struct sunxi_mmc_priv *priv, unsigned int hz)
{
	int rval;

	rval = priv->mmc_set_mod_clk(priv, hz);
	return rval;
}

static int mmc_config_clock(struct sunxi_mmc_priv *priv, struct mmc *mmc)
{
	unsigned rval = readl(&priv->reg->clkcr);

	/* Disable Clock */
	rval &= ~SUNXI_MMC_CLK_ENABLE;
	writel(rval, &priv->reg->clkcr);
	if (mmc_update_clk(priv)) {
		MMCINFO("Disable clock: mmc update clk failed\n");
		return -1;
	}

	/* Set mod_clk to new rate */
	if (mmc_set_mod_clk(priv, mmc->clock))
		return -1;
#if 0
	/* Clear internal divider */
	rval &= ~SUNXI_MMC_CLK_DIVIDER_MASK;
	writel(rval, &priv->reg->clkcr);
#endif
	/* Re-enable Clock */
	rval = readl(&priv->reg->clkcr);
	rval |= SUNXI_MMC_CLK_ENABLE;
	writel(rval, &priv->reg->clkcr);
	if (mmc_update_clk(priv)) {
		MMCINFO("Re-enable clock: mmc update clk failed\n");
		return -1;
	}

	return 0;
}

static int sunxi_mmc_set_ios_common(struct sunxi_mmc_priv *priv,
				    struct mmc *mmc)
{
	MMCDBG("set ios: bus_width: %x, clock: %d\n",
	      mmc->bus_width, mmc->clock);

	/* Change clock first */
	if (mmc->clock && mmc_config_clock(priv, mmc) != 0) {
		priv->fatal_err = 1;
		return -EINVAL;
	}

	/* Change bus width */
	if (mmc->bus_width == 8)
		writel(0x2, &priv->reg->width);
	else if (mmc->bus_width == 4)
		writel(0x1, &priv->reg->width);
	else
		writel(0x0, &priv->reg->width);

	/* set speed mode */
	priv->sunxi_mmc_set_speed_mode(priv, mmc);

	return 0;
}

#if !CONFIG_IS_ENABLED(DM_MMC)
static int sunxi_mmc_core_init(struct mmc *mmc)
{
	struct sunxi_mmc_priv *priv = mmc->priv;

	priv->sunxi_mmc_core_init(mmc);

	return 0;
}
#endif

static int mmc_save_regs(struct sunxi_mmc_priv *mmchost)
{
	struct mmc_reg_v4p1 *reg = (struct mmc_reg_v4p1 *)mmchost->reg;
	struct mmc_reg_v4p1 *reg_bak = (struct mmc_reg_v4p1 *)mmchost->reg_bak;

	reg_bak->gctrl     = readl(&reg->gctrl);
	reg_bak->clkcr     = readl(&reg->clkcr);
	reg_bak->timeout   = readl(&reg->timeout);
	reg_bak->width     = readl(&reg->width);
	reg_bak->imask     = readl(&reg->imask);
	reg_bak->ftrglevel = readl(&reg->ftrglevel);
	reg_bak->dbgc      = readl(&reg->dbgc);
	reg_bak->ntsr      = readl(&reg->ntsr);
	reg_bak->hwrst     = readl(&reg->hwrst);
	reg_bak->dmac      = readl(&reg->dmac);
	reg_bak->idie      = readl(&reg->idie);
	reg_bak->thldc     = readl(&reg->thldc);
	reg_bak->dsbd      = readl(&reg->dsbd);
#if (!defined(CONFIG_MACH_SUN8IW7))
	reg_bak->csdc      = readl(&reg->csdc);
	reg_bak->drv_dl    = readl(&reg->drv_dl);
	reg_bak->samp_dl   = readl(&reg->samp_dl);
	reg_bak->ds_dl     = readl(&reg->ds_dl);
#endif

	return 0;
}

static int mmc_restore_regs(struct sunxi_mmc_priv *mmchost)
{	struct mmc_reg_v4p1 *reg = (struct mmc_reg_v4p1 *)mmchost->reg;
	struct mmc_reg_v4p1 *reg_bak = (struct mmc_reg_v4p1 *)mmchost->reg_bak;

	writel(reg_bak->gctrl, &reg->gctrl);
	writel(reg_bak->clkcr, &reg->clkcr);
	writel(reg_bak->timeout, &reg->timeout);
	writel(reg_bak->width, &reg->width);
	writel(reg_bak->imask, &reg->imask);
	writel(reg_bak->ftrglevel, &reg->ftrglevel);
	if (reg_bak->dbgc)
		writel(0xdeb, &reg->dbgc);
	writel(reg_bak->ntsr, &reg->ntsr);
	writel(reg_bak->hwrst, &reg->hwrst);
	writel(reg_bak->dmac, &reg->dmac);
	writel(reg_bak->idie, &reg->idie);
	writel(reg_bak->thldc, &reg->thldc);
	writel(reg_bak->dsbd, &reg->dsbd);
#if (!defined(CONFIG_MACH_SUN8IW7))
	writel(reg_bak->csdc, &reg->csdc);
	sunxi_r_op(mmchost, writel(reg_bak->drv_dl, &reg->drv_dl));
	writel(reg_bak->samp_dl, &reg->samp_dl);
	writel(reg_bak->ds_dl, &reg->ds_dl);
#endif

	return 0;
}

static int mmc_trans_data_by_cpu(struct sunxi_mmc_priv *priv, struct mmc *mmc,
				 struct mmc_data *data)
{
	const int reading = !!(data->flags & MMC_DATA_READ);
	const uint32_t status_bit = reading ? SUNXI_MMC_STATUS_FIFO_EMPTY :
					      SUNXI_MMC_STATUS_FIFO_FULL;
	unsigned i;
	unsigned *buff = (unsigned int *)(reading ? data->dest : data->src);
	unsigned byte_cnt = data->blocksize * data->blocks;
	unsigned timeout_msecs = byte_cnt;
	unsigned long  start;

	if (timeout_msecs < 2000)
		timeout_msecs = 2000;

	/* Always read / write data through the CPU */
	setbits_le32(&priv->reg->gctrl, SUNXI_MMC_GCTRL_ACCESS_BY_AHB);

	start = get_timer(0);

	for (i = 0; i < (byte_cnt >> 2); i++) {
		while (readl(&priv->reg->status) & status_bit) {
			if (get_timer(start) > timeout_msecs)
				return -1;
		}

		if (reading)
			buff[i] = readl(&priv->reg->fifo);
		else
			writel(buff[i], &priv->reg->fifo);
	}

	return 0;
}

static int mmc_trans_data_by_dma(struct sunxi_mmc_priv *priv, struct mmc *mmc, struct mmc_data *data)
{
	struct mmc_des_v4p1 *pdes = priv->pdes;
	unsigned byte_cnt = data->blocksize * data->blocks;
	unsigned char *buff;
	unsigned des_idx = 0;
	unsigned buff_frag_num = 0;
	unsigned remain;
	unsigned i, rval;

	buff = data->flags & MMC_DATA_READ ?
			(unsigned char *)data->dest : (unsigned char *)data->src;
	buff_frag_num = byte_cnt >> SDXC_DES_NUM_SHIFT;
	remain = byte_cnt & (SDXC_DES_BUFFER_MAX_LEN - 1);
	if (remain)
		buff_frag_num++;
	else
		remain = SDXC_DES_BUFFER_MAX_LEN;
	flush_cache((unsigned long)buff, ALIGN((unsigned long)byte_cnt, CONFIG_SYS_CACHELINE_SIZE));
	for (i = 0; i < buff_frag_num; i++, des_idx++) {
		memset((void *)&pdes[des_idx], 0, sizeof(struct mmc_des_v4p1));
		pdes[des_idx].des_chain = 1;
		pdes[des_idx].own = 1;
		pdes[des_idx].dic = 1;
		if (buff_frag_num > 1 && i != buff_frag_num - 1)
			pdes[des_idx].data_buf1_sz = SDXC_DES_BUFFER_MAX_LEN;
		else
			pdes[des_idx].data_buf1_sz = remain;
		/* AW1823 AW1851 AW1855 ... support 4G ddr
		 * AW1823 sdc0/sdc1 version: 0x40200
		 * AW1823 sdc2 version: 0x40502
		 * AW1851 AW1855.. version: 0x50300
		 * AW1750 sdc0:version:0x40104
		 * */
		if (priv->version == 0x40200 || priv->version == 0x40502 || priv->version >= 0x50300 || priv->version == 0x40104)
			pdes[des_idx].buf_addr_ptr1 = ((ulong)buff + i * SDXC_DES_BUFFER_MAX_LEN)
							>> 2;
		else
			pdes[des_idx].buf_addr_ptr1 = ((ulong)buff + i * SDXC_DES_BUFFER_MAX_LEN);
		if (i == 0)
			pdes[des_idx].first_des = 1;

		if (i == buff_frag_num - 1) {
			pdes[des_idx].dic = 0;
			pdes[des_idx].last_des = 1;
			pdes[des_idx].end_of_ring = 1;
			pdes[des_idx].buf_addr_ptr2 = 0;
		} else {
			/* AW1823 AW1851 AW1855 ... support 4G ddr
			 * AW1823 sdc0/sdc1 version: 0x40200
			 * AW1823 sdc2 version: 0x40502
			 * AW1851 AW1855.. version: 0x50300
			 * AW1750 sdc0:version:0x40104
			 * */
			if (priv->version == 0x40200 || priv->version == 0x40502 || priv->version >= 0x50300 || priv->version == 0x40104)
				pdes[des_idx].buf_addr_ptr2 = ((ulong)&pdes[des_idx + 1]) >> 2;
			else
				pdes[des_idx].buf_addr_ptr2 = ((ulong)&pdes[des_idx + 1]);
		}
		MMCDBG("frag %d, remain %d, des[%d](%08x): "
			"[0] = %08x, [1] = %08x, [2] = %08x, [3] = %08x\n",
			i, remain, des_idx, PT_TO_PHU(&pdes[des_idx]),
			(u32)((u32 *)&pdes[des_idx])[0], (u32)((u32 *)&pdes[des_idx])[1],
			(u32)((u32 *)&pdes[des_idx])[2], (u32)((u32 *)&pdes[des_idx])[3]);
	}
	flush_cache((unsigned long)pdes, ALIGN(sizeof(struct mmc_des_v4p1) * (des_idx + 1), CONFIG_SYS_CACHELINE_SIZE));

	WR_MB();

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
	rval = readl(&priv->reg->gctrl);
	writel(rval | (1 << 5) | (1 << 2), &priv->reg->gctrl);	/* dma enable */
	writel((1 << 0), &priv->reg->dmac); /* idma reset */
	while (readl(&priv->reg->dmac) & 0x1) {
	} /* wait idma reset done */

	writel((1 << 1) | (1 << 7), &priv->reg->dmac); /* idma on */
	rval = readl(&priv->reg->idie) & (~3);
	if (data->flags & MMC_DATA_WRITE)
		rval |= (1 << 0);
	else
		rval |= (1 << 1);
	writel(rval, &priv->reg->idie);
	/* AW1823 AW1851 AW1855 ... support 4G ddr
	 * AW1823 sdc0/sdc1 version: 0x40200
	 * AW1823 sdc2 version: 0x40502
	 * AW1851 AW1855.. version: 0x50300
	 * AW1750 sdc0:version:0x40104
	 * */
	if (priv->version == 0x40200 || priv->version == 0x40502 || priv->version >= 0x50300 || priv->version == 0x40104)
		writel(((unsigned long)pdes) >> 2, &priv->reg->dlba);
	else
		writel(((unsigned long)pdes), &priv->reg->dlba);
	writel(priv->dma_tl, &priv->reg->ftrglevel);
	return 0;
}

static int mmc_rint_wait(struct sunxi_mmc_priv *priv, struct mmc *mmc,
			 uint timeout_msecs, uint done_bit, const char *what, uint usedma)
{
	unsigned int status;
	unsigned int done = 0;
	unsigned long start = get_timer(0);
	do {
		status = readl(&priv->reg->rint);
		if ((get_timer(start) > timeout_msecs) || (status & SUNXI_MMC_RINT_INTERRUPT_ERROR_BIT)) {
			MMCDBG(mmc, "mmc %d %s timeout %x status %x\n", priv->mmc_no, what,
					status & SUNXI_MMC_RINT_INTERRUPT_ERROR_BIT, status);
			return -ETIMEDOUT;
		}
		if (usedma && !strncmp(what, "data", sizeof("data")))
			done = ((status & done_bit) && (readl(&priv->reg->idst) & 0x3)) ? 1 : 0;
		else
			done = (status & done_bit);
	} while (!done);

	return 0;
}

static void sunxi_mmc_set_rdtmout_reg(struct sunxi_mmc_priv *priv, struct mmc *mmc,
					unsigned int rdtmout)
{
	unsigned int rval = 0;
	unsigned int rdto_clk = 0;
	unsigned int mode_2x = 0;
	unsigned int hs400_ntm_en = 0;

	rdto_clk = mmc->clock / 1000 * rdtmout;
	rval = readl(&priv->reg->ntsr);
	mode_2x = rval & (0x1 << 31);
	hs400_ntm_en = rval & 0x1;

	if ((mmc->speed_mode == HS400 && hs400_ntm_en)
	     || (mmc->speed_mode == HSDDR52_DDR50 && mmc->bus_width == 8)
	     || (mmc->speed_mode == HSDDR52_DDR50 && mmc->bus_width == 4 && mode_2x)) {
		rdto_clk = rdto_clk << 1;
	}

	rval = readl(&priv->reg->gctrl);
	/*ddr50 mode don't use 256x timeout unit*/
	if (rdto_clk > 0xffffff && mmc->speed_mode != HSDDR52_DDR50) {
		rdto_clk = (rdto_clk + 255)/256;
		rval |= (0x1 << 11);
	} else {
		rdto_clk = 0xffffff;
		rval &= ~(0x1 << 11);
	}
	writel(rval, &priv->reg->gctrl);

	rval = readl(&priv->reg->timeout);
	rval &= ~(0xffffff << 8);
	rval |= (rdto_clk << 8);
	writel(rval, &priv->reg->timeout);

	MMCDBG("rdtoclk:%d, reg-tmout:%d, gctl:%x, speed_mode:%d, clock:%d, nstr:%x\n",
		rdto_clk, readl(&priv->reg->timeout), readl(&priv->reg->gctrl),
		mmc->speed_mode, mmc->clock, readl(&priv->reg->ntsr));
}

static int sunxi_mmc_do_send_cmd_common(struct sunxi_mmc_priv *priv,
				     struct mmc *mmc, struct mmc_cmd *cmd,
				     struct mmc_data *data)
{
	unsigned int cmdval = SUNXI_MMC_CMD_START;
	unsigned int timeout_msecs;
	int error = 0;
	unsigned int status = 0;
	unsigned int usedma = 0;
	unsigned int bytecnt = 0;

	if (priv->fatal_err) {
		MMCINFO("mmc %d Found fatal err,so no send cmd\n", priv->mmc_no);
		return -1;
	}
	if (cmd->resp_type & MMC_RSP_BUSY)
		MMCDBG("mmc cmd %d check rsp busy\n", cmd->cmdidx);
	if (cmd->cmdidx == 12 && mmc->manual_stop_flag == 0) {
		MMCDBG("usually, cmd12 is sent after cmd18/cmd25 automantically.\n");
		/* don't wait write busy here, because no cmd12 will be sent for cmd24.
		 * write busy status will be check after sent cmd25. */
		return 0;
	}

	if (!cmd->cmdidx)
		cmdval |= SUNXI_MMC_CMD_SEND_INIT_SEQ;
	if (cmd->resp_type & MMC_RSP_PRESENT)
		cmdval |= SUNXI_MMC_CMD_RESP_EXPIRE;
	if (cmd->resp_type & MMC_RSP_136)
		cmdval |= SUNXI_MMC_CMD_LONG_RESPONSE;
	if (cmd->resp_type & MMC_RSP_CRC)
		cmdval |= SUNXI_MMC_CMD_CHK_RESPONSE_CRC;

#if 0
/* write ds dly Traversal test*/
	if ((uboot_spare_head.boot_data.work_mode == WORK_MODE_BOOT)
			&& ((cmd->cmdidx == 24) || (cmd->cmdidx == 25))) {
		unsigned tmode = priv->timing_mode;
		printf("===========there is write opration!========\n");
		if (mmc->speed_mode == HS200_SDR104 && (tmode == SUNXI_MMC_TIMING_MODE_4)) {
			u32 rval = 0;
			rval = readl(&priv->reg->samp_dl);
			rval &= (~SDXC_CfgDly);
			rval |= ((40 & SDXC_CfgDly) | SDXC_EnableDly);
			writel(rval, &priv->reg->samp_dl);
			printf("hs200 samp 0x%x\n", readl(&priv->reg->samp_dl));
		} else if (mmc->speed_mode == HSSDR52_SDR25 && (tmode == SUNXI_MMC_TIMING_MODE_4)) {
		    u32 rval = 0;
		    rval = readl(&priv->reg->samp_dl);
		    rval &= (~SDXC_CfgDly);
		    rval |= ((38 & SDXC_CfgDly) | SDXC_EnableDly);
		    writel(rval, &priv->reg->samp_dl);
		    printf("hssdr samp 0x%x\n", readl(&priv->reg->samp_dl));
		} else if (mmc->speed_mode == HS400 && (tmode == SUNXI_MMC_TIMING_MODE_4)) {
			    u32 rval = 0;
			    rval = readl(&priv->reg->ds_dl);
			    rval &= (~SDXC_CfgDly);
			    rval |= ((31 & SDXC_CfgDly) | SDXC_EnableDly);
			    writel(rval, &priv->reg->ds_dl);
			    printf("hs400 ds 0x%x\n", readl(&priv->reg->ds_dl));
		}
	}

#endif
#if 0
/*read ds dly Traversal test*/
	if ((uboot_spare_head.boot_data.work_mode == WORK_MODE_BOOT)
			&& ((cmd->cmdidx == 17) || (cmd->cmdidx == 18))) {
		unsigned tmode = priv->timing_mode;
		int work_mode = uboot_spare_head.boot_data.work_mode;
		if (work_mode != WORK_MODE_BOOT
				|| (mmc->do_tuning == 0x1 && mmc->tuning_end == 0x0)
				|| (mmc->cfg->sample_mode != AUTO_SAMPLE_MODE)) {
		} else {
			printf("===========There is read opration!========\n");
			if (mmc->speed_mode == HS200_SDR104 && (tmode == SUNXI_MMC_TIMING_MODE_4)) {
				u32 rval = 0;
				rval = readl(&priv->reg->samp_dl);
				rval &= (~SDXC_CfgDly);
				rval |= ((40 & SDXC_CfgDly) | SDXC_EnableDly);
				writel(rval, &priv->reg->samp_dl);
				printf("hs200 samp 0x%x\n", readl(&priv->reg->samp_dl));
			} else if (mmc->speed_mode == HSSDR52_SDR25 && (tmode == SUNXI_MMC_TIMING_MODE_4)) {
				u32 rval = 0;
				rval = readl(&priv->reg->samp_dl);
				rval &= (~SDXC_CfgDly);
				rval |= ((38 & SDXC_CfgDly) | SDXC_EnableDly);
				writel(rval, &priv->reg->samp_dl);
				printf("hssdr samp 0x%x\n", readl(&priv->reg->samp_dl));
			} else if (mmc->speed_mode == HS400 && (tmode == SUNXI_MMC_TIMING_MODE_4)) {
				u32 rval = 0;
				rval = readl(&priv->reg->ds_dl);
				rval &= (~SDXC_CfgDly);
				rval |= ((30 & SDXC_CfgDly) | SDXC_EnableDly);
				writel(rval, &priv->reg->ds_dl);
				printf("hs400 ds 0x%x\n", readl(&priv->reg->ds_dl));
			}
		}
	}

#endif
#if 0
/*read  ds retry test*/
	if ((uboot_spare_head.boot_data.work_mode == WORK_MODE_BOOT)
			&& ((cmd->cmdidx == 18) || (cmd->cmdidx == 17))) {
		unsigned tmode = priv->timing_mode;
		int work_mode = uboot_spare_head.boot_data.work_mode;
		static int i = 1;
		if (work_mode != WORK_MODE_BOOT
				|| (mmc->do_tuning == 0x1 && mmc->tuning_end == 0x0)
				|| (mmc->cfg->sample_mode != AUTO_SAMPLE_MODE)) {
		} else {
			printf("===========There is read opration!========\n");
			if ((i < 5) && (tmode == HS400)) {
				u32 rval = 0;
				rval = readl(&priv->reg->ds_dl);
				rval &= (~SDXC_CfgDly);
				rval |= ((40 & SDXC_CfgDly) | SDXC_EnableDly);
				writel(rval, &priv->reg->ds_dl);
				MMCINFO("ds %x\n", readl(&priv->reg->ds_dl));
				i++;
			} else
				MMCINFO("end mauanl failed%d\n", i);
		}
	}
#endif
#if 0
/*write ds delay retry test*/
	if ((uboot_spare_head.boot_data.work_mode == WORK_MODE_BOOT)
			&& ((cmd->cmdidx == 24) || (cmd->cmdidx == 25))) {
		unsigned tmode = priv->timing_mode;
		int work_mode = uboot_spare_head.boot_data.work_mode;
		static int i = 1;
		if (work_mode != WORK_MODE_BOOT
				|| (mmc->do_tuning == 0x1 && mmc->tuning_end == 0x0)
				|| (mmc->cfg->sample_mode != AUTO_SAMPLE_MODE)) {
		} else {
			if ((i < 5) && (tmode == HS400)) {
				u32 rval = 0;
				rval = readl(&priv->reg->ds_dl);
				rval &= (~SDXC_CfgDly);
				rval |= ((58 & SDXC_CfgDly) | SDXC_EnableDly);
				writel(rval, &priv->reg->ds_dl);
				printf("ds %x\n", readl(&priv->reg->ds_dl));
				i++;
			} else
				MMCINFO("end\n");
		}
	}
#endif
#if 0
/*sdlay retry test*/
	if ((uboot_spare_head.boot_data.work_mode == WORK_MODE_BOOT)
			&& ((cmd->cmdidx == 24) || (cmd->cmdidx == 25))) {
		unsigned tmode = priv->timing_mode;
		int work_mode = uboot_spare_head.boot_data.work_mode;
		static int i = 1;
		if (work_mode != WORK_MODE_BOOT
				|| (mmc->do_tuning == 0x1 && mmc->tuning_end == 0x0)
				|| (mmc->cfg->sample_mode != AUTO_SAMPLE_MODE)) {
		} else {
			if ((i < 3) && (tmode == SUNXI_MMC_TIMING_MODE_4) && (mmc->speed_mode == HS200_SDR104)) {
				u32 rval = 0;
				rval = readl(&priv->reg->samp_dl);
				rval &= (~SDXC_CfgDly);
				rval |= ((54 & SDXC_CfgDly) | SDXC_EnableDly);
				writel(rval, &priv->reg->samp_dl);
				MMCINFO("***********samp %x\n", readl(&priv->reg->samp_dl));
				i++;
			} else
				MMCINFO("end %d tm %d\n", i, tmode);
		}
	}
#endif
#if 0
/*sdly Traversal write*/
	if ((uboot_spare_head.boot_data.work_mode == WORK_MODE_BOOT)
			&& ((cmd->cmdidx == 24) || (cmd->cmdidx == 25))) {
		unsigned tmode = priv->timing_mode;
		int work_mode = uboot_spare_head.boot_data.work_mode;
		if (work_mode != WORK_MODE_BOOT
				|| (mmc->do_tuning == 0x1 && mmc->tuning_end == 0x0)
				|| (mmc->cfg->sample_mode != AUTO_SAMPLE_MODE)) {
		} else {
			printf("===========there is write opration!========\n");
			if (mmc->speed_mode == HS200_SDR104 && (tmode == SUNXI_MMC_TIMING_MODE_4)) {
				u32 rval = 0;
				rval = readl(&priv->reg->samp_dl);
				rval &= (~SDXC_CfgDly);
				rval |= ((40 & SDXC_CfgDly) | SDXC_EnableDly);
				writel(rval, &priv->reg->samp_dl);
				printf("hs200 samp 0x%x\n", readl(&priv->reg->samp_dl));
			} else if (mmc->speed_mode == HSSDR52_SDR25 && (tmode == SUNXI_MMC_TIMING_MODE_4)) {
				u32 rval = 0;
				rval = readl(&priv->reg->samp_dl);
				rval &= (~SDXC_CfgDly);
				rval |= ((45 & SDXC_CfgDly) | SDXC_EnableDly);
				writel(rval, &priv->reg->samp_dl);
				printf("hssdr samp 0x%x\n", readl(&priv->reg->samp_dl));
			}
		}
	}
#endif
#if 0
/*sdly Traversal read*/
	if ((uboot_spare_head.boot_data.work_mode == WORK_MODE_BOOT)
			&& ((cmd->cmdidx == 17) || (cmd->cmdidx == 18))) {
		unsigned tmode = priv->timing_mode;
		int work_mode = uboot_spare_head.boot_data.work_mode;
		if (work_mode != WORK_MODE_BOOT
				|| (mmc->do_tuning == 0x1 && mmc->tuning_end == 0x0)
				|| (mmc->cfg->sample_mode != AUTO_SAMPLE_MODE)) {
		} else {
			printf("===========There is read opration!========\n");
			if (mmc->speed_mode == HS200_SDR104 && (tmode == SUNXI_MMC_TIMING_MODE_4)) {
				u32 rval = 0;
				rval = readl(&priv->reg->samp_dl);
				rval &= (~SDXC_CfgDly);
				rval |= ((40 & SDXC_CfgDly) | SDXC_EnableDly);
				writel(rval, &priv->reg->samp_dl);
				printf("hs200 samp 0x%x\n", readl(&priv->reg->samp_dl));
			} else if (mmc->speed_mode == HSSDR52_SDR25 && (tmode == SUNXI_MMC_TIMING_MODE_4)) {
				u32 rval = 0;
				rval = readl(&priv->reg->samp_dl);
				rval &= (~SDXC_CfgDly);
				rval |= ((47 & SDXC_CfgDly) | SDXC_EnableDly);
				writel(rval, &priv->reg->samp_dl);
				printf("hssdr samp 0x%x\n", readl(&priv->reg->samp_dl));
			} else if (mmc->speed_mode == HS400 && (tmode == SUNXI_MMC_TIMING_MODE_4)) {
				u32 rval = 0;
				rval = readl(&priv->reg->samp_dl);
				rval &= (~SDXC_CfgDly);
				rval |= ((53 & SDXC_CfgDly) | SDXC_EnableDly);
				writel(rval, &priv->reg->samp_dl);
				printf("hs400 samp 0x%x\n", readl(&priv->reg->samp_dl));
			}
		}
	}
#endif
#if 0
/*read samply delay retry test*/
	if ((uboot_spare_head.boot_data.work_mode == WORK_MODE_BOOT)
			&& ((cmd->cmdidx == 18) || (cmd->cmdidx == 17))) {
		unsigned tmode = priv->timing_mode;
		int work_mode = uboot_spare_head.boot_data.work_mode;
		static int i = 1;
		if (work_mode != WORK_MODE_BOOT
				|| (mmc->do_tuning == 0x1 && mmc->tuning_end == 0x0)
				|| (mmc->cfg->sample_mode != AUTO_SAMPLE_MODE)) {
		} else {
			printf("===========There is read opration!========\n");
			if ((i < 4) && (tmode == HS400)) {
				u32 rval = 0;
				rval = readl(&priv->reg->samp_dl);
				rval &= (~SDXC_CfgDly);
				rval |= ((53 & SDXC_CfgDly) | SDXC_EnableDly);
#ifdef FPGA_PLATFORM
				rval &= (~0x7);
#endif
				writel(rval, &priv->reg->samp_dl);
				MMCINFO("samp %x\n", readl(&priv->reg->samp_dl));
				i++;
			} else
				MMCINFO("end mauanl failed%d\n", i);
		}
	}
#endif
	if (data) {
		if ((u32)(long)data->dest & 0x3) {
			error = -1;
			MMCINFO("%s,%d,dest is not 4 aligned\n", __FUNCTION__, __LINE__);
			goto out;
		}

		cmdval |= SUNXI_MMC_CMD_DATA_EXPIRE|SUNXI_MMC_CMD_WAIT_PRE_OVER;
		if (data->flags & MMC_DATA_WRITE)
			cmdval |= SUNXI_MMC_CMD_WRITE;
		if (data->blocks > 1)
			cmdval |= SUNXI_MMC_CMD_AUTO_STOP;
		writel(data->blocksize, &priv->reg->blksz);
		writel(data->blocks * data->blocksize, &priv->reg->bytecnt);
	} else {
		if (cmd->cmdidx == 12 && mmc->manual_stop_flag == 1) {
			/*stop current data transferin progress.*/
			cmdval |= SUNXI_MMC_CMD_STOP_ABORT;
			/*Send command at once, even if previous data transfer has not completed*/
			cmdval &= ~SUNXI_MMC_CMD_WAIT_PRE_OVER;
		}
	}

	MMCDBG("mmc %d, cmd %d(0x%08x), arg 0x%08x\n", priv->mmc_no,
	      cmd->cmdidx, cmdval | cmd->cmdidx, cmd->cmdarg);
	writel(cmd->cmdarg, &priv->reg->arg);

	if (!data)
		writel(cmdval | cmd->cmdidx, &priv->reg->cmd);
	/*
	 * transfer data and check status
	 * STATREG[2] : FIFO empty
	 * STATREG[3] : FIFO full
	 */
	if (data) {
		int ret = 0;

		/*dto set to 200ms*/
		sunxi_mmc_set_rdtmout_reg(priv, mmc, DTO_MAX);

		bytecnt = data->blocksize * data->blocks;
		MMCDBG("trans data %d bytes\n", bytecnt);
#ifdef CONFIG_MMC_SUNXI_USE_DMA
		if (bytecnt > 64) {
#else
		if (0) {
#endif
			usedma = 1;
			writel(readl(&priv->reg->gctrl) & (~SUNXI_MMC_GCTRL_ACCESS_BY_AHB), &priv->reg->gctrl);
			ret = mmc_trans_data_by_dma(priv, mmc, data);
			writel(cmdval | cmd->cmdidx, &priv->reg->cmd);
		} else {
			writel(readl(&priv->reg->gctrl) | SUNXI_MMC_GCTRL_ACCESS_BY_AHB, &priv->reg->gctrl);
			writel(cmdval | cmd->cmdidx, &priv->reg->cmd);
			ret = mmc_trans_data_by_cpu(priv, mmc, data);
		}
		if (ret) {
			error = readl(&priv->reg->rint) &
				SUNXI_MMC_RINT_INTERRUPT_ERROR_BIT;
			error = -ETIMEDOUT;
			goto out;
		}
	}

	error = mmc_rint_wait(priv, mmc, 1000, SUNXI_MMC_RINT_COMMAND_DONE,
			      "cmd", usedma);
	if (error) {
		goto out;
	}

	if (data) {
		timeout_msecs = 6000;
		MMCDBG("cacl timeout %x msec\n", timeout_msecs);
		error = mmc_rint_wait(priv, mmc, timeout_msecs,
				      data->blocks > 1 ?
				      SUNXI_MMC_RINT_AUTO_COMMAND_DONE :
				      SUNXI_MMC_RINT_DATA_OVER,
				      "data", usedma);
		if (error) {
			goto out;
		}
	}

	if ((cmd->resp_type & MMC_RSP_BUSY) ||
			((data) && (data->flags & MMC_DATA_WRITE))) {
		unsigned long start = get_timer(0);
		if ((cmd->cmdidx == MMC_CMD_ERASE) ||
				((cmd->cmdidx == MMC_CMD_SWITCH) &&
				(((cmd->cmdarg >> 16) & 0xFF) == EXT_CSD_SANITIZE_START)))
			timeout_msecs = 0x1fffffff;
		else
			timeout_msecs = 2000;

		do {
			status = readl(&priv->reg->status);
			if (get_timer(start) > timeout_msecs) {
				MMCDBG("busy timeout\n");
				error = -ETIMEDOUT;
				goto out;
			}
		} while (status & SUNXI_MMC_STATUS_CARD_DATA_BUSY);
		if ((cmd->cmdidx == MMC_CMD_ERASE) ||
				((cmd->cmdidx == MMC_CMD_SWITCH) &&
				 (((cmd->cmdarg >> 16) & 0xFF) == EXT_CSD_SANITIZE_START)))
			MMCINFO("%s: cmd %d wait rsp busy 0x%x ms \n", __FUNCTION__,
					cmd->cmdidx, get_timer(start));
	}

	if (cmd->resp_type & MMC_RSP_136) {
		cmd->response[0] = readl(&priv->reg->resp3);
		cmd->response[1] = readl(&priv->reg->resp2);
		cmd->response[2] = readl(&priv->reg->resp1);
		cmd->response[3] = readl(&priv->reg->resp0);
		MMCDBG("mmc resp 0x%08x 0x%08x 0x%08x 0x%08x\n",
		      cmd->response[3], cmd->response[2],
		      cmd->response[1], cmd->response[0]);
	} else {
		cmd->response[0] = readl(&priv->reg->resp0);
		MMCDBG("mmc resp 0x%08x\n", cmd->response[0]);
	}
out:
	if (error) {
		priv->raw_int_bak = readl(&priv->reg->rint) & SUNXI_MMC_RINT_INTERRUPT_ERROR_BIT;
		mmc_dump_errinfo(priv, cmd);
#if 0
	if (cmd->cmdidx == 8) {
		dumphex32("mmc", (char *)priv->reg, 0x200);
		dumphex32("ccmu_mmc0", (char *)SUNXI_CCM_BASE + 0x830, 0x4);
		dumphex32("ccmu_pll", (char *)SUNXI_PRCM_BASE + 0x1010, 0x10);
		dumphex32("ccmu_bgr", (char *)SUNXI_CCM_BASE + 0x84C, 0x4);
		dumphex32("ccmu_srr", (char *)SUNXI_CCM_BASE + 0x84C, 0x4);
		dumphex32("gpio_config", (char *)SUNXI_PIO_BASE + 0x48, 0x10);
		dumphex32("gpio_pull", (char *)SUNXI_PIO_BASE + 0x64, 0x8);
	}
#endif
	}
	if (data && usedma) {
		status = readl(&priv->reg->idst);
		writel(status, &priv->reg->idst);
		writel(0, &priv->reg->idie);
		writel(0, &priv->reg->dmac);
		writel(readl(&priv->reg->gctrl) & (~(1 << 5)), &priv->reg->gctrl);
	}
	if (error < 0) {
		/* during tuning sample point, some sample point may cause timing problem.
		for example, if a RTO error occurs, host may stop clock and device may still output data.
		we need to read all data(512bytes) from device to avoid to update clock fail.
		*/
		signed int timeout = 0;
		if (mmc->do_tuning && data && (data->flags&MMC_DATA_READ) && (bytecnt == 512)) {
			writel(readl(&priv->reg->gctrl)|0x80000000, &priv->reg->gctrl);
			writel(0xdeb, &priv->reg->dbgc);
			timeout = 1000;
			MMCMSG(mmc, "Read remain data\n");
			while (readl(&priv->reg->bbcr) < 512) {
				unsigned int tmp = readl(priv->reg->fifo);
				tmp = tmp + 1;
				MMCDBG("Read data 0x%x, bbcr 0x%x\n", tmp, readl(&priv->reg->bbcr));
				__usdelay(1);
				if (!(timeout--)) {
					MMCMSG(mmc, "Read remain data timeout\n");
					break;
				}
			}
		}

		writel(0x7, &priv->reg->gctrl);
		while (readl(&priv->reg->gctrl)&0x7) {
			MMCDBG("mmc reset dma fifo and fifo\n");
		};

		{
			mmc_save_regs(priv);
			mmc_clk_io_onoff(priv->mmc_no, 0, 0);
			MMCDBG(mmc, "mmc %d close bus gating and reset\n", priv->mmc_no);
			mmc_clk_io_onoff(priv->mmc_no, 1, 0);
			mmc_restore_regs(priv);

			writel(0x7, &priv->reg->gctrl);
			while (readl(&priv->reg->gctrl)&0x7) {
				MMCDBG("mmc reset dma fifo and fifo\n");
			};
		}

		writel(SUNXI_MMC_GCTRL_RESET, &priv->reg->gctrl);
		mmc_update_clk(priv);
	}
	writel(0xffffffff, &priv->reg->rint);
	writel(readl(&priv->reg->gctrl) | SUNXI_MMC_GCTRL_FIFO_RESET,
	       &priv->reg->gctrl);
	if (data && (data->flags&MMC_DATA_READ) && usedma) {
		unsigned char *buff = (unsigned char *)data->dest;
		unsigned byte_cnt = data->blocksize * data->blocks;
		invalidate_dcache_range((unsigned long)buff,
					((unsigned long)buff + ALIGN((unsigned long)byte_cnt,
					CONFIG_SYS_CACHELINE_SIZE)));

		MMCDBG("invald cache after read complete\n");
	}

	return error;
}

static int mmc_raw_send_manual_stop(struct mmc *mmc)
{
	int ret = 0;
	struct mmc_cmd cmd;
	struct sunxi_mmc_priv *priv = mmc->priv;

	cmd.cmdidx = MMC_CMD_STOP_TRANSMISSION;
	cmd.cmdarg = 0;
	cmd.resp_type = MMC_RSP_R1b;
	mmc->manual_stop_flag = 1;
	ret = sunxi_mmc_do_send_cmd_common(priv, mmc, &cmd, NULL);
	if (ret)
		MMCINFO("bsp send manual stop failed\n");

	return ret;
}

static int mmc_check_r1_ready(struct mmc *mmc, u32 timeout_us)
{
	struct sunxi_mmc_priv *priv = mmc->priv;
	struct mmc_reg_v4p1 *reg = priv->reg;
	int error = 0;
	unsigned int status = 0;

	do {
		status = readl(&reg->status);
		if (!timeout_us--) {
			error = -1;
			MMCINFO("mmc %d check busy timeout %u\n", priv->mmc_no, timeout_us);
			goto out;
		}
		__usdelay(1);
	} while (status & (1 << 9));
out:
	MMCDBG("host bsp ready\n");
	return error;
}

static int sunxi_mmc_send_cmd_common(struct sunxi_mmc_priv *priv,
				struct mmc *mmc, struct mmc_cmd *cmd,
				struct mmc_data *data)
{
	int work_mode = uboot_spare_head.boot_data.work_mode;
	int err = 0;
	int has_reinit = 0;

host_retry:
	err = sunxi_mmc_do_send_cmd_common(priv, mmc, cmd, data);
	if (work_mode != WORK_MODE_BOOT
			|| (mmc->do_tuning == 0x1 && mmc->tuning_end == 0x0)
			|| (mmc->cfg->sample_mode != AUTO_SAMPLE_MODE)) {
		return err;
	}

	if (err) {
		if (!has_reinit) {
			if (sunxi_need_rty(mmc)) {
#if 0
				MMCINFO("start reinit\n");
				struct mmc_config *cfg = (struct mmc_config *) (mmc->cfg);
				mmc->has_init = 0;
				cfg->host_caps &= ~(MMC_MODE_HS400 | MMC_MODE_HS200 | MMC_MODE_DDR_52MHz);
				mmc->speed_mode = 0;
				mmc->cfg->ops->init(mmc);
				mmc_set_bus_width(mmc, 1);
				mmc_set_clock(mmc, 1, false);
				mmc_init(mmc);
				has_reinit = 1;
				goto host_retry;
#else
				MMCINFO("give up reinit\n");
				mmc->cfg->ops->decide_retry(mmc, 0, 1);
				return err;
#endif
			} else {
				MMCINFO("host retry\n");
				mmc_raw_send_manual_stop(mmc);
				mmc_check_r1_ready(mmc, 1000*1000);
				mmc->cfg->ops->set_ios(mmc);
				goto host_retry;
			}
		} else {
			MMCINFO("retry giveup!\n");
			mmc->cfg->ops->decide_retry(mmc, 0, 1);
		}
	} else {
		MMCDBG("Reset retry cnt\n");
		mmc->cfg->ops->decide_retry(mmc, 0, 1);
	}

	return err;
}

#if !CONFIG_IS_ENABLED(DM_MMC)
static int sunxi_mmc_set_ios_legacy(struct mmc *mmc)
{
	struct sunxi_mmc_priv *priv = mmc->priv;

	return sunxi_mmc_set_ios_common(priv, mmc);
}

static int sunxi_mmc_send_cmd_legacy(struct mmc *mmc, struct mmc_cmd *cmd,
				     struct mmc_data *data)
{
	struct sunxi_mmc_priv *priv = mmc->priv;

	return sunxi_mmc_send_cmd_common(priv, mmc, cmd, data);
}

static int sunxi_mmc_getcd_legacy(struct mmc *mmc)
{
	struct sunxi_mmc_priv *priv = mmc->priv;
	int cd_pin;

	cd_pin = sunxi_mmc_getcd_gpio(priv->mmc_no);
	if (cd_pin < 0)
		return 1;

	return !gpio_get_value(cd_pin);
}

static int sunxi_tm4_retry(struct mmc *mmc, u8 tm4_retry_gap, u8 type)
{
	struct sunxi_mmc_priv *priv = (struct sunxi_mmc_priv *)mmc->priv;
	u32 spd_md, freq;
	u8 *sdly;

	spd_md = priv->tm4.cur_spd_md;
	freq = priv->tm4.cur_freq;

	if (type == 1) {
		sdly = &priv->tm4.sdly[spd_md*MAX_CLK_FREQ_NUM+freq];
		MMCINFO("Current spd_md %d freq_id %d sdly %d\n", spd_md, freq, *sdly);
		priv->sd_retry_cnt++;
		if (priv->sd_retry_cnt * tm4_retry_gap <  MMC_CLK_SAMPLE_POINIT_MODE_4) {
			if ((*sdly + tm4_retry_gap) < MMC_CLK_SAMPLE_POINIT_MODE_4) {
				*sdly = *sdly + tm4_retry_gap;
			} else {
				*sdly = *sdly + tm4_retry_gap - MMC_CLK_SAMPLE_POINIT_MODE_4;
			}
			MMCINFO("Get next samply point %d at spd_md %d freq_id %d\n", *sdly, spd_md, freq);
		} else {
			MMCINFO("Beyond the sdly retry times\n");
			return -1;
		}
	} else {
		sdly = &priv->tm4.dsdly[freq];
		MMCINFO("Current spd_md %d freq_id %d dsdly %d\n", spd_md, freq, *sdly);
		priv->dsd_retry_cnt++;
		if (priv->dsd_retry_cnt * tm4_retry_gap <  MMC_CLK_SAMPLE_POINIT_MODE_4) {
			if ((*sdly + tm4_retry_gap) < MMC_CLK_SAMPLE_POINIT_MODE_4) {
				*sdly = *sdly + tm4_retry_gap;
			} else {
				*sdly = *sdly + tm4_retry_gap - MMC_CLK_SAMPLE_POINIT_MODE_4;
			}
			MMCINFO("Get next ds point %d at spd_md %d freq_id %d\n", *sdly, spd_md, freq);
		} else {
			MMCINFO("Beyond the dsdly retry times\n");
			return -1;
		}
	}
	return 0;
}

static int sunxi_decide_rty(struct mmc *mmc, int err_no, uint rst_cnt)
{
	struct sunxi_mmc_priv *priv = (struct sunxi_mmc_priv *)mmc->priv;
	unsigned tmode = priv->timing_mode;
	u32 spd_md, freq;
	u8 *sdly;
	u8 tm1_retry_gap = 1;
	u8 tm3_retry_gap = 8;
#ifdef SUNXI_MMC_RETRY_TEST
	u8 tm4_retry_gap = 32;
#else
	u8 tm4_retry_gap = 2;
#endif

	if (rst_cnt) {
		priv->sd_retry_cnt = 0;
		priv->dsd_retry_cnt = 0;
	}

	if (err_no && (!(err_no & SDXC_RespTimeout) || (err_no == 0xffffffff))) {
		if (tmode == SUNXI_MMC_TIMING_MODE_1) {
			spd_md = priv->tm1.cur_spd_md;
			freq = priv->tm1.cur_freq;
			sdly = &priv->tm1.sdly[spd_md*MAX_CLK_FREQ_NUM+freq];

			priv->sd_retry_cnt++;
			if (priv->sd_retry_cnt * tm1_retry_gap <  MMC_CLK_SAMPLE_POINIT_MODE_1) {
				if ((*sdly + tm1_retry_gap) < MMC_CLK_SAMPLE_POINIT_MODE_1) {
					*sdly = *sdly + tm1_retry_gap;
				} else {
					*sdly = *sdly + tm1_retry_gap - MMC_CLK_SAMPLE_POINIT_MODE_1;
				}
				MMCINFO("Get next samply point %d at spd_md %d freq_id %d\n", *sdly, spd_md, freq);
			} else {
				MMCINFO("Beyond the retry times\n");
				return -1;
			}
		} else if (tmode == SUNXI_MMC_TIMING_MODE_3) {
			spd_md = priv->tm3.cur_spd_md;
			freq = priv->tm3.cur_freq;
			sdly = &priv->tm3.sdly[spd_md*MAX_CLK_FREQ_NUM+freq];

			priv->sd_retry_cnt++;
			if (priv->sd_retry_cnt * tm3_retry_gap <  MMC_CLK_SAMPLE_POINIT_MODE_3) {
				if ((*sdly + tm3_retry_gap) < MMC_CLK_SAMPLE_POINIT_MODE_3) {
					*sdly = *sdly + tm3_retry_gap;
				} else {
					*sdly = *sdly + tm3_retry_gap - MMC_CLK_SAMPLE_POINIT_MODE_3;
				}
				MMCINFO("Get next samply point %d at spd_md %d freq_id %d\n", *sdly, spd_md, freq);
			} else {
				MMCINFO("Beyond the retry times\n");
				return -1;
			}
		} else if (tmode == SUNXI_MMC_TIMING_MODE_4) {
			spd_md = priv->tm4.cur_spd_md;
			freq = priv->tm4.cur_freq;
			if (spd_md == HS400) {
				if ((err_no == 0xffffffff)) {
					if (sunxi_tm4_retry(mmc, tm4_retry_gap, 1)) {
						if (sunxi_tm4_retry(mmc, tm4_retry_gap, 0))
							return -1;
					}
				} else if ((err_no & (SDXC_RespErr | SDXC_RespCRCErr))) {
					if (sunxi_tm4_retry(mmc, tm4_retry_gap, 1))
						return -1;
				} else {
					if (sunxi_tm4_retry(mmc, tm4_retry_gap, 0))
						return -1;
				}
			} else {
				if (sunxi_tm4_retry(mmc, tm4_retry_gap, 1))
					return -1;
			}
		}
		priv->raw_int_bak = 0;
		return 0;
	}
	MMCDBG("rto or no error or software timeout,no need retry\n");

	return -1;
}

static int sunxi_detail_errno(struct mmc *mmc)
{
	struct sunxi_mmc_priv *priv = (struct sunxi_mmc_priv *)mmc->priv;
	u32 err_no = priv->raw_int_bak;

	priv->raw_int_bak = 0;
	return err_no;
}


static const struct mmc_ops sunxi_mmc_ops = {
	.send_cmd	= sunxi_mmc_send_cmd_legacy,
	.set_ios	= sunxi_mmc_set_ios_legacy,
	.init		= sunxi_mmc_core_init,
	.getcd		= sunxi_mmc_getcd_legacy,
	.decide_retry		= sunxi_decide_rty,
	.get_detail_errno	= sunxi_detail_errno,
};


int sunxi_mmcno_to_devnum(int sdc_no)
{
	struct mmc *m;
	struct sunxi_mmc_priv *ppriv = &mmc_host[sdc_no];
	if (ppriv->mmc_no != sdc_no) {
		printk("error,card no error\n");
		return -1;
	}
//	m = container_of((void *)ppriv, struct mmc, priv);
	m = ppriv->mmc;
	if (m == NULL) {
		printk("error card no error\n");
		return -1;
	}
	MMCDBG("%s ,devnum %d\n", __FUNCTION__,  m->block_dev.devnum);
	MMCDBG("devnum %d, pprv %x, bdesc %x\n",  m->block_dev.devnum, PT_TO_PHU(ppriv), PT_TO_PHU(&m->block_dev));
	MMCDBG("m %x, ppriv %x", PT_TO_PHU(m), PT_TO_PHU(ppriv));
	return m->block_dev.devnum;
}

struct mmc *sunxi_mmc_init(int sdc_no)
{
	__attribute__((unused)) struct sunxi_ccm_reg *ccm = (struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	struct sunxi_mmc_priv *priv = &mmc_host[sdc_no];
	struct mmc_config *cfg = &priv->cfg;
	int ret;
	int version;
	struct mmc *mmc = NULL;

	memset(priv, 0, sizeof(struct sunxi_mmc_priv));
	MMCINFO("mmc driver ver %s\n", DRIVER_VER);
	memset(&mmc_host_reg_bak[sdc_no], 0, sizeof(struct mmc_reg_v4p1));
	priv->reg_bak =  &mmc_host_reg_bak[sdc_no];

	if ((sdc_no == 2)) {
		cfg->odly_spd_freq = &ext_odly_spd_freq[0];
		cfg->sdly_spd_freq = &ext_sdly_spd_freq[0];
	} else if (sdc_no == 0) {
		cfg->odly_spd_freq = &ext_odly_spd_freq_sdc0[0];
		cfg->sdly_spd_freq = &ext_sdly_spd_freq_sdc0[0];
	}

	cfg->name = "SUNXI SD/MMC";
	cfg->host_no = sdc_no;
	cfg->ops  = &sunxi_mmc_ops;

	cfg->voltages = MMC_VDD_32_33 | MMC_VDD_33_34;
	cfg->host_caps |= MMC_MODE_HS_52MHz | MMC_MODE_HS | MMC_MODE_DDR_52MHz;
	cfg->b_max = CONFIG_SYS_MMC_MAX_BLK_COUNT;

	/* default f_min and f_max */
	cfg->f_min = 400000;/* 400K */
	cfg->f_max = 50000000;/* 50M */

	/* default timing mode */
	priv->timing_mode = SUNXI_MMC_TIMING_MODE_1;

	priv->pdes = memalign(CONFIG_SYS_CACHELINE_SIZE, 256 * 1024);

	if (priv->pdes == NULL) {
		MMCINFO("get mem for descripter failed !\n");
		return NULL;
	} else {
		MMCDBG("get mem for descripter OK !\n");
	}
	if (sunxi_host_mmc_config(sdc_no) != 0) {
		MMCINFO("sunxi host mmc config failed!\n");
		return NULL;
	}
	if (sunxi_mmc_pin_set(sdc_no) != 0) {
		MMCINFO("sunxi mmc pin set failed!\n");
		return NULL;
	}
	if (mmc_resource_init(sdc_no) != 0) {
		MMCINFO("mmc resourse init failed!\n");
		return NULL;
	}
	mmc_clk_io_onoff(sdc_no, 1, 1);

	version = readl(&priv->reg->vers);
	MMCINFO("SUNXI SDMMC Controller Version:0x%x", version);
	priv->version = version;

	if (cfg->io_is_1v8) {
		MMCDBG("io is 1.8V\n");
		cfg->host_caps |= MMC_MODE_HS200;
		if (cfg->host_caps & MMC_MODE_8BIT)
			cfg->host_caps |= MMC_MODE_HS400;
	}

	if (cfg->host_caps_mask) {
		u32 mask = cfg->host_caps_mask;
		if (mask & DRV_PARA_DISABLE_MMC_MODE_HS400)
			cfg->host_caps &= (~MMC_MODE_HS400);
		if (mask & DRV_PARA_DISABLE_MMC_MODE_HS200)
			cfg->host_caps &= (~(MMC_MODE_HS200
						| MMC_MODE_HS400));
		if (mask & DRV_PARA_DISABLE_MMC_MODE_DDR_52MHz)
			cfg->host_caps &= (~(MMC_MODE_DDR_52MHz
						| MMC_MODE_HS400
						| MMC_MODE_HS200));
		if (mask & DRV_PARA_DISABLE_MMC_MODE_HS_52MHz)
			cfg->host_caps &= (~(MMC_MODE_HS_52MHz
						| MMC_MODE_DDR_52MHz
						| MMC_MODE_HS400
						| MMC_MODE_HS200));
		if (mask & DRV_PARA_DISABLE_MMC_MODE_8BIT)
			cfg->host_caps &= (~(MMC_MODE_8BIT
						| MMC_MODE_HS400));
		if (mask & DRV_PARA_ENABLE_EMMC_HW_RST)
			cfg->host_caps |= DRV_PARA_ENABLE_EMMC_HW_RST;
	}
	MMCDBG("host_caps:0x%x\n", cfg->host_caps);

#ifdef FPGA_PLATFORM
	int i = 0;
	if (sdc_no == 0) {
		for (i = 0; i < 6; i++) {
			sunxi_gpio_set_cfgpin(SUNXI_GPF(i), SUNXI_GPF_SDC0);
			sunxi_gpio_set_pull(SUNXI_GPF(i), 1);
			sunxi_gpio_set_drv(SUNXI_GPF(i), 2);
		}
	} else {
		unsigned int pin;
		for (pin = SUNXI_GPC(0); pin <= SUNXI_GPF(25); pin++) {
				sunxi_gpio_set_cfgpin(pin, 2);
				sunxi_gpio_set_pull(pin, SUNXI_GPIO_PULL_UP);
				sunxi_gpio_set_drv(pin, 2);
		}
	}
#endif

	/* config ahb clock */
	MMCDBG("init mmc %d clock and io\n", sdc_no);
#if !defined(CONFIG_MACH_SUN50I_H6) && !defined(CONFIG_MACH_SUN8IW16)\
	&& !defined(CONFIG_MACH_SUN50IW9) && !defined(CONFIG_MACH_SUN8IW19)\
	&& !defined(CONFIG_MACH_SUN50IW10) && !defined(CONFIG_MACH_SUN8IW15)\
	&& !defined(CONFIG_MACH_SUN50IW11) && !defined(CONFIG_MACH_SUN50IW12)\
	&& !defined(CONFIG_MACH_SUN20IW1) && !defined(CONFIG_MACH_SUN8IW20)\
	&& !defined(CONFIG_MACH_SUN8IW21) && !defined(CONFIG_MACH_SUN50IW5)
	setbits_le32(&ccm->ahb_gate0, 1 << AHB_GATE_OFFSET_MMC(sdc_no));

#ifdef CONFIG_SUNXI_GEN_SUN6I
	/* unassert reset */
	setbits_le32(&ccm->ahb_reset0_cfg, 1 << AHB_RESET_OFFSET_MMC(sdc_no));
#endif
#if defined(CONFIG_MACH_SUN9I)
	/* sun9i has a mmc-common module, also set the gate and reset there */
	writel(SUNXI_MMC_COMMON_CLK_GATE | SUNXI_MMC_COMMON_RESET,
	       SUNXI_MMC_COMMON_BASE + 4 * sdc_no);
#endif
#elif defined CONFIG_MACH_SUN50I_H6 /* CONFIG_MACH_SUN50I_H6 */
	setbits_le32(&ccm->sd_gate_reset, 1 << sdc_no);
	/* unassert reset */
	setbits_le32(&ccm->sd_gate_reset, 1 << (RESET_SHIFT + sdc_no));
#endif
	mmc = mmc_create(cfg, priv);
	if (!mmc)
		return NULL;
	ret = mmc_set_mod_clk(priv, 24000000);
	if (ret)
		return NULL;
	return mmc;
}
#else

static int sunxi_mmc_set_ios(struct udevice *dev)
{
	struct sunxi_mmc_plat *plat = dev_get_platdata(dev);
	struct sunxi_mmc_priv *priv = dev_get_priv(dev);

	return sunxi_mmc_set_ios_common(priv, &plat->mmc);
}

static int sunxi_mmc_send_cmd(struct udevice *dev, struct mmc_cmd *cmd,
			      struct mmc_data *data)
{
	struct sunxi_mmc_plat *plat = dev_get_platdata(dev);
	struct sunxi_mmc_priv *priv = dev_get_priv(dev);

	return sunxi_mmc_send_cmd_common(priv, &plat->mmc, cmd, data);
}

static int sunxi_mmc_getcd(struct udevice *dev)
{
	struct sunxi_mmc_priv *priv = dev_get_priv(dev);

	if (dm_gpio_is_valid(&priv->cd_gpio)) {
		int cd_state = dm_gpio_get_value(&priv->cd_gpio);

		return cd_state ^ priv->cd_inverted;
	}
	return 1;
}

static const struct dm_mmc_ops sunxi_mmc_ops = {
	.send_cmd	= sunxi_mmc_send_cmd,
	.set_ios	= sunxi_mmc_set_ios,
	.get_cd		= sunxi_mmc_getcd,
};

static int sunxi_mmc_probe(struct udevice *dev)
{
	struct mmc_uclass_priv *upriv = dev_get_uclass_priv(dev);
	struct sunxi_mmc_plat *plat = dev_get_platdata(dev);
	struct sunxi_mmc_priv *priv = dev_get_priv(dev);
	struct mmc_config *cfg = &plat->cfg;
	struct ofnode_phandle_args args;
	u32 *gate_reg;
	int bus_width, ret;

	cfg->name = dev->name;
	bus_width = dev_read_u32_default(dev, "bus-width", 1);

	cfg->voltages = MMC_VDD_32_33 | MMC_VDD_33_34;
	cfg->host_caps = 0;
	if (bus_width == 8)
		cfg->host_caps |= MMC_MODE_8BIT;
	if (bus_width >= 4)
		cfg->host_caps |= MMC_MODE_4BIT;
	cfg->host_caps |= MMC_MODE_HS_52MHz | MMC_MODE_HS;
	cfg->b_max = CONFIG_SYS_MMC_MAX_BLK_COUNT;
	cfg->f_min = 400000;
	cfg->f_max = 52000000;

	priv->reg = (void *)dev_read_addr(dev);

	/* We don't have a sunxi clock driver so find the clock address here */
	ret = dev_read_phandle_with_args(dev, "clocks", "#clock-cells", 0,
					  1, &args);
	if (ret)
		return ret;
	priv->mclkreg = (u32 *)ofnode_get_addr(args.node);

	ret = dev_read_phandle_with_args(dev, "clocks", "#clock-cells", 0,
					  0, &args);
	if (ret)
		return ret;
	gate_reg = (u32 *)ofnode_get_addr(args.node);
	setbits_le32(gate_reg, 1 << args.args[0]);
	priv->mmc_no = args.args[0] - 8;

	ret = mmc_set_mod_clk(priv, 24000000);
	if (ret)
		return ret;

	/* This GPIO is optional */
	if (!gpio_request_by_name(dev, "cd-gpios", 0, &priv->cd_gpio,
				  GPIOD_IS_IN)) {
		int cd_pin = gpio_get_number(&priv->cd_gpio);

		sunxi_gpio_set_pull(cd_pin, SUNXI_GPIO_PULL_UP);
	}

	/* Check if card detect is inverted */
	priv->cd_inverted = dev_read_bool(dev, "cd-inverted");

	upriv->mmc = &plat->mmc;

	/* Reset controller */
	writel(SUNXI_MMC_GCTRL_RESET, &priv->reg->gctrl);
	udelay(1000);

	return 0;
}

static int sunxi_mmc_bind(struct udevice *dev)
{
	struct sunxi_mmc_plat *plat = dev_get_platdata(dev);

	return mmc_bind(dev, &plat->mmc, &plat->cfg);
}

static const struct udevice_id sunxi_mmc_ids[] = {
	{ .compatible = "allwinner,sun5i-a13-mmc" },
	{ }
};

U_BOOT_DRIVER(sunxi_mmc_drv) = {
	.name		= "sunxi_mmc",
	.id		= UCLASS_MMC,
	.of_match	= sunxi_mmc_ids,
	.bind		= sunxi_mmc_bind,
	.probe		= sunxi_mmc_probe,
	.ops		= &sunxi_mmc_ops,
	.platdata_auto_alloc_size = sizeof(struct sunxi_mmc_plat),
	.priv_auto_alloc_size = sizeof(struct sunxi_mmc_priv),
};
#endif
