/*
 * Copyright (C) 2019 Allwinnertech, <liuli@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include "include/eink_driver.h"
#include "include/eink_sys_source.h"
#include "lowlevel/eink_reg.h"
#include <asm/arch/gic.h>
#include "include/fmt_convert.h"

static struct eink_manager *g_eink_manager;

#ifdef CONFIG_PMIC_TPS65185
extern int tps65185_vcom_set(int vcom_mv);
extern int tps65185_temperature_get(int *temp);
extern void tps65185_return_main_i2c(void);
#endif

#ifdef VIRTUAL_REGISTER
static void *test_preg_base;
static void *test_vreg_base;
#endif

struct eink_manager *get_eink_manager(void)
{
	return g_eink_manager;
}

#if 0
int get_vcom_file_from_private_part(char **vcom_argv)
{
	int ret = 0;
	int partno = -1;

	ret = script_parser_fetch("waveform_feature", "vcom_path",
				  (int *)vcom_argv[4], 16);
	if ((ret < 0) || (strlen(vcom_argv[4]) == 0)) {
		printf("sunxi_waveform: vcom_path is not set, use default\n");
		return -1;
	}

	partno = sunxi_partition_get_partno_byname("private");
	printf("get_vcom_file_from_private_part partno === %d\n", partno);
	if (partno < 0) {
		printf("get_vcom_file_from_private_part failed\n");
		return -1;
	}
	sprintf(vcom_argv[2], "%x:0", partno);
	printf("use vcom.bin from private partation\n");

	return 0;
}
#endif

static int atoi_float(char *buf)
{
	int num = 0;
	int i = 0;
	int point_pos = -1;
	int pow = 0;

	while (buf[i] != '\0') {
		if (buf[i] >= '0' && buf[i] <= '9')	{
			if ((point_pos < 0) || ((point_pos >= 0) && ((point_pos + 4) > i))) {
				num = num * 10 + (buf[i] - '0');
			} else {
				//drop the left char
				break;
			}
		}

		if (buf[i] == '.') {
			point_pos = i;
		}
		i++;
	}

	//auto *1000
	if ((point_pos >= 0) && ((point_pos + 4) > i)) {
		pow = (point_pos + 4 - i);
		while (pow > 0) {
			num = num * 10;
			pow--;
		}
	}

	if (buf[0] == '-')
		num = num * (-1);

	return num;
}

static int __get_vcom_from_file(char *path, int *vcom)
{
	char *vcombuf = NULL;
	char vcom_value[5] = {0};
	char *vcom_argv[6];
	int partno = -1;
	char part_info[16] = {0}, vcom_addr[32] = {0};
	int nVcomVoltage = 0;
	int ret = 0;

	if ((!vcom) || (!path)) {
		printf("[%s]:input param is null\n", __func__);
		return -1;
	}
	partno = sunxi_partition_get_partno_byname("bootloader");
	if (partno < 0) {
		partno = sunxi_partition_get_partno_byname("boot-resource");
		if (partno < 0) {
			printf("[%s]:Get bootloader or boot-resource partition number fail!\n", __func__);
			goto OUT;
		}
	}
	snprintf(part_info, 16, "0:%x", partno);

	vcombuf = (char *)malloc_aligned(64, ARCH_DMA_MINALIGN);
	if (!vcombuf) {
		pr_err("malloc vcombuf fail!\n");
		goto OUT;
	}

	memset(vcombuf, 0, 64);
	snprintf(vcom_addr, 32, "%lx", (unsigned long)vcombuf);


	vcom_argv[0] = "fatload";
	vcom_argv[1] = "sunxi_flash";
	vcom_argv[2] = part_info;
	vcom_argv[3] = vcom_addr;
	vcom_argv[4] = path;
	vcom_argv[5] = NULL;

	if (do_fat_fsload(0, 0, 5, vcom_argv)) {
		printf("read VCOM file from  %s failed!\n", vcom_argv[4]);
		goto FREE_VCOM;
	} else{
		EINK_INFO_MSG("read VCOM file from  %s succeed\n", vcom_argv[4]);
	}

	memcpy(vcom_value, vcombuf, 5);
	nVcomVoltage = atoi_float(vcombuf);
	*vcom = nVcomVoltage;

FREE_VCOM:
	if (vcombuf) {
		free_aligned(vcombuf);
		vcombuf = NULL;
	}

	EINK_INFO_MSG("get nVcomVoltage = %d\n", nVcomVoltage);
OUT:
	return ret;
}

static int eink_set_vcom_voltage(int vcom)
{
	int ret = -1;
#ifdef CONFIG_PMIC_TPS65185
	ret = tps65185_vcom_set(vcom);		//unit is mv
	if (ret != 0) {
		printf("%s: set vcom fail, ret=%d\n", __func__, ret);
	}
#else
	//other method
#endif

	return ret;
}

#define DEFAULT_VCOM_VOLTAGE (-1550)
static int get_vcom_config_value(char *path)
{
	int vcom = 0;
	int ret  = -1;

	EINK_INFO_MSG("[%s]: path = %s\n", __func__, path);
	ret = __get_vcom_from_file(path, &vcom);
	if (ret != 0) {
		printf("fail to get vcom from file, use default value\n");
		vcom = DEFAULT_VCOM_VOLTAGE;
	}
	eink_set_vcom_voltage(vcom);
	return ret;
}

static int detect_fresh_thread(struct eink_manager *eink_mgr, struct eink_img *cur_img)
{
	int ret = 0;
	u32 temperature = 28;
	int pipe_id = -1;
	u32 request_fail_cnt = 0;
	struct pipe_manager *pipe_mgr = NULL;
	struct index_manager *index_mgr = NULL;
	struct pipe_info_node pipe_info;

#ifdef REGISTER_PRINT
	unsigned long reg_base = 0, reg_end = 0;
#endif

	if (!eink_mgr) {
		pr_err("%s: eink manager is not initial\n", __func__);
		return -1;
	}
	pipe_mgr = eink_mgr->pipe_mgr;
	index_mgr = eink_mgr->index_mgr;

	index_mgr->set_rmi_addr(index_mgr);

	temperature = eink_mgr->get_temperature(eink_mgr);

	EINK_INFO_MSG("Cur_img addr = 0x%p\n", cur_img->paddr);

	memset(&pipe_info, 0, sizeof(struct pipe_info_node));
	pipe_info.img = cur_img;
	memcpy(&pipe_info.upd_win, &cur_img->upd_win, sizeof(struct upd_win));
	pipe_info.upd_mode = cur_img->upd_mode;

#ifdef DRIVER_REMAP_WAVEFILE
		eink_get_wf_data(pipe_info.upd_mode, temperature,
				&pipe_info.total_frames, &pipe_info.wav_paddr,
				&pipe_info.wav_vaddr);/* 还没想好结果 fix me */
