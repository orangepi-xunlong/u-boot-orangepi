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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include <asm/arch/gpio.h>
#include <private_boot0.h>
#include "spi_hal.h"
#include <asm/io.h>

static u32 g_cfg_mclk = 0;

static volatile __spi_reg_t    *spi_reg;


#define SUNXI_DEBUG(fmt,args...)    printf(fmt ,##args)


#define set_wbit(addr, v)   (*((volatile unsigned long  *)(addr)) |=  (unsigned long)(v))
#define clr_wbit(addr, v)   (*((volatile unsigned long  *)(addr)) &= ~(unsigned long)(v))



extern __u32 get_pll_ahb1_clk(void);
int do_div( int divisor, int by);


static void CleanFlushDCacheRegion(void *mem, __u32 cnt)
{
    return ;
}


void spic_config_dual_mode(u32 spi_no, u32 rxdual, u32 dbc, u32 stc)
{
    writel((rxdual<<28)|(dbc<<24)|(stc), SPI_BCC);
}


static __s32 spic_clk_onoff(__u32 spi_no,__u32 onoff)
{
    __u32 reg_val;

    /* config ahb spi gating clock */
    reg_val  = *(volatile __u32 *)(CCMU_BUS_CLK_GATING_REG0);
    reg_val &= ~(0x1U << (SPI0_CKID +spi_no));
    if (onoff)
        reg_val |= 0x1U <<( SPI0_CKID +spi_no);
    *(volatile unsigned int *)(CCMU_BUS_CLK_GATING_REG0) = reg_val;


    /*  reset ahb spi module */
    reg_val  = *(volatile __u32 *)(CCMU_BUS_SOFT_RST_REG0);
    reg_val &= ~(0x1U << (SPI0_CKID +spi_no));
    if (onoff)
        reg_val |= 0x1U << (SPI0_CKID +spi_no);
    *(volatile unsigned int *)(CCMU_BUS_SOFT_RST_REG0) = reg_val;

    return 0;
}

void pattern_goto(int pos)
{
    return ;
}


u32 spi_get_mlk(u32 spi_no)
{
    return g_cfg_mclk ;//spicinfo[spi_no].sclk;
}


extern const boot0_file_head_t  BT0_head;
void spi_gpio_cfg(int spi_no)
{
    uint reg_val = 0;

    normal_gpio_cfg spi_nor_storage_gpio[4] = {
        { 3, 0, 2, 0, 0, 0, {0}},   // pc0--spi0_clck
        { 3, 1, 2, 0, 0, 0, {0}},   // pc1--spi0_cs
        { 3, 2, 2, 1, 0, 0, {0}},   // pc2--spi0_miso
        { 3, 3, 2, 0, 0, 0, {0}}    // pc3--spi0_misi
    };

    boot_set_gpio((normal_gpio_cfg *)spi_nor_storage_gpio, 4, 1);
    SUNXI_DEBUG("Reg pull reg_val=0x%x,read=0x%x\n",reg_val,readl((0x1c20800 + 0x64)));
}

void spi_onoff(u32 spi_no, u32 onoff)
{
    switch (spi_no) {
    case 0:
            spi_gpio_cfg(spi_no);
            break;
    }

    spic_clk_onoff(spi_no, onoff);
}


int spic_init(u32 spi_no)
{
    uint reg_val, div , ahb1_clk;
    spi_no = 0;

    spi_onoff(spi_no, 1);
    spi_reg =(volatile __spi_reg_t *)(SPI0_BASE + spi_no *SPIC_BASE_OS);

    reg_val = SPI_SOFT_RST|SPI_TXPAUSE_EN|SPI_MASTER|SPI_ENABLE ;
    spi_reg->global_control = reg_val;
    reg_val =  SPI_DHB | SPI_SET_SS_1 | SPI_SS_ACTIVE0 |SPI_MODE3;
    spi_reg->transfer_control = reg_val ;

        /* set spi clock */
    #ifndef FPGA_PLATFORM
        ahb1_clk = get_pll_ahb1_clk();
        div = ahb1_clk / SPI_MCLK;
        div  = div>> 1;
        if(div ==0)
            div = 1;
        div -=1;
    #else
        div = 0 ;
    #endif
    reg_val  = 1 << 12;  //select DRS1
    reg_val |= div;
    spi_reg->clock_rate_control = reg_val;

    /* enable  dma APB BUS gating clock*/
    reg_val  = readl(CCMU_BUS_CLK_GATING_REG0);
    reg_val |= 0x01 << 6;
    writel(reg_val, CCMU_BUS_CLK_GATING_REG0);

    /* disable dma auto gating clock */
    reg_val  = readl(DMA_PTY_CFG_REG);
    reg_val |= 0x01 << 16;
    writel(reg_val, DMA_PTY_CFG_REG);

    spi_reg->interrupt_control = SPI_ERROR_INT;
    spi_reg->fifo_control = SPI_TXFIFO_RST|(SPI_TX_WL<<16)|(SPI_RX_WL);

    return 0;

}

