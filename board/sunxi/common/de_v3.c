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

#include <sunxi_board.h>
#include <../drivers/video/sunxi/disp2/disp/de/include.h>

DECLARE_GLOBAL_DATA_PTR;

static __u32 screen_id = 0;
static __u32 disp_para = 0;
static __u32 disp_para1;
static __u32 disp_para2;
static __u32 fb_base_addr = SUNXI_DISPLAY_FRAME_BUFFER_ADDR;
extern __s32 disp_delay_ms(__u32 ms);
extern long disp_ioctl(void *hd, unsigned int cmd, void *arg);

static inline void *malloc_aligned(u32 size, u32 alignment)
{
	void *ptr = (void *)malloc(size + alignment);
	if (ptr) {
		void *aligned = (void *)(((long)ptr + alignment) & (~(alignment-1)));

		/* Store the original pointer just before aligned pointer*/
		((void **) aligned)[-1]  = ptr;
		return aligned;
	}

	return NULL;
}

static inline void free_aligned(void *aligned_ptr)
{
	if (aligned_ptr)
		free(((void **) aligned_ptr)[-1]);
}


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
		arg[1] = DISP_EXIT_MODE_CLEAN_PARTLY;
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
******************************************************************************
*
*    function
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
******************************************************************************
*/

static struct eink_8bpp_image cimage;
int board_display_eink_update(char *name, __u32 update_mode)

