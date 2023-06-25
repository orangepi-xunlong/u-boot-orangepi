/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <malloc.h>
#include <asm/arch/dma.h>
#include <asm/arch/gic.h>
#include <asm/arch/clock.h>
#include <asm/io.h>

#ifdef CONFIG_MACH_SUN8IW18
#define SUNXI_DMA_MAX     10
#else
#define SUNXI_DMA_MAX     4
#endif

static int dma_int_cnt = 0;
static int dma_init = -1;
static sunxi_dma_source   dma_channal_source[SUNXI_DMA_MAX];


static void sunxi_dma_reg_func(void *p)
{
	int i;
	uint pending;
	sunxi_dma_reg *dma_reg = (sunxi_dma_reg *)SUNXI_DMA_BASE;

	for (i = 0; i < 8 && i < SUNXI_DMA_MAX; i++) {
		pending = (DMA_PKG_END_INT << (i * 4));
		if (readl(&dma_reg->irq_pending0) & pending) {
			writel(pending, &dma_reg->irq_pending0);
			if (dma_channal_source[i].dma_func.m_func != NULL)
				dma_channal_source[i].dma_func.m_func(dma_channal_source[i].dma_func.m_data);
		}
	}
	for (i = 8; i < SUNXI_DMA_MAX; i++) {
		pending = (DMA_PKG_END_INT << ((i - 8) * 4));
		if (readl(&dma_reg->irq_pending1) & pending) {
			writel(pending, &dma_reg->irq_pending1);
			if (dma_channal_source[i].dma_func.m_func != NULL)
				dma_channal_source[i].dma_func.m_func(dma_channal_source[i].dma_func.m_data);
		}
	}
}

void sunxi_dma_init(void)
{
	int i;
	sunxi_dma_reg *const dma_reg = (sunxi_dma_reg *)SUNXI_DMA_BASE;
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

	if (dma_init > 0)
		return ;

#if defined(CONFIG_SUNXI_VERSION1)
	setbits_le32(&ccm->ahb_gate0, 1 << AHB_GATE_OFFSET_DMA);
	setbits_le32(&ccm->ahb_reset0_cfg, 1 << AHB_GATE_OFFSET_DMA);
#else
	/* dma : mbus clock gating */
	setbits_le32(&ccm->mbus_gate, 1 << 0);

	/* dma reset */
	setbits_le32(&ccm->dma_gate_reset, 1 << DMA_RST_OFS);

	/* dma gating */
	setbits_le32(&ccm->dma_gate_reset, 1 << DMA_GATING_OFS);
#endif

	writel(0, &dma_reg->irq_en0);
	writel(0, &dma_reg->irq_en0);

	writel(0xffffffff, &dma_reg->irq_pending0);
	writel(0xffffffff, &dma_reg->irq_pending1);

	/*auto MCLK gating  disable*/
	clrsetbits_le32(&dma_reg->auto_gate, 0x7 << 0, 0x7 << 0);

	memset((void *)dma_channal_source, 0, SUNXI_DMA_MAX * sizeof(struct sunxi_dma_source_t));

	for (i = 0; i < SUNXI_DMA_MAX; i++) {
		dma_channal_source[i].used = 0;
		dma_channal_source[i].channal = &(dma_reg->channal[i]);
		dma_channal_source[i].desc  =
			(sunxi_dma_desc *)malloc_align(sizeof(sunxi_dma_desc), CONFIG_SYS_CACHELINE_SIZE);
	}

	irq_install_handler(AW_IRQ_DMA, sunxi_dma_reg_func, 0);

	dma_int_cnt = 0;
	dma_init = 1;

	return ;
}

