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
#include "usb_base.h"
#include <scsi.h>
#include <asm/arch/intc.h>
#include <sys_partition.h>
#include <sunxi_board.h>
#include <asm/arch/dma.h>

#define  SUNXI_USB_EP0_BUFFER_SIZE       (512)

#define  HIGH_SPEED_EP_MAX_PACKET_SIZE   (512)
#define  FULL_SPEED_EP_MAX_PACKET_SIZE   (64)

#define  BULK_FIFOSIZE                   (512)

#define  SUNXI_USB_CTRL_EP_INDEX            0
#define  SUNXI_USB_BULK_IN_EP_INDEX         1   /* tx */
#define  SUNXI_USB_BULK_OUT_EP_INDEX        2   /* rx */

static uchar sunxi_usb_ep0_buffer[SUNXI_USB_EP0_BUFFER_SIZE];

sunxi_udc_t               sunxi_udc_source;
static sunxi_ubuf_t       sunxi_ubuf;
sunxi_usb_setup_req_t     *sunxi_udev_active;


static int  __usb_read_ep0_data (void  *buffer, uint data_type);
static int  __usb_write_fifo    (uchar *buffer, unsigned int buffer_size);
static void __usb_bulk_ep_reset (void);
static void __usb_clear_all_irq (void);

static void __usb_writecomplete (__hdle hUSB, u32 ep_type, u32 complete);
static void __usb_readcomplete  (__hdle hUSB, u32 ep_type, u32 complete);

static void __usb_recv_by_dma_isr(void *p_arg);

static int eptx_send_op(void);
static int eprx_recv_op(void);
static int ep0_recv_op (void);

#ifdef CONFIG_FASTBOOT
extern int fastboot_data_flag;
#endif

extern volatile int sunxi_usb_burn_from_boot_init;

void sunxi_usb_irq(void *data)
{
    u8  misc_irq   = 0;
    u16 tx_irq     = 0;
    u16 rx_irq     = 0;
    u32 old_ep_idx = 0;

    /* Save index */
    old_ep_idx = USBC_GetActiveEp(sunxi_udc_source.usbc_hd);

    /* Read status registers */
    misc_irq = USBC_INT_MiscPending(sunxi_udc_source.usbc_hd);
    tx_irq   = USBC_INT_EpPending(sunxi_udc_source.usbc_hd, USBC_EP_TYPE_TX);
    rx_irq   = USBC_INT_EpPending(sunxi_udc_source.usbc_hd, USBC_EP_TYPE_RX);

    /* RESET */
    if(misc_irq & USBC_INTUSB_RESET)
    {
        sunxi_usb_dbg("IRQ: reset\n");

        USBC_INT_ClearMiscPending(sunxi_udc_source.usbc_hd, USBC_INTUSB_RESET);
        __usb_clear_all_irq();

        USBC_SelectActiveEp(sunxi_udc_source.usbc_hd, 0);
        USBC_Dev_SetAddress_default(sunxi_udc_source.usbc_hd);

        sunxi_udc_source.address = 0;                     //default value
        sunxi_udc_source.speed   = USB_SPEED_HIGH;        //default value

        sunxi_dma_stop(sunxi_udc_source.dma_recv_channal);
        sunxi_dma_stop(sunxi_udc_source.dma_send_channal);

        sunxi_ubuf.rx_ready_for_data = 0;
        sunxi_udev_active->state_reset();

        return ;
    }

    /* RESUME */
    if (misc_irq & USBC_INTUSB_RESUME)
    {
        sunxi_usb_dbg("IRQ: resume\n");
        /* clear interrupt */
        USBC_INT_ClearMiscPending(sunxi_udc_source.usbc_hd, USBC_INTUSB_RESUME);
    }

    /* SUSPEND */
    if (misc_irq & USBC_INTUSB_SUSPEND)
    {
        sunxi_usb_dbg("IRQ: suspend\n");
        /* clear interrupt */
        USBC_INT_ClearMiscPending(sunxi_udc_source.usbc_hd, USBC_INTUSB_SUSPEND);
    }

    /* DISCONNECT */
    if(misc_irq & USBC_INTUSB_DISCONNECT)
    {
        sunxi_usb_dbg("IRQ: disconnect\n");

        USBC_INT_ClearMiscPending(sunxi_udc_source.usbc_hd, USBC_INTUSB_DISCONNECT);

        return ;
    }
    /* SOF */
    if(misc_irq & USBC_INTUSB_SOF)
    {
        sunxi_usb_dbg("IRQ: SOF\n");
        sunxi_usb_burn_from_boot_init = 1;
        USBC_INT_DisableUsbMiscUint(sunxi_udc_source.usbc_hd, USBC_INTUSB_SOF);
        USBC_INT_ClearMiscPending(sunxi_udc_source.usbc_hd, USBC_INTUSB_SOF);
    }
    /* ep0 */
    if (tx_irq & (1 << SUNXI_USB_CTRL_EP_INDEX) )
    {
        sunxi_usb_dbg("IRQ: EP0\n");

        USBC_INT_ClearEpPending(sunxi_udc_source.usbc_hd, USBC_EP_TYPE_TX, SUNXI_USB_CTRL_EP_INDEX);
        ep0_recv_op();

    }

    /* tx endpoint data transfers */
    if (tx_irq & (1 << SUNXI_USB_BULK_IN_EP_INDEX))
    {
        sunxi_usb_dbg("tx irq occur\n");
        /* Clear the interrupt bit by setting it to 1 */
        USBC_INT_ClearEpPending(sunxi_udc_source.usbc_hd, USBC_EP_TYPE_TX, SUNXI_USB_BULK_IN_EP_INDEX);

        eptx_send_op();

    }
    /* rx endpoint data transfers */
    if (rx_irq & (1 << SUNXI_USB_BULK_OUT_EP_INDEX))
    {
        sunxi_usb_dbg("rx irq occur\n");
        /* Clear the interrupt bit by setting it to 1 */
        USBC_INT_ClearEpPending(sunxi_udc_source.usbc_hd, USBC_EP_TYPE_RX, SUNXI_USB_BULK_OUT_EP_INDEX);

        eprx_recv_op();

    }

    USBC_SelectActiveEp(sunxi_udc_source.usbc_hd, old_ep_idx);


    return ;
}


