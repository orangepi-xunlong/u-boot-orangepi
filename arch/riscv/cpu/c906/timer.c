// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/timer.h>
#include <asm/arch/plic.h>
#include <div64.h>

DECLARE_GLOBAL_DATA_PTR;

#define TIMER_MODE   (0x0 << 7)	/* continuous mode */
#define TIMER_DIV    (0x0 << 4)	/* pre scale 1 */
#define TIMER_SRC    (0x1 << 2)	/* osc24m */
#define TIMER_RELOAD (0x1 << 1)	/* reload internal value */
#define TIMER_EN     (0x1 << 0)	/* enable timer */

#define TIMER_CLOCK		(24 * 1000 * 1000)
#define COUNT_TO_USEC(x)	((x) / 24)
#define USEC_TO_COUNT(x)	((x) * 24)
#define TICKS_PER_HZ		(TIMER_CLOCK / CONFIG_SYS_HZ)
#define TICKS_TO_HZ(x)		((x) / TICKS_PER_HZ)

#define TIMER_LOAD_VAL		0xffffffff

#define TIMER_NUM		0	/* we use timer 0 */

/* init timer register */
int timer_init(void)
{
	struct sunxi_timer_reg *timers =
		(struct sunxi_timer_reg *)SUNXI_TIMER_BASE;
#if 0
	struct sunxi_timer *timer = &timers->timer[TIMER_NUM];
	writel(TIMER_LOAD_VAL, &timer->inter);
	writel(TIMER_MODE | TIMER_DIV | TIMER_SRC | TIMER_RELOAD | TIMER_EN,
	       &timer->ctl);
#endif

	timers->tirqen  = 0;
	timers->tirqsta |= 0x03;

	return 0;
}

void timer_exit(void)
{
	struct sunxi_timer_reg *timers = (struct sunxi_timer_reg *)SUNXI_TIMER_BASE;

	timers->tirqen  = 0;
	timers->tirqsta |= 0x043f;
	timers->timer[0].ctl = 0;
	timers->timer[1].ctl = 0;

}

/*
* 64bit arch timer.CNTPCT
* Freq = 24000000Hz
*/
static inline u64 notrace read_timer(void)
{
	u64 cnt = 0;
	asm volatile("rdtime %0\n"
		: "=r" (cnt)
		:
		: "memory");
	return cnt;
}

void __udelay(unsigned long us)
{
	u64 t1, t2;

	t1 = read_timer();
	t2 = t1 + us*24;
	do {
		t1 = read_timer();
	} while (t2 >= t1);
}

void __usdelay(unsigned long us)
{
	u64 t1, t2;

	t1 = read_timer();
	t2 = t1 + us*24;
	do {
		t1 = read_timer();
	} while (t2 >= t1);
}

void __msdelay(unsigned long ms)
{
	__usdelay(ms * 1000);
	return ;
}

/* get the current time(ms), freq = 24000000Hz*/
int runtime_tick(void)
{
	u64 cnt = 0;
	cnt = read_timer();
	return lldiv(cnt, 24000);
}

unsigned long notrace timer_get_us(void)
{
	u64 cnt = 0;
	cnt	= read_timer();
	return lldiv(cnt, 24);
}

ulong get_timer_masked(void)
{
	/* current tick value */
	ulong now = runtime_tick();
	return now;
}

/* timer without interrupts */
/* count the delay by seconds */
ulong get_timer(ulong base)
{
    return get_timer_masked() - base;
}

/*
 * This function is derived from PowerPC code (read timebase as long long).
 * On ARM it just returns the timer value.
 */
