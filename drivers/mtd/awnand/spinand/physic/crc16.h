/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __CRC16_H
#define __CRC16_H

#include <linux/types.h>

extern u16 const crc16_table[256];

extern u16 crc16(u16 crc, const u8 *buffer, size_t len);

static inline u16 crc16_byte(u16 crc, const u8 data)
{
	return (crc >> 8) ^ crc16_table[(crc ^ data) & 0xff];
}

#endif /* __CRC16_H */
