/*
 * COM1 NS16550 support
 * originally from linux source (arch/powerpc/boot/ns16550.c)
 * modified to use CONFIG_SYS_ISA_MEM and new defines
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/uart.h>
#include <asm/arch/gpio.h>
#include <asm/arch/ccmu.h>
#include <asm/arch/clock.h>

#define thr rbr
#define dll rbr
#define dlh ier
#define iir fcr


static serial_hw_t *serial_ctrl_base = NULL;
static volatile u32 uart_reg_base = 0;
extern int do_div( int divisor, int by);
extern __u32 get_pll_apb_clk(void);


void sunxi_serial_init(int uart_port, void *gpio_cfg, int gpio_max)
{
    u32 reg, i;
    u32 uart_clk, bus_apb_clk;

    set_pll();

    if( (uart_port < 0) ||(uart_port > 2) )
    {
        return;
    }
    //reset
    reg = readl(CCMU_BUS_SOFT_RST_REG2);
    reg &= ~(1<<(CCM_UART_PORT_OFFSET + uart_port));
    writel(reg, CCMU_BUS_SOFT_RST_REG2);
    for( i = 0; i < 100; i++ );
    reg |=  (1<<(CCM_UART_PORT_OFFSET + uart_port));
    writel(reg, CCMU_BUS_SOFT_RST_REG2);
    //gate
    reg = readl(CCMU_BUS_CLK_GATING_REG2);
    reg &= ~(1<<(CCM_UART_PORT_OFFSET + uart_port));
    writel(reg, CCMU_BUS_CLK_GATING_REG2);
    for( i = 0; i < 100; i++ );
    reg |=  (1<<(CCM_UART_PORT_OFFSET + uart_port));
    writel(reg, CCMU_BUS_CLK_GATING_REG2);
    //gpio
    boot_set_gpio(gpio_cfg, gpio_max, 1);

    //uart init
    serial_ctrl_base = (serial_hw_t *)(SUNXI_UART0_BASE + uart_port * CCM_UART_ADDR_OFFSET);
    serial_ctrl_base->lcr |= 0x80;
    bus_apb_clk = get_pll_apb_clk() ;
    uart_clk = (bus_apb_clk + 8 * UART_BAUD) / (16 * UART_BAUD);
    serial_ctrl_base->dlh = uart_clk>>8;
    serial_ctrl_base->dll = uart_clk&0xff;
    serial_ctrl_base->lcr &= ~0x80;
    serial_ctrl_base->lcr = ((PARITY&0x03)<<4) |((PARITY_ENABLE &0x1) <<3)| ((STOP&0x01)<<2) | (DLEN&0x03);
    serial_ctrl_base->fcr = 0x7;

    return;
}



void sunxi_serial_putc (char c)
{
    while((serial_ctrl_base->lsr & ( 1 << 6 )) == 0);
    serial_ctrl_base->thr = c;
    return ;
}



char sunxi_serial_getc (void)
{
    while((serial_ctrl_base->lsr & 1) == 0);
    return serial_ctrl_base->rbr;
}



int sunxi_serial_tstc (void)
{
    return serial_ctrl_base->lsr & 1;
}

