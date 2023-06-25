/*
 * drivers/video/sunxi/disp2/disp/de/disp_display.c
 *
 * Copyright (c) 2007-2019 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <zhengxiaobin@allwinnertech.com>
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
#include "disp_display.h"

disp_dev_t gdisp;
s32 bsp_disp_init(disp_bsp_init_para *para)
{
	u32 num_screens, disp;

	memset(&gdisp, 0x00, sizeof(disp_dev_t));
	memcpy(&gdisp.init_para, para, sizeof(disp_bsp_init_para));
	para->shadow_protect = bsp_disp_shadow_protect;
	disp_init_feat(&para->feat_init);

	num_screens = bsp_disp_feat_get_num_screens();
	for (disp = 0; disp < num_screens; disp++) {
		spin_lock_init(&gdisp.screen[disp].flag_lock);
		tasklet_init(&gdisp.screen[disp].tasklet, disp_tasklet, (unsigned long)disp);
	}

	bsp_disp_set_print_level(DEFAULT_PRINT_LEVLE);
	disp_init_al(para);
#if defined (DE_VERSION_V33X)
	disp_al_init_tcon(para);
#endif
	disp_init_lcd(para);
#if defined(SUPPORT_HDMI)
	disp_init_hdmi(para);
#endif
#if defined(SUPPORT_TV)
	disp_init_tv_para(para);
#endif

#if defined(SUPPORT_EDP) && defined(CONFIG_EDP_DISP2_SUNXI)
	disp_init_edp(para);
#endif /*endif SUPPORT_EDP */

	disp_init_mgr(para);

#if defined(CONFIG_DISP2_SUNXI_SUPPORT_ENAHNCE)
	disp_init_enhance(para);
#endif

#if defined(CONFIG_DISP2_SUNXI_SUPPORT_SMBL)
	disp_init_smbl(para);
#endif
	disp_init_capture(para);

#if defined SUPPORT_EINK
	disp_init_eink(para);
#endif
	disp_init_connections(para);

	return DIS_SUCCESS;
}

s32 bsp_disp_exit(u32 mode)
{
	//u32 num_screens;

	//num_screens = bsp_disp_feat_get_num_screens();

	if (mode == DISP_EXIT_MODE_CLEAN_ALL) {
		/* close all mod and unregister all irq */

	}	else if (mode == DISP_EXIT_MODE_CLEAN_PARTLY) {
		/* unregister all irq */
	}

	return DIS_SUCCESS;
}

s32 bsp_disp_open(void)
{
	return DIS_SUCCESS;
}

s32 bsp_disp_close(void)
{
	return DIS_SUCCESS;
}

s32 disp_device_attached(int disp_mgr, int disp_dev, enum disp_output_type output_type, enum disp_output_type mode)
{
	struct disp_manager *mgr = NULL;
	struct disp_device *dispdev = NULL;

	mgr = disp_get_layer_manager(disp_mgr);
	if (!mgr)
		return -1;

	/* no need to attch */
	if (mgr->device && (output_type == mgr->device->type))
		return 0;

	/* detach manager and device first */
	if (mgr->device) {
		dispdev = mgr->device;
		if (dispdev->is_enabled && dispdev->is_enabled(dispdev)
			&& dispdev->disable)
			dispdev->disable(dispdev);
		if (dispdev->unset_manager)
			dispdev->unset_manager(dispdev);
	}

	dispdev = disp_device_get(disp_dev, output_type);
	if (dispdev && dispdev->set_manager) {
			dispdev->set_manager(dispdev, mgr);
			DE_WRN("attached ok, mgr%d<-->device%d, type=%d\n", disp_mgr, disp_dev, (u32)output_type);
			if (dispdev->set_mode)
				dispdev->set_mode(dispdev, mode);
			return 0;
	}

	return -1;
}

s32 disp_device_attached_and_enable(int disp_mgr, int disp_dev,
				    struct disp_device_config *config)
{
	struct disp_manager *mgr = NULL;
	struct disp_device *dispdev = NULL;
	int ret = 0;

	mgr = disp_get_layer_manager(disp_mgr);
	if (!mgr)
		return -1;

	if (mgr->device && mgr->device->type != config->type) {
		if (mgr->device->is_enabled(mgr->device))
			mgr->device->disable(mgr->device);

		if (mgr->device->unset_manager)
			mgr->device->unset_manager(mgr->device);
	}

	if ((!mgr->device) && (config->type != DISP_OUTPUT_TYPE_NONE)) {
		dispdev = disp_device_get(disp_dev, config->type);
		if (dispdev && dispdev->set_manager) {
			dispdev->set_manager(dispdev, mgr);
		} else {
			ret = -1;
			goto exit;
		}
	}

	if (mgr->device) {
		bool update = true;

		if (mgr->device->check_config_dirty)
			update = mgr->device->check_config_dirty(mgr->device,
								 config);

		if (update) {
			__attribute__((unused)) static char const *fmt_name[] = {
				"rgb",
				"yuv444",
				"yuv422",
				"yuv420"
			};
			__attribute__((unused)) static char const *bits_name[] = {
				"8bits",
				"10bits",
				"12bits",
				"16bits"
			};
			if (mgr->device->is_enabled(mgr->device))
				mgr->device->disable(mgr->device);

			if (mgr->device->set_static_config)
				ret = mgr->device->set_static_config(mgr->device,
							config);
			if (ret != 0)
				goto exit;

			if (config->type == DISP_OUTPUT_TYPE_TV)
				disp_delay_ms(300);
			ret = mgr->device->enable(mgr->device);
			DE_INF("attached %s, mgr%d<-->dev%d\n",
			       (ret == 0) ? "ok" : "fail",
				disp_mgr, disp_dev);
			DE_INF("type,mode,fmt,bits,eotf,cs=%d,%d,%s,%s,%d,%d\n",
			       config->type,
				config->mode,
				(config->format < 4) ?
				    fmt_name[config->format] : "undef",
				(config->bits < 4) ?
				    bits_name[config->bits] : "undef",
				config->eotf,
				config->cs);
			if (ret != 0)
				goto exit;
		}
	}

	return 0;

exit:
	return ret;
}

s32 disp_device_detach(int disp_mgr, int disp_dev, enum disp_output_type output_type)
{
	struct disp_manager *mgr = NULL;
	struct disp_device *dispdev = NULL;

	mgr = disp_get_layer_manager(disp_mgr);
	if (!mgr) {
		DE_WRN("get mgr%d fail\n", disp_mgr);
		return -1;
	}

	dispdev = disp_device_get(disp_dev, output_type);
	if (!dispdev) {
		DE_WRN("get device%d fail\n", disp_dev);
		return -1;
	}

	if (mgr->device == dispdev) {
		if (dispdev->disable)
			dispdev->disable(dispdev);
		if (dispdev->unset_manager)
			dispdev->unset_manager(dispdev);
	}

	return 0;
}

