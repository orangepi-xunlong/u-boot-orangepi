/*
 * drivers/video/sunxi/disp2/hdmi/drv_hdmi.c
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
#include "drv_hdmi_i.h"
#include "hdmi_core.h"
#include "../disp/disp_sys_intf.h"

#define HDMI_IO_NUM 5
#define __CLK_SUPPORT__

static bool hdmi_io_used[HDMI_IO_NUM]={0};
static disp_gpio_set_t hdmi_io[HDMI_IO_NUM];
static u32 io_enable_count = 0;
static u32 io_regulator_enable_count;
#if defined(__LINUX_PLAT__)
static struct semaphore *run_sem = NULL;
static struct task_struct * HDMI_task;
#endif
static bool hdmi_cec_support;
static char hdmi_power[25];
static char hdmi_io_regulator[25];
static bool hdmi_power_used;
static bool hdmi_io_regulator_used;
static bool hdmi_used;
static bool boot_hdmi = false;
static int hdmi_work_mode;
#if defined(__CLK_SUPPORT__)
static struct clk *hdmi_clk = NULL;
static struct clk *hdmi_ddc_clk = NULL;
static struct clk *hdmi_clk_parent;
#if defined(CONFIG_MACH_SUN8IW12)
static struct clk *hdmi_cec_clk;
#endif /*endif CONFIG_ARCH */
#endif
static u32 power_enable_count = 0;
static u32 clk_enable_count = 0;
__attribute__((unused)) static struct mutex mlock;
#if defined(CONFIG_SND_SUNXI_SOC_HDMIAUDIO)
static bool audio_enable = false;
#endif
static bool b_hdmi_suspend;
static bool b_hdmi_suspend_pre;

hdmi_info_t ghdmi;

static int hdmi_update_para_for_kernel(struct disp_video_timings *timing);
static s32 hdmi_get_video_timing_from_tv_mode(u32 tv_mode,
				struct disp_video_timings **video_info);

void hdmi_delay_ms(unsigned long ms)
{
#if defined(__LINUX_PLAT__)
	u32 timeout = ms*HZ/1000;
	set_current_state(TASK_INTERRUPTIBLE);
	schedule_timeout(timeout);
#elif defined(__UBOOT_PLAT__)
	__msdelay(ms);
#endif
}

void hdmi_delay_us(unsigned long us)
{
#if defined(__LINUX_PLAT__)
	udelay(us);
#elif defined(__UBOOT_PLAT__)
	__usdelay(us);
#endif
}

unsigned int hdmi_get_soc_version(void)
{
    unsigned int version = 0;
#if defined(CONFIG_MACH_SUN8IW7)
#if defined(SUN8IW7P1_REV_A) || defined(SUN8IW7P2_REV_B)
	unsigned int chip_ver = sunxi_get_soc_ver();

	switch (chip_ver) {
		case SUN8IW7P1_REV_A:
		case SUN8IW7P2_REV_A:
			version = 0;
			break;
		case SUN8IW7P1_REV_B:
		case SUN8IW7P2_REV_B:
			version = 1;
	}
#else
	version = 1;
#endif
#endif
	return version;
}

s32 hdmi_get_work_mode(void)
{
	return hdmi_work_mode;
}

static int hdmi_parse_io_config(void)
{
	disp_gpio_set_t  *gpio_info;
	int i, ret;
	char io_name[32];

	for (i=0; i<HDMI_IO_NUM; i++) {
		gpio_info = &(hdmi_io[i]);
		sprintf(io_name, "hdmi_io_%d", i);
		ret = disp_sys_script_get_item("hdmi", io_name, (int *)gpio_info, sizeof(disp_gpio_set_t)/sizeof(int));
		if (ret == 3)
		  hdmi_io_used[i]= 1;
		else
			hdmi_io_used[i] = 0;
	}

  return 0;
}

static int hdmi_io_config(u32 bon)
{
	int i;

	for (i=0; i<HDMI_IO_NUM; i++)	{
		if (hdmi_io_used[i]) {
			disp_gpio_set_t  gpio_info[1];

			memcpy(gpio_info, &(hdmi_io[i]), sizeof(disp_gpio_set_t));
			if (!bon) {
				gpio_info->mul_sel = 7;
			}
			disp_sys_gpio_request_simple(gpio_info, 1);
		}
	}
	return 0;
}
#if defined(__CLK_SUPPORT__)
static int hdmi_clk_enable(void)
{
	int ret = 0;

	if (hdmi_clk)
		ret = clk_prepare_enable(hdmi_clk);
	if (0 != ret) {
		__wrn("enable hdmi clk fail\n");
		return ret;
	}

	if (hdmi_ddc_clk)
		ret = clk_prepare_enable(hdmi_ddc_clk);

	if (0 != ret) {
		__wrn("enable hdmi ddc clk fail\n");
		clk_disable(hdmi_clk);
	}

#if defined(CONFIG_MACH_SUN8IW12)
	if (hdmi_cec_clk)
		ret = clk_prepare_enable(hdmi_cec_clk);
	if (0 != ret)
		__wrn("enable hdmi cec clk fail\n");
#endif

	return ret;
}

static int hdmi_clk_disable(void)
{
	if (hdmi_clk)
		clk_disable(hdmi_clk);
	if (hdmi_ddc_clk)
		clk_disable(hdmi_ddc_clk);
#if defined(CONFIG_MACH_SUN8IW12)
	if (hdmi_cec_clk)
		clk_disable(hdmi_cec_clk);
#endif

	return 0;
}

