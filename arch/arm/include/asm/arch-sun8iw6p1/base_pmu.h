/*
 * (C) Copyright 2016
 *Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *zhouhuacai <zhouhuacai@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __BASE_PMU_H_
#define __BASE_PMU_H_

int axp_i2c_read(unsigned char chip, unsigned char addr, unsigned char *buffer);
int axp_i2c_write(unsigned char chip, unsigned char addr, unsigned char data);

void i2c_init(int speed, int slaveaddr);
int pmu_init(u8 power_mode);

int set_ddr_voltage(int set_vol);
s32 sunxi_rsb_init(u32 slave_id);
s32 sunxi_rsb_config(u32 slave_id, u32 rsb_addr);

#endif  /* __BASE_PMU_H_ */

