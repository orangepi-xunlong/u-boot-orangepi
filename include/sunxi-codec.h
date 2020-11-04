/*
 * (C) Copyright 2014-2019
 * allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * caiyongheng <caiyongheng@allwinnertech.com>
 *
 * some simple description for this code
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 */

#ifndef _SUNXI_CODEC_H
#define _SUNXI_CODEC_H


int sunxi_codec_probe(void);
int sunxi_codec_hw_params(u32 sb, u32 sr, u32 ch);
int sunxi_codec_playback_prepare(void);
int sunxi_codec_playback_start(ulong handle, u32 *srcBuf, u32 cnt);

void sunxi_codec_fill_txfifo(u32 *data);
void sunxi_codec_dump_reg(void);

#endif /* __SUNXI_CODEC_H */
