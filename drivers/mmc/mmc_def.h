// SPDX-License-Identifier: GPL-2.0+
#ifndef __MMC_DEF__
#define __MMC_DEF__

#include <asm/types.h>
//#define  CONFIG_MMC_DEBUG_SUNXI
/*change address to iomem pointer*/
/*#define IOMEM_ADDR(addr) ((void __iomem *)((phys_addr_t)(addr)))*/
#define PT_TO_U32(p)   ((u32)((phys_addr_t)(p)))
#define WR_MB() 	wmb()
/*transfer pointer to unsigned type,if phy add is 64,trasfer to u64*/
#define PT_TO_PHU(p)   ((unsigned long)(p))
typedef volatile void __iomem *iom;

#ifdef CONFIG_MMC_DEBUG_SUNXI
#define MMCINFO(fmt, args...)	pr_err("[mmc]: "fmt, ##args)//err or info
#define MMCDBG(fmt, args...)	pr_err("[mmc]: "fmt, ##args)//dbg
#define MMCPRINT(fmt, args...)	pr_err(fmt, ##args)//data or register and so on
#else
#define MMCINFO(fmt, args...)	pr_err("[mmc]: "fmt, ##args)//err or info
#define MMCDBG(fmt...)
#define MMCPRINT(fmt...)
#endif

#define MMC_MSG_EN	(1U)
#define MMCMSG(d, fmt, args...) do {if ((d)->msglevel & MMC_MSG_EN)  pr_err("[mmc]: "fmt, ##args); } while (0)

#define DRIVER_VER  "uboot2018:2021-07-19 14:09:00"

//secure storage relate
#define MAX_SECURE_STORAGE_MAX_ITEM             32
#define SDMMC_SECURE_STORAGE_START_ADD  (6*1024*1024/512)//6M
#define SDMMC_ITEM_SIZE                                 (4*1024/512)//4K


#endif