#else
		get_waveform_data(pipe_info.upd_mode, temperature,
				&pipe_info.total_frames, &pipe_info.wav_paddr,
				&pipe_info.wav_vaddr);
#endif

	EINK_INFO_MSG("temp=%d, mode=0x%x, total=%d, waveform_paddr=0x%x, waveform_vaddr=0x%x\n",
		      temperature, pipe_info.upd_mode,
			pipe_info.total_frames, (unsigned int)pipe_info.wav_paddr,
			(unsigned int)pipe_info.wav_vaddr);
#ifdef WAVEDATA_DEBUG
	save_waveform_to_mem(curnode->upd_order,
			     (u8 *)pipe_info.wav_vaddr,
			pipe_info.total_frames,
			eink_mgr->panel_info.bit_num);
	if (pipe_info.upd_mode == EINK_INIT_MODE) {
		EINK_INFO_MSG("get_gray_mem mode = %d\n", pipe_info.upd_mode);
		eink_get_gray_from_mem((u8 *)pipe_info.wav_vaddr,
				       DEFAULT_INIT_WAV_PATH,
				(pipe_info.total_frames * 256 / 4), 0);
	} else if (pipe_info.upd_mode == EINK_GC16_MODE) {
		EINK_INFO_MSG("get_gray_mem mode = %d\n", pipe_info.upd_mode);
		eink_get_gray_from_mem((u8 *)pipe_info.wav_vaddr,
				       DEFAULT_GC16_WAV_PATH,
				(51 * 64), 0);
	}