s32 bsp_disp_device_switch(int disp, enum disp_output_type output_type, enum disp_output_type mode)
{
	int num_screens = 0;
	int disp_dev;
	int ret = -1;
	struct disp_device_config config;

	config.type = output_type;
	config.mode = mode;
	config.format = (output_type == DISP_OUTPUT_TYPE_LCD) ?
			DISP_CSC_TYPE_RGB : DISP_CSC_TYPE_YUV444;
	config.bits = DISP_DATA_8BITS;
	config.eotf = DISP_EOTF_GAMMA22;
	config.cs = DISP_UNDEF;

	ret = disp_device_attached_and_enable(disp, disp, &config);
	if (ret != 0) {
		num_screens = bsp_disp_feat_get_num_screens();
		for (disp_dev = 0; disp_dev < num_screens; disp_dev++) {
			ret = disp_device_attached_and_enable(disp,
							      disp_dev,
							      &config);
			if (ret == 0)
				break;
		}
	}

	return ret;
}

s32 bsp_disp_device_set_config(int disp, struct disp_device_config *config)
{
	int num_screens = 0;
	int disp_dev;
	int ret = -1;

	ret = disp_device_attached_and_enable(disp, disp, config);
	if (ret != 0) {
		num_screens = bsp_disp_feat_get_num_screens();
		for (disp_dev = 0; disp_dev < num_screens; disp_dev++) {
			ret = disp_device_attached_and_enable(disp,
							      disp_dev,
							      config);
			if (ret == 0)
				break;
		}
	}

	return ret;
}

#ifdef CONFIG_EINK_PANEL_USED
s32 bsp_disp_eink_update(struct disp_eink_manager *manager, struct eink_8bpp_image *cimage)
{
	int ret = -1;
	struct eink_8bpp_image current_image;

	memcpy(&current_image, cimage, sizeof(struct eink_8bpp_image));
	if (manager)
		ret = manager->eink_update(manager, &current_image);

	return ret;
}
#endif
s32 disp_init_connections(disp_bsp_init_para *para)
{
	u32 disp = 0;
	u32 num_screens = 0;
	u32 num_layers = 0, layer_id = 0;
	u32 i = 0;

	DE_INF("disp_init_connections\n");

	num_screens = bsp_disp_feat_get_num_screens();
	for (disp = 0; disp < num_screens; disp++) {
		struct disp_manager *mgr;
		struct disp_layer *lyr;
		struct disp_device *dispdev = NULL;
#if defined(CONFIG_DISP2_SUNXI_SUPPORT_ENAHNCE)
		struct disp_enhance *enhance = NULL;
#endif
#if defined(CONFIG_DISP2_SUNXI_SUPPORT_SMBL)
		struct disp_smbl *smbl = NULL;
#endif
		struct disp_capture *cptr = NULL;

		mgr = disp_get_layer_manager(disp);
		if (!mgr)
			continue;

		/* connect layer & it's manager */
		num_layers = bsp_disp_feat_get_num_layers(disp);
		for (layer_id = 0; layer_id < num_layers; layer_id++) {
			lyr = disp_get_layer_1(disp, layer_id);
			if (lyr) {
				lyr->set_manager(lyr, mgr);
			}
		}

		if ((para->boot_info.sync == 1) && (disp == para->boot_info.disp)
			&& (para->boot_info.type == DISP_OUTPUT_TYPE_LCD)) {
			/* connect device & it's manager */
			dispdev = disp_device_get(disp, DISP_OUTPUT_TYPE_LCD);
			if ((dispdev) && (dispdev->set_manager)) {
				dispdev->set_manager(dispdev, mgr);
			} else {
				for (i = 0; i < num_screens; i++) {
					dispdev = disp_device_get(i, DISP_OUTPUT_TYPE_LCD);
					if ((dispdev) && (dispdev->set_manager))
						dispdev->set_manager(dispdev, mgr);
				}
			}
		} else if (para->boot_info.sync == 0) {
			dispdev = disp_device_get(disp, DISP_OUTPUT_TYPE_LCD);
			if ((dispdev) && (dispdev->set_manager))
				dispdev->set_manager(dispdev, mgr);
		}

#if defined(CONFIG_DISP2_SUNXI_SUPPORT_ENAHNCE)
		enhance = disp_get_enhance(disp);
		if (enhance && (enhance->set_manager)) {
			enhance->set_manager(enhance, mgr);
		}
#endif

#if defined(CONFIG_DISP2_SUNXI_SUPPORT_SMBL)
		smbl = disp_get_smbl(disp);
		if (smbl && (smbl->set_manager)) {
			smbl->set_manager(smbl, mgr);
		}
#endif

		cptr = disp_get_capture(disp);
		if (cptr && (cptr->set_manager)) {
			cptr->set_manager(cptr, mgr);
		}
	}

	return 0;
}

s32 bsp_disp_sync_with_hw(disp_bsp_init_para *para)
{
	if ((para->boot_info.sync == 1)
		&& (para->boot_info.type != DISP_OUTPUT_TYPE_NONE)) {
		int num_screens = 0;
		int disp = para->boot_info.disp;
		int disp_dev = disp;
		enum disp_output_type type = (enum disp_output_type)para->boot_info.type;
		enum disp_output_type mode = (enum disp_output_type)para->boot_info.mode;
		int ret = -1;
		struct disp_manager *mgr = NULL;

		mgr = disp_get_layer_manager(disp);
		if (!mgr)
			return -1;

		/* attach manager and display device */
		ret = disp_device_attached(disp, disp_dev, type, mode);
		if (ret != 0) {
			num_screens = bsp_disp_feat_get_num_screens();
			for (disp_dev = 0; disp_dev < num_screens; disp_dev++) {
				ret = disp_device_attached(disp, disp_dev, type, mode);
				if (ret == 0)
					break;
			}
		}

		/* enable display device(only software) */
		if (ret != 0) {
			/* attach fail */
			DE_WRN("something error, can't find device(%d) for manager %d\n",
			       (int)type, disp);
			return -1;
		}
		if (mgr->device && mgr->device->sw_enable) {
			if (mgr->device->set_mode)
				mgr->device->set_mode(mgr->device, mode);
			return mgr->device->sw_enable(mgr->device);
		}
	}

	return -1;
}

/***********************************************************
 *
 * interrupt proc
 *
 ***********************************************************/
static s32 bsp_disp_cfg_get(u32 disp)
{
	return gdisp.screen[disp].cfg_cnt;
}

