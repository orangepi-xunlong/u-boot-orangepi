/*
 * sunxi_isp.c
 *
 * Copyright (c) 2007-2017 Allwinnertech Co., Ltd.
 *
 * Authors:  Zhao Wei <zhaowei@allwinnertech.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include "../utility/platform_cfg.h"
#include "sunxi_isp.h"

#include "../vin_csi/sunxi_csi.h"
#include "../vin_vipp/sunxi_scaler.h"
#include "../vin_video/vin_core.h"
#include "../utility/vin_io.h"
#include "isp_default_tbl.h"

#define MIN_IN_WIDTH			192
#define MIN_IN_HEIGHT			128
#define MAX_IN_WIDTH			4224
#define MAX_IN_HEIGHT			4224

struct isp_dev *isp;
extern struct vin_core *vinc;
extern struct sensor_format_struct sensor_formats[];
extern struct sensor_list *sensor;

static struct isp_pix_fmt sunxi_isp_formats[] = {
	{
		.name = "RAW8 (BGGR)",
		.fourcc = V4L2_PIX_FMT_SBGGR8,
		.depth = {8},
		.color = 0,
		.memplanes = 1,
		.mbus_code = MEDIA_BUS_FMT_SBGGR8_1X8,
		.infmt = ISP_BGGR,
	}, {
		.name = "RAW8 (GBRG)",
		.fourcc = V4L2_PIX_FMT_SGBRG8,
		.depth = {8},
		.color = 0,
		.memplanes = 1,
		.mbus_code = MEDIA_BUS_FMT_SGBRG8_1X8,
		.infmt = ISP_GBRG,
	}, {
		.name = "RAW8 (GRBG)",
		.fourcc = V4L2_PIX_FMT_SGRBG8,
		.depth = {8},
		.color = 0,
		.memplanes = 1,
		.mbus_code = MEDIA_BUS_FMT_SGRBG8_1X8,
		.infmt = ISP_GRBG,
	}, {
		.name = "RAW8 (RGGB)",
		.fourcc = V4L2_PIX_FMT_SRGGB8,
		.depth = {8},
		.color = 0,
		.memplanes = 1,
		.mbus_code = MEDIA_BUS_FMT_SRGGB8_1X8,
		.infmt = ISP_RGGB,
	}, {
		.name = "RAW10 (BGGR)",
		.fourcc = V4L2_PIX_FMT_SBGGR10,
		.depth = {10},
		.color = 0,
		.memplanes = 1,
		.mbus_code = MEDIA_BUS_FMT_SBGGR10_1X10,
		.infmt = ISP_BGGR,
	}, {
		.name = "RAW10 (GBRG)",
		.fourcc = V4L2_PIX_FMT_SGBRG8,
		.depth = {10},
		.color = 0,
		.memplanes = 1,
		.mbus_code = MEDIA_BUS_FMT_SGBRG10_1X10,
		.infmt = ISP_GBRG,
	}, {
		.name = "RAW10 (GRBG)",
		.fourcc = V4L2_PIX_FMT_SGRBG10,
		.depth = {10},
		.color = 0,
		.memplanes = 1,
		.mbus_code = MEDIA_BUS_FMT_SGRBG10_1X10,
		.infmt = ISP_GRBG,
	}, {
		.name = "RAW10 (RGGB)",
		.fourcc = V4L2_PIX_FMT_SRGGB10,
		.depth = {10},
		.color = 0,
		.memplanes = 1,
		.mbus_code = MEDIA_BUS_FMT_SRGGB10_1X10,
		.infmt = ISP_RGGB,
	}, {
		.name = "RAW12 (BGGR)",
		.fourcc = V4L2_PIX_FMT_SBGGR12,
		.depth = {12},
		.color = 0,
		.memplanes = 1,
		.mbus_code = MEDIA_BUS_FMT_SBGGR12_1X12,
		.infmt = ISP_BGGR,
	}, {
		.name = "RAW12 (GBRG)",
		.fourcc = V4L2_PIX_FMT_SGBRG12,
		.depth = {12},
		.color = 0,
		.memplanes = 1,
		.mbus_code = MEDIA_BUS_FMT_SGBRG12_1X12,
		.infmt = ISP_GBRG,
	}, {
		.name = "RAW12 (GRBG)",
		.fourcc = V4L2_PIX_FMT_SGRBG12,
		.depth = {12},
		.color = 0,
		.memplanes = 1,
		.mbus_code = MEDIA_BUS_FMT_SGRBG12_1X12,
		.infmt = ISP_GRBG,
	}, {
		.name = "RAW12 (RGGB)",
		.fourcc = V4L2_PIX_FMT_SRGGB12,
		.depth = {12},
		.color = 0,
		.memplanes = 1,
		.mbus_code = MEDIA_BUS_FMT_SRGGB12_1X12,
		.infmt = ISP_RGGB,
	},
};

int sunxi_isp_subdev_s_stream(int enable)
{
	unsigned int load_val;

	if (!isp->use_isp)
		return 0;

	switch (sensor_formats->fourcc) {
	case V4L2_PIX_FMT_SBGGR8:
	case V4L2_PIX_FMT_SGBRG8:
	case V4L2_PIX_FMT_SGRBG8:
	case V4L2_PIX_FMT_SRGGB8:
	case V4L2_PIX_FMT_SBGGR10:
	case V4L2_PIX_FMT_SGBRG10:
	case V4L2_PIX_FMT_SGRBG10:
	case V4L2_PIX_FMT_SRGGB10:
	case V4L2_PIX_FMT_SBGGR12:
	case V4L2_PIX_FMT_SGBRG12:
	case V4L2_PIX_FMT_SGRBG12:
	case V4L2_PIX_FMT_SRGGB12:
		vin_log(VIN_LOG_FMT, "%s output fmt is raw, return directly\n", __func__);
		if (isp->isp_dbg.debug_en) {
			bsp_isp_debug_output_cfg(isp->id, 1, isp->isp_dbg.debug_sel);
			break;
		} else {
			return 0;
		}
	default:
		break;
	}

	if (enable) {
		bsp_isp_enable(isp->id, 1);
#ifdef ISP_USE_IRQ
		bsp_isp_clr_irq_status(isp->id, ISP_IRQ_EN_ALL);
		bsp_isp_irq_enable(isp->id, FINISH_INT_EN | PARA_LOAD_INT_EN | SRC0_FIFO_INT_EN
					| FRAME_ERROR_INT_EN | FRAME_LOST_INT_EN);
#endif
		load_val = bsp_isp_load_update_flag(isp->id);
		load_val = load_val & ~WDR_UPDATE;
		bsp_isp_module_disable(isp->id, WDR_EN);
		bsp_isp_set_wdr_mode(isp->id, ISP_NORMAL_MODE);

		bsp_isp_update_table(isp->id, load_val);
		bsp_isp_module_enable(isp->id, SRC0_EN);
		bsp_isp_set_input_fmt(isp->id, isp->isp_fmt->infmt);
		bsp_isp_set_size(isp->id, &isp->isp_ob);

		flush_cache((unsigned long)isp->isp_load.phy_addr, ISP_LOAD_REG_SIZE);
		/* flush_cache((unsigned long)isp->isp_tbl.isp_gamma_tbl_dma_addr, ISP_GAMMA_MEM_SIZE); */
		flush_cache((unsigned long)isp->isp_tbl.isp_lsc_tbl_dma_addr, ISP_TABLE_MAPPING1_SIZE);
		flush_cache((unsigned long)isp->isp_tbl.isp_drc_tbl_dma_addr, ISP_TABLE_MAPPING2_SIZE);

		bsp_isp_set_para_ready(isp->id, PARA_READY);
		bsp_isp_set_last_blank_cycle(isp->id, 5);
		bsp_isp_set_speed_mode(isp->id, 3);
		bsp_isp_ch_enable(isp->id, ISP_CH0, 1);
		bsp_isp_capture_start(isp->id);
	} else {
		/*when pipeline reset ,no send isp off event*/
		bsp_isp_capture_stop(isp->id);
		bsp_isp_module_disable(isp->id, SRC0_EN);
		bsp_isp_ch_enable(isp->id, ISP_CH0, 0);
#ifdef ISP_USE_IRQ
		bsp_isp_irq_disable(isp->id, ISP_IRQ_EN_ALL);
		bsp_isp_clr_irq_status(isp->id, ISP_IRQ_EN_ALL);
#endif
		bsp_isp_enable(isp->id, 0);
		isp->f1_after_librun = 0;
	}

	vin_log(VIN_LOG_FMT, "isp%d %s, %d*%d hoff: %d voff: %d code: %x field: %d\n",
		isp->id, enable ? "stream on" : "stream off",
		isp->isp_ob.ob_valid.width, isp->isp_ob.ob_valid.height,
		isp->isp_ob.ob_start.hor, isp->isp_ob.ob_start.ver,
		sensor_formats->mbus_code, sensor_formats->field);

	return 0;
}