unsigned long long get_ticks(void)
{
	u64 cnt;

	cnt = read_timer();

	return cnt;
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
ulong get_tbclk(void)
{
	return CONFIG_SYS_HZ;
}

void watchdog_disable(void)
{
	struct sunxi_timer_reg *timers = (struct sunxi_timer_reg *)SUNXI_TIMER_BASE;

	writel(0, IOMEM_ADDR(timers->wdog[0].mode));
	return ;
}

void watchdog_enable(void)
{
	struct sunxi_timer_reg *timers = (struct sunxi_timer_reg *)SUNXI_TIMER_BASE;

	writel(1, IOMEM_ADDR(timers->wdog[0].cfg));
	writel(1, IOMEM_ADDR(timers->wdog[0].mode));

	return ;
}

static  int  timer_used_status;

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
static void timerX_callback_default(void *data);

struct __timer_callback {
	void (*func_back)(void *data);
	unsigned long data;
};

struct __timer_callback timer_callback[2] = {
	{timerX_callback_default, 0},
	{timerX_callback_default, 1}
};

static void timerX_callback_default(void *data)
{
	printf("this is only for test, timer number=%d\n", (uint)(ulong)data);
}

void timer0_func(void *data)
{
	struct sunxi_timer_reg *timer_control = (struct sunxi_timer_reg *)SUNXI_TIMER_BASE;

	if (!(timer_control->tirqsta & 0x01)) {
		return ;
	}
	timer_control->tirqen  &= ~0x01;
	timer_control->tirqsta  =  0x01;
	irq_disable(AW_IRQ_TIMER0);
	timer_used_status &= ~1;
	debug("timer 0 occur\n");
	timer_callback[0].func_back((void *)timer_callback[0].data);
}

void timer1_func(void *data)
{
	struct sunxi_timer_reg *timer_control = (struct sunxi_timer_reg *)SUNXI_TIMER_BASE;

	if (!(timer_control->tirqsta & 0x02)) {
		return ;
	}
	timer_control->tirqen  &= ~0x02;
	timer_control->tirqsta  =  0x02;
	irq_disable(AW_IRQ_TIMER1);
	timer_used_status &= ~(1 << 1);
	debug("timer 1 occur\n");
	timer_callback[1].func_back((void *)timer_callback[1].data);
}


void init_timer(struct timer_list *timer)
{
    return ;
}

void add_timer(struct timer_list *timer)
{
	u32 reg_val;
	int timer_num;
	struct sunxi_timer     *timer_tcontrol;
	struct sunxi_timer_reg *timer_reg;

	if (timer->expires <= 0) {
		timer->expires = 1000;
	}

	if (!timer->expires) {
		return ;
	}
	debug("timer delay time %d\n", timer->expires);
	if (!(timer_used_status & 0x01)) {
		timer_used_status |= 0x01;
		timer_num = 0;
	} else if (!(timer_used_status & 0x02)) {
		timer_used_status |= 0x02;
		timer_num = 1;
	} else {
		printf("timer err: there is no timer cound be used\n");
		return ;
	}
	timer->timer_num = timer_num;
	timer_reg      =   (struct sunxi_timer_reg *)SUNXI_TIMER_BASE;
	timer_tcontrol = &((struct sunxi_timer_reg *)SUNXI_TIMER_BASE)->timer[timer_num];
#ifndef FPGA_PLATFORM
	reg_val =	(0 << 0)  |
				(1 << 1)  |
				(1 << 2)  |
				(5 << 4)  |
				(1 << 7);
#else
	reg_val =	(0 << 0)  |
				(1 << 1)  |
				(0 << 2)  |
				(0 << 4)  |
				(1 << 7);
#endif
	timer_tcontrol->ctl = reg_val;
#ifndef FPGA_PLATFORM
	timer_tcontrol->inter = timer->expires * (24000 / 32);
#else
	timer_tcontrol->inter = timer->expires * 1000 / 32;
#endif
	timer_callback[timer_num].func_back = timer->function;
	timer_callback[timer_num].data      = timer->data;
	if (!timer_num) {
		irq_install_handler(AW_IRQ_TIMER0 + timer_num, timer0_func, (void *)&timer_callback[timer_num].data);
	} else {
		irq_install_handler(AW_IRQ_TIMER0 + timer_num, timer1_func, (void *)&timer_callback[timer_num].data);
	}
	timer_tcontrol->ctl |= (1 << 1);

	while
		(timer_tcontrol->ctl & 0x02);

	irq_enable(AW_IRQ_TIMER0 + timer_num);
	timer_tcontrol->ctl |= 1;
	timer_reg->tirqsta  |= (1 << timer_num);
	timer_reg->tirqen  |= (1 << timer_num);

	return ;
}


void del_timer(struct timer_list *timer)
{
	struct sunxi_timer *timer_tcontrol;
	struct sunxi_timer_reg *timer_reg;
	int    num = timer->timer_num;

	timer_reg      =   (struct sunxi_timer_reg *)SUNXI_TIMER_BASE;
	timer_tcontrol = &((struct sunxi_timer_reg *)SUNXI_TIMER_BASE)->timer[num];

	irq_disable(AW_IRQ_TIMER0 + num);
	timer_tcontrol->ctl &= ~1;
	timer_reg->tirqsta &= ~(1 << num);
	timer_reg->tirqen  &= ~(1 << num);

	timer_callback[num].data = num;
	timer_callback[num].func_back = timerX_callback_default;
	timer_used_status &= ~(1 << num);

	return ;
}