#endif
	pipe_id = -1;
	while (pipe_id < 0 && request_fail_cnt < REQUEST_PIPE_FAIL_MAX_CNT) {
		pipe_id = pipe_mgr->request_pipe(pipe_mgr);
		if (pipe_id < 0) {
			request_fail_cnt++;
			mdelay(5);
			continue;
		}
	}
	if (pipe_id < 0) {
		pr_err("Request free pipe failed!\n");
		ret = -1;
		goto err_out;
	}

	pipe_info.pipe_id = pipe_id;

	/* config pipe */
	ret = pipe_mgr->config_pipe(pipe_mgr, pipe_info);

	/* enable pipe */
	ret = pipe_mgr->active_pipe(pipe_mgr, pipe_info.pipe_id);

	/* enable eink engine */
	EINK_INFO_MSG("Eink start!\n");
	eink_start();


	tps65185_return_main_i2c();

#ifdef REGISTER_PRINT
	reg_base = eink_get_reg_base();
	reg_end = reg_base + 0x3ff;
	eink_print_register(reg_base, reg_end);
#endif

	return 0;
err_out:
	return ret;
}

int eink_update_image(struct eink_manager *eink_mgr, struct eink_img *cur_img)
{
	int ret = 0;

	char path[32] = {0};

	EINK_INFO_MSG("func input!\n");

	if (eink_mgr->waveform_init_flag == false) {
		ret = waveform_mgr_init(eink_mgr->wav_path, eink_mgr->panel_info.bit_num);
		if (ret) {
			pr_err("%s:init waveform failed!\n", __func__);
			return ret;
		} else
			eink_mgr->waveform_init_flag = true;
		strcpy(path, "wavefile/vcom.bin");
		eink_mgr->vcom_voltage = get_vcom_config_value(path);
	}


	ret = eink_mgr->eink_mgr_enable(eink_mgr);
	if (ret) {
		pr_err("enable eink mgr failed, ret = %d\n", ret);
		return -1;
	}

	detect_fresh_thread(eink_mgr, cur_img);

	return ret;
}

int upd_pic_accept_irq_handler(void)
{
	struct eink_manager *mgr = g_eink_manager;

	mgr->upd_pic_accept_flag = 1;

	return 0;
}

int upd_pic_finish_irq_handler(void)
{
	struct eink_manager *eink_mgr = get_eink_manager();

	eink_mgr->upd_pic_fin_flag = 1;

	return 0;
}

int pipe_finish_irq_handler(struct pipe_manager *mgr)
{
	unsigned long flags = 0;
	u32 pipe_cnt = 0, pipe_id = 0;
	u64 finish_val = 0;
	struct pipe_info_node *cur_pipe = NULL, *tmp_pipe = NULL;

	pipe_cnt = mgr->max_pipe_cnt;

	spin_lock_irqsave(&mgr->list_lock, flags);

	finish_val = eink_get_fin_pipe_id();
	EINK_INFO_MSG("Pipe finish val = 0x%llx\n", finish_val);

	for (pipe_id = 0; pipe_id < pipe_cnt; pipe_id++) {
		if ((1ULL << pipe_id) & finish_val) {
			list_for_each_entry_safe(cur_pipe, tmp_pipe, &mgr->pipe_used_list, node) {
				if (cur_pipe->pipe_id != pipe_id) {
					continue;
				}

				if (mgr->release_pipe)
					mgr->release_pipe(mgr, cur_pipe);
				list_move_tail(&cur_pipe->node, &mgr->pipe_free_list);
				break;
			}
		}
	}

	eink_reset_fin_pipe_id(finish_val);

	mgr->current_pipe_finish = true;
	spin_unlock_irqrestore(&mgr->list_lock, flags);
	return 0;
}

