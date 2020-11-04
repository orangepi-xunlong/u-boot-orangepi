/*
 *  * Copyright 2000-2009
 *   * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *    *
 *     * SPDX-License-Identifier:	GPL-2.0+
 *     */
#ifndef __SUNXI_SPRITE_CARD_H__
#define __SUNXI_SPRITE_CARD_H__

#include <common.h>
#include <sunxi_mbr.h>

extern uint sprite_card_firmware_start(void);

extern int sprite_card_firmware_probe(char *name);

extern int sprite_card_fetch_download_map(sunxi_download_info *dl_map);

extern int sprite_card_fetch_mbr(void *img_mbr);

extern int sunxi_sprite_deal_part(sunxi_download_info *dl_map);

extern int sunxi_sprite_deal_uboot(int production_media);

extern int sunxi_sprite_deal_boot0(int production_media);

extern int sunxi_sprite_deal_recorvery_boot(int production_media);

extern int card_download_uboot(uint length, void *buffer);

extern int card_erase_boot0(uint length, void *buffer, uint storage_type);

extern int card_download_standard_mbr(void *buffer);
#ifdef CONFIG_SUNXI_SPINOR
extern int sunxi_sprite_deal_fullimg(void);
#endif
extern int card_erase(int erase, void *mbr_buffer);
#endif
