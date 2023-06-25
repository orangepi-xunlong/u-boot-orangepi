/* linux/drivers/video/sunxi/disp/dev_disp.c
 *
 * Copyright (c) 2013 Allwinnertech Co., Ltd.
 * Author: Tyle <tyle@allwinnertech.com>
 *
 * Display driver for sunxi platform
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include "dev_disp.h"

disp_drv_info g_disp_drv;

#define MY_BYTE_ALIGN(x) ( ( (x + (4*1024-1)) >> 12) << 12)             /* alloc based on 4K byte */

static u32 suspend_output_type[2] = {0,0};
static u32 suspend_status = 0;//0:normal; suspend_status&1 != 0:in early_suspend; suspend_status&2 != 0:in suspend;
//static u32 suspend_prestep = 3; //0:after early suspend; 1:after suspend; 2:after resume; 3 :after late resume
static u32 power_status_init = 0;

struct disp_layer_config lyr_cfg[16];
struct disp_layer_config2 lyr_cfg2[16];

//uboot plat
static u32    lcd_flow_cnt[2] = {0};
static u32    lcd_open_flow_retry[2] = {0};
static s8   lcd_op_finished[2] = {0};
static struct timer_list lcd_timer[2];
static s8   lcd_op_start[2] = {0};

static u32 DISP_print = 0xffff;   //print cmd which eq DISP_print
__attribute__((unused)) static u32 g_output_type = DISP_OUTPUT_TYPE_LCD;

uintptr_t disp_getprop_regbase(char *main_name, char *sub_name, u32 index)
{
	char compat[32];
	u32 len = 0;
	int node;
	int ret = -1;
	int value[32] = {0};
	uintptr_t reg_base = 0;

	len = sprintf(compat, "%s", main_name);
	if (len > 32)
		__wrn("size of mian_name is out of range\n");

	//node = fdt_path_offset(working_fdt,compat);
	node = disp_fdt_nodeoffset(compat);
	if (node < 0) {
		__wrn("fdt_path_offset %s fail\n", compat);
		goto exit;
	}

	ret = fdt_getprop_u32(working_fdt, node, sub_name, (uint32_t*)value);
	if (0 > ret)
		__wrn("fdt_getprop_u32 %s.%s fail\n", main_name, sub_name);
	else {
		reg_base = value[index * 4] + value[index * 4 + 1];
	}

exit:
	return reg_base;
}

u32 disp_getprop_irq(char *main_name, char *sub_name, u32 index)
{
	char compat[32];
	u32 len = 0;
	int node;
	int ret = -1;
	int value[32] = {0};
	u32 irq = 0;

	len = sprintf(compat, "%s", main_name);
	if (len > 32)
		__wrn("size of mian_name is out of range\n");

	//node = fdt_path_offset(working_fdt,compat);
	node = disp_fdt_nodeoffset(compat);
	if (node < 0) {
		__wrn("fdt_path_offset %s fail\n", compat);
		goto exit;
	}

	ret = fdt_getprop_u32(working_fdt, node, sub_name, (uint32_t*)value);
	if (0 > ret)
		__wrn("fdt_getprop_u32 %s.%s fail\n", main_name, sub_name);
	else {
		irq = value[index * 3 + 1];
		if (0 == value[index * 3])
			irq += 32;
	}

exit:
	return irq;
}

static s32 copy_to_user(void *src, void* dest, u32 size)
{
	memcpy(dest, src, size);
	return 0;
}


static void drv_lcd_open_callback(void *parg)
{
	disp_lcd_flow *flow = NULL;
	u32 sel = (u32)(unsigned long)parg;
	s32 i = lcd_flow_cnt[sel]++;
	flow = bsp_disp_lcd_get_open_flow(sel);

	if (NULL == flow) {
		if (lcd_open_flow_retry[sel] <= 10) {
			lcd_open_flow_retry[sel]++;;
			lcd_flow_cnt[sel] = 0;
			drv_lcd_open_callback((void *)(unsigned long)sel);
			return;
		}
		printf("%s lcd open flow is NULL! LCD enable fail!\n",
		       __func__);
		lcd_op_start[sel] = 0;
		lcd_op_finished[sel] = 1;
		del_timer(&lcd_timer[sel]);
		return;
	}

	if (i < flow->func_num) {
		flow->func[i].func(sel);
		if (flow->func[i].delay == 0) {
			drv_lcd_open_callback((void *)(unsigned long)sel);
		} else {
			lcd_timer[sel].data = sel;
			lcd_timer[sel].expires = flow->func[i].delay;
			lcd_timer[sel].function = drv_lcd_open_callback;
			add_timer(&lcd_timer[sel]);
		}
	} else if (i >= flow->func_num) {
		lcd_op_finished[sel] = 1;
		tick_printf("LCD open finish\n");
	}
}

static s32 drv_lcd_enable(u32 sel)
{
	lcd_flow_cnt[sel] = 0;
	lcd_op_finished[sel] = 0;
	lcd_op_start[sel] = 1;

	init_timer(&lcd_timer[sel]);

	drv_lcd_open_callback((void *)(unsigned long)sel);

    return 0;
}

static s8 drv_lcd_check_open_finished(u32 sel)
{
	if ((lcd_op_start[sel] == 1)) {
		if (lcd_op_finished[sel]) {
			del_timer(&lcd_timer[sel]);
			lcd_op_start[sel] = 0;
		}
		return lcd_op_finished[sel];
	}

	return 1;
}

int sunxi_get_lcd_op_finished(void)
{
	int i;
	struct disp_manager *mgr    = NULL;
	struct disp_device *dispdev = NULL;

	for (i = 0; i < DISP_SCREEN_NUM; ++i) {
		dispdev = NULL;
		mgr     = g_disp_drv.mgr[i];
		if (mgr)
			dispdev = mgr->device;
		if (dispdev) {
			if (dispdev->type == DISP_OUTPUT_TYPE_LCD) {
				return drv_lcd_check_open_finished(i);
			}
		}
	}
	return 1;
}

static void drv_lcd_close_callback(void *parg)
{
	disp_lcd_flow *flow;
	u32 sel = (u32)(unsigned long)parg;
	s32 i = lcd_flow_cnt[sel]++;

	flow = bsp_disp_lcd_get_close_flow(sel);

	if (i < flow->func_num) {
		flow->func[i].func(sel);
		if (flow->func[i].delay == 0)
			drv_lcd_close_callback((void *)(unsigned long)sel);
		else {
			lcd_timer[sel].data = sel;
			lcd_timer[sel].expires = flow->func[i].delay;
			lcd_timer[sel].function = drv_lcd_close_callback;
			add_timer(&lcd_timer[sel]);
		}
	}	else if (i == flow->func_num) {
		lcd_op_finished[sel] = 1;
	}
}

