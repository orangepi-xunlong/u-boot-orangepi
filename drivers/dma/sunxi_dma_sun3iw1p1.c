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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.     See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <malloc.h>
#include <asm/arch/dma.h>
#include <asm/arch/intc.h>
#include <asm/arch/ccmu.h>
#include <asm/io.h>
#include <asm/arch/clock.h>

#define SUNXI_DMA_MAX             4

#define SUNXI_DMA_CHANNAL_BASE    (SUNXI_DMA_BASE + 0x100)
#define SUNXI_DMA_CHANANL_SIZE    (0x20)

#ifndef SUNXI_DMA_LINK_NULL
#define SUNXI_DMA_LINK_NULL       (0xfffff800)
#endif

struct dma_irq_handler
{
    void                 *m_data;
    void (*m_func)( void *data );
};

typedef struct
{
    unsigned int irq_en;
    unsigned int irq_pending;
    unsigned int priority;
}
sunxi_dma_int_set;

typedef struct sunxi_dma_channal_set_t
{
    volatile unsigned int enable;
    volatile unsigned int pause;
    volatile unsigned int start_addr;
    volatile unsigned int config;
    volatile unsigned int cur_src_addr;
    volatile unsigned int cur_dst_addr;
    volatile unsigned int left_bytes;
    volatile unsigned int parameters;
}
sunxi_dma_channal_set;


typedef struct sunxi_dma_source_t
{
    unsigned int            used;
    unsigned int            channal_count;
    unsigned int            type;
    sunxi_dma_channal_set   *channal;
    unsigned int            reserved;
    sunxi_dma_start_t       *config;
    struct dma_irq_handler  dma_func;
}
sunxi_dma_source;

#define  DMA_PKG_HALF_INT   (0)
#define  DMA_PKG_END_INT    (1)

static int                  dma_int_not   = 0;
static sunxi_dma_source     dma_channal_source_n[SUNXI_DMA_MAX];
static sunxi_dma_source     dma_channal_source_d[SUNXI_DMA_MAX];

extern void *malloc_noncache(uint num_bytes);


static void sunxi_dma_int_func(void *p)
{
    int i;
    uint pending0;
    uint pending1;
    sunxi_dma_int_set *dma_int = (sunxi_dma_int_set *)SUNXI_DMA_BASE;

    for(i=0;i<SUNXI_DMA_MAX;i++)
    {
        if(dma_channal_source_n[i].dma_func.m_func)
        {
            pending0 = dma_int->irq_pending;
            pending1 = (1<<((dma_channal_source_n[i].channal_count<<1)+DMA_PKG_END_INT));
            if(pending0 & pending1)
            {
                dma_int->irq_pending |= pending1;
                dma_channal_source_n[i].dma_func.m_func(dma_channal_source_n[i].dma_func.m_data);
            }
        }
    }
    for(i=0;i<SUNXI_DMA_MAX;i++)
    {
        if(dma_channal_source_d[i].dma_func.m_func)
        {
            pending0 = dma_int->irq_pending;
            pending1 = (1 << ((dma_channal_source_d[i].channal_count<<1)+DMA_PKG_END_INT + 16));
            if(pending0 & pending1)
            {
                dma_int->irq_pending |= pending1;
                dma_channal_source_d[i].dma_func.m_func(dma_channal_source_d[i].dma_func.m_data);
            }
        }
    }
}



void sunxi_dma_init(void)
{
    int i;
    sunxi_dma_int_set *dma_int = (sunxi_dma_int_set *)SUNXI_DMA_BASE;

    printf("int sunxi_dma_init---\n");

    memset((void *)dma_channal_source_n, 0, SUNXI_DMA_MAX * sizeof(struct sunxi_dma_source_t));
    memset((void *)dma_channal_source_d, 0, SUNXI_DMA_MAX * sizeof(struct sunxi_dma_source_t));

    for(i=0;i<SUNXI_DMA_MAX;i++)
    {
        dma_channal_source_n[i].used   = 0;
        dma_channal_source_n[i].type   = 0;
        dma_channal_source_n[i].config = ( sunxi_dma_start_t *)(SUNXI_DMA_BASE + i * SUNXI_DMA_CHANANL_SIZE + 0x100);
    }

    for(i=0;i<SUNXI_DMA_MAX;i++)
    {
        dma_channal_source_d[i].used   = 0;
        dma_channal_source_d[i].type   = 1;
        dma_channal_source_d[i].config = ( sunxi_dma_start_t *)(SUNXI_DMA_BASE + i * SUNXI_DMA_CHANANL_SIZE + 0x300);
    }

    //dma reset
    writel(readl(CCMU_BUS_SOFT_RST_REG0)   | (1 << 6), CCMU_BUS_SOFT_RST_REG0);
    //gating clock for dma pass
    writel(readl(CCMU_BUS_CLK_GATING_REG0) | (1 << 6), CCMU_BUS_CLK_GATING_REG0);

    dma_int->irq_en      = 0;
    dma_int->irq_pending = 0xffffffff;

    irq_install_handler(AW_IRQ_DMA, sunxi_dma_int_func, 0);
    printf("irq enable\n");
    irq_enable(AW_IRQ_DMA);

    return ;
}

