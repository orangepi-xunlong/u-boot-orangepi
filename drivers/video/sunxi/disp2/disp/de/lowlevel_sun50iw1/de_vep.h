/*
 * drivers/video/sunxi/disp2/disp/de/lowlevel_sun50iw1/de_vep.h
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
#ifndef __DE_VEP_H__
#define __DE_VEP_H__

int de_fcc_init(unsigned int sel, unsigned int reg_base);
int de_fcc_update_regs(unsigned int sel);
int de_fcc_set_reg_base(unsigned int sel, unsigned int chno, void *base);
int de_fcc_csc_set(unsigned int sel, unsigned int chno, unsigned int en, unsigned int mode);

#endif
