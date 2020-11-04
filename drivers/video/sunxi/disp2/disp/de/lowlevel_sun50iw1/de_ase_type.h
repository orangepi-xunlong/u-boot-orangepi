/*
 * drivers/video/sunxi/disp2/disp/de/lowlevel_sun50iw1/de_ase_type.h
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
#ifndef __DE_ASE_TYPE_H__
#define __DE_ASE_TYPE_H__

#include "de_rtmx.h"

#define ASE_PARA_NUM  1
#define ASE_MODE_NUM  4
typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int en				   :  1 ;    // Default: 0x0;
		unsigned int win_en            :  1 ;    // Default: 0x0;
		unsigned int res0              :  30;    // Default: ;
	} bits;
} ASE_CTRL_REG;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int width				:12;// Default: 0x0;
		unsigned int res0				:4;	// Default: ;
		unsigned int height				:12;// Default: 0x0;
		unsigned int res1				:4;	// Default: ;
	} bits;
} ASE_SIZE_REG;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int left				:12;// Default: 0x0;
		unsigned int res0				:4;	// Default: ;
		unsigned int top				:12;// Default: 0x0;
		unsigned int res1				:4;	// Default: ;
	} bits;
} ASE_WIN0_REG;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int right				:12;// Default: 0x0;
		unsigned int res0				:4;	// Default: ;
		unsigned int bot				:12;// Default: 0x0;
		unsigned int res1				:4;	// Default: ;
	} bits;
} ASE_WIN1_REG;


typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int gain				:5;		// Default: 0x0;
		unsigned int res0				:27;	// Default: ;
	} bits;
} ASE_GAIN_REG;

typedef struct
{
	ASE_CTRL_REG		ctrl;			//0x00
	ASE_SIZE_REG		size;			//0x04
	ASE_WIN0_REG		win0;			//0x08
	ASE_WIN1_REG		win1;			//0x0c
	ASE_GAIN_REG		gain;			//0x10

}__ase_reg_t;

typedef struct
{
	//ase
	unsigned int ase_en;
	unsigned int gain;

	//window
	unsigned int win_en;
	de_rect win;
}__ase_config_data;
#endif
