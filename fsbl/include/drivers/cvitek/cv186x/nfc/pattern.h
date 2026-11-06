/**********************************************************************
 * pattern.c
 *
 * Software to fill, clear and compare data buffers
 ***********************************************************************/




#ifndef PATTERN_H
#define PATTERN_H
#include <stdint.h>


void Clearbuf(void *buf, uint32_t sizeWords,uint32_t data_pattern);
void Fillbuf(void *buf, uint32_t sizeWords, uint32_t start);
uint32_t Comparebuf(void *buf1, void *buf2, uint32_t sizeWords);
void PrintCmpBuf(void *buf1, void *buf2, uint32_t sizeWords);


#endif
