#include <common.h>
#include <malloc.h>
#include "fb_con.h"
#include "canvas_utils.h"
#include "video_hal.h"
#include "boot_gui_config.h"

#define BUF_WIDTH_ALIGN_BYTE 4
#define BUF_WIDTH_ALIGN_BIT (BUF_WIDTH_ALIGN_BYTE << 3)
#define MY_ALIGN(x, a)   (((x) + (a) - 1L) & ~((a) - 1L))

framebuffer_t g_fb_list[FRAMEBUFFER_NUM];

/*
* @return: 0 mean success of setting fb crop, graphic produtor
*    need not set background color. Otherwies !0 mean failure of
*    setting fb crop, so graphic produtor should set background color.
*/
static int set_interest_region(struct canvas *cv,
	rect_t *rects, unsigned int count, argb_t *uninterest_region_color)
{
	framebuffer_t *fb = NULL;

	if ((NULL == cv) || (NULL == rects)) {
		printf("%s: cv=%p, rects=%p\n", __func__, cv, rects);
		return -1;
	}

	fb = (framebuffer_t *)(cv->this_fb);
	if ((NULL != fb) && (FB_LOCKED == fb->locked)) {
		/* deal one rect so far. */
		fb->interest_rect.left = rects->left;
		fb->interest_rect.top = rects->top;
		fb->interest_rect.right = rects->right;
		fb->interest_rect.bottom = rects->bottom;
#ifdef SUPORT_SET_FB_CROP
#error "FIXME: not verify yet"
		if ((0 != rects->left)
			|| (0 != rects->top)
			|| (cv->width != rects->right)
			|| (cv->height != rects->bottom))
			return hal_set_layer_crop(fb->handle, rects->left,
				rects->top, rects->right, rects->bottom);
		else
			return 0;
#else
		return -1;
#endif
	}
	printf("%s: fb=%p, locked=%d\n", __func__,
		fb, (NULL != fb) ? fb->locked : 0xFF);
	return -1;
}

static inline char valid_fb_cfg(fb_config_t *fb_cfg)
{
	return ((0 < fb_cfg->width)
		&& (0 < fb_cfg->height)
		&& (0 < fb_cfg->bpp));
}

static void *malloc_buf(unsigned int fb_id, const unsigned int size)
{
#if !defined(FB_NOT_USE_TOP_MEM)
	DECLARE_GLOBAL_DATA_PTR;
	printf("fb_id=%d, size=%d, gd->ram_size=%ld, SUNXI_DISPLAY_FRAME_BUFFER_SIZE=%d\n",
		fb_id, size, gd->ram_size, SUNXI_DISPLAY_FRAME_BUFFER_SIZE);
	if ((FB_ID_0 == fb_id)
		&& (size < gd->ram_size)
		&& (size <= SUNXI_DISPLAY_FRAME_BUFFER_SIZE))
		return (void *)(CONFIG_SYS_SDRAM_BASE + gd->ram_size
			- SUNXI_DISPLAY_FRAME_BUFFER_SIZE);
#endif

#if defined(SUNXI_DISPLAY_FRAME_BUFFER_ADDR)
	if (FB_ID_0 == fb_id) {
		printf("SUNXI_DISPLAY_FRAME_BUFFER_ADDR=0x%x\n",
			SUNXI_DISPLAY_FRAME_BUFFER_ADDR);
		return (void *)SUNXI_DISPLAY_FRAME_BUFFER_ADDR;
	}
#endif

	return malloc(size);
}

static void free_buf(unsigned int fb_id, void *p)
{
#if (!defined(FB_NOT_USE_TOP_MEM) || defined(SUNXI_DISPLAY_FRAME_BUFFER_ADDR))
	if (FB_ID_0 == fb_id)
		return;
#endif
	free(p);
}

static void wait_for_sync_buf(int *release_fence)
{
#ifdef BOOT_GUI_DOUBLE_BUF
	/*
	* release fence of showing buf should was assigned as 0
	*  when it has been finished displaying.
	* Todo: clear release fence in interupt of DE.
	*/
	int wait_time = 3 * 1000 * 1000; /* wait 3s */
	while ((0 != *release_fence) && (wait_time--))
		__usdelay(1);
	if (0 >= wait_time)
		printf("%s: timeout\n", __func__);
#endif
}

static void update_dirty_rect(framebuffer_t *fb)
{
#ifdef BOOT_GUI_DOUBLE_BUF
	/* update dirty rect of drawing buf from "the other buf" */
	rect_t *src_dirty = &(fb->buf_list->next->dirty_rect);
	int stride = fb->cv->stride;
	int offset = stride * src_dirty->top + fb->cv->bpp * src_dirty->left;
	int cp_bytes = fb->cv->bpp * (src_dirty->right - src_dirty->left) >> 3;

	char *p_dst = (char *)(fb->buf_list->addr) + offset;
	char *p_dst_e = p_dst + stride * (src_dirty->bottom - src_dirty->top);

	char *src_addr = (char *)(fb->buf_list->next->addr) + offset;

	for (; p_dst != p_dst_e; p_dst += stride) {
		memcpy((void *)p_dst, (void *)src_addr, cp_bytes);
		src_addr += stride;
	}
	flush_cache((uint)(fb->buf_list->addr), stride * fb->cv->height);
#endif
}

