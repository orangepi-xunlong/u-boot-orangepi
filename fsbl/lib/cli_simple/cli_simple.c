/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * Add to readline cmdline-editing by
 * (C) Copyright 2005
 * JinHua Luo, GuangDong Linux Center, <luo.jinhua@gd-linux.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

//#include <arch_helpers.h>
#include <cli.h>
#include <console.h>
#include <debug.h>
//#include <emmc.h>
//#include <sd.h>
#include <string.h>
#include <stdlib.h>
//#include <uart_loader.h>
#include <platform_def.h>
#include <mmio.h>
//#include <generic_delay_timer.h>
#include <delay_timer.h>
//#include <bm_emmc.h>
//#include <bm_sd.h>
//#include <bm_i2c.h>
//#include <bm_spi.h>
#include <bm_ddr.h>
#include "tempsen.h"
#include "a2_ddr_test.h"


/******** environment workaround ********/

#if DEBUG_PARSER
#define debug_parser tf_printf
#else
#define debug_parser(fmt, args...)
#endif
#define puts tf_printf
#define getc console_getc
#define putc console_putc
#define tstc console_tstc

/*
 * Rather than doubling the size of the _ctype lookup table to hold a 'blank'
 * flag, just check for space or tab.
 */
#define isblank(c)	(c == ' ' || c == '\t')

/**
 * strcpy - Copy a %NUL terminated string
 * @dest: Where to copy the string to
 * @src: Where to copy the string from
 */
char * strcpy(char * dest,const char *src)
{
	char *tmp = dest;

	while ((*dest++ = *src++) != '\0')
		/* nothing */;
	return tmp;
}

/**
 * strlcpy - Copy a C-string into a sized buffer
 * @dest: Where to copy the string to
 * @src: Where to copy the string from
 * @size: size of destination buffer
 *
 * Compatible with *BSD: the result is always a valid
 * NUL-terminated string that fits in the buffer (unless,
 * of course, the buffer size is zero). It does not pad
 * out the result like strncpy() does.
 */
size_t strlcpy(char *dest, const char *src, size_t size)
{
	size_t ret = strlen(src);

	if (size) {
		size_t len = (ret >= size) ? size - 1 : ret;
		memcpy(dest, src, len);
		dest[len] = '\0';
	}
	return ret;
}

extern char console_buffer[];

/**** workaround end ****/

#define TEST_CMD(a) strcmp(argv[0], a) == 0

int skip_loading;
uint32_t gpio_value_override;

void list_cmd(void)
{
	NOTICE("VERSION: %s\n", __DATE__" "__TIME__);
	NOTICE("VERSION detail: base\n");
	NOTICE("support cmd:\n");
	NOTICE("*****************************************************\n");
	NOTICE(">read_temp:  read_temp  get tempture from sensor\n");
	NOTICE(">mw:         mw addr val\n");
	NOTICE(">mr:         mr addr\n");
	NOTICE(">cpu_fill:   cpu_fill src_addr len patten_value\n");
	NOTICE(">cpu_cp:     cpu_cp dst_addr src_addr len\n");
	NOTICE(">cdma_cp:    cdma_cp dst_addr src_addr len\n");
	NOTICE(">gdma_cp:    gdma_cp dst_addr src_addr len\n");
	NOTICE(">cpu_rd:     cpu_rd src_addr len patten_value loop_number\n");
	NOTICE(">cpu_cmp:    cpu_cmp src_addr dst_addr len\n");
	NOTICE(">cpu_poll:   cpu_poll src_addr len loop_number flag\n");
	NOTICE(">cpu_byte3:  cpu_byte3 src_addr len loop_number flag\n");
	NOTICE(">cpu_poll_z: cpu_poll_z src_addr len loop_number flag\n");
	NOTICE(">cpu_2bit_w: cpu_2bit_w src_addr len flag\n");
	NOTICE(">cpu_2bit_r: cpu_2bit_r src_addr len loop_number flag\n");
	NOTICE(">cpu_rd_gpio:     cpu_rd_gpio src_addr len patten_value loop_number\n");
	NOTICE(">cpu_poll_gpio:   cpu_poll_gpio src_addr len loop_number flag\n");
	NOTICE("list end\n");
	NOTICE("*****************************************************\n");
}

