#include <common.h>
#include <malloc.h>
#include <sys_config.h>
#include <fdt_support.h>

#include <sys_partition.h>
#include <tinyjpeg.h>
#include <boot_gui.h>

struct boot_fb_private {
	char *base;
	int width;
	int height;
	int bpp;
	int stride;
	int id;
};

static int read_jpeg(const char *filename,
	char *buf, unsigned int buf_size)
{
#ifndef BOOTLOGO_PARTITION_NAME
#define BOOTLOGO_PARTITION_NAME "bootloader"
#endif

#ifdef CONFIG_CMD_FAT
	return aw_fat_fsload(BOOTLOGO_PARTITION_NAME, (char *)filename,
		(char *)buf, buf_size);
#else
	int size;
	unsigned int start_block, nblock;

	start_block = sunxi_partition_get_offset_byname(BOOTLOGO_PARTITION_NAME);
	nblock = sunxi_partition_get_size_byname(BOOTLOGO_PARTITION_NAME);
	size = sunxi_flash_read(start_block, nblock, (void *)buf);
	printf("debug-%s: start_block=%d, nblock=%d, size=%dkB\n", __func__,
		start_block, nblock, size);
	return size > 0 ? (size << 10) : 0;
#endif
}

static int get_disp_fdt_node(void)
{
	static int fdt_node = -1;

	if (0 <= fdt_node)
		return fdt_node;
	/* notice: make sure we use the only one nodt "disp". */
	fdt_node = fdt_path_offset(working_fdt, "disp");
	assert(fdt_node >= 0);
	return fdt_node;
}

static void disp_getprop_by_name(int node, const char *name,
	uint32_t *value, uint32_t defval)
{
	if (fdt_getprop_u32(working_fdt, node, name, value) < 0) {
		printf("fetch script data disp.%s fail. using defval=%d\n", name, defval);
		*value = defval;
	}
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

static int request_fb(struct boot_fb_private *fb,
	unsigned int width, unsigned int height)
{
	int format = 0;
	int fbsize = 0;

	disp_getprop_by_name(get_disp_fdt_node(), "fb0_format", (uint32_t *)(&format), 0);
	if (8 == format) /* DISP_FORMAT_RGB_888 = 8; */
		fb->bpp = 24;
	else
		fb->bpp = 32;
	fb->width = width;
	fb->height = height;
	fb->stride = width * fb->bpp >> 3;
	fbsize = fb->stride * fb->height;
	fb->id = FB_ID_0;
	fb->base = (char *)malloc_buf(fb->id, fbsize);

	if (NULL != fb->base)
		memset((void *)(fb->base), 0x0, fbsize);

	return 0;
}

static int release_fb(struct boot_fb_private *fb)
{
	flush_cache((uint)fb->base, fb->stride * fb->height);
	return 0;
}

static void save_fb_para_to_kernel(struct boot_fb_private *fb)
{
	int node = get_disp_fdt_node();
	int ret = 0;
	char fb_paras[128];
	sprintf(fb_paras, "%p,%x,%x,%x,%x",
		fb->base, fb->width, fb->height, fb->bpp, fb->stride);
	ret = fdt_setprop_string(working_fdt, node, "boot_fb0", fb_paras);
	printf("fdt_setprop_string disp.boot_fb0(%s). ret-code:%s\n",
		fb_paras, fdt_strerror(ret));
}

int sunxi_load_jpeg(const char *filename)
{
	int length_of_file;
	unsigned int width, height;
	unsigned char *buf = (unsigned char *)CONFIG_SYS_SDRAM_BASE;;
	struct jdec_private *jdec;
	struct boot_fb_private fb;
	int output_format = -1;

	/* Load the Jpeg into memory */
	length_of_file = read_jpeg(filename,
		(char *)buf, SUNXI_DISPLAY_FRAME_BUFFER_SIZE);
	if (0 >= length_of_file) {
		return -1;
	}

	/* tinyjpeg: init and parse header */
	jdec = tinyjpeg_init();
	if (jdec == NULL) {
		printf("tinyjpeg_init failed\n");
		return -1;
	}
	if (tinyjpeg_parse_header(jdec, buf, (unsigned int)length_of_file) < 0) {
		printf("tinyjpeg_parse_header failed: %s\n",
			tinyjpeg_get_errorstring(jdec));
		return -1;
	}

	/* Get the size of the image, request the same size fb */
	tinyjpeg_get_size(jdec, &width, &height);
	memset((void *)&fb, 0x0, sizeof(fb));
	request_fb(&fb, width, height);
	if (NULL == fb.base) {
		printf("fb.base is null !!!");
		return -1;
	}
	tinyjpeg_set_components(jdec, (unsigned char **)&(fb.base), 1);

	if (32 == fb.bpp)
		output_format = TINYJPEG_FMT_BGRA32;
	else if (24 == fb.bpp)
		output_format = TINYJPEG_FMT_BGR24;
	if (tinyjpeg_decode(jdec, output_format) < 0) {
		printf("tinyjpeg_decode failed: %s\n", tinyjpeg_get_errorstring(jdec));
		return -1;
	}

#if 0
{
	int w, h;
	char *pdata = fb.base;
	for (h = 0; h < fb.height; ++h) {
		if ((0 == h) || (fb.height - 1 == h)) {
			for (w = 0; w < fb.width; ++w) {
				*((int *)pdata + w) = 0xFFFF0000;
			}
		} else {
			for (w = 0; w < fb.width; ++w) {
				if ((0 == w) || (fb.width - 1 == w)) {
					*((int *)pdata + w) = 0xFFFF0000;
				} else {
				}
			}
		}
		pdata += fb.stride;
	}
}
#endif

	release_fb(&fb);
	save_fb_para_to_kernel(&fb);
	free(jdec);

	return 0;
}
