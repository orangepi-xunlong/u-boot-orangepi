/**
 * SPDX-License-Identifier: GPL-2.0+
 * aw_rawnand_nfc.c
 *
 * (C) Copyright 2020 - 2021
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * cuizhikui <cuizhikui@allwinnertech.com>
 *
 */

#include <common.h>
#include <linux/errno.h>
#include <malloc.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/aw-rawnand.h>
#include <linux/kernel.h>
#include <sys_config.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include "aw_rawnand_nfc.h"

struct aw_nand_host aw_host;

#define MBUS_GATE           0x0804
#define NAND0_CFG           0x0810
#define NAND1_CFG           0x0814
#define NAND_GATE           0x082C

#define CCM_NAND_CTRL_M		(0xF << 0)
#define CCM_NAND_CTRL_CM(x)	((x) - 1)
#define CCM_NAND_CTRL_N		(0x3 << 8)
#define CCM_NAND_CTRL_CN(x)	((x) << 8)
#define CCM_NAND_CTRL_ENABLE	(0x1 << 31)
#define CCM_NAND_SRC_SELECT	(0x7 << 24)
#define CCM_NAND_SRC_CSELECT(x)	(((x)& 0x7) << 24)

static struct aw_nand_host *get_host(void)
{
	return &aw_host;
}

void aw_nfc_reg_dump(struct nfc_reg *reg)
{
	uint32_t i = 0;

	for (i = 0; i < 12; i++) {
		printf("[%p : %08x]\n", reg->ctl + i, readl(reg->ctl + i));
	}

	for (i = 0; i < 7; i++) {
		printf("[%p : %08x]\n", reg->ecc_ctl + i, readl(reg->ecc_ctl + i));
	}

	for (i = 0; i < 8; i++) {
		printf("[%p : %08x]\n", reg->err_cnt[0] + i, readl(reg->err_cnt[0] + i));
	}

	for (i = 0; i < 4; i++) {
		printf("[%p : %08x]\n", reg->user_data_len_base + i,
				readl(reg->user_data_len_base + i));
	}

	for (i = 0; i < 16; i++) {
		printf("[%p : %08x]\n", reg->user_data_base + i,
				readl(reg->user_data_base + i));
	}

	printf("[%p : %08x]\n", reg->spare_area, readl(reg->spare_area));
	printf("[%p : %08x]\n", reg->pat_id, readl(reg->pat_id));

	volatile unsigned *mdclk = (volatile unsigned *)(0x03001810);
	volatile unsigned *mcclk = (volatile unsigned *)(0x03001814);
	volatile unsigned *mbus_gate = (volatile unsigned *)(0x03001804);
	volatile unsigned *per0 = (volatile unsigned *)(0x03001020);
	volatile unsigned *mbus  = (volatile unsigned *)(0x03001540);
	printf("[%p : %08x]\n", mbus_gate, readl(mbus_gate));
	printf("[%p : %08x]\n", per0, readl(per0));
	printf("[%p : %08x]\n", mbus, readl(mbus));
	printf("[%p : %08x]\n", mdclk, readl(mdclk));
	printf("[%p : %08x]\n", mcclk, readl(mcclk));
}


static void aw_nfc_reg_prepare(struct nfc_reg *reg)
{
	struct aw_nand_host *host = container_of(reg, struct aw_nand_host, nfc_reg);
	int i = 0;
	AWRAWNAND_TRACE_NFC("Enter %s reg@%p\n", __func__, reg);

	AWRAWNAND_TRACE_NFC("host:%p\n", host, host);
	AWRAWNAND_TRACE_NFC("nfc:%p\n", &host->nfc_reg);
	host->base = (void *)SUNXI_NFC_BASE;

	reg->ctl = (volatile uint32_t *)((uint8_t *)host->base + 0x0000);
	reg->sta = (volatile uint32_t *)((uint8_t *)host->base + 0x0004);
	reg->int_ctl = (volatile uint32_t *)((uint8_t *)host->base + 0x0008);
	reg->timing_ctl = (volatile uint32_t *)((uint8_t *)host->base + 0x000c);
	reg->timing_cfg = (volatile uint32_t *)((uint8_t *)host->base + 0x0010);
	reg->addr_low = (volatile uint32_t *)((uint8_t *)host->base + 0x0014);
	reg->addr_high = (volatile uint32_t *)((uint8_t *)host->base + 0x0018);
	reg->data_block_mask = (volatile uint32_t *)((uint8_t *)host->base + 0x001c);
	reg->cnt = (volatile uint32_t *)((uint8_t *)host->base + 0x0020);
	reg->cmd = (volatile uint32_t *)((uint8_t *)host->base + 0x0024);
	reg->read_cmd_set = (volatile uint32_t *)((uint8_t *)host->base + 0x0028);
	reg->write_cmd_set = (volatile uint32_t *)((uint8_t *)host->base + 0x002c);
	reg->ecc_ctl = (volatile uint32_t *)((uint8_t *)host->base + 0x0034);
	reg->ecc_sta = (volatile uint32_t *)((uint8_t *)host->base + 0x0038);
	reg->data_pattern_sta = (volatile uint32_t *)((uint8_t *)host->base + 0x003c);
	reg->efr = (volatile uint32_t *)((uint8_t *)host->base + 0x0040);
	reg->rdata_sta_ctl = (volatile uint32_t *)((uint8_t *)host->base + 0x0044);
	reg->rdata_sta_0 = (volatile uint32_t *)((uint8_t *)host->base + 0x0048);
	reg->rdata_sta_1 = (volatile uint32_t *)((uint8_t *)host->base + 0x004c);
	for (i = 0; i < MAX_ERR_CNT; i++)
		reg->err_cnt[i] = (volatile uint32_t *)((uint8_t *)host->base + 0x0050+(i * 4));
	reg->user_data_len_base = (volatile uint32_t *)((uint8_t *)host->base + 0x0070);
	reg->user_data_base = (volatile uint32_t *)((uint8_t *)host->base + 0x0080);
	reg->efnand_sta = (volatile uint32_t *)((uint8_t *)host->base + 0x0110);
	reg->spare_area = (volatile uint32_t *)((uint8_t *)host->base + 0x0114);
	reg->pat_id = (volatile uint32_t *)((uint8_t *)host->base + 0x0118);
	reg->ddr2_spec_ctl = (volatile uint32_t *)((uint8_t *)host->base + 0x011c);
	reg->ndma_mode_ctl = (volatile uint32_t *)((uint8_t *)host->base + 0x0120);
	reg->mbus_dma_dlba = (volatile uint32_t *)((uint8_t *)host->base + 0x0200);
	reg->mbus_dma_sta = (volatile uint32_t *)((uint8_t *)host->base + 0x0204);
	reg->mdma_int_mask =  (volatile uint32_t *)((uint8_t *)host->base + 0x0208);
	reg->mdma_cur_desc_addr = (volatile uint32_t *)((uint8_t *)host->base + 0x020c);
	reg->mdma_cur_buf_addr = (volatile uint32_t *)((uint8_t *)host->base + 0x0210);
	reg->dma_cnt = (volatile uint32_t *)((uint8_t *)host->base + 0x0214);
	reg->ver = (volatile uint32_t *)((uint8_t *)host->base + 0x02f0);
	reg->ram0_base = (volatile uint32_t *)((uint8_t *)host->base + 0x0400);
	reg->ram1_base = (volatile uint32_t *)((uint8_t *)host->base + 0x0800);

}


static int aw_host_set_pin(struct aw_nand_host *host)
{
	int err = 0;

	/*set nfc pin*/
	err = fdt_set_all_pin("nand0", "pinctrl-0");
	if (err)
		awrawnand_err("set pinctrl-0 fail\n");

	return err;

}

static inline void noraml_req_get_addr(struct aw_nfc_normal_req *req, uint32_t *addr_low,
		uint32_t *addr_high)
{
	int i = 0;

	for (i = 0; i < req->op.cmd_with_addr.addr_cycles; i++) {
		if (i < 4)
			(*addr_low) |= (req->op.cmd_with_addr.addr[i] << (i * 8));
		else
			(*addr_high) |= (req->op.cmd_with_addr.addr[i] << ((i - 4) * 8));
	}
}

static int aw_host_nfc_wait_cmd_fifo_empty(struct nfc_reg *nfc)
{
	int ret = -ETIMEDOUT;
	uint32_t timeout_ms = 1000;
	uint32_t time_start = 0;

	time_start = get_timer(0);

	do {
		if (!(readl(nfc->sta) & NFC_CMD_FIFO_STATUS)) {
			ret = 0;
			goto out;
		}
	} while (get_timer(time_start) < timeout_ms);

	if (ret) {
		awrawnand_err("wait cmd fifo empty timeout 1s status@%x %x\n",
				nfc->sta, readl(nfc->sta));
		aw_nfc_reg_dump(nfc);
	}
out:
	return ret;
}

static int aw_host_nfc_wait_fsm_idle(struct nfc_reg *nfc)
{
	int ret = -ETIMEDOUT;
	uint32_t timeout_ms = 1000;
	uint32_t time_start = 0;

	time_start = get_timer(0);

	do {
		if (!(readl(nfc->sta) & NFC_STA)) {
			ret = 0;
			goto out;
		}
	} while (get_timer(time_start) < timeout_ms);

	if (ret) {
		awrawnand_err("wait nfc fsm idle timeout 1s status@%x %x\n",
				nfc->sta, readl(nfc->sta));
		aw_nfc_reg_dump(nfc);
	}
out:
	return ret;
}

