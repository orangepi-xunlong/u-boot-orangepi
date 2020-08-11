
/*
 * vin_video.c for video api
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
#include "../utility/vin_io.h"
#include "../vin_csi/sunxi_csi.h"
#include "../vin_mipi/sunxi_mipi.h"
#include "../vin.h"

extern struct vin_core *vinc;
extern struct sensor_format_struct sensor_formats[];

#ifdef DMA_USE_IRQ_BUFFER_QUEUE
struct vin_buffer buff[BUF_NUM];
#else
struct vin_buffer buff;
#endif

int vin_set_addr(unsigned int phy_addr)
{
	struct vin_addr paddr;

	paddr.y = phy_addr;
	paddr.cb = (unsigned int)(phy_addr + sensor_formats->o_width * sensor_formats->o_height);
	paddr.cr = 0;

	csic_dma_buffer_address(vinc->vipp_sel, CSI_BUF_0_A, paddr.y);
	csic_dma_buffer_address(vinc->vipp_sel, CSI_BUF_1_A, paddr.cb);
	csic_dma_buffer_address(vinc->vipp_sel, CSI_BUF_2_A, paddr.cr);

	return 0;
}

int buffer_queue(void)
{
#ifdef DMA_USE_IRQ_BUFFER_QUEUE
	int i;

	INIT_LIST_HEAD(&vinc->vidq_active);

	for (i = 0; i < BUF_NUM; i++) {
		buff[i].phy_addr = (unsigned int *)malloc_align(sensor_formats->o_width * sensor_formats->o_height * 3/2, 0x100);
		list_add_tail(&buff[i].list, &vinc->vidq_active);
	}
#else
	/* buff.phy_addr = (unsigned int *)malloc_align(sensor_formats->o_width * sensor_formats->o_height * 3/2, 0x100); */
	buff.phy_addr = (unsigned int *)(0x61700000 + 0x12c00 + 0x400);
#endif
	return 0;
}

int buffer_free(void)
{
#ifdef DMA_USE_IRQ_BUFFER_QUEUE
	while (!list_empty(&vinc->vidq_active)) {
		struct vin_buffer *buf;
		buf =
		    list_entry(vinc->vidq_active.next, struct vin_buffer, list);
		list_del(&buf->list);
		free(buf->phy_addr);
	}
#else
	free(buff.phy_addr);
#endif
	vin_log(VIN_LOG_VIDEO, "buf free!\n");
	return 0;
}

#if 0
/* The color format (colplanes, memplanes) must be already configured. */
int vin_set_addr(struct vin_core *vinc, struct vb2_buffer *vb,
		      struct vin_frame *frame, struct vin_addr *paddr)
{
	u32 pix_size, depth, y_stride, u_stride, v_stride;

	if (vb == NULL || frame == NULL)
		return -EINVAL;

	pix_size = ALIGN(frame->o_width, VIN_ALIGN_WIDTH) * frame->o_height;

	depth = frame->fmt.depth[0] + frame->fmt.depth[1] + frame->fmt.depth[2];

	paddr->y = vb2_dma_contig_plane_dma_addr(vb, 0);

	if (frame->fmt.memplanes == 1) {
		switch (frame->fmt.colplanes) {
		case 1:
			paddr->cb = 0;
			paddr->cr = 0;
			break;
		case 2:
			/* decompose Y into Y/Cb */

			if (frame->fmt.fourcc == V4L2_PIX_FMT_FBC) {
				paddr->cb = (u32)(paddr->y + CEIL_EXP(frame->o_width, 7) * CEIL_EXP(frame->o_height, 5) * 96);
				paddr->cr = 0;

			} else {
				paddr->cb = (u32)(paddr->y + pix_size);
				paddr->cr = 0;
			}
			break;
		case 3:
			paddr->cb = (u32)(paddr->y + pix_size);
			/* 420 */
			if (12 == frame->fmt.depth[0])
				paddr->cr = (u32)(paddr->cb + (pix_size >> 2));
			/* 422 */
			else
				paddr->cr = (u32)(paddr->cb + (pix_size >> 1));
			break;
		default:
			return -EINVAL;
		}
	} else if (!frame->fmt.mdataplanes) {
		if (frame->fmt.memplanes >= 2)
			paddr->cb = vb2_dma_contig_plane_dma_addr(vb, 1);

		if (frame->fmt.memplanes == 3)
			paddr->cr = vb2_dma_contig_plane_dma_addr(vb, 2);
	}