s32 bsp_disp_shadow_protect(u32 disp, bool protect)
{
	s32 ret = -1;
	u32 cnt = 0;
	u32 max_cnt = 50;
	u32 delay = 10;//us
	//struct disp_video_timings tt;
	struct disp_manager *mgr = NULL;
	unsigned long flags;

	__inf("sel=%d, protect:%d,  cnt=%d\n", disp, protect, gdisp.screen[disp].cfg_cnt);

	mgr = disp_get_layer_manager(disp);
	if (mgr && mgr->device) {
		if (mgr->device->type == DISP_OUTPUT_TYPE_LCD) {
			//FIXME
		} else if (mgr->device->type == DISP_OUTPUT_TYPE_HDMI) {
			//FIXME
		}
	}

	if (protect) {
		while ((ret != 0) && (cnt < max_cnt)) {
			spin_lock_irqsave(&gdisp.screen[disp].flag_lock, flags);
			cnt++;
			if (gdisp.screen[disp].have_cfg_reg == false) {
				gdisp.screen[disp].cfg_cnt++;
				ret = 0;
			}
			spin_unlock_irqrestore(&gdisp.screen[disp].flag_lock, flags);
			if (ret != 0)
				disp_delay_us(delay);
		}

		if (ret != 0) {
			DE_INF("wait for reg load finish time out\n");
			spin_lock_irqsave(&gdisp.screen[disp].flag_lock, flags);
			gdisp.screen[disp].cfg_cnt++;
			spin_unlock_irqrestore(&gdisp.screen[disp].flag_lock, flags);
		}
	} else {
			spin_lock_irqsave(&gdisp.screen[disp].flag_lock, flags);
			gdisp.screen[disp].cfg_cnt--;
			spin_unlock_irqrestore(&gdisp.screen[disp].flag_lock, flags);
	}
	__inf("sel=%d, protect:%d,  cnt=%d\n", disp, protect, gdisp.screen[disp].cfg_cnt);
	return DIS_SUCCESS;
}

s32 bsp_disp_vsync_event_enable(u32 disp, bool enable)
{
	gdisp.screen[disp].vsync_event_en = enable;

	return DIS_SUCCESS;
}

static s32 disp_sync_all(u32 disp)
{
	struct disp_manager *mgr;
	struct disp_device *dispdev;

	mgr = disp_get_layer_manager(disp);
	if (!mgr) {
		DE_WRN("get mgr%d fail\n", disp);
	} else {
		dispdev = mgr->device;
		if (mgr->sync)
			mgr->sync(mgr);
		if (dispdev && dispdev->get_status) {
			if (dispdev->get_status(dispdev) != 0)
				gdisp.screen[disp].health_info.error_cnt++;
		}
	}

	return 0;
}

//return 10fps
s32 bsp_disp_get_fps(u32 disp)
{
	u32 pre_time_index, cur_time_index;
	u32 pre_time, cur_time;
	u32 fps = 0xff;

	pre_time_index = gdisp.screen[disp].health_info.sync_time_index;
	cur_time_index = (pre_time_index == 0) ? (DEBUG_TIME_SIZE - 1) : (pre_time_index - 1);

	pre_time = gdisp.screen[disp].health_info.sync_time[pre_time_index];
	cur_time = gdisp.screen[disp].health_info.sync_time[cur_time_index];

	if (pre_time != cur_time) {
		fps = 10000 * DEBUG_TIME_SIZE / (cur_time - pre_time);
	}

	return fps;
}

static void disp_sync_checkin(u32 disp)
{
	u32 index = gdisp.screen[disp].health_info.sync_time_index;
	gdisp.screen[disp].health_info.sync_time[index] = get_timer_masked();/*msecond*/
	index++;
	index = (index >= DEBUG_TIME_SIZE) ? 0 : index;
	gdisp.screen[disp].health_info.sync_time_index = index;
}

s32 bsp_disp_get_health_info(u32 disp, disp_health_info *info)
{
	if (info)
		memcpy(info, &gdisp.screen[disp].health_info, sizeof(disp_health_info));
	return 0;
}


void sync_event_proc(u32 disp, bool timeout)
{
	unsigned long flags;
	if (!timeout)
		disp_sync_checkin(disp);
	else
		gdisp.screen[disp].health_info.skip_cnt++;
	gdisp.screen[disp].health_info.irq_cnt++;

	spin_lock_irqsave(&gdisp.screen[disp].flag_lock, flags);
	if ((bsp_disp_cfg_get(disp) == 0) && (!timeout)) {
		gdisp.screen[disp].have_cfg_reg = true;
	spin_unlock_irqrestore(&gdisp.screen[disp].flag_lock, flags);
		disp_sync_all(disp);
		gdisp.screen[disp].have_cfg_reg = false;
		if (gdisp.init_para.disp_int_process)
			gdisp.init_para.disp_int_process(disp);

	} else {
	spin_unlock_irqrestore(&gdisp.screen[disp].flag_lock, flags);
	}

	if (gdisp.screen[disp].vsync_event_en && gdisp.init_para.vsync_event) {
		gdisp.init_para.vsync_event(disp);
		gdisp.screen[disp].health_info.vsync_cnt++;
	}

	if (!disp_feat_is_using_rcq(disp))
		tasklet_schedule(&gdisp.screen[disp].tasklet);

	return;
}

s32 bsp_disp_get_output_type(u32 disp)
{
	struct disp_manager *mgr = disp_get_layer_manager(disp);

	if (mgr) {
		struct disp_device *dispdev = mgr->device;

		if (dispdev && dispdev->is_enabled && dispdev->is_enabled(dispdev))
			return dispdev->type;
	}

	return DISP_OUTPUT_TYPE_NONE;
}

void disp_tasklet(unsigned long data)
{
	struct disp_manager *mgr;
	u32 disp = (u32)data;

	mgr = disp_get_layer_manager(disp);
	if (!mgr) {
		DE_WRN("get mgr%d fail\n", disp);
	} else {
		if (mgr->tasklet)
			mgr->tasklet(mgr);
	}
}

#ifdef CONFIG_DEVFREQ_DRAM_FREQ_IN_VSYNC
s32 bsp_disp_get_vb_time(void)
{
	u32 num_screens, screen_id;
	struct disp_video_timings tt;
	u32 vb_time = 0;

	num_screens = bsp_disp_feat_get_num_screens();
	for (screen_id = 0; screen_id < num_screens; screen_id++) {
		if (bsp_disp_get_output_type(screen_id) == DISP_OUTPUT_TYPE_LCD) {
			struct disp_device *lcd;
			u32 time_per_line = 0;
			u32 start_delay = 0;

			lcd = disp_get_lcd(screen_id);
			if (!lcd) {
				DE_WRN("get lcd%d fail\n", screen_id);
			}

			if (lcd && lcd->get_timings) {
				u32 fps = 0;

				lcd->get_timings(lcd, &tt);
				if ((tt.ver_total_time != 0) && (tt.hor_total_time != 0))
					fps = tt.pixel_clk * 1000 / (tt.ver_total_time * tt.hor_total_time);
				start_delay = tt.ver_total_time - tt.y_res - 10;
				fps = (fps == 0) ? 60 : fps;
				time_per_line = 1000000 / fps / tt.ver_total_time;
				vb_time = (start_delay) * time_per_line;

			} else if (bsp_disp_get_output_type(screen_id) == DISP_OUTPUT_TYPE_HDMI) {
				/* FIXME: add hdmi */
			}
		}
	}
	return vb_time;
}

