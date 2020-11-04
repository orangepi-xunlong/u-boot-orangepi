
#ifndef __SUNXI_SPRITE_SPARSE_H__
#define __SUNXI_SPRITE_SPARSE_H__

#define ANDROID_FORMAT_UNKNOW (0)
#define ANDROID_FORMAT_BAD (-1)
#define ANDROID_FORMAT_DETECT (1)

extern int unsparse_probe(char *source, unsigned int length,
			  unsigned int flash_start);
extern int unsparse_direct_write(void *pbuf, unsigned int length);
extern unsigned int unsparse_checksum(void);

#endif /* __SUNXI_SPRITE_SPARSE_H__ */
