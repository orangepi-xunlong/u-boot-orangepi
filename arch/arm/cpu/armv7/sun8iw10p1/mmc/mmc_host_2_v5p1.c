/*
 * (C) Copyright 2007-2015
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * mmc_bsp.c
 * Description: MMC driver for General mmc operations
 * Author: WenJiaqiang
 * Date: 2016/6/26 19:52:12
 */

#include "common.h"
#include "mmc_def.h"
#include "mmc_bsp.h"
#include "mmc_host_2_v5p1.h"
#include "mmc.h"
#include <asm/arch/gpio.h>
#include <private_boot0.h>
#include <private_toc.h>
#include <private_uboot.h>

extern int mmc_config_addr; /* const boot0_file_head_t BT0_head; */
extern int mmc_register(int dev_num, struct mmc *mmc);
extern int mmc_unregister(int dev_num);

extern struct mmc mmc_dev[MAX_MMC_NUM];
extern struct sunxi_mmc_host mmc_host[MAX_MMC_NUM];

static struct mmc_reg_v5p1 reg_v5p1_bak;

static void mmc_v5p1_save_regs(struct mmc *mmc, void *p)
{
	struct sunxi_mmc_host* mmchost = (struct sunxi_mmc_host *)mmc->priv;
	struct mmc_reg_v5p1* reg = (struct mmc_reg_v5p1*)mmchost->reg;
	struct mmc_reg_v5p1* reg_bak = (struct mmc_reg_v5p1 *)p;

	memset(p, 0x0, sizeof(struct mmc_reg_v5p1));

	/* don't save all registers */

	//reg_bak->cmd_arg2       = reg->cmd_arg2       ;
	//reg_bak->blk_cfg        = reg->blk_cfg        ;
	//reg_bak->cmd_arg1       = reg->cmd_arg1       ;
	//reg_bak->cmd            = reg->cmd            ;
	//reg_bak->resp0          = reg->resp0          ;
	//reg_bak->resp1          = reg->resp1          ;
	//reg_bak->resp2          = reg->resp2          ;
	//reg_bak->resp3          = reg->resp3          ;
	//reg_bak->buff           = reg->buff           ;
	//reg_bak->status         = reg->status         ;
	reg_bak->ctrl1          = reg->ctrl1          ;
	reg_bak->rst_clk_ctrl   = reg->rst_clk_ctrl   ;
	//reg_bak->int_sta        = reg->int_sta        ;
	reg_bak->int_sta_en     = reg->int_sta_en     ;
	reg_bak->int_sig_en     = reg->int_sig_en     ;
	reg_bak->acmd_err_ctrl2 = reg->acmd_err_ctrl2 ;
	//reg_bak->res_0[4]       = reg->res_0[4]       ;
	//reg_bak->set_err        = reg->set_err        ;
	//reg_bak->adma_err       = reg->adma_err       ;
	//reg_bak->adma_addr      = reg->adma_addr      ;
	//reg_bak->res_1[105]     = reg->res_1[105]     ;
	reg_bak->ctrl3          = reg->ctrl3          ;
	//reg_bak->cmd_attr       = reg->cmd_attr       ;
	reg_bak->to_ctrl        = reg->to_ctrl        ;
	reg_bak->thld           = reg->thld           ;
	reg_bak->atc            = reg->atc            ;
	reg_bak->rtc            = reg->rtc            ;
	reg_bak->ditc0          = reg->ditc0          ;
	reg_bak->ditc1          = reg->ditc1          ;
	reg_bak->tp0            = reg->tp0            ;
	reg_bak->tp1            = reg->tp1            ;
	//reg_bak->res_2[2]       = reg->res_2[2]       ;
	reg_bak->ds_dly         = reg->ds_dly         ;
	//reg_bak->res_3[3]       = reg->res_3[3]       ;
	//reg_bak->crc_sta        = reg->crc_sta        ;
	//reg_bak->tbc0           = reg->tbc0           ;
	//reg_bak->tbc1           = reg->tbc1           ;
	//reg_bak->buff_lvl       = reg->buff_lvl       ;

}

