/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include <malloc.h>
#include <sys_config.h>
#include <sunxi_display2.h>
#include <bmp_layout.h>
#include <fdt_support.h>

DECLARE_GLOBAL_DATA_PTR;

static __u32 screen_id = 0;
static __u32 disp_para = 0;
static __u32 fb_base_addr = SUNXI_DISPLAY_FRAME_BUFFER_ADDR;
extern __s32 disp_delay_ms(__u32 ms);
extern long disp_ioctl(void *hd, unsigned int cmd, void *arg);
extern int aw_fat_fsload(char *part_name, char *file_name, char* load_addr, ulong length);

static int board_display_update_para_for_kernel(char *name, int value)
{
#ifndef CONFIG_SUNXI_MULITCORE_BOOT
	int node;
	int ret = -1;

	node = fdt_path_offset(working_fdt,"disp");
	if (node < 0) {
		pr_error("%s:disp_fdt_nodeoffset %s fail\n", __func__,"disp");
		goto exit;
	}

	ret = fdt_setprop_u32(working_fdt, node, name, (uint32_t)value);
	if ( ret < 0)
		pr_error("fdt_setprop_u32 %s.%s(0x%x) fail.err code:%s\n", "disp", name, value,fdt_strerror(ret));
	else
		ret = 0;

exit:
	return ret;
#else
	return sunxi_fdt_getprop_store(working_fdt, "disp", name, value);
#endif
}

//void display_update_dtb(void)
//{
//	board_display_update_para_for_kernel("fb_base", fb_base_addr);
//	board_display_update_para_for_kernel("boot_disp", disp_para);
//}

