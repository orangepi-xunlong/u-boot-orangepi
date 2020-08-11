/* display driver
 *
 * Copyright (c) 2017 Allwinnertech Co., Ltd.
 * Author: Tyle <tyle@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include "dev_disp.h"


struct disp_drv_info g_disp_drv;

/* alloc based on 4K byte */
#define MY_BYTE_ALIGN(x) (((x + (4*1024-1)) >> 12) << 12)

static u32 suspend_output_type[3] = { 0, 0, 0 };

/* 0:normal; */
/* suspend_status&1 != 0:in early_suspend; */
/* suspend_status&2 != 0:in suspend; */
static u32 suspend_status;


//uboot plat
static u32    lcd_flow_cnt[2] = {0};
static s8   lcd_op_finished[2] = {0};
static struct timer_list lcd_timer[2];
static s8   lcd_op_start[2] = {0};

/* static u32 disp_print_cmd_level = 0; */
static u32 disp_cmd_print = 0xffff;	/* print cmd which eq disp_cmd_print */
static u32 g_output_type = DISP_OUTPUT_TYPE_LCD;

#if 0
static struct resource disp_resource[] = {

};
#endif

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

static s32 copy_from_user(void *dest, void* src, u32 size)
{
	memcpy(dest, src, size);
	return 0;
}

static s32 copy_to_user(void *src, void* dest, u32 size)
{
	memcpy(dest, src, size);
	return 0;
}

static void drv_lcd_open_callback(void *parg)
{
	disp_lcd_flow *flow = NULL;
	u32 sel = (u32)parg;
	s32 i = lcd_flow_cnt[sel]++;

	flow = bsp_disp_lcd_get_open_flow(sel);
	if (NULL == flow) {
		//printf("%s lcd open flow is NULL! LCD enable fail!\n", __func__);
		lcd_op_start[sel] = 0;
		lcd_op_finished[sel] = 1;
		del_timer(&lcd_timer[sel]);
		return ;
	}

	if (i < flow->func_num)
	{
		//printf("%s, step=%d, number=%d, todo\n", __func__, i, flow->func_num);
		flow->func[i].func(sel);
		//printf("%s, step=%d, number=%d, finish\n", __func__, i, flow->func_num);
		if (flow->func[i].delay == 0) {
			drv_lcd_open_callback((void*)sel);
		}	else {
			//printf("%s, setnext step timer, delay=%d\n", __func__, flow->func[i].delay);
			lcd_timer[sel].data = sel;
			lcd_timer[sel].expires = flow->func[i].delay;
			lcd_timer[sel].function = drv_lcd_open_callback;
			add_timer(&lcd_timer[sel]);
		}
	} else if (i >= flow->func_num) {
		bsp_disp_lcd_post_enable(sel);
		lcd_op_finished[sel] = 1;
	}
}


static s32 drv_lcd_enable(u32 sel)
{
	if(bsp_disp_lcd_is_used(sel)) {
		lcd_flow_cnt[sel] = 0;
		lcd_op_finished[sel] = 0;
		lcd_op_start[sel] = 1;

		init_timer(&lcd_timer[sel]);

		bsp_disp_lcd_pre_enable(sel);
		drv_lcd_open_callback((void*)sel);
	}

    return 0;
}

static s8 drv_lcd_check_open_finished(u32 sel)
{
	if(bsp_disp_lcd_is_used(sel) && (lcd_op_start[sel] == 1)) {
		if (lcd_op_finished[sel]) {
			del_timer(&lcd_timer[sel]);
			lcd_op_start[sel] = 0;
		}
		return lcd_op_finished[sel];
	}

	return 1;
}

