/*
 * (C) Copyright 2007
 * Gerald Van Baren, Custom IDEAS, vanbaren@cideas.com
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __UFDT_SUPPORT_H
#define __UFDT_SUPPORT_H

#ifdef CONFIG_OF_LIBUFDT
extern int sunxi_support_ufdt(void *dtb_base, u32 dtb_len);
extern int check_dtbo_idx(void);
#endif /* ifdef CONFIG_OF_LIBUFDT */

#endif /* ifndef __UFDT_SUPPORT_H */
