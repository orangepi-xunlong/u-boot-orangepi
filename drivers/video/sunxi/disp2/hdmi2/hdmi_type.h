#ifndef  _HDMI_BOOT_H_
#define  _HDMI_BOOT_H_
#if 0
typedef unsigned char      	u8;
typedef signed char        	s8;
typedef unsigned short     	u16;
typedef signed short       	s16;
typedef unsigned int       	u32;
typedef signed int         	s32;
typedef unsigned long long 	u64;
#endif
#include <common.h>
#include <malloc.h>
#include <sys_config.h>
#include <sunxi_display2.h>
#include <linux/list.h>
#include <linux/compat.h>
#include "../disp/disp_sys_intf.h"

#define pr_info printf
#define pr_err printf
#define pr_warn printf
#define BIT(x)			(1 << (x))
#endif