static bool hdmi_is_divide_by(unsigned long dividend,
			       unsigned long divisor)
{
	bool divide = false;
	unsigned long temp;

	if (divisor == 0)
		goto exit;

	temp = dividend / divisor;
	if (dividend == (temp * divisor))
		divide = true;

exit:
	return divide;
}

static int hdmi_clk_config(u32 vic)
{
	int index = 0;
	unsigned long rate = 0, rate_set = 0;
	unsigned long rate_round, rate_parent;
	int i;

	index = hdmi_core_get_video_info(vic);
	rate = video_timing[index].pixel_clk *
		(video_timing[index].pixel_repeat + 1);
	rate_parent = clk_get_rate(hdmi_clk_parent);
	if (!hdmi_is_divide_by(rate_parent, rate)) {
		if (hdmi_is_divide_by(297000000, rate))
				clk_set_rate(hdmi_clk_parent, 297000000);
		else if (hdmi_is_divide_by(594000000, rate))
				clk_set_rate(hdmi_clk_parent, 594000000);
	}

	if (hdmi_clk) {
		clk_set_rate(hdmi_clk, rate);
		rate_set = clk_get_rate(hdmi_clk);
	}

	if (hdmi_clk && (rate_set != rate)) {
		for (i = 1; i < 10; i++) {
			rate_parent = rate * i;
			rate_round = clk_round_rate(hdmi_clk_parent,
								rate_parent);
			if (rate_round == rate_parent) {
				clk_set_rate(hdmi_clk_parent, rate_parent);
				clk_set_rate(hdmi_clk, rate);
				break;
			}
		}
		if (i == 10)
			printf("clk_set_rate fail.need %ldhz,but get %ldhz\n",
				rate, rate_set);
	}

	return 0;
}

unsigned int hdmi_clk_get_div(void)
{
	unsigned long rate = 1, rate_parent = 1;
	unsigned int div = 4;

	if (!hdmi_clk || !hdmi_clk_parent) {
		__wrn("%s, get clk div fail\n", __func__);
		goto exit;
	}

	if (hdmi_clk)
		rate = clk_get_rate(hdmi_clk);
	if (hdmi_clk_parent)
		rate_parent = clk_get_rate(hdmi_clk_parent);

	if (rate != 0)
		div = rate_parent / rate;
	else
		__wrn("%s, hdmi clk rate is ZERO!\n", __func__);

exit:
	return div;
}

#else
static int hdmi_clk_enable(void){return 0;}
static int hdmi_clk_disable(void){return 0;}
static int hdmi_clk_config(u32 vic){return 0;}
#endif

static int hdmi_power_enable(char *name)
{
	return disp_sys_power_enable(name);
}

static int hdmi_power_disable(char *name)
{
	return disp_sys_power_disable(name);
}

static s32 hdmi_enable(void)
{
	__inf("[hdmi_enable]\n");

	mutex_lock(&mlock);
	if (1 != ghdmi.bopen) {
#if defined(__UBOOT_PLAT__)
		/* if force output hdmi when hdmi not plug in
			then detect one more time
		*/
		if (!hdmi_core_hpd_check())
			hdmi_core_loop();
#endif
		hdmi_clk_config(ghdmi.mode);
		hdmi_core_set_video_enable(1);
		ghdmi.bopen = 1;
	}
	mutex_unlock(&mlock);
	return 0;
}

static s32 hdmi_disable(void)
{
	__inf("[hdmi_disable]\n");

	mutex_lock(&mlock);
	if (0 != ghdmi.bopen) {
		hdmi_core_set_video_enable(0);
		ghdmi.bopen = 0;
	}
	mutex_unlock(&mlock);
	return 0;
}

static struct disp_hdmi_mode hdmi_mode_tbl[] = {
	{DISP_TV_MOD_480I,                HDMI1440_480I,     },
	{DISP_TV_MOD_576I,                HDMI1440_576I,     },
	{DISP_TV_MOD_480P,                HDMI480P,          },
	{DISP_TV_MOD_576P,                HDMI576P,          },
	{DISP_TV_MOD_720P_50HZ,           HDMI720P_50,       },
	{DISP_TV_MOD_720P_60HZ,           HDMI720P_60,       },
	{DISP_TV_MOD_1080I_50HZ,          HDMI1080I_50,      },
	{DISP_TV_MOD_1080I_60HZ,          HDMI1080I_60,      },
	{DISP_TV_MOD_1080P_24HZ,          HDMI1080P_24,      },
	{DISP_TV_MOD_1080P_50HZ,          HDMI1080P_50,      },
	{DISP_TV_MOD_1080P_60HZ,          HDMI1080P_60,      },
	{DISP_TV_MOD_1080P_25HZ,          HDMI1080P_25,      },
	{DISP_TV_MOD_1080P_30HZ,          HDMI1080P_30,      },
	{DISP_TV_MOD_1080P_24HZ_3D_FP,    HDMI1080P_24_3D_FP,},
	{DISP_TV_MOD_720P_50HZ_3D_FP,     HDMI720P_50_3D_FP, },
	{DISP_TV_MOD_720P_60HZ_3D_FP,     HDMI720P_60_3D_FP, },
	{DISP_TV_MOD_3840_2160P_30HZ,     HDMI3840_2160P_30, },
	{DISP_TV_MOD_3840_2160P_25HZ,     HDMI3840_2160P_25, },
	{DISP_TV_MOD_3840_2160P_24HZ,     HDMI3840_2160P_24, },
	{DISP_TV_MOD_4096_2160P_24HZ,     HDMI4096_2160P_24, },
	{DISP_TV_MOD_1280_1024P_60HZ,     HDMI1280_1024,     },
	{DISP_TV_MOD_1024_768P_60HZ,      HDMI1024_768,      },
	{DISP_TV_MOD_900_540P_60HZ,       HDMI900_540,       },
	{DISP_TV_MOD_1920_720P_60HZ,      HDMI1920_720,      },
	{DISP_HDMI_MOD_DT0,               HDMI_DT0,          },
	{DISP_HDMI_MOD_DT1,               HDMI_DT1,          },
	{DISP_HDMI_MOD_DT2,               HDMI_DT2,          },
	{DISP_HDMI_MOD_DT3,               HDMI_DT3,          },
};