static void mmc_v5p1_restore_regs(struct mmc *mmc, void *p)
{
	struct sunxi_mmc_host* mmchost = (struct sunxi_mmc_host *)mmc->priv;
	struct mmc_reg_v5p1* reg = mmchost->reg;
	struct mmc_reg_v5p1* reg_bak = (struct mmc_reg_v5p1 *)p;

	/* don't restore all registers */

	//reg->cmd_arg2       = reg_bak->cmd_arg2       ;
	//reg->blk_cfg        = reg_bak->blk_cfg        ;
	//reg->cmd_arg1       = reg_bak->cmd_arg1       ;
	//reg->cmd            = reg_bak->cmd            ;
	//reg->resp0          = reg_bak->resp0          ;
	//reg->resp1          = reg_bak->resp1          ;
	//reg->resp2          = reg_bak->resp2          ;
	//reg->resp3          = reg_bak->resp3          ;
	//reg->buff           = reg_bak->buff           ;
	//reg->status         = reg_bak->status         ;
	reg->ctrl1          = reg_bak->ctrl1          ;
	reg->rst_clk_ctrl   = reg_bak->rst_clk_ctrl   ;
	//reg->int_sta        = reg_bak->int_sta        ;
	reg->int_sta_en     = reg_bak->int_sta_en     ;
	reg->int_sig_en     = reg_bak->int_sig_en     ;
	reg->acmd_err_ctrl2 = reg_bak->acmd_err_ctrl2 ;
	//reg->res_0[4]       = reg_bak->res_0[4]       ;
	//reg->set_err        = reg_bak->set_err        ;
	//reg->adma_err       = reg_bak->adma_err       ;
	//reg->adma_addr      = reg_bak->adma_addr      ;
	//reg->res_1[105]     = reg_bak->res_1[105]     ;
	reg->ctrl3          = reg_bak->ctrl3          ;
	//reg->cmd_attr       = reg_bak->cmd_attr       ;
	reg->to_ctrl        = reg_bak->to_ctrl        ;
	reg->thld           = reg_bak->thld           ;
	reg->atc            = reg_bak->atc            ;
	reg->rtc            = reg_bak->rtc            ;
	reg->ditc0          = reg_bak->ditc0          ;
	reg->ditc1          = reg_bak->ditc1          ;
	reg->tp0            = reg_bak->tp0            ;
	reg->tp1            = reg_bak->tp1            ;
	//reg->res_2[2]       = reg_bak->res_2[2]       ;
	reg->ds_dly         = reg_bak->ds_dly         ;
	//reg->res_3[3]       = reg_bak->res_3[3]       ;
	//reg->crc_sta        = reg_bak->crc_sta        ;
	//reg->tbc0           = reg_bak->tbc0           ;
	//reg->tbc1           = reg_bak->tbc1           ;
	//reg->buff_lvl       = reg_bak->buff_lvl       ;
}

void mmc_v5p1_module_reset(struct mmc *mmc)
{
	struct sunxi_mmc_host* mmchost = (struct sunxi_mmc_host *)mmc->priv;
	u32 timeout = 0xff;

	mmchost->hclkbase &= (~(1U<<10)); //only for smch2
	mmchost->hclkrst  &= (~(1U<<10));

	timeout = 0xff;
	while (--timeout>0);

	mmchost->hclkbase |= (1U<<10); //only for smch2
	mmchost->hclkrst  |= (1U<<10);
}

#if 0
static int mmc_reset(struct mmc *mmc, u32 rst_mask)
{
	struct sunxi_mmc_host* mmchost = (struct sunxi_mmc_host *)mmc->priv;
	struct mmc_reg_v5p1 *reg = (struct mmc_reg_v5p1 *)mmchost->reg;
	u32 ret = 0;
	u32 timeout = 0xffff;

	reg->rst_clk_ctrl = (reg->rst_clk_ctrl|rst_mask);

	while ((reg->rst_clk_ctrl & rst_mask) && (--timeout>0)) ;
	if (timeout == 0) {
		mmcinfo("smc_reset: wait controller reset timeout!\n");
		ret = -1;
	}

	return ret;
}
#endif

static int mmc_core_init(struct mmc *mmc)
{
	struct sunxi_mmc_host* mmchost = (struct sunxi_mmc_host *)mmc->priv;
	struct mmc_reg_v5p1 *reg = (struct mmc_reg_v5p1 *)mmchost->reg;
	u32 rval = 0;

	writel(0xffffffff,&reg->int_sta);	/*clear all interrupt*/
	writel(0xffffffff,&reg->int_sta_en);	/*enable all interrupt status*/
	writel(0x00000000,&reg->int_sig_en);
	writel(0xffffffff,&reg->to_ctrl);	/*configure timeout*/
	
	/* enable write/read stop clock at block gap, bit8 read, bit9 write */
	rval  = readl(&reg->ctrl3);
	rval |= (0x3<<8);
	writel(rval,&reg->ctrl3);

	return 0;
}