static int aw_host_nfc_wait_cmd_finish(struct nfc_reg *nfc)
{
	int ret = -ETIMEDOUT;
	uint32_t timeout_ms = 10000;
	uint32_t time_start = 0;
	/*AWRAWNAND_TRACE_NFC("Enter %s\n", __func__);*/

	time_start = get_timer(0);

	do {
		if ((readl(nfc->sta) & NFC_CMD_INT_FLAG)) {
			ret = 0;
			goto out;
		}
	} while (get_timer(time_start) < timeout_ms);

	if (ret)
		awrawnand_err("wait nfc wait cmd finish 10s timeout[%x:%x]\n",
				nfc->sta, readl(nfc->sta));
out:
	/*write 1 to clear CMD INT FLAG*/
	writel(NFC_CMD_INT_FLAG, nfc->sta);
	/*AWRAWNAND_TRACE_NFC("Exit %s\n", __func__);*/
	return ret;
}
#if 0
static int aw_host_nfc_wait_B2R_int(struct nfc_reg *nfc)
{
	int ret = -ETIMEDOUT;
	uint32_t timeout_ms = 2000;
	uint32_t time_start = 0;
	uint32_t cfg = 0;

	time_start = get_timer(0);

	do {
		if ((readl(nfc->sta) & NFC_RB_B2R)) {
			ret = 0;
			goto out;
		}
	} while (get_timer(time_start) < timeout_ms);

	if (ret)
		awrawnand_err("wait nfc wait B2R 2s timeout [%x: %x]\n",
				nfc->sta, readl(nfc->sta));
out:
	/*write 1 to clear CMD B2R FLAG*/
	cfg = readl(nfc->sta);
	cfg |= NFC_RB_B2R;
	writel(cfg, nfc->sta);

	return ret;
}
#endif

static bool aw_host_nfc_wait_rb_ready(struct aw_nand_chip *chip, struct aw_nand_host *host)
{
	int ret = 0;
	uint32_t val = 0;
	int chip_no = chip->selected_chip.chip_no;
	uint32_t time_start = 0;
	AWRAWNAND_TRACE_NFC("Enter %s\n", __func__);
	time_start = get_timer(0);

	do {
		val = readl(host->nfc_reg.sta);
		if (chip->selected_chip.chip_no != -1) {
#ifndef DBG_NFC
			ret = (val & NFC_RB_STATE(chip->selected_chip.ceinfo[chip_no].relate_rb_no));
#else
			ret = ((val & NFC_RB_STATE(chip->selected_chip.ceinfo[chip_no].relate_rb_no)) && (val & NFC_RB_B2R));
#endif
			if (ret) {
				break;
			}
		}
	} while (get_timer(get_timer(time_start) < 60000));
	AWRAWNAND_TRACE_NFC("Exit %s %s status[%p:%x]\n", __func__, ret ? "ready" : "busy",
			host->nfc_reg.sta, readl(host->nfc_reg.sta));
	return ret ? true : false;
}

static bool aw_host_nfc_rb_ready(struct aw_nand_chip *chip, struct aw_nand_host *host)
{
	int ret = 0;
	uint32_t val = 0;
	int chip_no = chip->selected_chip.chip_no;

	AWRAWNAND_TRACE_NFC("Enter %s\n", __func__);

	AWRAWNAND_TRACE_NFC("selected chip_no:%d rb_no:%d\n", chip_no,
			chip->selected_chip.ceinfo[chip_no].relate_rb_no);

	val = readl(host->nfc_reg.sta);
	if (chip->selected_chip.chip_no != -1) {
#ifndef DBG_NFC
		ret = (val & NFC_RB_STATE(chip->selected_chip.ceinfo[chip_no].relate_rb_no));
#else
		ret = ((val & NFC_RB_STATE(chip->selected_chip.ceinfo[chip_no].relate_rb_no)) && (val & NFC_RB_B2R));
#endif
	}
	AWRAWNAND_TRACE_NFC("Exit %s %s status[%p:%x]\n", __func__, ret ? "ready" : "busy",
			host->nfc_reg.sta, readl(host->nfc_reg.sta));
	return ret ? true : false;
}

static int aw_host_wait_dma_ready_flag_timeout(struct aw_nand_host *host, uint32_t timeout_ms)
{
	int ret = -ETIMEDOUT;
	uint32_t time_start = 0;

	time_start = get_timer(0);

	do {
		if (host->dma_ready_flag) {
			ret = 0;
			goto out;
		}
	} while (get_timer(time_start) < timeout_ms);

	if (ret) {
		awrawnand_err("wait dma ready int tiemout status[%p:%x]\n",
				&host->nfc_reg.sta, readl(host->nfc_reg.sta));
	}
out:
	return ret;

}
static int aw_host_nfc_wait_status_timeout(struct nfc_reg *nfc, uint32_t mark, uint32_t value, uint32_t timeout_ms)
{
	int ret = -ETIMEDOUT;
	uint32_t time_start = 0;
	AWRAWNAND_TRACE_NFC("Enter %s\n", __func__);

	time_start = get_timer(0);

	do {
		if ((readl(nfc->sta) & mark) == value) {
			ret = 0;
			goto out;
		}
	} while (get_timer(time_start) < timeout_ms);

	if (ret)
		awrawnand_err("wait nfc wait status 10s timeout[%x:%x:%x:%x]\n",
				mark, value, nfc->sta, readl(nfc->sta));
out:
	AWRAWNAND_TRACE_NFC("Exit %s ret@%d sta[%x:%x]\n", __func__, ret, nfc->sta, readl(nfc->sta));
	return ret;
}
static int aw_host_nfc_noraml_op_cmd(struct aw_nand_chip *chip, struct aw_nfc_normal_req *req)
{
	struct aw_nand_host *host = awnand_chip_to_host(chip);
	struct nfc_reg *nfc = &host->nfc_reg;
	uint32_t cmd_cfg = 0;

	int ret = 0;

	AWRAWNAND_TRACE_NFC("Enter %s\n", __func__);

	if (aw_host_nfc_wait_cmd_fifo_empty(nfc) || aw_host_nfc_wait_fsm_idle(nfc)) {
		awrawnand_warn("cmd@%02x wait fifo or fsm timeout\n",
				req->op.cmd.code);
		ret = -ETIMEDOUT;
		goto out;
	}

	/*configure first command*/
	cmd_cfg |= req->op.cmd.code;
	cmd_cfg |= NFC_SEND_CMD1;


	if (req->wait_rb) {
		cmd_cfg |= NFC_WAIT_FLAG;
	}

	writel(cmd_cfg, nfc->cmd);

	AWRAWNAND_TRACE_NFC("cmd_cfg@%x reg_cmd@%x %x\n", cmd_cfg, nfc->cmd, readl(nfc->cmd));

	ret = aw_host_nfc_wait_cmd_finish(nfc);
	if (ret) {
		awrawnand_err("cmd@%d send fail\n", req->op.cmd.code);
	}


	AWRAWNAND_TRACE_NFC("Exit %s\n", __func__);
out:
	return ret;
}

static int aw_host_nfc_noraml_op_cmd_with_addr(struct aw_nand_chip *chip, struct aw_nfc_normal_req *req)
{
	struct aw_nand_host *host = awnand_chip_to_host(chip);
	struct nfc_reg *nfc = &host->nfc_reg;
	uint32_t low = 0;
	uint32_t high = 0;
	uint32_t cmd_cfg = 0;

	int ret = 0;
	AWRAWNAND_TRACE_NFC("Enter %s\n", __func__);

	if (aw_host_nfc_wait_cmd_fifo_empty(nfc) || aw_host_nfc_wait_fsm_idle(nfc)) {
		awrawnand_warn("cmd@%02x wait fifo or fsm timeout\n",
				req->op.cmd.code);
		ret = -ETIMEDOUT;
		goto out;
	}

	/*configure first command*/
	cmd_cfg |= req->op.cmd_with_addr.code;
	cmd_cfg |= NFC_SEND_CMD1;


	if (req->wait_rb) {
		cmd_cfg |= NFC_WAIT_FLAG;
	}

	/*configure address*/
	noraml_req_get_addr(req, &low, &high);
	writel(low, nfc->addr_low);
	if (req->op.cmd_with_addr.addr_cycles > 4)
		writel(high, nfc->addr_high);
	/*configure send's address number*/
	cmd_cfg |= NFC_ADR_NUM(req->op.cmd_with_addr.addr_cycles);
	cmd_cfg |= NFC_SEND_ADR;

	writel(cmd_cfg, nfc->cmd);
	AWRAWNAND_TRACE_NFC("cmd_cfg@%d cmd[%x:%x] addr_low:[%x:%x] addr_high:[%x:%x]\n", cmd_cfg, nfc->cmd, readl(nfc->cmd), nfc->addr_low, readl(nfc->addr_low), nfc->addr_high, readl(nfc->addr_high));

	ret = aw_host_nfc_wait_cmd_finish(nfc);
	if (ret) {
		awrawnand_err("cmd@%d send fail\n", req->op.cmd.code);
	}

	AWRAWNAND_TRACE_NFC("Exit %s\n", __func__);
out:
	return ret;
}


