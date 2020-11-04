/* drv_edp.c
 *
 * Copyright (c) 2007-2017 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <zhengxiaobin@allwinnertech.com>
 *
 * edp driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include "drv_edp.h"

static u32 g_edp_num;
struct drv_edp_info_t g_edp_info[EDP_NUM_MAX];
static u32 power_enable_count;

static int edp_power_enable(char *name, bool en)
{
	if (en)
		return disp_sys_power_enable(name);
	return disp_sys_power_disable(name);
}

/**
 * @name       :edp_clk_enable
 * @brief      :enable or disable edp clk
 * @param[IN]  :sel index of edp
 * @param[IN]  :en 1:enable , 0 disable
 * @return     :0 if success
 */
static s32 edp_clk_enable(u32 sel, bool en)
{
	s32 ret = -1;

	if (g_edp_info[sel].clk) {
		ret = 0;
		if (en)
			ret = clk_prepare_enable(g_edp_info[sel].clk);
		else
			clk_disable(g_edp_info[sel].clk);
	}

	return ret;
}

#if defined(CONFIG_SWITCH) || defined(CONFIG_ANDROID_SWITCH)
static struct task_struct *edp_hpd_task;
static u32 edp_hpd[EDP_NUM_MAX];
static struct switch_dev switch_dev[EDP_NUM_MAX];
static char switch_name[20];

s32 edp_report_hpd_work(u32 sel, u32 hpd)
{
	if (edp_hpd[sel] == hpd)
		return -1;

	switch (hpd) {
	case EDP_STATUE_CLOSE:
		switch_set_state(&switch_dev[sel], EDP_STATUE_CLOSE);
		break;

	case EDP_STATUE_OPEN:
		switch_set_state(&switch_dev[sel], EDP_STATUE_OPEN);
		break;

	default:
		switch_set_state(&switch_dev[sel], EDP_STATUE_CLOSE);
		break;
	}
	edp_hpd[sel] = hpd;
	return 0;
}

s32 edp_hpd_detect_thread(void *parg)
{
	s32 hpd[EDP_NUM_MAX];
	struct drv_edp_info_t *p_edp_info = NULL;
	u32 sel = 0;

	if (!parg) {
		edp_wrn("NUll ndl\n");
		return -1;
	}

	p_edp_info = (struct drv_edp_info_t *)parg;
	sel = p_edp_info->dev->id;

	edp_dbg("sel:%d\n", sel);

	while (1) {
		if (kthread_should_stop())
			break;

		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(200);

		if (!p_edp_info->suspend) {
			hpd[sel] = dp_get_hpd_status(sel);
			if (hpd[sel] != edp_hpd[sel]) {
				edp_dbg("hpd[%d] = %d\n", sel, hpd[sel]);
				edp_report_hpd_work(sel, hpd[sel]);
			}
		}
	}
	return 0;
}

s32 edp_detect_enable(u32 sel)
{
	s32 err = 0;

	if (!edp_hpd_task) {
		edp_hpd_task = kthread_create(edp_hpd_detect_thread,
					      &(g_edp_info[sel]),
					      "edp detect");
		if (IS_ERR(edp_hpd_task)) {
			err = PTR_ERR(edp_hpd_task);
			edp_hpd_task = NULL;
			return err;
		}
		edp_dbg("edp_hpd_task is ok!\n");
		wake_up_process(edp_hpd_task);
	}
	return 0;
}

s32 edp_detect_disable(s32 sel)
{
	if (edp_hpd_task) {
		kthread_stop(edp_hpd_task);
		edp_hpd_task = NULL;
	}
	return 0;
}

#elif defined(CONFIG_EXTCON)
static struct task_struct *edp_hpd_task;
static u32 edp_hpd[EDP_NUM_MAX];
static struct extcon_dev extcon_edp[EDP_NUM_MAX];
static char switch_name[20];
static const unsigned int edp_cable[] = {
	EXTCON_DISP_EDP,
	EXTCON_NONE,
};

