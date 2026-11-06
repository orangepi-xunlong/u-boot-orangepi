#ifndef __CDMA_TEST_
#define __CDMA_TEST_
#include "mmio.h"

typedef unsigned int u32;
typedef unsigned long long u64;

void cpu_fill(u64 src, u32 len, u32 val);
void cpu_cp(u64 dst, u64 src, u32 len);
void cdma_cp(u64 dst, u64 src, u32 len);
void gdma_cp(u64 dst, u64 src, u32 len);
u32 cpu_rd(u64 src, u32 len, u32 val, u32 cnt);
int cpu_cmp(u64 src, u64 dst, u32 len);
void cpu_poll(u64 src, u32 len, u32 poll_num, u32 flag);
void cpu_byte3(u64 src, u32 len, u32 poll_num, u32 flag);
void cpu_poll_z(u64 src, u32 len, u32 poll_num, u32 flag);
void cpu_2bit_w(u64 src, u32 len, u32 flag);
void cpu_2bit_r(u64 src, u32 len, u32 poll_num, u32 flag);
u32 cpu_rd_gpio(u64 src, u32 len, u32 val, u32 cnt);
void cpu_poll_gpio(u64 src, u32 len, u32 poll_num, u32 flag);
#endif