int sunxi_usb_recv_dma_reconfig(int ram_type)
{
    sunxi_dma_setting_t  usb_recv_by_dma;

    //config recv(from usb fifo to dram)
    usb_recv_by_dma.cfg.src_drq_type     = DMAC_CFG_SRC_TYPE_OTG_EP2; //OTG EP2
    usb_recv_by_dma.cfg.src_addr_mode    = DMAC_CFG_SRC_ADDR_TYPE_IO_MODE;
    usb_recv_by_dma.cfg.src_burst_length = DMAC_CFG_SRC_1_BURST;
    usb_recv_by_dma.cfg.src_data_width   = DMAC_CFG_SRC_DATA_WIDTH_32BIT;

    usb_recv_by_dma.cfg.dst_drq_type     = ram_type;                  //DRAM
    usb_recv_by_dma.cfg.dst_addr_mode    = DMAC_CFG_DEST_ADDR_TYPE_LINEAR_MODE;
    usb_recv_by_dma.cfg.dst_burst_length = DMAC_CFG_DEST_1_BURST;
    usb_recv_by_dma.cfg.dst_data_width   = DMAC_CFG_DEST_DATA_WIDTH_32BIT;

    usb_recv_by_dma.cfg.wait_state       = 4;
    usb_recv_by_dma.cfg.continuous_mode  = 0;

    sunxi_dma_setting(sunxi_udc_source.dma_recv_channal, &usb_recv_by_dma);

    return 0;

}

