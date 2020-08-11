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
#include <asm/arch/drv_display.h>
#include <sys_config.h>
#include <bmp_layout.h>
#include <fdt_support.h>

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_SUNXI_DISPLAY
static __u32 screen_id = 0;
static __u32 disp_para = 0;
extern __s32 disp_delay_ms(__u32 ms);

extern long disp_ioctl(void *hd, unsigned int cmd, void *arg);

int board_display_laner_request(void)
{
#if defined(CONFIG_VIDEO_SUNXI_V1)
	__u32 arg[4];

	arg[0] = screen_id;
	arg[1] = DISP_LAYER_WORK_MODE_NORMAL;

	gd->layer_hd = disp_ioctl(NULL, DISP_CMD_LAYER_REQUEST, (void*)arg);
	if(gd->layer_hd == 0)
	{
        tick_printf("sunxi display error : display request layer failed\n");

        return -1;
	}

	return 0;
#else
	gd->layer_hd = 0;
	return 0;
#endif
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
#if defined(CONFIG_VIDEO_SUNXI_V1)
	__u32 arg[4];

	if(gd->layer_hd == 0)
	{
        tick_printf("sunxi display error : display layer is NULL\n");

        return -1;
	}

	arg[0] = screen_id;
	arg[1] = gd->layer_hd;

	return disp_ioctl(NULL, DISP_CMD_LAYER_RELEASE, (void*)arg);
#else
	return 0;
#endif
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

	cmd = DISP_CMD_LCD_CHECK_OPEN_FINISH;

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

	cmd = DISP_CMD_LCD_CHECK_CLOSE_FINISH;
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

	cmd = DISP_CMD_SET_EXIT_MODE;
	if(lcd_off_only)
	{
		arg[0] = DISP_EXIT_MODE_CLEAN_PARTLY;
		disp_ioctl(NULL, cmd, (void *)arg);
	}
	else
	{
#if defined(CONFIG_VIDEO_SUNXI_V1)
	cmd = DISP_CMD_LCD_OFF;
#else
	cmd = DISP_CMD_LCD_DISABLE;
#endif

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
*    ´ò¿ªÍ¼²ã
*
* Parameters:
*    Layer_hd    :  input. Í¼²ã¾ä±ú
*
* Return value:
*    0  :  ³É¹¦
*   !0  :  Ê§°Ü
*
* note:
*    void
*
*******************************************************************************
*/
int board_display_layer_open(void)
{
    uint arg[4];

	arg[0] = screen_id;
	arg[1] = gd->layer_hd;
	arg[2] = 0;
	arg[3] = 0;

#if defined(CONFIG_VIDEO_SUNXI_V1)
	disp_ioctl(NULL,DISP_CMD_LAYER_OPEN,(void*)arg);
#else
	disp_ioctl(NULL,DISP_CMD_LAYER_ENABLE,(void*)arg);
#endif

    return 0;
}


/*
*******************************************************************************
*                     board_display_layer_close
*
* Description:
*    ¹Ø±ÕÍ¼²ã
*
* Parameters:
*    Layer_hd    :  input. Í¼²ã¾ä±ú
*
* Return value:
*    0  :  ³É¹¦
*   !0  :  Ê§°Ü
*
* note:
*    void
*
*******************************************************************************
*/
int board_display_layer_close(void)
{
    uint arg[4];

	arg[0] = screen_id;
	arg[1] = gd->layer_hd;
	arg[2] = 0;
	arg[3] = 0;

#if defined(CONFIG_VIDEO_SUNXI_V1)
	disp_ioctl(NULL,DISP_CMD_LAYER_CLOSE,(void*)arg);
#else
	disp_ioctl(NULL,DISP_CMD_LAYER_DISABLE,(void*)arg);
#endif

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
	arg[1] = gd->layer_hd;
	arg[2] = gd->layer_para;
	arg[3] = 0;

#if defined(CONFIG_VIDEO_SUNXI_V1)
	disp_ioctl(NULL,DISP_CMD_LAYER_SET_PARA,(void*)arg);
#else
	disp_ioctl(NULL,DISP_CMD_LAYER_SET_INFO,(void*)arg);
#endif

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
	printf("%s\n", __func__);
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
#if defined(CONFIG_VIDEO_SUNXI_V1)
	__disp_layer_info_t *layer_para;
	uint screen_width, screen_height;
	uint arg[4];

	if(!gd->layer_para)
	{
		layer_para = (__disp_layer_info_t *)malloc(sizeof(__disp_layer_info_t));
		if(!layer_para)
		{
			tick_printf("sunxi display error: unable to malloc memory for layer\n");

			return -1;
		}
	}
	else
	{
		layer_para = (__disp_layer_info_t *)gd->layer_para;
	}
	arg[0] = screen_id;
	screen_width = disp_ioctl(NULL, DISP_CMD_SCN_GET_WIDTH, (void*)arg);
	screen_height = disp_ioctl(NULL, DISP_CMD_SCN_GET_HEIGHT, (void*)arg);
	debug("screen_width =%d, screen_height =%d\n", screen_width, screen_height);
	memset((void *)layer_para, 0, sizeof(__disp_layer_info_t));
	layer_para->fb.addr[0]		= (uint)buffer;
	debug("frame buffer address %x\n", (uint)buffer);
	layer_para->fb.size.width	= width;
	layer_para->fb.size.height	= height;
	layer_para->fb.mode			= DISP_MOD_INTERLEAVED;
	layer_para->fb.format		= (bitcount == 24)? DISP_FORMAT_RGB888:DISP_FORMAT_ARGB8888;
	debug("bitcount = %d\n", bitcount);
	layer_para->fb.br_swap		= 0;
	layer_para->fb.seq			= DISP_SEQ_ARGB;
	layer_para->fb.b_trd_src 	= 0;
	layer_para->fb.trd_mode		= 0;
	layer_para->ck_enable		= 0;
	layer_para->mode            = DISP_LAYER_WORK_MODE_NORMAL;
	layer_para->alpha_en 		= 1;
	layer_para->alpha_val		= 0xff;
	layer_para->pipe 			= 0;
	layer_para->src_win.x		= 0;
	layer_para->src_win.y		= 0;
	layer_para->src_win.width	= width;
	layer_para->src_win.height	= height;
	layer_para->scn_win.x		= (screen_width - width) / 2;
	layer_para->scn_win.y		= (screen_height - height) / 2;
	layer_para->scn_win.width	= width;
	layer_para->scn_win.height	= height;
	layer_para->b_trd_out		= 0;
	layer_para->out_trd_mode 	= 0;

#else
	disp_layer_info *layer_para;
	uint screen_width, screen_height;
	uint arg[4];

	if(!gd->layer_para)
	{
		layer_para = (disp_layer_info *)malloc(sizeof(disp_layer_info));
		if(!layer_para)
		{
			tick_printf("sunxi display error: unable to malloc memory for layer\n");

			return -1;
		}
	}
	else
	{
		layer_para = (disp_layer_info *)gd->layer_para;
	}
	arg[0] = screen_id;
	screen_width = disp_ioctl(NULL, DISP_CMD_GET_SCN_WIDTH, (void*)arg);
	screen_height = disp_ioctl(NULL, DISP_CMD_GET_SCN_HEIGHT, (void*)arg);
	printf("screen_id =%d, screen_width =%d, screen_height =%d\n", screen_id, screen_width, screen_height);
	memset((void *)layer_para, 0, sizeof(disp_layer_info));
	layer_para->fb.addr[0]		= (uint)buffer;
	debug("frame buffer address %x\n", (uint)buffer);
	layer_para->fb.format		= (bitcount == 24)? DISP_FORMAT_RGB_888:DISP_FORMAT_ARGB_8888;
	layer_para->fb.size.width	= width;
	layer_para->fb.size.height	= height;
	debug("bitcount = %d\n", bitcount);
	layer_para->fb.b_trd_src 	= 0;
	layer_para->fb.trd_mode		= 0;
	layer_para->ck_enable		= 0;
	layer_para->mode            = DISP_LAYER_WORK_MODE_NORMAL;
	layer_para->alpha_mode 		= 1;
	layer_para->alpha_value		= 0xff;
	layer_para->pipe 			= 0;
	layer_para->screen_win.x		= (screen_width - width) / 2;
	layer_para->screen_win.y		= (screen_height - height) / 2;
	layer_para->screen_win.width	= width;
	layer_para->screen_win.height	= height;
	layer_para->b_trd_out		= 0;
	layer_para->out_trd_mode 	= 0;
#endif

	gd->layer_para = (uint)layer_para;

	return 0;
}

void board_display_set_alpha_mode(int mode)
{
    if(!gd->layer_para)
    {
        return;
    }
#if defined(CONFIG_VIDEO_SUNXI_V1)
    __disp_layer_info_t *layer_para;
    layer_para = (__disp_layer_info_t *)gd->layer_para;
    layer_para->alpha_en 		= mode;
#else
    disp_layer_info *layer_para;
    layer_para = (disp_layer_info *)gd->layer_para;
    layer_para->alpha_mode 		= mode;
#endif
}

int board_display_framebuffer_change(void *buffer)
{
#if defined(CONFIG_VIDEO_SUNXI_V1)
    uint arg[4];
	__disp_fb_t disp_fb;
	__disp_layer_info_t *layer_para = (__disp_layer_info_t *)gd->layer_para;

	arg[0] = screen_id;
	arg[1] = gd->layer_hd;
	arg[2] = (uint)&disp_fb;
	arg[3] = 0;

	if(disp_ioctl(NULL, DISP_CMD_LAYER_GET_FB, (void*)arg))
	{
		tick_printf("sunxi display error :get framebuffer failed\n");

		return -1;
	}
	disp_fb.addr[0] = (uint)buffer;
	arg[0] = screen_id;
    arg[1] = gd->layer_hd;
    arg[2] = (uint)&disp_fb;
    arg[3] = 0;
	//debug("try to set framebuffer %x\n", (uint)buffer);
    if(disp_ioctl(NULL, DISP_CMD_LAYER_SET_FB, (void*)arg))
    {
        tick_printf("sunxi display error :set framebuffer failed\n");

		return -1;
	}
	layer_para->fb.addr[0] = (uint)buffer;

	return 0;
#else
	return 0;
#endif
}

int board_display_device_open(void)
{
	int  value = 1;
	int  ret = 0;
	__u32 output_type = 0;
	__u32 output_mode = 0;
	unsigned long arg[4] = {0};
	__u32 auto_hpd = 0;
	__u32 err_count = 0;
	int node;

	debug("De_OpenDevice\n");

	node = fdt_path_offset(working_fdt,"boot_disp");
	if (node >= 0) {
		/* getproc output_disp, indicate which disp channel will be using */
		if (fdt_getprop_u32(working_fdt, node, "output_disp", (uint32_t*)&screen_id) < 0) {
			printf("fetch script data boot_disp.output_disp fail\n");
			err_count ++;
		} else
			printf("boot_disp.output_disp=%d\n", screen_id);

		/* getproc output_type, indicate which kind of device will be using */
		if (fdt_getprop_u32(working_fdt, node, "output_type", (uint32_t*)&value) < 0) {
			printf("fetch script data boot_disp.output_type fail\n");
			err_count ++;
		} else
			printf("boot_disp.output_type=%d\n", value);

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
			printf("invalid output_type %d\n", value);
			return -1;
		}

		/* getproc output_mode, indicate which kind of mode will be output */
		if (fdt_getprop_u32(working_fdt, node, "output_mode", (uint32_t*)&output_mode) < 0) {
			printf("fetch script data boot_disp.output_mode fail\n");
			err_count ++;
		} else
			printf("boot_disp.output_mode=%d\n", output_mode);

		/* getproc auto_hpd, indicate output device decided by the hot plug status of device */
		if (fdt_getprop_u32(working_fdt, node, "auto_hpd", (uint32_t*)&auto_hpd) < 0) {
			printf("fetch script data boot_disp.auto_hpd fail\n");
			err_count ++;
		 } else
			printf("boot_disp.auto_hpd=%d\n", auto_hpd);
	} else
		err_count = 4;

	if(err_count >= 4)//no boot_disp config
	{
		output_type = DISP_OUTPUT_TYPE_LCD;
	}
	else//has boot_disp config
	{

	}
	printf("disp%d device type(%d) enable\n", screen_id, output_type);

	if(output_type == DISP_OUTPUT_TYPE_LCD)
	{
		debug("lcd open\n");
		arg[0] = screen_id;
		arg[1] = 0;
		arg[2] = 0;
#if defined(CONFIG_VIDEO_SUNXI_V1)
		ret = disp_ioctl(NULL, DISP_CMD_LCD_ON, (void*)arg);
#else
		ret = disp_ioctl(NULL, DISP_CMD_LCD_ENABLE, (void*)arg);
#endif
		debug("lcd open,ret=%d\n",ret);
		disp_para = ((output_type << 8) | (output_mode)) << (screen_id*16);
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
#if defined(CONFIG_VIDEO_SUNXI_V1)
	return disp_ioctl(NULL, DISP_CMD_SCN_GET_WIDTH, (void*)arg);
#else
	return disp_ioctl(NULL, DISP_CMD_GET_SCN_WIDTH, (void*)arg);
#endif
}

int borad_display_get_screen_height(void)
{
	unsigned long arg[4] = {0};

	arg[0] = screen_id;
#if defined(CONFIG_VIDEO_SUNXI_V1)
	return disp_ioctl(NULL, DISP_CMD_SCN_GET_HEIGHT, (void*)arg);
#else
	return disp_ioctl(NULL, DISP_CMD_GET_SCN_HEIGHT, (void*)arg);
#endif
}

#else
int board_display_layer_request(void)
{
	return 0;
}

int board_display_layer_release(void)
{
	return 0;
}
int board_display_wait_lcd_open(void)
{
	return 0;
}
int board_display_wait_lcd_close(void)
{
	return 0;
}
int board_display_set_exit_mode(int lcd_off_only)
{
	return 0;
}
int board_display_layer_open(void)
{
	return 0;
}

int board_display_layer_close(void)
{
	return 0;
}

int board_display_layer_para_set(void)
{
	return 0;
}

int board_display_show_until_lcd_open(int display_source)
{
	return 0;
}

int board_display_show(int display_source)
{
	return 0;
}

int board_display_framebuffer_set(int width, int height, int bitcount, void *buffer)
{
	return 0;
}

void board_display_set_alpha_mode(int mode)
{
	return ;
}

int board_display_framebuffer_change(void *buffer)
{
	return 0;
}
int board_display_device_open(void)
{
	return 0;
}

int borad_display_get_screen_width(void)
{
	return 0;
}

int borad_display_get_screen_height(void)
{
	return 0;
}

void board_display_setenv(char *data)
{
	return;
}

#endif
