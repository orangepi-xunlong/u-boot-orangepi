#ifndef __CV_UART_H__
#define __CV_UART_H__

#include <mmio.h>

#define RBR    0x00
#define THR    0x00
#define DLL    0x00
#define DLH    0x04
#define IER    0x04
#define IIR    0x08
#define FCR    0x08
#define LCR    0x0C
#define MCR    0x10
#define LSR    0x14
#define MSR    0x18
#define SCR    0x1C
#define LPDLL  0x20
#define LPDLH  0x24
#define FAR    0x70
#define TFR    0x74
#define RFW    0x78
#define USR    0x7C
#define TFL    0x80
#define RFL    0x84
#define SRR    0x88

#define DMAC_CFGREG     0x10
#define DMAC_CHENREG    0x18
#define DMAC_RESETREG   0x58
#define CH_CFG          0x120
#define CH_LLP          0x128
#define CH_INTSTATUS    0x188
#define CH_INTCLRSTATUS 0x198

#define CH_RX_UP          1   /* UART4_RX(uart3 IP) */
#define CH_TX_UP          2   /* UART0_TX(uart0 IP) */
#define CH_RX_DOWN        3   /* UART2_RX(uart1 IP) */
#define CH_TX_DOWN        4   /* UART4_TX(uart3 IP) */

#define UART0_TX    0
#define UART0_RX    1
#define UART1_TX    2
#define UART1_RX    3
#define UART2_TX    4
#define UART2_RX    5
#define UART3_TX    6
#define UART3_RX    7

enum packet_type {
    pkt_conf = 0x5A,
    pkt_data = 0xA5,
    pkt_conf_rsp = 0x62,
    pkt_rsp = 0x63
};

enum response_type {
    rsp_conf_ack = 0,
    rsp_ack = 1,
    rsp_nack = 2
};

enum dmac_channel_number {
    Dmac_no_ch   = 0x0000,
    Dmac_ch1     = 0x0101,
    Dmac_ch2     = 0x0202,
    Dmac_ch3     = 0x0404,
    Dmac_ch4     = 0x0808,
    Dmac_ch5     = 0x1010,
    Dmac_ch6     = 0x2020,
    Dmac_ch7     = 0x4040,
    Dmac_ch8     = 0x8080,
    Dmac_all_ch  = 0xffff
};

enum dma_req_number {
    req_uart3_rx = 4,
    req_uart3_tx = 5,
    req_uart2_rx = 6,
    req_uart2_tx = 7,
    req_uart1_rx = 8,
    req_uart1_tx = 9,
    req_uart0_rx = 10,
    req_uart0_tx = 11
};

typedef struct {
    uint64_t addr;
    uint32_t num_of_packets;
    uint32_t baud;
    uint32_t rx_cnt;
    uint32_t tx_cnt;
    uint32_t rx_idx;
    uint32_t tx_idx;
    uint16_t down_trigger;
    uint16_t up_trigger;
    uint16_t conf_done;
    uint16_t run_bin_done;
} packet_conf_t;

#define SYSDMA_W64(offset, value) \
	mmio_write_64(SYSDMA_BASE + offset, value)

#define SYSDMA_R64(offset) \
	mmio_read_64(SYSDMA_BASE + offset)

uint8_t check_crc(uint32_t *buf, uint32_t size);
void bm_init_uart_pinmux();
void bm_init_general_uart_mode(uint32_t base, uint32_t baud, uint32_t dma_en, uint32_t rx_fifo_lv);
void bm_reinit_stage(uint32_t st);
void bm_uart_fw_dl_main();
void bm_send_tx_up(uint8_t type);
void bm_send_tx_down();
void bm_read_rx_up();
void bm_read_rx_down();
void init_uart_tx_dma(uint32_t uart_base, uint16_t num_of_blk, uint16_t req, uint32_t ch, uint32_t* llp, uint32_t* tx_buf);
void init_uart_rx_dma(uint32_t uart_base, uint16_t num_of_blk, uint16_t req, uint32_t ch, uint32_t* llp, uint32_t* rx_buf);
void start_dma(uint32_t ch);
void abort_dma(uint32_t ch);
void enable_dma();
void disable_dma();
void start_dma_ch(uint64_t ch);
void stop_dma_ch(uint64_t ch);
void construct_bin(uint32_t *dest, uint32_t *src, uint32_t num);
void cleanup_uart_sysdma();
void cpu_run(uint64_t addr);

#endif	/* __CV_UART_H__ */