__weak s32 drv_lcd_disable(u32 sel)
{
	lcd_flow_cnt[sel] = 0;
	lcd_op_finished[sel] = 0;
	lcd_op_start[sel] = 1;

	init_timer(&lcd_timer[sel]);

	drv_lcd_close_callback((void *)(unsigned long)sel);

	return 0;
}

static s8 drv_lcd_check_close_finished(u32 sel)
{
	if ((lcd_op_start[sel] == 1)) {
		if (lcd_op_finished[sel])	{
			del_timer(&lcd_timer[sel]);
			lcd_op_start[sel] = 0;
		}
		return lcd_op_finished[sel];
	}
	return 1;
}

#if defined(SUPPORT_HDMI)
s32 disp_set_hdmi_func(struct disp_device_func * func)
{
	return bsp_disp_set_hdmi_func(func);
}

s32 disp_set_hdmi_detect(bool hpd)
{
	return bsp_disp_hdmi_set_detect(hpd);
}
EXPORT_SYMBOL(disp_set_hdmi_detect);
#endif

#if defined(SUPPORT_EDP)
s32 disp_set_edp_func(struct disp_tv_func *func)
{
	return bsp_disp_set_edp_func(func);
}
EXPORT_SYMBOL(disp_set_edp_func);
#endif

#if defined(SUPPORT_TV)
s32 disp_tv_register(struct disp_tv_func * func)
{
	return bsp_disp_tv_register(func);
}
EXPORT_SYMBOL(disp_tv_register);
#endif

/*TODO:sunxi_board_shutdown*/
/*extern int sunxi_board_shutdown(void);*/
static s32 drv_disp_check_spec(void)
{
	unsigned int lcd_used = 0;
	unsigned int lcd_x = 0, lcd_y = 0;
	int ret = 0;
	int value = 0;
	int limit_w = 0xffff, limit_h = 0xffff;

#if defined(CONFIG_MACH_SUN8IW6)
	limit_w = 2048;
	limit_h = 1536;
#endif
	ret = disp_sys_script_get_item(FDT_LCD0_PATH, "lcd_used", &value, 1);
	if (ret == 1)
	  lcd_used = value;

	if (1 == lcd_used) {
		ret = disp_sys_script_get_item(FDT_LCD0_PATH, "lcd_x", &value, 1);
	  if (ret == 1)
	      lcd_x = value;

	  ret = disp_sys_script_get_item(FDT_LCD0_PATH, "lcd_y", &value, 1);
	  if (ret == 1)
	      lcd_y = value;

		if (((lcd_x > limit_w) && (lcd_y > limit_h)) ||
				((lcd_x > limit_h) && (lcd_y > limit_w))) {
			printf("fatal err: cannot support lcd with resolution(%d*%d) larger than %d*%d, the system will shut down!\n",
				lcd_x, lcd_y,limit_w,limit_h);
			/*sunxi_board_shutdown();*/
		}

	}

	return 0;
}

/**
 * @name       :disp_draw_colorbar
 * @brief      :draw colorbar using DE's LAYER MODE
 * @param[IN]  :disp:screen index
 * @return     :0 if success
 */
int disp_draw_colorbar(u32 disp)
{
	struct disp_manager *mgr = NULL;
	struct disp_layer_config config[4];
	unsigned int i = 0;
	unsigned int width = 0, height = 0, num_screens;
	int ret = -1;

	num_screens = bsp_disp_feat_get_num_screens();
	if (disp < num_screens)
		mgr = g_disp_drv.mgr[disp];
	else
		return ret;

	if (mgr && mgr->device && mgr->device->get_resolution)
		mgr->device->get_resolution(mgr->device, &width, &height);
	else
		return ret;

	memset(config, 0, 4 * sizeof(struct disp_layer_config));
	for (i = 0; i < 4; ++i) {
		config[i].channel = de_feat_get_num_vi_chns(disp);
		config[i].layer_id = i;
		config[i].enable = 1;
		config[i].info.zorder = 16;
		config[i].info.mode = LAYER_MODE_COLOR;
		config[i].info.fb.format = DISP_FORMAT_ARGB_8888;
		config[i].info.screen_win.width = width / 4;
		config[i].info.screen_win.height = height;
		config[i].info.screen_win.x = (width / 4) * i;
		config[i].info.screen_win.y = 0;
		config[i].info.fb.crop.x =
		    ((long long)(config[i].info.screen_win.x) << 32);
		config[i].info.fb.crop.y =
		    ((long long)(config[i].info.screen_win.y) << 32);
		config[i].info.fb.crop.width =
		    ((long long)(config[i].info.screen_win.width) << 32);
		config[i].info.fb.crop.height =
		    ((long long)(config[i].info.screen_win.height) << 32);
	}
	config[0].info.color = 0xffff0000; /*red*/
	config[1].info.color = 0xff00ff00; /*green*/
	config[2].info.color = 0xff0000ff; /*blue*/
	config[3].info.color = 0xffffff00; /*yellow*/

	if (mgr->set_layer_config)
		ret = mgr->set_layer_config(mgr, config, 4);

	return ret;
}

extern __s32 hdmi_init(void);
extern __s32 tv_init(void);
extern int tv_ac200_init(void);
extern void disp_fdt_init(void);
extern struct disp_eink_manager *disp_get_eink_manager(unsigned int disp);

