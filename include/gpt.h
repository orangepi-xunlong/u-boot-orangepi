/*
 * Copyright (C) 2016 Allwinner.
 * wangwei <wangwei@allwinnertech.com>
 *
 * GPT  Head.h
 *
 * SPDX-License-Identifier: GPL-2.0+
 */



#ifndef _GPT_H
#define _GPT_H

#include <part_efi.h>

#define GPT_HEADER_SIZE 0x5c

#define GPT_ENTRY_OFFSET        1024
#define GPT_HEAD_OFFSET         512

#define GPT_UPDATE_PRIMARY_MBR (1<<0)
#define GPT_UPDATE_SUNXI_MBR   (1<<1)

/*for SDMMC:40960 is the logic start address*/
#define PRIMARY_GPT_ENTRY_OFFSET (40960-32)

/*512(MBR)+512(GPT_HEAD)+128*128(entry)*/
#define GPT_BUF_MAX_SIZE (34*512)

#define SUNXI_GPT_SIZE (32*1024)

#endif	/* _GPT_H */
