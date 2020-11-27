/*
 * SPDX-License-Identifier: GPL-2.0+
 *
 * Based on bitrev from the Linux kernel, by Akinobu Mita
 */

extern u8 const byte_rev_table[256];

static inline u8 bitrev8(u8 byte)
{
	return byte_rev_table[byte];
}