int cmd_process(int flag, int argc, char * const argv[])
{
	if (argc < 1)
		return -1;

	if (TEST_CMD("exit")) {
		return 0xE0F;
	} else if (TEST_CMD("list_cmd")) {

        if (argc != 1) {
			ERROR("argcs error!\n");
			return -1;
		}
        list_cmd();
	} else if (TEST_CMD("mw")) {
        unsigned long addr;
        uint32_t value;

        if (argc < 3) {
			ERROR("argcs error!\n");
			return -1;
		}
        addr = simple_strtoul(argv[1], NULL, 16);
        value = simple_strtoul(argv[2], NULL, 16);
        mmio_write_32(addr, value);
        flush_dcache_range(addr, 4);
        NOTICE("0x%lx: 0x%x\n", addr, value);
    } else if (TEST_CMD("mr")) {
        unsigned long addr;

        if (argc < 2) {
			ERROR("argcs error!\n");
			return -1;
		}
        addr = simple_strtoul(argv[1], NULL, 16);
        NOTICE("0x%lx: 0x%08x\n", addr, mmio_read_32(addr));
	} else if (TEST_CMD("mtest")) {
		unsigned long addr;
		unsigned long size = 0x16;
		unsigned long iter = 1;
		NOTICE("mtest\n");
		if (argc < 2) {
			ERROR("argcs error!\n");
			return -1;
		}
		addr = simple_strtoul(argv[1], NULL, 16);
		size = simple_strtoul(argv[2], NULL, 16);
		iter = simple_strtoul(argv[3], NULL, 16);

		//plat_bm_ddr_init();
		mem_test(addr, size, iter);
	} else if (TEST_CMD("ddrint")) { 
		unsigned long src;
		unsigned long size = 0x16;
		unsigned long dst;
		NOTICE("ddr init start...\n");
		if (argc < 3) {
			ERROR("argcs error!\n");
			return -1;
		}
		src = simple_strtoul(argv[1], NULL, 16);
		dst = simple_strtoul(argv[2], NULL, 16);
		size = simple_strtoul(argv[3], NULL, 16);
		ddr_init_pattern(src, dst, size);
	} else if (TEST_CMD("gdma")) {
		unsigned long src;
		unsigned long size = 0x16;
		unsigned long dst;
		unsigned long err_cnt = 0;

		NOTICE("gdma start\n");
		if (argc < 3) {
			ERROR("argcs error!\n");
			return -1;
		}
		src = simple_strtoul(argv[1], NULL, 16);
		dst = simple_strtoul(argv[2], NULL, 16);
		size = simple_strtoul(argv[3], NULL, 16);
		for(int i = 0; i < 10; i++){
			NOTICE("Timer:%d\n", i);
			NOTICE("temp:%d\n", read_temp());
			err_cnt += ddr_gdma_copy(src, dst, size);
			NOTICE("\nAll 1BitErr=%ld\n\n", err_cnt);
		}
	} else if (TEST_CMD("memscan")) {
		unsigned int loop;
		int i=0;
		if (argc < 2) {
			ERROR("argcs error!\n");
			return -1;
		}
		loop = simple_strtoul(argv[1], NULL, 10);
		NOTICE("loop=%d\n", loop);
		NOTICE("memscan\n");
		//plat_bm_ddr_init();
	    for(i=0; i<loop; i++)
		{
			ddr_memtest_test();
		}
    } else if (TEST_CMD("read_temp")) {
        /* NOTICE("read_temp\n"); */
        NOTICE("Current temp: %d m℃\n", read_temp());
    } else if (TEST_CMD("mtestwp")) {
		unsigned long addr;
		unsigned long size = 0x16;
		unsigned long patt = 1;
		unsigned long iter = 1;

		NOTICE("mtestwp\n");
		if (argc < 2) {
			ERROR("argcs error!\n");
			return -1;
		}
		addr = simple_strtoul(argv[1], NULL, 16);
		size = simple_strtoul(argv[2], NULL, 16);
		patt = simple_strtoul(argv[3], NULL, 16);
		iter = simple_strtoul(argv[3], NULL, 16);
		//plat_bm_ddr_init();
		mem_mw3(addr, size, patt, iter);
	} else if (TEST_CMD("wp64")) {
		unsigned long addr;
		unsigned long size = 0x16;
		unsigned long patt = 1;
		unsigned long iter = 1;

		NOTICE("wp64\n");
		if (argc < 2) {
			ERROR("argcs error!\n");
			return 0;
		}
		addr = simple_strtoul(argv[1], NULL, 16);
		size = simple_strtoul(argv[2], NULL, 16);
		patt = simple_strtoul(argv[3], NULL, 16);
		iter = simple_strtoul(argv[4], NULL, 16);
		//plat_bm_ddr_init();
		mem_mw_64(addr, size, patt, iter);

	} else if (TEST_CMD("rp64")) {
		unsigned long addr;
		unsigned long size = 0x16;
		unsigned long patt = 1;
		unsigned long iter = 1;

		NOTICE("rp64\n");
		if (argc < 2) {
			ERROR("argcs error!\n");
			return 0;
		}
		addr = simple_strtoul(argv[1], NULL, 16);
		size = simple_strtoul(argv[2], NULL, 16);
		patt = simple_strtoul(argv[3], NULL, 16);
		iter = simple_strtoul(argv[4], NULL, 16);

		//plat_bm_ddr_init();
		mem_rd_64(addr, size, patt, iter);

	} else if (TEST_CMD("w32")) {
		unsigned long addr;
		unsigned long size = 0x16;
		NOTICE("w32\n");
		if (argc < 2) {
			ERROR("argcs error!\n");
			return 0;
		}
		addr = simple_strtoul(argv[1], NULL, 16);
		size = simple_strtoul(argv[2], NULL, 16);
		mem_write_fix(addr, size);

	} else if (TEST_CMD("fixw")) {
		NOTICE("fixw\n");
		mem_wr_fix();
	} else if (TEST_CMD("r32")) {
		unsigned long addr;
		unsigned long size = 0x16;
		NOTICE("r32\n");
		if (argc < 2) {
			ERROR("argcs error!\n");
			return 0;
		}
		addr = simple_strtoul(argv[1], NULL, 16);
		size = simple_strtoul(argv[2], NULL, 16);
		mem_read_fix(addr, size);
	} else if (TEST_CMD("cpu_fill")) {
		unsigned long long src;
		unsigned int len;
		unsigned int val;
		if (argc != 4) {
			ERROR("argcs error!input as: cpu_fill 0x100000000 0x800000 0xa5a5a5a5\n");
			return 0;
		}
		src = simple_strtoul(argv[1], NULL, 16);
		len = simple_strtoul(argv[2], NULL, 16);
		val = simple_strtoul(argv[3], NULL, 16);
		NOTICE("cpu_fill: src address: 0x%llx, len 0x%x\n", src, len);
		NOTICE("fill value: 0x%x\n", val);
		NOTICE("tempture: %d\n", read_temp());
		cpu_fill(src, len, val);
	} else if (TEST_CMD("cpu_cp")) {
		unsigned long long dst;
		unsigned long long src;
		unsigned int len;
		if (argc != 4) {
			ERROR("argcs error!input as: cpu_cp 0x180000000 0x100000000 0x40000\n");
			return 0;
		}
	
		dst = simple_strtoul(argv[1], NULL, 16);
		src = simple_strtoul(argv[2], NULL, 16);
		len = simple_strtoul(argv[3], NULL, 16);
		NOTICE("cpu copy data from 0x%llx to 0x%llx, len is 0x%x\n", src, dst, len);
		cpu_cp(dst, src, len);
	} else if (TEST_CMD("cdma_cp")) {
		unsigned long long dst;
		unsigned long long src;
		unsigned int len;
		if (argc != 4) {
			ERROR("argcs error!input as: cdma_cp 0x180000000 0x100000000 0x40000\n");
			return 0;
		}
	
		dst = simple_strtoul(argv[1], NULL, 16);
		src = simple_strtoul(argv[2], NULL, 16);
		len = simple_strtoul(argv[3], NULL, 16);
		NOTICE("cdma copy data from 0x%llx to 0x%llx, len is 0x%x\n", src, dst, len);
		cdma_cp(dst, src, len);
	} else if (TEST_CMD("gdma_cp")) {
		unsigned long long dst;
		unsigned long long src;
		unsigned int len;
		if (argc != 4) {
			ERROR("argcs error!input as: gdma_cp 0x180000000 0x100000000 0x40000\n");
			return 0;
		}
		dst = simple_strtoul(argv[1], NULL, 16);
		src = simple_strtoul(argv[2], NULL, 16);
		len = simple_strtoul(argv[3], NULL, 16);
		NOTICE("gdma copy data from 0x%llx to 0x%llx, len is 0x%x\n", src, dst, len);
		gdma_cp(dst, src, len);
	} else if (TEST_CMD("cpu_rd")) {
		unsigned long long src;
		unsigned int len;
		unsigned int val;
		unsigned int cnt;
		if (argc != 5) {
			ERROR("argcs error!input as: cpu_rd 0x100000000 0x800000 0xa5a5a5a5 1\n");
			return 0;
		}
		src = simple_strtoul(argv[1], NULL, 16);
		len = simple_strtoul(argv[2], NULL, 16);
		val = simple_strtoul(argv[3], NULL, 16);
		cnt = simple_strtoul(argv[4], NULL, 16);
		NOTICE("cpu_rd: src address: 0x%llx, len 0x%x, expect value: 0x%x, loop num: %d\n",
				src, len, val, cnt);
		NOTICE("tempture: %d\n", read_temp());
		cpu_rd(src, len, val, cnt);
	} else if (TEST_CMD("cpu_rd_gpio")) {
		unsigned long long src;
		unsigned int len;
		unsigned int val;
		unsigned int cnt;
		if (argc != 5) {
			ERROR("argcs error!input as: cpu_rd_gpio 0x100000000 0x800000 0xa5a5a5a5 1\n");
			return 0;
		}
		src = simple_strtoul(argv[1], NULL, 16);
		len = simple_strtoul(argv[2], NULL, 16);
		val = simple_strtoul(argv[3], NULL, 16);
		cnt = simple_strtoul(argv[4], NULL, 16);
		NOTICE("cpu_rd_gpio: src address: 0x%llx, len 0x%x, expect value: 0x%x, loop num: %d\n",
				src, len, val, cnt);
		NOTICE("tempture: %d\n", read_temp());
		cpu_rd_gpio(src, len, val, cnt);
	} else if (TEST_CMD("cpu_cmp")) {
		unsigned long long src;
		unsigned long long dst;
		unsigned int len;
		if (argc != 4) {
			ERROR("argcs error!input as: cpu_cmp 0x100000000 0x100000000 0x40000\n");
			return 0;
		}
		src = simple_strtoul(argv[1], NULL, 16);
		dst = simple_strtoul(argv[2], NULL, 16);
		len = simple_strtoul(argv[3], NULL, 16);
		NOTICE("cpu_cmp: src address: 0x%llx, dst address: 0x%llx, len 0x%x\n",
				src, dst, len);
		NOTICE("tempture: %d\n", read_temp());
		cpu_cmp(src, dst, len);
	} else if (TEST_CMD("cpu_poll")) {
		unsigned long long src;
		unsigned int len;
		unsigned int poll_num;
		unsigned int flag;
		if (argc != 5) {
			ERROR("argcs error!input as: cpu_poll 0x100000000 0x800000 a 0\n");
			return 0;
		}
		src = simple_strtoul(argv[1], NULL, 16);
		len = simple_strtoul(argv[2], NULL, 16);
		poll_num = simple_strtoul(argv[3], NULL, 16);
		flag = simple_strtoul(argv[4], NULL, 16);
		NOTICE("cpu_poll: src address: 0x%llx, len: 0x%x, poll_num: %d, flag: %d\n",
				src, len, poll_num, flag);
		NOTICE("tempture: %d\n", read_temp());
		cpu_poll(src, len, poll_num, flag);
	} else if (TEST_CMD("cpu_poll_gpio")) {
		unsigned long long src;
		unsigned int len;
		unsigned int poll_num;
		unsigned int flag;
		if (argc != 5) {
			ERROR("argcs error!input as: cpu_poll_gpio 0x100000000 0x800000 a 0\n");
			return 0;
		}
		src = simple_strtoul(argv[1], NULL, 16);
		len = simple_strtoul(argv[2], NULL, 16);
		poll_num = simple_strtoul(argv[3], NULL, 16);
		flag = simple_strtoul(argv[4], NULL, 16);
		NOTICE("cpu_poll_gpio: src address: 0x%llx, len: 0x%x, poll_num: %d, flag: %d\n",
				src, len, poll_num, flag);
		NOTICE("tempture: %d\n", read_temp());
		cpu_poll_gpio(src, len, poll_num, flag);
	} else if (TEST_CMD("cpu_byte3")) {
		unsigned long long src;
		unsigned int len;
		unsigned int poll_num;
		unsigned int flag;
		if (argc != 5) {
			ERROR("argcs error!input as: cpu_byte3 0x100000000 0x800000 a 0\n");
			return 0;
		}
		src = simple_strtoul(argv[1], NULL, 16);
		len = simple_strtoul(argv[2], NULL, 16);
		poll_num = simple_strtoul(argv[3], NULL, 16);
		flag = simple_strtoul(argv[4], NULL, 16);
		NOTICE("cpu_byte3: src address: 0x%llx, len: 0x%x, poll_num: %d, flag: %d\n",
				src, len, poll_num, flag);
		NOTICE("tempture: %d\n", read_temp());
		cpu_byte3(src, len, poll_num, flag);
	} else if (TEST_CMD("cpu_poll_z")) {
		unsigned long long src;
		unsigned int len;
		unsigned int poll_num;
		unsigned int flag;
		if (argc != 5) {
			ERROR("argcs error!input as: cpu_byte3 0x100000000 0x800000 a 0\n");
			return 0;
		}
		src = simple_strtoul(argv[1], NULL, 16);
		len = simple_strtoul(argv[2], NULL, 16);
		poll_num = simple_strtoul(argv[3], NULL, 16);
		flag = simple_strtoul(argv[4], NULL, 16);
		NOTICE("cpu_byte3: src address: 0x%llx, len: 0x%x, poll_num: %d, flag: %d\n",
				src, len, poll_num, flag);
		NOTICE("tempture: %d\n", read_temp());
		cpu_poll_z(src, len, poll_num, flag);
	} else if (TEST_CMD("cpu_2bit_w")) {
		unsigned long long src;
		unsigned int len;
		unsigned int flag;
		if (argc != 4) {
			ERROR("argcs error!input as: cpu_2bit_w 0x100000000 0x800000 0\n");
			return 0;
		}
		src = simple_strtoul(argv[1], NULL, 16);
		len = simple_strtoul(argv[2], NULL, 16);
		flag = simple_strtoul(argv[3], NULL, 16);
		NOTICE("cpu_2bit_w: src address: 0x%llx, len: 0x%x, flag: %d\n",
				src, len, flag);
		NOTICE("tempture: %d\n", read_temp());
		cpu_2bit_w(src, len, flag);
	} else if (TEST_CMD("cpu_2bit_r")) {
		unsigned long long src;
		unsigned int len;
		unsigned int poll_num;
		unsigned int flag;
		if (argc != 5) {
			ERROR("argcs error!input as: cpu_2bit_r 0x100000000 0x800000 a 0\n");
			return 0;
		}
		src = simple_strtoul(argv[1], NULL, 16);
		len = simple_strtoul(argv[2], NULL, 16);
		poll_num = simple_strtoul(argv[3], NULL, 16);
		flag = simple_strtoul(argv[4], NULL, 16);
		NOTICE("cpu_2bit_r: src address: 0x%llx, len: 0x%x, poll_num: %d, flag: %d\n",
				src, len, poll_num, flag);
		NOTICE("tempture: %d\n", read_temp());
		cpu_2bit_r(src, len, poll_num, flag);
	} else {
		ERROR("cmd error!\n");
	}

	return 0;
}