u32 hdmi_get_vic(u32 mode)
{
	u32 hdmi_mode = DISP_TV_MOD_720P_50HZ;
	u32 i;
	bool find = false;

	for (i=0; i<sizeof(hdmi_mode_tbl)/sizeof(struct disp_hdmi_mode); i++)
	{
		if (hdmi_mode_tbl[i].mode == mode) {
			hdmi_mode = hdmi_mode_tbl[i].hdmi_mode;
			find = true;
			break;
		}
	}

	if (false == find)
		__wrn("[HDMI]can't find vic for mode(%d)\n", mode);

	return hdmi_mode;
}

static s32 hdmi_set_display_mode(u32 mode)
{
	u32 hdmi_mode;
	bool find = false;
	u32 i = 0;

	__inf("[hdmi_set_display_mode],mode:%d\n",mode);

	for (i=0; i<sizeof(hdmi_mode_tbl)/sizeof(struct disp_hdmi_mode); i++)
	{
		if (hdmi_mode_tbl[i].mode == (enum disp_tv_mode)mode) {
			hdmi_mode = hdmi_mode_tbl[i].hdmi_mode;
			find = true;
			break;
		}
	}

	if (find) {
		ghdmi.mode = hdmi_mode;
		return hdmi_core_set_video_mode(hdmi_mode);
	} else {
		__wrn("unsupported video mode %d when set display mode\n", mode);
		return -1;
	}
}

#if defined(CONFIG_SND_SUNXI_SOC_HDMIAUDIO)
static s32 hdmi_audio_enable(u8 mode, u8 channel)
{
	__inf("[hdmi_audio_enable],mode:%d,ch:%d\n",mode, channel);
	mutex_lock(&mlock);
	audio_enable = mode;
	mutex_unlock(&mlock);
	return hdmi_core_set_audio_enable(audio_enable);
}

static s32 hdmi_set_audio_para(hdmi_audio_t * audio_para)
{
	__inf("[hdmi_set_audio_para]\n");
	return hdmi_core_audio_config(audio_para);
}
#endif

static s32 get_tv_mode_from_vic(u32 vic)
{
	int i = 0;
	bool find = false;
	enum disp_tv_mode tv_mode;

	for (i = 0; i < sizeof(hdmi_mode_tbl) / sizeof(struct disp_hdmi_mode); i++) {
		if (hdmi_mode_tbl[i].hdmi_mode == vic) {
			tv_mode = hdmi_mode_tbl[i].mode;
			find = true;
			break;
		}
	}

	if (find)
		return (s32)tv_mode;
	else
		return (s32)DISP_HDMI_MOD_DT0;
}

static s32 get_vic_from_tv_mode(s32 tv_mode)
{
	int i = 0;
	bool find = false;
	s32 vic = 0;

	for (i = 0; i < sizeof(hdmi_mode_tbl) / sizeof(struct disp_hdmi_mode); i++) {
		if (hdmi_mode_tbl[i].mode == tv_mode) {
			vic = hdmi_mode_tbl[i].hdmi_mode;
			find = true;
			break;
		}
	}

	if (find)
		return (s32)vic;
	else
		return (s32)HDMI_DT0;
}

static s32 hdmi_get_support_mode(u32 init_mode)
{
	int vic_mode = 0, tv_mode = 0, i;
	struct disp_video_timings *timing = NULL;
	bool find = false;

	for (i = 0; i < sizeof(hdmi_mode_tbl) / sizeof(struct disp_hdmi_mode); i++) {
		if (hdmi_mode_tbl[i].mode == init_mode) {
			vic_mode = hdmi_mode_tbl[i].hdmi_mode;
			find = true;
			break;
		}
	}

	if (find)
		tv_mode = get_tv_mode_from_vic(hdmi_core_get_supported_vic(vic_mode));
	else
		tv_mode = get_tv_mode_from_vic(hdmi_core_get_supported_vic(HDMI_DT0));

	hdmi_get_video_timing_from_tv_mode(tv_mode, &timing);
	hdmi_update_para_for_kernel(timing);

	return tv_mode;
}