static int aw_host_nfc_noraml_op_cmd_with_addr_data(struct aw_nand_chip *chip, struct aw_nfc_normal_req *req)
{
	struct aw_nand_host *host = awnand_chip_to_host(chip);
	struct nfc_reg *nfc = &host->nfc_reg;
	uint32_t cmd_cfg = 0;
	uint32_t ctl_cfg = 0;
	uint32_t low = 0;
	uint32_t high = 0;

	int ret = 0;

	AWRAWNAND_TRACE_NFC("Enter %s\n", __func__);
	if (unlikely(req->op.cmd_with_addr_data.len > SZ_1K))
		goto out_exceed_fail;

	if (unlikely(req->op.cmd_with_addr_data.len == 0))
		goto out_do_nothing;

	if (unlikely(req->op.cmd_with_addr_data.in == NULL)
			&& unlikely(req->op.cmd_with_addr_data.out == NULL))
		goto out_invalid_paramter;

	if (aw_host_nfc_wait_cmd_fifo_empty(nfc) || aw_host_nfc_wait_fsm_idle(nfc)) {
		ret = -ETIMEDOUT;
		goto out_fifo_fsm_fail;
	}

	/*configure first command*/
	cmd_cfg |= req->op.cmd_with_addr_data.code;
	cmd_cfg |= NFC_SEND_CMD1;


	if (req->wait_rb) {
		cmd_cfg |= NFC_WAIT_FLAG;
	}

	/*configure address*/
	noraml_req_get_addr(req, &low, &high);
	low = (req->op.cmd_with_addr_data.addr[0] | (req->op.cmd_with_addr_data.addr[1] << 8)
			| (req->op.cmd_with_addr_data.addr[2] << 16) |
			(req->op.cmd_with_addr_data.addr[3] << 24));
	writel(low, nfc->addr_low);
	if (req->op.cmd_with_addr.addr_cycles > 4) {
		high = req->op.cmd_with_addr_data.addr[5];
		writel(high, nfc->addr_high);
	}

	/*configure send's address number*/
	if (req->op.cmd_with_addr_data.addr_cycles) {
		cmd_cfg |= NFC_ADR_NUM(req->op.cmd_with_addr_data.addr_cycles);
		cmd_cfg |= NFC_SEND_ADR;
	}


	writel((req->op.cmd_with_addr_data.len & 0x3ff), nfc->cnt);

	ctl_cfg = readl(nfc->ctl);
	ctl_cfg &= ~NFC_RAM_METHOD_DMA;
	writel(ctl_cfg, nfc->ctl);


	if (req->op.cmd_with_addr_data.direct == WRITE) {
		cmd_cfg |= NFC_ACCESS_DIR;
		memcpy_toio(nfc->ram0_base, req->op.cmd_with_addr_data.out,
				req->op.cmd_with_addr_data.len);
	}


	cmd_cfg |= NFC_DATA_TRANS;
	writel(cmd_cfg, nfc->cmd);

	AWRAWNAND_TRACE_NFC("cmd_cfg@%x cmd[%x:%x] ctl[%x:%x] cnt[%x:%x]\n",
			cmd_cfg, nfc->cmd, readl(nfc->cmd),
			nfc->ctl, readl(nfc->ctl), nfc->cnt, readl(nfc->cnt));

	ret = aw_host_nfc_wait_cmd_finish(nfc);
	if (ret)
		goto out_read_data_fail;


	if ((req->op.cmd_with_addr_data.direct == READ) && req->op.cmd_with_addr_data.in) {
		/*AWRAWNAND_TRACE_NFC("ram:%x %x\n", readl(nfc->ram0_base), readl(nfc->ram0_base + 1));*/
		memcpy_fromio(req->op.cmd_with_addr_data.in, nfc->ram0_base,
				req->op.cmd_with_addr_data.len);
	}


	AWRAWNAND_TRACE_NFC("Exit %s\n", __func__);
	return ret;

out_exceed_fail:
	awrawnand_err("req len@%d exceed @%d\n", req->op.cmd_with_addr_data.len, SZ_1K);
	return -EINVAL;
out_do_nothing:
	awrawnand_err("req len@0 , do nothing\n");
	return 0;
out_invalid_paramter:
	awrawnand_err("invalid parameter\n");
	return -EINVAL;
out_read_data_fail:
	awrawnand_err("read data fail\n");
	return ret;
out_fifo_fsm_fail:
	awrawnand_err("fifo or fsm is not empty\n");
	return ret;
}

static int aw_host_nfc_normal_op(struct aw_nand_chip *chip, struct aw_nfc_normal_req *req)
{
	int ret = 0;

	AWRAWNAND_TRACE_NFC("Enter %s req type@%d\n", __func__, req->type);

	switch (req->type) {
	case CMD:
		ret = aw_host_nfc_noraml_op_cmd(chip, req);
		break;
	case CMD_WITH_ADDR:
		ret = aw_host_nfc_noraml_op_cmd_with_addr(chip, req);
		break;
	case CMD_WITH_ADDR_DATA:
		ret = aw_host_nfc_noraml_op_cmd_with_addr_data(chip, req);
		break;
	default:
		awrawnand_err("don't support normal req type@%d\n", req->type);
		break;
	}

	AWRAWNAND_TRACE_NFC("Exit %s\n", __func__);

	return ret;
}


static int aw_host_nfc_dma_config_start(struct aw_nand_host *host, uint8_t rw, void *addr, unsigned int len)
{

	struct nfc_reg *nfc = &host->nfc_reg;
	uint32_t cfg = 0;
	enum dma_data_direction dir = rw ? DMA_TO_DEVICE : DMA_BIDIRECTIONAL;
	uint32_t config_addr = 0;

	/*aw_host_flush_dcache(addr, len);*/
	AWRAWNAND_TRACE_NFC("Enter %s rw@%d addr@%p len@%u\n", __func__, rw, addr, len);

	if (host->dma_type == MBUS_DMA) {
		host->dma_addr = dma_map_single(addr, len, dir);
		if (host->dma_addr & 0x3)
			awrawnand_err("dma map single addr@0x%x is not 32bits aligned\n", host->dma_addr);
		/*config use mbus dma*/
		cfg = readl(nfc->ctl);
		/*use dma*/
		cfg &= ~NFC_DMA_TYPE;
		/*use mbus dma*/
		cfg |= NFC_RAM_METHOD_DMA;
		writel(cfg, nfc->ctl);

		host->nfc_dma_desc_cpu[0].bcnt = 0;
		host->nfc_dma_desc_cpu[0].bcnt |= NFC_DESC_BSIZE(len);
		host->nfc_dma_desc_cpu[0].buff = (unsigned int)host->dma_addr;

		host->nfc_dma_desc_cpu[0].cfg = 0;
		host->nfc_dma_desc_cpu[0].cfg |= NFC_DESC_FIRST_FLAG;
		host->nfc_dma_desc_cpu[0].cfg |= NFC_DESC_LAST_FLAG;
		host->nfc_dma_desc_cpu[0].next = &host->nfc_dma_desc_cpu[0];

		config_addr = dma_map_single(&host->nfc_dma_desc_cpu[0], 1024, DMA_TO_DEVICE);
		if (host->use_dma_int)
			aw_host_nfc_dma_int_enable(&host->nfc_reg);

		writel(config_addr, nfc->mbus_dma_dlba);

	} else {
		; /*to do*/
	}

	AWRAWNAND_TRACE_NFC("Exit %s\n", __func__);
	return 0;

}

static void aw_host_rb_wake_up(void)
{
	return;
}

static void aw_host_dma_wake_up(void)
{
	return;
}

void aw_host_nfc_do_nand_interrupt(void)
{
	struct aw_nand_host *host = &aw_host;
	if (aw_host_nfc_rb_b2r_int_occur_check(&host->nfc_reg)) {
		aw_host_nfc_rb_b2r_intstatus_clear(&host->nfc_reg);
		aw_host_nfc_rb_b2r_int_disable(&host->nfc_reg);
		host->rb_ready_flag = 1;
		aw_host_rb_wake_up();
	}

	if (host->use_dma_int) {
		if (aw_host_nfc_dma_int_occur_check(&host->nfc_reg)) {
			aw_host_nfc_dma_intstatus_clear(&host->nfc_reg);
			aw_host_nfc_dma_int_disable(&host->nfc_reg);
			host->dma_ready_flag = 1;
			aw_host_dma_wake_up();
		}
	}
}


static int aw_host_nfc_dma_wait_end(struct aw_nand_host *host, uint8_t rw, void *addr, unsigned int len)
{
	int ret = 0;
	enum dma_data_direction dir = rw ? DMA_TO_DEVICE : DMA_FROM_DEVICE;

	AWRAWNAND_TRACE_NFC("Enter %s\n", __func__);
	if (host->use_dma_int) {
		if (host->dma_ready_flag == 1)
			goto dma_int_end;
		if (!aw_host_wait_dma_ready_flag_timeout(host, 10000))
			goto dma_int_end;

	}

	ret = aw_host_nfc_wait_status_timeout(&host->nfc_reg,
			NFC_DMA_INT_FLAG, NFC_DMA_INT_FLAG, 60000);
	if (ret)
		aw_nfc_reg_dump(&host->nfc_reg);

	if (host->dma_ready_flag == 1)
		host->dma_ready_flag = 0;
	else {
		aw_host_nfc_dma_intstatus_clear(&host->nfc_reg);
		aw_host_nfc_dma_int_disable(&host->nfc_reg);
	}

dma_int_end:

	dma_unmap_single((volatile void *)host->dma_addr, len, dir);

	AWRAWNAND_TRACE_NFC("Exit %s\n", __func__);
	return ret;
}