int eink_intterupt_proc(int irq, void *parg)
{
	struct eink_manager *eink_mgr = NULL;
	struct pipe_manager *pipe_mgr = get_pipeline_manager();
	int reg_val = -1;
	unsigned int ee_fin, upd_pic_accept, upd_pic_fin, pipe_fin;

	eink_mgr = g_eink_manager;
	if ((!eink_mgr) && (!pipe_mgr)) {
		pr_err("%s:eink mgr or pipe mgr is NULL!\n", __func__);
		return EINK_IRQ_RETURN;
	}

	reg_val = eink_irq_query();
	EINK_INFO_MSG("Enter Interrupt Proc, Reg_Val = 0x%x\n", reg_val);

	upd_pic_accept = reg_val & 0x10;
	upd_pic_fin = reg_val & 0x100;
	pipe_fin = reg_val & 0x1000;
	ee_fin = reg_val & 0x1000000;

	if (upd_pic_accept == 0x10) {
		upd_pic_accept_irq_handler();
	}

	if (upd_pic_fin == 0x100) {
		upd_pic_finish_irq_handler();
	}

	if (ee_fin == 0x1000000) {
		eink_mgr->ee_finish = true;
	}

	if (pipe_fin == 0x1000) {
		pipe_finish_irq_handler(pipe_mgr);
		/*eink_mgr_exit(eink_mgr);*/
	}

	return EINK_IRQ_RETURN;
}

static int eink_set_temperature(struct eink_manager *mgr, u32 temp)
{
	int ret = 0;

	if (mgr)
		mgr->panel_temperature = temp;
	else {
		pr_err("%s:mgr is NULL!\n", __func__);
		ret = -1;
	}

	return ret;
}

s32 eink_get_resolution(struct eink_manager *mgr, u32 *xres, u32 *yres)
{
	if (!mgr) {
		pr_err("[%s]: eink manager is NULL!\n", __func__);
		return -EINVAL;
	}

	*xres = mgr->panel_info.width;
	*yres = mgr->panel_info.height;
	return 0;
}

u32 eink_get_temperature(struct eink_manager *mgr)
{
	int ret = -1;
	s32 temp = 28;

	ret = tps65185_temperature_get(&temp);
	if (ret) {
		printf("[%s]:Get cur temperture failed!use default 28\n", __func__);
		temp = 28;
	}

	return temp;
}

