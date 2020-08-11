/*
 * sunxi_spinor.h
 *
 * Copyright (C) 2017-2020 Allwinnertech Co., Ltd
 *
 * Author        : zhouhuacai
 *
 * Description   : spinor interface statement.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 *
 *
 * History       :
 *  1.Date        : 2017/4/12
 *    Author      : zhouhuacai
 *    Modification: Created file
 */

#ifndef __SUNXI_SPINOR_H__
#define   __SUNXI_SPINOR_H__

int   spinor_init(void);
int   spinor_exit(void);
int   spinor_read(unsigned int start, unsigned int nblock, void *buffer);

#endif /* __SUNXI_SPINOR_H__ */