s32 drv_disp_init(void)
{
	disp_bsp_init_para *para;
	int disp, num_screens;
	int i;
	int ret = 0;
	int counter = 0;
	int node_offset = 0;
	s32 value = 0;

	tick_printf("%s\n", __func__);
	disp_fdt_init();

	/* check if the resolution of lcd supported */
	drv_disp_check_spec();

	memset(&g_disp_drv, 0, sizeof(disp_drv_info));
	para = &g_disp_drv.para;

	/* iomap */
	/* de - [device(tcon-top)] - lcd0/1/2.. - dsi */
	counter = 0;
	para->reg_base[DISP_MOD_DE] = disp_getprop_regbase(FDT_DISP_PATH, "reg", counter);
	if (!para->reg_base[DISP_MOD_DE]) {
		__wrn("unable to map de registers\n");
		ret = -EINVAL;
		goto exit;
	}
	counter ++;

#if defined(CONFIG_INDEPENDENT_DE)
	para->reg_base[DISP_MOD_DE1] = disp_getprop_regbase(FDT_DISP_PATH, "reg", counter);
	if (!para->reg_base[DISP_MOD_DE1]) {
		__wrn("unable to map de registers\n");
		ret = -EINVAL;
		goto exit;
	}
	counter++;
#endif

#if defined(HAVE_DEVICE_COMMON_MODULE)
	para->reg_base[DISP_MOD_DEVICE] = disp_getprop_regbase(FDT_DISP_PATH, "reg", counter);
	if (!para->reg_base[DISP_MOD_DEVICE]) {
		__wrn("unable to map device common module registers\n");
		ret = -EINVAL;
		goto exit;
	}
	counter ++;
#if defined(CONFIG_INDEPENDENT_DE)
	para->reg_base[DISP_MOD_DEVICE1] = disp_getprop_regbase(FDT_DISP_PATH, "reg", counter);
	if (!para->reg_base[DISP_MOD_DEVICE1]) {
		__wrn("unable to map device common module registers\n");
		ret = -EINVAL;
		goto exit;
	}
	counter++;
#endif
#endif

	for (i=0; i<DISP_DEVICE_NUM; i++) {
		para->reg_base[DISP_MOD_LCD0 + i] = disp_getprop_regbase(FDT_DISP_PATH, "reg", counter);
		if (!para->reg_base[DISP_MOD_LCD0 + i]) {
			__wrn("unable to map timing controller %d registers\n", i);
			ret = -EINVAL;
			goto exit;
		}
		counter ++;
	}

#if defined(SUPPORT_DSI)
	for (i = 0; i < DEVICE_DSI_NUM; ++i) {
		para->reg_base[DISP_MOD_DSI0 + i] =
		    disp_getprop_regbase(FDT_DISP_PATH, "reg", counter);
		if (!para->reg_base[DISP_MOD_DSI0 + i]) {
			__wrn("unable to map dsi%d registers\n", i);
			ret = -EINVAL;
			goto exit;
		}
		++counter;
	}
#endif

#if defined(SUPPORT_EINK)
		para->reg_base[DISP_MOD_EINK] = disp_getprop_regbase(FDT_DISP_PATH, "reg", counter);
		if (!para->reg_base[DISP_MOD_EINK]) {
			__wrn("unable to map eink registers\n");
			ret = -EINVAL;
			goto exit;
		}
		counter++;
#endif

	/* parse and map irq */
	/* lcd0/1/2.. - dsi */
	counter = 0;


#ifdef DE_VERSION_V33X
	para->irq_no[DISP_MOD_DE] =
	    disp_getprop_irq(FDT_DISP_PATH, "interrupts", counter);
	if (!para->irq_no[DISP_MOD_DE]) {
		__wrn("irq_of_parse_and_map de irq fail\n");
	}
	++counter;
#endif

	for (i=0; i<DISP_DEVICE_NUM; i++) {
		para->irq_no[DISP_MOD_LCD0 + i] = disp_getprop_irq(FDT_DISP_PATH, "interrupts", counter);
		if (!para->irq_no[DISP_MOD_LCD0 + i]) {
			__wrn("irq_of_parse_and_map irq %d fail for lcd%d\n", counter, i);
		}
		counter ++;
	}

#if defined(SUPPORT_DSI)
	for (i = 0; i < DEVICE_DSI_NUM; ++i) {
		para->irq_no[DISP_MOD_DSI0 + i] =
		    disp_getprop_irq(FDT_DISP_PATH, "interrupts", counter);
		if (!para->irq_no[DISP_MOD_DSI0 + i])
			__wrn("irq_of_parse_and_map irq %d fail for dsi\n", i);
		counter++;
	}
#endif

#if defined(SUPPORT_EINK)
		para->irq_no[DISP_MOD_DE] = disp_getprop_irq(FDT_DISP_PATH, "interrupts", counter);
		if (!para->irq_no[DISP_MOD_DE]) {
			__wrn("irq_of_parse_and_map de irq %d fail for dsi\n", counter);
		}
		counter++;


		para->irq_no[DISP_MOD_EINK] = disp_getprop_irq(FDT_DISP_PATH, "interrupts", counter);
		if (!para->irq_no[DISP_MOD_EINK]) {
			__wrn("irq_of_parse_and_map irq %d fail for eink\n", counter);
		}
		counter++;
#endif

	node_offset = disp_fdt_nodeoffset("disp");
	of_periph_clk_config_setup(node_offset);

	/* get clk via device-tree-supported interface */
	/* de - [device(tcon-top)] - lcd0/1/2.. - lvds - dsi */
	counter = 0;
	para->mclk[DISP_MOD_DE] = of_clk_get(node_offset, counter);
	if (IS_ERR(para->mclk[DISP_MOD_DE])) {
		printf("fail to get clk for de\n");
	}
	counter ++;

#if defined(CONFIG_INDEPENDENT_DE)
	para->mclk[DISP_MOD_DE1] = of_clk_get(node_offset, counter);
	if (IS_ERR(para->mclk[DISP_MOD_DE1])) {
		printf("fail to get clk for de\n");
	}
	counter++;
#endif

#if defined(HAVE_DEVICE_COMMON_MODULE)
	para->mclk[DISP_MOD_DEVICE] = of_clk_get(node_offset, counter);
	if (IS_ERR(para->mclk[DISP_MOD_DEVICE])) {
		printf("fail to get clk for device common module\n");
	}
	counter ++;
#endif

#if defined(CONFIG_INDEPENDENT_DE)
	for (i = 0; i < DISP_DEVICE_NUM; i++) {
		para->mclk[DISP_MOD_DPSS0 + i] = of_clk_get(node_offset, counter);
		if (IS_ERR(para->mclk[DISP_MOD_DPSS0 + i])) {
			printf("fail to get clk for DPSS%d\n", i);
		}
		counter++;
	}
#endif
	for (i=0; i<DISP_DEVICE_NUM; i++) {
		para->mclk[DISP_MOD_LCD0 + i] = of_clk_get(node_offset, counter);
		if (IS_ERR(para->mclk[DISP_MOD_LCD0 + i])) {
			printf("fail to get clk for timing controller%d\n", i);
		}
		counter ++;
	}

#if defined(SUPPORT_LVDS)
	for (i = 0; i < DEVICE_LVDS_NUM; i++) {
		para->mclk[DISP_MOD_LVDS + i] = of_clk_get(node_offset, counter);
		if (IS_ERR(para->mclk[DISP_MOD_LVDS + i])) {
			printf("fail to get clk for lvds:%d\n", i);
		}
		counter++;
	}
#endif

#if defined(SUPPORT_DSI)
	for (i = 0; i < CLK_DSI_NUM; ++i) {
		para->mclk[DISP_MOD_DSI0 + i] =
		    of_clk_get(node_offset, counter);
		if (IS_ERR(para->mclk[DISP_MOD_DSI0 + i]))
			__wrn("fail to get clk %d for dsi\n", i);
		counter++;
	}
#endif

#if defined(SUPPORT_EINK)
	para->mclk[DISP_MOD_EINK] = of_clk_get(node_offset, counter);
	if (IS_ERR(para->mclk[DISP_MOD_EINK])) {
		printf("fail to get clk for eink\n");
	}
	counter ++;

	para->mclk[DISP_MOD_EDMA] = of_clk_get(node_offset, counter);
	if (IS_ERR(para->mclk[DISP_MOD_EDMA])) {
		printf("fail to get clk for edma\n");
	}
	counter ++;
#endif

#if defined(HAVE_DEVICE_COMMON_MODULE)
	struct clk* pll;
	pll = clk_get(NULL, "tcon_top");
			clk_prepare_enable(pll);
			clk_put(pll);
#endif

#if defined(CONFIG_MACH_SUN8IW10)
	{
		struct clk* pll;

		pll = clk_get(NULL, "pll_video0");
		if (pll) {
			ret = clk_set_parent(para->mclk[DISP_MOD_LCD0], pll);
			if (0 != ret) {
				printf("clk_set parent tcon0 fail\n");
			}
		} else
			printf("clk(pll_video0) get fail\n");

		ret = clk_set_parent(para->mclk[DISP_MOD_LCD1], pll);
		if (0 != ret) {
			printf("clk_set parent tcon1 fail\n");
		}

		pll = clk_get(NULL, "pll_de");
		if (pll) {
			ret = clk_set_parent(para->mclk[DISP_MOD_DE], pll);
			if (0 != ret) {
				printf("clk_set parent de fail\n");
			}
		} else
			printf("clk(pll_de) get fail\n");
		clk_put(pll);
	}
#endif

	ret =
	    disp_sys_script_get_item(FDT_DISP_PATH, "chn_cfg_mode", &value, 1);
	g_disp_drv.disp_init.chn_cfg_mode = (ret != 1) ? 0 : value;

	para->feat_init.chn_cfg_mode = g_disp_drv.disp_init.chn_cfg_mode;
	ret = bsp_disp_init(para);
	num_screens = bsp_disp_feat_get_num_screens();
	for (disp=0; disp<num_screens; disp++) {
		g_disp_drv.mgr[disp] = disp_get_layer_manager(disp);
	}
#if defined(SUPPORT_EINK)
		g_disp_drv.eink_manager[0] = disp_get_eink_manager(0);

#endif
	lcd_init();
#if defined(SUPPORT_HDMI) && (defined(CONFIG_HDMI_DISP2_SUNXI) || \
				defined(CONFIG_HDMI2_DISP2_SUNXI))
	hdmi_init();