/*len is align to ecc block size(1KB)*/
static int aw_host_nfc_check_ecc_status(struct nfc_reg *nfc, uint32_t len)
{

	struct aw_nand_host *host = awnand_nfc_to_host(nfc);

	uint8_t ecc_limit = ecc_limit_tab[NFC_ECC_GET(readl(nfc->ecc_ctl))];
	uint32_t ecc_block_cnt = B_TO_KB(len);
	uint32_t ecc_block_mask = ((1 << ecc_block_cnt) - 1);
	uint32_t ecc_cnt_w[MAX_ERR_CNT];
	uint8_t ecc_cnt = 0;
	int i = 0;
	AWRAWNAND_TRACE_NFC("Enter %s len@%d\n", __func__, len);

	if (readl(nfc->ecc_sta) & ecc_block_mask) {
		awrawnand_err("status[%p:%x]\n", nfc->ecc_sta, readl(nfc->ecc_sta));
		aw_nfc_reg_dump(nfc);
		return ECC_ERR;
	}

	/*check ecc limit*/
	for (i = 0; i < MAX_ERR_CNT; i++) {
		ecc_cnt_w[i] = readl(nfc->err_cnt[i]);
	}

	for (i = 0; i < ecc_block_cnt; i++) {
		ecc_cnt = (uint8_t)(ecc_cnt_w[i >> 2] >> ((i % 4) << 3));
		if (ecc_cnt > ecc_limit) {
			AWRAWNAND_TRACE_NFC("Exit %s ret@ECC_LIMIT\n", __func__);
			host->bitflips = ecc_cnt;
			awrawnand_info("ecc limit@%d\n", ecc_cnt);
			return ECC_LIMIT;
		}
	}

	host->bitflips = ecc_cnt;
	AWRAWNAND_TRACE_NFC("Exit %s ret@ECC_GOOG\n", __func__);
	return ECC_GOOD;
}

static bool aw_host_nfc_is_blank_page(struct nfc_reg *nfc, uint32_t len)
{
	uint32_t ecc_block_cnt = B_TO_KB(len);
	uint32_t ecc_block_mask = ((1 << ecc_block_cnt) - 1);

	if ((readl(nfc->ecc_ctl) & NFC_ECC_EXCEPTION) &&
			(!(readl(nfc->ecc_sta) & ecc_block_mask)) &&
			((readl(nfc->pat_id) & ecc_block_mask) == ecc_block_mask))
		return true;
	return false;
}

static void aw_host_nfc_get_spare_data(struct nfc_reg *nfc, uint8_t *spare, uint8_t len)
{
	uint8_t cnt = (len >> 2);
	uint8_t i = 0;
	uint32_t val = 0;

	if (likely(spare)) {
		for (i = 0; i < cnt; i++) {
			val = readl(nfc->user_data_base + i);
			spare[i * 4 + 0] = ((val >> 0) & 0xff);
			spare[i * 4 + 1] = ((val >> 8) & 0xff);
			spare[i * 4 + 2] = ((val >> 16) & 0xff);
			spare[i * 4 + 3] = ((val >> 24) & 0xff);
		}
	}
}

