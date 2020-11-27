/*
 * Copyright (c) 2011 The Chromium OS Authors.
 * (C) Copyright 2002-2006
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <linux/compiler.h>
#include <version.h>
#include <environment.h>
#include <fdtdec.h>
#include <fs.h>
#if defined(CONFIG_CMD_IDE)
#include <ide.h>
#endif
#include <i2c.h>
#include <initcall.h>
#include <logbuff.h>

/* TODO: Can we move these into arch/ headers? */
#ifdef CONFIG_8xx
#include <mpc8xx.h>
#endif
#ifdef CONFIG_5xx
#include <mpc5xx.h>
#endif
#ifdef CONFIG_MPC5xxx
#include <mpc5xxx.h>
#endif

#include <os.h>
#include <post.h>
#include <spi.h>
#include <trace.h>
#include <watchdog.h>
#include <asm/errno.h>
#include <asm/io.h>
#ifdef CONFIG_MP
#include <asm/mp.h>
#endif
#include <asm/sections.h>
#ifdef CONFIG_X86
#include <asm/init_helpers.h>
#include <asm/relocate.h>
#endif
#ifdef CONFIG_SANDBOX
#include <asm/state.h>
#endif
#include <linux/compiler.h>
#include <private_uboot.h>
#include <fdt_support.h>
#include <private_toc.h>
#include <sys_config_old.h>
#include <malloc.h>
/*
 * Pointer to initial global data area
 *
 * Here we initialize it if needed.
 */
#ifdef XTRN_DECLARE_GLOBAL_DATA_PTR
#undef	XTRN_DECLARE_GLOBAL_DATA_PTR
#define XTRN_DECLARE_GLOBAL_DATA_PTR	/* empty = allocate here */
DECLARE_GLOBAL_DATA_PTR = (gd_t *) (CONFIG_SYS_INIT_GD_ADDR);
#else
DECLARE_GLOBAL_DATA_PTR;
#endif

extern uint usb_dma_used[8];
/*
 * sjg: IMO this code should be
 * refactored to a single function, something like:
 *
 * void led_set_state(enum led_colour_t colour, int on);
 */
/************************************************************************
 * Coloured LED functionality
 ************************************************************************
 * May be supplied by boards if desired
 */
inline void __coloured_LED_init(void) {}
void coloured_LED_init(void)
	__attribute__((weak, alias("__coloured_LED_init")));
inline void __red_led_on(void) {}
void red_led_on(void) __attribute__((weak, alias("__red_led_on")));
inline void __red_led_off(void) {}
void red_led_off(void) __attribute__((weak, alias("__red_led_off")));
inline void __green_led_on(void) {}
void green_led_on(void) __attribute__((weak, alias("__green_led_on")));
inline void __green_led_off(void) {}
void green_led_off(void) __attribute__((weak, alias("__green_led_off")));
inline void __yellow_led_on(void) {}
void yellow_led_on(void) __attribute__((weak, alias("__yellow_led_on")));
inline void __yellow_led_off(void) {}
void yellow_led_off(void) __attribute__((weak, alias("__yellow_led_off")));
inline void __blue_led_on(void) {}
void blue_led_on(void) __attribute__((weak, alias("__blue_led_on")));
inline void __blue_led_off(void) {}
void blue_led_off(void) __attribute__((weak, alias("__blue_led_off")));

/*
 * Why is gd allocated a register? Prior to reloc it might be better to
 * just pass it around to each function in this file?
 *
 * After reloc one could argue that it is hardly used and doesn't need
 * to be in a register. Or if it is it should perhaps hold pointers to all
 * global data for all modules, so that post-reloc we can avoid the massive
 * literal pool we get on ARM. Or perhaps just encourage each module to use
 * a structure...
 */

/*
 * Could the CONFIG_SPL_BUILD infection become a flag in gd?
 */

#if defined(CONFIG_WATCHDOG)
static int init_func_watchdog_init(void)
{
	puts("       Watchdog enabled\n");
	WATCHDOG_RESET();

	return 0;
}

int init_func_watchdog_reset(void)
{
	WATCHDOG_RESET();

	return 0;
}
#endif /* CONFIG_WATCHDOG */

void __board_add_ram_info(int use_default)
{
	/* please define platform specific board_add_ram_info() */
}

void board_add_ram_info(int)
	__attribute__ ((weak, alias("__board_add_ram_info")));

#if 0
static int show_dram_config(void)
{
	unsigned long long size;

#ifdef CONFIG_NR_DRAM_BANKS
	int i;

	debug("\nRAM Configuration:\n");
	for (i = size = 0; i < CONFIG_NR_DRAM_BANKS; i++) {
		size += gd->bd->bi_dram[i].size;
		debug("Bank #%d: %08lx ", i, gd->bd->bi_dram[i].start);
#ifdef DEBUG
		print_size(gd->bd->bi_dram[i].size, "\n");
#endif
	}
	debug("\nDRAM:  ");
#else
	size = gd->ram_size;
#endif

	print_size(size, "");
	board_add_ram_info(0);
	putc('\n');

	return 0;
}
#endif