int cli_simple_parse_line(char *line, char *argv[])
{
	int nargs = 0;

	debug_parser("%s: \"%s\"\n", __func__, line);
	while (nargs < CONFIG_SYS_MAXARGS) {
		/* skip any white space */
		while (isblank(*line))
			++line;

		if (*line == '\0') {	/* end of line, no more args	*/
			argv[nargs] = NULL;
			debug_parser("%s: nargs=%d\n", __func__, nargs);
			return nargs;
		}

		argv[nargs++] = line;	/* begin of argument string	*/

		/* find end of string */
		while (*line && !isblank(*line))
			++line;

		if (*line == '\0') {	/* end of line, no more args	*/
			argv[nargs] = NULL;
			debug_parser("parse_line: nargs=%d\n", nargs);
			return nargs;
		}

		*line++ = '\0';		/* terminate current arg	 */
	}

	ERROR("** Too many args (max. %d) **\n", CONFIG_SYS_MAXARGS);

	debug_parser("%s: nargs=%d\n", __func__, nargs);
	return nargs;
}

 /*
 * WARNING:
 *
 * We must create a temporary copy of the command since the command we get
 * may be the result from getenv(), which returns a pointer directly to
 * the environment data, which may change magicly when the command we run
 * creates or modifies environment variables (like "bootp" does).
 */