static int mmc_config_clock(struct mmc *mmc, unsigned clk)
{
	struct sunxi_mmc_host* mmchost = (struct sunxi_mmc_host* )mmc->priv;
	struct mmc_reg_v5p1 *reg = (struct mmc_reg_v5p1 *)mmchost->reg;
	u32 rval = 0;

	if ((mmc->speed_mode == HSDDR52_DDR50) || (mmc->speed_mode == HS400)) {
		if (clk > mmc->f_max_ddr)
			clk = mmc->f_max_ddr;
	}

	/* disable card clock */
	rval = readl(&reg->rst_clk_ctrl);
	rval &= ~(1U << 2);
	writel(rval,&reg->rst_clk_ctrl);

	/* configure clock */
	if (mmc->speed_mode == HSDDR52_DDR50)
		mmchost->mod_clk = clk <<3;
	else
		mmchost->mod_clk = clk <<2;

	mmc_set_mclk(mmchost, mmchost->mod_clk);

	/* get mclk */
	if (mmc->speed_mode == HSDDR52_DDR50)
		mmc->clock = mmc_get_mclk(mmchost) >>3;
	else
		mmc->clock = mmc_get_mclk(mmchost) >>2;

	mmchost->clock = mmc->clock; /* bankup current clock frequency at host */
	mmcdbg("get round card clk %d, mod_clk %d\n", mmc->clock, mmchost->mod_clk);

	/* re-enable mclk */
	writel(readl(mmchost->mclkbase)|(1<<31), mmchost->mclkbase);
	mmcdbg("mmc %d mclkbase 0x%x\n", mmchost->mmc_no, readl(mmchost->mclkbase));


	/* Re-enable card clock */
	rval = readl(&reg->rst_clk_ctrl);
	rval |= (1U << 2);
	writel(rval,&reg->rst_clk_ctrl);

	/* configure delay for current frequency and speed mode */
	/*
		TODO...
	*/

	return 0;
}

static void mmc_set_ddr_mode(struct mmc *mmc)
{
	struct sunxi_mmc_host* mmchost = (struct sunxi_mmc_host *)mmc->priv;
	struct mmc_reg_v5p1 *reg = (struct mmc_reg_v5p1 *)mmchost->reg;
	u32 rval = reg->acmd_err_ctrl2;

	rval &= ~(0x7U<<16);
	if (mmc->speed_mode == HSDDR52_DDR50) {
		writel(0x50200000,&reg->atc);
		rval |= (0x4U<<16);
	} else if (mmc->speed_mode == HS400) {
		writel(0x50200000,&reg->atc);
		rval |= (0x5U<<16);
	} else {
		writel(0x30200000,&reg->atc);
	}
	writel(rval,&reg->acmd_err_ctrl2);
}

static void mmc_set_ios(struct mmc *mmc)
{
	struct sunxi_mmc_host* mmchost = (struct sunxi_mmc_host *)mmc->priv;
	struct mmc_reg_v5p1 *reg = (struct mmc_reg_v5p1 *)mmchost->reg;
	u32 rval = 0;

	mmcdbg("mmc %d ios: bus: %d, clock: %d\n", mmchost ->mmc_no,
		mmc->bus_width, mmc->clock);

	if (mmc->clock && mmc_config_clock(mmc, mmc->clock)) {
	    mmcinfo("[mmc]: " "*** update clock failed\n");
		mmchost->fatal_err = 1;
		return;
	}
	/* change bus width */
	rval = readl(&reg->ctrl1);
	
	if (mmc->bus_width == 8) {
		rval &= ~(0x1U<<1);
		rval |= 0x1U<<5;
	} else if (mmc->bus_width == 4) {
		rval |= 0x1U<<1;
		rval &= ~(0x1<<5);
	} else {
		rval &= ~(0x1U<<1);
		rval &= ~(0x1U<<5);
	}
	writel(rval,&reg->ctrl1);

	/* set ddr mode */
	mmc_set_ddr_mode(mmc);
}