/* returns: us */
s32 bsp_disp_get_next_vb_time(void)
{
	u32 cur_line;
	u32 num_screens, screen_id;
	struct disp_video_timings tt;
	u32 next_time = 16000;

	num_screens = bsp_disp_feat_get_num_screens();
	for (screen_id = 0; screen_id < num_screens; screen_id++) {
		if (bsp_disp_get_output_type(screen_id) == DISP_OUTPUT_TYPE_LCD) {
			struct disp_device *lcd;
			u32 time_per_line = 0;
			disp_panel_para info;

			memset(&info, 0, sizeof(disp_panel_para));
			lcd = disp_get_lcd(screen_id);
			if (lcd && lcd->get_panel_info)
				lcd->get_panel_info(lcd, &info);
			cur_line = disp_al_lcd_get_cur_line(screen_id, &info);
			if (!lcd) {
				DE_WRN("get lcd%d fail\n", screen_id);
			}
			if (info.lcd_if != LCD_IF_EDP) {
				if (lcd && lcd->get_timings) {
					u32 fps = 0;

					lcd->get_timings(lcd, &tt);
					if ((tt.ver_total_time != 0) && (tt.hor_total_time != 0))
						fps = tt.pixel_clk * 1000 / (tt.ver_total_time * tt.hor_total_time);
					fps = (fps == 0) ? 60 : fps;
					time_per_line = 1000000 / fps / tt.ver_total_time;
					next_time = (tt.ver_total_time - cur_line) * time_per_line;
				}
			}
		} else if (bsp_disp_get_output_type(screen_id) == DISP_OUTPUT_TYPE_HDMI) {
			/* FIXME: add hdmi */
		}
	}
	return next_time;
}

s32 bsp_disp_is_in_vb(void)
{
	u32 num_screens, screen_id;
	s32 ret = 1;

	num_screens = bsp_disp_feat_get_num_screens();
	for (screen_id = 0; screen_id < num_screens; screen_id++) {
		if (bsp_disp_get_output_type(screen_id) == DISP_OUTPUT_TYPE_LCD) {
			struct disp_device *lcd;
			disp_panel_para info;

			lcd = disp_get_lcd(screen_id);
			if (!lcd) {
				DE_WRN("get lcd%d fail\n", screen_id);
			}
			memset(&info, 0, sizeof(disp_panel_para));
			if (lcd && lcd->get_panel_info)
				lcd->get_panel_info(lcd, &info);
			ret = disp_al_lcd_query_irq(screen_id, LCD_IRQ_TCON0_VBLK, &info);
		} else if (bsp_disp_get_output_type(screen_id) == DISP_OUTPUT_TYPE_HDMI) {
			/* FIXME: add hdmi */
		}
	}
	return ret;
}
#endif

s32 bsp_disp_set_print_level(u32 print_level)
{
	gdisp.print_level = print_level;

	return 0;
}

s32 bsp_disp_get_print_level(void)
{
	return gdisp.print_level;
}

u32 bsp_disp_get_screen_physical_width(u32 disp)
{
	u32 width = 0, height = 0;
	struct disp_manager *mgr = NULL;

	mgr = disp_get_layer_manager(disp);
	if (mgr && mgr->device && mgr->device->get_dimensions) {
		mgr->device->get_dimensions(mgr->device, &width, &height);
	}

	return width;
}

u32 bsp_disp_get_screen_physical_height(u32 disp)
{
	u32 width = 0, height = 0;
	struct disp_manager *mgr = NULL;

	mgr = disp_get_layer_manager(disp);
	if (mgr && mgr->device && mgr->device->get_dimensions) {
		mgr->device->get_dimensions(mgr->device, &width, &height);
	}

	return height;
}

s32 bsp_disp_get_screen_width(u32 disp)
{
	s32 width = 0;
	//FIXME
	return width;
}

s32 bsp_disp_get_screen_height(u32 disp)
{
	s32 height = 0;
	//FIXME

	return height;
}

s32 bsp_disp_get_screen_width_from_output_type(u32 disp, u32 output_type, u32 output_mode)
{
	u32 width = 800, height = 480;
	struct disp_device *dispdev;

	if (output_type == DISP_OUTPUT_TYPE_LCD) {
		struct disp_manager *mgr;

		mgr = disp_get_layer_manager(disp);
		if (mgr && mgr->device && mgr->device->get_resolution) {
			mgr->device->get_resolution(mgr->device, &width, &height);
		}
	} else if (output_type == DISP_OUTPUT_TYPE_EDP) {
		dispdev = disp_device_get(disp, DISP_OUTPUT_TYPE_EDP);
		if (dispdev)
			dispdev->get_resolution(dispdev, &width, &height);
	} else if ((output_type == DISP_OUTPUT_TYPE_HDMI)
			|| (output_type == DISP_OUTPUT_TYPE_TV)
			|| (output_type == DISP_OUTPUT_TYPE_VGA)) {
		switch (output_mode) {
		case DISP_TV_MOD_NTSC:
		case DISP_TV_MOD_480I:
		case DISP_TV_MOD_480P:
			width = 720;
			height = 480;
			break;
		case DISP_TV_MOD_PAL:
		case DISP_TV_MOD_576I:
		case DISP_TV_MOD_576P:
			width = 720;
			height = 576;
			break;
		case DISP_TV_MOD_720P_50HZ:
		case DISP_TV_MOD_720P_60HZ:
			width = 1280;
			height = 720;
			break;
		case DISP_TV_MOD_1080P_50HZ:
		case DISP_TV_MOD_1080P_60HZ:
		case DISP_TV_MOD_1080P_30HZ:
		case DISP_TV_MOD_1080P_25HZ:
		case DISP_TV_MOD_1080P_24HZ:
		case DISP_TV_MOD_1080I_50HZ:
		case DISP_TV_MOD_1080I_60HZ:
			width = 1920;
			height = 1080;
			break;
		case DISP_TV_MOD_3840_2160P_30HZ:
		case DISP_TV_MOD_3840_2160P_25HZ:
		case DISP_TV_MOD_3840_2160P_24HZ:
			width = 3840;
			height = 2160;
			break;
		case DISP_TV_MOD_4096_2160P_24HZ:
			width = 4096;
			height = 2160;
			break;
		case DISP_VGA_MOD_800_600P_60:
			width = 800;
			height = 600;
			break;
		case DISP_VGA_MOD_1024_768P_60:
			width = 1024;
			height = 768;
			break;
		case DISP_VGA_MOD_1280_768P_60:
			width = 1280;
			height = 768;
			break;
		case DISP_VGA_MOD_1280_800P_60:
			width = 1280;
			height = 800;
			break;
		case DISP_VGA_MOD_1366_768P_60:
			width = 1366;
			height = 768;
			break;
		case DISP_VGA_MOD_1440_900P_60:
			width = 1440;
			height = 900;
			break;
		case DISP_VGA_MOD_1920_1080P_60:
			width = 1920;
			height = 1080;
			break;
		case DISP_VGA_MOD_1920_1200P_60:
			width = 1920;
			height = 1200;
			break;
		case DISP_VGA_MOD_1280_720P_60:
			width = 1280;
			height = 720;
			break;
		case DISP_VGA_MOD_1600_900P_60:
			width = 1600;
			height = 900;
			break;
		case DISP_TV_MOD_1280_1024P_60HZ:
			width = 1280;
			height = 1024;
			break;
		case DISP_TV_MOD_1024_768P_60HZ:
			width = 1024;
			height = 768;
			break;
		case DISP_TV_MOD_900_540P_60HZ:
			width = 900;
			height = 540;
			break;
		}
	}
	/* FIXME: add other output device res */

	return width;
}