int cli_simple_run_command(const char *cmd, int flag)
{
	char cmdbuf[CONFIG_SYS_CBSIZE];	/* working copy of cmd		*/
	char *token;			/* start of token in cmdbuf	*/
	char *sep;			/* end of token (separator) in cmdbuf */
	char *str = cmdbuf;
	char *argv[CONFIG_SYS_MAXARGS + 1];	/* NULL terminated	*/
	int argc, inquotes;
	int rc = 0;

	debug_parser("[RUN_COMMAND] cmd[%p]=\"", cmd);
	if (DEBUG_PARSER) {
		/* use puts - string may be loooong */
		puts(cmd ? cmd : "NULL");
		puts("\"\n");
	}

	if (!cmd || !*cmd)
		return -1;	/* empty command */

	if (strlen(cmd) >= CONFIG_SYS_CBSIZE) {
		puts("## Command too long!\n");
		return -1;
	}

	strcpy(cmdbuf, cmd);

	/* Process separators and check for invalid
	 * repeatable commands
	 */

	debug_parser("[PROCESS_SEPARATORS] %s\n", cmd);
	while (*str) {
		/*
		 * Find separator, or string end
		 * Allow simple escape of ';' by writing "\;"
		 */
		for (inquotes = 0, sep = str; *sep; sep++) {
			if ((*sep == '\'') &&
			    (*(sep - 1) != '\\'))
				inquotes = !inquotes;

			if (!inquotes &&
			    (*sep == ';') &&	/* separator		*/
			    (sep != str) &&	/* past string start	*/
			    (*(sep - 1) != '\\'))	/* and NOT escaped */
				break;
		}

		/*
		 * Limit the token to data between separators
		 */
		token = str;
		if (*sep) {
			str = sep + 1;	/* start of command for next pass */
			*sep = '\0';
		} else {
			str = sep;	/* no more commands for next pass */
		}
		debug_parser("token: \"%s\"\n", token);

		/* Extract arguments */
		argc = cli_simple_parse_line(token, argv);
		if (argc == 0) {
			rc = -1;	/* no command at all */
			continue;
		}

		rc = cmd_process(flag, argc, argv);
	}

	return rc;
}