#if defined(CONFIG_HARD_SPI)
static int init_func_spi(void)
{
	puts("SPI:   ");
	spi_init();
	puts("ready\n");
	return 0;
}
#endif

__maybe_unused
static int zero_global_data(void)
{
	memset((void *)gd, '\0', sizeof(gd_t));

	return 0;
}

__weak int arch_cpu_init(void)
{
	return 0;
}

#ifdef CONFIG_OF_HOSTFILE

static int read_fdt_from_file(void)
{
	struct sandbox_state *state = state_get_current();
	const char *fname = state->fdt_fname;
	void *blob;
	ssize_t size;
	int err;
	int fd;

	blob = map_sysmem(CONFIG_SYS_FDT_LOAD_ADDR, 0);
	if (!state->fdt_fname) {
		err = fdt_create_empty_tree(blob, 256);
		if (!err)
			goto done;
		printf("Unable to create empty FDT: %s\n", fdt_strerror(err));
		return -EINVAL;
	}

	size = os_get_filesize(fname);
	if (size < 0) {
		printf("Failed to file FDT file '%s'\n", fname);
		return -ENOENT;
	}
	fd = os_open(fname, OS_O_RDONLY);
	if (fd < 0) {
		printf("Failed to open FDT file '%s'\n", fname);
		return -EACCES;
	}
	if (os_read(fd, blob, size) != size) {
		os_close(fd);
		return -EIO;
	}
	os_close(fd);

done:
	gd->fdt_blob = blob;

	return 0;
}
#endif

#ifdef CONFIG_SANDBOX
static int setup_ram_buf(void)
{
	struct sandbox_state *state = state_get_current();

	gd->arch.ram_buf = state->ram_buf;
	gd->ram_size = state->ram_size;

	return 0;
}
#endif


/* Round memory pointer down to next 4 kB limit */
static int reserve_round_4k(void)
{
printf("%s %d\n", __FILE__, __LINE__);
	gd->relocaddr = CONFIG_SYS_TEXT_BASE;
	return 0;
}

#if !(defined(CONFIG_SYS_ICACHE_OFF) && defined(CONFIG_SYS_DCACHE_OFF)) && \
		defined(CONFIG_ARM)
static int reserve_mmu(void)
{
	/* reserve TLB table */
	gd->arch.tlb_size = PGTABLE_SIZE;
	gd->relocaddr -= gd->arch.tlb_size;

	/* round down to next 64 kB limit */
	gd->relocaddr &= ~(0x10000 - 1);

	gd->arch.tlb_addr = gd->relocaddr;
	debug("TLB table from %08lx to %08lx\n", gd->arch.tlb_addr,
	      gd->arch.tlb_addr + gd->arch.tlb_size);
	return 0;
}
#endif


#ifndef CONFIG_SPL_BUILD
/* reserve memory for malloc() area */
static int reserve_malloc(void)
{

	gd->start_addr_sp = gd->relocaddr - TOTAL_MALLOC_LEN;
	debug("Reserving %dk for malloc() at: %08lx\n",
			TOTAL_MALLOC_LEN >> 10, gd->start_addr_sp);

#ifdef CONFIG_NONCACHE_MEMORY
	gd->start_addr_sp &= ~(1024 * 1024 - 1);
	gd->start_addr_sp -= CONFIG_NONCACHE_MEMORY_SIZE;
	debug("Reserving %dk for nocache malloc() at: %08lx\n",
			CONFIG_NONCACHE_MEMORY_SIZE >> 10, gd->start_addr_sp);
	gd->malloc_noncache_start = gd->start_addr_sp;
#endif

	printf("%s %d\n", __FILE__, __LINE__);

	return 0;
}

/* (permanently) allocate a Board Info struct */
static int reserve_board(void)
{
	gd->start_addr_sp -= sizeof(bd_t);
	gd->bd = (bd_t *)map_sysmem(gd->start_addr_sp, sizeof(bd_t));
	memset(gd->bd, '\0', sizeof(bd_t));
	debug("Reserving %zu Bytes for Board Info at: %08lx\n",
			sizeof(bd_t), gd->start_addr_sp);
	return 0;
}
#endif