static struct isp_pix_fmt *__isp_find_format(const u32 *pixelformat,
							const u32 *mbus_code,
							int index)
{
	struct isp_pix_fmt *fmt, *def_fmt = NULL;
	unsigned int i;
	int id = 0;

	if (index >= (int)ARRAY_SIZE(sunxi_isp_formats))
		return NULL;

	for (i = 0; i < ARRAY_SIZE(sunxi_isp_formats); ++i) {
		fmt = &sunxi_isp_formats[i];
		if (pixelformat && fmt->fourcc == *pixelformat)
			return fmt;
		if (mbus_code && fmt->mbus_code == *mbus_code)
			return fmt;
		if (index == id)
			def_fmt = fmt;
		id++;
	}
	return def_fmt;
}

static struct isp_pix_fmt *__isp_try_format(void)
{
	struct isp_pix_fmt *isp_fmt;
	struct isp_size_settings *ob = &isp->isp_ob;

	isp_fmt = __isp_find_format(NULL, &sensor_formats->mbus_code, 0);
	if (isp_fmt == NULL)
		isp_fmt = &sunxi_isp_formats[0];

	ob->ob_black.width = sensor_formats->width;
	ob->ob_black.height = sensor_formats->height;

	if (isp->id == 1) {
		sensor_formats->width = clamp_t(u32, sensor_formats->width, MIN_IN_WIDTH, 3264);
		sensor_formats->height = clamp_t(u32, sensor_formats->height, MIN_IN_HEIGHT, 3264);
	} else {
		sensor_formats->width = clamp_t(u32, sensor_formats->width, MIN_IN_WIDTH, 4224);
		sensor_formats->height = clamp_t(u32, sensor_formats->height, MIN_IN_HEIGHT, 4224);
	}