s32 edp_report_hpd_work(u32 sel, u32 hpd)
{
	if (edp_hpd[sel] == hpd)
		return -1;

	switch (hpd) {
	case EDP_STATUE_CLOSE:
		extcon_set_state(&extcon_edp[sel], EDP_STATUE_CLOSE);
		break;

	case EDP_STATUE_OPEN:
		extcon_set_state(&extcon_edp[sel], EDP_STATUE_OPEN);
		break;

	default:
		extcon_set_state(&extcon_edp[sel], EDP_STATUE_CLOSE);
		break;
	}
	edp_hpd[sel] = hpd;
	return 0;
}

s32 edp_hpd_detect_thread(void *parg)
{
	s32 hpd[EDP_NUM_MAX];
	struct drv_edp_info_t *p_edp_info = NULL;
	u32 sel = 0;

	if (!parg) {
		edp_wrn("NUll ndl\n");
		return -1;
	}

	p_edp_info = (struct drv_edp_info_t *)parg;
	sel = p_edp_info->dev->id;

	edp_dbg("sel:%d\n", sel);

	while (1) {
		if (kthread_should_stop())
			break;

		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(200);

		if (!p_edp_info->suspend) {
			hpd[sel] = dp_get_hpd_status(sel);
			if (hpd[sel] != edp_hpd[sel]) {
				edp_dbg("hpd[%d] = %d\n", sel, hpd[sel]);
				edp_report_hpd_work(sel, hpd[sel]);
			}
		}
	}
	return 0;
}

s32 edp_detect_enable(u32 sel)
{
	s32 err = 0;

	if (!edp_hpd_task) {
		edp_hpd_task = kthread_create(edp_hpd_detect_thread,
					      &(g_edp_info[sel]),
					      "edp detect");
		if (IS_ERR(edp_hpd_task)) {
			err = PTR_ERR(edp_hpd_task);
			edp_hpd_task = NULL;
			return err;
		}
		edp_dbg("edp_hpd_task is ok!\n");
		wake_up_process(edp_hpd_task);
	}
	return 0;
}

s32 edp_detect_disable(s32 sel)
{
	if (edp_hpd_task) {
		kthread_stop(edp_hpd_task);
		edp_hpd_task = NULL;
	}
	return 0;
}
#else
s32 edp_report_hpd_work(u32 sel, u32 hpd)
{
	edp_dbg("You need to config SWITCH or EXTCON!\n");
	return 0;
}
s32 edp_hpd_detect_thread(void *parg)
{
	edp_dbg("You need to config SWITCH or EXTCON!\n");
	return 0;
}
s32 edp_detect_enable(u32 sel)
{
	edp_dbg("You need to config SWITCH or EXTCON!\n");
	return 0;
}
s32 edp_detect_disable(s32 sel)
{
	edp_dbg("You need to config SWITCH or EXTCON!\n");
	return 0;
}
#endif