static int reserve_stacks(void)
{
	/* setup stack pointer for exceptions */
	gd->start_addr_sp -= 16;
	gd->start_addr_sp &= ~0xf;
	gd->irq_sp = gd->start_addr_sp;

	/*
	 * Handle architecture-specific things here
	 * TODO(sjg@chromium.org): Perhaps create arch_reserve_stack()
	 * to handle this and put in arch/xxx/lib/stack.c
	 */

	gd->start_addr_sp -= (CONFIG_STACKSIZE_IRQ + CONFIG_STACKSIZE_FIQ);
	debug("Reserving %zu Bytes for IRQ stack at: %08lx\n",
		CONFIG_STACKSIZE_IRQ + CONFIG_STACKSIZE_FIQ, gd->start_addr_sp);

	/* 8-byte alignment for ARM ABI compliance */
	gd->start_addr_sp &= ~0x07;

	gd->start_addr_sp -= 16;

	return 0;
}

#ifdef CONFIG_PPC
static int setup_board_part1(void)
{
	bd_t *bd = gd->bd;

	/*
	 * Save local variables to board info struct
	 */

	bd->bi_memstart = CONFIG_SYS_SDRAM_BASE;	/* start of memory */
	bd->bi_memsize = gd->ram_size;			/* size in bytes */

#ifdef CONFIG_SYS_SRAM_BASE
	bd->bi_sramstart = CONFIG_SYS_SRAM_BASE;	/* start of SRAM */
	bd->bi_sramsize = CONFIG_SYS_SRAM_SIZE;		/* size  of SRAM */
#endif

#if defined(CONFIG_8xx) || defined(CONFIG_MPC8260) || defined(CONFIG_5xx) || \
		defined(CONFIG_E500) || defined(CONFIG_MPC86xx)
	bd->bi_immr_base = CONFIG_SYS_IMMR;	/* base  of IMMR register     */
#endif
#if defined(CONFIG_MPC5xxx)
	bd->bi_mbar_base = CONFIG_SYS_MBAR;	/* base of internal registers */
#endif
#if defined(CONFIG_MPC83xx)
	bd->bi_immrbar = CONFIG_SYS_IMMR;
#endif

	return 0;
}

static int setup_board_part2(void)
{
	bd_t *bd = gd->bd;

	bd->bi_intfreq = gd->cpu_clk;	/* Internal Freq, in Hz */
	bd->bi_busfreq = gd->bus_clk;	/* Bus Freq,      in Hz */
#if defined(CONFIG_CPM2)
	bd->bi_cpmfreq = gd->arch.cpm_clk;
	bd->bi_brgfreq = gd->arch.brg_clk;
	bd->bi_sccfreq = gd->arch.scc_clk;
	bd->bi_vco = gd->arch.vco_out;
#endif /* CONFIG_CPM2 */
#if defined(CONFIG_MPC512X)
	bd->bi_ipsfreq = gd->arch.ips_clk;
#endif /* CONFIG_MPC512X */
#if defined(CONFIG_MPC5xxx)
	bd->bi_ipbfreq = gd->arch.ipb_clk;
	bd->bi_pcifreq = gd->pci_clk;
#endif /* CONFIG_MPC5xxx */

	return 0;
}
#endif

#ifdef CONFIG_SYS_EXTBDINFO
static int setup_board_extra(void)
{
	bd_t *bd = gd->bd;

	strncpy((char *) bd->bi_s_version, "1.2", sizeof(bd->bi_s_version));
	strncpy((char *) bd->bi_r_version, U_BOOT_VERSION,
		sizeof(bd->bi_r_version));

	bd->bi_procfreq = gd->cpu_clk;	/* Processor Speed, In Hz */
	bd->bi_plb_busfreq = gd->bus_clk;
#if defined(CONFIG_405GP) || defined(CONFIG_405EP) || \
		defined(CONFIG_440EP) || defined(CONFIG_440GR) || \
		defined(CONFIG_440EPX) || defined(CONFIG_440GRX)
	bd->bi_pci_busfreq = get_PCI_freq();
	bd->bi_opbfreq = get_OPB_freq();
#elif defined(CONFIG_XILINX_405)
	bd->bi_pci_busfreq = get_PCI_freq();
#endif

	return 0;
}
#endif

/* ARM calls relocate_code from its crt0.S */
#if !defined(CONFIG_ARM) && !defined(CONFIG_SANDBOX)

static int jump_to_copy(void)
{
	/*
	 * x86 is special, but in a nice way. It uses a trampoline which
	 * enables the dcache if possible.
	 *
	 * For now, other archs use relocate_code(), which is implemented
	 * similarly for all archs. When we do generic relocation, hopefully
	 * we can make all archs enable the dcache prior to relocation.
	 */
#ifdef CONFIG_X86
	/*
	 * SDRAM and console are now initialised. The final stack can now
	 * be setup in SDRAM. Code execution will continue in Flash, but
	 * with the stack in SDRAM and Global Data in temporary memory
	 * (CPU cache)
	 */
	board_init_f_r_trampoline(gd->start_addr_sp);
#else
	relocate_code(gd->start_addr_sp, gd->new_gd, gd->relocaddr);
#endif

	return 0;
}
#endif

