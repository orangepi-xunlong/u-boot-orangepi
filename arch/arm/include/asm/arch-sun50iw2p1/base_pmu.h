/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __BASE_PMU_H_
#define __BASE_PMU_H_


void i2c_init_cpus(int speed, int slaveaddr);

int axp_i2c_read(unsigned char chip, unsigned char addr, unsigned char *buffer);
int axp_i2c_write(unsigned char chip, unsigned char addr, unsigned char data);


int pmu_init(u8 power_mode);
int set_ddr_voltage(int set_vol);
int set_pll_voltage(int set_vol);


#endif  /* __BASE_PMU_H_ */