static s32 hdmi_mode_support(u32 mode)
{
	u32 hdmi_mode;
	u32 i;
	bool find = false;

	__inf("[%s] mode:%d\n", __func__, mode);
	for (i=0; i<sizeof(hdmi_mode_tbl)/sizeof(struct disp_hdmi_mode); i++)
	{
		if (hdmi_mode_tbl[i].mode == (enum disp_tv_mode)mode) {
			hdmi_mode = hdmi_mode_tbl[i].hdmi_mode;
			find = true;
			break;
		}
	}

	if (find && (mode == DISP_TV_MOD_1280_1024P_60HZ
		|| mode == DISP_TV_MOD_1024_768P_60HZ
		|| mode == DISP_TV_MOD_900_540P_60HZ
		|| mode == DISP_TV_MOD_1920_720P_60HZ)) {
		return 1;
	}

	if (find) {
		return hdmi_core_mode_support(hdmi_mode);
	} else {
		return 0;
	}
}

static s32 hdmi_get_HPD_status(void)
{
#if defined(__UBOOT_PLAT__)
	/* check hw status once when get hpd status
		no need on linux plat while there's detect thread
	*/
	hdmi_core_loop();
#endif
	return hdmi_core_hpd_check();
}

#if defined(__LINUX_PLAT__)
static s32 hdmi_get_hdcp_enable(void)
{
	return hdmi_core_get_hdcp_enable();
}
#endif

static s32 hdmi_get_video_timing_from_tv_mode(u32 tv_mode,
				struct disp_video_timings **video_info)
{
	struct disp_video_timings *info;
	u32 vic = 0;
	int ret = -1;
	int i, list_num;

	vic = get_vic_from_tv_mode(tv_mode);
	info = video_timing;
	list_num = hdmi_core_get_list_num();
	for (i = 0; i < list_num; i++) {
		if (info->vic == vic) {
			*video_info = info;
			ret = 0;
			break;
		}
		info++;
	}

	return ret;
}

static s32 hdmi_get_video_timming_info(struct disp_video_timings **video_info)
{
	struct disp_video_timings *info;
	int ret = -1;
	int i, list_num;

	__inf("hdmi_get_video_timming_info\n");
	info = video_timing;
	list_num = hdmi_core_get_list_num();
	for (i = 0; i < list_num; i++) {
		if (info->vic == ghdmi.mode) {
			*video_info = info;
			ret = 0;
			break;
		}
		info++;
	}

	return ret;
}

static s32 hdmi_get_input_csc(void)
{
	return hdmi_core_get_csc_type();
}

#if defined(__LINUX_PLAT__)
static int hdmi_run_thread(void *parg)
{
	while (1) {
		if (kthread_should_stop()) {
			break;
		}

		mutex_lock(&mlock);
		if (false == b_hdmi_suspend) {
			/* normal state */
			b_hdmi_suspend_pre = b_hdmi_suspend;
			mutex_unlock(&mlock);
			hdmi_core_loop();

			if (false == b_hdmi_suspend) {
				if (hdmi_get_hdcp_enable()==1)
					hdmi_delay_ms(100);
				else
					hdmi_delay_ms(200);
			}
		} else {
			/* suspend state */
			if (false == b_hdmi_suspend_pre) {
				/* first time after enter suspend state */
				//hdmi_core_enter_lp();
			}
			b_hdmi_suspend_pre = b_hdmi_suspend;
			mutex_unlock(&mlock);
		}
	}

	return 0;
}

#if defined(CONFIG_SWITCH) || defined(CONFIG_ANDROID_SWITCH)
static struct switch_dev hdmi_switch_dev = {
	.name = "hdmi",
};

s32 disp_set_hdmi_detect(bool hpd);
static void hdmi_report_hpd_work(struct work_struct *work)
{
	if (hdmi_get_HPD_status())	{
		if (hdmi_switch_dev.dev)
			switch_set_state(&hdmi_switch_dev, 1);
		disp_set_hdmi_detect(1);
		__inf("switch_set_state 1\n");
	}	else {
		if (hdmi_switch_dev.dev)
			switch_set_state(&hdmi_switch_dev, 0);
		disp_set_hdmi_detect(0);
		__inf("switch_set_state 0\n");
	}
}

s32 hdmi_hpd_state(u32 state)
{
	if (state == 0) {
		if (hdmi_switch_dev.dev)
			switch_set_state(&hdmi_switch_dev, 0);
	} else {
		if (hdmi_switch_dev.dev)
			switch_set_state(&hdmi_switch_dev, 1);
	}

	return 0;
}
#else
static void hdmi_report_hpd_work(struct work_struct *work)
{
}

s32 hdmi_hpd_state(u32 state)
{
	return 0;
}
#endif
#endif
/**
 * hdmi_hpd_report - report hdmi hot plug state to user space
 * @hotplug:	0: hdmi plug out;   1:hdmi plug in
 *
 * always return success.
 */
s32 hdmi_hpd_event(void)
{
#if defined(__LINUX_PLAT__)
	schedule_work(&ghdmi.hpd_work);
#endif
	return 0;
}

static s32 hdmi_suspend(void)
{
#if defined(__LINUX_PLAT__)
	hdmi_core_update_detect_time(0);
	mutex_lock(&mlock);
	if (hdmi_used && (false == b_hdmi_suspend)) {
		b_hdmi_suspend = true;
		if (HDMI_task) {
			kthread_stop(HDMI_task);
			HDMI_task = NULL;
		}
		hdmi_core_enter_lp();
		if (0 != clk_enable_count) {
			hdmi_clk_disable();
			clk_enable_count --;
		}
		if (0 != io_enable_count) {
			hdmi_io_config(0);
			io_enable_count --;
		}
		if ((hdmi_power_used) && (0 != power_enable_count)) {
			hdmi_power_disable(hdmi_power);
			power_enable_count --;
		}
		__wrn("[HDMI]hdmi suspend\n");
	}
	mutex_unlock(&mlock);
#endif
	return 0;
}