static void drv_lcd_close_callback(void *parg)
{
	disp_lcd_flow *flow;
	u32 sel = (__u32)parg;
	s32 i = lcd_flow_cnt[sel]++;

	flow = bsp_disp_lcd_get_close_flow(sel);

	if (i < flow->func_num) {
		flow->func[i].func(sel);
		if (flow->func[i].delay == 0)
			drv_lcd_close_callback((void*)sel);
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

static s32 drv_lcd_disable(u32 sel)
{
	lcd_flow_cnt[sel] = 0;
	lcd_op_finished[sel] = 0;
	lcd_op_start[sel] = 1;

	init_timer(&lcd_timer[sel]);

	drv_lcd_close_callback((void*)sel);

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
s32 disp_set_hdmi_func(u32 screen_id, disp_hdmi_func *func)
{
	return bsp_disp_set_hdmi_func(screen_id, func);
}
EXPORT_SYMBOL(disp_set_hdmi_func);

s32 disp_set_hdmi_hpd(u32 hpd)
{
	/* bsp_disp_set_hdmi_hpd(hpd); */

	return 0;
}
EXPORT_SYMBOL(disp_set_hdmi_hpd);

#endif


extern int sunxi_board_shutdown(void);
static s32 drv_disp_check_spec(void)
{
	unsigned int lcd_used = 0;
	unsigned int lcd_x = 0, lcd_y = 0;
	int ret = 0;
	int value = 0;
	int limit_w = 0xffff, limit_h = 0xffff;

#if defined(CONFIG_ARCH_SUN8IW6)
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
			sunxi_board_shutdown();
		}

	}

	return 0;
}

s32 drv_disp_init(void)
{
	struct __disp_bsp_init_para *para;
	int i;
	int ret = 0;
	int counter = 0;
	int node_offset = 0;

	printf("%s\n", __func__);
	disp_fdt_init();

	clk_init();
	/* check if the resolution of lcd supported */
	drv_disp_check_spec();

	memset(&g_disp_drv, 0, sizeof(struct disp_drv_info));
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
	counter++;

#if defined(HAVE_DEVICE_COMMON_MODULE)
	para->reg_base[DISP_MOD_DEVICE] = disp_getprop_regbase(FDT_DISP_PATH, "reg", counter);
	if (!para->reg_base[DISP_MOD_DEVICE]) {
		__wrn("unable to map device common module registers\n");
		ret = -EINVAL;
		goto exit;
	}
	counter ++;
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
	para->reg_base[DISP_MOD_DSI0] = disp_getprop_regbase(FDT_DISP_PATH, "reg", counter);
	if (!para->reg_base[DISP_MOD_DSI0]) {
		__wrn("unable to map dsi registers\n");
		ret = -EINVAL;
		goto exit;
	}
	counter ++;
#endif

	/* parse and map irq */
	/* lcd0/1/2.. - dsi */
	counter = 0;
	for (i=0; i<DISP_DEVICE_NUM; i++) {
		para->irq_no[DISP_MOD_LCD0 + i] = disp_getprop_irq(FDT_DISP_PATH, "interrupts", counter);
		if (!para->irq_no[DISP_MOD_LCD0 + i])
			__wrn("irq_of_parse_and_map irq %d fail for lcd%d\n", counter, i);

		counter ++;
	}

#if defined(SUPPORT_DSI)
	para->irq_no[DISP_MOD_DSI0] = disp_getprop_irq(FDT_DISP_PATH, "interrupts", counter);
	if (!para->irq_no[DISP_MOD_DSI0])
		__wrn("irq_of_parse_and_map irq %d fail for dsi\n", counter);

	counter ++;
#endif

	para->irq_no[DISP_MOD_BE0] = disp_getprop_irq(FDT_DISP_PATH, "interrupts", counter);
	if (!para->irq_no[DISP_MOD_BE0])
		__wrn("irq_of_parse_and_map irq %d fail for be\n", counter);

	counter++;

	node_offset = disp_fdt_nodeoffset("disp");
	of_periph_clk_config_setup(node_offset);
	/* get clk */
	/* de - [device(tcon-top)] - lcd0/1/2.. - dsi - lvds - other */
	counter = 0;
	para->mclk[MOD_CLK_DEBE0] = of_clk_get(node_offset, counter);
	if (IS_ERR(para->mclk[MOD_CLK_DEBE0]))
		printf("fail to get clk for be\n");

	counter++;

	para->mclk[MOD_CLK_DEFE0] = of_clk_get(node_offset, counter);
	if (IS_ERR(para->mclk[MOD_CLK_DEFE0]))
		printf("fail to get clk for fe\n");

	counter++;

#if defined(HAVE_DEVICE_COMMON_MODULE)
	para->mclk[DISP_MOD_DEVICE] = of_clk_get(node_offset, counter);
	if (IS_ERR(para->mclk[DISP_MOD_DEVICE]))
		printf("fail to get clk for device common module\n");

	counter++;
#endif

	for (i = 0; i < DISP_DEVICE_NUM; i++) {
		para->mclk[MOD_CLK_LCD0CH0 + i] = of_clk_get(node_offset, counter);
		if (IS_ERR(para->mclk[MOD_CLK_LCD0CH0 + i]))
		printf("fail to get clk for timing controller%d\n", i);

		counter++;
	}

#if defined(SUPPORT_DSI)
	para->mclk[MOD_CLK_MIPIDSIS] = of_clk_get(node_offset, counter);
	if (IS_ERR(para->mclk[MOD_CLK_MIPIDSIS]))
		printf("fail to get clk for dsi\n");

	counter++;

	para->mclk[MOD_CLK_MIPIDSIP] = of_clk_get(node_offset, counter);
	if (IS_ERR(para->mclk[MOD_CLK_MIPIDSIP]))
		printf("fail to get clk for dsi phy\n");

	counter++;
#endif

#if defined(SUPPORT_LVDS)
	para->mclk[MOD_CLK_LVDS] = of_clk_get(node_offset, counter);
	if (IS_ERR(para->mclk[MOD_CLK_LVDS]))
		printf("fail to get clk for lvds\n");

	counter++;
#endif

	para->mclk[MOD_CLK_IEPDRC0] = of_clk_get(node_offset, counter);
	if (IS_ERR(para->mclk[MOD_CLK_IEPDRC0]))
		printf("fail to get clk for drc0\n");

	counter++;

	para->mclk[MOD_CLK_SAT0] = of_clk_get(node_offset, counter);
	if (IS_ERR(para->mclk[MOD_CLK_SAT0]))
		printf("fail to get clk for sat0\n");
	counter++;




	bsp_disp_init(para);
	lcd_init();
	bsp_disp_open();

	g_disp_drv.inited = true;

	__inf("DRV_DISP_Init end\n");

exit:
	return ret;
}
EXPORT_SYMBOL(DRV_DISP_Init);

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
	src_ops->sunxi_lcd_delay_ms = bsp_disp_lcd_delay_ms;
	src_ops->sunxi_lcd_delay_us = bsp_disp_lcd_delay_us;
	src_ops->sunxi_lcd_tcon_enable = bsp_disp_lcd_tcon_enable;
	src_ops->sunxi_lcd_tcon_disable = bsp_disp_lcd_tcon_disable;
	src_ops->sunxi_lcd_pwm_enable = bsp_disp_lcd_pwm_enable;
	src_ops->sunxi_lcd_pwm_disable = bsp_disp_lcd_pwm_disable;
	src_ops->sunxi_lcd_backlight_enable = bsp_disp_lcd_backlight_enable;
	src_ops->sunxi_lcd_backlight_disable = bsp_disp_lcd_backlight_disable;
	src_ops->sunxi_lcd_power_enable = bsp_disp_lcd_power_enable;
	src_ops->sunxi_lcd_power_disable = bsp_disp_lcd_power_disable;
	src_ops->sunxi_lcd_set_panel_funs = bsp_disp_lcd_set_panel_funs;
	src_ops->sunxi_lcd_dsi_write = dsi_dcs_wr;
	src_ops->sunxi_lcd_dsi_clk_enable = dsi_clk_enable;
	src_ops->sunxi_lcd_pin_cfg = bsp_disp_lcd_pin_cfg;
	src_ops->sunxi_lcd_gpio_set_value = bsp_disp_lcd_gpio_set_value;
	src_ops->sunxi_lcd_gpio_set_direction = bsp_disp_lcd_gpio_set_direction;
	return 0;
}
EXPORT_SYMBOL(sunxi_disp_get_source_ops);