#endif
#if defined(SUPPORT_TV) && defined(CONFIG_TV_DISP2_SUNXI)
	tv_init();
#endif

#if defined(SUPPORT_EDP) && defined(CONFIG_EDP_DISP2_SUNXI)
	edp_init();
#endif

#if defined(CONFIG_DISP2_TV_AC200)
	tv_ac200_init();
#endif
	bsp_disp_open();

	g_disp_drv.inited = true;

	tick_printf("%s finish\n", __func__);

exit:
	return ret;
}

s32 drv_disp_exit(void)
{
	printf("%s\n", __func__);
	if (g_disp_drv.inited == true) {
		g_disp_drv.inited = false;
		bsp_disp_close();
		bsp_disp_exit(g_disp_drv.exit_mode);
	}

	return 0;
}

int sunxi_disp_get_source_ops(struct sunxi_disp_source_ops *src_ops)
{
	memset((void *)src_ops, 0, sizeof(struct sunxi_disp_source_ops));

	src_ops->sunxi_lcd_set_panel_funs = bsp_disp_lcd_set_panel_funs;
	src_ops->sunxi_lcd_delay_ms = disp_delay_ms;
	src_ops->sunxi_lcd_delay_us = disp_delay_us;
	src_ops->sunxi_lcd_backlight_enable = bsp_disp_lcd_backlight_enable;
	src_ops->sunxi_lcd_backlight_disable = bsp_disp_lcd_backlight_disable;
	src_ops->sunxi_lcd_pwm_enable = bsp_disp_lcd_pwm_enable;
	src_ops->sunxi_lcd_pwm_disable = bsp_disp_lcd_pwm_disable;
	src_ops->sunxi_lcd_power_enable = bsp_disp_lcd_power_enable;
	src_ops->sunxi_lcd_power_disable = bsp_disp_lcd_power_disable;
	src_ops->sunxi_lcd_tcon_enable = bsp_disp_lcd_tcon_enable;
	src_ops->sunxi_lcd_tcon_disable = bsp_disp_lcd_tcon_disable;
	src_ops->sunxi_lcd_pin_cfg = bsp_disp_lcd_pin_cfg;
	src_ops->sunxi_lcd_gpio_set_value = bsp_disp_lcd_gpio_set_value;
	src_ops->sunxi_lcd_gpio_set_direction = bsp_disp_lcd_gpio_set_direction;
#ifdef SUPPORT_DSI
	src_ops->sunxi_lcd_dsi_dcs_write = bsp_disp_lcd_dsi_dcs_wr;
	src_ops->sunxi_lcd_dsi_gen_write = bsp_disp_lcd_dsi_gen_wr;
	src_ops->sunxi_lcd_dsi_clk_enable = bsp_disp_lcd_dsi_clk_enable;
	src_ops->sunxi_lcd_dsi_mode_switch = bsp_disp_lcd_dsi_mode_switch;
	src_ops->sunxi_lcd_dsi_gen_short_read = bsp_disp_lcd_dsi_gen_short_read;
	src_ops->sunxi_lcd_dsi_dcs_read = bsp_disp_lcd_dsi_dcs_read;
	src_ops->sunxi_lcd_dsi_set_max_ret_size = bsp_disp_lcd_set_max_ret_size;
#endif
	src_ops->sunxi_lcd_cpu_write = tcon0_cpu_wr_16b;
	src_ops->sunxi_lcd_cpu_write_data = tcon0_cpu_wr_16b_data;
	src_ops->sunxi_lcd_cpu_write_index = tcon0_cpu_wr_16b_index;
	src_ops->sunxi_lcd_cpu_set_auto_mode = tcon0_cpu_set_auto_mode;
	src_ops->sunxi_lcd_switch_compat_panel = bsp_disp_lcd_switch_compat_panel;
	return 0;
}

