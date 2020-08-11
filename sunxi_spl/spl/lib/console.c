/*
**********************************************************************************************************************
*
*						           the Embedded Secure Bootloader System
*
*
*						       Copyright(C), 2006-2014, Allwinnertech Co., Ltd.
*                                           All Rights Reserved
*
* File    :
*
* By      :
*
* Version : V2.00
*
* Date	  :
*
* Descript:
**********************************************************************************************************************
*/
#include <stdarg.h>
#include <common.h>
#include <asm/arch/uart.h>

#define  MASK_LOW4      0xf
#define  NEGATIVE       1
#define  POSITIVE       0
#define  HEX_x          'x'
#define  HEX_X          'X'

extern u32 get_sys_ticks(void);
int   debug_enable = LOG_LEVEL_INFO;

/*
******************************************************************************************************************
*
*Function Name : int_to_string_dec
*
*Description : This function is to convert an 'int' data 'input' to a string in decimalism, and the string
*              converted is in 'str'.
*
*Input : int input : 'int' data to be converted.
*        char * str : the start address of the string storing the converted result.
*
*Output : void
*
*call for :
*
*Others : None at present.
*
*******************************************************************************************************************
*/
void int_to_string_dec( int input , char * str)
{
	char stack[12];
	char sign_flag = POSITIVE ;      // 'sign_flag indicates wheater 'input' is positive or negative, default
	int i ;                           // value is 'POSITIVE'.
	int j ;

	if( input == 0 )
	{
		str[0] = '0';
		str[1] = '\0';                   // 'str' must end with '\0'
		return ;
	}

	if( input < 0 )                      // If 'input' is negative, 'input' is assigned to its absolute value.
	{
		sign_flag = NEGATIVE ;
		input = -input ;
	}

	for( i = 0; input > 0; ++i )
	{
		stack[i] = input%10 + '0';      // characters in reverse order are put in 'stack' .
		input /= 10;
	}                                   // at the end of 'for' loop, 'i' is the number of characters.


    j = 0;
	if( sign_flag == NEGATIVE )
		str[j++] = '-';		            // If 'input' is negative, minus sign '-' is placed in the head.
	for( --i  ; i >= 0; --i, ++j )
		str[j] = stack[i];
	str[j] = '\0';				        // 'str' must end with '\0'

	return;
}


void int_to_string_hex( int input, char * str )
{
	int i;
	static char base[] = "0123456789abcdef";

	for( i = 7; i >= 0; --i )
	{
		str[i] = base[input&MASK_LOW4];
		input >>= 4;
	}

	str[8] = '\0';                        // 'str' must end with '\0'

	return;
}

static __u32 mem_puts(const char *str, char *p )
{
    __u32 len = 0;

	while( *str != '\0' )
	{
		if( *str == '\n' )                      // if current character is '\n', insert and output '\r'
		{
		    *p++ = '\r';
		    len ++;
        }
        *p++ = *str++;
        len ++;
	}

	return len;
}

int vsprintf(char *buf, const char *fmt, va_list args)
{
	char string[16];
	char *p, *q = buf;

	while( *fmt )
	{
		if( *fmt == '%' )
		{
			++fmt;
			p = string;
			switch( *fmt )
			{
				case 'd': int_to_string_dec( va_arg( args, int), string );
                          q += mem_puts( p, q );
						  ++fmt;
						  break;
				case 'x':
				case 'X': int_to_string_hex( va_arg( args,  int ), string );
						  q += mem_puts( p, q );
                          ++fmt;
						  break;
				case 'c': *q++ = va_arg( args,  __s32 );
						  ++fmt;
						  break;
				case 's': q += mem_puts( va_arg( args, char * ), q );
						  ++fmt;
						  break;
				default : *q++ = '%';                                    // if current character is not Conversion Specifiers 'dxpXucs',
						  *q++ = *fmt++;                                 // output directly '%' and current character, and then
						                                                 // let 'fmt' point to next character.
			}
		}
		else
		{
			if( *fmt == '\n' )                      // if current character is '\n', insert and output '\r'
				*q++ = '\r';

            *q++ = *fmt++;
		}
	}

    *q = 0;

	return q-buf;
}

void puts(const char *s)
{
	char *src = (char *)s;

	while (*src != '\0')
	{
		sunxi_serial_putc (*src++);
	}
}

int sprintf(char * buf, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i=vsprintf(buf,fmt,args);
	va_end(args);
	return i;
}

int uprintf(int log_level, const char *fmt, ...)
{
	va_list args;
	u32 i,time_msec;
	char printbuffer[384];

	if (log_level > debug_enable) {
		return 0;
	}

	time_msec = get_sys_ticks();

	va_start(args, fmt);

	/* For this to work, printbuffer must be larger than
	 * anything we ever want to print.
	 */
	i = sprintf(printbuffer, "[%d]",time_msec);
	i = vsprintf(printbuffer + i, fmt, args);

	va_end(args);
	/* Print the string */
	puts(printbuffer);

	return i;
}


int printf(const char *fmt, ...)
{
	va_list args;
	u32 i,j,time_msec, count;
	char printbuffer[384];

	if (!debug_enable)
	{
		return 0;
	}
	time_msec = get_sys_ticks();
	va_start(args, fmt);

	/* For this to work, printbuffer must be larger than
	 * anything we ever want to print.
	 */
	i = sprintf(printbuffer, "[%d]",time_msec);
	j = vsprintf(printbuffer + i, fmt, args);

	va_end(args);
	count = i+ j;

	/* Print the string */
	puts(printbuffer);

	return count;
}

char get_uart_input(void)
{
	char c = 0;
	u32 start= 0;

	start = get_sys_ticks();
	while(1)
	{
		if(sunxi_serial_tstc())
		{
			c = sunxi_serial_getc();
			pr_force("key press : %c\n", c);
			break;
		}

		/* test time: 10 ms */
		if(get_sys_ticks()- start > 10)
		{
			break;
		}
		__usdelay(500);
	}
	return c;
}

void printf_all(void)
{

}
