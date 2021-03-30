/**
 * spi_hal.h
 * date:    2015/10/13
 * author:  wangwei<wangwei@allwinnertech.com>
 * history: V0.1
 */
 
#ifndef __SPI_HAL_H
#define __SPI_HAL_H

extern int   spic_init(unsigned int spi_no);
extern int   spic_exit(unsigned int spi_no);
extern int   spic_rw( u32 txlen, void* txbuff, u32 rxlen, void* rxbuff);
extern void  spic_config_dual_mode(u32 spi_no, u32 rxdual, u32 dbc, u32 stc);

#endif