s32 edp_get_sys_config(u32 disp, struct disp_video_timings *p_info)
{
	s32 ret = -1;
	s32  value = 1;
	char primary_key[20], sub_name[25];

	if (!p_info)
		goto OUT;

	sprintf(primary_key, "edp%d", disp);
	memset(p_info, 0, sizeof(struct disp_video_timings));

	sprintf(sub_name, "edp_io_power");
	g_edp_info[disp].edp_io_power_used = 0;
	ret = disp_sys_script_get_item(
	    primary_key, sub_name, (int *)(g_edp_info[disp].edp_io_power), 2);
	if (ret == 2) {
		g_edp_info[disp].edp_io_power_used = 1;
		mutex_lock(&g_edp_info[disp].mlock);
		ret = edp_power_enable(g_edp_info[disp].edp_io_power, true);
		if (ret) {
			mutex_unlock(&g_edp_info[disp].mlock);
			goto OUT;
		}
		++power_enable_count;
		mutex_unlock(&g_edp_info[disp].mlock);
	}


	ret = disp_sys_script_get_item(primary_key, "edp_x", &value, 1);
	if (ret == 1)
		p_info->x_res = value;

	ret = disp_sys_script_get_item(primary_key, "edp_y", &value, 1);
	if (ret == 1)
		p_info->y_res = value;

	ret = disp_sys_script_get_item(primary_key, "edp_hbp", &value, 1);
	if (ret == 1)
		p_info->hor_back_porch = value;

	ret = disp_sys_script_get_item(primary_key, "edp_ht", &value, 1);
	if (ret == 1)
		p_info->hor_total_time = value;

	ret = disp_sys_script_get_item(primary_key, "edp_hspw", &value, 1);
	if (ret == 1)
		p_info->hor_sync_time = value;

	ret = disp_sys_script_get_item(primary_key, "edp_vt", &value, 1);
	if (ret == 1)
		p_info->ver_total_time = value;

	ret = disp_sys_script_get_item(primary_key, "edp_vspw", &value, 1);
	if (ret == 1)
		p_info->ver_sync_time = value;

	ret = disp_sys_script_get_item(primary_key, "edp_vbp", &value, 1);
	if (ret == 1)
		p_info->ver_back_porch = value;

	ret = disp_sys_script_get_item(primary_key, "edp_rate", &value, 1);
	if (ret == 1) {
		switch (value) {
		case 0:
			g_edp_info[disp].para.edp_rate = BR_1P62G;
			break;
		case 1:
			g_edp_info[disp].para.edp_rate = BR_2P7G;
			break;
		case 2:
			g_edp_info[disp].para.edp_rate = BR_5P4G;
			break;
		default:
			edp_wrn("edp_rate out of range!\n");
			break;
		}
	}

	ret = disp_sys_script_get_item(primary_key, "edp_lane", &value, 1);
	if (ret == 1)
		g_edp_info[disp].para.edp_lane = value;

	ret = disp_sys_script_get_item(primary_key, "edp_training_func", &value,
				       1);
	if (ret == 1)
		g_edp_info[disp].para.edp_training_func = value;

	ret = disp_sys_script_get_item(primary_key, "edp_sramble_seed", &value,
				       1);
	if (ret == 1)
		g_edp_info[disp].para.edp_sramble_seed = value;

	ret =
	    disp_sys_script_get_item(primary_key, "edp_colordepth", &value, 1);
	if (ret == 1)
		g_edp_info[disp].para.edp_colordepth = value;

	ret = disp_sys_script_get_item(primary_key, "edp_fps", &value, 1);
	if (ret == 1)
		g_edp_info[disp].para.edp_fps = value;

	p_info->pixel_clk = p_info->hor_total_time * p_info->ver_total_time *
			    g_edp_info[disp].para.edp_fps;

OUT:
	return 0;
}
/**
 * @name       edp_disable
 * @brief      disable edp module
 * @param[IN]  sel:index of edp
 * @param[OUT] none
 * @return     0 if success
 */
s32 edp_disable(u32 sel)
{
	s32 ret = 0;

	edp_here;
	mutex_lock(&g_edp_info[sel].mlock);
	if (g_edp_info[sel].enable) {
		dp_disable(sel);
		g_edp_info[sel].enable = 0;
	}
	mutex_unlock(&g_edp_info[sel].mlock);
	return ret;
}

/**
 * @name       edp_enable
 * @brief      edp enable
 * @param[IN]  sel index of edp
 * @return     0 if success
 */
s32 edp_enable(u32 sel)
{
	s32 ret = 0;

	edp_here;
	if (!g_edp_info[sel].enable) {
		ret = dp_enable(sel, &g_edp_info[sel].para,
				&g_edp_info[sel].timings);
		if (ret)
			goto OUT;
		mutex_lock(&g_edp_info[sel].mlock);
		g_edp_info[sel].enable = 1;
		mutex_unlock(&g_edp_info[sel].mlock);
	}

OUT:
	return ret;
}