static void commit_fb(framebuffer_t *const fb, int flag)
{
#ifdef BOOT_GUI_DOUBLE_BUF
	/*
	* comit the drawing buf to DE by reset address of FB.
	* and then set the release fence of buffer as 1 .
	* Todo: fb->buf_list->release_fence = 1;
	*/
	if (FB_COMMIT_ADDR & flag)
		hal_set_layer_addr(fb->handle, (void *)fb->cv->base);
#else
	hal_set_layer_addr(fb->handle, (void *)fb->cv->base);
#endif
}

static void switch_buf(framebuffer_t *const fb, rect_t *dirty_rects, int count)
{
#ifdef BOOT_GUI_DOUBLE_BUF
	/*
	* we had comitted the drawing-buf yet.
	* so we switch to point to the next buf.
	* we just support one dirty rect so far.
	*/
	if (0 != count) {
		if (NULL != dirty_rects) {
			memcpy((void *)&(fb->buf_list->dirty_rect),
				(void *)dirty_rects, sizeof(*dirty_rects));
		} else {
			rect_t *dirty = &(fb->buf_list->dirty_rect);
			dirty->left = 0;
			dirty->top = 0;
			dirty->right = fb->cv->width;
			dirty->bottom = fb->cv->height;
		}
	}
	fb->buf_list = fb->buf_list->next;
#endif
}

/* creat_framebuffer <--> destroy_framebuffer */
static int creat_framebuffer(framebuffer_t *fb, const fb_config_t *const fb_cfg)
{
	int buf_size = 0;
	int buf_num = 0;
	void *buf_addr = NULL;

	fb->cv = (struct canvas *)malloc(sizeof(*(fb->cv)));
	if (!fb->cv) {
		printf("malloc for canvas failed !\n");
		return -1;
	}

#ifndef BOOT_GUI_DOUBLE_BUF
	buf_num = 1;
	fb->buf_list = (struct buf_node *)malloc(sizeof(*(fb->buf_list)));
#else
	buf_num = 2;
	fb->buf_list = (struct buf_node *)malloc(sizeof(*(fb->buf_list)) << 1);
#endif
	if (!fb->buf_list) {
		printf("malloc for buf_list failed !\n");
		goto free_cv;
	}

	memset((void *)fb->buf_list, 0, sizeof(*(fb->buf_list)) * buf_num);
	memset((void *)fb->cv, 0, sizeof(*(fb->cv)));
	fb->cv->width = fb_cfg->width;
	fb->cv->height = fb_cfg->height;
	if (32 == fb_cfg->bpp) {
		fb->cv->bpp = 32;
		fb->cv->pixel_format_name = "ARGB8888";
	} else if (24 == fb_cfg->bpp) {
		fb->cv->bpp = 24;
		fb->cv->pixel_format_name = "RGB888";
	} else {
		fb->cv->bpp = 32;
		fb->cv->pixel_format_name = "ARGB8888";
		printf("no support this bpp=%d\n", fb_cfg->bpp);
	}
	fb->cv->stride = MY_ALIGN(fb->cv->width * fb->cv->bpp, BUF_WIDTH_ALIGN_BIT) >> 3;
	buf_size = fb->cv->stride * fb->cv->height;
	buf_addr = malloc_buf(fb->fb_id, buf_size * buf_num);
	if (!buf_addr) {
		printf("malloc buf_addr failed ! alloc size %d\n", buf_size * buf_num);
		goto free_buf_list;
	}
	fb->buf_list->buf_size = buf_size;
	fb->buf_list->addr = buf_addr;
	if (1 == buf_num) {
		fb->buf_list->next = fb->buf_list;
	} else {
		fb->buf_list->next = &fb->buf_list[1];
		fb->buf_list->next->next = fb->buf_list;
		fb->buf_list->next->buf_size = buf_size;
		fb->buf_list->next->addr = (char *)fb->buf_list->addr + buf_size;
	}
	fb->cv->base = (unsigned char *)buf_addr;

	get_canvas_utils(fb->cv);
	fb->cv->set_interest_region = set_interest_region;

	fb->cv->this_fb = (void *)fb;

	return 0;

/*
*free_buf_addr:
*	free_buf(fb->fb_id, buf_addr);
*/
free_buf_list:
	free(fb->buf_list);
	fb->buf_list = NULL;

free_cv:
	free(fb->cv);
	fb->cv = NULL;

	return -1;
}