	if (vinc->vid_cap.frame.fmt.fourcc == V4L2_PIX_FMT_YVU420) {
		csic_dma_buffer_address(vinc->vipp_sel, CSI_BUF_0_A, paddr->y);
		csic_dma_buffer_address(vinc->vipp_sel, CSI_BUF_2_A, paddr->cb);
		csic_dma_buffer_address(vinc->vipp_sel, CSI_BUF_1_A, paddr->cr);
	} else {
		csic_dma_buffer_address(vinc->vipp_sel, CSI_BUF_0_A, paddr->y);
		csic_dma_buffer_address(vinc->vipp_sel, CSI_BUF_1_A, paddr->cb);
		csic_dma_buffer_address(vinc->vipp_sel, CSI_BUF_2_A, paddr->cr);
	}
	return 0;
}
#endif
int vin_subdev_s_stream(int enable)
{
	struct csic_dma_cfg cfg;
	struct csic_dma_flip flip;
	struct dma_output_size size;
	struct dma_buf_len buf_len;
	struct dma_flip_size flip_size;
	int flag = 0;
	int flip_mul = 2;

	vin_log(VIN_LOG_FMT, "csic_dma%d %s, %d*%d hoff: %d voff: %d\n",
		vinc->id, enable ? "stream on" : "stream off",
		sensor_formats->o_width, sensor_formats->o_height,
		sensor_formats->offs_h, sensor_formats->offs_v);

	if (enable) {
		memset(&cfg, 0, sizeof(cfg));
		memset(&size, 0, sizeof(size));
		memset(&buf_len, 0, sizeof(buf_len));

		switch (sensor_formats->field) {
		case V4L2_FIELD_ANY:
		case V4L2_FIELD_NONE:
			cfg.field = FIELD_EITHER;
			break;
		case V4L2_FIELD_TOP:
			cfg.field = FIELD_1;
			flag = 1;
			break;
		case V4L2_FIELD_BOTTOM:
			cfg.field = FIELD_2;
			flag = 1;
			break;
		case V4L2_FIELD_INTERLACED:
			cfg.field = FIELD_EITHER;
			flag = 1;
			break;
		default:
			cfg.field = FIELD_EITHER;
			break;
		}

		switch (sensor_formats->fourcc) {
		case V4L2_PIX_FMT_NV12:
		case V4L2_PIX_FMT_NV12M:
		case V4L2_PIX_FMT_FBC:
			cfg.fmt = flag ? FRAME_UV_CB_YUV420 : FIELD_UV_CB_YUV420;
			buf_len.buf_len_y = sensor_formats->o_width;
			buf_len.buf_len_c = buf_len.buf_len_y;
			break;
		case V4L2_PIX_FMT_NV21:
		case V4L2_PIX_FMT_NV21M:
			cfg.fmt = flag ? FRAME_VU_CB_YUV420 : FIELD_VU_CB_YUV420;
			buf_len.buf_len_y = sensor_formats->o_width;
			buf_len.buf_len_c = buf_len.buf_len_y;
			break;
		case V4L2_PIX_FMT_YVU420:
		case V4L2_PIX_FMT_YUV420:
		case V4L2_PIX_FMT_YUV420M:
			cfg.fmt = flag ? FRAME_PLANAR_YUV420 : FIELD_PLANAR_YUV420;
			buf_len.buf_len_y = sensor_formats->o_width;
			buf_len.buf_len_c = buf_len.buf_len_y >> 1;
			break;
		case V4L2_PIX_FMT_YUV422P:
			cfg.fmt = flag ? FRAME_PLANAR_YUV422 : FIELD_PLANAR_YUV422;
			buf_len.buf_len_y = sensor_formats->o_width;
			buf_len.buf_len_c = buf_len.buf_len_y >> 1;
			break;
		case V4L2_PIX_FMT_NV61:
		case V4L2_PIX_FMT_NV61M:
			cfg.fmt = flag ? FRAME_VU_CB_YUV422 : FIELD_VU_CB_YUV422;
			buf_len.buf_len_y = sensor_formats->o_width;
			buf_len.buf_len_c = buf_len.buf_len_y;
			break;
		case V4L2_PIX_FMT_NV16:
		case V4L2_PIX_FMT_NV16M:
			cfg.fmt = flag ? FRAME_UV_CB_YUV422 : FIELD_UV_CB_YUV422;
			buf_len.buf_len_y = sensor_formats->o_width;
			buf_len.buf_len_c = buf_len.buf_len_y;
			break;
		case V4L2_PIX_FMT_SBGGR8:
		case V4L2_PIX_FMT_SGBRG8:
		case V4L2_PIX_FMT_SGRBG8:
		case V4L2_PIX_FMT_SRGGB8:
			flip_mul = 1;
			cfg.fmt = flag ? FRAME_RAW_8 : FIELD_RAW_8;
			buf_len.buf_len_y = sensor_formats->o_width;
			buf_len.buf_len_c = buf_len.buf_len_y;
			break;
		case V4L2_PIX_FMT_SBGGR10:
		case V4L2_PIX_FMT_SGBRG10:
		case V4L2_PIX_FMT_SGRBG10:
		case V4L2_PIX_FMT_SRGGB10:
			flip_mul = 1;
			cfg.fmt = flag ? FRAME_RAW_10 : FIELD_RAW_10;
			buf_len.buf_len_y = sensor_formats->o_width * 2;
			buf_len.buf_len_c = buf_len.buf_len_y;
			break;
		case V4L2_PIX_FMT_SBGGR12:
		case V4L2_PIX_FMT_SGBRG12:
		case V4L2_PIX_FMT_SGRBG12:
		case V4L2_PIX_FMT_SRGGB12:
			flip_mul = 1;
			cfg.fmt = flag ? FRAME_RAW_12 : FIELD_RAW_12;
			buf_len.buf_len_y = sensor_formats->o_width * 2;
			buf_len.buf_len_c = buf_len.buf_len_y;
			break;
		default:
			cfg.fmt = flag ? FRAME_UV_CB_YUV420 : FIELD_UV_CB_YUV420;
			buf_len.buf_len_y = sensor_formats->o_width;
			buf_len.buf_len_c = buf_len.buf_len_y;
			break;
		}

		if (vinc->isp_dbg.debug_en) {
			buf_len.buf_len_y = 0;
			buf_len.buf_len_c = 0;
		}

		switch (vinc->fps_ds) {
		case 0:
			cfg.ds = FPS_NO_DS;
			break;
		case 1:
			cfg.ds = FPS_2_DS;
			break;
		case 2:
			cfg.ds = FPS_3_DS;
			break;
		case 3:
			cfg.ds = FPS_4_DS;
			break;
		default:
			cfg.ds = FPS_NO_DS;
			break;
		}

		csic_dma_config(vinc->vipp_sel, &cfg);
		size.hor_len = vinc->isp_dbg.debug_en ? 0 : sensor_formats->o_width;
		size.ver_len = vinc->isp_dbg.debug_en ? 0 : sensor_formats->o_height;
		size.hor_start = vinc->isp_dbg.debug_en ? 0 : sensor_formats->offs_h;
		size.ver_start = vinc->isp_dbg.debug_en ? 0 : sensor_formats->offs_v;
		flip_size.hor_len = vinc->isp_dbg.debug_en ? 0 : sensor_formats->o_width * flip_mul;
		flip_size.ver_len = vinc->isp_dbg.debug_en ? 0 : sensor_formats->o_height;
		flip.hflip_en = vinc->hflip;
		flip.vflip_en = vinc->vflip;

		csic_dma_output_size_cfg(vinc->vipp_sel, &size);
		csic_dma_buffer_length(vinc->vipp_sel, &buf_len);
		csic_dma_flip_size(vinc->vipp_sel, &flip_size);
		csic_dma_flip_en(vinc->vipp_sel, &flip);
		/* give up line_cut interrupt. process in vsync and frame_done isr.*/
		/*csic_dma_line_cnt(vinc->vipp_sel, cap->frame.o_height / 16 * 12);*/
		csic_frame_cnt_enable(vinc->vipp_sel);
		if (sensor_formats->fourcc == V4L2_PIX_FMT_FBC)
			csic_fbc_enable(vinc->vipp_sel);
		else
			csic_dma_enable(vinc->vipp_sel);
#ifdef DMA_USE_IRQ_BUFFER_QUEUE
		csic_dma_int_clear_status(vinc->vipp_sel, DMA_INT_ALL);
		if (vinc->isp_dbg.debug_en)
			csic_dma_int_enable(vinc->vipp_sel, DMA_INT_ALL & ~(DMA_INT_FBC_OVHD_WRDDR_FULL));
		else
			csic_dma_int_enable(vinc->vipp_sel, DMA_INT_ALL);
		csic_dma_int_disable(vinc->vipp_sel, DMA_INT_LINE_CNT);
#endif
	} else {
#ifdef DMA_USE_IRQ_BUFFER_QUEUE
		csic_dma_int_disable(vinc->vipp_sel, DMA_INT_ALL);
		csic_dma_int_clear_status(vinc->vipp_sel, DMA_INT_ALL);
#endif
		if (sensor_formats->fourcc == V4L2_PIX_FMT_FBC)
			csic_fbc_disable(vinc->vipp_sel);
		else
			csic_dma_disable(vinc->vipp_sel);
	}

	return 0;
}


