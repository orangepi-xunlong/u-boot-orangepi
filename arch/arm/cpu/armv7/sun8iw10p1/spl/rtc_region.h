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

#ifndef __RTC_REGION_I_H__
#define __RTC_REGION_I_H__

#include<common.h>
#include<asm/arch/platform.h>
#include<asm/arch-sunxi/dram.h>
#include<asm/arch-sun8iw10p1/dram.h>
#define IO_NUM (2)
#define PLL_NUM         (14)
#define BUS_NUM     (8)
#define STANDBY_SPACE_BASE_ADDR	     (0x43000000)
#define EXTENDED_STANDBY_BASE_OFFSET (0x400)


enum VDD_BIT {
  VDD_CPUA_BIT = 0,
  VDD_CPUB_BIT,
  VCC_DRAM_BIT,
  VDD_GPU_BIT,
  VDD_SYS_BIT,
  VDD_VPU_BIT,
  VDD_CPUS_BIT,
  VDD_DRAMPLL_BIT,
  VCC_ADC_BIT,
  VCC_PL_BIT,
  VCC_PM_BIT,
  VCC_IO_BIT,
  VCC_CPVDD_BIT,
  VCC_LDOIN_BIT,
  VCC_PLL_BIT,
  VCC_LPDDR_BIT,
  VDD_TEST_BIT,
  VDD_RES1_BIT,
  VDD_RES2_BIT,
  VDD_RES3_BIT,
  VCC_MAX_INDEX,
};

typedef struct {
  unsigned int src;
  unsigned int pre_div;
  unsigned int div_ratio;
  unsigned int n;
  unsigned int m;
} bus_para_t;

typedef struct {
  unsigned int factor1;
  unsigned int factor2;
  unsigned int factor3;
  unsigned int factor4;
} pll_para_t;

typedef struct {
  /*
   * for state bitmap:
   * bitx = 1: keep state.
   * bitx = 0: mean close corresponding power src.
   */
  unsigned int state;
  /* bitx=1, the corresponding state is effect,
   * otherwise, the corresponding power is in charge in device driver.
   *      sys_mask&state          : bitx=1,
   *                      mean the power is on,
   *                      for the "on" state power,
   *                      u need care about the voltage.;
   *  ((~sys_mask)|state) : bitx=0, mean the power is close;
   *
   * pwr_dm_state bitmap
   * actually: we care about the pwr_dm voltage,
   *   such as: we want to keep the vdd_sys at 1.0v at standby period.
   * we actually do not care how to do it.
   * it can be sure that cpus can do it with the pmu's help.
   */
  unsigned int sys_mask;
  unsigned int volt[VCC_MAX_INDEX];	/* unsigned short is 16bit width. */
} pwr_dm_state_t;


/* selfresh_flag must be compatible with vdd_sys pwr state.*/
typedef struct pm_dram_para {
  unsigned int selfresh_flag;
  unsigned int crc_en;
  unsigned int crc_start;
  unsigned int crc_len;
} pm_dram_para_t;

typedef struct cpus_clk_para {
  unsigned int cpus_id;
} cpus_para_t;

typedef struct io_state_config {
  unsigned int paddr;
  unsigned int value_mask;	/* specify the effect bit. */
  unsigned int value;
} io_state_config_t;