static int destroy_framebuffer(framebuffer_t *fb)
{
#ifdef BOOT_GUI_DOUBLE_BUF
	if (fb->buf_list > fb->buf_list->next)
		fb->buf_list = fb->buf_list->next;
#endif
	free_buf(fb->fb_id, fb->buf_list->addr);
	free(fb->buf_list);
	free(fb->cv);
	fb->buf_list = NULL;
	fb->cv = NULL;
	fb->fb_id = FB_ID_INVALID;
	return 0;
}

/* setup_framebuffer <--> hal_release_layer */
static int setup_framebuffer(framebuffer_t *const fb)
{
	fb->handle = hal_request_layer(fb->fb_id);
	if (NULL == fb->handle)
		return -1;

	hal_set_layer_geometry(fb->handle, fb->cv->width, fb->cv->height,
		fb->cv->bpp, BUF_WIDTH_ALIGN_BYTE);

	hal_show_layer(fb->handle, 1);

	return 0;
}

int fb_init(void)
{
	int fb_num = FRAMEBUFFER_NUM;
	fb_config_t fb_cfg;
	int i = 0;

	hal_get_fb_configs(&fb_cfg, &fb_num);
	for (i = 0; i < fb_num; ++i) {
		if (valid_fb_cfg(&fb_cfg)) {
			memset((void *)&(g_fb_list[i]), 0, sizeof(g_fb_list[0]));
			g_fb_list[i].fb_id = i;
			if (creat_framebuffer(&g_fb_list[i], &fb_cfg)) {
				g_fb_list[i].fb_id = FB_ID_INVALID;
				continue;
			}
			setup_framebuffer(&g_fb_list[i]);
		} else {
			printf("bad fb_cfg[w=%d,h=%d,bpp=%d,format=%d]\n",
				fb_cfg.width, fb_cfg.height, fb_cfg.bpp, fb_cfg.format_cfg);
			g_fb_list[i].fb_id = FB_ID_INVALID;
		}
	}

	return fb_num;
}

int fb_quit(void)
{
	int i;
	for (i = 0; i < sizeof(g_fb_list) / sizeof(g_fb_list[0]); ++i) {
		if (FB_ID_INVALID != g_fb_list[i].fb_id) {
			hal_release_layer(g_fb_list[i].fb_id, g_fb_list[i].handle);
			destroy_framebuffer(&g_fb_list[i]);
		}
	}
	return 0;
}

struct canvas *fb_lock(const unsigned int fb_id)
{
	framebuffer_t *fb = &g_fb_list[fb_id];
	if ((fb_id < FRAMEBUFFER_NUM) && (fb_id == fb->fb_id)) {
		if (FB_LOCKED == fb->locked) {
			printf("somebody locked the fb%d\n", fb_id);
			return NULL;
		}
		fb->locked = FB_LOCKED;
		wait_for_sync_buf(&(fb->buf_list->release_fence));
		update_dirty_rect(fb);
		fb->cv->base = (unsigned char *)fb->buf_list->addr;
	} else {
		printf("%s: bad fb_id=%d\n", __func__, fb_id);
		return NULL;
	}
	return fb->cv;
}

int fb_unlock(unsigned int fb_id, rect_t *dirty_rects, int count)
{
	framebuffer_t *fb = &g_fb_list[fb_id];
	if ((fb_id < FRAMEBUFFER_NUM) && (fb_id == fb->fb_id)) {
		if (FB_LOCKED != fb->locked) {
			printf("no one locked the fb%d\n", fb_id);
			return -1;
		}
		if (0 != count) {
			flush_cache((uint)fb->cv->base, fb->cv->stride * fb->cv->height);
			commit_fb(fb, FB_COMMIT_ADDR);
			switch_buf(fb, dirty_rects, count);
		}
		fb->locked = FB_UNLOCKED;
	} else {
		printf("%s: bad fb_id=%d\n", __func__, fb_id);
		return -1;
	}
	return 0;
}

/*
* fb should be being unlocked when this function is called.
*/
int fb_save_para(unsigned int fb_id)
{
	framebuffer_t *fb = &g_fb_list[fb_id];
	char name[16] = {0};
	char fb_paras[128] = {0};
	rect_t *interest_rect = NULL;

	if ((FRAMEBUFFER_NUM <= fb_id)
		|| (fb_id != fb->fb_id)
		|| (FB_LOCKED == fb->locked)) {
		printf("%s: fb_id[%d,%d], locked=%d\n", __func__, fb_id,
			(FRAMEBUFFER_NUM > fb_id) ? fb->fb_id : FB_ID_INVALID,
			(FRAMEBUFFER_NUM > fb_id) ? fb->locked : FB_ID_INVALID);
		return -1;
	}

	interest_rect = &(fb->interest_rect);
	sprintf(name, "boot_fb%d", fb_id);
	sprintf(fb_paras, "%p,%x,%x,%x,%x,%x,%x,%x,%x",
		fb->cv->base, fb->cv->width, fb->cv->height, fb->cv->bpp, fb->cv->stride,
		interest_rect->left, interest_rect->top, interest_rect->right, interest_rect->bottom);
	hal_save_string_to_kernel(name, fb_paras);
	return 0;
}