int sunxi_usb_init(int delaytime)
{
    if(sunxi_udev_active->state_init())
    {
        printf("sunxi usb err: fail to init usb device\n");

        return -1;
    }
    irq_disable(AW_IRQ_USB_OTG);
    memset(&sunxi_udc_source, 0, sizeof(sunxi_udc_t));
    sunxi_udc_source.usbc_hd = USBC_open_otg(0);
    if(sunxi_udc_source.usbc_hd == 0)
    {
        printf("sunxi usb err : USBC_open_otg failed\n");

        return -1;
    }
    USBC_Dev_ConectSwitch(sunxi_udc_source.usbc_hd, USBC_DEVICE_SWITCH_OFF);
    usb_close_clock();

    printf("delay time %d\n", delaytime);
    __msdelay(delaytime);

    sunxi_udc_source.dma_recv_channal = sunxi_dma_request(0);
    if(!sunxi_udc_source.dma_recv_channal)
    {
        printf("sunxi usb err : unable to request dma for usb receive data\n");

        goto __sunxi_usb_init_fail;
    }

    sunxi_dma_install_int(sunxi_udc_source.dma_recv_channal, __usb_recv_by_dma_isr, 0);
    sunxi_usb_dbg("enable dma isr\n");
    sunxi_dma_enable_int(sunxi_udc_source.dma_recv_channal);

    sunxi_udc_source.address             = 0;
    sunxi_udc_source.speed               = USB_SPEED_HIGH;
    sunxi_udc_source.bulk_ep_max         = HIGH_SPEED_EP_MAX_PACKET_SIZE;
    sunxi_udc_source.fifo_size           = BULK_FIFOSIZE;
    sunxi_udc_source.bulk_in_addr        = 100;
    sunxi_udc_source.bulk_out_addr       = sunxi_udc_source.bulk_in_addr + sunxi_udc_source.fifo_size * 2;

    memset(&sunxi_ubuf, 0, sizeof(sunxi_ubuf_t));

    sunxi_ubuf.rx_base_buffer = (uchar *)malloc(1024);
    if(!sunxi_ubuf.rx_base_buffer)
    {
        printf("sunxi usb err: fail to malloc memory for rx command buffer\n");

        goto __sunxi_usb_init_fail;
    }
    sunxi_ubuf.rx_req_buffer = sunxi_ubuf.rx_base_buffer;

    usb_open_clock();



    USBC_ForceId(sunxi_udc_source.usbc_hd, USBC_ID_TYPE_DEVICE);
    USBC_ForceVbusValid(sunxi_udc_source.usbc_hd, USBC_VBUS_TYPE_HIGH);

    USBC_Dev_ConectSwitch(sunxi_udc_source.usbc_hd, USBC_DEVICE_SWITCH_OFF);
    USBC_EnableDpDmPullUp(sunxi_udc_source.usbc_hd);
    USBC_EnableIdPullUp(sunxi_udc_source.usbc_hd);
    USBC_SelectBus(sunxi_udc_source.usbc_hd, USBC_IO_TYPE_PIO, 0, 0);
    USBC_ConfigFIFO_Base(sunxi_udc_source.usbc_hd, 0, 0);

    USBC_EnhanceSignal(sunxi_udc_source.usbc_hd);
#ifdef  CONFIG_USB_1_1_DEVICE
    USBC_Dev_ConfigTransferMode(sunxi_udc_source.usbc_hd, USBC_TS_TYPE_BULK, USBC_TS_MODE_FS);
#else
    USBC_Dev_ConfigTransferMode(sunxi_udc_source.usbc_hd, USBC_TS_TYPE_BULK, USBC_TS_MODE_HS);
#endif
    /* disable all interrupt */
    USBC_INT_DisableUsbMiscAll(sunxi_udc_source.usbc_hd);
    USBC_INT_DisableEpAll(sunxi_udc_source.usbc_hd, USBC_EP_TYPE_RX);
    USBC_INT_DisableEpAll(sunxi_udc_source.usbc_hd, USBC_EP_TYPE_TX);

    USBC_INT_EnableUsbMiscUint(sunxi_udc_source.usbc_hd, USBC_INTUSB_SUSPEND | USBC_INTUSB_RESUME   \
                                                       | USBC_INTUSB_RESET);


    /* enbale ep0_tx_irq */
    USBC_INT_EnableEp(sunxi_udc_source.usbc_hd, USBC_EP_TYPE_TX, SUNXI_USB_CTRL_EP_INDEX);

    __usb_bulk_ep_reset();

    USBC_Dev_ConectSwitch(sunxi_udc_source.usbc_hd, USBC_DEVICE_SWITCH_ON);

    irq_install_handler(AW_IRQ_USB_OTG, sunxi_usb_irq, NULL);

    irq_enable(AW_IRQ_USB_OTG);


    return 0;

__sunxi_usb_init_fail:
    if(sunxi_udc_source.usbc_hd)
    {
        USBC_close_otg(sunxi_udc_source.usbc_hd);
    }

    if(sunxi_udc_source.dma_send_channal)
    {
        sunxi_dma_release(sunxi_udc_source.dma_send_channal);
    }
    if(sunxi_udc_source.dma_recv_channal)
    {
        sunxi_dma_release(sunxi_udc_source.dma_recv_channal);
    }

    if(sunxi_ubuf.rx_base_buffer)
    {
        free(sunxi_ubuf.rx_base_buffer);
    }

    return -1;
}


int sunxi_usb_exit(void)
{
    if(sunxi_udc_source.dma_send_channal)
    {
        sunxi_dma_release(sunxi_udc_source.dma_send_channal);
    }
    if(sunxi_udc_source.dma_recv_channal)
    {
        sunxi_dma_release(sunxi_udc_source.dma_recv_channal);
    }

    if(sunxi_ubuf.rx_base_buffer)
    {
        free(sunxi_ubuf.rx_base_buffer);
    }
    USBC_close_otg(sunxi_udc_source.usbc_hd);

    irq_disable(AW_IRQ_USB_OTG);
    irq_free_handler(AW_IRQ_USB_OTG);

    usb_close_clock();

    sunxi_udev_active->state_exit();

    memset(&sunxi_ubuf, 0, sizeof(sunxi_ubuf_t));

    return 0;
}



static void __usb_recv_by_dma_isr(void *p_arg)
{
    u32 old_ep_idx;

    sunxi_usb_dbg("in  usb_recv_by_dma_isr fun --\n");
    old_ep_idx = USBC_GetActiveEp(sunxi_udc_source.usbc_hd);
    USBC_SelectActiveEp(sunxi_udc_source.usbc_hd, SUNXI_USB_BULK_OUT_EP_INDEX);
    if(sunxi_ubuf.request_size % sunxi_udc_source.bulk_ep_max)
    {
        USBC_Dev_ReadDataStatus(sunxi_udc_source.usbc_hd, USBC_EP_TYPE_RX, 1);
    }
    sunxi_usb_dbg("select io mode to transfer data\n");
    USBC_Dev_ClearEpDma(sunxi_udc_source.usbc_hd, USBC_EP_TYPE_RX);

    USBC_SelectActiveEp(sunxi_udc_source.usbc_hd, old_ep_idx);

    sunxi_udev_active->dma_rx_isr(p_arg);
}