static int mmc_trans_data_by_cpu(struct mmc *mmc, struct mmc_data *data)
{
	struct sunxi_mmc_host* mmchost = (struct sunxi_mmc_host *)mmc->priv;
	struct mmc_reg_v5p1 *reg = (struct mmc_reg_v5p1 *)mmchost->reg;
	unsigned i;
	unsigned byte_cnt = data->blocksize * data->blocks;
	unsigned *buff;
	unsigned timeout = 0xffffff;
	unsigned fifo_level = 0, fifo_level_leftover = 0;

	/* use ahb to access fifo */
	reg->ctrl3 |= CPUAcessBuffEn;

	if (data->flags & MMC_DATA_READ) {
		buff = (unsigned int *)data->b.dest;
		i = 0;
		while (i < (byte_cnt>>2)) {//word
			if(reg->status & BuffRDEn) {
				fifo_level = reg->buff_lvl & 0x1ff;
				while (fifo_level--) {
					buff[i++] = *(volatile u32*)mmchost->database;
					timeout = 0xffffff;
				}
			}
			if (!timeout--) {
				goto out;
			}
		}
	} else {
		buff = (unsigned int *)data->b.src;
		i = 0;
		while (i < (byte_cnt>>2)) {//word
			if (reg->status & BuffWREn) {
				fifo_level_leftover = FIFO_DEPTH_WORD - (reg->buff_lvl & 0x1ff);
				while (fifo_level_leftover--) {
					*(volatile u32*)mmchost->database = buff[i++];
					timeout = 0xffffff;
				}
			}
			if (!timeout--) {
				goto out;
			}
		}
	}

out:
	/* use dma to access fifo */
	reg->ctrl3 &= (~CPUAcessBuffEn);

	if (timeout <= 0){
		mmcinfo("mmc %d transfer by cpu failed\n",mmchost ->mmc_no);
		return -1;
	}

	return 0;
}

static int mmc_trans_data_by_dma(struct mmc *mmc, struct mmc_data *data)
{
	struct sunxi_mmc_host* mmchost = (struct sunxi_mmc_host *)mmc->priv;
	struct mmc_reg_v5p1 *reg = (struct mmc_reg_v5p1 *)mmchost->reg;
	struct mmc_des_v5p1 *pdes = NULL;
	unsigned byte_cnt = data->blocksize * data->blocks;
	unsigned char *buff;
	unsigned des_idx = 0;
	unsigned buff_frag_num = 0;
	unsigned remain;
	unsigned i, rval;
	int ret = 0;

	buff = data->flags & MMC_DATA_READ ?
			(unsigned char *)data->b.dest : (unsigned char *)data->b.src;
	buff_frag_num = byte_cnt >> SMHC_V5P1_DES_NUM_SHIFT;
	remain = byte_cnt & (SMHC_V5P1_DES_BUFFER_MAX_LEN-1);
	
	if (remain)
		buff_frag_num ++;
	else
		remain = SMHC_V5P1_DES_BUFFER_MAX_LEN;

	pdes = mmchost->pdes;

	for (i=0; i < buff_frag_num; i++, des_idx++) {
		memset((void*)&pdes[des_idx], 0, sizeof(struct mmc_des_v5p1));
		pdes[des_idx].valid = 1;
		pdes[des_idx].act = ACT_TRANS;

		if (buff_frag_num > 1 && i != buff_frag_num-1) {
			pdes[des_idx].length = SMHC_V5P1_DES_BUFFER_MAX_LEN;
		} else
			pdes[des_idx].length = remain;

		pdes[des_idx].addr = (u32)buff + i * SMHC_V5P1_DES_BUFFER_MAX_LEN;

		if (i == buff_frag_num-1) {
			pdes[des_idx].end = 1;
			pdes[des_idx].int_en = 1;
		}
		mmcdbg("mmc %d frag %d, remain %d, des[%d](%x): "
			"[0] = %x, [1] = %x\n",mmchost ->mmc_no,
			i, remain, des_idx, (u32)&pdes[des_idx],
			(u32)((u32*)&pdes[des_idx])[0], (u32)((u32*)&pdes[des_idx])[1]);
	}

	__asm("DSB");
	__asm("ISB");

	/* use dma to access fifo */
	reg->ctrl3 &= (~CPUAcessBuffEn);

	/* select dma type */
	rval = reg->ctrl1;
	rval &= ~(0x3U<<3);
	rval |= 0x1U<<3;
	//rval |= 0x3U<<3;
	reg->ctrl1 = rval;

	/* set dma desc list base addres */
	reg->adma_addr = (u32)mmchost->pdes;

#if 0
	/* reset dma. it will disable some interrupt status, including TransOverInt... */
	if (mmc_reset(mmc, ResetDat)) {
		mmcinfo("reset data path error!\n");
		ret = -1;
	}
#endif
	return ret;
}

