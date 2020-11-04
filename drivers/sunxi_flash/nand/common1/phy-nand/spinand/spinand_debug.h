/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __SPINAND_DEBUG_H__
#define __SPINAND_DEBUG_H__
#include <vsprintf.h>
#include <linux/printk.h>
#include <linux/kernel.h>


#define SPINAND_INFO(fmt, ...) \
	pr_info("[NI]" fmt, ##__VA_ARGS__)
#define SPINAND_DBG(fmt, ...) \
	pr_debug("[ND]" fmt, ##__VA_ARGS__)
#define SPINAND_ERR(fmt, ...) \
	pr_err("[NE]" fmt, ##__VA_ARGS__)


#define spinand_print(fmt, arg...)                        \
	do {                                                \
		pr_err("[spinand] "fmt "\n", ##arg); \
	} while (0)

#endif /*SPINAND_DEBUG_H*/
