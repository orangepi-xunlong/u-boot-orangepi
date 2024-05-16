// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2022-2023 StarFive Technology Co., Ltd.
 * Author:	yanhong <yanhong.wang@starfivetech.com>
 *
 */

#include <common.h>
#include <clk.h>
#include <dm/device.h>
#include <dm/read.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <misc.h>

/*otp param*/
#define OTP_MEM_START   0x800
#define OTP_MEM_SIZE    0x800
#define OTP_DIV_1US	1000000

/* program pulse time select. default value is 11*/
#define OTP_TPW_TIME	11

/* Read Data Access Time. max 50ns*/
#define OTP_TCD_TIME	20

#define OTP_PAOGRAM_DELAY	100
#define OTP_TURN_ON_DELAY	200

/*timing Register*/
#define OTP_CFGR_DIV_1US_MASK   0xff
#define OTP_CFGR_DIV_1US_SHIFT  8
#define OTP_CFGR_RD_CYC_MASK    0x0f
#define OTP_CFGR_RD_CYC_SHIFT   16

/*status Register*/
#define OTPC_SRR_BUSY           (1<<31)

/*otp operation mode select*/
#define OTP_OPRR_OPR_MASK           0x7
#define OTP_OPR_STANDBY             0x0 /* set otp to standby*/
#define OTP_OPR_READ                0x1 /* set otp to read*/

#define OTPC_TIMEOUT_CNT	10000
#define BYTES_PER_INT		4

struct starfive_otp_regs {
	u32 otp_cfg;        /*timing Register*/
	u32 otpc_ie;        /*interrupt Enable Register*/
	u32 otpc_sr;        /*status Register*/
	u32 otp_opr;        /*operation mode select Register*/
	u32 otpc_ctl;       /*otp control port*/
	u32 otpc_addr;      /*otp pa port*/
	u32 otpc_din;       /*otp pdin port*/
	u32 otpc_dout;      /*otp pdout*/
};

struct starfive_otp_platdata {
	struct starfive_otp_regs __iomem *regs;
	struct clk clk;
	u32 pclk_hz;
};

static int starfive_otp_regstatus(struct starfive_otp_regs *regs,
							u32 mask)
{
	int delay = OTPC_TIMEOUT_CNT;

	while (readl(&regs->otpc_sr) & mask) {
		udelay(OTP_PAOGRAM_DELAY);
		delay--;
		if (delay <= 0) {
			printf("%s: check otp status timeout\n", __func__);
			return -ETIMEDOUT;
		}
	}
	return 0;
}

static int starfive_otp_setmode(struct starfive_otp_regs *regs,
							u32 mode)
{
	writel(mode & OTP_OPRR_OPR_MASK, &regs->otp_opr);
	starfive_otp_regstatus(regs, OTPC_SRR_BUSY);

	return 0;
}

static int starfive_otp_config(struct udevice *dev)
{
	struct starfive_otp_platdata *plat = dev_get_plat(dev);
	struct starfive_otp_regs *regs = (struct starfive_otp_regs *)plat->regs;
	u32 div_1us;
	u32 rd_cyc;
	u32 val;

	div_1us = plat->pclk_hz / OTP_DIV_1US;
	rd_cyc = div_1us / OTP_TCD_TIME + 2;

	val = OTP_TPW_TIME;
	val |= (rd_cyc & OTP_CFGR_RD_CYC_MASK) << OTP_CFGR_RD_CYC_SHIFT;
	val |= (div_1us & OTP_CFGR_DIV_1US_MASK) << OTP_CFGR_DIV_1US_SHIFT;

	writel(val, &regs->otp_cfg);

	return 0;
}

static int starfive_otp_read(struct udevice *dev, int offset,
			   void *buf, int size)
{
	struct starfive_otp_platdata *plat = dev_get_plat(dev);
	void *base = (void *)plat->regs;
	u32 *databuf = (u32 *)buf;
	u32 data;
	int bytescnt;
	int i;

	if ((size % BYTES_PER_INT) || (offset % BYTES_PER_INT)) {
		printf("%s: size and offset must be multiple of 4.\n", __func__);
		return -EINVAL;
	}

	/* check bounds */
	if (!buf)
		return -EINVAL;
	if (offset >= OTP_MEM_SIZE)
		return -EINVAL;
	if ((offset + size) > OTP_MEM_SIZE)
		return -EINVAL;

	bytescnt = size / BYTES_PER_INT;

	for (i = 0; i < bytescnt; i++) {
		starfive_otp_setmode(plat->regs, OTP_OPR_READ);

		/* read the value */
		data = readl(base + OTP_MEM_START + offset);
		starfive_otp_regstatus(plat->regs, OTPC_SRR_BUSY);
		starfive_otp_setmode(plat->regs, OTP_OPR_STANDBY);
		databuf[i] = data;
		offset += 4;
	}

	return size;
}

static int starfive_otp_probe(struct udevice *dev)
{
	starfive_otp_config(dev);
	udelay(OTP_TURN_ON_DELAY);

	return 0;
}

static int starfive_otp_ofdata_to_platdata(struct udevice *dev)
{
	struct starfive_otp_platdata *plat = dev_get_plat(dev);
	int ret;

	plat->regs = dev_read_addr_ptr(dev);
	if (!plat->regs)
		goto err;

	ret = dev_read_u32(dev, "clock-frequency", &plat->pclk_hz);
	if (ret < 0)
		goto err;

	ret = clk_get_by_name(dev, "apb", &plat->clk);
	if (ret)
		goto err;

	ret = clk_enable(&plat->clk);
	if (ret < 0)
		goto err;

	return 0;
err:
	printf("%s init fail.\n", __func__);
	return ret;
}
static const struct misc_ops starfive_otp_ops = {
	.read = starfive_otp_read,
};
static const struct udevice_id starfive_otp_ids[] = {
	{ .compatible = "starfive,jh7110-otp" },
	{}
};
U_BOOT_DRIVER(starfive_otp) = {
	.name = "starfive_otp",
	.id = UCLASS_MISC,
	.of_match = starfive_otp_ids,
	.probe = starfive_otp_probe,
	.of_to_plat = starfive_otp_ofdata_to_platdata,
	.plat_auto = sizeof(struct starfive_otp_platdata),
	.ops = &starfive_otp_ops,
};