s32 bsp_disp_get_screen_height_from_output_type(u32 disp, u32 output_type, u32 output_mode)
{
	u32 width = 800, height = 480;
	struct disp_device *dispdev;

	if (output_type == DISP_OUTPUT_TYPE_LCD) {
		struct disp_manager *mgr;

		mgr = disp_get_layer_manager(disp);
		if (mgr && mgr->device && mgr->device->get_resolution) {
			mgr->device->get_resolution(mgr->device, &width, &height);
		}
	} else if (output_type == DISP_OUTPUT_TYPE_EDP) {
		dispdev = disp_device_get(disp, DISP_OUTPUT_TYPE_EDP);
		if (dispdev)
			dispdev->get_resolution(dispdev, &width, &height);
	} else if ((output_type == DISP_OUTPUT_TYPE_HDMI)
			|| (output_type == DISP_OUTPUT_TYPE_TV)
			|| (output_type == DISP_OUTPUT_TYPE_VGA)) {
		switch (output_mode) {
		case DISP_TV_MOD_NTSC:
		case DISP_TV_MOD_480I:
		case DISP_TV_MOD_480P:
			width = 720;
			height = 480;
			break;
		case DISP_TV_MOD_PAL:
		case DISP_TV_MOD_576I:
		case DISP_TV_MOD_576P:
			width = 720;
			height = 576;
			break;
		case DISP_TV_MOD_720P_50HZ:
		case DISP_TV_MOD_720P_60HZ:
			width = 1280;
			height = 720;
			break;
		case DISP_TV_MOD_1080P_50HZ:
		case DISP_TV_MOD_1080P_60HZ:
		case DISP_TV_MOD_1080P_30HZ:
		case DISP_TV_MOD_1080P_25HZ:
		case DISP_TV_MOD_1080P_24HZ:
		case DISP_TV_MOD_1080I_50HZ:
		case DISP_TV_MOD_1080I_60HZ:
			width = 1920;
			height = 1080;
			break;
		case DISP_TV_MOD_3840_2160P_30HZ:
		case DISP_TV_MOD_3840_2160P_25HZ:
		case DISP_TV_MOD_3840_2160P_24HZ:
			width = 3840;
			height = 2160;
			break;
		case DISP_TV_MOD_4096_2160P_24HZ:
			width = 4096;
			height = 2160;
			break;
		case DISP_VGA_MOD_800_600P_60:
			width = 800;
			height = 600;
			break;
		case DISP_VGA_MOD_1024_768P_60:
			width = 1024;
			height = 768;
			break;
		case DISP_VGA_MOD_1280_768P_60:
			width = 1280;
			height = 768;
			break;
		case DISP_VGA_MOD_1280_800P_60:
			width = 1280;
			height = 800;
			break;
		case DISP_VGA_MOD_1366_768P_60:
			width = 1366;
			height = 768;
			break;
		case DISP_VGA_MOD_1440_900P_60:
			width = 1440;
			height = 900;
			break;
		case DISP_VGA_MOD_1920_1080P_60:
			width = 1920;
			height = 1080;
			break;
		case DISP_VGA_MOD_1920_1200P_60:
			width = 1920;
			height = 1200;
			break;
		case DISP_VGA_MOD_1280_720P_60:
			width = 1280;
			height = 720;
			break;
		case DISP_VGA_MOD_1600_900P_60:
			width = 1600;
			height = 900;
			break;
		case DISP_TV_MOD_1280_1024P_60HZ:
			width = 1280;
			height = 1024;
			break;
		case DISP_TV_MOD_1024_768P_60HZ:
			width = 1024;
			height = 768;
			break;
		case DISP_TV_MOD_900_540P_60HZ:
			width = 900;
			height = 540;
			break;
		}
	}
	/* FIXME: add other output device res */

	return height;
}

s32 bsp_disp_set_hdmi_func(struct disp_device_func *func)
{
		u32 disp = 0;
	u32 num_screens = 0;
	s32 ret = 0, registered_cnt = 0;

	num_screens = bsp_disp_feat_get_num_screens();
	for (disp = 0; disp < num_screens; disp++) {
		struct disp_device *hdmi;

		hdmi = disp_device_find(disp, DISP_OUTPUT_TYPE_HDMI);
		if (hdmi) {
			ret = hdmi->set_func(hdmi, func);
			if (ret == 0)
				registered_cnt++;
		}
	}

	if (registered_cnt != 0) {
		DE_INF("registered!!\n");
		gdisp.hdmi_registered = 1;
		if (gdisp.init_para.start_process)
			gdisp.init_para.start_process();

		return 0;
	}

	return -1;
}

s32 bsp_disp_set_edp_func(struct disp_tv_func *func)
{
	u32 disp = 0;
	u32 num_screens = 0;
	s32 ret = 0, registered_cnt = 0;

	num_screens = bsp_disp_feat_get_num_screens();
	for (disp = 0; disp < num_screens; disp++) {
		struct disp_device *edp;

		edp = disp_device_find(disp, DISP_OUTPUT_TYPE_EDP);
		if (edp) {
			if (edp->set_tv_func)
				ret = edp->set_tv_func(edp, func);
			if (ret == 0)
				registered_cnt++;
		}
	}

	if (registered_cnt != 0) {
		DE_INF("edp registered!!\n");
		gdisp.edp_registered = 1;
		if (gdisp.init_para.start_process)
			gdisp.init_para.start_process();

		return 0;
	}

	return -1;
}

s32 bsp_disp_hdmi_get_support_mode(u32 disp, u32 init_mode)
{
	u32 num_screens = 0;
	s32 ret = -1;

	num_screens = bsp_disp_feat_get_num_screens();
	for (disp = 0; disp < num_screens; disp++) {
		struct disp_device *hdmi;

		hdmi = disp_device_find(disp, DISP_OUTPUT_TYPE_HDMI);
		if (hdmi && hdmi->get_support_mode) {
			ret = hdmi->get_support_mode(hdmi, init_mode);
			break;
		}
	}

	return ret;
}

s32 bsp_disp_hdmi_get_work_mode(u32 disp)
{
	u32 num_screens = 0;
	s32 ret = -1;

	num_screens = bsp_disp_feat_get_num_screens();
	for (disp = 0; disp < num_screens; disp++) {
		struct disp_device *hdmi;

		hdmi = disp_device_find(disp, DISP_OUTPUT_TYPE_HDMI);
		if (hdmi && hdmi->get_work_mode) {
			ret = hdmi->get_work_mode(hdmi);
			break;
		}
	}

	return ret;
}

s32 bsp_disp_hdmi_check_support_mode(u32 disp, enum disp_output_type mode)
{
	u32 num_screens = 0;
	s32 ret = 0;

	num_screens = bsp_disp_feat_get_num_screens();
	for (disp = 0; disp < num_screens; disp++) {
		struct disp_device *hdmi;

		hdmi = disp_device_find(disp, DISP_OUTPUT_TYPE_HDMI);
		if (hdmi && hdmi->check_support_mode) {
			ret = hdmi->check_support_mode(hdmi, (u32)mode);
			break;
		}
	}

	return ret;
}

