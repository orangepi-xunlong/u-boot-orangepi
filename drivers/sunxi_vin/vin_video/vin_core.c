/*
 * vin_core.c for video manage
 *
 * Copyright (c) 2017 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  Zhao Wei <zhaowei@allwinnertech.com>
 *           Yang Feng <yangfeng@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "../vin.h"
#include "vin_core.h"
#include "../vin_cci/cci_helper.h"
#include "../modules/sensor/camera.h"
#include "../utility/vin_io.h"
#include "../vin_csi/sunxi_csi.h"
#include "../vin_mipi/sunxi_mipi.h"

#ifdef DMA_USE_IRQ_BUFFER_QUEUE
extern struct vin_buffer buff[BUF_NUM];
#else
extern struct vin_buffer buff;
#endif

extern struct vin_core *vinc;
extern struct sensor_format_struct sensor_formats[];

#ifdef DMA_USE_IRQ_BUFFER_QUEUE
static int vin_isr(int irq, void *priv)
{
	struct dma_int_status status;
	struct vin_buffer *buf;

	csic_dma_int_get_status(vinc->vipp_sel, &status);

	/* exception handle */
	if ((status.buf_0_overflow) || (status.buf_1_overflow) ||
	    (status.buf_2_overflow) || (status.hblank_overflow)) {
		if ((status.buf_0_overflow) || (status.buf_1_overflow) ||
		    (status.buf_2_overflow)) {
			csic_dma_int_clear_status(vinc->vipp_sel,
						 DMA_INT_BUF_0_OVERFLOW |
						 DMA_INT_BUF_1_OVERFLOW |
						 DMA_INT_BUF_2_OVERFLOW);
			vin_err("video%d fifo overflow\n", vinc->id);
		}
		if (status.hblank_overflow) {
			csic_dma_int_clear_status(vinc->vipp_sel,
						 DMA_INT_HBLANK_OVERFLOW);
			vin_err("video%d hblank overflow\n", vinc->id);
		}
		vin_log(VIN_LOG_VIDEO, "reset csi dma%d module\n", vinc->id);
		if (sensor_formats->fourcc == V4L2_PIX_FMT_FBC) {
			csic_fbc_disable(vinc->vipp_sel);
			csic_fbc_enable(vinc->vipp_sel);
		} else {
			csic_dma_disable(vinc->vipp_sel);
			csic_dma_enable(vinc->vipp_sel);
		}
	}

	if (status.fbc_ovhd_wrddr_full) {
		csic_dma_int_clear_status(vinc->vipp_sel, DMA_INT_FBC_OVHD_WRDDR_FULL);
		/*when open isp debug please ignore fbc error!*/
		if (!vinc->isp_dbg.debug_en)
			vin_err("video%d fbc overhead write ddr full\n", vinc->id);
	}

	if (status.fbc_data_wrddr_full) {
		csic_dma_int_clear_status(vinc->vipp_sel, DMA_INT_FBC_DATA_WRDDR_FULL);
		vin_err("video%d fbc data write ddr full\n", vinc->id);
	}

	if (status.vsync_trig) {
		csic_dma_int_clear_status(vinc->vipp_sel, DMA_INT_VSYNC_TRIG);

		if (vinc->first_frame) {
			buf = list_entry(vinc->vidq_active.next, struct vin_buffer, list);
			disp_set_addr(sensor_formats->o_width, sensor_formats->o_height, (unsigned int)buf->phy_addr);
			list_del(&buf->list);
			list_add_tail(&buf->list, &vinc->vidq_active);
		}

		if (vinc->first_frame == 0) {
			vin_print("\n***************First Frame!***************\n");
			vinc->first_frame = 1;
		}

		buf = list_entry(vinc->vidq_active.next->next, struct vin_buffer, list);
		vin_set_addr((unsigned int)buf->phy_addr);
	}

	if (status.frame_done)
		csic_dma_int_clear_status(vinc->vipp_sel, DMA_INT_FRAME_DONE);

	return 0;
}

static int vin_irq_request(void)
{
	vin_sys_register_irq(vinc->irq, 0, vin_isr, (void *)vinc, 0, 0);
	vin_sys_enable_irq(vinc->irq);
	return 0;
}

static void vin_irq_release(void)
{
	if (vinc->irq > 0) {
		vin_sys_unregister_irq(vinc->irq, vin_isr, (void *)vinc);
		vin_sys_disable_irq(vinc->irq);
	}
}
#endif

int vin_core_probe(void)
{
	char main_name[10];
	int ret = 0;

	vin_log(VIN_LOG_VIDEO, "%s\n", __func__);

	sprintf(main_name, "dma%d", vinc->id);

	vinc->base = csi_getprop_regbase(main_name, "reg", 0);
	if (!vinc->base) {
		vin_err("Fail to get DMA base addr!\n");
		vinc->is_empty = 1;
		ret = -1;
		goto freedev;
	}
	vin_log(VIN_LOG_MD, "vinc%d reg is 0x%lx\n", vinc->id, vinc->base);
	csic_dma_set_base_addr(vinc->id, vinc->base);

#ifdef DMA_USE_IRQ_BUFFER_QUEUE
	vinc->irq = vin_getprop_irq(main_name, "interrupts", 0);
	if (vinc->irq <= 0) {
		vin_err("failed to get CSI DMA IRQ resource\n");
		return -1;
	}
	vin_log(VIN_LOG_MD, "vinc%d irq is %d\n", vinc->id, vinc->irq);
	vin_irq_request();
#endif

	vin_log(VIN_LOG_VIDEO, "rear_sensor_sel = %d\n", vinc->rear_sensor);
	vin_log(VIN_LOG_VIDEO, "csi_sel = %d\n", vinc->csi_sel);
	vin_log(VIN_LOG_VIDEO, "mipi_sel = %d\n", vinc->mipi_sel);
	vin_log(VIN_LOG_VIDEO, "isp_sel = %d\n", vinc->isp_sel);
	vin_log(VIN_LOG_VIDEO, "vipp_sel = %d\n", vinc->vipp_sel);

	buffer_queue();

#ifdef DMA_USE_IRQ_BUFFER_QUEUE
	struct vin_buffer *buf;

	buf = list_entry(vinc->vidq_active.next, struct vin_buffer, list);
	vin_set_addr((unsigned int)buf->phy_addr);
#else
	vin_set_addr((unsigned int)buff.phy_addr);
	disp_set_addr(sensor_formats->o_width, sensor_formats->o_height, (unsigned int)buff.phy_addr);
#endif

	return 0;
freedev:
	free(vinc);

	return ret;
}

int vin_core_remove(void)
{
	if (!vinc->is_empty) {
#ifdef DMA_USE_IRQ_BUFFER_QUEUE
		vin_irq_release();
#endif
		buffer_free();
		free(vinc);
	}
	vin_log(VIN_LOG_VIDEO, "%s end\n", __func__);
	return 0;
}

