/*
 * drivers/video/sunxi/disp2/disp/de/lowlevel_v2x/de_eink.h
 *
 * Copyright (c) 2007-2019 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <zhengxiaobin@allwinnertech.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifndef __DE_EINK_H__
#define __DE_EINK_H__

#include "../include.h"

int eink_config(unsigned char in_mode, unsigned int out_mode);
int eink_start_idx(struct ee_img *last_img, struct ee_img *curr_img, unsigned char flash_mode, unsigned char win_en,
					unsigned long last_idx_addr, unsigned long curr_idx_addr, struct area_info *info);
int eink_set_base(unsigned long reg_base);
int eink_irq_enable(void);
int eink_irq_disable(void);
int eink_irq_query(void);
int eink_set_mode(unsigned char in_mode, unsigned char out_mode);
int eink_get_updata_area(struct area_info *info);
int eink_pipe_enable(unsigned int pipe_no);
int eink_pipe_disable(unsigned int pipe_no);
int eink_pipe_config(struct area_info *info, unsigned int pipe_no);
int eink_pipe_config_wavefile(unsigned long wav_file_addr, unsigned int pipe_no);

int eink_edma_init(unsigned char mode);
int eink_edma_en(unsigned char en);
int eink_dbuf_rdy(void);
int eink_set_wb(unsigned char wb_en, unsigned long wb_addr);
int eink_decoder_start(unsigned long new_idx_addr, unsigned long wav_data_addr,
						struct eink_init_param *para);
int eink_edma_cfg(unsigned long wav_addr, struct eink_init_param *para);
int eink_edma_cfg_addr(unsigned long wav_addr);
int eink_index_finish(void);


#endif