long disp_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	unsigned long karg[4];
	unsigned long ubuffer[4] = { 0 };
	s32 ret = 0;
	int num_screens = 2;

	if (false == g_disp_drv.inited) {
		printf("%s, display not init yet\n", __func__);
		return -1;
	}
	num_screens = bsp_disp_feat_get_num_screens();

	if (copy_from_user((void *)karg,
			   (void __user *)arg, 4 * sizeof(unsigned long))) {
		__wrn("copy_from_user fail\n");
		return -EFAULT;
	}

	ubuffer[0] = *(unsigned long *)karg;
	ubuffer[1] = (*(unsigned long *)(karg + 1));
	ubuffer[2] = (*(unsigned long *)(karg + 2));
	ubuffer[3] = (*(unsigned long *)(karg + 3));

	if (cmd < DISP_CMD_FB_REQUEST) {
		if (ubuffer[0] >= num_screens) {
			__wrn("para err in disp_ioctl, cmd = 0x%x\n", cmd);
			__wrn("screen id = %d", (int)ubuffer[0]);
			return -1;
		}
	}
	if (DISPLAY_DEEP_SLEEP & suspend_status) {
		__wrn("ioctl:%x fail when in suspend!\n", cmd);
		return -1;
	}

	if (cmd == disp_cmd_print)
		__wrn("cmd:0x%x,%ld,%ld\n", cmd, ubuffer[0], ubuffer[1]);

	switch (cmd) {
		/* ----disp global---- */
	case DISP_CMD_SET_BKCOLOR:
		{
			disp_color_info para;

			if (copy_from_user(&para,
					   (void __user *)ubuffer[1],
					   sizeof(disp_color_info))) {
				__wrn("copy_from_user fail\n");
				return -EFAULT;
			}
			ret = bsp_disp_set_back_color(ubuffer[0], &para);
			break;
		}

	case DISP_CMD_SET_COLORKEY:
		{
			disp_colorkey para;

			if (copy_from_user(&para,
					   (void __user *)ubuffer[1],
					   sizeof(disp_colorkey))) {
				__wrn("copy_from_user fail\n");
				return -EFAULT;
			}
			ret = bsp_disp_set_color_key(ubuffer[0], &para);
			break;
		}

	case DISP_CMD_GET_OUTPUT_TYPE:
		if (suspend_status == DISPLAY_NORMAL)
			ret = bsp_disp_get_output_type(ubuffer[0]);
		else
			ret = suspend_output_type[ubuffer[0]];
#if defined(CONFIG_HOMLET_PLATFORM)
		if (ret == DISP_OUTPUT_TYPE_LCD)
			ret = bsp_disp_get_lcd_output_type(ubuffer[0]);
#endif
		break;

	case DISP_CMD_GET_SCN_WIDTH:
		ret = bsp_disp_get_screen_width(ubuffer[0]);
		break;

	case DISP_CMD_GET_SCN_HEIGHT:
		ret = bsp_disp_get_screen_height(ubuffer[0]);
		break;

	case DISP_CMD_SHADOW_PROTECT:
		ret = bsp_disp_shadow_protect(ubuffer[0], ubuffer[1]);
		break;

	case DISP_CMD_VSYNC_EVENT_EN:
		ret = bsp_disp_vsync_event_enable(ubuffer[0], ubuffer[1]);
		break;

		/* ----layer---- */
	case DISP_CMD_LAYER_ENABLE:
		ret = bsp_disp_layer_enable(ubuffer[0], ubuffer[1]);
		break;

	case DISP_CMD_LAYER_DISABLE:
		ret = bsp_disp_layer_disable(ubuffer[0], ubuffer[1]);
		break;

	case DISP_CMD_LAYER_SET_INFO:
		{
			disp_layer_info para;

			if (copy_from_user(&para,
					   (void __user *)ubuffer[2],
					   sizeof(disp_layer_info))) {
				__wrn("copy_from_user fail\n");
				return -EFAULT;
			}
			ret = bsp_disp_layer_set_info(ubuffer[0],
						      ubuffer[1], &para);
			break;
		}

	case DISP_CMD_LAYER_GET_INFO:
		{
			disp_layer_info para;

			ret = bsp_disp_layer_get_info(ubuffer[0],
						      ubuffer[1], &para);
			if (copy_to_user((void __user *)ubuffer[2],
					 &para, sizeof(disp_layer_info))) {
				__wrn("copy_to_user fail\n");
				return -EFAULT;
			}
			break;
		}

	case DISP_CMD_LAYER_GET_FRAME_ID:
		ret = bsp_disp_layer_get_frame_id(ubuffer[0], ubuffer[1]);
		break;

		/* ----lcd---- */
	case DISP_CMD_LCD_ENABLE:
		ret = drv_lcd_enable(ubuffer[0]);
		suspend_output_type[ubuffer[0]] = DISP_OUTPUT_TYPE_LCD;

		break;

	case DISP_CMD_LCD_DISABLE:
		ret = drv_lcd_disable(ubuffer[0]);
		suspend_output_type[ubuffer[0]] = DISP_OUTPUT_TYPE_NONE;
		break;

	case DISP_CMD_LCD_SET_BRIGHTNESS:
		ret = bsp_disp_lcd_set_bright(ubuffer[0], ubuffer[1]);
		break;

	case DISP_CMD_LCD_GET_BRIGHTNESS:
		ret = bsp_disp_lcd_get_bright(ubuffer[0]);
		break;

	case DISP_CMD_LCD_BACKLIGHT_ENABLE:
		if (suspend_status == DISPLAY_NORMAL)
			ret = bsp_disp_lcd_backlight_enable(ubuffer[0]);
		break;

	case DISP_CMD_LCD_BACKLIGHT_DISABLE:
		if (suspend_status == DISPLAY_NORMAL)
			ret = bsp_disp_lcd_backlight_disable(ubuffer[0]);
		break;
#if defined(CONFIG_ARCH_SUN9IW1P1)
		/* ----hdmi---- */
	case DISP_CMD_HDMI_ENABLE:
		ret = bsp_disp_hdmi_enable(ubuffer[0]);
		suspend_output_type[ubuffer[0]] = DISP_OUTPUT_TYPE_HDMI;
		break;

	case DISP_CMD_HDMI_DISABLE:
		ret = bsp_disp_hdmi_disable(ubuffer[0]);
		suspend_output_type[ubuffer[0]] = DISP_OUTPUT_TYPE_NONE;
		break;

	case DISP_CMD_HDMI_SET_MODE:
		ret = bsp_disp_hdmi_set_mode(ubuffer[0], ubuffer[1]);
		break;

	case DISP_CMD_HDMI_GET_MODE:
		ret = bsp_disp_hdmi_get_mode(ubuffer[0]);
		break;

	case DISP_CMD_HDMI_SUPPORT_MODE:
		ret = bsp_disp_hdmi_check_support_mode(ubuffer[0], ubuffer[1]);
		break;

	case DISP_CMD_HDMI_GET_HPD_STATUS:
		if (suspend_status == DISPLAY_NORMAL)
			ret = bsp_disp_hdmi_get_hpd_status(ubuffer[0]);
		else
			ret = 0;
		break;
	case DISP_CMD_HDMI_GET_VENDOR_ID:
		{
			__u8 vendor_id[2] = { 0 };

			bsp_disp_hdmi_get_vendor_id(ubuffer[0], vendor_id);
			ret = vendor_id[1] << 8 | vendor_id[0];
			__inf("vendor id [1]=%x, [0]=%x. --> %x",
			      vendor_id[1], vendor_id[0], ret);
		}
		break;

	case DISP_CMD_HDMI_GET_EDID:
		{
			u8 *buf;
			u32 bytes = 1024;

			ret = 0;
			buf = (u8 *) bsp_disp_hdmi_get_edid(ubuffer[0]);
			if (buf) {
				bytes =
				    (ubuffer[2] > bytes) ? bytes : ubuffer[2];
				if (copy_to_user
				    ((void __user *)ubuffer[1], buf, bytes))
					__wrn("copy_to_user fail\n");
				else
					ret = bytes;
			}

			break;
		}
#endif
#if 0
	case DISP_CMD_HDMI_SET_SRC:
		ret = bsp_disp_hdmi_set_src(ubuffer[0],
					    (disp_lcd_src) ubuffer[1]);
		break;
#endif

#if defined(__LINUX_PLAT__)
		/* ----framebuffer---- */
	case DISP_CMD_FB_REQUEST:
		{
			disp_fb_create_info para;

			if (copy_from_user(&para,
					   (void __user *)ubuffer[1],
					   sizeof(disp_fb_create_info))) {
				__wrn("copy_from_user fail\n");
				return -EFAULT;
			}
			ret = Display_Fb_Request(ubuffer[0], &para);
			break;
		}

	case DISP_CMD_FB_RELEASE:
		ret = Display_Fb_Release(ubuffer[0]);
		break;
#endif
#if 0
	case DISP_CMD_FB_GET_PARA:
		{
			disp_fb_create_info para;

			ret = Display_Fb_get_para(ubuffer[0], &para);
			if (copy_to_user((void __user *)ubuffer[1],
					 &para, sizeof(disp_fb_create_info))) {
				__wrn("copy_to_user fail\n");
				return -EFAULT;
			}
			break;
		}

	case DISP_CMD_GET_DISP_INIT_PARA:
		{
			disp_init_para para;

			ret = Display_get_disp_init_para(&para);
			if (copy_to_user((void __user *)ubuffer[0],
					 &para, sizeof(disp_init_para))) {
				__wrn("copy_to_user fail\n");
				return -EFAULT;
			}
			break;
		}

#endif

		/* capture */
	case DISP_CMD_CAPTURE_SCREEN:
		ret = bsp_disp_capture_screen(ubuffer[0],
					      (disp_capture_para *) ubuffer[1]);
		break;

	case DISP_CMD_CAPTURE_SCREEN_STOP:
		ret = bsp_disp_capture_screen_stop(ubuffer[0]);
		break;

		/* ----enhance---- */
	case DISP_CMD_SET_BRIGHT:
		ret = bsp_disp_smcl_set_bright(ubuffer[0], ubuffer[1]);
		break;

	case DISP_CMD_GET_BRIGHT:
		ret = bsp_disp_smcl_get_bright(ubuffer[0]);
		break;

	case DISP_CMD_SET_CONTRAST:
		ret = bsp_disp_smcl_set_contrast(ubuffer[0], ubuffer[1]);
		break;

	case DISP_CMD_GET_CONTRAST:
		ret = bsp_disp_smcl_get_contrast(ubuffer[0]);
		break;

	case DISP_CMD_SET_SATURATION:
		ret = bsp_disp_smcl_set_saturation(ubuffer[0], ubuffer[1]);
		break;

	case DISP_CMD_GET_SATURATION:
		ret = bsp_disp_smcl_get_saturation(ubuffer[0]);
		break;

	case DISP_CMD_SET_HUE:
		ret = bsp_disp_smcl_set_hue(ubuffer[0], ubuffer[1]);
		break;

	case DISP_CMD_GET_HUE:
		ret = bsp_disp_smcl_get_hue(ubuffer[0]);
		break;

	case DISP_CMD_ENHANCE_ENABLE:
		ret = bsp_disp_smcl_enable(ubuffer[0]);
		break;

	case DISP_CMD_ENHANCE_DISABLE:
		ret = bsp_disp_smcl_disable(ubuffer[0]);
		break;

	case DISP_CMD_GET_ENHANCE_EN:
		ret = bsp_disp_smcl_is_enabled(ubuffer[0]);
		break;

	case DISP_CMD_SET_ENHANCE_MODE:
		ret = bsp_disp_smcl_set_mode(ubuffer[0], ubuffer[1]);
		break;

	case DISP_CMD_GET_ENHANCE_MODE:
		ret = bsp_disp_smcl_get_mode(ubuffer[0]);
		break;

	case DISP_CMD_SET_ENHANCE_WINDOW:
		{
			disp_window para;

			if (copy_from_user(&para,
					   (void __user *)ubuffer[1],
					   sizeof(disp_window))) {
				__wrn("copy_from_user fail\n");
				return -EFAULT;
			}
			ret = bsp_disp_smcl_set_window(ubuffer[0], &para);
			break;
		}

	case DISP_CMD_GET_ENHANCE_WINDOW:
		{
			disp_window para;

			ret = bsp_disp_smcl_get_window(ubuffer[0], &para);
			if (copy_to_user((void __user *)ubuffer[1],
					 &para, sizeof(disp_window))) {
				__wrn("copy_to_user fail\n");
				return -EFAULT;
			}
			break;
		}

	case DISP_CMD_DRC_ENABLE:
		ret = bsp_disp_smbl_enable(ubuffer[0]);
		break;

	case DISP_CMD_DRC_DISABLE:
		ret = bsp_disp_smbl_disable(ubuffer[0]);
		break;

	case DISP_CMD_GET_DRC_EN:
		ret = bsp_disp_smbl_is_enabled(ubuffer[0]);
		break;

	case DISP_CMD_DRC_SET_WINDOW:
		{
			disp_window para;

			if (copy_from_user(&para,
					   (void __user *)ubuffer[1],
					   sizeof(disp_window))) {
				__wrn("copy_from_user fail\n");
				return -EFAULT;
			}
			ret = bsp_disp_smbl_set_window(ubuffer[0], &para);
			break;
		}

	case DISP_CMD_DRC_GET_WINDOW:
		{
			disp_window para;

			ret = bsp_disp_smbl_get_window(ubuffer[0], &para);
			if (copy_to_user((void __user *)ubuffer[1],
					 &para, sizeof(disp_window))) {
				__wrn("copy_to_user fail\n");
				return -EFAULT;
			}
			break;
		}

#if defined(CONFIG_ARCH_SUN9IW1P1)
		/* ---- cursor ---- */
	case DISP_CMD_CURSOR_ENABLE:
		ret = bsp_disp_cursor_enable(ubuffer[0]);
		break;

	case DISP_CMD_CURSOR_DISABLE:
		ret = bsp_disp_cursor_disable(ubuffer[0]);
		break;

	case DISP_CMD_CURSOR_SET_POS:
		{
			disp_position para;

			if (copy_from_user(&para,
					   (void __user *)ubuffer[1],
					   sizeof(disp_position))) {
				__wrn("copy_from_user fail\n");
				return -EFAULT;
			}
			ret = bsp_disp_cursor_set_pos(ubuffer[0], &para);
			break;
		}

	case DISP_CMD_CURSOR_GET_POS:
		{
			disp_position para;

			ret = bsp_disp_cursor_get_pos(ubuffer[0], &para);
			if (copy_to_user((void __user *)ubuffer[1],
					 &para, sizeof(disp_position))) {
				__wrn("copy_to_user fail\n");
				return -EFAULT;
			}
			break;
		}

	case DISP_CMD_CURSOR_SET_FB:
		{
			disp_cursor_fb para;

			if (copy_from_user(&para,
					   (void __user *)ubuffer[1],
					   sizeof(disp_cursor_fb))) {
				__wrn("copy_from_user fail\n");
				return -EFAULT;
			}
			ret = bsp_disp_cursor_set_fb(ubuffer[0], &para);
			break;
		}

	case DISP_CMD_CURSOR_SET_PALETTE:
		if ((ubuffer[1] == 0) || ((int)ubuffer[3] <= 0)) {
			__wrn("para invalid ,buffer:0x%x,size:0x%x\n",
			      (unsigned int)ubuffer[1],
			      (unsigned int)ubuffer[3]);
			return -1;
		}
		if (copy_from_user(gbuffer, (void __user *)ubuffer[1],
				   ubuffer[3])) {
			__wrn("copy_from_user fail\n");
			return -EFAULT;
		}
		ret = bsp_disp_cursor_set_palette(ubuffer[0], (void *)gbuffer,
						  ubuffer[2], ubuffer[3]);
		break;
#endif

#if defined(__LINUX_PLAT__)		/* ----for test---- */
	case DISP_CMD_MEM_REQUEST:
		ret = disp_mem_request(ubuffer[0], ubuffer[1]);
		break;

	case DISP_CMD_MEM_RELEASE:
		ret = disp_mem_release(ubuffer[0]);
		break;

	case DISP_CMD_MEM_SELIDX:
		g_disp_mm_sel = ubuffer[0];
		break;

	case DISP_CMD_MEM_GETADR:
		ret = g_disp_mm[ubuffer[0]].mem_start;
		break;
#endif
/* case DISP_CMD_PRINT_REG: */
/* ret = bsp_disp_print_reg(1, ubuffer[0], 0); */
/* break; */

		/* ----for tv ---- */
	case DISP_CMD_TV_ON:
#if defined(CONFIG_ARCH_SUN9IW1P1)
		ret = drv_lcd_enable(ubuffer[0]);
		suspend_output_type[ubuffer[0]] = DISP_OUTPUT_TYPE_LCD;
#endif
		break;
	case DISP_CMD_TV_OFF:
#if defined(CONFIG_ARCH_SUN9IW1P1)
		ret = drv_lcd_disable(ubuffer[0]);
		suspend_output_type[ubuffer[0]] = DISP_OUTPUT_TYPE_NONE;
#endif
		break;
	case DISP_CMD_TV_GET_MODE:
#if defined(CONFIG_ARCH_SUN9IW1P1)
		ret = bsp_disp_lcd_get_tv_mode(ubuffer[0]);
#endif
		break;
	case DISP_CMD_TV_SET_MODE:
#if defined(CONFIG_ARCH_SUN9IW1P1)
		ret = bsp_disp_lcd_set_tv_mode(ubuffer[0], ubuffer[1]);
#endif
		break;

	case DISP_CMD_SET_EXIT_MODE:
        ret = g_disp_drv.exit_mode = ubuffer[0];
		break;

	case DISP_CMD_LCD_CHECK_OPEN_FINISH:
		ret = drv_lcd_check_open_finished(ubuffer[0]);
		break;

	case DISP_CMD_LCD_CHECK_CLOSE_FINISH:
		ret = drv_lcd_check_close_finished(ubuffer[0]);
		break;

	default:
		break;
	}

	return ret;
}
#define  DELAY_ONCE_TIME   (50)