	ob->ob_valid.width = sensor_formats->width;
	ob->ob_valid.height = sensor_formats->height;
	ob->ob_start.hor = (ob->ob_black.width - ob->ob_valid.width) / 2;
	ob->ob_start.ver = (ob->ob_black.height - ob->ob_valid.height) / 2;

	return isp_fmt;
}

int sunxi_isp_subdev_init(void)
{

	if (!isp->have_init) {
		memcpy(isp->isp_load.phy_addr, &isp_default_reg[0], ISP_LOAD_REG_SIZE);
		/* memcpy(isp->isp_tbl.isp_gamma_tbl_dma_addr, &gamma_tbl[0], ISP_GAMMA_MEM_SIZE); */
		memcpy(isp->isp_tbl.isp_lsc_tbl_dma_addr, &isp_lut_tbl[0], ISP_TABLE_MAPPING1_SIZE);
		memcpy(isp->isp_tbl.isp_drc_tbl_dma_addr, &isp_drc_tbl[0], ISP_TABLE_MAPPING2_SIZE);
		isp->load_flag = 0;
		isp->have_init = 1;
	}
	bsp_isp_set_statistics_addr(isp->id, (unsigned long)isp->isp_stat.phy_addr);
	bsp_isp_set_load_addr(isp->id, (unsigned long)isp->isp_load.phy_addr);
	bsp_isp_set_saved_addr(isp->id, (unsigned long)isp->isp_save.phy_addr);
	bsp_isp_set_table_addr(isp->id, LENS_GAMMA_TABLE, (unsigned long)(isp->isp_tbl.isp_lsc_tbl_dma_addr));
	bsp_isp_set_table_addr(isp->id, DRC_TABLE, (unsigned long)(isp->isp_tbl.isp_drc_tbl_dma_addr));

	bsp_isp_set_para_ready(isp->id, PARA_NOT_READY);

	return 0;
}

