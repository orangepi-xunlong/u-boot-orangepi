/*
 * drivers/video/drm/drm_framebuffer/drm_framebuffer.c
 *
 * Copyright (c) 2007-2024 Allwinnertech Co., Ltd.
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
#include <common.h>
#include <drm/drm_modes.h>
#include <drm/drm_framebuffer.h>
#include "sunxi_drm_drv.h"
#include <drm/drm_fourcc.h>
#include <drm/drm_util.h>



static int fb_plane_width(int width,
			  const struct drm_format_info *format, int plane)
{
	if (plane == 0)
		return width;

	return DIV_ROUND_UP(width, format->hsub);
}

static int fb_plane_height(int height,
			   const struct drm_format_info *format, int plane)
{
	if (plane == 0)
		return height;

	return DIV_ROUND_UP(height, format->vsub);
}


/**
 * drm_framebuffer_plane_width - width of the plane given the first plane
 * @width: width of the first plane
 * @fb: the framebuffer
 * @plane: plane index
 *
 * Returns:
 * The width of @plane, given that the width of the first plane is @width.
 */
int drm_framebuffer_plane_width(int width,
				const struct drm_framebuffer *fb, int plane)
{
	if (plane >= fb->format->num_planes)
		return 0;

	return fb_plane_width(width, fb->format, plane);
}


/**
 * drm_framebuffer_plane_height - height of the plane given the first plane
 * @height: height of the first plane
 * @fb: the framebuffer
 * @plane: plane index
 *
 * Returns:
 * The height of @plane, given that the height of the first plane is @height.
 */
int drm_framebuffer_plane_height(int height,
				 const struct drm_framebuffer *fb, int plane)
{
	if (plane >= fb->format->num_planes)
		return 0;

	return fb_plane_height(height, fb->format, plane);
}


static int framebuffer_check(const struct drm_mode_fb_cmd2 *r, const struct drm_format_info *info)
{
	int i;

	if (!info) {
		pr_err("bad framebuffer format %p4cc\n",
			    &r->pixel_format);
		return -EINVAL;
	}

	if (r->width == 0) {
		pr_err("bad framebuffer width %u\n", r->width);
		return -EINVAL;
	}

	if (r->height == 0) {
		pr_err("bad framebuffer height %u\n", r->height);
		return -EINVAL;
	}

	for (i = 0; i < info->num_planes; i++) {
		unsigned int width = fb_plane_width(r->width, info, i);
		unsigned int height = fb_plane_height(r->height, info, i);
		unsigned int block_size = info->char_per_block[i];
		u64 min_pitch = drm_format_info_min_pitch(info, i, width);

		if (!block_size && (r->modifier[i] == DRM_FORMAT_MOD_LINEAR)) {
			pr_err("Format requires non-linear modifier for plane %d\n", i);
			return -EINVAL;
		}


		if (min_pitch > UINT_MAX)
			return -ERANGE;

		if ((uint64_t) height * r->pitches[i] + r->offsets[i] > UINT_MAX)
			return -ERANGE;

		if (block_size && r->pitches[i] < min_pitch) {
			pr_err("bad pitch %u for plane %d\n", r->pitches[i], i);
			return -EINVAL;
		}

		if (r->modifier[i] && !(r->flags & DRM_MODE_FB_MODIFIERS)) {
			pr_err("bad fb modifier %llu for plane %d\n",
				    r->modifier[i], i);
			return -EINVAL;
		}

		if (r->flags & DRM_MODE_FB_MODIFIERS &&
		    r->modifier[i] != r->modifier[0]) {
			pr_err("bad fb modifier %llu for plane %d\n",
				    r->modifier[i], i);
			return -EINVAL;
		}

		/* modifier specific checks: */
		switch (r->modifier[i]) {
		case DRM_FORMAT_MOD_SAMSUNG_64_32_TILE:
			/* NOTE: the pitch restriction may be lifted later if it turns
			 * out that no hw has this restriction:
			 */
			if (r->pixel_format != DRM_FORMAT_NV12 ||
					width % 128 || height % 32 ||
					r->pitches[i] % 128) {
				pr_err("bad modifier data for plane %d\n", i);
				return -EINVAL;
			}
			break;

		default:
			break;
		}
	}

	for (i = info->num_planes; i < 4; i++) {
		if (r->modifier[i]) {
			pr_err("non-zero modifier for unused plane %d\n", i);
			return -EINVAL;
		}

		/* Pre-FB_MODIFIERS userspace didn't clear the structs properly. */
		if (!(r->flags & DRM_MODE_FB_MODIFIERS))
			continue;


		if (r->pitches[i]) {
			pr_err("non-zero pitch for unused plane %d\n", i);
			return -EINVAL;
		}

		if (r->offsets[i]) {
			pr_err("non-zero offset for unused plane %d\n", i);
			return -EINVAL;
		}
	}

	return 0;
}

