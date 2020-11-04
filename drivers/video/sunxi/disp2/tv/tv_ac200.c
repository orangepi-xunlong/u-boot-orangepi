/*
 * drivers/video/sunxi/disp2/tv/tv_ac200.c
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
#include "tv_ac200.h"
#include "tv_ac200_lowlevel.h"
#include <i2c.h>
#if defined(CONFIG_MACH_SUN50IW6)
#include <efuse_map.h>
#endif

/* clk */
#define DE_LCD_CLK "lcd0"
#define DE_LCD_CLK_SRC "pll_video0"

static char modules_name[40] = {0};
static enum disp_tv_mode g_tv_mode = DISP_TV_MOD_PAL;
static char key_name[20] = "ac200";
static struct clk *tv_clk;

static u32   tv_screen_id = 0;
static u32   tv_used = 0;
static u32   tv_power_used = 0;
static char  tv_power[16] = {0};
static unsigned long pwm_handle = 0;
static u32 ccir_clk_div;

static bool tv_suspend_status;
//static bool tv_io_used[28];
//static disp_gpio_set_t tv_io[28];

/*#define	 AC200_DEBUG*/
#define __ac200_err(msg...) do { \
	{printf("[ac200] %s,line:%d:    ", \
	__func__, __LINE__); printf(msg); } \
	} while (0)

#if defined(AC200_DEBUG)
#define __ac200_dbg(msg...) do { \
	{printf("[ac200] %s,line:%d:    ", \
	__func__, __LINE__); printf(msg); } \
	} while (0)
#else
#define __ac200_dbg(msg...)
#endif /*endif AC200_DEBUG */

static struct disp_device *tv_device = NULL;
static struct disp_vdevice_source_ops tv_source_ops;

u32 ac200_twi_addr;

struct ac200_tv_priv tv_priv;
struct disp_video_timings tv_video_timing[] =
{
 /* vic  tv_mode         PCLK     AVI   x   y   HT  HBP HFP HST  VT  VBP VFP VST  H_P V_P I vas TRD */

 	{0, DISP_TV_MOD_NTSC,54000000, 0, 720, 480, 858, 57, 19, 62, 525, 15, 4,  3,  0,  0,  0, 0, 0},
	{0,	DISP_TV_MOD_PAL, 54000000, 0, 720, 576, 864, 69, 12, 63, 625, 19, 2,  3,  0,  0,  0, 0, 0},
};

extern struct disp_device* disp_vdevice_register(struct disp_vdevice_init_data *data);
extern s32 disp_vdevice_unregister(struct disp_device *vdevice);
extern s32 disp_vdevice_get_source_ops(struct disp_vdevice_source_ops *ops);
extern unsigned int disp_boot_para_parse(void);

void tv_report_hpd_work(void)
{
	printf("there is null report hpd work,you need support the switch class!");
}

s32 tv_detect_thread(void *parg)
{
	printf("there is null tv_detect_thread,you need support the switch class!");
	return -1;
}

s32 tv_detect_enable(void)
{
	printf("there is null tv_detect_enable,you need support the switch class!");
	return -1;
}

s32 tv_detect_disable(void)
{
	printf("there is null tv_detect_disable,you need support the switch class!");
    	return -1;
}

#if 0
static s32 tv_power_on(u32 on_off)
{
	if(tv_power_used == 0)
	{
		return 0;
	}
    if(on_off)
    {
        disp_sys_power_enable(tv_power);
    }
    else
    {
        disp_sys_power_disable(tv_power);
    }

    return 0;
}
#endif

static s32 tv_clk_init(void)
{
	//disp_sys_clk_set_parent(DE_LCD_CLK, DE_LCD_CLK_SRC);

	return 0;
}
#if 0
static s32 tv_clk_exit(void)
{
	return 0;
}
#endif

static int tv_i2c_used = 0;

