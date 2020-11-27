#ifndef __CANVAS_UTILS_H__
#define __CANVAS_UTILS_H__

#ifdef BOOT_GUI_FONT_8X16
#undef CONFIG_VIDEO_FONT_4X6
#else
#define CONFIG_VIDEO_FONT_4X6
#endif

int get_canvas_utils(struct canvas *cv);

#endif /* #ifndef __CANVAS_UTILS_H__ */
