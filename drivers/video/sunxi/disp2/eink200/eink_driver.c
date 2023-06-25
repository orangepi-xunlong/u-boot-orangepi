 /*
 * Copyright (c) 2019 Allwinnertech Co., Ltd.
 * Author: Liuli <liuli@allwinnertech.com>
 *
 * EINK driver for sunxi platform
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include "include/eink_driver.h"
#include "include/eink_sys_source.h"

struct eink_driver_info g_eink_drvdata;

u32 eink_dbg_info;
struct disp_layer_config_inner eink_para[16];
struct disp_layer_config eink_lyr_cfg2[16];

s32 eink_set_print_level(u32 level)
{
	EINK_INFO_MSG("print level = 0x%x\n", level);
	eink_dbg_info = level;

	return 0;
}

s32 eink_get_print_level(void)
{
	return eink_dbg_info;
}

/* unload resources of eink device */
static void eink_unload_resource(struct eink_driver_info *drvdata)
{
	if (!IS_ERR_OR_NULL(drvdata->init_para.panel_clk))
		clk_put(drvdata->init_para.panel_clk);
	if (!IS_ERR_OR_NULL(drvdata->init_para.ee_clk))
		clk_put(drvdata->init_para.ee_clk);
	if (!IS_ERR_OR_NULL(drvdata->init_para.de_clk))
		clk_put(drvdata->init_para.de_clk);
	return;
}

static int eink_init(struct init_para *para)
{
	eink_get_sys_config(para);
	fmt_convert_mgr_init(para);
	eink_mgr_init(para);
	return 0;
}

#if 0
static void eink_exit(void)
{
/*
	fmt_convert_mgr_exit(para);
	eink_mgr_exit(para);
*/
	return;
}
#endif

int eink_driver_init(void)
{
	int ret = 0, counter = 0;
	int node_offset = 0;
	struct eink_driver_info *drvdata = NULL;

	eink_fdt_init();

	drvdata = &g_eink_drvdata;

	eink_set_print_level(EINK_PRINT_LEVEL);
	/* reg_base */
	drvdata->init_para.ee_reg_base = eink_getprop_regbase(FDT_EINK_PATH, "reg", counter);
	if (!drvdata->init_para.ee_reg_base) {
		pr_err("of_iomap eink reg base failed!\n");
		ret =  -ENOMEM;
		goto err_out;
	}
	counter++;
	EINK_INFO_MSG("ee reg_base=0x%x\n", (unsigned int)drvdata->init_para.ee_reg_base);

	drvdata->init_para.de_reg_base = eink_getprop_regbase(FDT_EINK_PATH, "reg", counter);
	if (!drvdata->init_para.de_reg_base) {
		pr_err("of_iomap de wb reg base failed\n");
		ret =  -ENOMEM;
		goto err_out;
	}
	EINK_INFO_MSG("de reg_base=0x%x\n", (unsigned int)drvdata->init_para.de_reg_base);

	/* clk */
	node_offset = eink_fdt_nodeoffset(FDT_EINK_PATH);
	of_periph_clk_config_setup(node_offset);

	counter = 0;
	drvdata->init_para.de_clk = of_clk_get(node_offset, counter);
	if (IS_ERR_OR_NULL(drvdata->init_para.de_clk)) {
		pr_err("get disp engine clock failed!\n");
		ret = PTR_ERR(drvdata->init_para.ee_clk);
		goto err_out;
	}
	counter++;
	drvdata->init_para.ee_clk = of_clk_get(node_offset, counter);
	if (IS_ERR_OR_NULL(drvdata->init_para.ee_clk)) {
		pr_err("get eink engine clock failed!\n");
		ret = PTR_ERR(drvdata->init_para.ee_clk);
		goto err_out;
	}
	counter++;
	drvdata->init_para.panel_clk = of_clk_get(node_offset, counter);
	if (IS_ERR_OR_NULL(drvdata->init_para.panel_clk)) {
		pr_err("get edma clk clock failed!\n");
		ret = PTR_ERR(drvdata->init_para.panel_clk);
		goto err_out;
	}

	/* irq */
	counter = 0;
	drvdata->init_para.ee_irq_no = eink_getprop_irq(FDT_EINK_PATH, "interrupts", counter);
	if (!drvdata->init_para.ee_irq_no) {
		pr_err("get eink irq failed!\n");
		ret = -EINVAL;
		goto err_out;
	}

	counter++;
	EINK_INFO_MSG("eink irq_no=%u\n", drvdata->init_para.ee_irq_no);

	drvdata->init_para.de_irq_no = eink_getprop_irq(FDT_EINK_PATH, "interrupts", counter);
	if (!drvdata->init_para.de_irq_no) {
		pr_err("get de irq failed!\n");
		ret = -EINVAL;
		goto err_out;
	}
	EINK_INFO_MSG("de irq_no=%u\n", drvdata->init_para.de_irq_no);

	eink_init(&drvdata->init_para);

	g_eink_drvdata.eink_mgr = get_eink_manager();

	pr_info("[EINK] INIT finish!\n");

	return 0;

err_out:
	eink_unload_resource(drvdata);
	pr_err("eink probe failed, errno %d!\n", ret);
	return ret;
}