static int  tv_i2c_init(void)
{
    int ret;
    int value;

    ret = disp_sys_script_get_item(key_name, "tv_twi_used", &value, 1);
    if (1 == ret) {
	    tv_i2c_used = value;
	    if (tv_i2c_used == 1) {
		    ret = disp_sys_script_get_item(key_name, "tv_twi_addr",
						   &value, 1);
		    if (ret != 1) {
			    printf("get tv_twi_addr failed\n");
			    return -1;
		    }
		    ac200_twi_addr = value;
		    ret = disp_sys_script_get_item(key_name, "tv_twi_id",
						   &value, 1);
		    if (ret != 1) {
			    printf("get tv_twi_id failed\n");
			    return -1;
		    }
#if defined(CONFIG_SYS_I2C) && defined (CONFIG_MACH_SUN50IW6)
		    i2c_set_bus_num(1);
		    i2c_init(CONFIG_SYS_I2C_AC200_SPEED,
			     value);
		    printf("speed=%d, slave=%d\n",
			   (u32)CONFIG_SYS_I2C_AC200_SPEED,
			   (u32)ac200_twi_addr);
#else
		    i2c_init(CONFIG_SYS_I2C_SPEED,
			     value);
		    printf("speed=%d, slave=%d\n",
			   (u32)CONFIG_SYS_I2C_SPEED,
			   (u32)ac200_twi_addr);
#endif /*endif CONFIG_SYS_I2C */
	    }
    }
    return 0;
}

#if defined(CONFIG_MACH_SUN50IW6)
/**
 * @name       tv_read_sid
 * @brief      read tv out sid from efuse
 * @param[IN]   none
 * @param[OUT]  p_dac_cali:tv_out dac cali
 * @param[OUT]  p_bandgap:tv_out bandcap
 * @return	return 0 if success,-1 if fail
 */
static s32 tv_read_sid(u16 *p_dac_cali, u16 *p_bandgap)
{
	s32 ret = 0;
	u8 buf[48];
	s32 buf_len = 48;

	if (p_dac_cali == NULL || p_bandgap == NULL) {
		__ac200_err("%s's pointer type args are NULL!\n", __func__);
		return -1;
	}
	ret = sunxi_efuse_read(EFUSE_EMAC_NAME, buf, &buf_len);
	if (ret < 0) {
		__ac200_err("sunxi_efuse_readn failed:%d\n", ret);
		return ret;
	}
	*p_dac_cali = buf[2] + (buf[3] << 8);
	*p_bandgap = buf[4] + (buf[5] << 8);
	__ac200_dbg("buf[2]:0x%x, buf[3]:0x%x, buf[4]:0x%x, buf[5]:0x%x\n",
		    buf[2], buf[3], buf[4], buf[5]);
	return 0;
}
#endif /*endif CONFIG_MACH_SUN50IW6 */

static s32 tv_clk_config(u32 mode)
{
	unsigned long pixel_clk, pll_rate, lcd_rate, dclk_rate;//hz
	unsigned long pll_rate_set, lcd_rate_set, dclk_rate_set;//hz
	u32 pixel_repeat, tcon_div, lcd_div;
	struct clk * parent = NULL;

	if(11 == mode) {
		pixel_clk = tv_video_timing[1].pixel_clk;
		pixel_repeat = tv_video_timing[1].pixel_repeat;
	}
	else {
		pixel_clk = tv_video_timing[0].pixel_clk;
		pixel_repeat = tv_video_timing[0].pixel_repeat;
	}
	lcd_div = 1;
	dclk_rate = pixel_clk * (pixel_repeat + 1);
	tcon_div = ccir_clk_div;
	lcd_rate = dclk_rate * tcon_div;
	pll_rate = lcd_rate * lcd_div;

	parent = clk_get_parent(tv_clk);

	if (parent)
		clk_set_rate(tv_clk->parent, pll_rate);
	else
	{
		__ac200_err("tv_clk has no parent clk\n");
		return -1;
	}

	__ac200_dbg("tcon_div:%d lcd_div:%d\n", tcon_div, lcd_div);
	pll_rate_set = clk_get_rate(parent);
	lcd_rate_set = pll_rate_set / lcd_div;
	clk_set_rate(tv_clk, lcd_rate_set);
	lcd_rate_set = clk_get_rate(tv_clk);
	dclk_rate_set = lcd_rate_set / tcon_div;
	if(dclk_rate_set != dclk_rate)
		__ac200_err("pclk=%ld, cur=%ld\n", dclk_rate, dclk_rate_set);

	__ac200_dbg("parent clk=%ld, pclk=%ld\n", pll_rate_set, dclk_rate_set);

	return 0;
}

