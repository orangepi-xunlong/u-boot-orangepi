#include <common.h>
#include <asm/global_data.h>
#include <spare_head.h>

/*
 * About this defination, please refer to common/board_f.c
 * Pointer to initial global data area
 *
 * Here we initialize it if needed.
 */
#ifdef XTRN_DECLARE_GLOBAL_DATA_PTR
#undef  XTRN_DECLARE_GLOBAL_DATA_PTR
#define XTRN_DECLARE_GLOBAL_DATA_PTR    /* empty = allocate here */
DECLARE_GLOBAL_DATA_PTR = (gd_t *) (CONFIG_SYS_INIT_GD_ADDR);
#else
DECLARE_GLOBAL_DATA_PTR;
#endif

/*
 * one element record one function event : enter or exit
 * time [bit 31 ~ 3] : event time stamp, uint16_t about 32 seconds,
 *                     which is enough for boot0 & uboot
 * time [bit  2 ~ 0] : enter or exit flag
 * funaddr : function address, using "unsigned long"
 *           (32-bits in arm, 64-bits in arm64)
 * In 32-bit-ARCH : 8-bytes can record one event
 * In 64-bit_ARCH : need 12-bytes
 */
struct ff_one_element {
	uint32_t time;
	unsigned long funaddr;
};
#define ELEMENT_FLAG_BITS (3)
#define ELEMENT_FLAG_MASK ((1<<ELEMENT_FLAG_BITS)-1)

struct aw_uboot_trace_buf_head {
	unsigned long reloc_off;
	uint32_t valid_data_size;
};
#define FF_BUF_HEAD_SIZE   (sizeof(struct aw_uboot_trace_buf_head))

/* total memory size for recording, suggest use 1M bytes space */
#define FF_PRINT_BUF_SIZE  (2*1024*1024)
/* memory size for recording events-info,reserve 128-bytes */
#define FF_MAX_PRINT_BUF (FF_PRINT_BUF_SIZE - FF_BUF_HEAD_SIZE)
/* the max number of events */
#define FF_MAX_ELEMENT_NUM (FF_MAX_PRINT_BUF/sizeof(struct ff_one_element))
#define	FF_FUN_ENTER_FLAG (1)
#define FF_FUN_EXIT_FLAG  (2)
#define FF_BUF_NOT_FULL   (1)
#define FF_BUF_FULL       (2)

/*
 * Please note this array must:
 * 1. locate in bss section
 * 2. 16 bytes aligne
 * 3. note the relocation issue
 */
char ff_print_buf[FF_PRINT_BUF_SIZE] __aligned(16);

/*
 * because of the affect of bss 0-clear of relocation-code
 * bellow variables can not be located at bss section
 * so do not set 0 to them when initialization
 */
static char *ff_buf_cur = (char *)ff_print_buf + FF_BUF_HEAD_SIZE;
static int ff_buf_element_cnt = 1;
static int ff_buf_full_flag = FF_BUF_NOT_FULL;

/*
 * 64bit arch timer.CNTPCT
 * Freq = 24000000Hz
 */
u64  __attribute__((__no_instrument_function__)) fun_gcc_get_arch_counter(void)
{
	u32 low = 0, high = 0;
	asm volatile("mrrc p15, 0, %0, %1, c14"
		: "=r" (low), "=r" (high)
		:
		: "memory");
	return ((u64)high)<<32 | low;
}

int  __attribute__((__no_instrument_function__))
		ff_handle_one_event(void *this_func, uint32_t flag)
{
	uint64_t time;
#define ARM64_HANDLE_LOW32BITS  (32)
#define ARM64_HANDLE_LOW32BITS_MASK (0xffffffff)
	/*
	 * Only enable trace funtion in BOOT mode
	 * In other modes, exit directly without error
	 */
	if (WORK_MODE_BOOT != uboot_spare_head.boot_data.work_mode)
		return 0;

	if (ff_buf_element_cnt > FF_MAX_ELEMENT_NUM) {
		ff_buf_full_flag = FF_BUF_FULL;
		return -1;
	}

	time = fun_gcc_get_arch_counter();
	time = time / 24;
	*((uint32_t *)ff_buf_cur) = ((uint32_t)(time<<ELEMENT_FLAG_BITS))
				| ((uint32_t)(flag & ELEMENT_FLAG_MASK));
	ff_buf_cur += sizeof(uint32_t);
	/*
	 * this "if-else" is used for ARM64. In arm64
	 * unsigned long is 8-bytes, for saving space,
	 * current version use 12-bytes for on event-trace data.
	 * So fun-address may not be
	 * 8-byte-alligned.
	 * Here, all see it as little-endian ARCH to store data,
	 * if machine is big-endian in real board,
	 * please handle it by parser script.
	 * if it ARM64, must define Macro AW_FF_CONFIG_ARM64
	 * in the "include/configs/sunxxxxx.h"
	 */
#ifndef CONFIG_AW_FF_CONFIG_ARM64
	*((unsigned long *)ff_buf_cur) = (unsigned long)this_func;
#else
	*((uint32_t *)ff_buf_cur) = (uint32_t)((unsigned long)this_func
					& ARM64_HANDLE_LOW32BITS_MASK);
	*((uint32_t *)(ff_buf_cur + sizeof(uint32_t))) =
	(uint32_t)((unsigned long)this_func >> ARM64_HANDLE_LOW32BITS);
#endif
	ff_buf_cur += sizeof(unsigned long);
	ff_buf_element_cnt++;
	return 0;
}

