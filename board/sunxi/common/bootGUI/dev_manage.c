#include "dev_manage.h"
#include "boot_gui_config.h"

int disp_devices_open(void)
{
	disp_device_t devices[DISP_DEV_NUM];
	disp_device_t *output_dev = NULL;
	int actual_dev_num = DISP_DEV_NUM;

	/* 1.get display devices list */
	output_dev = hal_get_disp_devices(devices, &actual_dev_num);
	if (NULL == output_dev)
		return -1;

	/* 2.switch the device */
	hal_switch_device(output_dev, 0); /* fixme */

#ifdef UPDATE_DISPLAY_MODE
	int init_disp = 0;
	int i = 0;
	for (i = 0; i < actual_dev_num; ++i) {
		if (0 == devices[i].opened) {
			init_disp |= (((devices[i].type << 8) | devices[i].mode)
				<< (devices[i].screen_id * 16));
		}
	}
	if (0 != init_disp)
		hal_save_int_to_kernel("init_disp", init_disp);
#endif

	return 0;
}

