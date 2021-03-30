#include "tv_ac200.h"
#include "tv_ac200_lowlevel.h"
#include <i2c.h>

#define PWM_ID 16
/* clk */
#define DE_LCD_CLK "lcd0"
#define DE_LCD_CLK_SRC "pll_video0"

static char modules_name[16] = {0};
static enum disp_tv_mode g_tv_mode = DISP_TV_MOD_PAL;
static char key_name[20] = "ac200";
static struct clk *tv_clk;

static u32   tv_screen_id = 0;
static u32   tv_used = 0;
static u32   tv_power_used = 0;
static char  tv_power[16] = {0};
static unsigned long pwm_handle = 0;

static bool tv_suspend_status;
//static bool tv_io_used[28];
//static disp_gpio_set_t tv_io[28];

static struct disp_device *tv_device = NULL;
static struct disp_vdevice_source_ops tv_source_ops;

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
    if(1 == ret)
    {
        tv_i2c_used = value;
        if(tv_i2c_used == 1)
        {
        	printf("speed=%d, slave=%d\n",(u32)CONFIG_SYS_I2C_SPEED, (u32)CONFIG_SYS_I2C_SLAVE);
            i2c_init(CONFIG_SYS_I2C_SPEED,CONFIG_SYS_I2C_SLAVE);         //cpus twi0 for cvbs
        }
    }
    return 0;
}

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
	tcon_div = 8;//fixme
	lcd_rate = dclk_rate * tcon_div;
	pll_rate = lcd_rate * lcd_div;

	parent = clk_get_parent(tv_clk);
	if (parent)
		clk_set_rate(tv_clk->parent, pll_rate);
	pll_rate_set = clk_get_rate(parent);
	lcd_rate_set = pll_rate_set / lcd_div;
	clk_set_rate(tv_clk, lcd_rate_set);
	lcd_rate_set = clk_get_rate(tv_clk);
	dclk_rate_set = lcd_rate_set / tcon_div;
	if(dclk_rate_set != dclk_rate)
		printf("pclk=%ld, cur=%ld\n", dclk_rate, dclk_rate_set);

	return 0;
}
/*
static s32 tv_clk_config(u32 mode)
{
	unsigned long pixel_clk, pll_rate, lcd_rate, dclk_rate;//hz
	unsigned long pll_rate_set, lcd_rate_set, dclk_rate_set;//hz
	u32 pixel_repeat, tcon_div, lcd_div;

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
	tcon_div = 8;//fixme
	lcd_rate = dclk_rate * tcon_div;
	pll_rate = lcd_rate * lcd_div;
	disp_sys_clk_set_rate(DE_LCD_CLK_SRC, pll_rate);
	pll_rate_set = disp_sys_clk_get_rate(DE_LCD_CLK_SRC);
	lcd_rate_set = pll_rate_set / lcd_div;
	disp_sys_clk_set_rate(DE_LCD_CLK, lcd_rate_set);
	lcd_rate_set = disp_sys_clk_get_rate(DE_LCD_CLK_SRC);
	dclk_rate_set = lcd_rate_set / tcon_div;
	if(dclk_rate_set != dclk_rate)
		printf("pclk=%ld, cur=%ld\n", dclk_rate, dclk_rate_set);

	return 0;
}
*/
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

/*
static s32 tv_clk_enable(u32 mode)
{
	tv_clk_config(mode);
	disp_sys_clk_enable(DE_LCD_CLK);

	return 0;
}

static s32 tv_clk_disable(void)
{

	disp_sys_clk_disable(DE_LCD_CLK);

	return 0;
}
*/

/*
static int tv_parse_config(void)
{
	disp_gpio_set_t  *gpio_info;
	int i, ret;
	char io_name[32];

	for(i=0; i<28; i++) {
		gpio_info = &(tv_io[i]);
		sprintf(io_name, "tv_d%d", i);
		ret = disp_sys_script_get_item(key_name, io_name, (int *)gpio_info, sizeof(disp_gpio_set_t)/sizeof(int));
		if(ret == 3)
		{
		  tv_io_used[i]= 1;
		}
	}

  return 0;
}*/
/*
static int tv_pin_config(u32 bon)
{
	int hdl,i;

	for(i=0; i<28; i++)	{
		if(tv_io_used[i]) {
			disp_gpio_set_t  gpio_info[1];

			memcpy(gpio_info, &(tv_io[i]), sizeof(disp_gpio_set_t));
			if(!bon) {
				gpio_info->mul_sel = 7;
			}
			hdl = disp_sys_gpio_request(gpio_info, 1);
			disp_sys_gpio_release(hdl, 2);
		}
	}
	return 0;
}*/

static int tv_pin_config(u32 bon)
{
	return disp_sys_pin_set_state("ac200", (1==bon)? DISP_PIN_STATE_ACTIVE:DISP_PIN_STATE_SLEEP);
}

static s32 tv_open(void)
{
	aw1683_tve_set_mode(g_tv_mode);
	if(tv_source_ops.tcon_enable)
    	tv_source_ops.tcon_enable(tv_device);

	aw1683_tve_open();
    return 0;
}

static s32 tv_close(void)
{
	aw1683_tve_close();
	if(tv_source_ops.tcon_disable)
    	tv_source_ops.tcon_disable(tv_device);

    if(tv_source_ops.tcon_simple_enable)
    	tv_source_ops.tcon_simple_enable(tv_device);
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

static s32 tv_get_interface_para(void* para)
{
	struct disp_vdevice_interface_para intf_para;

	intf_para.intf = 0;
	intf_para.sub_intf = 12;
	intf_para.sequence = 0;
	intf_para.clk_phase = 0;
	intf_para.sync_polarity = 0;
	if(g_tv_mode == DISP_TV_MOD_NTSC)
		intf_para.fdelay = 1;//ntsc
	else
		intf_para.fdelay = 2;//pal

	if(para)
		memcpy(para, &intf_para, sizeof(struct disp_vdevice_interface_para));

	return 0;
}

//0:rgb;  1:yuv
static s32 tv_get_input_csc(void)
{
	return 1;
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
		if(tv_source_ops.tcon_simple_enable)
			tv_source_ops.tcon_simple_enable(tv_device);
	}

	return  0;
}

int tv_ac200_init(void)
{

	int ret;
	int value;
	struct disp_vdevice_init_data init_data;

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

			tv_clk = clk_get(NULL, "tcon0");
			if (IS_ERR(tv_clk)) {
				__wrn("fail to get clk for hdmi\n");
			}

			tv_pin_config(1);

			ret = disp_sys_script_get_item(key_name, "tv_power", (int*)tv_power, 2);

			if(2 == ret) {
				tv_power_used = 1;
				printf("[TV] tv_power: %s\n", tv_power);
			}
			/* use spwm, pwm_id is 16 */
			pwm_handle = disp_sys_pwm_request(PWM_ID);
			disp_sys_pwm_config(pwm_handle, 0, 41);
			disp_sys_pwm_enable(pwm_handle);

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
			if(tv_source_ops.tcon_simple_enable)
        		tv_source_ops.tcon_simple_enable(tv_device);
        	else
				printf("tv_init_tcon_not_enable!\n");
			aw1683_tve_init();
			disp_delay_ms(200);
			aw1683_tve_plug_status();
		}
	} else
		tv_used = 0;
	return 0;
}