static int aw_host_nfc_batch_op(struct aw_nand_chip *chip, struct aw_nfc_batch_req *req)
{
	struct aw_nand_host *host = awnand_chip_to_host(chip);
	struct nfc_reg *nfc = &host->nfc_reg;
	/*int rb_no = aw_host_nfc_get_selected_rb_no(nfc);*/
	uint32_t page = req->addr.page;
	uint32_t page_in_block = page & chip->pages_per_blk_mask;

	int ret = 0;
	uint32_t val = 0;
	uint8_t col1 = 0, col2 = 0, row1 = 0, row2 = 0, row3 = 0;
	uint32_t low = 0, high = 0;
	uint32_t read_set0 = 0;
	uint32_t cmd = 0;
	uint32_t ecc_block = 0, ecc_block_bitmap = 0;
	uint8_t spare[MAX_SPARE_SIZE];
	int dummy_byte = 0;

	AWRAWNAND_TRACE_NFC("Enter %s\n", __func__);

	if (req->data.type == ONLY_SPARE) {
		/*data layout interleave mode, ecc and oob store in spare area
		 * need to read main data and spare data to do ecc,though user only read spare*/
		req->data.main_len = SZ_1K;
		ecc_block = B_TO_KB(chip->pagesize);
		ecc_block_bitmap = ((1 << ecc_block) - 1);
		/*one ecc_block can attach to 32 bytes*/
		if (req->data.spare_len < 32)
			ecc_block_bitmap = 0x1;
		/*to do : ecc_block_bitmap from req.data.spare_len*/
	} else {
		ecc_block = B_TO_KB(req->data.main_len);
		ecc_block_bitmap = ((1 << ecc_block) - 1);
	}

	val = readl(nfc->sta);
	val &= (NFC_RB_B2R | NFC_CMD_INT_FLAG | NFC_DMA_INT_FLAG);
	val |= readl(nfc->sta);
	writel(val, nfc->sta);

	/*set ecc mode, randomizer,exception,pipeline*/
	if (!chip->operate_boot0) {
		aw_host_nfc_set_ecc_mode(nfc, chip->ecc_mode);
		aw_host_nfc_ecc_enable(nfc, 1);
		if (chip->random) {
			aw_host_nfc_randomize_enable(nfc, page_in_block);
		} else
			aw_host_nfc_randomize_disable(nfc);
	} else {
		aw_host_nfc_set_ecc_mode(nfc, chip->boot0_ecc_mode);
		aw_host_nfc_ecc_enable(nfc, 1);
		aw_host_nfc_randomize_enable(nfc, page);
	}

	/*configure addr*/
	row1 = (page & 0xff);
	row2 = ((page >> 8) & 0xff);
	row3 = ((page >> 16) & 0xff);

	low = (col1 | (col2 << 8) | (row1 << 16) | (row2 << 24));
	high |= row3;

	writel(low, nfc->addr_low);
	writel(high, nfc->addr_high);

	cmd |= NFC_ADR_NUM((req->addr.row_cycles + 2));
	cmd |= NFC_SEND_ADR;


	cmd |= NFC_SEND_CMD2;
	cmd |= NFC_SEND_CMD1;
	cmd |= NFC_DATA_TRANS;

	/*default:batch mode use dma*/
	if (req->data.type != ONLY_SPARE)
		cmd |= NFC_DATA_SWAP_METHOD;

	if (req->layout == SEQUENCE) {
		cmd |= NFC_SEQ;
	}

	cmd |= NFC_BATCH_OP;
	/*configure read command set*/
	if (req->type == READ) {
		/*cmd |= req->cmd.r.READ0;*/
		cmd |= req->cmd.val.first;
		cmd |= NFC_WAIT_FLAG;

		/*
		 *read_set0 = ((req->cmd.r.READSTART << 0) | (req->cmd.r.RNOUT << 8) |
		 *                (req->cmd.r.RNOUTSTART << 16));
		 */

		read_set0 = ((req->cmd.val.snd << 0) | (req->cmd.val.rnd1 << 8) |
				(req->cmd.val.rnd2 << 16));
		writel(read_set0, nfc->read_cmd_set);

		/*configure user data len*/
		if (req->data.spare_len) {
			aw_host_nfc_set_user_data_len(nfc, req->data.spare_len);
			uint8_t default_spare[req->data.spare_len];
			memset(default_spare, 0x99, req->data.spare_len);
			aw_host_nfc_set_user_data(nfc, default_spare, req->data.spare_len);
		} else {
			aw_host_nfc_set_user_data_len(nfc, chip->avalid_sparesize);
			uint8_t default_spare[chip->avalid_sparesize];
			memset(default_spare, 0x99, chip->avalid_sparesize);
			aw_host_nfc_set_user_data(nfc, default_spare, chip->avalid_sparesize);
		}

	} else {

		/*configure user data len*/
		if (req->data.spare_len) {
			memset(spare, 0xff, MAX_SPARE_SIZE);
			/*data.spare_len should be less than MAX_SPARE_SIZE or equal*/
			memcpy(spare, req->data.spare, req->data.spare_len);
			/*avalid_sparesize maximum equal to MAX_SPARE_SIZE*/
			aw_host_nfc_set_user_data(nfc, spare, req->data.spare_len);
		} else {
			aw_host_nfc_set_user_data(nfc, host->spare_default, chip->avalid_sparesize);
		}

		if (!chip->operate_boot0)
			aw_host_nfc_set_user_data_len(nfc, req->data.spare_len);
		else
			aw_host_nfc_set_boot0_user_data_len(nfc, req->data.spare_len);

		cmd |= NFC_ACCESS_DIR;
		/*cmd |= req->cmd.w.SEQIN;*/
		cmd |= req->cmd.val.first;

		/*writel((req->cmd.w.PAGEPROG | (req->cmd.w.RNDIN << 8)), nfc->write_cmd_set);*/
		writel((req->cmd.val.snd | (req->cmd.val.rnd1 << 8)), nfc->write_cmd_set);

		int real_pagesize = chip->real_pagesize;
		int ecc_mode = chip->ecc_mode;
		int w_ecc_block_cnt = B_TO_KB(chip->pagesize);
		int w_user_data_len = chip->avalid_sparesize;

		dummy_byte = aw_host_nfc_get_dummy_byte(real_pagesize, ecc_mode, w_ecc_block_cnt, w_user_data_len);
		if (dummy_byte > 0)
			aw_host_nfc_set_dummy_byte(nfc, dummy_byte);

	}

	if (aw_host_nfc_wait_cmd_fifo_empty(nfc) || aw_host_nfc_wait_fsm_idle(nfc)) {
		ret = -ETIMEDOUT;
		awrawnand_err("fifo or fsm is not empty\n");
		goto out_err;
	}


	if (req->data.type != ONLY_SPARE) {
		aw_host_nfc_dma_config_start(host, req->type, req->data.main, req->data.main_len);
		/*write command*/
		writel(ecc_block_bitmap, nfc->data_block_mask);
		writel(cmd, nfc->cmd);
		/*aw_nfc_reg_dump(nfc);*/
		ret = aw_host_nfc_dma_wait_end(host, req->type, req->data.main, req->data.main_len);
		if (ret) {
			awrawnand_err("%s wait dma end fail\n", __func__);
			goto out_err;
		}
	} else {
		writel(ecc_block_bitmap, nfc->data_block_mask);
		/*write command*/
		writel(cmd, nfc->cmd);
	}

	ret = aw_host_nfc_wait_cmd_finish(nfc);
	if (ret) {
		awrawnand_err("%s wait cmd finish fail\n", __func__);
		goto out_err;
	}

	if (host->use_rb_int) {
		/*to do*/
	} else {
		ret = aw_host_nfc_wait_rb_ready(chip, host);
		if (!ret) {
			awrawnand_err("%s wait rb ready fail\n", __func__);
			aw_nfc_reg_dump(nfc);
			ret = -ETIMEDOUT;
			goto out_err;
		}
		ret = 0;
	}

	if (req->type == READ) {
		ret = aw_host_nfc_check_ecc_status(nfc, req->data.main_len);
		if (ret == ECC_GOOD) {
			int len = req->data.main_len ? req->data.main_len : req->data.spare_len;
			len = ((len < 1024) ? 1024 : len);
			if (aw_host_nfc_is_blank_page(nfc, len)) {
				memset(req->data.main, 0xff, req->data.main_len);
				memset(req->data.spare, 0xff, req->data.spare_len);
			} else {
				aw_host_nfc_get_spare_data(nfc, spare, MAX_SPARE_SIZE);
				memcpy(req->data.spare, spare, req->data.spare_len);
			}
		} else {
			aw_host_nfc_get_spare_data(nfc, spare, MAX_SPARE_SIZE);
			memcpy(req->data.spare, spare, req->data.spare_len);
		}

		chip->bitflips = host->bitflips;

		aw_host_nfc_ecc_disable(nfc);
		if (chip->random) {
			aw_host_nfc_randomize_disable(nfc);
		}
	} else {
		aw_host_nfc_ecc_disable(nfc);
		if (chip->random) {
			aw_host_nfc_randomize_disable(nfc);
		}
		aw_host_nfc_set_dummy_byte(nfc, 0);
	}


	AWRAWNAND_TRACE_NFC("Exit %s\n", __func__);
	return ret;

out_err:
	aw_host_nfc_ecc_disable(nfc);
	if (chip->random) {
		aw_host_nfc_randomize_disable(nfc);
	}
	if (req->type == WRITE)
		aw_host_nfc_set_dummy_byte(nfc, 0);
	AWRAWNAND_TRACE_NFC("Exit %s\n", __func__);
	return ret;
}
#if 0
static int aw_host_nfc_batch_op_cache_rw(struct aw_nand_chip *chip, struct aw_nfc_batch_req *req)
{
	struct aw_nand_host *host = awnand_chip_to_host(chip);
	struct nfc_reg *nfc = &host->nfc_reg;
	/*int rb_no = aw_host_nfc_get_selected_rb_no(nfc);*/
	uint32_t page = req->addr.page;
	uint32_t page_in_block = page & chip->pages_per_blk_mask;

	int ret = 0;
	uint32_t val = 0;
	uint8_t col1 = 0, col2 = 0, row1 = 0, row2 = 0, row3 = 0;
	uint32_t low = 0, high = 0;
	uint32_t read_set0 = 0;
	uint32_t cmd = 0;
	uint32_t ecc_block = 0, ecc_block_bitmap = 0;
	uint8_t spare[MAX_SPARE_SIZE];
	int dummy_byte = 0;


	AWRAWNAND_TRACE_NFC("Enter %s\n", __func__);

	if (req->data.type == ONLY_SPARE) {
		/*data layout interleave mode, ecc and oob store in spare area
		 * need to read main data and spare data to do ecc,though user only read spare*/
		ecc_block = B_TO_KB(chip->pagesize);
		ecc_block_bitmap = ((1 << ecc_block) - 1);
		/*one ecc_block can attach to 32 bytes*/
		if (req->data.spare_len < 32)
			ecc_block_bitmap = 0x1;
		/*to do : ecc_block_bitmap from req.data.spare_len*/
	} else {
		ecc_block = B_TO_KB(req->data.main_len);
		ecc_block_bitmap = ((1 << ecc_block) - 1);
	}

	val = readl(nfc->sta);
	val &= (NFC_RB_B2R | NFC_CMD_INT_FLAG | NFC_DMA_INT_FLAG);
	val |= readl(nfc->sta);
	writel(val, nfc->sta);

	/*set ecc mode, randomizer,exception,pipeline*/
	if (!chip->operate_boot0) {
		aw_host_nfc_set_ecc_mode(nfc, chip->ecc_mode);
		aw_host_nfc_ecc_enable(nfc, 1);
		if (chip->random) {
			aw_host_nfc_randomize_enable(nfc, page_in_block);
		}
	} else {
		aw_host_nfc_set_ecc_mode(nfc, chip->boot0_ecc_mode);
		aw_host_nfc_ecc_enable(nfc, 1);
		aw_host_nfc_randomize_enable(nfc, page);
	}

	/*configure addr*/
	row1 = (page & 0xff);
	row2 = ((page >> 8) & 0xff);
	row3 = ((page >> 16) & 0xff);

	low = (col1 | (col2 << 8) | (row1 << 16) | (row2 << 24));
	high |= row3;

	writel(low, nfc->addr_low);
	writel(high, nfc->addr_high);

	cmd |= NFC_ADR_NUM((req->addr.row_cycles + 2));
	cmd |= NFC_SEND_ADR;


	cmd |= NFC_SEND_CMD2;
	cmd |= NFC_SEND_CMD1;
	cmd |= NFC_DATA_TRANS;

	/*default:batch mode use dma*/
	if (req->data.type != ONLY_SPARE)
		cmd |= NFC_DATA_SWAP_METHOD;

	if (req->layout == SEQUENCE) {
		cmd |= NFC_SEQ;
	}

	cmd |= NFC_BATCH_OP;
	/*configure read command set*/
	if (req->type == READ) {
		cmd |= req->cmd.r.READ0;
		cmd |= NFC_WAIT_FLAG;

		read_set0 = ((req->cmd.r.READSTART << 0) | (req->cmd.r.RNOUT << 8) |
				(req->cmd.r.RNOUTSTART << 16));
		writel(read_set0, nfc->read_cmd_set);

		/*configure user data len*/
		aw_host_nfc_set_user_data_len(nfc, chip->avalid_sparesize);
		uint8_t default_spare[chip->avalid_sparesize];
		memset(default_spare, 0x99, chip->avalid_sparesize);
		aw_host_nfc_set_user_data(nfc, default_spare, chip->avalid_sparesize);

	} else {

		/*configure user data len*/
		if (req->data.spare_len) {
			memset(spare, 0xff, MAX_SPARE_SIZE);
			/*data.spare_len should be less than MAX_SPARE_SIZE or equal*/
			memcpy(spare, req->data.spare_in, req->data.spare_len);
			/*avalid_sparesize maximum equal to MAX_SPARE_SIZE*/
			aw_host_nfc_set_user_data(nfc, spare, chip->avalid_sparesize);
		} else {
			aw_host_nfc_set_user_data(nfc, host->spare_default, chip->avalid_sparesize);
		}

		if (!chip->operate_boot0)
			aw_host_nfc_set_user_data_len(nfc, chip->avalid_sparesize);
		else
			aw_host_nfc_set_boot0_user_data_len(nfc, req->data.spare_len);

		cmd |= NFC_ACCESS_DIR;
		cmd |= req->cmd.cw.SEQIN;

		writel((req->cmd.cw.CACHEDPROG | (req->cmd.cw.RNDIN << 8)), nfc->write_cmd_set);

		int real_pagesize = chip->real_pagesize;
		int ecc_mode = chip->ecc_mode;
		int w_ecc_block_cnt = B_TO_KB(chip->pagesize);
		int w_user_data_len = chip->avalid_sparesize;

		dummy_byte = aw_host_nfc_get_dummy_byte(real_pagesize, ecc_mode, w_ecc_block_cnt, w_user_data_len);
		if (dummy_byte > 0)
			aw_host_nfc_set_dummy_byte(nfc, dummy_byte);

	}

	if (aw_host_nfc_wait_cmd_fifo_empty(nfc) || aw_host_nfc_wait_fsm_idle(nfc)) {
		ret = -ETIMEDOUT;
		awrawnand_err("fifo or fsm is not empty\n");
		goto out_err;
	}


	if (req->data.type != ONLY_SPARE) {
		aw_host_nfc_dma_config_start(host, req->type, req->data.main_in, req->data.main_len);
		/*write command*/
		writel(ecc_block_bitmap, nfc->data_block_mask);
		writel(cmd, nfc->cmd);
		/*printf("cmd_cfg@%x cmd reg@%x\n", cmd, readl(nfc->cmd));*/
		/*aw_nfc_reg_dump(nfc);*/
		ret = aw_host_nfc_dma_wait_end(host, req->type, req->data.main_in, req->data.main_len);
		if (ret) {
			awrawnand_err("%s wait dma end fail\n", __func__);
			goto out_err;
		}
	} else {
		writel(ecc_block_bitmap, nfc->data_block_mask);
		/*write command*/
		writel(cmd, nfc->cmd);
	}

	ret = aw_host_nfc_wait_cmd_finish(nfc);
	if (ret) {
		awrawnand_err("%s wait cmd finish fail\n", __func__);
		goto out_err;
	}

	if (host->use_rb_int) {
		/*to do*/
	} else {
		ret = aw_host_nfc_wait_rb_ready(chip, host);
		if (!ret) {
			awrawnand_err("%s wait rb ready fail\n", __func__);
			aw_nfc_reg_dump(nfc);
			ret = -ETIMEDOUT;
			goto out_err;
		}
		ret = 0;
	}

	if (req->type == READ) {
		ret = aw_host_nfc_check_ecc_status(nfc, req->data.main_len);
		if (ret == ECC_GOOD) {
			int len = req->data.main_len ? req->data.main_len : req->data.spare_len;
			len = ((len < 1024) ? 1024 : len);
			if (aw_host_nfc_is_blank_page(nfc, len)) {
				AWRAWNAND_TRACE_NFC("%s-%d page%d is blank\n", __func__, __LINE__, page);
				memset(req->data.main_in, 0xff, req->data.main_len);
				memset(req->data.spare_in, 0xff, req->data.spare_len);
			} else {
				aw_host_nfc_get_spare_data(nfc, spare, MAX_SPARE_SIZE);
				memcpy(req->data.spare_in, spare, req->data.spare_len);
			}
		} else {
			aw_host_nfc_get_spare_data(nfc, spare, MAX_SPARE_SIZE);
			memcpy(req->data.spare_in, spare, req->data.spare_len);
		}

		chip->bitflips = host->bitflips;

		aw_host_nfc_ecc_disable(nfc);
		if (chip->random) {
			aw_host_nfc_randomize_disable(nfc);
		}
	} else {
		aw_host_nfc_ecc_disable(nfc);
		if (chip->random) {
			aw_host_nfc_randomize_disable(nfc);
		}
		aw_host_nfc_set_dummy_byte(nfc, 0);
	}


	AWRAWNAND_TRACE_NFC("Exit %s\n", __func__);
	return ret;

out_err:
	aw_host_nfc_ecc_disable(nfc);
	if (chip->random) {
		aw_host_nfc_randomize_disable(nfc);
	}
	if (req->type == WRITE)
		aw_host_nfc_set_dummy_byte(nfc, 0);
	AWRAWNAND_TRACE_NFC("Exit %s\n", __func__);
	return ret;
}