static int disp_blank(bool blank)
{
	u32 screen_id = 0;
	int num_screens;
	struct disp_manager *mgr = NULL;

	num_screens = bsp_disp_feat_get_num_screens();

	for (screen_id=0; screen_id<num_screens; screen_id++) {
		mgr = g_disp_drv.mgr[screen_id];
		if (!mgr || !mgr->device)
			continue;

		if (mgr->blank)
			mgr->blank(mgr, blank);
	}

	return 0;
}

struct eink_8bpp_image *cimage;

long disp_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	unsigned long karg[4];
	unsigned long ubuffer[4] = {0};
	s32 ret = 0;
	int num_screens = 2;
	struct disp_manager *mgr = NULL;
	struct disp_device *dispdev = NULL;
	struct disp_enhance *enhance = NULL;
	struct disp_smbl *smbl = NULL;
	struct disp_capture *cptr = NULL;
#if defined (SUPPORT_EINK)
	struct disp_eink_manager *eink_manager = NULL;
#endif

	if (false == g_disp_drv.inited) {
		printf("%s, display not init yet\n", __func__);
		return -1;
	}
	num_screens = bsp_disp_feat_get_num_screens();

	if (copy_from_user((void*)karg,(void __user*)arg,4*sizeof(unsigned long))) {
		__wrn("copy_from_user fail\n");
		return -EFAULT;
	}

	ubuffer[0] = *(unsigned long*)karg;
	ubuffer[1] = (*(unsigned long*)(karg+1));
	ubuffer[2] = (*(unsigned long*)(karg+2));
	ubuffer[3] = (*(unsigned long*)(karg+3));

	if (ubuffer[0] < num_screens)
		mgr = g_disp_drv.mgr[ubuffer[0]];
	if (mgr) {
		dispdev = mgr->device;
		enhance = mgr->enhance;
		smbl = mgr->smbl;
		cptr = mgr->cptr;
	}
#if defined (SUPPORT_EINK)
	eink_manager = g_disp_drv.eink_manager[0];

	if (!eink_manager)
		__wrn("eink_manager is NULL!\n");

