/*
 *  * Copyright 2000-2009
 *   * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *    *
 *     * SPDX-License-Identifier:	GPL-2.0+
 *     */
#ifndef __IMAGE_DECODE_H____
#define __IMAGE_DECODE_H____ 1

//------------------------------------------------------------------------------------------------------------
#define PLUGIN_TYPE IMGDECODE_PLUGIN_TYPE
#define PLUGIN_NAME "imgDecode" //scott note
#define PLUGIN_VERSION 0x0100
#define PLUGIN_AUTHOR "scottyu"
#define PLUGIN_COPYRIGHT "scottyu"

//------------------------------------------------------------------------------------------------------------
//插件的通用接口
//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
// image 解析接口
//------------------------------------------------------------------------------------------------------------
typedef void *HIMAGE;
typedef void *HIMAGEITEM;

extern HIMAGE Img_Open(char *ImageFile);
extern long long Img_GetSize(HIMAGE hImage);
extern HIMAGEITEM Img_OpenItem(HIMAGE hImage, char *MainType, char *subType);
extern long long Img_GetItemSize(HIMAGE hImage, HIMAGEITEM hItem);
extern uint Img_GetItemStart(HIMAGE hImage, HIMAGEITEM hItem);
extern uint Img_ReadItem(HIMAGE hImage, HIMAGEITEM hItem, void *buffer,
			 uint buffer_size);
extern int Img_CloseItem(HIMAGE hImage, HIMAGEITEM hItem);
extern void Img_Close(HIMAGE hImage);
extern uint Img_GetItemOffset(HIMAGE hImage, HIMAGEITEM hItem);
extern HIMAGE Img_Fat_Open(char *ImageFile);
extern uint Img_Fat_ReadItem(HIMAGE hImage, HIMAGEITEM hItem, char *ImageFile,
			     void *buffer, uint buffer_size);

//------------------------------------------------------------------------------------------------------------
// THE END !
//------------------------------------------------------------------------------------------------------------

#endif //__IMAGE_DECODE_H____