s32 edp_resume(u32 sel)
{
	s32 ret = -1;

	edp_here;
	mutex_lock(&g_edp_info[sel].mlock);
	if (g_edp_info[sel].suspend) {
		edp_clk_enable(sel, true);
		g_edp_info[sel].suspend = true;
		ret = dp_enable(sel, &g_edp_info[sel].para,
				&g_edp_info[sel].timings);
		edp_detect_enable(sel);
		if (g_edp_info[sel].edp_io_power_used && !power_enable_count) {
			edp_power_enable(g_edp_info[sel].edp_io_power, true);
			++power_enable_count;
		}
	}
	mutex_unlock(&g_edp_info[sel].mlock);
	return ret;
}

s32 edp_suspend(u32 sel)
{
	s32 ret = -1;

	edp_here;
	mutex_lock(&g_edp_info[sel].mlock);
	if (!g_edp_info[sel].suspend) {
		g_edp_info[sel].suspend = true;
		edp_detect_disable(sel);
		ret = edp_clk_enable(sel, false);
		if (g_edp_info[sel].edp_io_power_used && power_enable_count) {
			ret = edp_power_enable(g_edp_info[sel].edp_io_power,
					       false);
			--power_enable_count;
		}
	}
	mutex_unlock(&g_edp_info[sel].mlock);
	return ret;
}

/**
 * @name       edp_get_video_timing_info
 * @brief      get timing info
 * @param[IN]  sel:index of edp module
 * @param[OUT] video_info:timing info
 * @return     0 if success
 */
static s32 edp_get_video_timing_info(u32 sel,
				      struct disp_video_timings **video_info)
{
	s32 ret = 0;

	edp_here;
	*video_info = &g_edp_info[sel].timings;
	return ret;
}


static s32 __edp_init(u32 sel)
{
	s32 ret = -1;

	edp_dbg("start edp init\n");

	mutex_init(&g_edp_info[sel].mlock);

	ret = edp_get_sys_config(sel, &g_edp_info[sel].timings);
	if (ret != 0)
		goto OUT;

	dp_set_reg_base(sel, g_edp_info[sel].base_addr);

#if defined(CONFIG_SWITCH) || defined(CONFIG_ANDROID_SWITCH)
	snprintf(switch_name, sizeof(switch_name), "edp%d", sel);
	switch_dev[sel].name = switch_name;
	switch_dev_register(&switch_dev[sel]);
#elif defined(CONFIG_EXTCON)
	snprintf(switch_name, sizeof(switch_name), "edp%d", sel);
	extcon_edp->name = switch_name;
	extcon_edp[sel].supported_cable = edp_cable;
	extcon_dev_register_attr(&extcon_edp[sel], &pdev->dev);
#endif
OUT:
	edp_dbg("end of edp init:%d\n", ret);
	return ret;
}

s32 edp_get_start_delay(u32 sel)
{
	s32 ret = -1;

	ret = dp_get_start_dly(sel);
	return ret;
}

void edp_show_builtin_patten(u32 sel, u32 patten)
{
	dp_show_builtin_patten(sel, patten);
}


unsigned int edp_get_cur_line(u32 sel)
{
	u32 ret = 0;

	ret = dp_get_cur_line(sel);
	return ret;
}

s32 edp_irq_enable(u32 sel, u32 irq_id, u32 en)
{
	s32 ret = 0;

	edp_here;
	dp_int_enable(sel, LINE0, (bool)en);
	return ret;
}

s32 edp_irq_query(u32 sel)
{
	s32 ret = -1;

	ret = dp_int_query(sel, LINE0);
	if (ret == 1)
		dp_int_clear(sel, LINE0);
	return ret;
}