#endif

	if (cmd < DISP_FB_REQUEST)	{
		if (ubuffer[0] >= num_screens) {
			__wrn("para err in disp_ioctl, cmd = 0x%x,screen id = %d\n", cmd, (int)ubuffer[0]);
			return -1;
		}
	}
	if (DISPLAY_DEEP_SLEEP & suspend_status) {
		__wrn("ioctl:%x fail when in suspend!\n", cmd);
		return -1;
	}
	if (cmd == DISP_print) {
		__wrn("cmd:0x%x,%ld,%ld\n",cmd, ubuffer[0], ubuffer[1]);
	}

	switch(cmd)	{
	//----disp global----
	case DISP_SET_BKCOLOR:
	{
		struct disp_color para;

		if (copy_from_user(&para, (void __user *)ubuffer[1],sizeof(struct disp_color)))	{
			__wrn("copy_from_user fail\n");
			return  -EFAULT;
		}
		if (mgr && (mgr->set_back_color != NULL))
			ret = mgr->set_back_color(mgr, &para);
		break;
	}

	case DISP_GET_OUTPUT_TYPE:
	{
		if (suspend_status != DISPLAY_NORMAL)
			ret = suspend_output_type[ubuffer[0]];
		else
			ret = bsp_disp_get_output_type(ubuffer[0]);
		break;
	}

	case DISP_GET_SCN_WIDTH:
	{
		unsigned int width = 0,height = 0;
		if (mgr && mgr->device && mgr->device->get_resolution)
			mgr->device->get_resolution(mgr->device, &width, &height);
		ret = width;
		break;
	}

	case DISP_GET_SCN_HEIGHT:
	{
		unsigned int width = 0,height = 0;
		if (mgr && mgr->device && mgr->device->get_resolution)
			mgr->device->get_resolution(mgr->device, &width, &height);
		ret = height;
		break;
	}

	case DISP_VSYNC_EVENT_EN:
	{
		ret = bsp_disp_vsync_event_enable(ubuffer[0], ubuffer[1]);
		break;
	}

	case DISP_SHADOW_PROTECT:
	{
		ret = bsp_disp_shadow_protect(ubuffer[0], ubuffer[1]);
		break;
	}

	case DISP_BLANK:
	{
		/* only response main device' blank request */
		if (0 != ubuffer[0])
			break;

		if (ubuffer[1]) {
#if defined(CONFIG_PM_RUNTIME)
			if (g_disp_drv.dev)
				pm_runtime_put(g_disp_drv.dev);
			else
				pr_warn("%s, display device is null\n", __func__);
#endif
			disp_blank(true);
		} else {
			if (power_status_init) {
				/* avoid first unblank, because device is ready when driver init */
				power_status_init = 0;
				break;
			}

			disp_blank(false);
#if defined(CONFIG_PM_RUNTIME)
			if (g_disp_drv.dev) {
				/* recover the pm_runtime status */
				pm_runtime_disable(g_disp_drv.dev);
				pm_runtime_set_suspended(g_disp_drv.dev);
				pm_runtime_enable(g_disp_drv.dev);
				pm_runtime_get_sync(g_disp_drv.dev);
			}
			else
				pr_warn("%s, display device is null\n", __func__);
#endif
		}
		break;
	}

	case DISP_DEVICE_SWITCH:
	{
		if (ubuffer[1] == (unsigned long)DISP_OUTPUT_TYPE_LCD)
			ret = drv_lcd_enable(ubuffer[0]);
		else
			ret = bsp_disp_device_switch(ubuffer[0], (enum disp_output_type)ubuffer[1], (enum disp_tv_mode)ubuffer[2]);
		suspend_output_type[ubuffer[0]] = ubuffer[1];
		break;
	}

	case DISP_DEVICE_SET_CONFIG:
	{
		struct disp_device_config config;

		if (copy_from_user(&config, (void __user *)ubuffer[1],
			sizeof(struct disp_device_config))) {
			__wrn("copy_from_user fail\n");
			return  -EFAULT;
		}

		ret = bsp_disp_device_set_config(ubuffer[0], &config);
		break;
	}

	case DISP_DEVICE_GET_CONFIG:
	{
		struct disp_device_config config;

		if (mgr && dispdev)
			dispdev->get_static_config(dispdev, &config);
		else
			ret = -EFAULT;

		if (ret == 0) {
			if (copy_to_user((void __user *)ubuffer[1], &config,
				sizeof(struct disp_device_config))) {
				__wrn("copy_to_user fail\n");
				return  -EFAULT;
			}
		}
		break;
	}

#if defined(SUPPORT_EINK) && defined(CONFIG_EINK_PANEL_USED)

	/*---eink ----*/
	case DISP_EINK_UPDATE:
	{
		struct eink_8bpp_image current_image;
		if (!eink_manager) {
			printf("there is no eink manager!\n");
			break;
		}

		memset(&current_image, 0, sizeof(struct eink_8bpp_image));
		if (copy_from_user(&current_image, (void __user *)ubuffer[0], sizeof(struct eink_8bpp_image))) {
			__wrn("copy_from_user fail\n");
			return  -EFAULT;
		}

		ret = bsp_disp_eink_update(eink_manager, &current_image);
		if (ret != 0)
			__wrn("eink update wrong!\n");
		break;
	}

#endif

	case DISP_GET_OUTPUT:
	{
		struct disp_output para;

		memset(&para, 0, sizeof(struct disp_output));

		if (mgr && mgr->device) {
			para.type = bsp_disp_get_output_type(ubuffer[0]);
			if (mgr->device->get_mode)
				para.mode = mgr->device->get_mode(mgr->device);
		}

		if (copy_to_user((void __user *)ubuffer[1],&para, sizeof(struct disp_output))) {
			__wrn("copy_from_user fail\n");
			return  -EFAULT;
		}
		break;
	}

	case DISP_SET_COLOR_RANGE:
	{
		if (mgr && mgr->set_output_color_range) {
			ret = mgr->set_output_color_range(mgr, ubuffer[1]);
		}

		break;
	}

	case DISP_GET_COLOR_RANGE:
	{
		if (mgr && mgr->get_output_color_range) {
			ret = mgr->get_output_color_range(mgr);
		}

		break;
	}

	//----layer----
	case DISP_LAYER_SET_CONFIG:
	{
		if (copy_from_user(lyr_cfg,
			(void __user *)ubuffer[1],
			sizeof(struct disp_layer_config) * ubuffer[2]))	{
			__wrn("copy_from_user fail\n");
			return  -EFAULT;
		}
		if (mgr && mgr->set_layer_config)
			ret = mgr->set_layer_config(mgr, lyr_cfg, ubuffer[2]);
		break;
	}

	case DISP_LAYER_GET_CONFIG:
	{
		if (copy_from_user(lyr_cfg,
			(void __user *)ubuffer[1],
			sizeof(struct disp_layer_config) * ubuffer[2]))	{
			__wrn("copy_from_user fail\n");

			return  -EFAULT;
		}
		if (mgr && mgr->get_layer_config)
			ret = mgr->get_layer_config(mgr, lyr_cfg, ubuffer[2]);
		if (copy_to_user((void __user *)ubuffer[1],
			lyr_cfg,
			sizeof(struct disp_layer_config) * ubuffer[2]))	{
			__wrn("copy_to_user fail\n");

			return  -EFAULT;
		}
		break;
	}

	case DISP_LAYER_SET_CONFIG2:
	{
		if (copy_from_user(lyr_cfg2,
		    (void __user *)ubuffer[1],
		    sizeof(struct disp_layer_config2) * ubuffer[2])) {
			__wrn("copy_from_user fail\n");

			return  -EFAULT;
		}
		if (mgr && mgr->set_layer_config2)
			ret = mgr->set_layer_config2(mgr, lyr_cfg2, ubuffer[2]);
		break;
	}

	case DISP_LAYER_GET_CONFIG2:
	{
		if (copy_from_user(lyr_cfg2,
		    (void __user *)ubuffer[1],
		    sizeof(struct disp_layer_config2) * ubuffer[2])) {
			__wrn("copy_from_user fail\n");

			return  -EFAULT;
		}
		if (mgr && mgr->get_layer_config2)
			ret = mgr->get_layer_config2(mgr, lyr_cfg2, ubuffer[2]);
		if (copy_to_user((void __user *)ubuffer[1],
			lyr_cfg2,
			sizeof(struct disp_layer_config2) * ubuffer[2])) {
			__wrn("copy_to_user fail\n");

			return  -EFAULT;
		}
		break;
	}

	//---- lcd ---
	case DISP_LCD_SET_BRIGHTNESS:
	{
		if (dispdev && dispdev->set_bright) {
			ret = dispdev->set_bright(dispdev, ubuffer[1]);
		}
		break;
	}

	case DISP_LCD_GET_BRIGHTNESS:
	{
		if (dispdev && dispdev->get_bright) {
			ret = dispdev->get_bright(dispdev);
		}
		break;
	}

	case DISP_LCD_GAMMA_CORRECTION_ENABLE:
	{
		if (dispdev && (DISP_OUTPUT_TYPE_LCD == dispdev->type))
			ret = dispdev->enable_gamma(dispdev);
		break;
	}

	case DISP_LCD_GAMMA_CORRECTION_DISABLE:
	{
		if (dispdev && (DISP_OUTPUT_TYPE_LCD == dispdev->type))
			ret = dispdev->disable_gamma(dispdev);

		break;
	}

	case DISP_LCD_SET_GAMMA_TABLE:
	{
		if (dispdev && (DISP_OUTPUT_TYPE_LCD == dispdev->type)) {
			u32 *gamma_tbl = kmalloc(LCD_GAMMA_TABLE_SIZE,
						 GFP_KERNEL | __GFP_ZERO);
			u32 size = ubuffer[2];

			if (gamma_tbl == NULL) {
				__wrn("kmalloc fail\n");
				ret = -EFAULT;
				break;
			}

			size = (size > LCD_GAMMA_TABLE_SIZE) ?
			    LCD_GAMMA_TABLE_SIZE : size;
			if (copy_from_user(gamma_tbl, (void __user *)ubuffer[1],
					  size)) {
				__wrn("copy_from_user fail\n");
				kfree(gamma_tbl);
				ret = -EFAULT;

				break;
			}
			ret = dispdev->set_gamma_tbl(dispdev, gamma_tbl, size);
			kfree(gamma_tbl);
		}
		break;
	}

	//---- hdmi ---
	case DISP_HDMI_GET_HPD_STATUS:
	if (DISPLAY_NORMAL == suspend_status)
		ret = bsp_disp_hdmi_get_hpd_status(ubuffer[0]);
	else
		ret = 0;

	break;
	case DISP_HDMI_SUPPORT_MODE:
	{
		ret = bsp_disp_hdmi_check_support_mode(ubuffer[0], ubuffer[1]);
		break;
	}

	case DISP_HDMI_GET_SUPPORT_MODE:
	{
		ret = bsp_disp_hdmi_get_support_mode(ubuffer[0], ubuffer[1]);
		if (ret <= 0)
			ret = ubuffer[1];
		break;
	}

	case DISP_HDMI_GET_WORK_MODE:
	{
		ret = bsp_disp_hdmi_get_work_mode(ubuffer[0]);
		break;
	}

	//---- tv ---
	case DISP_SET_TV_HPD:
	{
		ret = bsp_disp_tv_set_hpd(ubuffer[0]);
		break;
	}
#if defined (SUPPORT_TV)
	case DISP_TV_GET_HPD_STATUS:

	if (DISPLAY_NORMAL == suspend_status)
		ret = bsp_disp_tv_get_hpd_status(ubuffer[0]);
	else
		ret = 0;

	break;
#endif

	//----enhance----
	case DISP_ENHANCE_ENABLE:
	{
		if (enhance && enhance->enable)
			ret = enhance->enable(enhance);
		break;
	}

	case DISP_ENHANCE_DISABLE:
	{
		if (enhance && enhance->disable)
			ret = enhance->disable(enhance);
		break;
	}

	case DISP_ENHANCE_DEMO_ENABLE:
	{
		if (enhance && enhance->demo_enable)
			ret = enhance->demo_enable(enhance);
		break;
	}

	case DISP_ENHANCE_DEMO_DISABLE:
	{
		if (enhance && enhance->demo_disable)
			ret = enhance->demo_disable(enhance);
		break;
	}

	case DISP_ENHANCE_SET_MODE:
	{
		if (enhance && enhance->set_mode)
			ret = enhance->set_mode(enhance, ubuffer[1]);
		break;
	}

	case DISP_ENHANCE_GET_MODE:
	{
		if (enhance && enhance->get_mode)
			ret = enhance->get_mode(enhance);
		break;
	}

	//---smart backlight --
	case DISP_SMBL_ENABLE:
	{
		if (smbl && smbl->enable)
			ret = smbl->enable(smbl);
		break;
	}

	case DISP_SMBL_DISABLE:
	{
		if (smbl && smbl->disable)
			ret = smbl->disable(smbl);
		break;
	}

	case DISP_SMBL_SET_WINDOW:
	{
		struct disp_rect rect;

		if (copy_from_user(&rect, (void __user *)ubuffer[1],sizeof(struct disp_rect)))	{
			__wrn("copy_from_user fail\n");
			return  -EFAULT;
		}
		if (smbl && smbl->set_window)
			ret = smbl->set_window(smbl, &rect);
		break;
	}

	//---capture --
	case DISP_CAPTURE_START:
	{
		if (cptr && cptr->start)
			ret = cptr->start(cptr);
		break;
	}

	case DISP_CAPTURE_STOP:
	{
		if (cptr && cptr->stop)
			ret = cptr->stop(cptr);
		break;
	}

	case DISP_CAPTURE_COMMIT:
	{
		struct disp_capture_info info;

		if (copy_from_user(&info, (void __user *)ubuffer[1],sizeof(struct disp_capture_info)))	{
			__wrn("copy_from_user fail\n");
			return  -EFAULT;
		}
		if (cptr && cptr->commmit)
			ret = cptr->commmit(cptr, &info);
		break;
	}

#if defined(__LINUX_PLAT__)
	//----for test----
	case DISP_MEM_REQUEST:
		ret =  disp_mem_request(ubuffer[0],ubuffer[1]);
		break;

	case DISP_MEM_RELEASE:
		ret =  disp_mem_release(ubuffer[0]);
		break;

	case DISP_MEM_GETADR:
		return g_disp_mm[ubuffer[0]].mem_start;
#endif

	case DISP_LCD_BACKLIGHT_ENABLE:
		{
			if (mgr && mgr->device) {
				if (mgr->device->pwm_enable) {
					mgr->device->pwm_enable(mgr->device);
				}
				if (mgr->device->backlight_enable) {
					mgr->device->backlight_enable(mgr->device);
				}

				return 0;
			}
			return -1;
			break;
		}
	case DISP_LCD_BACKLIGHT_DISABLE:
		{
			if (mgr && mgr->device) {
				if (mgr->device->pwm_disable)
					mgr->device->pwm_disable(mgr->device);
				if (mgr->device->backlight_disable)
					mgr->device->backlight_disable(mgr->device);
				return 0;
			}
			return -1;
			break;
		}

	case DISP_SET_KSC_PARA:
		{

			struct disp_ksc_info ksc;

			if (copy_from_user(&ksc, (void __user *)ubuffer[1],
					   sizeof(struct disp_ksc_info))) {
				__wrn("copy_from_user fail\n");
				return  -EFAULT;
			}
			if (mgr && mgr->set_ksc_para)
				ret = mgr->set_ksc_para(mgr, &ksc);

			break;
		}

	case DISP_SET_EXIT_MODE:
		ret = g_disp_drv.exit_mode = ubuffer[1];
		break;

	case DISP_LCD_CHECK_OPEN_FINISH:
		ret = drv_lcd_check_open_finished(ubuffer[0]);
		break;

	case DISP_LCD_CHECK_CLOSE_FINISH:
		ret = drv_lcd_check_close_finished(ubuffer[0]);
		break;
	default:
		break;
	}

  return ret;
}

