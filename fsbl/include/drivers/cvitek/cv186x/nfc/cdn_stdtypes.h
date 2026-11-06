 /*
 * Copyright (c) 2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef INCLUDE_CDN_STDTYPES_H___
#define	INCLUDE_CDN_STDTYPES_H___
#ifdef __KERNEL__
#include <linux/types.h>
#else
#include "cdn_stdint.h"

/* Define NULL constant */
#ifndef NULL
#define	NULL	((void *)0)
#endif

/* Define size_t data type */
typedef	uintptr_t	size_t;

/* Define bool data type */
#define bool	_Bool
#define	true	1
#define	false	0
#define	__bool_true_false_are_defined 1
#endif /* __KERNEL__ */
#endif	/* INCLUDE_CDN_STDTYPES_H__ */