static void __usb_clear_all_irq(void)
{
    USBC_INT_ClearEpPendingAll(sunxi_udc_source.usbc_hd, USBC_EP_TYPE_TX);
    USBC_INT_ClearEpPendingAll(sunxi_udc_source.usbc_hd, USBC_EP_TYPE_RX);
    USBC_INT_ClearMiscPendingAll(sunxi_udc_source.usbc_hd);
}


static void __usb_readcomplete(__hdle hUSB, u32 ep_type, u32 complete)
{
    USBC_Dev_ReadDataStatus(hUSB, ep_type, complete);

    while(USBC_Dev_IsReadDataReady(hUSB, ep_type));


    if(ep_type == USBC_EP_TYPE_EP0)
    {
        /* clear data end */
        if(complete)
        {
            USBC_Dev_Ctrl_ClearSetupEnd(hUSB);
        }

        /* clear irq */
        USBC_INT_ClearEpPending(hUSB, USBC_EP_TYPE_TX, SUNXI_USB_CTRL_EP_INDEX);
    }

    return;
}


static void __usb_writecomplete(__hdle hUSB, u32 ep_type, u32 complete)
{
    USBC_Dev_WriteDataStatus(hUSB, ep_type, complete);

    /* wait for tx packet sent out */
    while(USBC_Dev_IsWriteDataReady(hUSB, ep_type));

    if(ep_type == USBC_EP_TYPE_EP0)
    {
        /* clear data end */
        if(complete)
        {
            USBC_Dev_Ctrl_ClearSetupEnd(hUSB);
        }
        /* clear irq */
        USBC_INT_ClearEpPending(hUSB, USBC_EP_TYPE_TX, SUNXI_USB_CTRL_EP_INDEX);
    }

    return;
}


static void __usb_bulk_ep_reset (void)
{
    u8 old_ep_index = 0;

    old_ep_index = USBC_GetActiveEp(sunxi_udc_source.usbc_hd);
    /* tx */
    USBC_SelectActiveEp (sunxi_udc_source.usbc_hd, SUNXI_USB_BULK_IN_EP_INDEX);
    USBC_Dev_ConfigEp   (sunxi_udc_source.usbc_hd, USBC_TS_TYPE_BULK, USBC_EP_TYPE_TX, 0, sunxi_udc_source.bulk_ep_max & 0x7ff);
    USBC_ConfigFifo     (sunxi_udc_source.usbc_hd, USBC_EP_TYPE_TX, 0, sunxi_udc_source.fifo_size, ( uint)sunxi_udc_source.bulk_out_addr);
    USBC_INT_EnableEp   (sunxi_udc_source.usbc_hd, USBC_EP_TYPE_TX, SUNXI_USB_BULK_IN_EP_INDEX);
    /* rx */
    USBC_SelectActiveEp (sunxi_udc_source.usbc_hd, SUNXI_USB_BULK_OUT_EP_INDEX);
    USBC_Dev_ConfigEp   (sunxi_udc_source.usbc_hd, USBC_TS_TYPE_BULK, USBC_EP_TYPE_RX,0, sunxi_udc_source.bulk_ep_max & 0x7ff);
    USBC_ConfigFifo     (sunxi_udc_source.usbc_hd, USBC_EP_TYPE_RX, 0, sunxi_udc_source.fifo_size, ( uint)sunxi_udc_source.bulk_in_addr);
    USBC_INT_EnableEp   (sunxi_udc_source.usbc_hd, USBC_EP_TYPE_RX, SUNXI_USB_BULK_OUT_EP_INDEX);

    USBC_SelectActiveEp (sunxi_udc_source.usbc_hd, old_ep_index);

    return;
}


static int __usb_read_ep0_data(void *buffer, uint data_type)
{
    u32 fifo_count   = 0;
    u32 fifo         = 0;
    int ret          = 0;
    u32 old_ep_index = 0;

    old_ep_index = USBC_GetActiveEp(sunxi_udc_source.usbc_hd);
    fifo = USBC_SelectFIFO(sunxi_udc_source.usbc_hd, SUNXI_USB_CTRL_EP_INDEX);
    fifo_count = USBC_ReadLenFromFifo(sunxi_udc_source.usbc_hd, USBC_EP_TYPE_EP0);
    if(!data_type)
    {
        if(fifo_count != 8 )
        {
            printf("err: ep0 fifo_count %d is not 8\n", fifo_count);

            return -1;
        }
    }
    USBC_ReadPacket(sunxi_udc_source.usbc_hd, fifo, fifo_count, (void *)buffer);
    __usb_readcomplete(sunxi_udc_source.usbc_hd, USBC_EP_TYPE_EP0, 1);
    USBC_SelectActiveEp(sunxi_udc_source.usbc_hd, old_ep_index);

    return ret;
}