void sunxi_dma_exit(void)
{
    int i;
    ulong hdma;
    sunxi_dma_int_set *dma_int = (sunxi_dma_int_set *)SUNXI_DMA_BASE;
    printf("sunxi dma exit\n");
    //free dma channal if other module not free it
    for(i=0;i<SUNXI_DMA_MAX;i++)
    {
        if(dma_channal_source_d[i].used == 1)
        {
            hdma = (ulong)&dma_channal_source_d[i];
            sunxi_dma_disable_int(hdma);
            dma_channal_source_d[i].used   = 0;
        }

        if(dma_channal_source_n[i].used == 1)
        {
            hdma = (ulong)&dma_channal_source_n[i];
            sunxi_dma_disable_int(hdma);
            dma_channal_source_d[i].used   = 0;
        }
    }

    //irp disable
    dma_int->irq_en      = 0;
    dma_int->irq_pending = 0xffffffff;
    dma_int->priority   |= (0x01 << 16);

    irq_disable(AW_IRQ_DMA);
    irq_free_handler(AW_IRQ_DMA);

    return ;
}
/*
****************************************************************************************************
*
*             DMAC_RequestDma
*
*  Description:
*       request dma
*
*  Parameters:
*        type    0: normal timer
*                1: special timer
*  Return value:
*        dma handler
*        if 0, fail
****************************************************************************************************
*/
ulong sunxi_dma_request(uint dmatype)
{
    int   i;

    if(dma_int_not++ == 0)
    {
        sunxi_dma_init();
    }
    if(dmatype == 0)
    {
        for(i=0;i<SUNXI_DMA_MAX;i++)
        {
            if(dma_channal_source_n[i].used == 0)
            {
                dma_channal_source_n[i].type          = 0;
                dma_channal_source_n[i].used          = 1;
                dma_channal_source_n[i].channal_count = i;

                return (ulong)&dma_channal_source_n[i];
            }
        }
        printf("dmatype=0 and all chanal is used\n");

    }
    else if(dmatype == 1)
    {
        for(i=0;i<SUNXI_DMA_MAX;i++)
        {
            if(dma_channal_source_d[i].used == 0)
            {
                dma_channal_source_n[i].type          = 1;
                dma_channal_source_d[i].used          = 1;
                dma_channal_source_d[i].channal_count = i;

                return (ulong)&dma_channal_source_d[i];
            }
        }
        printf("dmatype=0 and all chanal is used\n");
    }

    return 0;
}
/*
****************************************************************************************************
*
*             DMAC_ReleaseDma
*
*  Description:
*       release dma
*
*  Parameters:
*       hDma    dma handler
*
*  Return value:
*        EPDK_OK/FAIL
****************************************************************************************************
*/
int sunxi_dma_release(ulong hdma)
{
    struct sunxi_dma_source_t  *dma_channal = (struct sunxi_dma_source_t *)hdma;
    if(!dma_channal->used)
    {
        return -1;
    }

    sunxi_dma_disable_int(hdma);
    dma_channal->channal->enable = 0;
    dma_channal->used   = 0;

    if(--dma_int_not == 0)
    {
        sunxi_dma_exit();
    }

    return 0;
}

