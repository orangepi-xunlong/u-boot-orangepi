/*
 * (C) Copyright 2012
 *     tyle@allwinnertech.com
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program;
 *
 */
#include <common.h>
#include <asm/arch/pwm.h>
#include <asm/arch/platform.h>
#include <sys_config.h>
#include <pwm.h>
#include <fdt_support.h>

#define sys_get_wvalue(n)   (*((volatile uint *)(n)))          /* word input */
#define sys_put_wvalue(n,c) (*((volatile uint *)(n))  = (c))   /* word output */
#ifndef abs
#define abs(x) (((x)&0x80000000)? (0-(x)):(x))
#endif

uint pwm_active_sta[4] = {1, 0, 0, 0};

#define sunxi_pwm_debug 0
#undef  sunxi_pwm_debug

#ifdef sunxi_pwm_debug
    #define pwm_debug(fmt,args...)  printf(fmt ,##args)
#else
    #define pwm_debug(fmt,args...)
#endif

#define PWM_PIN_STATE_ACTIVE "active"
#define PWM_PIN_STATE_SLEEP  "sleep"

uint sunxi_pwm_read_reg(uint offset)
{
    return -1;
}

uint sunxi_pwm_write_reg(uint offset, uint value)
{
    return -1;
}

extern int fdt_set_all_pin(const char* node_path,const char* pinctrl_name);
static int sunxi_pwm_pin_set_state(char *dev_name, char *name)
{
    char compat[32];
    u32 len   = 0;
    int state = 0;
    int ret   = -1;

    if (!strcmp(name, PWM_PIN_STATE_ACTIVE))
        state = 1;
    else
        state = 0;

    len = sprintf(compat, "%s", dev_name);
    if (len > 32)
        printf("disp_sys_set_state, size of mian_name is out of range\n");

    ret = fdt_set_all_pin(compat, (1 == state)?"pinctrl-0":"pinctrl-1");
    if (0 != ret)
        printf("%s, fdt_set_all_pin, ret=%d\n", __func__, ret);

    return ret;
}

int sunxi_pwm_set_polarity(int pwm, enum pwm_polarity polarity)
{
    uint temp;

    temp = sunxi_pwm_read_reg(0);
    if(polarity == PWM_POLARITY_NORMAL)
    {
        pwm_active_sta[pwm] = 1;
        if(pwm == 0)
            temp |= 1 << 5;
        else
            temp |= 1 << 20;
    }
    else
    {
        pwm_active_sta[pwm] = 0;
        if(pwm == 0)
            temp &= ~(1 << 5);
        else
            temp &= ~(1 << 20);
    }

    sunxi_pwm_write_reg(0, temp);

    return 0;
}

int sunxi_pwm_config(int pwm, int duty_ns, int period_ns)
{
    uint pre_scal[11][2] = {{15, 1}, {0, 120}, {1, 180}, {2, 240}, {3, 360}, {4, 480}, {8, 12000}, {9, 24000}, {10, 36000}, {11, 48000}, {12, 72000}};
    uint freq;
    uint pre_scal_id       = 0;
    uint entire_cycles     = 256;
    uint active_cycles     = 192;
    uint entire_cycles_max = 65536;
    uint temp;

    if(period_ns < 10667)
        freq = 93747;
    else if(period_ns > 1000000000)
        freq = 1;
    else
        freq = 1000000000 / period_ns;

    entire_cycles = 24000000 / freq / pre_scal[pre_scal_id][1];

    while(entire_cycles > entire_cycles_max)
    {
        pre_scal_id++;
        if(pre_scal_id > 10)
            break;
        entire_cycles = 24000000 / freq / pre_scal[pre_scal_id][1];
    }

    if(period_ns < 5*100*1000)
        active_cycles = (duty_ns * entire_cycles + (period_ns/2)) /period_ns;
    else if(period_ns >= 5*100*1000 && period_ns < 6553500)
        active_cycles = ((duty_ns / 100) * entire_cycles + (period_ns /2 / 100)) / (period_ns/100);
    else
        active_cycles = ((duty_ns / 10000) * entire_cycles + (period_ns /2 / 10000)) / (period_ns/10000);

    temp = sunxi_pwm_read_reg(0);

    if(pwm == 0)
        temp = (temp & 0xfffffff0) |pre_scal[pre_scal_id][0];
    else
        temp = (temp & 0xfff87fff) |pre_scal[pre_scal_id][0];

    sunxi_pwm_write_reg(0, temp);

    sunxi_pwm_write_reg((pwm + 1)  * 0x04, ((entire_cycles - 1)<< 16) | active_cycles);

    pwm_debug("PWM _TEST: duty_ns=%d, period_ns=%d, freq=%d, per_scal=%d, period_reg=0x%x\n", duty_ns, period_ns, freq, pre_scal_id, temp);

    return 0;
}

int sunxi_pwm_enable(int pwm)
{
    uint temp;


    temp = sunxi_pwm_read_reg(0);

    if(pwm == 0)
    {
        temp |= 1 << 4;
        temp |= 1 << 6;
    }
    else
    {
        temp |= 1 << 19;
        temp |= 1 << 21;
    }

    sunxi_pwm_pin_set_state("pwm", PWM_PIN_STATE_ACTIVE);
    sunxi_pwm_write_reg(0, temp);

    return 0;
}

void sunxi_pwm_disable(int pwm)
{
    uint temp;


    temp = sunxi_pwm_read_reg(0);

    if(pwm == 0)
    {
        temp &= ~(1 << 4);
        temp &= ~(1 << 6);
    }
    else
    {
        temp &= ~(1 << 19);
        temp &= ~(1 << 21);
    }
    sunxi_pwm_write_reg(0, temp);
    sunxi_pwm_pin_set_state("pwm", PWM_PIN_STATE_SLEEP);

}

void sunxi_pwm_init(void)
{
    return ;
}

