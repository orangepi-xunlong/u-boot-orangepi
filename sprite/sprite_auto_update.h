#ifndef  __SPRITE_AUTO_UPDATE_H__
#define  __SPRITE_AUTO_UPDATE_H__

static inline void *malloc_aligned(u32 size, u32 align)
{
	return (void *)(((u32)malloc(size + align) + align - 1) &
			~(align - 1));
}

extern int fat_fs_read(const char *filename, void *addr, int offset, int len);

#endif	/* __SPRITE_AUTO_UPDATE_H__ */