static int __usb_write_fifo(uchar *buffer, unsigned int buffer_size)
{
    u32 old_ep_idx = 0;
    u32 fifo       = 0;
    u32 transfered = 0;
    u32 left       = 0;
    u32 this_len;

    /* Save index */
    old_ep_idx = USBC_GetActiveEp(sunxi_udc_source.usbc_hd);
    USBC_SelectActiveEp(sunxi_udc_source.usbc_hd, SUNXI_USB_BULK_IN_EP_INDEX);

    left = buffer_size;
    fifo = USBC_SelectFIFO(sunxi_udc_source.usbc_hd, SUNXI_USB_BULK_IN_EP_INDEX);
    printf("---%d,fifo = %x fifo_size = %d\n",left,fifo,sunxi_udc_source.fifo_size);
    while(left)
    {
        this_len = MIN(sunxi_udc_source.fifo_size, left);
        this_len = USBC_WritePacket(sunxi_udc_source.usbc_hd, fifo, this_len, buffer + transfered);

        transfered += this_len;
        left -= this_len;
        printf("-left=%d , this_len = %d,transfered=%d\n",left, this_len,transfered);

        __usb_writecomplete(sunxi_udc_source.usbc_hd, USBC_EP_TYPE_TX, 1);
        printf("-1--%d\n",left);
    }


    USBC_SelectActiveEp(sunxi_udc_source.usbc_hd, old_ep_idx);

    return 0;
}


void sunxi_udc_ep_reset(void)
{
    __usb_bulk_ep_reset();
}



int sunxi_udc_start_recv_by_dma(void* mem_base, uint length)
{
    uint old_ep_idx;
    uint fifo;
    ulong mem_buf = (ulong)mem_base;

    sunxi_usb_dbg("sub_base_sun3iw1p1.c%d, length = %x\n",__LINE__, length);
    if(length<=512)
    {
        old_ep_idx = USBC_GetActiveEp(sunxi_udc_source.usbc_hd);
        USBC_SelectActiveEp(sunxi_udc_source.usbc_hd, SUNXI_USB_BULK_OUT_EP_INDEX);
        fifo = USBC_SelectFIFO(sunxi_udc_source.usbc_hd, SUNXI_USB_BULK_OUT_EP_INDEX);
        flush_cache((uint)mem_buf, length);
        USBC_ReadPacket(sunxi_udc_source.usbc_hd, fifo, length, mem_base);
        sunxi_ubuf.request_size = length;
        __usb_readcomplete(sunxi_udc_source.usbc_hd, USBC_EP_TYPE_RX, 1);
        USBC_SelectActiveEp(sunxi_udc_source.usbc_hd, old_ep_idx);
        sunxi_ubuf.request_size = length;

        sunxi_udev_active->dma_rx_isr(0);
        return 0;
    }
    else
    {
        if(mem_buf >= CONFIG_SYS_SDRAM_BASE)
        {
            sunxi_usb_recv_dma_reconfig(DMAC_CFG_TYPE_DRAM);
        }
        else
        {
            sunxi_usb_recv_dma_reconfig(DMAC_CFG_TYPE_SRAM);
        }

        old_ep_idx = USBC_GetActiveEp(sunxi_udc_source.usbc_hd);
        USBC_SelectActiveEp(sunxi_udc_source.usbc_hd, SUNXI_USB_BULK_OUT_EP_INDEX);

        USBC_Dev_ConfigEpDma(sunxi_udc_source.usbc_hd, USBC_EP_TYPE_RX);

        fifo = USBC_SelectFIFO(sunxi_udc_source.usbc_hd, SUNXI_USB_BULK_OUT_EP_INDEX);

        flush_cache(mem_buf, length);

        sunxi_ubuf.request_size = length;

        sunxi_dma_start(sunxi_udc_source.dma_recv_channal, fifo, mem_buf, length);

        USBC_SelectActiveEp(sunxi_udc_source.usbc_hd, old_ep_idx);
    }

    return 0;
}



void sunxi_udc_send_setup(uint bLength, void *buffer)
{
    u32 fifo = 0;

    if(!bLength)
    {
        /* sent zero packet */
        __usb_writecomplete(sunxi_udc_source.usbc_hd, USBC_EP_TYPE_EP0, 1);
    }
    else
    {
        fifo = USBC_SelectFIFO(sunxi_udc_source.usbc_hd, SUNXI_USB_CTRL_EP_INDEX);
        USBC_WritePacket(sunxi_udc_source.usbc_hd, fifo, bLength, (void *)buffer);
        __usb_writecomplete(sunxi_udc_source.usbc_hd, USBC_EP_TYPE_EP0, 1);
    }
}


int sunxi_udc_set_configuration(int config_param)
{
    return 0;
}



int sunxi_udc_set_address(uchar address)
{
    USBC_Dev_SetAddress(sunxi_udc_source.usbc_hd, address);
    if(USBC_Dev_QueryTransferMode(sunxi_udc_source.usbc_hd) == USBC_TS_MODE_HS)
    {
        sunxi_udc_source.speed = USB_SPEED_HIGH;
        sunxi_udc_source.fifo_size = HIGH_SPEED_EP_MAX_PACKET_SIZE;
        sunxi_usb_dbg("usb speed: HIGH\n");
    }
    else
    {
        sunxi_udc_source.speed = USB_SPEED_FULL;
        sunxi_udc_source.fifo_size = FULL_SPEED_EP_MAX_PACKET_SIZE;
        sunxi_usb_dbg("usb speed: FULL\n");
    }

    return SUNXI_USB_REQ_SUCCESSED;
}



