/*
 * Allwinner SoCs grallocator.
 *
 * Copyright (C) 2017 Allwinner.
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <common.h>
#include <malloc.h>
#include "grallocator.h"

#define DO_ALIGN(v, a) (((v) + (a) - 1L) & ~((a) - 1L))
#define GR_ALIGN_BYTE 4

int graphic_buffer_alloc(unsigned int w, unsigned h, unsigned int bpp,
	int usage, void **handle, unsigned int *stride)
{
	unsigned int size;
	void *addr = NULL;

	if (!w || !h)
		w = h = 1;

	*stride = DO_ALIGN(w * bpp >> 3, GR_ALIGN_BYTE);
	size = *stride * h;

	if (GRALLOC_USAGE_HW_FB & usage) {
		/* only for FB_ID_0 */
#if !defined(HW_FB_ADDR)
		DECLARE_GLOBAL_DATA_PTR;
		debug("size=0x%x, ram_size=0x%lx, FRAME_BUFFER_SIZE=0x%x\n",
			size, gd->ram_size, SUNXI_DISPLAY_FRAME_BUFFER_SIZE);
		if ((size < gd->ram_size)
			&& (size <= SUNXI_DISPLAY_FRAME_BUFFER_SIZE))
			addr = (void *)(CONFIG_SYS_SDRAM_BASE + gd->ram_size
				- SUNXI_DISPLAY_FRAME_BUFFER_SIZE);
#else
		debug("HW_FB_ADDR=0x%x\n", HW_FB_ADDR);
		addr = (void *)HW_FB_ADDR;
#endif

	} else {
		addr = (void *)malloc(size);
	}

	if (NULL != addr) {
		*handle = addr;
		return 0;
	} else {
		pr_msg("%s: failed\n", __func__);
		return -1;
	}
}

int graphic_buffer_free(void *handle, int usage)
{
	if (!(GRALLOC_USAGE_HW_FB & usage)) {
		free(handle);
	}
	return 0;
}