static int mmc_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd,
			struct mmc_data *data)

{
	struct sunxi_mmc_host* mmchost = (struct sunxi_mmc_host *)mmc->priv;
	struct mmc_reg_v5p1 *reg = (struct mmc_reg_v5p1 *)mmchost->reg;
	unsigned int timeout = 0;
	int error = 0;
	unsigned int status = 0;
	unsigned int usedma = 0;
	unsigned int bytecnt = 0;
	u32 cmd_attr0 = ((cmd->cmdidx&0x3f) << 24);
	u32 cmd_attr1 = 0;

	if (mmchost->fatal_err) {
		mmcinfo("mmc %d Found fatal err,so no send cmd\n", mmchost ->mmc_no);
		return -1;
	}
	if (cmd->resp_type & MMC_RSP_BUSY)
		mmcdbg("mmc %d cmd %d check rsp busy\n", mmchost->mmc_no, cmd->cmdidx);

	if (cmd->cmdidx == 12)  
		return 0;

#ifdef MMC_TRANS_BY_DMA
	/* dma's descriptors are allocate at dram(0x42000000). smhc's dma only support descriptors and 
	data buffer at same memory type(dram or sram) */
	if(data && ((u32)(data->b.dest) > 0x40000000))
		usedma = 1;
#endif 

	/* wait command line idle */
	timeout = 0xff;
	while ((--timeout>0) && (reg->status&CmdInhibitCmd)) ;
	if (timeout == 0) {
		mmcinfo("wait Command Inhibit(CMD) timeout!\n");
		error = -1;
		goto OUT;
	}

	if (!cmd->cmdidx)
		cmd_attr1 |= SendInitSeq;

	if (cmd->resp_type & MMC_RSP_PRESENT)
	{
		if (cmd->resp_type & MMC_RSP_136)
			cmd_attr0 |= Rsp136;
		else if (cmd->resp_type & MMC_RSP_BUSY)
			cmd_attr0 |= Rsp48b;
		else
			cmd_attr0 |= Rsp48;

		if (cmd->resp_type & MMC_RSP_CRC)
			cmd_attr0 |= CheckRspCRC; /* response crc */
		if (cmd->resp_type & MMC_RSP_OPCODE)
			cmd_attr0 |= CheckRspIdx; /* response cmd index */
	}

	if (data)
	{
		/* wait data line idle */
		timeout = 0xff;
		while ((--timeout>0) && (reg->status&CmdInhibitDat)) ;
		if (timeout == 0) {
			mmcinfo("wait Command Inhibit(DAT) timeout!\n");
			error = -1;
			goto OUT;
		}

		if ((u32)data->b.dest & 0x3) {
			mmcinfo("mmc %d dest is not 4 byte align\n",mmchost ->mmc_no);
			error = -1;
			goto OUT;
		}

		cmd_attr0 |= DataExp;

		if (data->flags & MMC_DATA_READ)
			cmd_attr0 |= Read;

		if (usedma)
			cmd_attr0 |= DMAEn;

 		if (data->blocks > 1) {
			cmd_attr0 |= MultiBlkTrans;

			/* make sure auto cmd12 arg is 0 to avoid to be considered as HPI cmd by some emmc that support HPI  */
			cmd_attr0 |= AutoCmd12;
			reg->cmd_arg2 = 0x0;
		}

		reg->blk_cfg = ((data->blocks&0xffff)<<16) | (data->blocksize&0xfff);
	}

	reg->int_sta = 0xffffffff; /* clear int status */
	reg->cmd_arg1= cmd->cmdarg;
	reg->cmd_attr = cmd_attr1;

	mmcdbg("mmc %d, cmd %d(0x%x), arg 0x%x\n", mmchost->mmc_no, cmd->cmdidx,
		cmd_attr0, cmd->cmdarg);