int sunxi_udc_send_data(void *buffer, unsigned int buffer_size)
{
    sunxi_ubuf.rx_ready_for_data = 0;

    return __usb_write_fifo((uchar *)buffer, buffer_size);
}



int sunxi_udc_get_ep_max(void)
{
    return  sunxi_udc_source.bulk_ep_max;
}


int sunxi_udc_get_ep_in_type(void)
{
    return  (0x80 | SUNXI_USB_BULK_IN_EP_INDEX);
}


int sunxi_udc_get_ep_out_type(void)
{
    return  SUNXI_USB_BULK_OUT_EP_INDEX;
}


static int ep0_recv_op(void)
{
    u32 old_ep_index  = 0;
    int ret = 0;
    static uint ep0_stage = 0;

    if(!ep0_stage)
    {
        memset(&sunxi_udc_source.standard_reg, 0, sizeof(struct usb_device_request));
    }

    old_ep_index = USBC_GetActiveEp(sunxi_udc_source.usbc_hd);
    USBC_SelectActiveEp(sunxi_udc_source.usbc_hd, SUNXI_USB_CTRL_EP_INDEX);
    //clear stall status
    if(USBC_Dev_IsEpStall(sunxi_udc_source.usbc_hd, USBC_EP_TYPE_EP0))
    {
        printf("ERR: handle_ep0: ep0 stall\n");

        USBC_Dev_EpClearStall(sunxi_udc_source.usbc_hd, USBC_EP_TYPE_EP0);
        ret = -1;

        goto __ep0_recv_op_err;
    }

    //clear setup end
    if (USBC_Dev_Ctrl_IsSetupEnd(sunxi_udc_source.usbc_hd))
    {
        USBC_Dev_Ctrl_ClearSetupEnd(sunxi_udc_source.usbc_hd);
    }

    if(USBC_Dev_IsReadDataReady(sunxi_udc_source.usbc_hd, USBC_EP_TYPE_EP0))
    {
        uint status;

        if(!ep0_stage)
        {
            status = __usb_read_ep0_data(&sunxi_udc_source.standard_reg, ep0_stage);
        }
        else
        {
            status = __usb_read_ep0_data(sunxi_usb_ep0_buffer, ep0_stage);
        }
        if(status!= 0)
        {
            printf("sunxi usb err: read_request failed\n");
            ret = -1;

            goto __ep0_recv_op_err;
        }
    }
    else
    {
        sunxi_usb_dbg("sunxi usb msg: ep0 rx data is not ready\n");

        goto __ep0_recv_op_err;
    }



    /* Check data */
    if(USB_REQ_TYPE_STANDARD == (sunxi_udc_source.standard_reg.bmRequestType & USB_REQ_TYPE_MASK))
    {
        ret = SUNXI_USB_REQ_UNMATCHED_COMMAND;

        /* standard */
        switch(sunxi_udc_source.standard_reg.bRequest)
        {
            case USB_REQ_GET_STATUS:        //   0x00
            {
                /* device-to-host */
                if(USB_DIR_IN == (sunxi_udc_source.standard_reg.bmRequestType & USB_REQ_DIRECTION_MASK))
                {
                    ret = sunxi_udev_active->standard_req_op(USB_REQ_GET_STATUS, &sunxi_udc_source.standard_reg, sunxi_usb_ep0_buffer);
                }

                break;
            }
            case USB_REQ_CLEAR_FEATURE:     //   0x01
            {
                /* host-to-device */
                if(USB_DIR_OUT == (sunxi_udc_source.standard_reg.bmRequestType & USB_REQ_DIRECTION_MASK))
                {
                    ret = sunxi_udev_active->standard_req_op(USB_REQ_CLEAR_FEATURE, &sunxi_udc_source.standard_reg, NULL);
                }

                break;
            }
            case USB_REQ_SET_FEATURE:       //   0x03
            {
                /* host-to-device */
                if(USB_DIR_OUT == (sunxi_udc_source.standard_reg.bmRequestType & USB_REQ_DIRECTION_MASK))
                {
                    ret = sunxi_udev_active->standard_req_op(USB_REQ_SET_FEATURE, &sunxi_udc_source.standard_reg, NULL);
                }

                break;
            }
            case USB_REQ_SET_ADDRESS:       //   0x05
            {
                /* host-to-device */
                if(USB_DIR_OUT == (sunxi_udc_source.standard_reg.bmRequestType & USB_REQ_DIRECTION_MASK))
                {
                    if(USB_RECIP_DEVICE == (sunxi_udc_source.standard_reg.bmRequestType & USB_REQ_RECIPIENT_MASK))
                    {
                        /* receiver is device */
                        ret = sunxi_udev_active->standard_req_op(USB_REQ_SET_ADDRESS, &sunxi_udc_source.standard_reg, NULL);
                    }
                }

                break;
            }
            case USB_REQ_GET_DESCRIPTOR:    //   0x06
            {

                /* device-to-host */
                if(USB_DIR_IN == (sunxi_udc_source.standard_reg.bmRequestType & USB_REQ_DIRECTION_MASK))
                {
                    if(USB_RECIP_DEVICE == (sunxi_udc_source.standard_reg.bmRequestType & USB_REQ_RECIPIENT_MASK))
                    {
                        ret = sunxi_udev_active->standard_req_op(USB_REQ_GET_DESCRIPTOR, &sunxi_udc_source.standard_reg, sunxi_usb_ep0_buffer);
                    }
                }

                break;
            }
            case USB_REQ_SET_DESCRIPTOR:        //   0x07
            {
                /* host-to-device */
                if(USB_DIR_OUT == (sunxi_udc_source.standard_reg.bmRequestType & USB_REQ_DIRECTION_MASK))
                {
                    if(USB_RECIP_DEVICE == (sunxi_udc_source.standard_reg.bmRequestType & USB_REQ_RECIPIENT_MASK))
                    {
                        //there is some problem
                        ret = sunxi_udev_active->standard_req_op(USB_REQ_SET_DESCRIPTOR, &sunxi_udc_source.standard_reg, sunxi_usb_ep0_buffer);
                    }
                }

                break;
            }
            case USB_REQ_GET_CONFIGURATION:     //   0x08
            {
                /* device-to-host */
                if(USB_DIR_IN == (sunxi_udc_source.standard_reg.bmRequestType & USB_REQ_DIRECTION_MASK))
                {
                    if(USB_RECIP_DEVICE == (sunxi_udc_source.standard_reg.bmRequestType & USB_REQ_RECIPIENT_MASK))
                    {
                        ret = sunxi_udev_active->standard_req_op(USB_REQ_GET_CONFIGURATION, &sunxi_udc_source.standard_reg, sunxi_usb_ep0_buffer);
                    }
                }

                break;
            }
            case USB_REQ_SET_CONFIGURATION:     //   0x09
            {
                /* host-to-device */
                if(USB_DIR_OUT == (sunxi_udc_source.standard_reg.bmRequestType & USB_REQ_DIRECTION_MASK))
                {
                    if(USB_RECIP_DEVICE == (sunxi_udc_source.standard_reg.bmRequestType & USB_REQ_RECIPIENT_MASK))
                    {
                        ret = sunxi_udev_active->standard_req_op(USB_REQ_SET_CONFIGURATION, &sunxi_udc_source.standard_reg, NULL);
                    }
                }

                break;
            }
            case USB_REQ_GET_INTERFACE:     //   0x0a
            {
                /* device-to-host */
                if(USB_DIR_IN == (sunxi_udc_source.standard_reg.bmRequestType & USB_REQ_DIRECTION_MASK))
                {
                    if(USB_RECIP_DEVICE == (sunxi_udc_source.standard_reg.bmRequestType & USB_REQ_RECIPIENT_MASK))
                    {
                        ret = sunxi_udev_active->standard_req_op(USB_REQ_GET_INTERFACE, &sunxi_udc_source.standard_reg, sunxi_usb_ep0_buffer);
                    }
                }

                break;
            }
            case USB_REQ_SET_INTERFACE:     //   0x0b
            {
                /* host-to-device */
                if(USB_DIR_OUT == (sunxi_udc_source.standard_reg.bmRequestType & USB_REQ_DIRECTION_MASK))
                {
                    if(USB_RECIP_INTERFACE == (sunxi_udc_source.standard_reg.bmRequestType & USB_REQ_RECIPIENT_MASK))
                    {
                        ret = sunxi_udev_active->standard_req_op(USB_REQ_SET_INTERFACE, &sunxi_udc_source.standard_reg, NULL);
                    }
                }

                break;
            }
            case USB_REQ_SYNCH_FRAME:       //   0x0c
            {
                /* device-to-host */
                if(USB_DIR_IN == (sunxi_udc_source.standard_reg.bmRequestType & USB_REQ_DIRECTION_MASK))
                {
                    if(USB_RECIP_INTERFACE == (sunxi_udc_source.standard_reg.bmRequestType & USB_REQ_RECIPIENT_MASK))
                    {
                        //there is some problem
                        if(!ep0_stage)
                        {
                            ep0_stage = 1;
                        }
                        else
                        {
                            ret = sunxi_udev_active->standard_req_op(USB_REQ_SYNCH_FRAME, &sunxi_udc_source.standard_reg, NULL);
                            ep0_stage = 0;
                        }
                    }
                }

                break;
            }
            default:
            {
                printf("sunxi usb err: unknown usb out request to device\n");

                USBC_Dev_EpSendStall(sunxi_udc_source.usbc_hd, USBC_EP_TYPE_EP0);
                ret = SUNXI_USB_REQ_DEVICE_NOT_SUPPORTED;
                ep0_stage = 0;

                break;
            }
        }
    }
    else
    {
        /* Non-Standard Req */
        printf("non standard req\n");
        ret = sunxi_udev_active->nonstandard_req_op(USB_REQ_GET_STATUS, &sunxi_udc_source.standard_reg, sunxi_usb_ep0_buffer, ep0_stage);
        if(ret == SUNXI_USB_REQ_DATA_HUNGRY)
        {
            ep0_stage = 1;
        }
        else if(ret == SUNXI_USB_REQ_SUCCESSED)
        {
            ep0_stage = 0;
        }
        else if(ret < 0)
        {
            ep0_stage = 0;
            printf("err: unkown bmRequestType(%d)\n", sunxi_udc_source.standard_reg.bmRequestType);
            USBC_Dev_EpSendStall(sunxi_udc_source.usbc_hd, USBC_EP_TYPE_EP0);
        }
    }

__ep0_recv_op_err:
    USBC_SelectActiveEp(sunxi_udc_source.usbc_hd, old_ep_index);

    return ret;
}