int do_sunxi_disp_sys(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
#define SYS_BUF_SIZE 5120
	struct disp_manager *mgr = NULL;
	struct disp_device *dispdev = NULL;
	ssize_t count = 0;
	int num_screens, screen_id;
	int num_layers, layer_id;
	int num_chans, chan_id;
	char *buf = NULL;
#if defined(CONFIG_DISP2_LCD_ESD_DETECT)
	struct disp_lcd_esd_info esd_inf;

	memset(&esd_inf, 0, sizeof(struct disp_lcd_esd_info));
#endif
	/* int hpd; */

	buf = kmalloc(SYS_BUF_SIZE, GFP_KERNEL | __GFP_ZERO);
	num_screens = bsp_disp_feat_get_num_screens();
	for (screen_id = 0; screen_id < num_screens; screen_id++) {
		u32 width = 0, height = 0;
		int fps = 0;
		disp_health_info info;

		mgr = disp_get_layer_manager(screen_id);
		if (mgr == NULL)
			continue;
		dispdev = mgr->device;
		if (dispdev == NULL)
			continue;
		dispdev->get_resolution(dispdev, &width, &height);
		fps = bsp_disp_get_fps(screen_id);
		bsp_disp_get_health_info(screen_id, &info);

		if (!dispdev->is_enabled(dispdev))
			continue;
		count += sprintf(buf + count, "screen %d:\n", screen_id);
		count += sprintf(buf + count, "de_rate %d hz, ref_fps:%d\n",
				 mgr->get_clk_rate(mgr),
				 dispdev->get_fps(dispdev));
		count += mgr->dump(mgr, buf + count);
		/* output */
		if (dispdev->type == DISP_OUTPUT_TYPE_LCD) {
			count += sprintf(buf + count,
				"\tlcd output\tbacklight(%3d)\tfps:%d.%d",
				dispdev->get_bright(dispdev), fps / 10,
				fps % 10);
#if defined(CONFIG_DISP2_LCD_ESD_DETECT)
			if (dispdev->get_esd_info) {
				dispdev->get_esd_info(dispdev, &esd_inf);
				count += sprintf(buf + count,
				"\tesd level(%u)\tfreq(%u)\tpos(%u)\treset(%u)",
				esd_inf.level, esd_inf.freq,
				esd_inf.esd_check_func_pos, esd_inf.rst_cnt);
			}
#endif
		} else if (dispdev->type == DISP_OUTPUT_TYPE_HDMI) {
			int mode = dispdev->get_mode(dispdev);

			count += sprintf(buf + count,
					 "\thdmi output mode(%d)\tfps:%d.%d",
					 mode, fps / 10, fps % 10);
		} else if (dispdev->type == DISP_OUTPUT_TYPE_TV) {
			int mode = dispdev->get_mode(dispdev);

			count += sprintf(buf + count,
					 "\ttv output mode(%d)\tfps:%d.%d",
					 mode, fps / 10, fps % 10);
		}
/* else if (dispdev->type == DISP_OUTPUT_TYPE_VGA) {
			int mode = dispdev->get_mode(dispdev);

			count += sprintf(buf + count,
					 "\tvga output mode(%d)\tfps:%d.%d",
					 mode, fps / 10, fps % 10);
		}  else if (dispdev->type == DISP_OUTPUT_TYPE_VDPO) {
			int mode = dispdev->get_mode(dispdev);

			count += sprintf(buf + count,
					 "\tvdpo output mode(%d)\tfps:%d.%d",
					 mode, fps / 10, fps % 10);
		}  else if (dispdev->type == DISP_OUTPUT_TYPE_RTWB) {
			int mode = dispdev->get_mode(dispdev);

			count += sprintf(buf + count,
					 "\trtwb output mode(%d)\tfps:%d.%d",
					 mode, fps / 10, fps % 10);
		} else if (dispdev->type == DISP_OUTPUT_TYPE_EDP) {
			count += sprintf(
			    buf + count, "\tEDP output(%s) \tfps:%d.%d",
			    (dispdev->is_enabled(dispdev) == 1) ? "enable"
								: "disable",
			    fps / 10, fps % 10);
		}
*/
		if (dispdev->type != DISP_OUTPUT_TYPE_NONE) {
			count += sprintf(buf + count, "\t%4ux%4u\n",
					 width, height);
			count += sprintf(buf + count,
					"\terr:%u\tskip:%u\tirq:%u\tvsync:%u\tvsync_skip:%u\t\n",
					info.error_cnt, info.skip_cnt,
					info.irq_cnt, info.vsync_cnt,
					info.vsync_skip_cnt);
		}

		printf("%s\n", buf);
		memset(buf, 0, SYS_BUF_SIZE);
		count = 0;
		num_chans = bsp_disp_feat_get_num_channels(screen_id);

		/* layer info */
		for (chan_id = 0; chan_id < num_chans; chan_id++) {
			num_layers =
			    bsp_disp_feat_get_num_layers_by_chn(screen_id,
								chan_id);
			for (layer_id = 0; layer_id < num_layers; layer_id++) {
				struct disp_layer *lyr = NULL;
				struct disp_layer_config config;

				lyr = disp_get_layer(screen_id, chan_id,
						     layer_id);
				config.channel = chan_id;
				config.layer_id = layer_id;
				mgr->get_layer_config(mgr, &config, 1);
				if (lyr && (true == config.enable) && lyr->dump) {
					count += lyr->dump(lyr, buf + count);
					printf("%s\n", buf);
					memset(buf, 0, SYS_BUF_SIZE);
					count = 0;
				}
			}
		}
	}
	kfree(buf);
	return 0;
}

