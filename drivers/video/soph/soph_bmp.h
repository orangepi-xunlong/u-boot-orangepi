#ifndef __SOPH_BMP_H__
#define __SOPH_BMP_H__

#define BMP_RLE8_ESCAPE		0
#define BMP_RLE8_EOL		0
#define BMP_RLE8_EOBMP		1
#define BMP_RLE8_DELTA		2

#define range(x, min, max) ((x) < (min)) ? (min) : (((x) > (max)) ? (max) : (x))

int bmpdecoder(void *bmp_addr, void *dst, int dst_bpp);

#endif /* __SOPH_BMP_H__ */