#ifdef ISP_USE_IRQ
/*
 * must reset all the pipeline through isp.
 */
static void __sunxi_isp_reset(void)
{
	struct prs_cap_mode mode = {.mode = VCAP};

	vin_log(VIN_LOG_ISP, "isp%d reset!!!\n", isp->id);

	/*****************stop*******************/
	csic_prs_capture_stop(vinc->csi_sel);
	csic_prs_disable(vinc->csi_sel);

	if (sensor_formats->fourcc == V4L2_PIX_FMT_FBC)
		csic_fbc_disable(vinc->vipp_sel);
	else
		csic_dma_disable(vinc->vipp_sel);
	vipp_disable(vinc->vipp_sel);
	vipp_top_clk_en(vinc->vipp_sel, 0);

	bsp_isp_enable(isp->id, 0);
	bsp_isp_capture_stop(isp->id);

	/*****************start*******************/
	vipp_top_clk_en(vinc->vipp_sel, 1);
	vipp_enable(vinc->vipp_sel);
	if (sensor_formats->fourcc == V4L2_PIX_FMT_FBC)
		csic_fbc_enable(vinc->vipp_sel);
	else
		csic_dma_enable(vinc->vipp_sel);

	bsp_isp_enable(isp->id, 1);
	bsp_isp_capture_start(isp->id);

	csic_prs_enable(vinc->csi_sel);
	csic_prs_capture_start(vinc->csi_sel, 1, &mode);
}
#endif

static int isp_resource_alloc(void)
{
	struct isp_table_addr *tbl = &isp->isp_tbl;
	void *dma;

	isp->isp_stat.size = ISP_STAT_TOTAL_SIZE + ISP_LOAD_REG_SIZE + ISP_SAVED_REG_SIZE
			+ ISP_TABLE_MAPPING1_SIZE + ISP_TABLE_MAPPING2_SIZE;
	/* isp->isp_stat.phy_addr = malloc_align(isp->isp_stat.size, 0x100); */
	isp->isp_stat.phy_addr = (void *)0x61700000;

	isp->isp_load.phy_addr = isp->isp_stat.phy_addr + ISP_STAT_TOTAL_SIZE;
	isp->isp_save.phy_addr = isp->isp_load.phy_addr + ISP_LOAD_REG_SIZE;
	isp->isp_lut_tbl.phy_addr = isp->isp_save.phy_addr + ISP_SAVED_REG_SIZE;
	isp->isp_drc_tbl.phy_addr = isp->isp_lut_tbl.phy_addr + ISP_TABLE_MAPPING1_SIZE;

	dma = isp->isp_lut_tbl.phy_addr;
	tbl->isp_lsc_tbl_dma_addr = (void *)(dma + ISP_LSC_MEM_OFS);
	tbl->isp_gamma_tbl_dma_addr = (void *)(dma + ISP_GAMMA_MEM_OFS);
	tbl->isp_linear_tbl_dma_addr = (void *)(dma + ISP_LINEAR_MEM_OFS);

	dma = isp->isp_drc_tbl.phy_addr;

	tbl->isp_drc_tbl_dma_addr = (void *)(dma + ISP_DRC_MEM_OFS);

	return 0;
}

