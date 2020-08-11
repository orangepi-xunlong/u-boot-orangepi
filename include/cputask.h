/*
 * Copyright (C)  All rights reserved.
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Author: <wangflord@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */

/*
 * This file contains clk functions, adapt for clk operation in boot.
 */



#ifndef __CPU_TASK_H__
#define __CPU_TASK_H__


int sunxi_secendary_cpu_task(void);
int sunxi_third_cpu_task(void);

int secondary_cpu_start(void);
int third_cpu_start(void);
int get_core_pos(void);

int sunxi_secondary_cpu_poweroff(void);

int cpu0_set_irq_stack(uint32_t sp);

#endif /* __CPU_TASK_H__ */