	/* send cmd */
	if (!data)
	{
		reg->cmd = cmd_attr0;
	}
	else
	{
		int ret = 0;
		bytecnt = data->blocksize * data->blocks;
		mmcdbg("mmc %d trans data %d bytes\n",mmchost->mmc_no, bytecnt);
		if (usedma) 
		{	
			ret = mmc_trans_data_by_dma(mmc, data);
			reg->cmd = cmd_attr0;
		}
		else
		{
			reg->cmd = cmd_attr0;
			ret = mmc_trans_data_by_cpu(mmc, data);
		}

		if (ret) {
			mmcinfo("mmc %d Transfer failed\n", mmchost->mmc_no);
			error = reg->int_sta & 0x437f0000;
			if(!error)
				error = 0xffffffff;
			goto OUT;
		}
	}

	timeout = 0xffffff;
	do {
		status = reg->int_sta;
		if (!timeout-- || (status & 0x437f0000)) {
			error = status & 0x437f0000;
			if (!error)
				error = 0xffffffff; //represet software timeout
			mmcinfo("mmc %d cmd %d timeout, err %x\n",mmchost->mmc_no, cmd->cmdidx, error);
			goto OUT;
		}
	} while (!(status&CmdOverInt));

	if (data) {
		timeout = usedma ? 0xffff*bytecnt : 0xffff;

		do {
			status = reg->int_sta;
			if (!timeout-- || (status & 0x437f0000)) {
				error = status & 0x437f0000;
				if(!error)
					error = 0xffffffff;//represet software timeout
				mmcinfo("mmc %d data timeout, err %x\n",mmchost->mmc_no, error);
				goto OUT;
			}
		} while (!(status&TransOverInt));

		if (usedma) {
			timeout = 0xffffff;
			mmcdbg("mmc %d cacl rd dma timeout %x\n", mmchost->mmc_no, timeout);
			do {
				status = reg->int_sta;
				if (!timeout-- || (status & 0x437f0000)) {
					error = status & 0x437f0000;
					if(!error)
						error = 0xffffffff; //represet software timeout
					mmcinfo("mmc %d wait dma over err %x, adma err %x, current des 0x%x 0x%x\n",
						mmchost->mmc_no, error, reg->adma_err, reg->cddlw, reg->cddhw);
					goto OUT;
				}
			} while (!(status&DmaInt));
		}
	}

	if (cmd->resp_type & MMC_RSP_BUSY) {
		timeout = 0x4ffffff;
		do {
			status = reg->status;
			if (!timeout--) {
				error = -1;
				mmcinfo("mmc %d busy timeout\n", mmchost->mmc_no);
				goto OUT;
			}
		} while (!(status&Dat0LineSta));
	}

	if (cmd->resp_type & MMC_RSP_136) {
		/* Response Field[127:8] <-> RESP[119:0] */
		cmd->response[0] = ((reg->resp3 & 0xffffff) << 8);
		cmd->response[0] |= ((reg->resp2 >>24) & 0xff);
		cmd->response[1] = ((reg->resp2 & 0xffffff) << 8);
		cmd->response[1] |= ((reg->resp1 >>24) & 0xff);
		cmd->response[2] = ((reg->resp1 & 0xffffff) << 8);
		cmd->response[2] |= ((reg->resp0 >>24) & 0xff);
		cmd->response[3] = ((reg->resp0 & 0xffffff) << 8);

		mmcdbg("mmc %d resp 0x%x 0x%x 0x%x 0x%x\n", mmchost->mmc_no,
			cmd->response[3], cmd->response[2],
			cmd->response[1], cmd->response[0]);
	} else {
		cmd->response[0] = reg->resp0;
		mmcdbg("mmc %d resp 0x%x\n", mmchost->mmc_no, cmd->response[0]);
	}


OUT:

	if (error) {
		mmc_v5p1_save_regs(mmc, &reg_v5p1_bak);
		mmc_v5p1_module_reset(mmc);
		mmc_v5p1_restore_regs(mmc, &reg_v5p1_bak);
		mmcinfo("mmc %d cmd %d err %x\n",mmchost->mmc_no, cmd->cmdidx, error);
	}
	
	reg->int_sta = 0xffffffff; /*clear all interrupt*/
	reg->int_sta_en = 0xffffffff; 

	if (error)
		return -1;
	else
		return 0;
}

