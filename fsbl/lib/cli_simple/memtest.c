#include <assert.h>
#include <debug.h>
#include <mmio.h>
#include <delay_timer.h>
#include <errno.h>
#include <platform_def.h>
#include <string.h>
#include <utils.h>
#include <bm_ddr.h>

/*
 * only test ddr1~ddr3
 * ddr0 for linux os
 *
 */
#if 0
typedef		unsigned char      uint8_t;
typedef		signed char         int8_t;
typedef		unsigned short    uint16_t;
typedef		signed short       int16_t;
typedef		unsigned int      uint32_t;
typedef		signed int         int32_t;
typedef	unsigned long long	  uint64_t;
#endif
enum _rw_type{
	RW_BYTE,
	RW_WORD,
	RW_DWORD,
	RW_QWORD,
};

typedef struct _sg2042_ddr{
	uint8_t      idx;
	uint32_t     rw_size;
	uint64_t	 addr_start;
	uint64_t	 addr_end;
}sg2042_ddr;

sg2042_ddr ddr_slt_area[] = {
	{4 , RW_DWORD, 0x0100000000, 0x010fffffff },//ddr3
};

static const uint32_t bitpattern[] = {
	0x00000001, /* single bit */
	0x00000003, /* two adjacent bits */
	0x00000007, /* three adjacent bits */
	0x0000000F, /* four adjacent bits */
	0x00000005, /* two non-adjacent bits */
	0x00000015, /* three non-adjacent bits */
	0x00000055, /* four non-adjacent bits */
	0xaaaaaaaa, /* alternating 1/0 */
};
#if 0
static uint64_t mmio_read_64(uint64_t addr)
{
	uint64_t val = *(volatile uint64_t*)addr;

	INFO("0x%llx <- 0x%llx\n", addr, val);
	return val;
}

static void mmio_write_64(uint64_t addr, uint64_t val)
{
	*(volatile uint64_t *)addr = val;
	INFO("0x%llx -> 0x%llx\n", addr, val);
}

static uint32_t mmio_read_32(uint64_t addr)
{
	uint32_t val = *(volatile uint32_t *)addr;

	INFO("0x%llx <- 0x%x\n", addr, val);
	return val;
}

static void mmio_write_32(uint64_t addr, uint32_t val)
{
	*(volatile uint32_t *)addr = val;
	INFO("0x%llx -> 0x%x\n", addr, val);
}

static uint8_t mmio_read_8(uint64_t addr)
{
	uint8_t val = *(volatile uint8_t *)addr;

	INFO("0x%llx <- 0x%x\n", addr, val);
	return val;
}

static void mmio_write_8(uint64_t addr, uint8_t val)
{
	*(volatile uint8_t *)addr = val;
	INFO("0x%llx -> 0x%x\n", addr, val);
}
#endif
int ddr_mem_pattern_scan(uintptr_t addr, uint32_t rw_size, uint32_t scan_size)
{
	uint32_t val,readback;
	uintptr_t rw_addr;
	uint32_t rw_count;
	for (int j = 0; j < sizeof(bitpattern) / sizeof(bitpattern[0]); j++) {
		val = bitpattern[j];

		for (; val != 0; val <<= 1) {
			rw_addr = addr;
			rw_count = scan_size / rw_size;
			while(rw_count--){
				mmio_write_32(rw_addr, val);
				rw_addr +=rw_size;
				//NOTICE("w a(%lx) v(%x)\r\n", rw_addr, val);
			}

			rw_addr = addr;
			rw_count = scan_size / rw_size;
			while(rw_count--){
				readback = mmio_read_32(rw_addr);
				//NOTICE("r a(%lx) v(%x)\r\n", rw_addr, readback);
                udelay(1);
				if (readback != val) {
					 ERROR("FAILURE (data line): ""expected %08x, actual %08x, addr=0x%lx\n",
							val, readback, rw_addr);
					return -1;
				}
				rw_addr +=rw_size;
			}

			rw_addr = addr;
			rw_count = scan_size / rw_size;
			while(rw_count--){
				mmio_write_32(rw_addr, ~val);
				//NOTICE("w a(%lx) v(%x)\r\n", rw_addr, ~val);
				rw_addr +=rw_size;
			}

			rw_addr = addr;
			rw_count = scan_size / rw_size;
			while(rw_count--){
				readback = mmio_read_32(rw_addr);
				//NOTICE("r a(%lx) v(%x)\r\n", rw_addr, readback);
				if (readback != ~val) {
					ERROR("FAILURE (data line): ""Is %08x, should be %08x, addr=0x%lx\n",
							readback, ~val, rw_addr);
					return -1;
				}
				rw_addr +=rw_size;
			}
		}
	}
	//DBG("%s addr=0x%llx success!\r\n",__func__, addr);
	return 0;
}

int ddr_memtest_test(void)
{
	uint32_t ret;
	uint32_t rw_step=4;
	uint32_t scan_step;
	uint32_t scan_count;
	uintptr_t addr;
	uint64_t mem_size;


	for(int i = 0; i < sizeof(ddr_slt_area) / sizeof(ddr_slt_area[0]); i++) {
		mem_size = ddr_slt_area[i].addr_end - ddr_slt_area[i].addr_start + 1;
		addr = (uint64_t)ddr_slt_area[i].addr_start;

		NOTICE("DDR%d, pa=0x%lx,map_base=0x%lx,size=0x%lx \r\n",
			 ddr_slt_area[i].idx,
			 ddr_slt_area[i].addr_start,
			 addr,
			 mem_size);
		if (ddr_slt_area[i].rw_size == RW_BYTE){
			 rw_step = 1;

		} else if (ddr_slt_area[i].rw_size == RW_DWORD){
			 rw_step = 4;
		}
		scan_step = 0x100000;
		scan_count = mem_size / scan_step;
		//scan_count = 0x10;

		NOTICE("rw_step(%x), scan_step=0x%x, scan_count=0x%x \r\n", rw_step, scan_step, scan_count);

		for(int k = 0; k < scan_count; k++){
			NOTICE("addr(%lx)\r\n", addr);
			ret = ddr_mem_pattern_scan(addr, rw_step, scan_step);
			if(ret != 0)
				break;
			addr += scan_step;
		}
		if(ret != 0)
			NOTICE("DDR%d,scan size=0x%lx failed!\r\n",
				 ddr_slt_area[i].idx,
				 mem_size);
		else
			NOTICE("DDR%d,scan size=0x%lx success!\r\n",
				 ddr_slt_area[i].idx,
				 mem_size);

	}
	return 0;
}



