static s32 tv_clk_enable(u32 mode)
{
	int ret = 0;

	tv_clk_config(mode);
	if (tv_clk)
		ret = clk_prepare_enable(tv_clk);

	return ret;
}

static s32 tv_clk_disable(void)
{
	if (tv_clk)
		clk_disable(tv_clk);

	return 0;
}


static int tv_pin_config(void)
{
	int ret = 0;
	ret = gpio_request_simple("ac200", NULL);
	return ret;
}

static s32 tv_open(void)
{
	s32 ret = 0;
	if (tv_source_ops.tcon_enable)
		tv_source_ops.tcon_enable(tv_device);

	aw1683_tve_set_mode(g_tv_mode);

	ret = aw1683_tve_open();
	if (ret != 0)
		__ac200_err("tve_open failed:%d\n", ret);
	else
		printf("tv_open finsih\n");
	return 0;
}

static s32 tv_close(void)
{
	aw1683_tve_close();
	if(tv_source_ops.tcon_disable)
    	tv_source_ops.tcon_disable(tv_device);

    return 0;
}

static s32 tv_set_mode(enum disp_tv_mode tv_mode)
{

    g_tv_mode = tv_mode;
    return 0;
}

static s32 tv_get_hpd_status(void)
{
	s32 hot_plug_state = 0;
	hot_plug_state = aw1683_tve_plug_status();
	return hot_plug_state;
}

static s32 tv_get_mode_support(enum disp_tv_mode tv_mode)
{
    if(tv_mode == DISP_TV_MOD_PAL || tv_mode == DISP_TV_MOD_NTSC)
		return 1;

    return 0;
}

static s32 tv_get_video_timing_info(struct disp_video_timings **video_info)
{
	struct disp_video_timings *info;
	int ret = -1;
	int i, list_num;
	info = tv_video_timing;

	list_num = sizeof(tv_video_timing)/sizeof(struct disp_video_timings);
	for(i=0; i<list_num; i++) {
		if(info->tv_mode == g_tv_mode){
			*video_info = info;
			ret = 0;
			break;
		}

		info ++;
	}
	return ret;
}

/*0:rgb;  1:yuv*/
static s32 tv_get_input_csc(void)
{
	return 1;
}

static s32 tv_get_interface_para(void* para)
{
	struct disp_vdevice_interface_para intf_para;

	intf_para.intf = 0;
	intf_para.sub_intf = 12;
	intf_para.sequence = 0;
	intf_para.clk_phase = 0;
	intf_para.sync_polarity = 0;
	intf_para.ccir_clk_div = ccir_clk_div;
	intf_para.input_csc = tv_get_input_csc();
	if(g_tv_mode == DISP_TV_MOD_NTSC)
		intf_para.fdelay = 2;//ntsc
	else
		intf_para.fdelay = 1;//pal

	if(para)
		memcpy(para, &intf_para, sizeof(struct disp_vdevice_interface_para));

	return 0;
}


s32 tv_suspend(void)
{
	if(tv_used && (false == tv_suspend_status)) {
		tv_suspend_status = true;
		tv_detect_disable();
		if(tv_source_ops.tcon_disable)
			tv_source_ops.tcon_disable(tv_device);

		tv_clk_disable();
	}

	return 0;
}