int mmc_v5p1_init(int sdc_no, unsigned bus_width, const normal_gpio_cfg *gpio_info, int offset ,void *extra_data)
{
	struct mmc *mmc;
	int ret;
	//u8 sdly=0xff, odly=0xff;
	//struct boot_sdmmc_private_info_t *priv_info =
	//	(struct boot_sdmmc_private_info_t *)(BT0_head.prvt_head.storage_data + SDMMC_PRIV_INFO_ADDR_OFFSET);
	//u8 ext_f_max = priv_info->boot_mmc_cfg.boot_hs_f_max;

	mmcinfo("mmc driver ver %s\n", DRIVER_VER);

	if (sdc_no != 2) {
		mmcinfo("mmc_v5p1_init: input sdc no error: %d, return -1\n", sdc_no);
		return -1;
	}

	memset(&mmc_dev[sdc_no], 0, sizeof(struct mmc));
	memset(&mmc_host[sdc_no], 0, sizeof(struct sunxi_mmc_host));
	mmc = &mmc_dev[sdc_no];
	mmc_host[sdc_no].mmc = mmc;

	/* host v5p1's timing mode is SUNXI_MMC_TIMING_MODE_2 */
	mmc_host[sdc_no].timing_mode = SUNXI_MMC_TIMING_MODE_2;

	strcpy(mmc->name, "SUNXI SD/MMC");
	mmc->priv = &mmc_host[sdc_no];
	mmc->send_cmd = mmc_send_cmd;
	mmc->set_ios = mmc_set_ios;
	mmc->init = mmc_core_init;
	mmc->update_phase = NULL;

	mmc->voltages = MMC_VDD_29_30|MMC_VDD_30_31|MMC_VDD_31_32|MMC_VDD_32_33|
	                MMC_VDD_33_34|MMC_VDD_34_35|MMC_VDD_35_36;
	mmc->host_caps = MMC_MODE_HS_52MHz|MMC_MODE_HS|MMC_MODE_HC;
	if (bus_width == 4)
		mmc->host_caps |= MMC_MODE_4BIT;
	if ((sdc_no == 2) && (bus_width == 8)) {
		mmc->host_caps |= MMC_MODE_8BIT | MMC_MODE_4BIT;
	}

	mmc->f_min = 400000;
	mmc->f_max = 50000000;
	mmc->f_max_ddr = 50000000;

#if 0  // only support HSSDR mode now for host v5p1
	if (!mmc_get_timing_cfg(sdc_no, 1, 2, &odly, &sdly)) {
		if (!((odly != 0xff) && (sdly != 0xff)))
			mmc->f_max = 25000000;
	} else
		mmc->f_max = 25000000;

	if ( !mmc_get_timing_cfg(sdc_no, 2, 2, &odly, &sdly) ) {
		if ((odly != 0xff) && (sdly != 0xff))
			mmc->host_caps |= MMC_MODE_DDR_52MHz;
		else
			mmc->f_max_ddr = 25000000;
	} else
		mmc->f_max_ddr = 25000000;

	if (ext_f_max && ((mmc->f_max_ddr/1000000) > ext_f_max))
		mmc->f_max_ddr = ext_f_max * 1000000;
	if (ext_f_max && ((mmc->f_max/1000000) > ext_f_max))
		mmc->f_max = ext_f_max * 1000000;
#endif

	mmc->control_num = sdc_no;

	mmc_host[sdc_no].pdes = (struct mmc_des_v5p1*)DMAC_DES_BASE_IN_SDRAM; 
	if (mmc_resource_init(sdc_no)){
		mmcinfo("mmc %d resource init failed\n",sdc_no);
		return -1;
	}

	mmc_clk_io_onoff(sdc_no, 1, gpio_info, offset);
	ret = mmc_register(sdc_no, mmc);
	if (ret < 0){
		mmcinfo("mmc %d register failed\n",sdc_no);
		return -1;
	}

	return mmc->lba;
}

int mmc_v5p1_exit(int sdc_no, const normal_gpio_cfg *gpio_info, int offset)
{
	mmc_clk_io_onoff(sdc_no, 0, gpio_info, offset);
	mmc_unregister(sdc_no);
	memset(&mmc_dev[sdc_no], 0, sizeof(struct mmc));
	memset(&mmc_host[sdc_no], 0, sizeof(struct sunxi_mmc_host));

	mmcdbg("sunxi mmc%d exit\n",sdc_no);
	return 0;
}