static s32 hdmi_resume(void)
{
	int ret = 0;
#if defined(__LINUX_PLAT__)
	mutex_lock(&mlock);
	if (hdmi_used && (true == b_hdmi_suspend)) {
		/* normal state */
		if (clk_enable_count == 0) {
			ret = hdmi_clk_enable();
			if (0 == ret)
				clk_enable_count ++;
			else {
				pr_warn("fail to enable hdmi's clock\n");
				goto exit;
			}
		}
		if ((hdmi_power_used) && (power_enable_count == 0)) {
			hdmi_power_enable(hdmi_power);
			power_enable_count ++;
		}
		if (io_enable_count == 0) {
			hdmi_io_config(1);
			io_enable_count ++;
		}
		/* first time after exit suspend state */
		hdmi_core_exit_lp();

		HDMI_task = kthread_create(hdmi_run_thread, (void*)0, "hdmi proc");
		if (IS_ERR(HDMI_task)) {
			s32 err = 0;
			pr_warn("Unable to start kernel thread %s.\n\n", "hdmi proc");
			err = PTR_ERR(HDMI_task);
			HDMI_task = NULL;
		} else
			wake_up_process(HDMI_task);

		__wrn("[HDMI]hdmi resume\n");
	}

exit:
	mutex_unlock(&mlock);

	hdmi_core_update_detect_time(200);//200ms
	b_hdmi_suspend = false;
#endif
	return  ret;
}

#if defined(CONFIG_SND_SUNXI_SOC_HDMIAUDIO)
extern void audio_set_hdmi_func(__audio_hdmi_func * hdmi_func);
#endif
extern s32 disp_set_hdmi_func(struct disp_device_func * func);
extern unsigned int disp_boot_para_parse(void);

static int hdmi_update_para_for_kernel(struct disp_video_timings *timing)
{
#ifndef CONFIG_SUNXI_MULITCORE_BOOT
	int node;
	int ret = -1;

	if (hdmi_get_work_mode() != DISP_HDMI_FULL_AUTO)
		return -1;

	node = fdt_path_offset(working_fdt, "hdmi");
	if (node < 0) {
		printf("%s:hdmi_fdt_nodeoffset %s fail\n", __func__, "hdmi");
		goto exit;
	}

	ret = fdt_setprop_u32(working_fdt, node, "pixel_clk",
					timing->pixel_clk);
	if (ret < 0) {
		printf("set pixel_clk failed\n");
		goto exit;
	}
	ret = fdt_setprop_u32(working_fdt, node, "interlace",
					timing->b_interlace);
	if (ret < 0) {
		printf("set interlace failed\n");
		goto exit;
	}
	ret = fdt_setprop_u32(working_fdt, node, "hactive",
					timing->x_res);
	if (ret < 0) {
		printf("set hactive failed\n");
		goto exit;
	}
	ret = fdt_setprop_u32(working_fdt, node, "hblank",
			timing->hor_total_time - timing->x_res);
	if (ret < 0) {
		printf("set hblank failed\n");
		goto exit;
	}
	ret = fdt_setprop_u32(working_fdt, node, "hsync_offset",
					timing->hor_front_porch);
	if (ret < 0) {
		printf("set hsync_offset failed\n");
		goto exit;
	}
	ret = fdt_setprop_u32(working_fdt, node, "hsync",
					timing->hor_sync_time);
	if (ret < 0) {
		printf("set hsync failed\n");
		goto exit;
	}
	ret = fdt_setprop_u32(working_fdt, node, "hpol",
				timing->hor_sync_polarity);
	if (ret < 0) {
		printf("set hpol failed\n");
		goto exit;
	}
	ret = fdt_setprop_u32(working_fdt, node, "vactive",
		timing->b_interlace ? (timing->y_res / 2) : timing->y_res);
	if (ret < 0) {
		printf("set vactive failed\n");
		goto exit;
	}
	ret = fdt_setprop_u32(working_fdt, node, "vblank",
		timing->b_interlace ?
		(timing->ver_total_time - timing->y_res) / 2 : timing->y_res);
	if (ret < 0) {
		printf("set vblank failed\n");
		goto exit;
	}
	ret = fdt_setprop_u32(working_fdt, node, "vsync_offset",
					timing->ver_front_porch);
	if (ret < 0) {
		printf("set vblank failed\n");
		goto exit;
	}
	ret = fdt_setprop_u32(working_fdt, node, "vsync",
					timing->ver_sync_time);
	if (ret < 0) {
		printf("set vblank failed\n");
		goto exit;
	}
	ret = fdt_setprop_u32(working_fdt, node, "vpol",
					timing->ver_sync_polarity);
	if (ret < 0) {
		printf("set vblank failed\n");
		goto exit;
	}
exit:
	return ret;
#else
	int ret = -1;

	if (hdmi_get_work_mode() != DISP_HDMI_FULL_AUTO)
		return -1;

	ret = sunxi_fdt_getprop_store(working_fdt, "hdmi", "pixel_clk",
					timing->pixel_clk);
	if (ret < 0) {
		printf("set pixel_clk failed\n");
		goto exit;
	}
	ret = sunxi_fdt_getprop_store(working_fdt, "hdmi", "interlace",
					timing->b_interlace);
	if (ret < 0) {
		printf("set interlace failed\n");
		goto exit;
	}
	ret = sunxi_fdt_getprop_store(working_fdt, "hdmi", "hactive",
					timing->x_res);
	if (ret < 0) {
		printf("set hactive failed\n");
		goto exit;
	}
	ret = sunxi_fdt_getprop_store(working_fdt, "hdmi", "hblank",
			timing->hor_total_time - timing->x_res);
	if (ret < 0) {
		printf("set hblank failed\n");
		goto exit;
	}
	ret = sunxi_fdt_getprop_store(working_fdt, "hdmi", "hsync_offset",
					timing->hor_front_porch);
	if (ret < 0) {
		printf("set hsync_offset failed\n");
		goto exit;
	}
	ret = sunxi_fdt_getprop_store(working_fdt, "hdmi", "hsync",
					timing->hor_sync_time);
	if (ret < 0) {
		printf("set hsync failed\n");
		goto exit;
	}
	ret = sunxi_fdt_getprop_store(working_fdt, "hdmi", "hpol",
				timing->hor_sync_polarity);
	if (ret < 0) {
		printf("set hpol failed\n");
		goto exit;
	}
	ret = sunxi_fdt_getprop_store(working_fdt, "hdmi", "vactive",
				timing->y_res / (1 + timing->b_interlace));
	if (ret < 0) {
		printf("set vactive failed\n");
		goto exit;
	}
	ret = sunxi_fdt_getprop_store(working_fdt, "hdmi", "vblank",
		(timing->ver_total_time - timing->y_res) / (1 + timing->b_interlace));
	if (ret < 0) {
		printf("set vblank failed\n");
		goto exit;
	}
	ret = sunxi_fdt_getprop_store(working_fdt, "hdmi", "vsync_offset",
					timing->ver_front_porch);
	if (ret < 0) {
		printf("set vblank failed\n");
		goto exit;
	}
	ret = sunxi_fdt_getprop_store(working_fdt, "hdmi", "vsync",
					timing->ver_sync_time);
	if (ret < 0) {
		printf("set vblank failed\n");
		goto exit;
	}
	ret = sunxi_fdt_getprop_store(working_fdt, "hdmi", "vpol",
					timing->ver_sync_polarity);
	if (ret < 0) {
		printf("set vblank failed\n");
		goto exit;
	}
exit:
	return ret;

#endif
}