#if 0
static int eink_driver_exit(void)
{
	struct eink_driver_info *drvdata;

	pr_info("%s\n", __func__);

	drvdata = &g_eink_drvdata;
	if (drvdata) {
		eink_unload_resource(drvdata);
	} else
		pr_err("%s:drvdata is NULL!\n", __func__);

	pr_info("%s finish!\n", __func__);

	return 0;
}
#endif

static s32 copy_to_user(void *dest, void *src, u32 size)
{
	memcpy(dest, src, size);
	return 0;
}

long eink_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	long ret = 0;
	unsigned long karg[4];
	unsigned long ubuffer[4] = { 0 };
	struct eink_manager *eink_mgr = NULL;

	eink_mgr = g_eink_drvdata.eink_mgr;

	if (copy_from_user
			((void *)karg, (void __user *)arg, 4 * sizeof(unsigned long))) {
		pr_err("copy_from_user fail\n");
		return -EFAULT;
	}

	ubuffer[0] = *(unsigned long *)karg;
	ubuffer[1] = (*(unsigned long *)(karg + 1));
	ubuffer[2] = (*(unsigned long *)(karg + 2));
	ubuffer[3] = (*(unsigned long *)(karg + 3));

	switch (cmd) {
	case EINK_WRITE_BACK_IMG:
		{
			s32 i = 0;
			struct eink_img last_img;
			struct eink_img cur_img;
			if (!eink_mgr) {
				pr_err("there is no eink manager!\n");
				break;
			}
			memset(eink_lyr_cfg2, 0,
					16 * sizeof(struct disp_layer_config2));

			if (copy_from_user(eink_lyr_cfg2, (void __user *)ubuffer[0],
						sizeof(struct disp_layer_config2) * ubuffer[1])) {
				pr_err("layer config copy_from_user fail\n");
				return -EFAULT;
			}

			if (copy_from_user(&last_img, (void __user *)ubuffer[2],
						sizeof(struct eink_img))) {
				pr_err("last img copy_from_user fail\n");
				return -EFAULT;
			}

			if (copy_from_user(&cur_img, (void __user *)ubuffer[3],
						sizeof(struct eink_img))) {
				pr_err("cur img copy_from_user fail\n");
				return -EFAULT;
			}

			for (i = 0; i < ubuffer[1]; i++)
				__disp_config_transfer2inner(&eink_para[i],
						&eink_lyr_cfg2[i]);

			if (eink_mgr->eink_fmt_cvt_img)
				ret = eink_mgr->eink_fmt_cvt_img(
						(struct disp_layer_config_inner *)&eink_para[0],
						(unsigned int)ubuffer[1],
						&last_img, &cur_img);
			if (ret == 0) {
				if (copy_to_user((void __user *)ubuffer[3], &cur_img,
							sizeof(struct eink_img))) {
					pr_err("cur img copy_to_user fail\n");
					return -EFAULT;
				}
			}
			break;
		}
	case EINK_UPDATE_IMG:
		{
			struct eink_img cur_img;

			if (!eink_mgr) {
				pr_err("[%s]:there is no eink manager!\n", __func__);
				break;
			}

			memset(&cur_img, 0, sizeof(struct eink_img));
			if (copy_from_user(&cur_img, (void __user *)ubuffer[0],
					   sizeof(struct eink_img))) {
				pr_err("copy_from_user fail\n");
				return -EFAULT;
			}

			if (eink_mgr->eink_update)
				ret = eink_mgr->eink_update(eink_mgr, &cur_img);
			if (ret)
				pr_warn("EINK UPDATE failed!\n");
			break;
		}

	case EINK_SET_TEMP:
		{
			if (eink_mgr->set_temperature)
				ret = eink_mgr->set_temperature(eink_mgr, ubuffer[0]);
			break;
		}

	case EINK_GET_TEMP:
		{
			if (eink_mgr->get_temperature)
				ret = eink_mgr->get_temperature(eink_mgr);
			break;
		}

	case EINK_SET_GC_CNT:
		{
			if (eink_mgr->eink_set_global_clean_cnt)
				ret = eink_mgr->eink_set_global_clean_cnt(eink_mgr, ubuffer[0]);
			break;
		}
	case EINK_WAIT_PIPE_FINISH:
		{
			return eink_mgr->get_pipe_finish_status(eink_mgr);
			break;
		}

	default:
		pr_err("The cmd is err!\n");
		break;
	}
	return ret;
}