typedef struct cpux_clk_para {
  /*
   * Hosc: losc: ldo1: ldo0
   * the osc bit map is as follow:
   * bit3: bit2: bit1:bit0
   * Hosc: losc: ldo1: ldo0
   */
  int osc_en;

  /*
   * for a83, pll bitmap as follow:
   *
   * pll11(video1):   pll10(de):  pll9(hsic)
   * bit7:        bit6:       bit5:       bit4:
   * pll8(gpu): pll6(periph): pll5(dram): pll4(ve):
   * bit3:            bit2:           bit1:       bit0
   * pll3(video):     pll2(audio):    c1cpux:     c0cpux
   *
   *
   *
   * */

  /* for disable bitmap:
   * bitx = 0: close
   * bitx = 1: do not care.
   */
  int init_pll_dis;

  /* for enable bitmap:
   * bitx = 0: do not care.
   * bitx = 1: open
   */
  int exit_pll_en;


  /*
   * set corresponding bit if it's pll factors need to be set some value.
   */
  int pll_change;

  /*
   * fill in the enabled pll freq factor sequently.
   * unit is khz pll6: 0x90041811
   * factor n/m/k/p already do the pretreatment of the minus one
   */
  pll_para_t pll_factor[PLL_NUM];

  /*
   * **********A31************
   * at a31 platform, the clk relationship is as follow:
   * pll1->cpu -> axi
   *       -> atb/apb
   * ahb1 -> apb1
   * apb2
   * so, at a31, only cpu:ahb1:apb2 need be cared.
   *
   * *********A80************
   * at a83 platform, the clk relationship is as follow:
   * c1 -> axi1
   * c0 -> axi0
   * gtbus
   * ahb0
   * ahb1
   * ahb2
   * apb0
   * apb1
   * so, at a80, c1:c0:gtbus:ahb0:ahb1:ahb2:apb0:apb1 need be cared.
   *
   * *********A83************
   * at a83 platform, the clk relationship is as follow:
   * c1 -> axi1
   * c0 -> axi0
   * ahb1 -> apb1
   * apb2
   * ahb2
   * so, at a83, only c1:c0:ahb1:apb2:ahb2 need be cared.
   *
   *
   * normally, only clk src need be cared.
   * the bus bitmap as follow:
   * for a83, bus_en:
   * bit4: bit3: bit2: bit1: bit0
   * ahb2: apb2: ahb1: c1:   c0
   *
   * for a31, bus_en:
   * bit2:bit1:bit0
   * cpu:ahb1:apb2
   *
   * for a80, bus_en:
   * bit7: bit6:  bit5:  bit4: bit3: bit2: bit1: bit0
   * c1:   c0:    gtbus: ahb0: ahb1: ahb2: apb0: apb1
   */
  int bus_change;

  /*
   * bus_src: ahb1, apb2 src;
   * option:  pllx:axi:hosc:losc
   */
  bus_para_t bus_factor[BUS_NUM];

} cpux_clk_para_t;



typedef struct soc_io_para {
  /*
   *  hold: mean before power off vdd_sys, whether hold gpio pad or not.
   *  this flag only effect: when vdd_sys is powered_off;
   *  this flag only affect hold&unhold operation.
   *  the recommended hold sequence is as follow:
   *  backup_io_cfg -> cfg_io(enter_low_power_mode)
   *  -> hold -> assert vdd_sys_reset -> poweroff vdd_sys.
   *  the recommended unhold sequence is as follow:
   *  poweron vdd_sys -> de_assert vdd_sys -> restore_io_cfg -> unhold.
   * */
  unsigned int hold_flag;


  /*
   * note: only specific bit mark by value_mask is effect.
   * IO_NUM: only uart and jtag needed care.
   * */
  io_state_config_t io_state[IO_NUM];
} soc_io_para_t;


typedef struct extended_standby {
  /*
   * id of extended standby
   */
  unsigned int id;
  unsigned int pmu_id;
  /* for: 808 || 809+806 || 803 || 813
   * support 4 pmu: each pmu_id range is: 0-255;
   * pmu_id <--> pmu_name have directly mapping,
   * u can get the pmu_name from sys_config.fex files.;
   * bitmap as follow:
   * pmu0: 0-7 bit; pmu1: 8-15 bit; pmu2: 16-23 bit; pmu3: 24-31 bit
   */


  unsigned int soc_id;
  /* a33, a80, a83,...,
   * for compatible reason, different soc,
   * each bit have different meaning.
   */

  pwr_dm_state_t soc_pwr_dm_state;
  pm_dram_para_t soc_dram_state;
  soc_io_para_t soc_io_state;
  cpux_clk_para_t cpux_clk_state;
} extended_standby_t;


#endif