#ifdef ISP_USE_IRQ
static int isp_isr(int irq, void *priv)
{
	unsigned int load_val;

	if (!isp->use_isp)
		return 0;

	vin_log(VIN_LOG_ISP, "isp%d interrupt, status is 0x%x!!!\n", isp->id,
		bsp_isp_get_irq_status(isp->id, ISP_IRQ_STATUS_ALL));

	if (bsp_isp_get_irq_status(isp->id, SRC0_FIFO_OF_PD)) {
		vin_err("isp%d source0 fifo overflow\n", isp->id);
		bsp_isp_clr_irq_status(isp->id, SRC0_FIFO_OF_PD);
		if (bsp_isp_get_irq_status(isp->id, CIN_FIFO_OF_PD)) {
			vin_err("isp%d Cin0 fifo overflow\n", isp->id);
			bsp_isp_clr_irq_status(isp->id, CIN_FIFO_OF_PD);
		}
		if (bsp_isp_get_irq_status(isp->id, DPC_FIFO_OF_PD)) {
			vin_err("isp%d DPC fifo overflow\n", isp->id);
			bsp_isp_clr_irq_status(isp->id, DPC_FIFO_OF_PD);
		}
		if (bsp_isp_get_irq_status(isp->id, D2D_FIFO_OF_PD)) {
			vin_err("isp%d D2D fifo overflow\n", isp->id);
			bsp_isp_clr_irq_status(isp->id, D2D_FIFO_OF_PD);
		}
		if (bsp_isp_get_irq_status(isp->id, BIS_FIFO_OF_PD)) {
			vin_err("isp%d BIS fifo overflow\n", isp->id);
			bsp_isp_clr_irq_status(isp->id, BIS_FIFO_OF_PD);
		}
		if (bsp_isp_get_irq_status(isp->id, CNR_FIFO_OF_PD)) {
			vin_err("isp%d CNR fifo overflow\n", isp->id);
			bsp_isp_clr_irq_status(isp->id, CNR_FIFO_OF_PD);
		}
		if (bsp_isp_get_irq_status(isp->id, PLTM_FIFO_OF_PD)) {
			vin_err("isp%d PLTM fifo overflow\n", isp->id);
			bsp_isp_clr_irq_status(isp->id, PLTM_FIFO_OF_PD);
		}
		if (bsp_isp_get_irq_status(isp->id, D3D_WRITE_FIFO_OF_PD)) {
			vin_err("isp%d D3D cmp write to DDR fifo overflow\n", isp->id);
			bsp_isp_clr_irq_status(isp->id, D3D_WRITE_FIFO_OF_PD);
		}
		if (bsp_isp_get_irq_status(isp->id, D3D_READ_FIFO_OF_PD)) {
			vin_err("isp%d D3D umcmp read from DDR fifo empty\n", isp->id);
			bsp_isp_clr_irq_status(isp->id, D3D_READ_FIFO_OF_PD);
		}
		if (bsp_isp_get_irq_status(isp->id, D3D_WT2CMP_FIFO_OF_PD)) {
			vin_err("isp%d D3D write to cmp fifo overflow\n", isp->id);
			bsp_isp_clr_irq_status(isp->id, D3D_WT2CMP_FIFO_OF_PD);
		}

		if (bsp_isp_get_irq_status(isp->id, WDR_WRITE_FIFO_OF_PD)) {
			vin_err("isp%d WDR cmp write to DDR fifo overflow\n", isp->id);
			bsp_isp_clr_irq_status(isp->id, WDR_WRITE_FIFO_OF_PD);
		}
		if (bsp_isp_get_irq_status(isp->id, WDR_READ_FIFO_OF_PD)) {
			vin_err("isp%d WDR umcmp read from DDR fifo empty\n", isp->id);
			bsp_isp_clr_irq_status(isp->id, WDR_READ_FIFO_OF_PD);
		}
		if (bsp_isp_get_irq_status(isp->id, WDR_WT2CMP_FIFO_OF_PD)) {
			vin_err("isp%d WDR write to cmp fifo overflow\n", isp->id);
			bsp_isp_clr_irq_status(isp->id, WDR_WT2CMP_FIFO_OF_PD);
		}
		if (bsp_isp_get_irq_status(isp->id, D3D_HB_PD)) {
			vin_err("isp%d Hblanking is not enough for D3D\n", isp->id);
			bsp_isp_clr_irq_status(isp->id, D3D_HB_PD);
		}
		/*isp reset*/
		__sunxi_isp_reset();
	}

	if (bsp_isp_get_irq_status(isp->id, HB_SHROT_PD)) {
		vin_err("isp%d Hblanking is short (less than 96 cycles)\n", isp->id);
		bsp_isp_clr_irq_status(isp->id, HB_SHROT_PD);
	}

	if (bsp_isp_get_irq_status(isp->id, FRAME_ERROR_PD)) {
		vin_err("isp%d frame error\n", isp->id);
		bsp_isp_clr_irq_status(isp->id, FRAME_ERROR_PD);
		__sunxi_isp_reset();
	}

	if (bsp_isp_get_irq_status(isp->id, FRAME_LOST_PD)) {
		vin_err("isp%d frame lost\n", isp->id);
		bsp_isp_clr_irq_status(isp->id, FRAME_LOST_PD);
		__sunxi_isp_reset();
	}

	if (bsp_isp_get_irq_status(isp->id, PARA_LOAD_PD)) {
		bsp_isp_clr_irq_status(isp->id, PARA_LOAD_PD);
		bsp_isp_set_para_ready(isp->id, PARA_NOT_READY);
		/*
		if (isp->load_flag)
			memcpy(isp->isp_load.phy_addr, &isp->load_shadow[0], ISP_LOAD_REG_SIZE);
		*/
		load_val = bsp_isp_load_update_flag(isp->id);
		vin_log(VIN_LOG_ISP, "please close wdr in normal mode!!\n");
		load_val = load_val & ~WDR_UPDATE;
		bsp_isp_module_disable(isp->id, WDR_EN);
		bsp_isp_set_wdr_mode(isp->id, ISP_NORMAL_MODE);

		bsp_isp_set_size(isp->id, &isp->isp_ob);
		bsp_isp_update_table(isp->id, (unsigned short)load_val);
		bsp_isp_set_para_ready(isp->id, PARA_READY);
	}

	if (bsp_isp_get_irq_status(isp->id, FINISH_PD)) {
		bsp_isp_clr_irq_status(isp->id, FINISH_PD);
		vin_log(VIN_LOG_ISP, "call tasklet schedule!\n");
		/*
		if (!isp->f1_after_librun) {
			sunxi_isp_frame_sync_isr(&isp->subdev);
			if (isp->h3a_stat.stata_en_flag)
				isp->f1_after_librun = 1;
		} else {
			if (isp->load_flag)
				sunxi_isp_frame_sync_isr(&isp->subdev);
		}
		*/
		isp->load_flag = 0;
	}

	return 0;
}