static int aw_host_nfc_batch_op_mrw(struct aw_nand_chip *chip, struct aw_nfc_batch_req *req)
{
	struct aw_nand_host *host = awnand_chip_to_host(chip);
	struct nfc_reg *nfc = &host->nfc_reg;
	/*int rb_no = aw_host_nfc_get_selected_rb_no(nfc);*/
	uint32_t page = req->addr.page;
	uint32_t page_in_block = page & chip->pages_per_blk_mask;

	int ret = 0;
	uint32_t val = 0;
	uint8_t col1 = 0, col2 = 0, row1 = 0, row2 = 0, row3 = 0;
	uint32_t low = 0, high = 0;
	uint32_t read_set0 = 0;
	uint32_t cmd = 0;
	uint32_t ecc_block = 0, ecc_block_bitmap = 0;
	uint8_t spare[MAX_SPARE_SIZE];
	int dummy_byte = 0;


	AWRAWNAND_TRACE_NFC("Enter %s\n", __func__);

	if (req->data.type == ONLY_SPARE) {
		/*data layout interleave mode, ecc and oob store in spare area
		 * need to read main data and spare data to do ecc,though user only read spare*/
		ecc_block = B_TO_KB(chip->pagesize);
		ecc_block_bitmap = ((1 << ecc_block) - 1);
		/*one ecc_block can attach to 32 bytes*/
		if (req->data.spare_len < 32)
			ecc_block_bitmap = 0x1;
		/*to do : ecc_block_bitmap from req.data.spare_len*/
	} else {
		ecc_block = B_TO_KB(req->data.main_len);
		ecc_block_bitmap = ((1 << ecc_block) - 1);
	}

	val = readl(nfc->sta);
	val &= (NFC_RB_B2R | NFC_CMD_INT_FLAG | NFC_DMA_INT_FLAG);
	val |= readl(nfc->sta);
	writel(val, nfc->sta);

	/*set ecc mode, randomizer,exception,pipeline*/
	if (!chip->operate_boot0) {
		aw_host_nfc_set_ecc_mode(nfc, chip->ecc_mode);
		aw_host_nfc_ecc_enable(nfc, 1);
		if (chip->random) {
			aw_host_nfc_randomize_enable(nfc, page_in_block);
		}
	} else {
		aw_host_nfc_set_ecc_mode(nfc, chip->boot0_ecc_mode);
		aw_host_nfc_ecc_enable(nfc, 1);
		aw_host_nfc_randomize_enable(nfc, page);
	}

	/*configure addr*/
	row1 = (page & 0xff);
	row2 = ((page >> 8) & 0xff);
	row3 = ((page >> 16) & 0xff);

	low = (col1 | (col2 << 8) | (row1 << 16) | (row2 << 24));
	high |= row3;

	writel(low, nfc->addr_low);
	writel(high, nfc->addr_high);

	cmd |= NFC_ADR_NUM((req->addr.row_cycles + 2));
	cmd |= NFC_SEND_ADR;


	cmd |= NFC_SEND_CMD2;
	cmd |= NFC_SEND_CMD1;
	cmd |= NFC_DATA_TRANS;

	/*default:batch mode use dma*/
	if (req->data.type != ONLY_SPARE)
		cmd |= NFC_DATA_SWAP_METHOD;

	if (req->layout == SEQUENCE) {
		cmd |= NFC_SEQ;
	}

	cmd |= NFC_BATCH_OP;
	/*configure read command set*/
	if (req->type == READ) {
		cmd |= req->cmd.mr.READ0;
		cmd |= NFC_WAIT_FLAG;

		read_set0 = ((req->cmd.mr.MULTIREADSTART << 0) | (req->cmd.mr.RNOUT << 8) |
				(req->cmd.mr.RNOUTSTART << 16));
		writel(read_set0, nfc->read_cmd_set);

		/*configure user data len*/
		aw_host_nfc_set_user_data_len(nfc, chip->avalid_sparesize);
		uint8_t default_spare[chip->avalid_sparesize];
		memset(default_spare, 0x99, chip->avalid_sparesize);
		aw_host_nfc_set_user_data(nfc, default_spare, chip->avalid_sparesize);

	} else {

		/*configure user data len*/
		if (req->data.spare_len) {
			memset(spare, 0xff, MAX_SPARE_SIZE);
			/*data.spare_len should be less than MAX_SPARE_SIZE or equal*/
			memcpy(spare, req->data.spare_in, req->data.spare_len);
			/*avalid_sparesize maximum equal to MAX_SPARE_SIZE*/
			aw_host_nfc_set_user_data(nfc, spare, chip->avalid_sparesize);
		} else {
			aw_host_nfc_set_user_data(nfc, host->spare_default, chip->avalid_sparesize);
		}

		if (!chip->operate_boot0)
			aw_host_nfc_set_user_data_len(nfc, chip->avalid_sparesize);
		else
			aw_host_nfc_set_boot0_user_data_len(nfc, req->data.spare_len);

		cmd |= NFC_ACCESS_DIR;
		cmd |= req->cmd.mw.SEQIN;

		writel((req->cmd.mw.MULTIPROG | (req->cmd.w.RNDIN << 8)), nfc->write_cmd_set);

		int real_pagesize = chip->real_pagesize;
		int ecc_mode = chip->ecc_mode;
		int w_ecc_block_cnt = B_TO_KB(chip->pagesize);
		int w_user_data_len = chip->avalid_sparesize;

		dummy_byte = aw_host_nfc_get_dummy_byte(real_pagesize, ecc_mode, w_ecc_block_cnt, w_user_data_len);
		if (dummy_byte > 0)
			aw_host_nfc_set_dummy_byte(nfc, dummy_byte);

	}

	if (aw_host_nfc_wait_cmd_fifo_empty(nfc) || aw_host_nfc_wait_fsm_idle(nfc)) {
		ret = -ETIMEDOUT;
		awrawnand_err("fifo or fsm is not empty\n");
		goto out_err;
	}


	if (req->data.type != ONLY_SPARE) {
		aw_host_nfc_dma_config_start(host, req->type, req->data.main_in, req->data.main_len);
		/*write command*/
		writel(ecc_block_bitmap, nfc->data_block_mask);
		writel(cmd, nfc->cmd);
		/*aw_nfc_reg_dump(nfc);*/
		ret = aw_host_nfc_dma_wait_end(host, req->type, req->data.main_in, req->data.main_len);
		if (ret) {
			awrawnand_err("%s wait dma end fail\n", __func__);
			goto out_err;
		}
	} else {
		writel(ecc_block_bitmap, nfc->data_block_mask);
		/*write command*/
		writel(cmd, nfc->cmd);
	}

	ret = aw_host_nfc_wait_cmd_finish(nfc);
	if (ret) {
		awrawnand_err("%s wait cmd finish fail\n", __func__);
		goto out_err;
	}

	if (host->use_rb_int) {
		/*to do*/
	} else {
		ret = aw_host_nfc_wait_rb_ready(chip, host);
		if (!ret) {
			awrawnand_err("%s wait rb ready fail\n", __func__);
			aw_nfc_reg_dump(nfc);
			ret = -ETIMEDOUT;
			goto out_err;
		}
		ret = 0;
	}

	if (req->type == READ) {
		ret = aw_host_nfc_check_ecc_status(nfc, req->data.main_len);
		if (ret == ECC_GOOD) {
			int len = req->data.main_len ? req->data.main_len : req->data.spare_len;
			len = ((len < 1024) ? 1024 : len);
			if (aw_host_nfc_is_blank_page(nfc, len)) {
				AWRAWNAND_TRACE_NFC("%s-%d page%d is blank\n", __func__, __LINE__, page);
				memset(req->data.main_in, 0xff, req->data.main_len);
				memset(req->data.spare_in, 0xff, req->data.spare_len);
			} else {
				aw_host_nfc_get_spare_data(nfc, spare, MAX_SPARE_SIZE);
				memcpy(req->data.spare_in, spare, req->data.spare_len);
			}
		} else {
			aw_host_nfc_get_spare_data(nfc, spare, MAX_SPARE_SIZE);
			memcpy(req->data.spare_in, spare, req->data.spare_len);
		}

		chip->bitflips = host->bitflips;

		aw_host_nfc_ecc_disable(nfc);
		if (chip->random) {
			aw_host_nfc_randomize_disable(nfc);
		}
	} else {
		aw_host_nfc_ecc_disable(nfc);
		if (chip->random) {
			aw_host_nfc_randomize_disable(nfc);
		}
		aw_host_nfc_set_dummy_byte(nfc, 0);
	}


	AWRAWNAND_TRACE_NFC("Exit %s\n", __func__);
	return ret;

out_err:
	aw_host_nfc_ecc_disable(nfc);
	if (chip->random) {
		aw_host_nfc_randomize_disable(nfc);
	}
	if (req->type == WRITE)
		aw_host_nfc_set_dummy_byte(nfc, 0);
	AWRAWNAND_TRACE_NFC("Exit %s\n", __func__);
	return ret;
}
#endif