s32 bsp_disp_hdmi_get_hpd_status(u32 disp)
{
	u32 num_screens = 0;
	s32 ret = 0;

	num_screens = bsp_disp_feat_get_num_screens();
	for (disp = 0; disp < num_screens; disp++) {
		struct disp_device *hdmi;

		hdmi = disp_device_find(disp, DISP_OUTPUT_TYPE_HDMI);
		if (hdmi && hdmi->detect) {
			ret = hdmi->detect(hdmi);
			break;
		}
	}

	return ret;
}

s32 bsp_disp_tv_get_hpd_status(u32 disp)
{
	u32 num_screens = 0;
	s32 ret = 0;

	num_screens = bsp_disp_feat_get_num_screens();
	for (disp = 0; disp < num_screens; disp++) {
		struct disp_device *tv;

		tv = disp_device_find(disp, DISP_OUTPUT_TYPE_TV);
		if (tv && tv->detect) {
			ret = tv->detect(tv);
			break;
		}
	}

	return ret;
}

s32 bsp_disp_hdmi_set_detect(bool hpd)
{
	u32 num_screens = 0;
	u32 disp;
	s32 ret = 0;

	num_screens = bsp_disp_feat_get_num_screens();
	for (disp = 0; disp < num_screens; disp++) {
		struct disp_device *hdmi;

		hdmi = disp_device_find(disp, DISP_OUTPUT_TYPE_HDMI);
		if (hdmi && hdmi->set_detect) {
			ret = hdmi->set_detect(hdmi, hpd);
			break;
		}
	}

	return ret;
}

s32 bsp_disp_hdmi_get_color_format(void)
{
	u32 num_screens = 0;
	s32 ret = 0;
	u32 disp;

	num_screens = bsp_disp_feat_get_num_screens();
	for (disp = 0; disp < num_screens; disp++) {
		struct disp_device *hdmi;

		hdmi = disp_device_find(disp, DISP_OUTPUT_TYPE_HDMI);
		if (hdmi && hdmi->get_input_csc) {
			ret = hdmi->get_input_csc(hdmi);
			break;
		}
	}

	return ret;

}

s32 bsp_disp_tv_set_hpd(u32 state)
{
#if defined SUPPORT_TV

	u32 disp = 0, num_screens = 0;
	s32 ret = 0;
	struct disp_device *ptv = NULL;

	num_screens = bsp_disp_feat_get_num_screens();
	for (disp = 0; disp < num_screens; disp++) {
		ptv = disp_device_find(disp, DISP_OUTPUT_TYPE_TV);
		if (ptv) {
			ret = disp_tv_set_hpd(ptv, state);
		} else {
			ret &= ret;
			//DE_WRN("'ptv is null\n");
			continue;
		}
	}

	if (ret != 0) {
		DE_WRN("'tv set hpd is fail!\n");
		return -1;
	}

#endif

	return 0;
}

s32 bsp_disp_tv_register(struct disp_tv_func *func)
{
#if defined SUPPORT_TV
	u32 disp = 0;
	u32 num_screens = 0;
	s32 ret = 0, registered_cnt = 0;
	struct disp_device *dispdev = NULL;

	num_screens = bsp_disp_feat_get_num_screens();
	disp_init_tv();
	for (disp = 0; disp < num_screens; disp++) {
		dispdev = disp_device_find(disp, DISP_OUTPUT_TYPE_TV);
		if (dispdev && dispdev->set_tv_func) {
			ret = dispdev->set_tv_func(dispdev, func);
			if (ret == 0)
			registered_cnt++;
		}
	}
#if defined(SUPPORT_VGA)
	disp_init_vga();
	for (disp = 0; disp < num_screens; disp++) {
		dispdev = disp_device_find(disp, DISP_OUTPUT_TYPE_VGA);
		if (dispdev && dispdev->set_tv_func) {
			ret = dispdev->set_tv_func(dispdev, func);
			if (ret == 0)
				registered_cnt++;
		}
	}
#endif
	if (registered_cnt != 0) {
		gdisp.tv_registered = 1;
		if (gdisp.init_para.start_process) {
			gdisp.init_para.start_process();
		}
		return 0;
	}
#endif
	return 0;
}

disp_lcd_flow *bsp_disp_lcd_get_open_flow(u32 disp)
{
	struct disp_device *lcd;
	disp_lcd_flow *flow = NULL;

	lcd = disp_get_lcd(disp);
	if (lcd && lcd->get_open_flow)
		flow = lcd->get_open_flow(lcd);

	return flow;
}

disp_lcd_flow *bsp_disp_lcd_get_close_flow(u32 disp)
{
	struct disp_device *lcd;
	disp_lcd_flow *flow = NULL;

	lcd = disp_get_lcd(disp);
	if (lcd && lcd->get_close_flow)
		flow = lcd->get_close_flow(lcd);

	return flow;
}

s32 bsp_disp_lcd_set_panel_funs(char *name, disp_lcd_panel_fun *lcd_cfg)
{
	struct disp_device *lcd;
	u32 num_screens;
	u32 screen_id;
	u32 registered_cnt = 0;
	u32 num_compat_cnt;
	num_screens = bsp_disp_feat_get_num_screens();
	for (screen_id = 0; screen_id < num_screens; screen_id++) {
		lcd = disp_get_lcd(screen_id);
		if (lcd && (lcd->set_panel_func)) {
			if (!lcd->set_panel_func(lcd, name, lcd_cfg)) {
				gdisp.lcd_registered[screen_id] = 1;
				registered_cnt++;
				DE_INF("panel driver %s register\n", name);
			}
		}
	}
	/*now only support lcd0*/
	num_compat_cnt = disp_get_compat_lcd_panel_num(0);
	for (screen_id = 1; screen_id <= num_compat_cnt; screen_id++) {
		lcd = disp_get_direct_lcd_compat(0, screen_id);
		if (lcd && (lcd->set_panel_func)) {
			if (!lcd->set_panel_func(lcd, name, lcd_cfg)) {
				registered_cnt++;
				DE_INF("panel driver %s register for compatible usage\n", name);
			}
		}
	}
	return 0;
}

void LCD_OPEN_FUNC(u32 disp, LCD_FUNC func, u32 delay)
{
	struct disp_device *lcd;

	lcd = disp_get_lcd(disp);

	if (lcd && lcd->set_open_func) {
		lcd->set_open_func(lcd, func, delay);
	}
}

void LCD_CLOSE_FUNC(u32 disp, LCD_FUNC func, u32 delay)
{
	struct disp_device *lcd;

	lcd = disp_get_lcd(disp);

	if (lcd && lcd->set_close_func) {
		lcd->set_close_func(lcd, func, delay);
	}
}

s32 bsp_disp_get_lcd_registered(u32 disp)
{
	return gdisp.lcd_registered[disp];
}

s32 bsp_disp_get_hdmi_registered(void)
{
	return gdisp.hdmi_registered;
}

