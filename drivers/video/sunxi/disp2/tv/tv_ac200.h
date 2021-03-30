
#ifndef  _DRV_TV_AC200_H_
#define  _DRV_TV_AC200_H_
#if 0
#include <linux/module.h>
#include <asm/uaccess.h>
#include <asm/memory.h>
#include <asm/unistd.h>
#include "asm-generic/int-ll64.h"
#include "linux/kernel.h"
#include "linux/mm.h"
#include "linux/semaphore.h"
#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/dma-mapping.h>
#include <linux/sched.h>   //wake_up_process()
#include <linux/kthread.h> //kthread_create()??��|kthread_run()
#include <linux/err.h> //IS_ERR()??��|PTR_ERR()
#include <linux/platform_device.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/clk.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <mach/sys_config.h>
#include <mach/platform.h>
#include "../disp/disp_sys_intf.h"
#include <linux/mfd/acx00-mfd.h>
#include <video/sunxi_display2.h>
#else
#include <common.h>
#include <malloc.h>
#include <sunxi_display2.h>
#include <sys_config.h>
#include <asm/arch/intc.h>
#include <pwm.h>
#include <asm/arch/timer.h>
#include <asm/arch/platform.h>
#include <linux/list.h>
#include <asm/memory.h>
#include <div64.h>
#include <fdt_support.h>
#include <power/sunxi/pmu.h>
#include "asm/io.h"
#include <i2c.h>

//#include <common.h>
//#include <malloc.h>
//#include <asm/arch/sunxi_display2.h>
//#include <asm/arch/intc.h>
//#include <asm/arch/cpu.h>
//#include <pmu.h>
//#include <asm/arch/timer.h>
//#include <asm/arch/pwm.h>
//#include <axp_power.h>
#include "../disp/de/bsp_display.h"
#include "../disp/disp_sys_intf.h"

#endif


enum hpd_status
{
	STATUE_CLOSE = 0,
	STATUE_OPEN  = 1,

};

struct ac200_tv_priv {
	struct acx00 *acx00;
};

int tv_ac200_init(void);

extern struct ac200_tv_priv tv_priv;
extern struct disp_video_timings tv_video_timing[];
#endif