static int isp_irq_request(void)
{
	vin_sys_register_irq(isp->irq, 0, isp_isr, (void *)isp, 0, 0);
	vin_sys_enable_irq(isp->irq);
	return 0;
}

static void isp_irq_release(void)
{
	if (vinc->irq > 0) {
		vin_sys_unregister_irq(isp->irq, isp_isr, (void *)isp);
		vin_sys_disable_irq(isp->irq);
	}
}
#endif

int isp_probe(void)
{
	char main_name[10];
	int ret = 0;

	sprintf(main_name, "isp%d", vinc->isp_sel);

	isp = malloc(sizeof(struct isp_dev));
	if (!isp) {
		ret = -1;
		goto ekzalloc;
	}
	memset(isp, 0, sizeof(struct isp_dev));
	isp->base = csi_getprop_regbase(main_name, "reg", 0);
	if (!isp->base) {
		ret = -1;
		goto freedev;
	}

	isp->id = vinc->isp_sel;
	isp->use_isp = sensor->use_isp;
	vin_log(VIN_LOG_MD, "isp%d reg is 0x%lx\n", isp->id, isp->base);

	if (!isp->base) {
		vin_err("Fail toget the ISP base addr\n");
		isp->is_empty = 1;
		ret = -1;
		goto freedev;
	} else {
		isp->is_empty = 0;
		isp_resource_alloc();

		bsp_isp_map_reg_addr(isp->id, (unsigned long)isp->base);
		bsp_isp_map_load_dram_addr(isp->id, (unsigned long)isp->isp_load.phy_addr);
	}
#ifdef ISP_USE_IRQ
	isp->irq = vin_getprop_irq(main_name, "interrupts", 0);
	if (isp->irq <= 0) {
		vin_err("failed to get ISP IRQ resource\n");
		return -1;
	}
	vin_log(VIN_LOG_MD, "isp%d irq is %d\n", isp->id, isp->irq);

	isp_irq_request();
#endif
	isp->isp_fmt = __isp_try_format();

	sunxi_isp_subdev_init();

	vin_log(VIN_LOG_ISP, "isp%d probe end!\n", isp->id);
	return 0;
freedev:
	free(isp);
ekzalloc:
	vin_err("isp probe err!\n");
	return ret;
}

int isp_remove(void)
{
	if (!isp->is_empty) {
#ifdef ISP_USE_IRQ
		isp_irq_release();
#endif
		free(isp->isp_stat.phy_addr);
		free(isp);
	}
	return 0;
}
