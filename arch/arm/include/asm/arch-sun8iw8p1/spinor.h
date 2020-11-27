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
#ifndef   __SPI_NOR_H__
#define   __SPI_NOR_H__

/* instruction definition */
#define SPINOR_READ        0x03
#define SPINOR_FREAD       0x0b
#define SPINOR_WREN        0x06
#define SPINOR_WRDI        0x04
#define SPINOR_RDSR        0x05
#define SPINOR_WRSR        0x01
#define SPINOR_PP          0x02
#define SPINOR_SE          0xd8
#define SPINOR_BE          0x60
#define SPINOR_RDID        0x9f
extern int spinor_init(int stage);
extern int spinor_exit(int force);
extern int spinor_read(uint start, uint nblock, void *buffer);
extern int spinor_write(uint start, uint nblock, void *buffer);
extern int spinor_flush_cache(void);
extern int spinor_erase_all_blocks(int erase);
extern int spinor_size(void);
extern int spinor_sprite_write(uint start, uint nblock, void *buffer);
extern int spinor_datafinish(void);
extern s32 spi_nor_rw_test(u32 spi_no);
extern u32 try_spi_nor(u32 spino);

#endif