int spic_rw( u32 tcnt, void* txbuf, u32 rcnt, void* rxbuf)
{
     __u32 rdma = 0;
     __u32 config;
     u32  pspi_reg;
     __s32 time, ret = -1;
     __u8  *tx_addr, *rx_addr;
     pspi_reg = (u32)spi_reg;

     if ((tcnt + rcnt) > 64 * 1024)
     {
        SUNXI_DEBUG("(scount + rcount) > 64 * 1024 fail\n");
         return -1;
     }

     tx_addr = (__u8 *)txbuf;
     rx_addr = (__u8 *)rxbuf;
     spi_reg->interrupt_status     |= 0xffff;   // clear all the interrrupt
     spi_reg->fifo_control |= SPI_TXFIFO_RST|SPI_RXFIFO_RST;    // reset tx,rx fifo
     spi_reg->burst_counter  = (tcnt + rcnt);
     spi_reg->transmit_counter = tcnt;
     spi_reg->burst_control_counter = (0<<28)|(0<<24)|tcnt;
     spi_reg->interrupt_control |= SPI_TC_INT;

     if (rcnt > 7)
     {
         /* enable spi rx DMA enable , set DMA mode to dedicate mode ,set trigger level*/
         spi_reg->fifo_control  |=SPI_RXDMA_DMODE | SPI_RXDMAREQ_EN | (SPI_RX_WL<<0);
         /* config DMA */
         DMAC_REG_N0_SRC_ADDR = (__u32)(pspi_reg+SPI_RXD_OFFSET);
         DMAC_REG_N0_DST_ADDR = (__u32)rx_addr;
         DMAC_REG_N0_BYTE_CNT = rcnt;
         config = 0x3  << 26
                | WID8 << 24 | SINGLE << 23 | INCR     << 21
                | WID8 << 8  | SINGLE << 7  | NOCHANGE << 5 | (NDMA_SPI0);
         CleanFlushDCacheRegion(rxbuf, rcnt);

         if ((__u32)rx_addr < 0x80000000)
            config |= NDMA_SRAM  << 16;
         else
            config |= NDMA_SDRAM << 16;
         DMAC_REG_N0_CFG = config | 0x80000000; // start transfer

         rdma   = 1;
         rcnt = 0;
     }
     spi_reg->transfer_control |= SPI_EXCHANGE; // spi start to transfer and receive data

     if (tcnt)
     {
         time = 0xffffff;
         if(tcnt > 7)
         {
              /* enable spi tx DMA enable , set DMA mode to dedicate mode ,set trigger level*/
             spi_reg->fifo_control  |=  SPI_TXDMAREQ_EN |(SPI_TX_WL<<16);

             /* config DMA */
             DMAC_REG_N1_SRC_ADDR = (__u32)tx_addr;
             DMAC_REG_N1_DST_ADDR = (__u32)(pspi_reg+SPI_TXD_OFFSET);
             DMAC_REG_N1_BYTE_CNT = tcnt;
             config = 0x80000000 | 0x3    << 26
                       | WID8 << 24 | SINGLE << 23 | NOCHANGE << 21 | ((NDMA_SPI0)<<16)
                       | WID8 << 8  | SINGLE << 7  | INCR     << 5;
             CleanFlushDCacheRegion(txbuf, tcnt);

             if ((__u32)tx_addr < 0x80000000)
                config |= NDMA_SRAM;
             else
                config |= NDMA_SDRAM;
             DMAC_REG_N1_CFG = config | 0x80000000;

             /* wait DMA finish */
             while ((time-- > 0) && (DMAC_REG_N1_CFG & 0x40000000));
         }
         else
         {
             for (; tcnt > 0; tcnt--)
             {
                 *(volatile __u8 *)(pspi_reg + SPI_TXD_OFFSET) = *tx_addr;
                 tx_addr += 1;
             }
         }

     }

     time = 0xffff;
     while (rcnt && (time > 0))
     {
         if (spi_reg->interrupt_status & SPI_TC_INT)    // wait for previous byte transmit finish
         {
             *rx_addr++ = *(volatile __u8 *)(pspi_reg + SPI_RXD_OFFSET);
             --rcnt;
             time = 0xffff;
         }
         --time;
     }

     if (time <= 0)
     {
        SUNXI_DEBUG("transfer timeout1 \n");
        return ret;
     }

     if (rdma)
     {
         time = 0xffffff;
         while ((time-- > 0)&& (DMAC_REG_N0_CFG & 0x40000000));
         if (time <= 0)
         {
            SUNXI_DEBUG("reice time out\n");
             return ret;
         }
     }

     if (time > 0)
     {
         volatile __u32 tmp;

         time = 0xfffffff;
         tmp = spi_reg->interrupt_status & SPI_TC_INT ;
         while (!tmp)
         {
             tmp = spi_reg->interrupt_status & SPI_TC_INT ;
             time--;
             if (time <= 0)
             {
                SUNXI_DEBUG("transfer timeout2 \n");
                return ret;
             }
         }
     }
    return 0;
}

int spic_exit(u32 spi_no)
{
    return 0;
}