static int __abortboot(int bootdelay)
{
	int abort = 0;
	unsigned long ts;

#ifdef CLI_NO_WAIT
	return 0;
#endif
	puts("Hit any key to stop autoboot: %d", bootdelay);

	/*
	 * Check if key already pressed
	 */
	if (tstc()) {	/* we got a key press	*/
#ifdef CLI_HOT_KEY
		if (getc() == CLI_HOT_KEY) {	/* consume input	*/
			puts("\b0");
			abort = 1;	/* don't auto boot	*/
		}
#else
		(void) getc();  /* consume input	*/
		puts("\b0");
		abort = 1;	/* don't auto boot	*/
#endif
	}

	while ((bootdelay > 0) && (!abort)) {
		--bootdelay;
		/* delay 1000 ms */
		ts = get_timer(0);
		do {
			if (tstc()) {	/* we got a key press	*/
#ifdef CLI_HOT_KEY
				if (getc() == CLI_HOT_KEY) {	/* consume input	*/
					abort  = 1;	/* don't auto boot	*/
					bootdelay = 0;	/* no more delay	*/
					break;
				}
#else
				abort  = 1;	/* don't auto boot	*/
				bootdelay = 0;	/* no more delay	*/
				(void) getc();  /* consume input	*/
				break;
#endif
			}
			udelay(100);
		} while (!abort && get_timer(ts) < 1000);

//		printf("\b%d", bootdelay);
	}

	putc('\n');

	return abort;
}


void cli_simple_loop(int force)
{
	int len;
	int flag;
	int rc = 1;

	if (!force && !__abortboot(1))
		return;

	NOTICE("VERSION: %s\n", __DATE__" "__TIME__);
	for (;;) {
		len = cli_readline(CONFIG_SYS_PROMPT);

		flag = 0;	/* assume no special flags for now */

		if (len == -1)
			puts("<INTERRUPT>\n");
		else
			rc = cli_simple_run_command(console_buffer, flag);

		if (rc == 0xE0F) {
			puts("<CLI EXIT>\n");
			break;
		}
	}
}