int eink_get_sys_config(struct init_para *para)
{
	int ret = 0, i = 0;
	s32 value = 0;
	char primary_key[20], sub_name[25];
	struct eink_gpio_cfg *gpio_info;

	struct eink_panel_info *panel_info = NULL;
	struct timing_info *timing_info = NULL;

	panel_info = &para->panel_info;
	timing_info = &para->timing_info;

	sprintf(primary_key, "eink");

	/* eink power */
	ret = eink_sys_script_get_item(primary_key, "eink_power", (int *)para->eink_power, 2);
	if (ret == 2) {
		para->power_used = 1;
	}

	/* eink panel gpio */
	for (i = 0; i < EINK_GPIO_NUM; i++) {
		sprintf(sub_name, "eink_gpio_%d", i);

		gpio_info = &para->eink_gpio[i];
		ret = eink_sys_script_get_item(primary_key, sub_name,
					       (int *)gpio_info, 3);
		if (ret == 3)
			para->eink_gpio_used[i] = 1;
		EINK_INFO_MSG("eink_gpio_%d used = %d\n", i, para->eink_gpio_used[i]);
	}

	/* eink pin power */
	ret = eink_sys_script_get_item(primary_key, "eink_pin_power", (int *)para->eink_pin_power, 2);

	/* single or dual 0 single 1 dual 2 four*/
	ret = eink_sys_script_get_item(primary_key, "eink_scan_mode", &value, 1);
	if (ret == 1) {
		panel_info->eink_scan_mode = value;
	}
	/* eink panel cfg */
	ret = eink_sys_script_get_item(primary_key, "eink_width", &value, 1);
	if (ret == 1) {
		panel_info->width = value;
	}

	ret = eink_sys_script_get_item(primary_key, "eink_height", &value, 1);
	if (ret == 1) {
		panel_info->height = value;
	}

	ret = eink_sys_script_get_item(primary_key, "eink_fresh_hz", &value, 1);
	if (ret == 1) {
		panel_info->fresh_hz = value;
	}

	ret = eink_sys_script_get_item(primary_key, "eink_gray_level", &value, 1);
	if (ret == 1) {
		panel_info->gray_level_cnt = value;
	}

/*
	ret = disp_sys_script_get_item(primary_key, "eink_sdck", &value, 1);
	if (ret == 1) {
		panel_info->sdck = value;
	}
*/
	ret = eink_sys_script_get_item(primary_key, "eink_bits", &value, 1);
	if (ret == 1) {
		panel_info->bit_num = value;
	}

	ret = eink_sys_script_get_item(primary_key, "eink_data_len", &value, 1);
	if (ret == 1) {
		panel_info->data_len = value;
	}
	/* eink timing para */
	ret = eink_sys_script_get_item(primary_key, "eink_lsl", &value, 1);
	if (ret == 1) {
		timing_info->lsl = value;
	}

	ret = eink_sys_script_get_item(primary_key, "eink_lbl", &value, 1);
	if (ret == 1) {
		timing_info->lbl = value;
	}

	ret = eink_sys_script_get_item(primary_key, "eink_lel", &value, 1);
	if (ret == 1) {
		timing_info->lel = value;
	}

	ret = eink_sys_script_get_item(primary_key, "eink_gdck_sta", &value, 1);
	if (ret == 1) {
		timing_info->gdck_sta = value;
	}

	ret = eink_sys_script_get_item(primary_key, "eink_lgonl", &value, 1);
	if (ret == 1) {
		timing_info->lgonl = value;
	}

	ret = eink_sys_script_get_item(primary_key, "eink_gdoe_start_line", &value, 1);
	if (ret == 1) {
		timing_info->gdoe_start_line = value;
	}

	ret = eink_sys_script_get_item(primary_key, "eink_fsl", &value, 1);
	if (ret == 1) {
		timing_info->fsl = value;
	}

	ret = eink_sys_script_get_item(primary_key, "eink_fbl", &value, 1);
	if (ret == 1) {
		timing_info->fbl = value;
	}

	ret = eink_sys_script_get_item(primary_key, "eink_fel", &value, 1);
	if (ret == 1) {
		timing_info->fel = value;
	}

	timing_info->ldl = (panel_info->width) / (panel_info->data_len / 2);
	timing_info->fdl = panel_info->height;

	strcpy(para->wav_path, "wavefile/default.awf");

	return ret;
}

s32 eink_set_global_clean_cnt(struct eink_manager *mgr, u32 cnt)
{
	return 0;
}

static void print_panel_info(struct init_para *para)
{
	struct eink_panel_info *info = &para->panel_info;
	struct timing_info *timing = &para->timing_info;

	EINK_INFO_MSG("width=%d, height=%d, fresh_hz=%d, scan_mode=%d\n",
		      info->width, info->height, info->fresh_hz, info->eink_scan_mode);
	EINK_INFO_MSG("sdck=%d, bit_num=%d, data_len=%d, gray_level_cnt=%d\n",
		      info->sdck, info->bit_num, info->data_len, info->gray_level_cnt);
	EINK_INFO_MSG("lsl=%d, lbl=%d, ldl=%d, lel=%d, gdck_sta=%d, lgonl=%d\n",
		      timing->lsl, timing->lbl, timing->ldl, timing->lel,
			timing->gdck_sta, timing->lgonl);
	EINK_INFO_MSG("fsl=%d, fbl=%d, fdl=%d, fel=%d, gdoe_start_line=%d\n",
		      timing->fsl, timing->fbl, timing->fdl, timing->fel,
			timing->gdoe_start_line);
	EINK_INFO_MSG("wavdata_path = %s\n", para->wav_path);
}