{
	uint arg[4] = { 0 };
	uint cmd = 0;
	int ret;

	u32 width = 800;
	u32 height = 600;

	char *bmp_buffer = NULL;
	u32 buf_size = 0;

	char primary_key[25];
	s32 value = 0;
	u32 disp = 0;
	sprintf(primary_key, "lcd%d", disp);

	ret = disp_sys_script_get_item(primary_key, "eink_width", &value, 1);
	if (ret == 1)
		width = value;

	ret = disp_sys_script_get_item(primary_key, "eink_height", &value, 1);
	if (ret == 1)
		height = value;

	buf_size = (width * height) << 2;

	bmp_buffer = (char *)malloc_aligned(buf_size, ARCH_DMA_MINALIGN);
	if (NULL == bmp_buffer)
		printf("fail to alloc memory for display bmp.\n");

	sunxi_Eink_Get_bmp_buffer(name, bmp_buffer);

	cimage.update_mode = update_mode;
	cimage.flash_mode = GLOBAL;
	cimage.state = USED;
	cimage.window_calc_enable = false;
	cimage.size.height = height;
	cimage.size.width = width;
	cimage.size.align = 4;
	cimage.paddr = bmp_buffer;
	cimage.vaddr = bmp_buffer;
	cimage.update_area.x_top = 0;
	cimage.update_area.y_top = 0;
	cimage.update_area.x_bottom = width - 1;
	cimage.update_area.y_bottom = height - 1;

	arg[0] = (uint)&cimage;
	arg[1] = 0;
	arg[2] = 0;
	arg[3] = 0;

	cmd = DISP_EINK_UPDATE;
	ret = disp_ioctl(NULL, cmd, (void *)arg);
	if (ret != 0) {
		printf("update eink image fail\n");
		return -1;
	}
	if (bmp_buffer) {
		free_aligned(bmp_buffer);
		bmp_buffer = NULL;
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
	if (full || (screen_width < width || screen_height < height)) {
		layer_para->info.screen_win.x = 0;
		layer_para->info.screen_win.y = 0;
		layer_para->info.screen_win.width = screen_width;
		layer_para->info.screen_win.height = screen_height;
		if (!full)
			pr_error("\nBMP file is to large!!!!!!!!!!!!!!\n");
	} else {
		layer_para->info.screen_win.x = (screen_width - width) / 2;
		layer_para->info.screen_win.y = (screen_height - height) / 2;
		layer_para->info.screen_win.width = width;
		layer_para->info.screen_win.height = height;
	}
	layer_para->info.b_trd_out		= 0;
	layer_para->info.out_trd_mode 	= 0;
	gd->layer_para = (uint)layer_para;
	fb_base_addr=(uint)buffer - sizeof(bmp_header_t);
#ifndef CONFIG_SUNXI_MULITCORE_BOOT
	board_display_update_para_for_kernel("fb_base", (uint)buffer - sizeof(bmp_header_t));
#endif
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

int board_display_device_open(void)
{
	int  value = 1;
	int  ret = 0;
	struct disp_device_config config;

	__u32 auto_hpd = 0;
	__u32 err_count = 0;
	__u32 using_device_config = 0;
	unsigned long arg[4] = {0};
	int node;

	debug("De_OpenDevice\n");

	memset(&config, 0, sizeof(struct disp_device_config));

	node = fdt_path_offset(working_fdt,"boot_disp");
	if (node >= 0) {
		/* getproc output_disp,
		 * indicate which disp channel will be using
		 */
		if (fdt_getprop_u32(working_fdt, node, "output_disp",
						(uint32_t *)&screen_id) < 0) {
			pr_error("fetch script boot_disp.output_disp fail\n");
			err_count ++;
		} else
			pr_msg("boot_disp.output_disp=%d\n", screen_id);

		/* getproc output_type,
		 * indicate which kind of device will be using
		 */
		if (fdt_getprop_u32(working_fdt, node, "output_type",
						(uint32_t *)&value) < 0) {
			pr_error("fetch script boot_disp.output_type fail\n");
			err_count ++;
		} else
			pr_msg("boot_disp.output_type=%d\n", value);

		if(value == 0)
		{
			config.type = DISP_OUTPUT_TYPE_NONE;
		}
		else if(value == 1)
		{
			config.type = DISP_OUTPUT_TYPE_LCD;
		}
		else if(value == 2)
		{
			config.type = DISP_OUTPUT_TYPE_TV;
		}
		else if(value == 3)
		{
			config.type = DISP_OUTPUT_TYPE_HDMI;
		}
		else if(value == 4)
		{
			config.type = DISP_OUTPUT_TYPE_VGA;
		} else if (value == 6) {
			config.type = DISP_OUTPUT_TYPE_EDP;
		} else {
			pr_error("invalid output_type %d\n", value);
			return -1;
		}

		/* getproc output_mode,
		 * indicate which kind of mode will be output
		 */
		if (fdt_getprop_u32(working_fdt, node, "output_mode",
					(uint32_t *)&config.mode) < 0) {
			pr_error("fetch script boot_disp.output_mode fail\n");
			err_count ++;
		} else
			pr_msg("boot_disp.output_mode=%d\n", config.mode);

		/* getproc auto_hpd,
		 * indicate output device decided by the hot plug status
		 * of device
		 */
		if (fdt_getprop_u32(working_fdt, node, "auto_hpd",
						(uint32_t *)&auto_hpd) < 0) {
			pr_error("fetch script data boot_disp.auto_hpd fail\n");
			err_count ++;
		} else
			pr_msg("boot_disp.auto_hpd=%d\n", auto_hpd);

		if (config.type == DISP_OUTPUT_TYPE_HDMI) {
			/* getproc output_format, indicate output YUV or RGB */
			if (fdt_getprop_u32(working_fdt, node, "output_format",
					    (uint32_t *)&config.format) < 0) {
				pr_error("fetch script boot_disp.output_format "
					 "fail\n");
			} else {
				using_device_config = 1;
				pr_msg("boot_disp.output_format=%d\n",
				       config.format);
			}
			/* getproc output_bits, indicate output color deep */
			if (fdt_getprop_u32(working_fdt, node, "output_bits",
					    (uint32_t *)&config.bits) < 0) {
				pr_error("fetch script boot_disp.output_bits "
					 "fail\n");
			} else {
				using_device_config = 1;
				pr_msg("boot_disp.output_bits=%d\n",
				       config.bits);
			}
			/* getproc eotf, indicate output range */
			if (fdt_getprop_u32(working_fdt, node, "output_eotf",
					    (uint32_t *)&config.eotf) < 0) {
				pr_msg("fetch script boot_disp.output_eotf "
				       "fail\n");
			} else {
				using_device_config = 1;
				pr_msg("boot_disp.output_eotf=%d\n",
				       config.eotf);
			}

			/* getproc eotf, indicate output range */
			if (fdt_getprop_u32(working_fdt, node, "output_cs",
					    (uint32_t *)&config.cs) < 0) {
				pr_error(
				    "fetch script boot_disp.output_cs fail\n");
			} else {
				using_device_config = 1;
				pr_msg("boot_disp.output_cs=%d\n", config.cs);
			}
		}

	} else
		err_count = 4;

	if(err_count >= 4)//no boot_disp config
	{
		config.type = DISP_OUTPUT_TYPE_LCD;
	}
	else//has boot_disp config
	{

	}

	int hdmi_work_mode = DISP_HDMI_SEMI_AUTO;

	if (config.type == DISP_OUTPUT_TYPE_HDMI) {
		arg[0] = screen_id;
		arg[1] = config.mode;
		hdmi_work_mode = disp_ioctl(NULL, DISP_HDMI_GET_WORK_MODE, (void *)arg);
		if (hdmi_work_mode == DISP_HDMI_FULL_AUTO)
			config.mode = disp_ioctl(NULL, DISP_HDMI_GET_SUPPORT_MODE, (void *)arg);
	}

	pr_notice("disp%d device type(%d) enable\n", screen_id, config.type);

	arg[0] = screen_id;
	arg[1] = config.type;
	arg[2] = config.mode;

	disp_ioctl(NULL, DISP_DEVICE_SWITCH, (void *)arg);

	disp_para = ((config.type << 8) | (config.mode)) << (screen_id*16);
	board_display_update_para_for_kernel("boot_disp", disp_para);
	if (using_device_config == 1) {
		arg[0] = screen_id;
		arg[1] = (unsigned long)&config;

		disp_ioctl(NULL, DISP_DEVICE_SET_CONFIG, (void *)arg);

		disp_para1 = (config.cs << 16) | (config.bits << 8) |
								(config.format);
		disp_para2 = config.eotf;

		board_display_update_para_for_kernel("boot_disp1", disp_para1);
		board_display_update_para_for_kernel("boot_disp2", disp_para2);
	}

	return ret;
}

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