static int aw_host_set_mdclk(struct aw_nand_host *host, uint32_t mdclk)
{

	uint32_t mdclk_val = 0;
	uint32_t mdclk_src = 0;
	uint32_t div_m = 0;
	uint32_t div_n = 0;
	uint32_t val = 0;
	int err = 0;

	if (mdclk == 0) {
		val = readl(host->mdclk);
		val &= (~(CCM_NAND_CTRL_ENABLE));
		writel(val, host->mdclk);
		goto out;
	}
	/*mdclk double, then ndfc division 2(default)*/
	mdclk_val = mdclk << 1;

	switch (host->mdclk_src) {
	case OSC24M:
		mdclk_src = 24;
		break;
	case PLL_PERI0_1X:
		mdclk_src = clock_get_pll6();
		break;
	case PLL_PERI0_2X:
		mdclk_src = (clock_get_pll6() << 1);
		break;
	default:
		awrawnand_warn("don't support the mdclk src:%d\n", host->mdclk_src);
		err = -EINVAL;
		break;
	}

	div_m = mdclk_src / mdclk_val;
	if (mdclk_src % mdclk_val)
		div_m++;

	div_n = 0;
	while (div_m > 16) {
		div_n++;
		div_m = (div_m + 1) / 2;
	}

	if (div_n > 3) {
		awrawnand_err("clock can't div factor m:%d n:%d\n", div_m, div_n);
		err = -EINVAL;
	}

	/*close clock*/
	val = readl(host->mdclk);
	val &= ~(CCM_NAND_CTRL_ENABLE);
	writel(val, host->mdclk);

	/*config clk src select*/
	val = readl(host->mdclk);
	val &= (~(CCM_NAND_SRC_SELECT));
	val |= CCM_NAND_SRC_CSELECT(host->mdclk_src);

	val &= ~(CCM_NAND_CTRL_N);
	val |= CCM_NAND_CTRL_CN(div_n);

	val &= ~(CCM_NAND_CTRL_M);
	val |= CCM_NAND_CTRL_CM(div_m);

	val |= CCM_NAND_CTRL_ENABLE;

	writel(val, host->mdclk);

out:
	return err;
}

static int aw_host_set_mcclk(struct aw_nand_host *host, uint32_t mcclk)
{

	uint32_t mcclk_val = 0;
	uint32_t mcclk_src = 0;
	uint32_t div_m = 0;
	uint32_t div_n = 0;
	uint32_t val = 0;
	int err = 0;

	mcclk_val = mcclk;

	if (mcclk_val == 0) {
		val = readl(host->mcclk);
		val &= (~(CCM_NAND_CTRL_ENABLE));
		writel(val, host->mcclk);
		goto out;
	}


	switch (host->mcclk_src) {
	case OSC24M:
		mcclk_src = 24;
		break;
	case PLL_PERI0_1X:
		mcclk_src = clock_get_pll6();
		break;
	case PLL_PERI0_2X:
		mcclk_src = (clock_get_pll6() << 1);
		break;
	default:
		awrawnand_warn("don't support the mdclk src:%d\n", host->mcclk_src);
		err = -EINVAL;
		break;
	}

	div_m = mcclk_src / mcclk_val;
	if (mcclk_src % mcclk_val)
		div_m++;

	div_n = 0;
	while (div_m > 16) {
		div_n++;
		div_m = (div_m + 1) / 2;
	}

	if (div_n > 3) {
		awrawnand_err("clock can't div factor m:%d n:%d\n", div_m, div_n);
		err = -EINVAL;
	}

	/*close clock*/
	val = readl(host->mcclk);
	val &= ~(CCM_NAND_CTRL_ENABLE);
	writel(val, host->mcclk);

	/*config clk src select*/
	val = readl(host->mcclk);
	val &= (~(CCM_NAND_SRC_SELECT));
	val |= CCM_NAND_SRC_CSELECT(host->mcclk_src);

	val &= ~(CCM_NAND_CTRL_N);
	val |= CCM_NAND_CTRL_CN(div_n);

	val &= ~(CCM_NAND_CTRL_M);
	val |= CCM_NAND_CTRL_CM(div_m);

	val |= CCM_NAND_CTRL_ENABLE;

	writel(val, host->mcclk);

out:

	return err;
}

static int aw_host_set_clk(struct aw_nand_host *host, uint32_t mdclk,
		uint32_t mcclk)
{
	int ret = 0;
	uint32_t val = 0;

	AWRAWNAND_TRACE_NFC("Enter %s [%d:%d]\n", __func__, mdclk, mcclk);


	if (host->clk_rate == mdclk)
		goto out;

	if (!host->init) {
		val = readl(host->bus_gate);
		val &= (~(0x1U << 16));
		val |= (0x1U << 16);
		writel(val, host->bus_gate);
		/* ahb clock bus_gate */
		val = readl(host->bus_gate);
		val &= (~(0x1U << 0));
		val |= (0x1U << 0);
		writel(val, host->bus_gate);

		/* enable nand mbus gate */
		val = readl(host->mbus_gate);
		val &= (~(0x1U << 5));
		val |= (0x1U << 5);
		writel(val, host->mbus_gate);
	}

	ret = aw_host_set_mdclk(host, mdclk);
	if (ret) {
		awrawnand_err("set mdclk fail\n");
		goto out;
	}

	ret = aw_host_set_mcclk(host, mcclk);
	if (ret) {
		awrawnand_err("set mcclk fail\n");
		goto out;
	}

	host->clk_rate = mdclk;

	printf("awrawnand(mtd):bus_gate:%08x mbus_gate:%08x\n",
			readl(host->bus_gate), readl(host->mbus_gate));
	printf("awrawnand(mtd):mdclk:%08x mcclk:%08x\n",
			readl(host->mdclk), readl(host->mcclk));
out:
	AWRAWNAND_TRACE_NFC("Exit %s\n", __func__);
	return ret;
}

static int aw_host_nfc_init(struct nfc_reg *nfc)
{
	int ret = 0;

	AWRAWNAND_TRACE_NFC("Enter %s\n", __func__);
	ret = aw_host_nfc_reset(nfc);
	if (ret) {
		awrawnand_err("aw nfc reset fail@%d\n", ret);
		goto out;
	}
	aw_host_nfc_ctl_init(nfc);
	aw_host_nfc_spare_area_init(nfc);
	aw_host_nfc_efr_init(nfc);

	aw_host_nfc_randomize_disable(nfc);
	aw_host_nfc_timing_init(nfc);

	AWRAWNAND_TRACE_NFC("Exit %s\n", __func__);
out:
	return ret;
}

static inline int aw_host_resource_init(struct aw_nand_host *host)
{
	int ret = 0;
	host->bus_gate = (void __iomem *)(SUNXI_CCM_BASE + NAND_GATE);
	host->mdclk_src = PLL_PERI0_2X;
	host->mdclk = (void __iomem *)(SUNXI_CCM_BASE + NAND0_CFG);
	host->mcclk_src = PLL_PERI0_2X;
	host->mcclk = (void __iomem *)SUNXI_CCM_BASE + NAND1_CFG;
	host->mbus_gate = (void __iomem *)SUNXI_CCM_BASE + MBUS_GATE;
	host->mdclk_val = 10;
	host->mcclk_val = 20;
	host->dma_type = MBUS_DMA;
	host->use_dma_int = 0;
	host->spare_default = kzalloc(MAX_SPARE_SIZE, GFP_KERNEL);
	if (!host->spare_default) {
		awrawnand_err("kzalloc spare default fail\n");
		goto out;
	}

	memset(host->spare_default, 0xff, MAX_SPARE_SIZE);

	host->nfc_dma_desc_cpu = malloc_align(
			sizeof(struct aw_nfc_dma_desc) * NFC_DMA_DESC_MAX_NUM, 64);
	if (host->nfc_dma_desc_cpu == NULL) {
		awrawnand_err("malloc for dma desc fail\n");
		ret = -ENOMEM;
		kfree(host->spare_default);
		goto out;
	}

	host->nfc_dma_desc = host->nfc_dma_desc_cpu;

	host->normal_op = aw_host_nfc_normal_op;
	host->batch_op = aw_host_nfc_batch_op;
	host->rb_ready = aw_host_nfc_rb_ready;

	aw_nfc_reg_prepare(&host->nfc_reg);

	AWRAWNAND_TRACE_NFC("bus_gate:%08x\n", host->bus_gate);
	AWRAWNAND_TRACE_NFC("mbus_gate:%08x\n", host->mbus_gate);
	AWRAWNAND_TRACE_NFC("mdclk:%08x\n", host->mdclk);
	AWRAWNAND_TRACE_NFC("mcclk:%08x\n", host->mcclk);
out:
	return ret;
}