int eink_clk_enable(struct eink_manager *mgr)
{
	int ret = 0;
	u32 vsync = 0, hsync = 0;
	struct timing_info *timing = NULL;
	u32 fresh_rate = 0;
	unsigned long panel_freq = 0, temp_freq = 0;

	if (mgr->clk_enable_flag) {
		printf("[%s]has been enable\n", __func__);
		return 0;
	}
	if (mgr->ee_clk) {
		ret = clk_prepare_enable(mgr->ee_clk);
	}

	timing = &mgr->timing_info;
	fresh_rate = mgr->panel_info.fresh_hz;

	hsync = timing->lsl + timing->lbl + timing->ldl + timing->lel;
	vsync = timing->fsl + timing->fbl + timing->fdl + timing->fel;
	panel_freq = fresh_rate * hsync * vsync;

	EINK_INFO_MSG("panel_freq = %lu\n", panel_freq);

	if (mgr->panel_clk_parent) {
		ret = clk_set_rate(mgr->panel_clk_parent, panel_freq);
		if (ret) {
			pr_err("%s:set panel parent freq failed!\n", __func__);
			return -1;
		}
	}
	if (mgr->panel_clk) {
		ret = clk_set_rate(mgr->panel_clk, panel_freq);
		if (ret) {
			pr_err("%s:set panel freq failed!\n", __func__);
			return -1;
		}

		temp_freq = clk_get_rate(mgr->panel_clk);
		if (panel_freq != temp_freq) {
			pr_warn("%s: not set real clk, freq=%lu\n", __func__, temp_freq);
		}

		ret = clk_prepare_enable(mgr->panel_clk);
	}

	mgr->clk_enable_flag = 1;
	return ret;
}

void eink_clk_disable(struct eink_manager *mgr)
{
	if (mgr->clk_enable_flag == 0) {
		printf("[%s]has been disable\n", __func__);
		return;
	}
	if (mgr->ee_clk)
		clk_disable(mgr->ee_clk);

	if (mgr->panel_clk)
		clk_disable(mgr->panel_clk);

	mgr->clk_enable_flag = 0;
	return;
}

s32 eink_get_clk_rate(struct clk *device_clk)
{
	unsigned long freq = 0;

	if (!device_clk) {
		pr_err("[%s]: clk is NULL!\n", __func__);
		return -EINVAL;
	}

	freq = clk_get_rate(device_clk);
	EINK_DEFAULT_MSG("clk freq = %ld\n", freq);

	return freq;
}

s32 eink_get_fps(struct eink_manager *mgr)
{
	int fps = 0;

	if (!mgr) {
		pr_err("[%s]: mgr is NULL\n", __func__);
		return -EINVAL;
	}
	fps = mgr->panel_info.fresh_hz;
	return fps;
}

s32 eink_dump_config(struct eink_manager *mgr, char *buf)
{
	u32 count = 0;
	return count;
}

