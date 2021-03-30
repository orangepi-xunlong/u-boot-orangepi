#ifndef __FB_CON_H__
#define __FB_CON_H__

#include "boot_gui.h"

enum {
	FB_UNLOCKED = 0x0,
	FB_LOCKED,
};

enum {
	FB_COMMIT_ADDR = 0x00000001,
	FB_COMMIT_GEOMETRY = 0x00000002,
};

struct buf_node {
	void *addr;
	int buf_size;
	int release_fence;
	rect_t dirty_rect;
	struct buf_node *next;
};

typedef struct framebuffer {
	unsigned int fb_id;
	int locked;
	rect_t interest_rect;
	struct canvas *cv;
	struct buf_node *buf_list;
	void *handle;
} framebuffer_t;

int fb_save_para(unsigned int fb_id);

#endif /* #ifndef __FB_CON_H__ */
