#include <common.h>
#include <boot_gui.h>
#include "fb_con.h"
#include "boot_gui_config.h"

int save_disp_cmd(void)
{
	int i;
	for (i = FB_ID_0; i < FRAMEBUFFER_NUM; ++i) {
		fb_save_para(i);
	}
	return 0;
}