s32 eink_mgr_enable(struct eink_manager *eink_mgr)
{
	int ret = 0;

	struct pipe_manager *pipe_mgr = NULL;
	struct timing_ctrl_manager *timing_cmgr = NULL;

	EINK_INFO_MSG("input!\n");

	if (!eink_mgr) {
		pr_err("%s: eink is not initial\n", __func__);
		return -1;
	}

	pipe_mgr = eink_mgr->pipe_mgr;
	timing_cmgr = eink_mgr->timing_ctrl_mgr;

	if (!pipe_mgr || !timing_cmgr) {
		pr_err("%s: pipe or timing mgr not initial\n", __func__);
		return -1;
	}

	mutex_lock(&eink_mgr->enable_lock);
	if (eink_mgr->enable_flag == true) {
		mutex_unlock(&eink_mgr->enable_lock);
		return 0;
	}

	eink_clk_enable(eink_mgr);

	ret = pipe_mgr->pipe_mgr_enable(pipe_mgr);
	if (ret) {
		pr_err("%s:fail to enable pipe mgr", __func__);
		goto pipe_enable_fail;
	}

	ret = timing_cmgr->enable(timing_cmgr);
	if (ret) {
		pr_err("%s:fail to enable timing ctrl mgr", __func__);
		goto timing_enable_fail;
	}

	eink_mgr->enable_flag = true;
	mutex_unlock(&eink_mgr->enable_lock);

	return 0;

timing_enable_fail:
	pipe_mgr->pipe_mgr_disable();
pipe_enable_fail:
	eink_mgr->enable_flag = false;
	mutex_unlock(&eink_mgr->enable_lock);
	return ret;
}

s32 eink_mgr_disable(struct eink_manager *eink_mgr)
{
	int ret = 0;

	struct pipe_manager *pipe_mgr = NULL;
	struct timing_ctrl_manager *timing_cmgr = NULL;

	pipe_mgr = eink_mgr->pipe_mgr;
	timing_cmgr = eink_mgr->timing_ctrl_mgr;

	if ((!pipe_mgr) || (!timing_cmgr)) {
		pr_err("%s:pipe or timing ctrl manager is not initial\n", __func__);
		return -1;
	}

	mutex_lock(&eink_mgr->enable_lock);
	if (eink_mgr->enable_flag == false) {
		mutex_unlock(&eink_mgr->enable_lock);
		return 0;
	}

	ret = timing_cmgr->disable(timing_cmgr);
	if (ret) {
		pr_err("%s:fail to enable timing ctrl mgr", __func__);
		goto timing_enable_fail;
	}
	ret = pipe_mgr->pipe_mgr_disable();
	if (ret) {
		pr_err("fail to disable pipe(%d)\n", ret);
		goto pipe_disable_fail;
	}

	eink_mgr->enable_flag = false;
	mutex_unlock(&eink_mgr->enable_lock);

	return 0;

pipe_disable_fail:
	timing_cmgr->enable(timing_cmgr);
timing_enable_fail:
	eink_mgr->enable_flag = true;
	mutex_unlock(&eink_mgr->enable_lock);

	return ret;
}

int eink_fmt_convert_image(struct disp_layer_config_inner *config, unsigned int layer_num,
		struct eink_img *last_img, struct eink_img *cur_img)
{
	int ret = 0, sel = 0;
	struct fmt_convert_manager *cvt_mgr = NULL;
	struct eink_manager *eink_mgr = get_eink_manager();

	if (eink_mgr == NULL) {
		pr_err("[%s]:eink_mgr is NULL!\n", __func__);
		return -1;
	}

	if ((config == NULL) || (last_img == NULL) || (cur_img == NULL)) {
		pr_err("%s:layer config or img is null, please check\n", __func__);
		return -EINVAL;
	}

	cvt_mgr = get_fmt_convert_mgr(sel);

	ret = cvt_mgr->enable(sel);
	if (ret) {
		pr_err("%s:enable convert failed\n", __func__);
		return ret;
	}

	/* used DE hardware to convert 32bpp to 8bpp */
	ret = cvt_mgr->start_convert(sel, config, layer_num, last_img, cur_img);
	if (ret < 0) {
		pr_err("%s: fmt convert failed!\n", __func__);
		return ret;
	}


	return ret;
}

bool eink_get_pipe_finish_status(struct eink_manager *mgr)
{
	if (mgr && mgr->pipe_mgr) {
		return mgr->pipe_mgr->current_pipe_finish;
	}

	return true;
}

