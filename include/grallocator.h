#ifndef __GR_ALLOCATOR_H__
#define __GR_ALLOCATOR_H__

/* usage values for alloc */
enum {
	/* buffer will be used with the framebuffer device */
	GRALLOC_USAGE_HW_FB                 = 0x00001000,

	/* buffer should be displayed full-screen on an external display when
	* possible
	*/
	GRALLOC_USAGE_EXTERNAL_DISP         = 0x00002000,

	/* implementation-specific private usage flags */
	GRALLOC_USAGE_PRIVATE_0             = 0x10000000,
	GRALLOC_USAGE_PRIVATE_1             = 0x20000000,
	GRALLOC_USAGE_PRIVATE_2             = 0x40000000,
	GRALLOC_USAGE_PRIVATE_3             = 0x80000000,
	GRALLOC_USAGE_PRIVATE_MASK          = 0xF0000000,
};

int graphic_buffer_alloc(unsigned int w, unsigned h, unsigned int bpp,
	int usage, void **handle, unsigned int *stride);

int graphic_buffer_free(void *handle, int usage);

#endif /* #ifndef __GR_ALLOCATOR_H__ */