#define MAX_IDS 10

struct id_manager {
	int ids[MAX_IDS];
	int count;
};

static struct id_manager g_id_mgr;

static int id_manager_alloc(struct id_manager *mgr)
{
	int i;
	for (i = 0; i < MAX_IDS; i++) {
		if (mgr->ids[i] == 0) {
			mgr->ids[i] = 1; // id has been allocated
			mgr->count++;
			return i; // id
		}
	}
	return -1;
}

static void id_manager_free(struct id_manager *mgr, int id)
{
	if (id >= 0 && id < MAX_IDS && mgr->ids[id] == 1) {
		mgr->ids[id] = 0; // release id
		mgr->count--;
	}
}

int drm_framebuffer_alloc(struct sunxi_drm_device *drm, struct drm_mode_fb_cmd2 *r)
{
	struct drm_framebuffer *fb = NULL;
	int ret = -1, i = 0;
	const struct drm_format_info *info = NULL;
	u32 cpp = 0;

	if (!drm || !r) {
		return -EINVAL;
	}

	fb = kmalloc(sizeof(struct drm_framebuffer), __GFP_ZERO);
	if (!fb) {
		return -ENOMEM;
	}

	fb->fb_id = id_manager_alloc(&g_id_mgr);
	if (fb->fb_id < 0) {
		pr_err("id_manager_alloc fail!\n");
		goto FREE_FB;
	}

	/* check if the format is supported at all */
	info = __drm_format_info(r->pixel_format);
	//check format support
	ret = framebuffer_check(r, info);
	if (ret)
		goto FREE_FB;

	cpp = DIV_ROUND_UP(info->depth, 8);

	fb->buf_size = ALIGN(cpp * r->width * r->height, 4096);

	fb->dma_addr = (unsigned long)memalign(4096, fb->buf_size);
	if (!fb->dma_addr) {
		ret = -ENOMEM;
		goto FREE_FB;
	}

	fb->format = drm_format_info(r->pixel_format);
	fb->width = r->width;
	fb->height = r->height;
	for (i = 0; i < 4; i++) {
		fb->pitches[i] = r->pitches[i];
		fb->offsets[i] = r->offsets[i];
	}
	fb->modifier = r->modifier[0];
	fb->flags = r->flags;
	fb->drm = drm;
	r->pitches[0] = DIV_ROUND_UP(r->width * info->depth, 8);
	INIT_LIST_HEAD(&fb->head);
	list_add_tail(&fb->head, &drm->fb_list);
	++drm->num_fb;
	return fb->fb_id;

FREE_FB:
	if (fb) {
		kfree(fb);
	}

	printf("test\n");

	return ret;

}


int drm_framebuffer_free(struct sunxi_drm_device *drm, struct drm_framebuffer *fb)
{
	if (!drm || !fb) {
		return -EINVAL;
	}

	list_del(&fb->head);
	if (fb->dma_addr) {
		kfree((void *)fb->dma_addr);
	}
	id_manager_free(&g_id_mgr, fb->fb_id);

	kfree(fb);
	return 0;
}

#define sunxi_drm_for_each_fb_id(fb, drm, id) \
	list_for_each_entry(fb, &(drm)->fb_list, head) \
	for_each_if(id == (fb)->fb_id)

#define sunxi_drm_for_each_fb(fb, drm) \
	list_for_each_entry(fb, &(drm)->fb_list, head)


struct drm_framebuffer *drm_framebuffer_lookup(struct sunxi_drm_device *drm, int fb_id)
{
	struct drm_framebuffer *fb;

	sunxi_drm_for_each_fb_id(fb, drm, fb_id) {
		return fb;
	}

	return NULL;
}