int eink_mgr_init(struct init_para *para)
{
	int ret = 0;
	int irq_no = 0;
	struct eink_manager *eink_mgr = NULL;

	eink_mgr = (struct eink_manager *)malloc(sizeof(struct eink_manager));
	if (!eink_mgr) {
		pr_err("%s:malloc mgr failed!\n", __func__);
		ret = -ENOMEM;
		goto eink_mgr_err;
	}

	g_eink_manager = eink_mgr;

	memset(eink_mgr, 0, sizeof(struct eink_manager));

	print_panel_info(para);

	memcpy(&eink_mgr->panel_info, &para->panel_info, sizeof(struct eink_panel_info));
	memcpy(&eink_mgr->timing_info, &para->timing_info, sizeof(struct timing_info));
	memcpy(eink_mgr->wav_path, para->wav_path, WAV_PATH_LEN);
	memcpy(eink_mgr->eink_pin_power, para->eink_pin_power, POWER_STR_LEN);

	eink_mgr->eink_update = eink_update_image;
	eink_mgr->eink_set_global_clean_cnt = eink_set_global_clean_cnt;
	eink_mgr->eink_mgr_enable = eink_mgr_enable;
	eink_mgr->eink_mgr_disable = eink_mgr_disable;
	eink_mgr->eink_fmt_cvt_img = eink_fmt_convert_image;

	eink_mgr->set_temperature = eink_set_temperature;
	eink_mgr->get_temperature = eink_get_temperature;
	eink_mgr->get_resolution = eink_get_resolution;
	eink_mgr->get_clk_rate = eink_get_clk_rate;
	eink_mgr->dump_config = eink_dump_config;
	eink_mgr->get_fps = eink_get_fps;
	eink_mgr->get_pipe_finish_status = eink_get_pipe_finish_status;

	eink_mgr->upd_pic_accept_flag = 0;
	eink_mgr->panel_temperature = 28;
	eink_mgr->waveform_init_flag = false;
	eink_mgr->enable_flag = false;

	eink_mgr->ee_clk = para->ee_clk;
	eink_mgr->panel_clk = para->panel_clk;
	eink_mgr->panel_clk_parent = clk_get_parent(eink_mgr->panel_clk);
	if (!eink_mgr->panel_clk_parent)
		printf("[%s]: panel clk parent is NULL\n", __func__);

	eink_mgr->clk_enable_flag = 0;
#ifdef REGISTER_PRINT
	para->ee_reg_base = (uintptr_t)malloc(0x1ffff);
	memset((void *)para->ee_reg_base, 0, 0x1ffff);
#endif
	eink_set_reg_base(para->ee_reg_base);

	irq_no = para->ee_irq_no;
	irq_install_handler(irq_no, (interrupt_handler_t *)eink_intterupt_proc, (void *)eink_mgr);
	irq_enable(irq_no);

	mutex_init(&eink_mgr->enable_lock);

	ret = index_mgr_init(eink_mgr);
	if (ret) {
		pr_err("%s:init index mgr failed!\n", __func__);
		goto eink_mgr_err;
	}
	ret = pipe_mgr_init(eink_mgr);
	if (ret) {
		pr_err("%s:init pipe mgr failed!\n", __func__);
		goto eink_mgr_err;
	}
	ret = timing_ctrl_mgr_init(para);
	if (ret) {
		pr_err("%s:init timing ctrl mgr failed!\n", __func__);
		goto eink_mgr_err;
	}

	return ret;
eink_mgr_err:
	free(eink_mgr);
	return ret;
}

void eink_mgr_exit(struct eink_manager *mgr)
{
	if (!mgr) {
		pr_err("[%s]mgr is NULL!\n", __func__);
		return;
	}
	pr_info("[%s]\n", __func__);
	eink_clk_disable(mgr);
	mgr->eink_mgr_disable(mgr);
	return;
}