static inline void aw_host_resource_destroy(struct aw_nand_host *host)
{
	if (host->spare_default)
		kfree(host->spare_default);

	if (host->nfc_dma_desc_cpu)
		kfree(host->nfc_dma_desc_cpu);


	host->nfc_dma_desc = NULL;

}

static int aw_nfc_pagesize_set(struct nfc_reg *nfc, int pagesize)
{
	int ret = 0;
	uint32_t cfg = 0;
	AWRAWNAND_TRACE_NFC("Enter %s pagesize@%d\n", __func__, pagesize);

	cfg = readl(nfc->ctl);

	cfg &= ~NFC_PAGE_SHIFT_MSK;

	switch (B_TO_KB(pagesize)) {
	case 1:
		cfg |= NFC_PAGE_SIZE_1KB;
		break;
	case 2:
		cfg |= NFC_PAGE_SIZE_2KB;
		break;
	case 4:
		cfg |= NFC_PAGE_SIZE_4KB;
		break;
	case 8:
		cfg |= NFC_PAGE_SIZE_8KB;
		break;
	case 16:
		cfg |= NFC_PAGE_SIZE_16KB;
		break;
	case 32:
		cfg |= NFC_PAGE_SIZE_32KB;
		break;
	default:
		awrawnand_err("Don't support pagesize@%d\n", pagesize);
		ret = -EINVAL;
	}

	writel(cfg, nfc->ctl);
	writel(pagesize, nfc->spare_area);

	AWRAWNAND_TRACE_NFC("Exit %s ctl[%p:%x] spare_area[%p:%x]\n", __func__,
			nfc->ctl, readl(nfc->ctl), nfc->spare_area, readl(nfc->spare_area));
	return ret;
}

static void aw_nfc_ecc_mode_set(struct nfc_reg *nfc, int ecc_mode)
{
	uint32_t cfg = 0;
	AWRAWNAND_TRACE_NFC("Enter %s ecc_mode@%d\n", __func__, ecc_mode);

	cfg = readl(nfc->ecc_ctl);
	cfg &= ~NFC_ECC_MODE_MSK;
	cfg |= ecc_mode;
	writel(cfg, nfc->ecc_ctl);
	AWRAWNAND_TRACE_NFC("Exit %s ecc ctl[%p:%x]\n", __func__, nfc->ecc_ctl, readl(nfc->ecc_ctl));

}

static void aw_nfc_set_interface(struct nfc_reg *nfc, enum rawnand_data_interface_type type,
	uint32_t timing_ctl, uint32_t timing_cfg)
{
	uint32_t cfg = 0;
	AWRAWNAND_TRACE_NFC("Enter %s type@%x timing_ctl@%x timing_cfg@%x\n", __func__,
		type, timing_ctl, timing_cfg);

	/*set ctl interface*/
	cfg = readl(nfc->ctl);
	cfg &= ~NFC_DATA_INTERFACE_TYPE_MSK;
	cfg |= (type & 0x3);
	writel(cfg, nfc->ctl);

	writel(timing_ctl, nfc->timing_ctl);
	writel(timing_cfg, nfc->timing_cfg);
	AWRAWNAND_TRACE_NFC("Exit %s ctl[%p: %x] timing_ctl[%p: %x] timing_cfg[%p: %x]\n",
		__func__, nfc->ctl, nfc->timing_ctl, nfc->timing_cfg);
}

static int aw_host_set_clk_itf_sdr(struct aw_nand_host *host)
{
	struct aw_nand_chip *chip = awnand_host_to_chip(host);
	struct nfc_reg *nfc = &host->nfc_reg;
	uint32_t mdclk = chip->clk_rate;
	uint32_t mcclk = mdclk * 2;
	AWRAWNAND_TRACE_NFC("Enter %s mdclk@%d mcclk@%d\n", __func__, mdclk, mcclk);

	uint32_t cfg = 0;
	int ret = 0;

	/*set NAND FLASH Type*/
	cfg = readl(nfc->ctl);
	cfg &= ~NFC_DATA_INTERFACE_TYPE_MSK;
	cfg |= NFC_DATA_INTERFACE_TYPE_SDR;
	writel(cfg, nfc->ctl);

	/*set READ PIPE to EDO*/
	cfg = readl(nfc->timing_ctl);
	cfg &= ~NFC_TIMING_CTL_PIPE_MSK;
	cfg |= NFC_TIMING_SDR_EDO;
	writel(cfg, nfc->timing_ctl);

	ret = aw_host_set_clk(host, mdclk, mcclk);

	return ret;
}

static int aw_host_set_interface(struct aw_nand_host *host)
{
	int ret = 0;
	struct aw_nand_chip *chip = awnand_host_to_chip(host);
	struct mtd_info *mtd = awnand_chip_to_mtd(chip);

	int c = 0;
	AWRAWNAND_TRACE_NFC("Enter %s\n", __func__);

	if (RAWNAND_HAS_ONLY_TOGGLE(chip)) {
		/*init toggle ddr interface with classic clock cfg(20MHz)*/
		if (NAND_DATA_ITF_TYPE_TOGGLE_DDR1_2(chip)) {
			ret = aw_host_set_clk(host, 20, 20*2);
			if (ret)
				goto out;
			host->nf_type = RAWNAND_TOGGLE_DDR;
			host->timing_ctl = NFC_TIMING_DDR_PIPE_SEL(0x2);
			host->timing_ctl |= NFC_TIMING_DC_SEL(0x1f);

			/*1. default value : 0x95
			 *2. bit16 tCCS=1 for micron l85a, nvddr-100mHZ*/
			host->timing_cfg = 0x10095;

			 aw_nfc_set_interface(&host->nfc_reg, host->nf_type, host->timing_ctl, host->timing_cfg);
		}
	}

	if (RAWNAND_NEED_CHANGE_TO_SDR(chip)) {
		int feature_addr = TOGGLE_INTERFACE_CHANGE_ADDR;
		uint8_t p[4] = {1, 0, 0, 0};
		for (c = 0; c < chip->chips; c++) {
			chip->select_chip(mtd, c);
			ret = chip->data_interface.set_feature(chip, feature_addr, p);
			chip->select_chip(mtd, -1);
		}
	}

	if (RAWNAND_HAS_ITF_SDR(chip)) {
		ret = aw_host_set_clk_itf_sdr(host);

	}
	if (RAWNAND_HAS_ITF_ONFI_DDR(chip)) {
		/*to do*/
	}

	if (RAWNAND_HAS_ITF_TOGGLE_DDR(chip)) {
		/*to do*/
	}

out:
	AWRAWNAND_TRACE_NFC("Exit %s\n", __func__);
	return ret;
}

int aw_host_init_tail(struct aw_nand_host *host)
{

	int ret = 0;
	struct aw_nand_chip *chip = awnand_host_to_chip(host);
	int pagesize = 1 << chip->pagesize_shift;

	/*update pagesize*/
	aw_nfc_pagesize_set(&host->nfc_reg, pagesize);
	/*update ecc mode*/
	aw_nfc_ecc_mode_set(&host->nfc_reg, chip->ecc_mode);

	ret = aw_host_set_interface(host);

	return ret;
}

int aw_host_init(struct udevice *dev)
{
	struct aw_nand_host *host = get_host();
	int ret = 0;

	memset(&aw_host, 0, sizeof(struct aw_nand_host));
	host->priv = &awnand_chip;
	host->init = false;
	host->dev = dev;

	ret = aw_host_resource_init(host);
	if (ret)
		goto out;

	/*set pin*/
	ret = aw_host_set_pin(host);
	if (ret) {
		awrawnand_err("set pin fail err@%d\n", ret);
		goto out;
	}

	/*set clock*/
	ret = aw_host_set_clk(host, host->mdclk_val, host->mcclk_val);
	if (ret) {
		awrawnand_err("set clk fail err@%d\n", ret);
		goto out;
	}


	ret = aw_host_nfc_init(&host->nfc_reg);
	if (ret) {
		awrawnand_err("host nfc fail err@%d\n", ret);
		goto out;
	}

	host->init = true;
	return ret;

out:
	pr_err("%s-%d err@%d fail\n", __func__, __LINE__, ret);
	memset(&aw_host, 0, sizeof(struct aw_nand_host));
	return ret;
}
EXPORT_SYMBOL_GPL(aw_host_init);

void aw_host_exit(struct aw_nand_host *host)
{
	aw_host_resource_destroy(host);
}
EXPORT_SYMBOL_GPL(aw_host_exit);