void sunxi_dma_exit(void)
{
	int i;
	ulong hdma;
	sunxi_dma_reg *dma_reg = (sunxi_dma_reg *)SUNXI_DMA_BASE;
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

	/* free dma channal if other module not free it */
	for (i = 0; i < SUNXI_DMA_MAX; i++) {
		if (dma_channal_source[i].used == 1) {
			hdma = (ulong)&dma_channal_source[i];
			sunxi_dma_disable_int(hdma);
			sunxi_dma_free_int(hdma);
			writel(0, &dma_channal_source[i].channal->enable);
			dma_channal_source[i].used   = 0;
		}
	}
	/* irq disable */
	writel(0, &dma_reg->irq_en0);
	writel(0, &dma_reg->irq_en0);

	writel(0xffffffff, &dma_reg->irq_pending0);
	writel(0xffffffff, &dma_reg->irq_pending1);

	irq_free_handler(AW_IRQ_DMA);

#if defined(CONFIG_SUNXI_VERSION1)
	clrbits_le32(&ccm->ahb_gate0, 1 << AHB_GATE_OFFSET_DMA);
#else
	/* close dma clock when dma exit */
	clrbits_le32(&ccm->dma_gate_reset, 1 << DMA_GATING_OFS | 1 << DMA_RST_OFS);
#endif

	dma_init--;

}

ulong sunxi_dma_request_from_last(uint dmatype)
{
	int   i;

	for (i = SUNXI_DMA_MAX - 1; i >= 0; i--) {
		if (dma_channal_source[i].used == 0) {
			dma_channal_source[i].used = 1;
			dma_channal_source[i].channal_count = i;
			return (ulong)&dma_channal_source[i];
		}
	}

	return 0;
}

ulong sunxi_dma_request(uint dmatype)
{
	int   i;

	for (i = 0; i < SUNXI_DMA_MAX; i++) {
		if (dma_channal_source[i].used == 0) {
			dma_channal_source[i].used = 1;
			dma_channal_source[i].channal_count = i;
			return (ulong)&dma_channal_source[i];
		}
	}

	return 0;
}

int sunxi_dma_release(ulong hdma)
{
	struct sunxi_dma_source_t  *dma_source = (struct sunxi_dma_source_t *)hdma;

	if (!dma_source->used)
		return -1;

	sunxi_dma_disable_int(hdma);
	sunxi_dma_free_int(hdma);

	dma_source->used   = 0;

	return 0;
}


int sunxi_dma_setting(ulong hdma, sunxi_dma_set *cfg)
{

	uint   commit_para;
	sunxi_dma_set     *dma_set = cfg;
	sunxi_dma_source  *dma_source = (sunxi_dma_source *)hdma;
	sunxi_dma_desc    *desc = dma_source->desc;
	uint channal_addr  = (ulong)(&(dma_set->channal_cfg));

	if (!dma_source->used)
		return -1;

	if (dma_set->loop_mode)
		desc->link = (ulong)(&dma_source->desc);
	else
		desc->link = SUNXI_DMA_LINK_NULL;

	commit_para  = (dma_set->wait_cyc & 0xff);
	commit_para |= (dma_set->data_block_size & 0xff) << 8;

	writel(commit_para, &desc->commit_para);
	writel(readl((volatile void __iomem *)(ulong)channal_addr), &desc->config);

	return 0;
}


int sunxi_dma_start(ulong hdma, uint saddr, uint daddr, uint bytes)
{
	sunxi_dma_source  	  *dma_source = (sunxi_dma_source *)hdma;
	sunxi_dma_channal_reg *channal = dma_source->channal;
	sunxi_dma_desc    *desc = dma_source->desc;

	if (!dma_source->used)
		return -1;

	/*config desc */
	writel(saddr, &desc->source_addr);
	writel(daddr, &desc->dest_addr);
	writel(bytes, &desc->byte_count);

	flush_cache((ulong)desc,
		    ALIGN(sizeof(sunxi_dma_desc), CONFIG_SYS_CACHELINE_SIZE));

	/* start dma */
	writel((ulong)(desc), &channal->desc_addr);
	writel(1, &channal->enable);

	return 0;
}

int sunxi_dma_stop(ulong hdma)
{
	sunxi_dma_source *dma_source = (sunxi_dma_source *)hdma;
	sunxi_dma_channal_reg *channal = dma_source->channal;

	if (!dma_source->used)
		return -1;
	writel(0, &channal->enable);

	return 0;
}

int sunxi_dma_querystatus(ulong hdma)
{
	uint  channal_count;
	sunxi_dma_source *dma_source = (sunxi_dma_source *)hdma;
	sunxi_dma_reg    *dma_reg = (sunxi_dma_reg *)SUNXI_DMA_BASE;

	if (!dma_source->used)
		return -1;

	channal_count = dma_source->channal_count;

	return (readl(&dma_reg->status) >> channal_count) & 0x01;
}

