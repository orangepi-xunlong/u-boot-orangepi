/*
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */
#include <common.h>

void u8_to_string_hex( u8 input, char * str )
{
	int i;
	static char base[] = "0123456789abcdef";

	for( i = 1; i >= 0; --i )
	{
		str[i] = base[input&0xf];
		input >>= 4;
	}

	str[2] = '\0';

	return;
}

void ndump(u8 *buf, int count)
{
	int i;
	char str[3] = {0};
	for( i=0; i < count; i++ )
	{
		u8_to_string_hex(buf[i], str);
		puts(str);
		puts(" ");
		if((i+1)%16 == 0) puts("\n");
	}
	puts("\n");
}