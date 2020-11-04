/*
 * drivers/video/sunxi/disp2/tv/de_tve_sun8iw11.h
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
#ifndef __DE_TVE_SUN8IW11_H__
#define __DE_TVE_SUN8IW11_H__

#define TVE_GET_REG_BASE(sel)                   (tve_reg_base[sel])
#define TVE_WUINT32(sel,offset,value)           (*((volatile u32 *)( TVE_GET_REG_BASE(sel) + (offset) ))=(value))
#define TVE_RUINT32(sel,offset)                 (*((volatile u32 *)( TVE_GET_REG_BASE(sel) + (offset) )))
#define TVE_SET_BIT(sel,offset,bit)             (*((volatile u32 *)( TVE_GET_REG_BASE(sel) + (offset) )) |= (bit))
#define TVE_CLR_BIT(sel,offset,bit)             (*((volatile u32 *)( TVE_GET_REG_BASE(sel) + (offset) )) &= (~(bit)))
#define TVE_INIT_BIT(sel,offset,c,s)            (*((volatile u32 *)( TVE_GET_REG_BASE(sel) + (offset) )) = \
                                                (((*(volatile u32 *)( TVE_GET_REG_BASE(sel) + (offset) )) & (~(c))) | (s)))

#define TVE_TOP_GET_REG_BASE                    (tve_top_reg_base[0])
#define TVE_TOP_WUINT32(offset,value)           (*((volatile u32 *)( TVE_TOP_GET_REG_BASE + (offset) ))=(value))
#define TVE_TOP_RUINT32(offset)                 (*((volatile u32 *)( TVE_TOP_GET_REG_BASE + (offset) )))
#define TVE_TOP_SET_BIT(offset,bit)             (*((volatile u32 *)( TVE_TOP_GET_REG_BASE + (offset) )) |= (bit))
#define TVE_TOP_CLR_BIT(offset,bit)             (*((volatile u32 *)( TVE_TOP_GET_REG_BASE + (offset) )) &= (~(bit)))
#define TVE_TOP_INIT_BIT(offset,c,s)            (*((volatile u32 *)( TVE_TOP_GET_REG_BASE + (offset) )) = \
                                               (((*(volatile u32 *)( TVE_TOP_GET_REG_BASE + (offset) )) & (~(c))) | (s)))
/*
enum tv_mode {
	CVBS,
	YPBPR,
	VGA,
};
*/

enum disp_cvbs_mode{
	TV_NTSC = 0,
	TV_PAL  = 1,
};

s32 tve_low_set_reg_base(u32 sel,void __iomem * address);
s32 tve_low_dac_init(u32 dac_no,u32 cali,s32 offset);
s32 tve_low_dac_map(u32 sel, u32 *dac_no, u32 num);
s32 tve_low_dac_enable(u32 sel);
s32 tve_low_dac_disable(u32 sel);
s32 tve_low_open(u32 sel);
s32 tve_low_close(u32 sel);
s32 tve_low_set_tv_mode(u32 sel, u32 is_cvbs);
s32 tve_low_set_ypbpr_mode(u32 sel, enum disp_tv_mode mode);
s32 tve_low_set_vga_mode(u32 sel);
s32 tve_low_set_cvbs_mode(u32 sel, u8 mode);
s32 tve_low_get_dac_status(u32 sel);
s32 tve_low_dac_autocheck_enable(u32 sel, u8 index);
s32 tve_low_dac_autocheck_disable(u32 sel,u8 index);
s32 tve_low_enhance(u32 sel, u32 mode);

#endif