s32 drv_disp_standby(u32 cmd, void *pArg)
{
	s32 ret;
	s32 timedly = 5000;
	s32 check_time = timedly/DELAY_ONCE_TIME;

	if (cmd == BOOT_MOD_ENTER_STANDBY)
	{
	    if (g_output_type == DISP_OUTPUT_TYPE_HDMI)
	    {
		}
		else
        {
            drv_lcd_disable(0);
		}
		do
		{
			ret = drv_lcd_check_close_finished(0);
			if (ret == 1)
			{
				break;
			}
			else if (ret == -1)
			{
				return -1;
			}
			__msdelay(DELAY_ONCE_TIME);
			check_time --;
			if (check_time <= 0)
			{
				return -1;
			}
		}
		while (1);

		return 0;
	}
	else if (cmd == BOOT_MOD_EXIT_STANDBY)
	{
		if (g_output_type == DISP_OUTPUT_TYPE_HDMI)
		{
		}
		else
		{
			drv_lcd_enable(0);
        }

		do
		{
			ret = drv_lcd_check_open_finished(0);
			if (ret == 1)
			{
				break;
			}
			else if (ret == -1)
			{
				return -1;
			}
			__msdelay(DELAY_ONCE_TIME);
			check_time --;
			if (check_time <= 0)
			{
				return -1;
			}
		}
		while (1);

		return 0;
	}

	return -1;
}