extern s32 sunxi_rsb_init(u32 slave_id);

static int init_func_pmubus(void)
{
	s32 ret = 0;

#if defined(CONFIG_AXP_USE_RSB)
	//ret = sunxi_rsb_init(0);
#elif defined (CONFIG_AXP_USE_I2C)
	/*i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);*/
#else

#endif
	printf("pmbus:   %s\n", ret? "not ready":"ready");
	return (0);
}

extern void mem_noncache_malloc_init(uint noncache_start, uint noncache_size);

#define  CONFIG_DUMP_MALLOC_LEN   (3 * 1024 * 1024)
#define  CONFIG_NOCACHE_MALLOC_LEN   (2 * 1024 * 1024)

static int initr_malloc(void)
{
	uint32_t malloc_start;
	uint32_t malloc_size;
printf("%s %d\n", __FILE__, __LINE__);
	malloc_start = gd->relocaddr - CONFIG_DUMP_MALLOC_LEN;
	malloc_size  = CONFIG_DUMP_MALLOC_LEN;
	malloc_size += (malloc_start & 0x000fffff);
	malloc_start &= 0xfff00000;
printf("%s %d\n", __FILE__, __LINE__);
	printf("malloc init: start 0x%x, range 0x%x\n", malloc_start, malloc_size);
	mem_malloc_init((ulong)map_sysmem(malloc_start, CONFIG_DUMP_MALLOC_LEN),
			malloc_size);
	debug("malloc start addr is %x, size %dk \n",malloc_start,CONFIG_DUMP_MALLOC_LEN>>10 );

	printf("malloc init ok\n");
#ifdef  CONFIG_NONCACHE_MEMORY
{
	//mem_noncache_malloc_init(gd->malloc_noncache_start, CONFIG_NOCACHE_MALLOC_LEN);
	//debug("no cache malloc start addr is %lx, size %dk \n",gd->malloc_noncache_start,CONFIG_NOCACHE_MALLOC_LEN>>10 );
}
#endif

	return 0;
}

static int cache_enable(void)
{
	icache_enable();
	dcache_enable();

	return 0;
}

static int initr_enable_interrupts(void)
{
	enable_interrupts();
	return 0;
}

extern int sunxi_usb_dev_register(uint dev_name);
extern void sunxi_usb_main_loop(int mode);

static int sunxi_usb_efex_reg(void)
{
printf("%s %d\n", __FILE__, __LINE__);
	sunxi_usb_dev_register(2);
printf("%s %d\n", __FILE__, __LINE__);
	sunxi_usb_main_loop(2500);

	return 0;
}

int script_init(void);

static init_fnc_t init_sequence_f[] = {
	timer_init,		/* initialize timer */
	serial_init,		/* serial communications setup */

	init_func_pmubus,

	reserve_round_4k,
	reserve_mmu,

	cache_enable,
#ifndef CONFIG_SPL_BUILD
	reserve_malloc,
	reserve_board,
#endif
	initr_malloc,

	reserve_stacks,
	initr_enable_interrupts,

	sunxi_usb_efex_reg,


};

void board_init_f(ulong boot_flags)
{
	int *cp = (int *)__bss_start;
	int *_end = (int *)__bss_end;
#ifdef CONFIG_SYS_GENERIC_GLOBAL_DATA
	/*
	 * For some archtectures, global data is initialized and used before
	 * calling this function. The data should be preserved. For others,
	 * CONFIG_SYS_GENERIC_GLOBAL_DATA should be defined and use the stack
	 * here to host global data until relocation.
	 */
	gd_t data;

	gd = &data;

	/*
	 * Clear global data before it is accessed at debug print
	 * in initcall_run_list. Otherwise the debug print probably
	 * get the wrong vaule of gd->have_console.
	 */
	zero_global_data();
#endif

	gd->flags = boot_flags;
	gd->have_console = 0;
	gd->debug_mode = 1;
		
	/* Zero out BSS */
	while (cp < _end)
		*cp++ = 0;
	
	if (initcall_run_list(init_sequence_f))
	{
		hang();
	}

#if !defined(CONFIG_ARM) && !defined(CONFIG_SANDBOX)
	/* NOTREACHED - jump_to_copy() does not return */
	hang();
#endif
}


void board_init_r(gd_t *new_gd, ulong dest_addr)
{
	while(1);
}

void s_init(void)
{
}

__weak void cpu_spin_lock(unsigned int *lock)
{
}
__weak unsigned int cpu_spin_trylock(unsigned int *lock)
{
        return 0;
}
__weak void cpu_spin_unlock(unsigned int *lock)
{
}
int get_core_pos(void)
{
	return 0;
}
