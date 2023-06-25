/*
 * drivers/video/sunxi/disp2/disp/de/disp_device.c
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
#include "disp_device.h"

static LIST_HEAD(device_list);

s32 disp_device_set_manager(struct disp_device* dispdev, struct disp_manager *mgr)
{
	if ((NULL == dispdev) || (NULL == mgr)) {
		DE_WRN("NULL hdl!\n");
		return DIS_FAIL;
	}
	DE_INF("device %d, mgr %d\n", dispdev->disp, mgr->disp);

	dispdev->manager = mgr;
	mgr->device = dispdev;

	return DIS_SUCCESS;
}

s32 disp_device_swap_manager(struct disp_device *dispdev1, struct disp_device *dispdev2)
{
struct disp_manager *mgr;
	if ((NULL == dispdev1) || (NULL == dispdev2)) {
		DE_WRN("NULL hdl!\n");
		return DIS_FAIL;
	}
	DE_INF("device %p, %p swap manager\n", dispdev1, dispdev2);
	if ((NULL == dispdev1) && (NULL == dispdev2)) {
		return DIS_SUCCESS;
	} else if (dispdev1->manager == NULL) {
		dispdev1->manager = dispdev2->manager;
		dispdev2->manager->device = dispdev1;
		dispdev2->manager = NULL;
	} else if (dispdev2->manager == NULL) {
		dispdev2->manager = dispdev1->manager;
		dispdev1->manager->device = dispdev2;
		dispdev1->manager = NULL;
	} else {
		mgr = dispdev1->manager;
		dispdev1->manager = dispdev2->manager;
		dispdev2->manager = mgr;
		dispdev1->manager->device = dispdev1;
		dispdev2->manager->device = dispdev2;

	}
	return DIS_SUCCESS;
}

s32 disp_device_unset_manager(struct disp_device* dispdev)
{
	if ((NULL == dispdev)) {
		DE_WRN("NULL hdl!\n");
		return DIS_FAIL;
	}

	if (dispdev->manager)
		dispdev->manager->device = NULL;
	dispdev->manager = NULL;

	return DIS_SUCCESS;
}

s32 disp_device_get_resolution(struct disp_device* dispdev, u32 *xres, u32 *yres)
{
	if ((NULL == dispdev)) {
		DE_WRN("NULL hdl!\n");
		return DIS_FAIL;
	}

	*xres = dispdev->timings.x_res;
	*yres = dispdev->timings.y_res;

	return 0;
}

s32 disp_device_get_timings(struct disp_device* dispdev, struct disp_video_timings *timings)
{
	if ((NULL == dispdev)) {
		DE_WRN("NULL hdl!\n");
		return DIS_FAIL;
	}

	if (timings)
		memcpy(timings, &dispdev->timings, sizeof(struct disp_video_timings));

	return 0;
}

s32 disp_device_is_interlace(struct disp_device *dispdev)
{
	if ((NULL == dispdev)) {
		DE_WRN("NULL hdl!\n");
		return DIS_FAIL;
	}

	return dispdev->timings.b_interlace;
}

bool disp_device_is_in_safe_period(struct disp_device *dispdev)
{
	int cur_line;
	int start_delay;
	bool ret = true;

	if (dispdev == NULL) {
		DE_WRN("NULL hdl!\n");
		goto exit;
	}

	start_delay = disp_al_device_get_start_delay(dispdev->hwdev_index);
	cur_line = disp_al_device_get_cur_line(dispdev->hwdev_index);
	if (cur_line >= start_delay)
		ret = false;

exit:
	return ret;
}

/* get free device */
struct disp_device* disp_device_get(int disp, enum disp_output_type output_type)
{
	struct disp_device* dispdev = NULL;

	list_for_each_entry(dispdev, &device_list, list) {
		if ((dispdev->type == output_type) && (dispdev->disp == disp)
			&& (NULL == dispdev->manager)) {
			return dispdev;
		}
	}

	return NULL;
}

struct disp_device* disp_device_find(int disp, enum disp_output_type output_type)
{
	struct disp_device* dispdev = NULL;

	list_for_each_entry(dispdev, &device_list, list) {
		if ((dispdev->type == output_type) && (dispdev->disp == disp)) {
			return dispdev;
		}
	}

	return NULL;
}

struct list_head* disp_device_get_list_head(void)
{
	return (&device_list);
}


s32 disp_device_register(struct disp_device *dispdev)
{
	list_add_tail(&dispdev->list, &device_list);
	return 0;
}

s32 disp_device_unregister(struct disp_device *dispdev)
{
	list_del(&dispdev->list);
	return 0;
}

void disp_device_show_builtin_patten(struct disp_device *dispdev, u32 patten)
{
	if (dispdev)
		disp_al_show_builtin_patten(dispdev->hwdev_index, patten);
}