static int eprx_recv_op(void)
{
    uint old_ep_index;
    uint this_len;
    uint fifo;

    old_ep_index = USBC_GetActiveEp(sunxi_udc_source.usbc_hd);
    USBC_SelectActiveEp(sunxi_udc_source.usbc_hd, SUNXI_USB_BULK_OUT_EP_INDEX);

    if (USBC_Dev_IsEpStall(sunxi_udc_source.usbc_hd, USBC_EP_TYPE_RX))
    {
        USBC_Dev_EpClearStall(sunxi_udc_source.usbc_hd, USBC_EP_TYPE_RX);

        printf("sunxi ubs read error: usb rx ep is busy already\n");
    }
    else
    {
        if(USBC_Dev_IsReadDataReady(sunxi_udc_source.usbc_hd, USBC_EP_TYPE_RX))
        {
            this_len = USBC_ReadLenFromFifo(sunxi_udc_source.usbc_hd, USBC_EP_TYPE_RX);

		sunxi_usb_dbg("irq:eprx_recv_op rx_ready_for_data = %d, this_len = %d\n",
			    sunxi_ubuf.rx_ready_for_data, this_len);
#ifdef CONFIG_FASTBOOT
		sunxi_usb_dbg("fastboot_data_flag=%d\n", fastboot_data_flag);
            if(fastboot_data_flag == 1)
            {
                fifo = USBC_SelectFIFO(sunxi_udc_source.usbc_hd, SUNXI_USB_BULK_OUT_EP_INDEX);

                sunxi_ubuf.rx_req_length = USBC_ReadPacket(sunxi_udc_source.usbc_hd, fifo, this_len, sunxi_ubuf.rx_req_buffer);
                sunxi_ubuf.rx_req_buffer += this_len;

                //sunxi_usb_dbg("special read ep bytes 0x%x\n", sunxi_ubuf.rx_req_length);
                __usb_readcomplete(sunxi_udc_source.usbc_hd, USBC_EP_TYPE_RX, 1);
            }
            else
#endif
            if(!sunxi_ubuf.rx_ready_for_data)
            {
                fifo = USBC_SelectFIFO(sunxi_udc_source.usbc_hd, SUNXI_USB_BULK_OUT_EP_INDEX);

                memset(sunxi_ubuf.rx_req_buffer, 0, 64);
                sunxi_ubuf.rx_req_length = USBC_ReadPacket(sunxi_udc_source.usbc_hd, fifo, this_len, sunxi_ubuf.rx_req_buffer);
                sunxi_ubuf.rx_ready_for_data = 1;

                sunxi_usb_dbg("irq:eprx_recv_op read ep bytes 0x%x\n", sunxi_ubuf.rx_req_length);
                __usb_readcomplete(sunxi_udc_source.usbc_hd, USBC_EP_TYPE_RX, 1);
            }
            else
            {
                sunxi_usb_dbg("eprx do nothing and left it to dma\n");
            }
        }
        else
        {
            sunxi_usb_dbg("sunxi usb rxdata not ready\n");
        }
    }

    USBC_SelectActiveEp(sunxi_udc_source.usbc_hd, old_ep_index);

    return 0;
}


static int eptx_send_op(void)
{
    return 0;
}


void sunxi_usb_main_loop(int delaytime)
{
    int ret;


    if(sunxi_usb_init(delaytime))
    {
        printf("usb init fail\n");

        sunxi_usb_exit();

        return ;
    }
    printf("usb init ok\n");

    while(1)
    {
	__msdelay(5);
        ret = sunxi_udev_active->state_loop(&sunxi_ubuf);
        if(ret)
        {
            break;
        }

        if(ctrlc())
        {
            break;
        }
    }

    printf("exit usb\n");
    sunxi_usb_exit();

    sunxi_update_subsequent_processing(ret);

    return ;
}


int sunxi_usb_extern_loop(void)
{
    return sunxi_udev_active->state_loop(&sunxi_ubuf);
}
