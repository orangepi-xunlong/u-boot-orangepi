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
#ifndef _DMA_H_
#define _DMA_H_

#include <asm/arch/intc.h>


#define DMAC_DMATYPE_NORMAL                     0
#define DMAC_DMATYPE_DEDICATED                  1


#define DMAC_CFG_TYPE_SPI0                      (04)
#define DMAC_CFG_TYPE_SPI1                      (05)


#define DMA_PTY_CFG_REG                         (SUNXI_DMA_BASE + 0x08)
#define DMA_CHAN_STA_REG                        (SUNXI_DMA_BASE + 0x30)


#define DMAC_CFG_CONTINUOUS_ENABLE              (0x01)
#define DMAC_CFG_CONTINUOUS_DISABLE             (0x00)

#define DMAC_CFG_DEST_DATA_WIDTH_8BIT           (0x00)
#define DMAC_CFG_DEST_DATA_WIDTH_16BIT          (0x01)
#define DMAC_CFG_DEST_DATA_WIDTH_32BIT          (0x02)


#define DMAC_CFG_DEST_1_BURST                   (0x00)
#define DMAC_CFG_DEST_4_BURST                   (0x01)


#define DMAC_CFG_DEST_ADDR_TYPE_LINEAR_MODE     (0x00)
#define DMAC_CFG_DEST_ADDR_TYPE_IO_MODE         (0x01)


#define DMAC_CFG_SRC_DATA_WIDTH_8BIT            (0x00)
#define DMAC_CFG_SRC_DATA_WIDTH_16BIT           (0x01)
#define DMAC_CFG_SRC_DATA_WIDTH_32BIT           (0x02)


#define DMAC_CFG_SRC_1_BURST                    (0x00)
#define DMAC_CFG_SRC_4_BURST                    (0x01)


#define DMAC_CFG_SRC_ADDR_TYPE_LINEAR_MODE      (0x00)
#define DMAC_CFG_SRC_ADDR_TYPE_IO_MODE          (0x01)


#define DMAC_CFG_TYPE_SRAM                      (16)
#define DMAC_CFG_TYPE_DRAM                      (17)

#define DMAC_CFG_DEST_TYPE_SRAM                 (16)
#define DMAC_CFG_DEST_TYPE_DRAM                 (17)

#define DMAC_CFG_DEST_TYPE_NAND                 (5)

#define DMAC_CFG_DEST_TYPE_CODEC                (15)

#define DMAC_CFG_DEST_TYPE_OTG_EP1              (0x15)
#define DMAC_CFG_DEST_TYPE_OTG_EP2              (0x16)
#define DMAC_CFG_DEST_TYPE_OTG_EP3              (0x17)

#define DMAC_CFG_SRC_TYPE_SRAM                  (16)
#define DMAC_CFG_SRC_TYPE_DRAM                  (17)

#define DMAC_CFG_SRC_TYPE_NAND                  (5)

#define DMAC_CFG_SRC_TYPE_CODEC                 (15)

#define DMAC_CFG_SRC_TYPE_OTG_EP1               (0x15)
#define DMAC_CFG_SRC_TYPE_OTG_EP2               (0x16)
#define DMAC_CFG_SRC_TYPE_OTG_EP3               (0x17)



typedef struct
{
    unsigned int config;
    unsigned int source_addr;
    unsigned int dest_addr;
    unsigned int byte_count;
    unsigned int pgsz;        // 0x10, just for dedicated dma
    unsigned int pgstp;       // 0x14, just for dedicated dma
    unsigned int cmt_blk_cnt; // 0x18, just for dedicated dma
    unsigned int gen_data_n3; // 0x1c, just for dedicated dma, not used!
}
sunxi_dma_start_t;


typedef struct
{
    unsigned int      src_drq_type     : 5;
    unsigned int      src_addr_mode    : 2;
    unsigned int      src_burst_length : 1;
    unsigned int      src_data_width   : 2;
    unsigned int      reserved0        : 6;
    unsigned int      dst_drq_type     : 5;
    unsigned int      dst_addr_mode    : 2;
    unsigned int      dst_burst_length : 1;
    unsigned int      dst_data_width   : 2;
    unsigned int      wait_state       : 3;
    unsigned int      continuous_mode  : 1;
    unsigned int      reserved1        : 2;
}
sunxi_dma_channal_config;

typedef struct
{
    sunxi_dma_channal_config  cfg;
    unsigned int    loop_mode;
    unsigned int    data_block_size;
    unsigned int    wait_cyc;
}
sunxi_dma_setting_t;

extern    void      sunxi_dma_init(void);
extern    void      sunxi_dma_exit(void);

extern    ulong     sunxi_dma_request(unsigned int dmatype);
extern    int       sunxi_dma_release(ulong hdma);
extern    int       sunxi_dma_setting(ulong hdma, sunxi_dma_setting_t *cfg);
extern    int       sunxi_dma_start(ulong hdma, unsigned int saddr, unsigned int daddr, unsigned int bytes);
extern    int       sunxi_dma_stop(ulong hdma);
extern    int       sunxi_dma_querystatus(ulong hdma);

extern    int       sunxi_dma_install_int(ulong hdma, interrupt_handler_t dma_int_func, void *p);
extern    int       sunxi_dma_disable_int(ulong hdma);

extern    int       sunxi_dma_enable_int(ulong hdma);
extern    int       sunxi_dma_free_int(ulong hdma);

#endif  //_DMA_H_