int hdmi_parse_dts_timing(struct disp_video_timings *timing)
{
	int ret = 0, value = 0;
	int blanking = 0;

	ret = disp_sys_script_get_item(FDT_HDMI_PATH, "pixel_clk", &value, 1);
	if (ret == 1) {
		timing->pixel_clk = value;
	} else {
		__inf("fetch hdmi pixel clk err.\n");
		return -1;
	}

	ret = disp_sys_script_get_item(FDT_HDMI_PATH, "interlace", &value, 1);
	if (ret == 1) {
		timing->b_interlace = value ? true : false;
	} else {
		printf("fetch hdmi interlace err.\n");
		timing->b_interlace = 0;
	}

	ret = disp_sys_script_get_item(FDT_HDMI_PATH, "hactive", &value, 1);
	if (ret == 1) {
		timing->x_res = value;
	} else {
		printf("fetch hdmi hacive err.\n");
		return -1;
	}

	ret = disp_sys_script_get_item(FDT_HDMI_PATH, "hblank", &value, 1);
	if (ret == 1) {
		blanking = value;
		timing->hor_total_time = timing->x_res + value;
	} else {
		printf("fetch hdmi hblanking err.\n");
		return -1;
	}

	ret = disp_sys_script_get_item(FDT_HDMI_PATH, "hsync", &value, 1);
	if (ret == 1) {
		timing->hor_sync_time = value;
	} else {
		printf("fetch hdmi hsync err.\n");
		return -1;
	}

	ret = disp_sys_script_get_item(FDT_HDMI_PATH, "hsync_offset", &value, 1);
	if (ret == 1) {
		timing->hor_front_porch = value;
		timing->hor_back_porch = blanking - timing->hor_sync_time
						  - value;
	} else {
		printf("fetch hdmi hsync_offset err.\n");
		return -1;
	}

	ret = disp_sys_script_get_item(FDT_HDMI_PATH, "hpol", &value, 1);
	if (ret == 1) {
		timing->hor_sync_polarity = value;
	} else {
		printf("fetch hdmi hpol err.\n");
		return -1;
	}

	ret = disp_sys_script_get_item(FDT_HDMI_PATH, "vactive", &value, 1);
	if (ret == 1) {
		timing->y_res = timing->b_interlace ? (2 * value) : value;
	} else {
		printf("fetch hdmi vacive err.\n");
		return -1;
	}

	ret = disp_sys_script_get_item(FDT_HDMI_PATH, "vblank", &value, 1);
	if (ret == 1) {
		blanking = value;
		timing->ver_total_time = timing->y_res +
			(timing->b_interlace ? (2 * value) : value);
	} else {
		printf("fetch hdmi vblanking err.\n");
		return -1;
	}

	ret = disp_sys_script_get_item(FDT_HDMI_PATH, "vsync_offset", &value, 1);
	if (ret == 1) {
		timing->ver_front_porch = value;
	} else {
		printf("fetch hdmi vsync_offset err.\n");
		return -1;
	}

	ret = disp_sys_script_get_item(FDT_HDMI_PATH, "vsync", &value, 1);
	if (ret == 1) {
		timing->ver_sync_time = value;
		timing->ver_back_porch = blanking - value
					 - timing->ver_front_porch;
	} else {
		printf("fetch hdmi vsync err.\n");
		return -1;
	}

	ret = disp_sys_script_get_item(FDT_HDMI_PATH, "vpol", &value, 1);
	if (ret == 1) {
		timing->ver_sync_polarity = value;
	} else {
		printf("fetch hdmi vpol err.\n");
		return -1;
	}

	return 0;
}