static s32 edp_probe(u32 sel)
{
	struct disp_tv_func edp_func;
	s32 ret = -1;
	char main_key[20];
	int node_offset = 0;

	edp_dbg("Welcome to edp_probe %d\n", g_edp_num);
	sprintf(main_key, "edp%d", sel);

	if (!g_edp_num)
		memset(&g_edp_info, 0,
		       sizeof(struct drv_edp_info_t) * EDP_NUM_MAX);

	if (g_edp_num > EDP_NUM_MAX - 1) {
		edp_wrn("g_edp_num(%d) is greater then EDP_NUM_MAX-1(%d)\n",
			g_edp_num, EDP_NUM_MAX - 1);
		goto OUT;
	}

	g_edp_info[g_edp_num].base_addr =
	    disp_getprop_regbase(main_key, "reg", 0);
	if (!g_edp_info[g_edp_num].base_addr) {
		edp_wrn("fail to get addr for edp%d!\n", sel);
		goto ERR_IOMAP;
	}

	node_offset = disp_fdt_nodeoffset(main_key);
	g_edp_info[sel].clk = of_clk_get(node_offset, 0);
	if (IS_ERR(g_edp_info[sel].clk)) {
		edp_wrn("fail to get clk for edp%d!\n", sel);
		goto ERR_IOMAP;
	}

	ret = edp_clk_enable(sel, true);
	if (ret) {
		edp_wrn("edp%d edp_clk_enable fail!!\n", sel);
		goto ERR_IOMAP;
	}

	ret = __edp_init(sel);
	if (ret) {
		edp_wrn("__edp_init for edp%d fail!\n", sel);
		goto ERR_IOMAP;
	}


	ret = edp_detect_enable(sel);
	if (ret) {
		edp_wrn("edp_detect_enable fail!\n");
		goto ERR_CLK;
	}

	if (!g_edp_num) {
		memset(&edp_func, 0, sizeof(struct disp_tv_func));
		edp_func.tv_enable = edp_enable;
		edp_func.tv_disable = edp_disable;
		edp_func.tv_resume = edp_resume;
		edp_func.tv_suspend = edp_suspend;
		edp_func.tv_get_video_timing_info = edp_get_video_timing_info;
		edp_func.tv_irq_enable = edp_irq_enable;
		edp_func.tv_irq_query = edp_irq_query;
		edp_func.tv_get_startdelay = edp_get_start_delay;
		edp_func.tv_get_cur_line = edp_get_cur_line;
		edp_func.tv_show_builtin_patten = edp_show_builtin_patten;
		edp_here;
		ret = disp_set_edp_func(&edp_func);
		if (ret) {
			edp_wrn("disp_set_edp_func edp%d fail!\n", sel);
			goto ERR_HPD_DETECT;
		}
	} else
		ret = 0;
	++g_edp_num;
	if (ret == 0)
		goto OUT;

ERR_HPD_DETECT:
	edp_detect_enable(sel);
ERR_CLK:
	edp_clk_enable(sel, false);
ERR_IOMAP:
OUT:
	return ret;
}

s32 edp_init(void)
{
	s32 i = 0, ret = 0;
	char main_key[20];
	int value = 0;
	u32 edp_num = 1;

#if defined(DEVICE_EDP_NUM)
	edp_num = DEVICE_EDP_NUM;
#endif /*endif def */

	for (i = 0; i < edp_num; i++) {
		printf("%s:\n", __func__);
		sprintf(main_key, "edp%d", i);

		ret = disp_sys_script_get_item(main_key, "used", &value, 1);
		if (ret != 1) {
			edp_wrn("fetch edp%d err.\n", i);
			continue;
		}
		if (1 != value)
			continue;

		edp_probe(i);
	}

	return 0;
}

s32 edp_remove(void)
{
	s32 ret = 0;
	u32 i = 0;

	for (i = 0; i < g_edp_num; ++i) {
		edp_detect_disable(i);
		if (g_edp_info[i].edp_io_power_used && power_enable_count) {
			edp_power_enable(g_edp_info[i].edp_io_power, false);
			--power_enable_count;
		}
		edp_disable(i);
		edp_clk_enable(i, false);
	}

	return ret;
}