void __attribute__((__no_instrument_function__))
	__cyg_profile_func_enter(void *this_func, void *call_site)
{
	ff_handle_one_event(this_func, FF_FUN_ENTER_FLAG);
	return;
}

void __attribute__((__no_instrument_function__))
	__cyg_profile_func_exit(void *this_func, void *call_site)
{
	ff_handle_one_event(this_func, FF_FUN_EXIT_FLAG);
	return;
}

int __attribute__((__no_instrument_function__)) ff_ouput_print_buf(void)
{
	return 0; /* only open this function when debug*/
	int i;
	struct ff_one_element *element;
	char *buf_pointer = (char *)ff_print_buf + FF_BUF_HEAD_SIZE;

	printf("Reloc_off : 0x%lx : element_cnt=%d, FF_MAX_ELEMENT_NUM=%d\n",
		gd->reloc_off, ff_buf_element_cnt, FF_MAX_ELEMENT_NUM);

	for (i = 0; i < (ff_buf_element_cnt-1); i++) {
		element = (struct ff_one_element *)((char *)buf_pointer
			+ i*sizeof(struct ff_one_element));
		if (0 == element->time && 0 == element->funaddr) {
			element = (struct ff_one_element *)((char *)buf_pointer
			- gd->reloc_off + i*sizeof(struct ff_one_element));
		}
		printf("%9d:%c:0x%lx\n", (element->time)>>ELEMENT_FLAG_BITS,
			((((element->time)
			& ELEMENT_FLAG_MASK) == FF_FUN_ENTER_FLAG)?'G':'L'),
			element->funaddr);
	}

	return 0;
}


extern int __attribute__((__no_instrument_function__))
	ff_write_fun_trace_data2flash(void *data_buffer,
	unsigned int data_size);
int __attribute__((__no_instrument_function__)) ff_move_reloc_data(void)
{
	unsigned int i;
	unsigned int data_size;
	struct ff_one_element *element;
	struct aw_uboot_trace_buf_head *buf_head;
	char *buf_pointer = (char *)ff_print_buf + FF_BUF_HEAD_SIZE;

	for (i = 0; i < (ff_buf_element_cnt-1); i++) {
		element = (struct ff_one_element *)
			((char *)buf_pointer
			  + i*sizeof(struct ff_one_element));
		if (0 == element->time && 0 == element->funaddr) {
			*element = *((struct ff_one_element *)
				((char *)buf_pointer - gd->reloc_off
				 + i*sizeof(struct ff_one_element)));
		} else {
			break;
		}
	}

	/*
	 * write the buf head size
	 */
	data_size = (ff_buf_element_cnt-1) * sizeof(struct ff_one_element);
	buf_head = (struct aw_uboot_trace_buf_head *)ff_print_buf;
	buf_head->reloc_off = gd->reloc_off;
	buf_head->valid_data_size = data_size;

/* for debug */
#if 0
	printf("buffer head info : reloc=0x%x, data_size=0x%x\n",
		buf_head->reloc_off, buf_head->valid_data_size);
	buf_pointer = (char *)ff_print_buf + FF_BUF_HEAD_SIZE;

	for (i = 0; i < (ff_buf_element_cnt-1); i++) {
		element = (struct ff_one_element *)
			((char *)buf_pointer
			 + i*sizeof(struct ff_one_element));
		printf("* [%d, 0x%x] %9d:%c:0x%lux\n",
			i, (unsigned int)element,
			(element->time)>>ELEMENT_FLAG_BITS,
			((((element->time) & ELEMENT_FLAG_MASK)
			== FF_FUN_ENTER_FLAG)?'G':'L'), element->funaddr);
	}
#endif

	/* flush data to storage from ddr */
	data_size += FF_BUF_HEAD_SIZE;
	ff_write_fun_trace_data2flash(ff_print_buf, data_size);

	return 0;
}