#if defined(__LINUX_PLAT__)
s32 hdmi_init(struct platform_device *pdev)
#else
s32 hdmi_init(void)
#endif
{
#if defined(CONFIG_SND_SUNXI_SOC_HDMIAUDIO)
	__audio_hdmi_func audio_func;
#if defined (CONFIG_SND_SUNXI_SOC_AUDIOHUB_INTERFACE)
	__audio_hdmi_func audio_func_muti;
#endif
#endif
#if defined(__LINUX_PLAT__)
	unsigned int output_type0, output_mode0, output_type1, output_mode1;
#endif
	struct disp_device_func disp_func;
	int ret = 0;
	uintptr_t reg_base;
	int value;
	int node_offset = 0;
	struct disp_video_timings *vt = NULL;
	char str[10] = {0};

	hdmi_used = 0;
	b_hdmi_suspend_pre = b_hdmi_suspend = false;
	hdmi_power_used = 0;

	ret = disp_sys_script_get_item(FDT_HDMI_PATH, "status", (int *)str, 2);
	if (ret != 2) {
		printf("fetch hdmi err.\n");
		return -1;
	}
	if (0 != strncmp(str, "okay", 10)) {
		printf("hdmi not okay!%s\n", str);
		return -1;
	}


	hdmi_used = 1;

#if defined(__LINUX_PLAT__)
	/*  parse boot para */
	value = disp_boot_para_parse();
	output_type0 = (value >> 8) & 0xff;
	output_mode0 = (value) & 0xff;

	output_type1 = (value >> 24)& 0xff;
	output_mode1 = (value >> 16) & 0xff;
	if ((output_type0 == DISP_OUTPUT_TYPE_HDMI) ||
		(output_type1 == DISP_OUTPUT_TYPE_HDMI)) {
		boot_hdmi = true;
		ghdmi.bopen = 1;
		ghdmi.mode = (output_type0 == DISP_OUTPUT_TYPE_HDMI)?output_mode0:output_mode1;
		ghdmi.mode = hdmi_get_vic(ghdmi.mode);
	}
#endif

	hdmi_work_mode = DISP_HDMI_SEMI_AUTO;
	ret = disp_sys_script_get_item("hdmi", "hdmi_work_mode", &value, 1);
	if (ret == 1) {
		hdmi_work_mode = value;
	} else {
		__inf("Failed to parse hdmi_mode from dts\n");
		hdmi_work_mode = DISP_HDMI_SEMI_AUTO;
	}

	if (hdmi_work_mode == DISP_HDMI_SEMI_AUTO) {
		hdmi_get_video_timing_from_tv_mode(DISP_HDMI_MOD_DT0,
						&vt);
		hdmi_parse_dts_timing(vt);
	}

	ret = disp_sys_script_get_item("hdmi", "boot_mask", &value, 1);
	if (ret == 1 && value == 1) {
		__wrn("skip hdmi in boot.\n");
		return -1;
	}

	/* iomap */
	reg_base = disp_getprop_regbase("hdmi", "reg", 0);
	if (0 == reg_base) {
		__wrn("unable to map hdmi registers\n");
		ret = -EINVAL;
		goto err_iomap;
	}
	hdmi_core_set_base_addr(reg_base);

	node_offset = disp_fdt_nodeoffset("hdmi");
	of_periph_clk_config_setup(node_offset);
	/* get clk */
	hdmi_clk = of_clk_get(node_offset, 0);
	if (IS_ERR(hdmi_clk)) {
		__wrn("fail to get clk for hdmi\n");
		goto err_clk_get;
	}
	clk_enable_count = hdmi_clk->enable_count;
	hdmi_ddc_clk = of_clk_get(node_offset, 1);
	if (IS_ERR(hdmi_ddc_clk)) {
		__wrn("fail to get clk for hdmi ddc\n");
		goto err_clk_get;
	}

#if defined(CONFIG_MACH_SUN8IW12)
	hdmi_cec_clk = of_clk_get(node_offset, 2);
	if (hdmi_cec_clk == NULL) {
		__wrn("fail to get clk for hdmi cec\n");
		goto err_clk_get;
	}
#endif

	hdmi_clk_parent = clk_get_parent(hdmi_clk);
	/* parse io config */
	hdmi_parse_io_config();
	mutex_init(&mlock);

	if ((hdmi_power_used) && (power_enable_count == 0)) {
		hdmi_power_enable(hdmi_power);
		power_enable_count ++;
	}
	if (io_enable_count == 0) {
		hdmi_io_config(1);
		io_enable_count ++;
	}
	mutex_lock(&mlock);
	if (clk_enable_count == 0) {
		ret = hdmi_clk_enable();
		clk_enable_count ++;
	}
	mutex_unlock(&mlock);
	if (0 != ret) {
		clk_enable_count --;
		__wrn("fail to enable hdmi clk\n");
		goto err_clk_enable;
	}

#if defined(__LINUX_PLAT__)
	INIT_WORK(&ghdmi.hpd_work, hdmi_report_hpd_work);
#endif
#if defined(CONFIG_SWITCH) || defined(CONFIG_ANDROID_SWITCH)
	switch_dev_register(&hdmi_switch_dev);
#endif

	ret = disp_sys_script_get_item("hdmi", "hdmi_power", (int*)hdmi_power, 2);
	if (2 == ret) {
		hdmi_power_used = 1;
		if (hdmi_power_used) {
			__inf("[HDMI] power %s\n", hdmi_power);
			mutex_lock(&mlock);
			ret = hdmi_power_enable(hdmi_power);
			power_enable_count ++;
			mutex_unlock(&mlock);
			if (0 != ret) {
				power_enable_count --;
				__wrn("fail to enable hdmi power %s\n", hdmi_power);
				goto err_power;
			}
		}
	}

	ret = disp_sys_script_get_item("hdmi", "hdmi_io_regulator",
				       (int *)hdmi_io_regulator, 2);
	if (2 == ret) {
		hdmi_io_regulator_used = 1;
		mutex_lock(&mlock);
		ret = hdmi_power_enable(hdmi_io_regulator);
		mutex_unlock(&mlock);
		if (0 != ret)
			__wrn("fail to enable hdmi io power %s\n",
			      hdmi_io_regulator);
		else
			++io_regulator_enable_count;
	}

	ret = disp_sys_script_get_item("hdmi", "hdmi_cts_compatibility", &value, 1);
	if (1 == ret) {
		hdmi_core_set_cts_enable(value);
	}
	ret = disp_sys_script_get_item("hdmi", "hdmi_hdcp_enable", &value, 1);
	if (1 == ret) {
		hdmi_core_set_hdcp_enable(value);
	}

	ret = disp_sys_script_get_item("hdmi", "hdmi_hpd_mask", &value, 1);
	if (1 == ret) {
		hdmi_hpd_mask = value;
	}

	ret = disp_sys_script_get_item("hdmi", "hdmi_cec_support", &value, 1);
	if ((1 == ret) && (1 == value)) {
		hdmi_cec_support = true;
	}
	hdmi_core_cec_enable(hdmi_cec_support);
#if defined(__UBOOT_PLAT__)
	hdmi_core_update_detect_time(10);
#endif
	hdmi_core_initial(boot_hdmi);

#if defined(__LINUX_PLAT__)
	run_sem = kmalloc(sizeof(struct semaphore),GFP_KERNEL | __GFP_ZERO);
	if (!run_sem) {
		__wrn("fail to kmalloc memory for run_sem\n");
		goto err_sem;
	}
	sema_init((struct semaphore*)run_sem,0);

	HDMI_task = kthread_create(hdmi_run_thread, (void*)0, "hdmi proc");
	if (IS_ERR(HDMI_task)) {
		s32 err = 0;
		__wrn"Unable to start kernel thread %s.\n\n", "hdmi proc");
		err = PTR_ERR(HDMI_task);
		HDMI_task = NULL;
		goto err_thread;
	}
	wake_up_process(HDMI_task);