int sunxi_dma_install_int(ulong hdma, interrupt_handler_t dma_reg_func, void *p)
{
	sunxi_dma_source     *dma_channal = (sunxi_dma_source *)hdma;
	sunxi_dma_reg    *dma_reg  = (sunxi_dma_reg *)SUNXI_DMA_BASE;
	uint  channal_count;

	if (!dma_channal->used)
		return -1;

	channal_count = dma_channal->channal_count;

	if (channal_count < 8)
		writel((7 << channal_count * 4), &dma_reg->irq_pending0);
	else
		writel((7 << (channal_count - 8) * 4), &dma_reg->irq_pending1);

	if (!dma_channal->dma_func.m_func) {
		dma_channal->dma_func.m_func = dma_reg_func;
		dma_channal->dma_func.m_data = p;
	} else {
		printf("dma 0x%lx int is used already, you have to free it first\n", hdma);
	}

	return 0;
}

int sunxi_dma_enable_int(ulong hdma)
{
	sunxi_dma_source     *dma_channal = (sunxi_dma_source *)hdma;
	sunxi_dma_reg    *dma_status  = (sunxi_dma_reg *)SUNXI_DMA_BASE;
	uint  channal_count;

	if (!dma_channal->used)
		return -1;

	channal_count = dma_channal->channal_count;
	if (channal_count < 8) {
		if (readl(&dma_status->irq_en0) & (DMA_PKG_END_INT << channal_count * 4)) {
			printf("dma 0x%lx int is avaible already\n", hdma);
			return 0;
		}
		setbits_le32(&dma_status->irq_en0, (DMA_PKG_END_INT << channal_count * 4));
	} else {
		if (readl(&dma_status->irq_en1) & (DMA_PKG_END_INT << (channal_count - 8) * 4)) {
			printf("dma 0x%lx int is avaible already\n", hdma);
			return 0;
		}
		setbits_le32(&dma_status->irq_en1, (DMA_PKG_END_INT << (channal_count - 8) * 4));
	}

	if (!dma_int_cnt)
		irq_enable(AW_IRQ_DMA);

	dma_int_cnt ++;

	return 0;
}

int sunxi_dma_disable_int(ulong hdma)
{
	sunxi_dma_source     *dma_channal = (sunxi_dma_source *)hdma;
	sunxi_dma_reg    *dma_reg  = (sunxi_dma_reg *)SUNXI_DMA_BASE;
	uint  channal_count;

	if (!dma_channal->used)
		return -1;

	channal_count = dma_channal->channal_count;
	if (channal_count < 8) {
		if (!(readl(&dma_reg->irq_en0) & (DMA_PKG_END_INT << channal_count * 4))) {
			debug("dma 0x%lx int is not used yet\n", hdma);
			return 0;
		}
		clrbits_le32(&dma_reg->irq_en0, (DMA_PKG_END_INT << channal_count * 4));
	} else {
		if (!(readl((volatile void __iomem *)(ulong)dma_reg->irq_en1) & (DMA_PKG_END_INT << (channal_count - 8) * 4))) {
			debug("dma 0x%lx int is not used yet\n", hdma);
			return 0;
		}
		clrbits_le32(&dma_reg->irq_en1, (DMA_PKG_END_INT << (channal_count - 8) * 4));
	}

	//disable golbal int
	if (dma_int_cnt > 0)
		dma_int_cnt --;

	if (!dma_int_cnt)
		irq_disable(AW_IRQ_DMA);

	return 0;
}

int sunxi_dma_free_int(ulong hdma)
{
	sunxi_dma_source     *dma_channal = (sunxi_dma_source *)hdma;
	sunxi_dma_reg    *dma_status  = (sunxi_dma_reg *)SUNXI_DMA_BASE;
	uint  channal_count;

	if (!dma_channal->used)
		return -1;

	channal_count = dma_channal->channal_count;
	if (channal_count < 8)
		writel((7 << channal_count), &dma_status->irq_pending0);
	else
		writel((7 << (channal_count - 8)), &dma_status->irq_pending1);

	if (dma_channal->dma_func.m_func) {
		dma_channal->dma_func.m_func = NULL;
		dma_channal->dma_func.m_data = NULL;
	} else {
		debug("dma 0x%lx int is free, you do not need to free it again\n", hdma);
		return -1;
	}

	return 0;
}