int board_display_layer_request(void)
{
	gd->layer_hd = 0;
	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int board_display_layer_release(void)
{
	return 0;

}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int board_display_wait_lcd_open(void)
{
	int ret;
	int timedly = 5000;
	int check_time = timedly/50;
	uint arg[4] = { 0 };
	uint cmd = 0;

	cmd = DISP_LCD_CHECK_OPEN_FINISH;

	do
	{
    	ret = disp_ioctl(NULL, cmd, (void*)arg);
		if(ret == 1)		//open already
		{
			break;
		}
		else if(ret == -1)  //open falied
		{
			return -1;
		}
		__msdelay(50);
		check_time --;
		if(check_time <= 0)
		{
			return -1;
		}
	}
	while(1);

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int board_display_wait_lcd_close(void)
{
	int ret;
	int timedly = 5000;
	int check_time = timedly/50;
	uint arg[4] = { 0 };
	uint cmd = 0;

	cmd = DISP_LCD_CHECK_CLOSE_FINISH;

	do
	{
    	ret = disp_ioctl(NULL, cmd, (void*)arg);
		if(ret == 1)		//open already
		{
			break;
		}
		else if(ret == -1)  //open falied
		{
			return -1;
		}
		__msdelay(50);
		check_time --;
		if(check_time <= 0)
		{
			return -1;
		}
	}
	while(1);

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int board_display_set_exit_mode(int lcd_off_only)
{
	uint arg[4] = { 0 };
	uint cmd = 0;
	cmd = DISP_SET_EXIT_MODE;

	if(lcd_off_only)
	{
		arg[0] = DISP_EXIT_MODE_CLEAN_PARTLY;
		disp_ioctl(NULL, cmd, (void *)arg);
	}
	else
	{
		cmd = DISP_LCD_DISABLE;
		disp_ioctl(NULL, cmd, (void *)arg);
		board_display_wait_lcd_close();
	}

	return 0;
}
/*
*******************************************************************************
*                     board_display_layer_open
*
* Description:
*    打开图层
*
* Parameters:
*    Layer_hd    :  input. 图层句柄
*
* Return value:
*    0  :  成功
*   !0  :  失败
*
* note:
*    void
*
*******************************************************************************
*/
int board_display_layer_open(void)
{
	uint arg[4];
	struct disp_layer_config *config = (struct disp_layer_config *)gd->layer_para;

	arg[0] = screen_id;
	arg[1] = (unsigned long)config;
	arg[2] = 1;
	arg[3] = 0;

	disp_ioctl(NULL,DISP_LAYER_GET_CONFIG,(void*)arg);
	config->enable = 1;
	disp_ioctl(NULL,DISP_LAYER_SET_CONFIG,(void*)arg);

	return 0;
}


/*
*******************************************************************************
*                     board_display_layer_close
*
* Description:
*    关闭图层
*
* Parameters:
*    Layer_hd    :  input. 图层句柄
*
* Return value:
*    0  :  成功
*   !0  :  失败
*
* note:
*    void
*
*******************************************************************************
*/
int board_display_layer_close(void)
{
	uint arg[4];
	struct disp_layer_config *config = (struct disp_layer_config *)gd->layer_para;

	arg[0] = screen_id;
	arg[1] = (unsigned long)config;
	arg[2] = 1;
	arg[3] = 0;

	disp_ioctl(NULL,DISP_LAYER_GET_CONFIG,(void*)arg);
	config->enable = 0;
	disp_ioctl(NULL,DISP_LAYER_SET_CONFIG,(void*)arg);



    return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int board_display_layer_para_set(void)
{
	uint arg[4];

	arg[0] = screen_id;
	arg[1] = gd->layer_para;
	arg[2] = 1;
	arg[3] = 0;
	disp_ioctl(NULL,DISP_LAYER_SET_CONFIG,(void*)arg);

    return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int board_display_show_until_lcd_open(int display_source)
{
	pr_msg("%s\n", __func__);
	if(!display_source)
	{
		board_display_wait_lcd_open();
	}
	board_display_layer_para_set();
	board_display_layer_open();

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int board_display_show(int display_source)
{
	board_display_layer_para_set();
	board_display_layer_open();

	return 0;

}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int board_display_framebuffer_set(int width, int height, int bitcount, void *buffer)
{

	struct disp_layer_config *layer_para;
	uint screen_width, screen_height;
	uint arg[4];
	uint32_t full = 0;
	int nodeoffset;

	if(!gd->layer_para)
	{
		layer_para = (struct disp_layer_config *)malloc(sizeof(struct disp_layer_config));
		if(!layer_para)
		{
			pr_error("display: unable to malloc memory for layer\n");

			return -1;
		}
	}
	else
	{
		layer_para = (struct disp_layer_config *)gd->layer_para;
	}

	nodeoffset = fdt_path_offset(working_fdt,"boot_disp");
	if (nodeoffset >= 0)
	{
		if(fdt_getprop_u32(working_fdt,nodeoffset,"output_full",&full) < 0)
		{
			pr_error("fetch script data boot_disp.output_full fail\n");
		}
	}

	arg[0] = screen_id;
	screen_width = disp_ioctl(NULL, DISP_GET_SCN_WIDTH, (void*)arg);
	screen_height = disp_ioctl(NULL, DISP_GET_SCN_HEIGHT, (void*)arg);
	pr_notice("screen_id =%d, screen_width =%d, screen_height =%d\n", screen_id, screen_width, screen_height);
	memset((void *)layer_para, 0, sizeof(struct disp_layer_config));
	layer_para->info.fb.addr[0]		= (uint)buffer;
	pr_notice("frame buffer address %x\n", (uint)buffer);
	layer_para->channel = 1;
	layer_para->layer_id = 0;
	layer_para->info.fb.format		= (bitcount == 24)? DISP_FORMAT_RGB_888:DISP_FORMAT_ARGB_8888;
	layer_para->info.fb.size[0].width	= width;
	layer_para->info.fb.size[0].height	= height;
	layer_para->info.fb.crop.x	= 0;
	layer_para->info.fb.crop.y	= 0;
	layer_para->info.fb.crop.width	= ((unsigned long long)width) << 32;
	layer_para->info.fb.crop.height	= ((unsigned long long)height) << 32;
	layer_para->info.fb.flags = DISP_BF_NORMAL;
	layer_para->info.fb.scan = DISP_SCAN_PROGRESSIVE;
	debug("bitcount = %d\n", bitcount);
	layer_para->info.mode     = LAYER_MODE_BUFFER;
	layer_para->info.alpha_mode    = 1;
	layer_para->info.alpha_value   = 0xff;
	if(full) {
		layer_para->info.screen_win.x	= 0;
		layer_para->info.screen_win.y	= 0;
		layer_para->info.screen_win.width	= screen_width;
		layer_para->info.screen_win.height	= screen_height;
	} else {
		layer_para->info.screen_win.x	= (screen_width - width) / 2;
		layer_para->info.screen_win.y	= (screen_height - height) / 2;
		layer_para->info.screen_win.width	= width;
		layer_para->info.screen_win.height	= height;
	}
	layer_para->info.b_trd_out		= 0;
	layer_para->info.out_trd_mode 	= 0;
	gd->layer_para = (uint)layer_para;
	fb_base_addr=(uint)buffer - sizeof(bmp_header_t);
	board_display_update_para_for_kernel("fb_base", (uint)buffer - sizeof(bmp_header_t));
	return 0;
}

void board_display_set_alpha_mode(int mode)
{
	if(!gd->layer_para)
	{
		return;
	}

	struct disp_layer_config *layer_para;
	layer_para = (struct disp_layer_config *)gd->layer_para;
	layer_para->info.alpha_mode = mode;

}

int board_display_framebuffer_change(void *buffer)
{
	return 0;
}

#if (defined CONFIG_ARCH_HOMELET)
int get_display_resolution(int type)
{
	int read_bytes = 0;
	int temp_value = 0;
	char buf[512] = {0};
	char *p = buf;
	char *pstr;

	read_bytes = aw_fat_fsload("Reserve0", "disp_rsl.fex", buf, 512);
	while (read_bytes > 0) {
		pstr = p;
		while (*p!='\n' && *p!='\0')
			p++;

		*p++ = '\0';
		temp_value = simple_strtoul(pstr, NULL, 16);
		if (((temp_value>>8) & 0xff) == type) {
			printf("display resolution %d, type %d\n", temp_value&0xff, type);
			return temp_value & 0xff;
		}
		read_bytes = read_bytes - (p - pstr);
	}
	return 0;
}

static int get_saved_hdmi_vendor_id(void)
{
	char data_buf[512] = {0};
	char *pvendor_id, *pdata, *pdata_end;
	int vendor_id = 0;
	int i = 0;
	int ret = 0;

	ret = aw_fat_fsload("Reserve0", "tv_vdid.fex", data_buf, 512);
	if (0 >= ret)
		return 0;

	pvendor_id = data_buf;
	pdata = data_buf;
	pdata_end = pdata + ret;
	for (; (i < 4) && (pdata != pdata_end); pdata++) {
		if('\n' == *pdata) {
			*pdata = '\0';
			ret = (int)simple_strtoul(pvendor_id, NULL, 16);
			vendor_id |= ((ret & 0xFF) << (8 * (3 - i)));
			pvendor_id = pdata + 1;
			i++;
		}
	}

	printf("last television vendor id: 0x%08x\n", vendor_id);
	return vendor_id;
}

static int get_hdmi_edid(int sel, unsigned char **edid_buf, int length)
{
	unsigned long arg[4] = {0};
	arg[0] = sel;
	arg[1] = (unsigned long)edid_buf;
	arg[2] = length;
	disp_ioctl(NULL, DISP_HDMI_GET_EDID, (void*)arg);
	return 0;
}

static int get_hdmi_vendor_id(int sel)
{
#define ID_VENDOR 0x08
	int vendor_id = 0;
	unsigned char *pVendor = NULL;
	unsigned char *edid_buf = NULL;

	get_hdmi_edid(sel, &edid_buf, 1024);
	if (edid_buf == NULL) {
		printf("read hdmi edid failed\n");
		return 0;
	}
	pVendor = edid_buf + ID_VENDOR;
	vendor_id = (pVendor[0] << 24);
	vendor_id |= (pVendor[1] << 16);
	vendor_id |= (pVendor[2] << 8);
	vendor_id |= pVendor[3];
	printf("current vendor id=0x%08x\n", vendor_id);
	return vendor_id;
}

static int verify_hdmi_mode(int channel, int mode, int *vid, int check)
{
	unsigned long arg[4] = {0};
	const int HDMI_MODES[] = { // self-define hdmi mode list
		DISP_TV_MOD_720P_60HZ,
		DISP_TV_MOD_720P_50HZ,
		DISP_TV_MOD_1080P_60HZ,
		DISP_TV_MOD_1080P_50HZ,
	};
	int i = 0;
	int actual_vendor_id = get_hdmi_vendor_id(channel);
	int saved_vendor_id = get_saved_hdmi_vendor_id();

	*vid = actual_vendor_id?actual_vendor_id:saved_vendor_id;
	if(2 == check){
		/* if vendor id change , check mode: check = 1 */
		if (actual_vendor_id && (actual_vendor_id != saved_vendor_id)) {
			check = 1;
			*vid = actual_vendor_id;
			printf("vendor:0x%x, saved_vendor:0x%x\n",
				actual_vendor_id, saved_vendor_id);
		}
	}

	/* check if support the output_mode by television,
	 * return 0 is not support */
	if (1 == check) {
		arg[0] = channel;
		arg[1] = mode;
		if (1 == disp_ioctl(NULL, DISP_HDMI_SUPPORT_MODE, (void*)arg)) {
			return mode;
		}
		for(i = 0; i < sizeof(HDMI_MODES) / sizeof(HDMI_MODES[0]); i++) {
			arg[1] = HDMI_MODES[i];
			if (1 == disp_ioctl(NULL, DISP_HDMI_SUPPORT_MODE, (void*)arg)) {
				printf("not support mode[%d], find mode[%d] in HDMI_MODES\n", mode, HDMI_MODES[i]);
				return HDMI_MODES[i];
			}
		}
		mode = HDMI_MODES[0]; // any mode of HDMI_MODES is not supported.  fixme:  use HDMI_MODES[0] ???
		printf("not find suitable mode in HDMI_MODES either, use mode[%d]\n", mode);
	}

	return mode;
}

static int init_display_out_attr(int type, __u32 *used, __u32 *channel, __u32 *mode)
{
	const char *channel_prop[] = {
		[DISP_OUTPUT_TYPE_TV]   = "cvbs_channel",
		[DISP_OUTPUT_TYPE_HDMI] = "hdmi_channel",
	};
	const char *mode_prop[] = {
		[DISP_OUTPUT_TYPE_TV]   = "cvbs_mode",
		[DISP_OUTPUT_TYPE_HDMI] = "hdmi_mode",
	};

	int node;
	int output_mode;
	const char *cprop = channel_prop[type];
	const char *mprop = mode_prop[type];

	node = fdt_path_offset(working_fdt, "boot_disp");
	if (node < 0) {
		printf("fdt could not find path 'boot_disp'\n");
		return -1;
	}

	if (fdt_getprop_u32(working_fdt, node, cprop, (uint32_t *)channel) < 0) {
		printf("fdt get prop '%s' failed\n", cprop);
		*used = 0;
	} else {
		*used = 1;
		*mode = 0;
		output_mode = get_display_resolution(type);
		if (output_mode <= 0) {
			printf("could not get output resolution for '%s'\n", cprop);
			if (fdt_getprop_u32(working_fdt, node, mprop, (uint32_t *)&output_mode) < 0) {
				printf("fdt get prop '%s' failed\n", mprop);
			}
		}
		*mode = (__u32)output_mode;
	}

	printf("display output attr: type %d, used %d, channel %d, mode %d\n",
		type, *used, *channel, *mode);
	return 0;
}

static void disp_getprop_by_name(int node, const char *name, uint32_t *value, uint32_t defval)
{
	if (fdt_getprop_u32(working_fdt, node, name, value) < 0) {
		printf("fetch script data boot_disp.%s fail\n", name);
		*value = defval;
	 } else {
		printf("boot_disp.%s=%d\n", name, *value);
	}
}

static int output_type_verify(int type)
{
	const int _define_type[] = {
		[0] = DISP_OUTPUT_TYPE_NONE,
		[1] = DISP_OUTPUT_TYPE_LCD,
		[2] = DISP_OUTPUT_TYPE_TV,
		[3] = DISP_OUTPUT_TYPE_HDMI,
		[4] = DISP_OUTPUT_TYPE_VGA,
	};

	if (type<0 || type>(sizeof(_define_type)/sizeof(_define_type[0])-1))
		return DISP_OUTPUT_TYPE_NONE;
	return _define_type[type];
}

int board_display_device_open(void)
{
	int i;
	int node;
	unsigned long arg[4] = {0};
	__u32 auto_hpd;
	__u32 hdmi_used = 0;
	__u32 hdmi_channel, hdmi_mode;
	__u32 cvbs_used = 0;
	__u32 cvbs_channel, cvbs_mode;
	__u32 lcd_channel;
	int hdmi_connect = 0;
	int cvbs_connect = 0;
	int output_type = 0;
	int output_mode = 0;
	int boot_disp = 0;
	int init_disp = 0;
	int hdmi_mode_check = 0;
	int hdmi_vendor_id = 0;

	init_display_out_attr(DISP_OUTPUT_TYPE_HDMI, &hdmi_used, &hdmi_channel, &hdmi_mode);
	init_display_out_attr(DISP_OUTPUT_TYPE_TV, &cvbs_used, &cvbs_channel, &cvbs_mode);

	node = fdt_path_offset(working_fdt, "boot_disp");
	if (node < 0) {
		printf("fdt could not find path 'boot_disp'\n");
		return -1;
	}

	/* getproc auto_hpd, indicate output device decided by the hot plug status of device */
	disp_getprop_by_name(node, "auto_hpd", (uint32_t*)&auto_hpd, 0);
	disp_getprop_by_name(node, "hdmi_mode_check", (uint32_t*)&hdmi_mode_check, 1);
	disp_getprop_by_name(node, "output_type", (uint32_t*)&output_type, 0);
	output_type = output_type_verify(output_type);

	if (auto_hpd && (hdmi_used || cvbs_used)) {
		/* auto detect hdmi and cvbs */
		arg[0] = hdmi_channel;
		arg[1] = 0;
		hdmi_connect = disp_ioctl(NULL, DISP_HDMI_GET_HPD_STATUS, (void*)arg);
		for(i=0; (i<100)&&(hdmi_connect==0) && ((0==cvbs_used)||(i<50)||(cvbs_connect==0)); i++){
			disp_delay_ms(10);
			arg[0] = hdmi_channel;
			hdmi_connect = disp_ioctl(NULL, DISP_HDMI_GET_HPD_STATUS, (void*)arg);
		    if(cvbs_connect == 0){
				arg[0] = cvbs_channel;
				cvbs_connect = disp_ioctl(NULL, DISP_TV_GET_HPD_STATUS, (void*)arg);
		    }
		}
		printf("auto hpd result: hdmi_connect=%d, cvbs_connect=%d!\n", hdmi_connect, cvbs_connect);

		/* init output_type */
		if(hdmi_connect != 0){
			output_type = DISP_OUTPUT_TYPE_HDMI;
		} else if(cvbs_connect != 0) {
			output_type = DISP_OUTPUT_TYPE_TV;
		}
	} else {
		printf("auto hpd disable, use default output_type=%d\n", output_type);
	}

    switch(output_type) {
    case DISP_OUTPUT_TYPE_HDMI:
        screen_id = hdmi_channel;
		output_mode = verify_hdmi_mode(hdmi_channel, hdmi_mode,
						&hdmi_vendor_id, hdmi_mode_check);
        break;
    case DISP_OUTPUT_TYPE_TV:
        screen_id = cvbs_channel;
		output_mode = cvbs_mode;
        break;
    case DISP_OUTPUT_TYPE_LCD:
		disp_getprop_by_name(node, "lcd_channel", (uint32_t*)&lcd_channel, -1);
		if (lcd_channel<0)
			goto _error;
		screen_id = lcd_channel;
		break;
    default:
		printf("check what the output_type=%d\n", output_type);
		return -1;
    }

	/* open output display */
	if (((output_type==DISP_OUTPUT_TYPE_HDMI) && hdmi_connect) ||
		((output_type==DISP_OUTPUT_TYPE_TV) && cvbs_connect) ||
		output_type == DISP_OUTPUT_TYPE_LCD ) {
		printf("disp%d device type(%d) enable\n", screen_id, output_type);
		arg[0] = screen_id;
		arg[1] = output_type;
		arg[2] = output_mode;
		disp_ioctl(NULL, DISP_DEVICE_SWITCH, (void *)arg);

		/* update dts for kernel driver */
		boot_disp = ((output_type << 8) | (output_mode)) << (screen_id*16);
	}

	if (hdmi_used)
		init_disp |= (((DISP_OUTPUT_TYPE_HDMI & 0xff) << 8) | (hdmi_mode & 0xff)) << (hdmi_channel << 4);
	if(cvbs_used)
		init_disp |= (((DISP_OUTPUT_TYPE_TV & 0xff) << 8) | (cvbs_mode & 0xff)) << (cvbs_channel << 4);

	board_display_update_para_for_kernel("boot_disp", boot_disp);
	board_display_update_para_for_kernel("init_disp", init_disp);
	board_display_update_para_for_kernel("tv_vdid", hdmi_vendor_id);
	return 0;

_error:
	printf("skip display device open, check sys_config and hardward!\n");
	return -1;
}
#else
int board_display_device_open(void)
{

	int  value = 1;
	int  ret = 0;
	__u32 output_type = 0;
	__u32 output_mode = 0;
	__u32 auto_hpd = 0;
	__u32 err_count = 0;
	unsigned long arg[4] = {0};
	int node;

	debug("De_OpenDevice\n");

	node = fdt_path_offset(working_fdt,"boot_disp");
	if (node >= 0) {
		/* getproc output_disp, indicate which disp channel will be using */
		if (fdt_getprop_u32(working_fdt, node, "output_disp", (uint32_t*)&screen_id) < 0) {
			pr_error("fetch script data boot_disp.output_disp fail\n");
			err_count ++;
		} else
			pr_msg("boot_disp.output_disp=%d\n", screen_id);

		/* getproc output_type, indicate which kind of device will be using */
		if (fdt_getprop_u32(working_fdt, node, "output_type", (uint32_t*)&value) < 0) {
			pr_error("fetch script data boot_disp.output_type fail\n");
			err_count ++;
		} else
			pr_msg("boot_disp.output_type=%d\n", value);

		if(value == 0)
		{
			output_type = DISP_OUTPUT_TYPE_NONE;
		}
		else if(value == 1)
		{
			output_type = DISP_OUTPUT_TYPE_LCD;
		}
		else if(value == 2)
		{
			output_type = DISP_OUTPUT_TYPE_TV;
		}
		else if(value == 3)
		{
			output_type = DISP_OUTPUT_TYPE_HDMI;
		}
		else if(value == 4)
		{
			output_type = DISP_OUTPUT_TYPE_VGA;
		}
		else
		{
			pr_error("invalid output_type %d\n", value);
			return -1;
		}

		/* getproc output_mode, indicate which kind of mode will be output */
		if (fdt_getprop_u32(working_fdt, node, "output_mode", (uint32_t*)&output_mode) < 0) {
			pr_error("fetch script data boot_disp.output_mode fail\n");
			err_count ++;
		} else
			printf("boot_disp.output_mode=%d\n", output_mode);

		/* getproc auto_hpd, indicate output device decided by the hot plug status of device */
		if (fdt_getprop_u32(working_fdt, node, "auto_hpd", (uint32_t*)&auto_hpd) < 0) {
			pr_error("fetch script data boot_disp.auto_hpd fail\n");
			err_count ++;
		 } else
			pr_msg("boot_disp.auto_hpd=%d\n", auto_hpd);
	} else
		err_count = 4;

	if(err_count >= 4)//no boot_disp config
	{
		output_type = DISP_OUTPUT_TYPE_LCD;
	}
	else//has boot_disp config
	{

	}
	pr_notice("disp%d device type(%d) enable\n", screen_id, output_type);

	arg[0] = screen_id;
	arg[1] = output_type;
	arg[2] = output_mode;
	disp_ioctl(NULL, DISP_DEVICE_SWITCH, (void *)arg);



	disp_para = ((output_type << 8) | (output_mode)) << (screen_id*16);

	board_display_update_para_for_kernel("boot_disp", disp_para);

	return ret;
}
#endif

void board_display_setenv(char *data)
{
	if (!data)
		return;

	sprintf(data, "%x", disp_para);
}

int borad_display_get_screen_width(void)
{
	unsigned long arg[4] = {0};
	arg[0] = screen_id;

	return disp_ioctl(NULL, DISP_GET_SCN_WIDTH, (void*)arg);

}

int borad_display_get_screen_height(void)
{
	unsigned long arg[4] = {0};

	arg[0] = screen_id;
	return disp_ioctl(NULL, DISP_GET_SCN_HEIGHT, (void*)arg);

}