#endif

#if defined(CONFIG_SND_SUNXI_SOC_HDMIAUDIO)
	audio_func.hdmi_audio_enable = hdmi_audio_enable;
	audio_func.hdmi_set_audio_para = hdmi_set_audio_para;
	audio_set_hdmi_func(&audio_func);
#if defined (CONFIG_SND_SUNXI_SOC_AUDIOHUB_INTERFACE)
	audio_func_muti.hdmi_audio_enable = hdmi_audio_enable;
	audio_func_muti.hdmi_set_audio_para = hdmi_set_audio_para;
	audio_set_muti_hdmi_func(&audio_func_muti);
#endif
#endif
	memset(&disp_func, 0, sizeof(struct disp_device_func));
	disp_func.enable = hdmi_enable;
	disp_func.disable = hdmi_disable;
	disp_func.get_work_mode = hdmi_get_work_mode;
	disp_func.set_mode = hdmi_set_display_mode;
	disp_func.mode_support = hdmi_mode_support;
	disp_func.get_support_mode = hdmi_get_support_mode;
	disp_func.get_HPD_status = hdmi_get_HPD_status;
	disp_func.get_input_csc = hdmi_get_input_csc;
	disp_func.get_video_timing_info = hdmi_get_video_timming_info;
	disp_func.suspend = hdmi_suspend;
	disp_func.resume = hdmi_resume;
	disp_set_hdmi_func(&disp_func);

	return 0;

#if defined(__LINUX_PLAT__)
err_thread:
	kfree(run_sem);
err_sem:
#endif
	hdmi_power_disable(hdmi_power);
err_power:
	hdmi_clk_disable();
err_clk_enable:
#if defined(__CLK_SUPPORT__)
err_clk_get:
#endif
err_iomap:
	return -1;
}

s32 hdmi_exit(void)
{
	if (hdmi_used) {
		hdmi_core_exit();
#if defined(__LINUX_PLAT__)
		if (run_sem)	{
			kfree(run_sem);
		}

		run_sem = NULL;
		if (HDMI_task) {
			kthread_stop(HDMI_task);
			HDMI_task = NULL;
		}
#endif
		if ((1 == hdmi_power_used) && (0 != power_enable_count)) {
			hdmi_power_disable(hdmi_power);
		}
		if (0 != clk_enable_count) {
			hdmi_clk_disable();
			clk_enable_count--;
		}
	}

	return 0;
}