s32 bsp_disp_get_tv_registered(void)
{
	return gdisp.tv_registered;
}

s32 bsp_disp_lcd_backlight_enable(u32 disp)
{
	s32 ret = -1;
	struct disp_device *lcd;

	lcd = disp_get_lcd(disp);
	if (!lcd)
		return ret;

	if (lcd->backlight_enable) {
		ret = lcd->backlight_enable(lcd);
	}

	return ret;
}

s32 bsp_disp_lcd_backlight_disable(u32 disp)
{
	s32 ret = -1;
	struct disp_device *lcd;

	lcd = disp_get_lcd(disp);
	if (!lcd)
		return ret;

	if (lcd && lcd->backlight_disable) {
		ret = lcd->backlight_disable(lcd);
	}

	return ret;
}

s32 bsp_disp_lcd_pwm_enable(u32 disp)
{
	s32 ret = -1;
	struct disp_device *lcd;

	lcd = disp_get_lcd(disp);
	if (!lcd)
		return ret;

	if (lcd && lcd->pwm_enable) {
		ret = lcd->pwm_enable(lcd);
	}

	return ret;
}

s32 bsp_disp_lcd_pwm_disable(u32 disp)
{
	s32 ret = -1;
	struct disp_device *lcd;

	lcd = disp_get_lcd(disp);
	if (!lcd)
		return ret;

	if (lcd && lcd->pwm_disable) {
		ret = lcd->pwm_disable(lcd);
	}

	return ret;
}

s32 bsp_disp_lcd_power_enable(u32 disp, u32 power_id)
{
	s32 ret = -1;
	struct disp_device *lcd;

	lcd = disp_get_lcd(disp);
	if (!lcd)
		return ret;

	if (lcd && lcd->power_enable) {
		ret = lcd->power_enable(lcd, power_id);
	}

	return ret;
}

s32 bsp_disp_lcd_power_disable(u32 disp, u32 power_id)
{
	s32 ret = -1;
	struct disp_device *lcd;

	lcd = disp_get_lcd(disp);
	if (!lcd)
		return ret;

	if (lcd && lcd->power_disable) {
		ret = lcd->power_disable(lcd, power_id);
	}

	return ret;
}

s32 bsp_disp_lcd_switch_compat_panel(u32 disp, u32 index)
{
	s32 ret = -1;
	struct disp_device *lcd;

	lcd = disp_get_lcd(disp);
	if (!lcd)
		return ret;

	if (lcd && lcd->switch_compat_panel) {
		ret = lcd->switch_compat_panel(lcd, index);
	}

	return ret;
}

s32 bsp_disp_lcd_set_bright(u32 disp, u32 bright)
{
	s32 ret = -1;
	struct disp_device *lcd;

	lcd = disp_get_lcd(disp);
	if (!lcd)
		return ret;

	if (lcd && lcd->set_bright) {
		ret = lcd->set_bright(lcd, bright);
	}

	return ret;
}

s32 bsp_disp_lcd_get_bright(u32 disp)
{
	u32 bright = 0;
	struct disp_device *lcd;

	lcd = disp_get_lcd(disp);
	if (lcd && lcd->get_bright)
		bright = lcd->get_bright(lcd);

	return bright;
}

s32 bsp_disp_lcd_tcon_enable(u32 disp)
{
	int ret = -1;
	struct disp_device *lcd;
	struct disp_device *lcd_slave;

	disp_panel_para *panel_info =
	    kmalloc(sizeof(disp_panel_para), GFP_KERNEL | __GFP_ZERO);

	if (!panel_info)
		goto OUT;

	lcd = disp_get_lcd(disp);
	if (!lcd)
		goto FREE_INFO;

	if (lcd && lcd->get_panel_info)
		ret = lcd->get_panel_info(lcd, panel_info);

	if (ret != 0)
		goto FREE_INFO;

	if (panel_info->lcd_tcon_mode == DISP_TCON_SLAVE_MODE) {
		ret = 0;
		goto FREE_INFO;
	}

	if (lcd && lcd->tcon_enable)
		ret = lcd->tcon_enable(lcd);

	if (panel_info->lcd_tcon_mode <= DISP_TCON_MASTER_SYNC_EVERY_FRAME &&
	    panel_info->lcd_tcon_mode >= DISP_TCON_MASTER_SYNC_AT_FIRST_TIME) {
		lcd_slave = disp_get_lcd(panel_info->lcd_slave_tcon_num);
		if (!lcd_slave)
			goto FREE_INFO;
		if (lcd_slave && lcd_slave->tcon_enable)
			ret = lcd_slave->tcon_enable(lcd_slave);
	}

FREE_INFO:
	kfree(panel_info);
OUT:
	return ret;
}

s32 bsp_disp_lcd_tcon_disable(u32 disp)
{
	int ret = -1;
	struct disp_device *lcd;
	struct disp_device *lcd_slave;

	disp_panel_para *panel_info =
	    kmalloc(sizeof(disp_panel_para), GFP_KERNEL | __GFP_ZERO);

	if (!panel_info)
		goto OUT;

	lcd = disp_get_lcd(disp);
	if (!lcd)
		goto FREE_INFO;

	if (lcd && lcd->get_panel_info)
		ret = lcd->get_panel_info(lcd, panel_info);

	if (ret != 0)
		goto FREE_INFO;

	if (lcd && lcd->tcon_disable)
		ret = lcd->tcon_disable(lcd);

	if (panel_info->lcd_tcon_mode <= DISP_TCON_MASTER_SYNC_EVERY_FRAME &&
	    panel_info->lcd_tcon_mode >= DISP_TCON_MASTER_SYNC_AT_FIRST_TIME) {
		lcd_slave = disp_get_lcd(panel_info->lcd_slave_tcon_num);
		if (!lcd_slave)
			goto FREE_INFO;
		if (lcd_slave && lcd_slave->tcon_disable)
			ret = lcd_slave->tcon_disable(lcd_slave);
	}

FREE_INFO:
	kfree(panel_info);
OUT:
	return ret;
}

s32 bsp_disp_lcd_pin_cfg(u32 disp, u32 en)
{
	int ret = -1;
	struct disp_device *lcd;

	lcd = disp_get_lcd(disp);
	if (lcd && lcd->pin_cfg)
		ret = lcd->pin_cfg(lcd, en);

	return ret;
}

s32 bsp_disp_lcd_gpio_set_value(u32 disp, u32 io_index, u32 value)
{
	int ret = -1;
	struct disp_device *lcd;

	lcd = disp_get_lcd(disp);
	if (lcd && lcd->gpio_set_value)
		ret = lcd->gpio_set_value(lcd, io_index, value);

	return ret;
}

s32 bsp_disp_lcd_gpio_set_direction(u32 disp, unsigned int io_index, u32 direction)
{
	int ret = -1;
	struct disp_device *lcd;

	lcd = disp_get_lcd(disp);
	if (lcd && lcd->gpio_set_direction)
		ret = lcd->gpio_set_direction(lcd, io_index, direction);

	return ret;
}