int sunxi_dma_setting(ulong hdma, sunxi_dma_setting_t *cfg)
{
    uint   *config_addr;
    sunxi_dma_setting_t           *dma_set     = cfg;
    struct sunxi_dma_source_t     *dma_channal = (struct sunxi_dma_source_t *)hdma;

    if(!dma_channal->used)
    {
        return -1;
    }

    config_addr                 = (uint *)&(dma_set->cfg);
    dma_channal->config->config = *config_addr;

    return 0;
}


int sunxi_dma_start(ulong hdma, uint saddr, uint daddr, uint bytes)
{
    struct sunxi_dma_source_t      *dma_channal = (struct sunxi_dma_source_t *)hdma;
    uint   config_para;

    if(!dma_channal->used)
    {
        return -1;
    }

    config_para  = dma_channal->config->config;
    config_para |= 0x80000000;

    dma_channal->config->source_addr = saddr;
    dma_channal->config->dest_addr   = daddr;
    dma_channal->config->byte_count  = bytes;
    dma_channal->config->config      = config_para;

    return 0;
}

int sunxi_dma_stop(ulong hdma)
{
    struct sunxi_dma_source_t      *dma_channal = (struct sunxi_dma_source_t *)hdma;
    uint   config_para;

    if(!dma_channal->used)
    {
        return -1;
    }

    config_para  = dma_channal->config->config;
    config_para &= 0x7fffffff;
    dma_channal->config->config = config_para;

    return 0;
}

int sunxi_dma_querystatus(ulong hdma)
{
    struct sunxi_dma_source_t      *dma_channal = (struct sunxi_dma_source_t *)hdma;

    if(!dma_channal->used)
    {
        return -1;
    }

    return (dma_channal->config->config >> 31) & 0x01;

}

int sunxi_dma_install_int(ulong hdma, interrupt_handler_t dma_int_func, void *p)
{
    sunxi_dma_source     *dma_channal = (sunxi_dma_source  *)hdma;


    if(!dma_channal->used)
    {
        return -1;
    }

    if(!dma_channal->dma_func.m_func)
    {
        dma_channal->dma_func.m_func = dma_int_func;
        dma_channal->dma_func.m_data = p;
    }
    else
    {
        printf("dma 0x%lx int is used already, you have to free it first\n", hdma);

        return -1;
    }

    printf("sunxi_dma_install_int ok\n");

    return 0;
}

int sunxi_dma_enable_int(ulong hdma)
{
    sunxi_dma_source     *dma_channal = (sunxi_dma_source *)hdma;
    sunxi_dma_int_set    *dma_status  = (sunxi_dma_int_set *)SUNXI_DMA_BASE;

    if(!dma_channal->used)
    {
        return -1;
    }

    if(dma_channal->type == 0)
    {
        dma_status->irq_pending = (1 <<((dma_channal->channal_count<<1) + DMA_PKG_END_INT));
        //enable dma full transfer interrupt
        dma_status->irq_en |= (1 <<((dma_channal->channal_count<<1) + DMA_PKG_END_INT));
    }
    else
    {
        dma_status->irq_pending = (1 <<((dma_channal->channal_count<<1) + DMA_PKG_END_INT + 16));
        //enable dma full transfer interrupt
        dma_status->irq_en |= (1 <<((dma_channal->channal_count<<1) + DMA_PKG_END_INT + 16));
    }

    return 0;
}

int sunxi_dma_disable_int(ulong hdma)
{
    sunxi_dma_source     *dma_channal = (sunxi_dma_source *)hdma;
    sunxi_dma_int_set    *dma_status  = (sunxi_dma_int_set *)SUNXI_DMA_BASE;

    if(!dma_channal->used)
    {
        return -1;
    }


    if(dma_channal->type == 0)
    {
        /*disable half interrupt and full interrupt all*/
        dma_status->irq_en &= ~(3<<((dma_channal->channal_count<<1)));
    }
    else
    {
        /*disable half interrupt and full interrupt all*/
        dma_status->irq_en &= ~(3<<((dma_channal->channal_count<<1)+ 16));
    }

    return 0;
}

int sunxi_dma_free_int(ulong hdma)
{
    sunxi_dma_source     *dma_channal = (sunxi_dma_source *)hdma;

    if(dma_channal->dma_func.m_func)
    {
        dma_channal->dma_func.m_func = NULL;
        dma_channal->dma_func.m_data = NULL;
    }
    else
    {
        printf("dma 0x%lx int is free, you do not need to free it again\n", hdma);
        return -1;
    }

    return 0;
}