s32 tv_resume(void)
{
	if(tv_used && (true == tv_suspend_status)) {
		tv_suspend_status= false;
		tv_clk_enable(g_tv_mode);

		tv_detect_enable();
	}

	return  0;
}

int tv_ac200_init(void)
{

	int ret;
	int value;
	struct disp_vdevice_init_data init_data;
	u16 dac_cali = 0, bandgap = 0;
	/*int node_offset = 0;*/

	tv_suspend_status = 0;

	ret = disp_sys_script_get_item("ac200", "tv_used", &value, 1);

	if(1 == ret) {
		tv_used = value;
 		if(tv_used) {

			ret = disp_sys_script_get_item(key_name, "tv_module_name", (int*)modules_name, 2);
			if(2 == ret) {
				ret = strcmp(modules_name, "tv_ac200");
				if (ret) {
					printf("tv driver is not tv_ac200! module_name = %s.\n", modules_name);
					return -1;
				}
			}

			tv_clk = clk_get(NULL, "tcon_lcd");
			if (IS_ERR(tv_clk)) {
				__ac200_err("fail to get clk for ac200\n");
				return -1;
			}

			ret = tv_pin_config();
			if (ret != 0) {
				__ac200_err("tv_pin_config failed:%d\n", ret);
				return -1;
			}

			ret = disp_sys_script_get_item(key_name, "tv_power", (int*)tv_power, 2);

			if(2 == ret) {
				tv_power_used = 1;
				printf("[TV] tv_power: %s\n", tv_power);
			}

			ret = disp_sys_script_get_item("ac200", "tv_pwm_ch",
						       &value, 1);
			if (ret != 1) {
				__ac200_err("fail to get pwm ch num\n");
				return -1;
			}
			pwm_handle = disp_sys_pwm_request(value);
			disp_sys_pwm_config(pwm_handle, 20, 41);
			disp_sys_pwm_enable(pwm_handle);

			ret = disp_sys_script_get_item("ac200", "tv_clk_div",
						       &value, 1);
			if (ret != 1) {
				__ac200_err("fail to get tv_clk_div\n");
				return -1;
			}
			ccir_clk_div = value;

			tv_i2c_init();

			memset(&init_data, 0, sizeof(struct disp_vdevice_init_data));
			init_data.disp = tv_screen_id;
			memcpy(init_data.name, modules_name, 32);
			init_data.type = DISP_OUTPUT_TYPE_TV;
			init_data.fix_timing = 0;
			init_data.func.enable = tv_open;
			init_data.func.disable = tv_close;
			init_data.func.get_HPD_status = tv_get_hpd_status;
			init_data.func.set_mode = tv_set_mode;
			init_data.func.mode_support = tv_get_mode_support;
			init_data.func.get_video_timing_info = tv_get_video_timing_info;
			init_data.func.get_interface_para = tv_get_interface_para;
			init_data.func.get_input_csc = tv_get_input_csc;
			tv_device = disp_vdevice_register(&init_data);

			tv_clk_init();
			tv_clk_enable(g_tv_mode);
			disp_vdevice_get_source_ops(&tv_source_ops);
			if (tv_source_ops.tcon_simple_enable)
				tv_source_ops.tcon_simple_enable(tv_device);
			else
				printf("tcon_simple_enable failed\n");
#if defined(CONFIG_MACH_SUN50IW6)
			if (tv_read_sid(&dac_cali, &bandgap) != 0) {
				dac_cali = 0;
				bandgap = 0;
			}
#endif /*endif CONFIG_MACH_SUN50IW6*/
			__ac200_dbg("dac cali:0x%x, bandgap:0x%x\n", dac_cali,
				    bandgap);
			aw1683_tve_init(&dac_cali, &bandgap);
		}
	} else
		tv_used = 0;
	return 0;
}