U_BOOT_CMD(
	disp,	3,	0,	do_sunxi_disp_sys,
	"show display status",
	"\nparameters  : NULL\n"
);
/*TODO:drv_disp_standby is useful??*/

int do_sunxi_disp_colorbar(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	int color_code = -1, screen_index = 0;
	unsigned int num_screens;
	struct disp_manager *mgr = NULL;

	if (argc < 2 || argc > 4)
		return cmd_usage(cmdtp);

	color_code = simple_strtoul(argv[1], NULL, 10);
	if (color_code < 0 || color_code > 8)
		return cmd_usage(cmdtp);

	if (argc == 3)
		screen_index = simple_strtoul(argv[2], NULL, 10);

	num_screens = bsp_disp_feat_get_num_screens();
	if (screen_index < num_screens)
		mgr = g_disp_drv.mgr[screen_index];
	else {
		printf("Screen index out of range:%d!\n", screen_index);
		return cmd_usage(cmdtp);
	}

	if (color_code == 8) {
		disp_draw_colorbar(screen_index);
		if (mgr && mgr->device && mgr->device->show_builtin_patten)
			mgr->device->show_builtin_patten(mgr->device, 0);
	} else {
		if (mgr && mgr->device && mgr->device->show_builtin_patten)
			mgr->device->show_builtin_patten(mgr->device, color_code);
	}
	return 0;
}

U_BOOT_CMD(
	colorbar,	3,	0,	do_sunxi_disp_colorbar,
	"show colorbar",
	"\nparameters 1 : colorbar code 0~8\n"
	"- 0:picture\n"
	"- 1~7:tcon or edp or other device's builtin patten\n"
	"- 8:de colorbar\n"
	"parameters 2[optional]: screen index.0~3\n"
);