s32 bsp_disp_get_panel_info(u32 disp, disp_panel_para *info)
{
	struct disp_device *lcd;

	lcd = disp_get_lcd(disp);
	if (!lcd)
		DE_WRN("get lcd%d fail\n", disp);

	if (lcd && lcd->get_panel_info)
		return lcd->get_panel_info(lcd, info);

	return DIS_FAIL;
}

int bsp_disp_get_fb_info(unsigned int disp, struct disp_layer_info *info)
{
	return disp_al_get_fb_info(disp, info);
}

int bsp_disp_get_display_size(u32 disp, unsigned int *width, unsigned int *height)
{
	return disp_al_get_display_size(disp, width, height);
}

#if defined(SUPPORT_DSI)
/**
 * @name       :bsp_disp_lcd_dsi_mode_switch
 * @brief      :dsi module mode switch
 * @param[IN]  :cmd_en: command mode enable
 * @param[IN]  :lp_en: lower power mode enable
 * @return     :0 if success
 */
s32 bsp_disp_lcd_dsi_mode_switch(u32 disp, u32 cmd_en, u32 lp_en)
{
	s32 ret = -1;
	disp_panel_para *panel_info =
	    kmalloc(sizeof(disp_panel_para), GFP_KERNEL | __GFP_ZERO);

	ret = bsp_disp_get_panel_info(disp, panel_info);
	if (ret == DIS_FAIL) {
		DE_WRN("%s:Get panel info failed\n", __func__);
		goto OUT;
	}

	if (panel_info->lcd_tcon_mode == DISP_TCON_SLAVE_MODE)
		goto OUT;

	ret = dsi_mode_switch(disp, cmd_en, lp_en);
	if (panel_info->lcd_tcon_mode == DISP_TCON_DUAL_DSI &&
	    disp + 1 < DEVICE_DSI_NUM)
		ret = dsi_mode_switch(disp + 1, cmd_en, lp_en);
	else if (panel_info->lcd_tcon_mode != DISP_TCON_NORMAL_MODE &&
		 panel_info->lcd_tcon_mode != DISP_TCON_DUAL_DSI)
		ret = dsi_mode_switch(panel_info->lcd_slave_tcon_num, cmd_en,
				      lp_en);
OUT:
	kfree(panel_info);
	return ret;
}

s32 bsp_disp_lcd_dsi_clk_enable(u32 disp, u32 en)
{
	s32 ret = -1;
	disp_panel_para *panel_info =
	    kmalloc(sizeof(disp_panel_para), GFP_KERNEL | __GFP_ZERO);

	ret = bsp_disp_get_panel_info(disp, panel_info);
	if (ret == DIS_FAIL) {
		DE_WRN("%s:Get panel info failed\n", __func__);
		goto OUT;
	}

	if (panel_info->lcd_tcon_mode == DISP_TCON_SLAVE_MODE)
		goto OUT;

	ret = dsi_clk_enable(disp, en);
	if (panel_info->lcd_tcon_mode == DISP_TCON_DUAL_DSI &&
	    disp + 1 < DEVICE_DSI_NUM)
		ret = dsi_clk_enable(disp + 1, en);
	else if (panel_info->lcd_tcon_mode != DISP_TCON_NORMAL_MODE &&
		 panel_info->lcd_tcon_mode != DISP_TCON_DUAL_DSI)
		ret = dsi_clk_enable(panel_info->lcd_slave_tcon_num, en);
OUT:
	kfree(panel_info);
	return ret;
}

s32 bsp_disp_lcd_dsi_dcs_wr(u32 disp, u8 command, u8 *para, u32 para_num)
{
	s32 ret = -1;
	disp_panel_para *panel_info =
	    kmalloc(sizeof(disp_panel_para), GFP_KERNEL | __GFP_ZERO);

	ret = bsp_disp_get_panel_info(disp, panel_info);
	if (ret == DIS_FAIL) {
		DE_WRN("%s:Get panel info failed\n", __func__);
		goto OUT;
	}

	if (panel_info->lcd_tcon_mode == DISP_TCON_SLAVE_MODE)
		goto OUT;

	ret = dsi_dcs_wr(disp, command, para, para_num);
	if (panel_info->lcd_tcon_mode == DISP_TCON_DUAL_DSI &&
	    disp + 1 < DEVICE_DSI_NUM &&
	    panel_info->lcd_dsi_port_num == DISP_LCD_DSI_SINGLE_PORT)
		ret = dsi_dcs_wr(disp + 1, command, para, para_num);
	else if (panel_info->lcd_tcon_mode != DISP_TCON_NORMAL_MODE &&
		 panel_info->lcd_tcon_mode != DISP_TCON_DUAL_DSI)
		ret = dsi_dcs_wr(panel_info->lcd_slave_tcon_num, command, para,
				 para_num);
OUT:
	kfree(panel_info);
	return ret;
}

s32 bsp_disp_lcd_dsi_gen_wr(u32 disp, u8 command, u8 *para, u32 para_num)
{
	s32 ret = -1;
	disp_panel_para *panel_info =
	    kmalloc(sizeof(disp_panel_para), GFP_KERNEL | __GFP_ZERO);

	ret = bsp_disp_get_panel_info(disp, panel_info);
	if (ret == DIS_FAIL) {
		DE_WRN("%s:Get panel info failed\n", __func__);
		goto OUT;
	}

	if (panel_info->lcd_tcon_mode == DISP_TCON_SLAVE_MODE)
		goto OUT;

	ret = dsi_gen_wr(disp, command, para, para_num);
	if (panel_info->lcd_tcon_mode == DISP_TCON_DUAL_DSI &&
	    disp + 1 < DEVICE_DSI_NUM &&
	    panel_info->lcd_dsi_port_num == DISP_LCD_DSI_SINGLE_PORT)
		ret = dsi_gen_wr(disp + 1, command, para, para_num);
	else if (panel_info->lcd_tcon_mode != DISP_TCON_NORMAL_MODE &&
		 panel_info->lcd_tcon_mode != DISP_TCON_DUAL_DSI)
		ret = dsi_gen_wr(panel_info->lcd_slave_tcon_num, command, para,
				 para_num);
OUT:
	kfree(panel_info);
	return ret;
}

s32 bsp_disp_lcd_dsi_gen_short_read(u32 sel, u8 *para_p, u8 para_num,
				    u8 *result)
{
	s32 ret = -1;

	if (!result || !para_p || para_num > 2) {
		DE_WRN("Wrong para!\n");
		goto OUT;
	}
	ret = dsi_gen_short_rd(sel, para_p, para_num, result);
OUT:
	return ret;
}

s32 bsp_disp_lcd_dsi_dcs_read(u32 sel, u8 cmd, u8 *result, u32 *num_p)
{
	s32 ret = -1;

	if (!result || !num_p) {
		DE_WRN("Wrong para!\n");
		goto OUT;
	}
	ret = dsi_dcs_rd(sel, cmd, result, num_p);
OUT:
	return ret;
}

s32 bsp_disp_lcd_set_max_ret_size(u32 sel, u32 size)
{
	return dsi_set_max_ret_size(sel, size);
}
#endif /*endif SUPPORT_DSI */
