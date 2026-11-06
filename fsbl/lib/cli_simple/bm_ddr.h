#ifndef __BM_DDR_H__
#define __BM_DDR_H__

int mem_test(unsigned long addr, unsigned long size, unsigned long iter);
int mem_mw(unsigned long addr, unsigned long size, unsigned long iter);
int mem_mw2(unsigned long addr, unsigned long size, unsigned long iter);
int mem_mw3(unsigned long addr, unsigned long size, unsigned long patt, unsigned long iter);
int mem_mw_64(unsigned long addr, unsigned long size, unsigned long patt, unsigned long iter);
int mem_rd_64(unsigned long addr, unsigned long size, unsigned long patt, unsigned long iter);
int mem_wr_fix(void);
int mem_read_fix(unsigned long src, unsigned long size);
int mem_write_fix(unsigned long scr, unsigned long dst);
uint64_t ddr_gdma_copy(uint64_t src, uint64_t dst, uint64_t size);
int ddr_init_pattern(uint64_t src, uint64_t dst, uint64_t size);

void gdma_start(uint64_t dst, uint64_t src, uint32_t cnt);
int gdma_poll_state(void);
int ddr_memtest_test(void);

void gdma_memcpy(uint64_t dst, uint64_t src, uint32_t cnt);
uint64_t ddr_fill_fix_patt(uint64_t src, uint64_t dst, uint64_t patt, uint64_t size);
#endif
