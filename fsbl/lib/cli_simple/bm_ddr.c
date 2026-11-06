/*
 * ddr basic test for aA53
 */
//#include <arch_helpers.h>
#include <assert.h>
#include <debug.h>
#include <mmio.h>
#include <delay_timer.h>
#include <errno.h>
#include <platform_def.h>
#include <string.h>
#include <utils.h>
#include <bm_ddr.h>
#include <cli.h>

#define BIT_ERR_PRINT
#define BIT0   (1 << 0)
#define DDR_BANK_NUM            0x4
#define DDR_MEM_START_ADDR      0x100000000
#define DDR_MEM_BANK_SIZE       0x80000000

#define DDR_BASIC_TEST
//#define DDR_FULLCHIP_SCAN
#define HIGH_BYTE_CLR    (0x00FFFFFF)

#ifdef DDR_FULLCHIP_SCAN
int ddr_fullchip_scan(void)
{
  unsigned long long i;
  unsigned long long base = (unsigned long long)DDR_MEM_START_ADDR;
  unsigned long long total_size = (unsigned long long)DDR_MEM_BANK_SIZE * DDR_BANK_NUM;
  INFO("%s\n", __func__);
  INFO("total_size: 0x%llx\n", total_size);

  for (i = 0; i < total_size; i += 4) {
    mmio_write_32(base + i, i);
    //INFO("wdata:0x%llx\n", i);
    if ((i & 0xffffff) == 0)
      INFO("Writing 0x%llx\n", base + i);
  }
  //dump_hex((char *)"Write data", (void *)base, check_len);

  for (i = 0; i < total_size; i += 4) {
    unsigned long long rdata = mmio_read_32(base + i);
    //INFO("rdata:0x%llx\n", rdata);
    if (rdata != i) {
      INFO("error address:0x%llx, expdata:0x%llx, rdata:0x%llx\n", base + i, i, rdata);
      return -1;
    }
    if ((i & 0xffffff) == 0)
      INFO("Reading 0x%llx\n", base + i);
  }
  //dump_hex((char *)"Check data", (void *)base, check_len);

  return 0;
}
#endif

int ddr_basic_test(unsigned long addr, unsigned long size, unsigned long iter)
{
  INFO("%s\n", __func__);
  INFO("addr:0x%lx\n", addr);
  INFO("size:0x%lx\n", size);
  INFO("iter:0x%lx\n", iter);
  for (int i = 0; i < iter; i++) {
    for(unsigned long j = 0; j < size; j += 4) {
      if (j % 0x100000 == 0)
        INFO("WR 0x%lx ...\n", j);
      mmio_write_32(addr + j, j);
      INFO("addr:0x%lx,wdata:0x%lx\n",addr + j, j);
      unsigned long rdata = mmio_read_32(addr + j);
      INFO("addr:0x%lx,rdata:0x%lx\n",addr + j, rdata);
      if (rdata != j) {
        ERROR("%s failed\n", __func__);
        return -1;
      }
    }
  }

  INFO("!!!!!!!!!!!!!!!!!!%s pass!!!!!!!!!!!!!!!\n", __func__);
  return 0;
}

int mem_read_fix(unsigned long src, unsigned long size)
{
	NOTICE("DDR R/W test from 0 ~ 0x10000000.\n");
	uint64_t i;
	uint32_t u4_value;
	unsigned long errcount =0;
//	while (1)
		for (i=src; i < src + size; i = i+4) {
		//	NOTICE("i=%lx, length=%lx \n",i, src + size);
			if (i%0x1000000 == 0)
				NOTICE(".");
			u4_value =  mmio_read_32(i);

			if(i%0x400 == 0)
				udelay(1);

			if(((i%0x8==0)&&(u4_value != 0x5a5a5a5a))) {
				NOTICE("read error: 0x%lx = 0x%x errcount(%ld) \n", i, u4_value,  errcount++);

				NOTICE("read : 0x%lx = 0x%x\n", i, mmio_read_32(i));
				NOTICE("read : 0x%lx = 0x%x\n", i, mmio_read_32(i));
			}

			if(((i%0x8!=0)&&(u4_value != 0xa5a5a5a5))) {
				NOTICE("read error: 0x%lx = 0x%x errcount(%ld) \n", i, u4_value,  errcount++);

				NOTICE("read : 0x%lx = 0x%x\n", i, mmio_read_32(i));
				NOTICE("read : 0x%lx = 0x%x\n", i, mmio_read_32(i));
			}
		}
	return 0;
}

int mem_write_fix(unsigned long scr, unsigned long size)
{
	NOTICE("DDR write test from 0 ~ 0x10000000.\n");
	uint64_t i;
	//uint32_t u4_value;

	for (i=scr; i < scr + size; i = i+4)
	{
		if(i%0x8 == 0)
			mmio_write_32(i, 0x5a5a5a5a);
		else
			mmio_write_32(i, 0xa5a5a5a5);

		if (i%0x1000000 == 0)
			NOTICE(".");
	}
	NOTICE("DDR W finish. delay 10s\n");

	return 0;
}

int mem_wr_fix(void)
{
	NOTICE("DDR R/W test from 0 ~ 0x10000000.\n");
	uint64_t i;
	uint32_t u4_value;

	for (i=0x100000000; i <0x110000000; i = i+4)
	{
		if(i%0x8 == 0)
			mmio_write_32(i, 0x5a5a5a5a);
		else
			mmio_write_32(i, 0xa5a5a5a5);

		if (i%0x1000000 == 0)
			NOTICE(".");
	}
	NOTICE("DDR W finish. delay 10s\n");

	//mmio_write_32(0x031c + PHYD_BASE_ADDR, 0xffffffff);

	//NOTICE("0x%lx = 0x%x.\n", 0x031c + PHYD_BASE_ADDR, mmio_read_32(0x031c + PHYD_BASE_ADDR));

	mdelay(10000);

	NOTICE("DDR Read:\n");

	while (1)
		for (i=0x100000000; i < 0x110000000; i = i+4) {
			if (i%0x1000000 == 0)
				NOTICE(".");
			u4_value = mmio_read_32(i);

			if(i%0x400 == 0)
				udelay(1);

			if(((i%0x8==0)&&(u4_value != 0x5a5a5a5a))) {
				NOTICE("read error: 0x%lx = 0x%x\n", i, u4_value);

				NOTICE("read : 0x%lx = 0x%x\n", i, mmio_read_32(i));
				NOTICE("read : 0x%lx = 0x%x\n", i, mmio_read_32(i));
			}

			if(((i%0x8!=0)&&(u4_value != 0xa5a5a5a5))) {
				NOTICE("read error: 0x%lx = 0x%x\n", i, u4_value);

				NOTICE("read : 0x%lx = 0x%x\n", i, mmio_read_32(i));
				NOTICE("read : 0x%lx = 0x%x\n", i, mmio_read_32(i));
			}
		}

	NOTICE("DDR R finish.\n");
	return 0;
}
//first write size patt then read check
int mem_mw_64(unsigned long addr, unsigned long size, unsigned long patt, unsigned long iter)
{
  unsigned long errcount =0;
  NOTICE("\n\n");
  NOTICE("%s\n", __func__);
  NOTICE("addr:0x%lx\n", addr);
  NOTICE("size:0x%lx\n", size);
  NOTICE("patt:0x%lx\n", patt);
  NOTICE("iter:0x%lx\n", iter);

  for (int i = 0; i < iter; i++) {
	//write
	 for(unsigned long j = 0; j < size; j += 8) {
		  if (j % 0x1000000 == 0)
			NOTICE("%s write64\n", __func__);
		mmio_write_64(addr + j, patt);
	  }
	//read
	for(unsigned long j = 0; j < size; j += 8) {
		if (j % 0x1000000 == 0)
			NOTICE("%s read64\n", __func__);
		unsigned long rdata = mmio_read_64(addr + j);
		if (rdata != patt) {
			for(int k=0; k < 5; k++) {
				rdata = mmio_read_64(addr + j);
				if (rdata == patt) {
					break;
				}
				//ERROR("addr:0x%llx,rdata:0x%llx\n",addr + j, rdata);
				ERROR("addr:0x%lx,rdata:0x%lx ,err(%ld)\n",addr + j, rdata, errcount++);
			}
		}
	  }
  }
  NOTICE("!!!!!!!!!!!!!!!!!!%s over!!!!!!!!!!!!!!!\n", __func__);
  return 0;
}

//read & check patt
int mem_rd_64(unsigned long addr, unsigned long size, unsigned long patt, unsigned long iter)
{
	unsigned long errcount =0;

	NOTICE("\n\n");
	NOTICE("%s\n", __func__);
	NOTICE("addr:0x%lx\n", addr);
	NOTICE("size:0x%lx\n", size);
	NOTICE("patt:0x%lx\n", patt);
	NOTICE("iter:0x%lx\n", iter);
	//read
	for (int i = 0; i < iter; i++) {
	for(unsigned long j = 0; j < size; j += 8) {
		if (j % 0x1000000 == 0)
			NOTICE("%s read64\n", __func__);
		unsigned long rdata = mmio_read_64(addr + j);
		if (rdata != patt) {
#if 0
			for(int k=0; k < 5; k++) {
				rdata = mmio_read_64(addr + j);
				if (rdata == patt) {
					break;
				}
				ERROR("addr:0x%llx,rdata:0x%llx ,err(%lld)\n",addr + j, rdata, errcount++);
			}
#endif
			ERROR("addr:0x%lx,rdata:0x%lx ,err(%ld)\n",addr + j, rdata, errcount++);
		}

		//NOTICE("addr:0x%lx,rdata:0x%lx\n",addr + j, rdata);
	  }
	}
	NOTICE("!!!!!!!!!!!!!!!!!!%s over!!!!!!!!!!!!!!!\n", __func__);
	return 0;
}

//first write size patt then read check
int mem_mw3(unsigned long addr, unsigned long size, unsigned long patt, unsigned long iter)
{
  NOTICE("%s\n", __func__);
  NOTICE("addr:0x%lx\n", addr);
  NOTICE("size:0x%lx\n", size);
  NOTICE("patt:0x%lx\n", patt);
  NOTICE("iter:0x%lx\n", iter);

  for (int i = 0; i < iter; i++) {
	//write
	 for(unsigned long j = 0; j < size; j += 4) {
		  if (j % 0x1000000 == 0)
			NOTICE("%s write\n", __func__);
		mmio_write_32(addr + j, patt);
	  }
	//read
	for(unsigned long j = 0; j < size; j += 4) {
		if (j % 0x1000000 == 0)
			NOTICE("%s read\n", __func__);
		unsigned long rdata = mmio_read_32(addr + j);
		if (rdata != patt) {
			for(int k=0; k < 5; k++) {
				rdata = mmio_read_32(addr + j);
				if (rdata == patt) {
					break;
				}
				ERROR("addr:0x%lx,rdata:0x%lx\n",addr + j, rdata);
			}
		}
	  }
  }
  NOTICE("!!!!!!!!!!!!!!!!!!%s over!!!!!!!!!!!!!!!\n", __func__);
  return 0;
}

//first write size data then read size theck
int mem_mw2(unsigned long addr, unsigned long size, unsigned long iter)
{
  INFO("%s\n", __func__);
  INFO("addr:0x%lx\n", addr);
  INFO("size:0x%lx\n", size);
  INFO("iter:0x%lx\n", iter);
  for (int i = 0; i < iter; i++) {
//write
	 for(unsigned long j = 0; j < size; j += 4) {
		mmio_write_32(addr + j, j);
	  }
//read
	for(unsigned long j = 0; j < size; j += 4) {
		unsigned long rdata = mmio_read_32(addr + j);
		if (rdata != j) {
			for(int k=0; k < 5; k++) {
				rdata = mmio_read_32(addr + j);
				if (rdata == j) {
					break;
				}
				ERROR("addr:0x%lx,rdata:0x%lx\n",addr + j, rdata);
			}
		}
	  }
	  NOTICE("!!!!!!!!!!!!!!!!!!%s pass!!!!!!!!!!!!!!!\n", __func__);
  }
  return 0;
}

//write size data
int mem_mw(unsigned long addr, unsigned long size, unsigned long iter)
{
  NOTICE("%s\n", __func__);
  NOTICE("addr:0x%lx\n", addr);
  NOTICE("size:0x%lx\n", size);
  NOTICE("iter:0x%lx\n", iter);
  for (int i = 0; i < iter; i++) {
    for(unsigned long j = 0; j < size; j += 4) {
      if (j % 0x100000 == 0)
		NOTICE("WR 0x%lx ...\n", j);
		mmio_write_32(addr + j, j);
      }
  }
  return 0;
}

int mem_test(unsigned long addr, unsigned long size, unsigned long iter)
{
  INFO("%s\n", __func__);
  int ret = 0;
  ret = ddr_basic_test(addr, size, iter);

#ifdef DDR_FULLCHIP_SCAN
  ret = ddr_fullchip_scan();
#endif
  INFO("%s %s\n", __func__, ret?"failed":"passed");
  return ret;
}

////////////////////////////////////////////////////////
/* axi monitor function */
////////////////////////////////////////////////////////
#define GENMASK(msb, lsb)	((2U << (msb))-(1U << (lsb)))
static inline uint32_t modified_bits_by_value(uint32_t orig, uint32_t value, 
											  uint32_t msb, uint32_t lsb)
{
	uint32_t bitmask = GENMASK(msb, lsb);
	orig &= ~bitmask;
	return (orig | ((value << lsb) & bitmask));
}

void set_axi_mon(uint32_t base, uint64_t st_addr, uint64_t sp_addr) 
{
	//1. axi start addr (0x3_ABCD_EFGH)
	//*(base + 0xc) = 0x0000_0002;	// 3 - 1 = 2
	//*(base + 0x8) = 0xABCD_EFGH;
    //*(base + 0xc) = st_addr>>32;	// 3 - 1 = 2
	//*(base + 0x8) = st_addr&0xffffffff;
    mmio_write_32(base + 0xc, st_addr>>32);
    mmio_write_32(base + 0x8, st_addr&0xffffffff);
	//2. axi stop addr (0x5_ABCD_EFGH)
	//*(base + 0x14) = 0x0000_0004;	// 5 - 1 = 4
	//*(base + 0x10) = 0xABCD_EFGH;
    //*(base + 0x14) = sp_addr>>32;	// 5 - 1 = 4
	//*(base + 0x10) = sp_addr&0xffffffff;
    mmio_write_32(base + 0x14, sp_addr>>32);
    mmio_write_32(base + 0x10, sp_addr&0xffffffff);
	//3. hit_sel by axaddr
	//*(base + 0x4) = 0x1;
    mmio_write_32(base + 0x4, 0x1);
	//4. input_sel = 1
	//*(base + 0x0) = 0x3F000100;
    mmio_write_32(base + 0x0, 0x3F000100);
	//5. clear
	//*(base + 0x0) = 0x00020002;
	//*(base + 0x0) = 0x00020000;
    mmio_write_32(base + 0x0, 0x00020002);
    mmio_write_32(base + 0x0, 0x00020000);
	//6. enable
	//*(base + 0x0) = 0x00010001;
    mmio_write_32(base + 0x0, 0x00010001);
}

//1. enable AXI monitor clk gating
void enalbe_AXI_monitor_clk_gating(void)
{
	int rddata;
    rddata = mmio_read_32(0x6FFF004C);
    rddata = modified_bits_by_value(rddata, 7, 2, 0);
    mmio_write_32(0x6FFF004C, rddata);
    //0x6FFF_004C[2:0] = 0x7
}

//2. enable realtime fabric AXI monior
void enable_real_fabric_axi_monitor(uint64_t st_addr, uint64_t sp_addr)
{
    uint8_t i;
    for (i =0; i < 2; i++) {
        set_axi_mon(0x6fff3000 + 128 * i, st_addr-0x100000000, sp_addr-0x100000000);
    }
}

//3. enable offline fabric AXI monitor
void enable_offline_fabric_axi_monitor(uint64_t st_addr, uint64_t sp_addr)
{
    uint8_t i; 
    for (i =0; i < 2; i++) {
        set_axi_mon(0x6fff4000 + 128 * i, st_addr-0x100000000, sp_addr-0x100000000);
    }
}

void show_real_axi_monitor_hit_cnt()
{
    uint8_t i;
    for (i =0; i < 2; i++) {
	    NOTICE("0x%08x = 0x%08x.\n", 0x6fff3000 + i*0x80 + 0x28, mmio_read_32(0x6fff3000 + i*0x80 + 0x28));
    }
}

void axi_snapshot_all()
{
    mmio_write_32(0x6fff3000, 0x80008);
	mmio_write_32(0x6fff3000, 0x80000);

	mmio_write_32(0x6fff4000, 0x80008);
	mmio_write_32(0x6fff4000, 0x80000);
} 

void show_offline_axi_monitor_hit_cnt(void)
{
    uint8_t i;
    for (i =0; i < 2; i++) {
	    INFO("0x%08x = 0x%08x\n", 0x6fff4000 + i*0x80 + 0x28, mmio_read_32(0x6fff4000 + i*0x80 + 0x28));
    }

}

//call for enable axi monitor
void axi_enable_monitor(void)
{
    enalbe_AXI_monitor_clk_gating();
//设置monitor的起始地址，和结束地址
    enable_real_fabric_axi_monitor(0x100000000, 0x300000000);
//设置monitor的起始地址，和结束地址
    enable_offline_fabric_axi_monitor(0x100000000, 0x300000000);
    axi_snapshot_all();//读monitor的hit cnt前需要call axi_snapshot_all();
    show_offline_axi_monitor_hit_cnt();// 初始打印的hit cnt为0；
}

//call show axi monitor
void axi_monitor_show(void)
{
    axi_snapshot_all();//读monitor的hit cnt前需要call axi_snapshot_all();
    show_offline_axi_monitor_hit_cnt();//此处会打印monitor到访问dram的次数
}
////////////////////////////////////////////////////////
/* gdma function */
////////////////////////////////////////////////////////
#define GDMA_BASE	0x26020000UL
#define GDMA_CMD_BASE	GDMA_BASE
#define GDMA_CMD_SIZE	0x100UL
#define GDMA_CTL_BASE	(GDMA_CMD_BASE + GDMA_CMD_SIZE)
static inline void WRITE_CMD_GDMA(int idx, uint64_t hi, uint64_t lo)
{
	// volatile uint128_t *addr = (uint128_t *)(GDMA_CMD_BASE + idx * 16);
	// *addr = ((uint128_t)hi << 64) | lo;
	// volatile uint64_t *addr = (uint64_t *)(GDMA_CMD_BASE + idx * 16);
	// addr[0] = lo;
	// addr[1] = hi;
	volatile uint32_t *addr = (uint32_t *)(GDMA_CMD_BASE + idx * 16);
	addr[0] = lo & 0xffffffff;
	addr[1] = lo >> 32;
	addr[2] = hi & 0xffffffff;
	addr[3] = hi >> 32;
}

static inline void WRITE_CTL_GDMA(int idx, uint64_t hi, uint64_t lo)
{
	// volatile uint128_t *addr = (uint128_t *)(GDMA_CTL_BASE + idx * 16);
	// *addr = ((uint128_t)hi << 64) | lo;
	// volatile uint64_t *addr = (uint64_t *)(GDMA_CMD_BASE + idx * 16);
	// addr[0] = lo;
	// addr[1] = hi;
	volatile uint32_t *addr = (uint32_t *)(GDMA_CTL_BASE + idx * 16);
	addr[0] = lo & 0xffffffff;
	addr[1] = lo >> 32;
	addr[2] = hi & 0xffffffff;
	addr[3] = hi >> 32;

}

static inline uint32_t gdma_sync_id(void)
{
	return  mmio_read_32(GDMA_CTL_BASE + (128 * 1) / 8) & ~(1 << 20);
}

void gdma_start(uint64_t dst, uint64_t src, uint32_t cnt)
{
	uint64_t low, high;

	if (src >= 0x100000000ul) {
      src |= (1ull << 39);
	}
	if (dst >= 0x100000000ul) {
      dst |= (1ull << 39);
	}
	mmio_write_32(0x26050000, 0xff);//open gdam clock
	WRITE_CTL_GDMA(0, 0, 1 << 1);
	WRITE_CTL_GDMA(1, 0, 0);
	low = ((uint64_t)0) |
		((uint64_t)3 << 32) |
		((uint64_t)0 << 36) |
		((uint64_t)4 << 40);
	high = ((uint64_t)0);
	WRITE_CMD_GDMA(0, high, low);
	low = ((uint64_t)cnt << 32);
	WRITE_CMD_GDMA(1, 0ull, low);
	WRITE_CMD_GDMA(2, 0ull, 0ull);
	WRITE_CMD_GDMA(3, 0ull, 0ull);
	low = src & 0xffffffffff;
	high = dst & 0xffffffffff;
	WRITE_CMD_GDMA(4, high, low);
	WRITE_CMD_GDMA(5, 0ull, 0ull);
}

//return 1:busy,  0:idle
int gdma_poll_state(void)
{
	int ret = 0;
	ret = gdma_sync_id();
//	tf_printf("gmda poll ret(%d)\n",ret);
	return (ret != 1);
}

void gdma_memcpy(uint64_t dst, uint64_t src, uint32_t cnt)
{
#if 0
	// uint64_t start_tick;
	// uint64_t end_tick;
	uint64_t low, high;
	// uint64_t kbs = 0;
	//INFO("%s dst(0x%lx) , src(0x%lx) ,cnt(0x%x):\n",__func__, dst, src, cnt);
	// start_tick = timer_get_tick();
	if (src >= 0x100000000ul) {
      src |= (1ull << 39);
	}
	if (dst >= 0x100000000ul) {
      dst |= (1ull << 39);
	}
	WRITE_CTL_GDMA(0, 0, 1 << 1);
	WRITE_CTL_GDMA(1, 0, 0);
	// for (i = 0; i < 10; ++i) {
	// 	printf("CTL_REG_val, offset 0x%x: 0x%x\n", i * 4, readl((void *)0x26020100 + i * 4));
	// }
	low = ((uint64_t)0) |
		((uint64_t)3 << 32) |
		((uint64_t)0 << 36) |
		((uint64_t)4 << 40);
	high = ((uint64_t)0);
	WRITE_CMD_GDMA(0, high, low);
	low = ((uint64_t)cnt << 32);
	WRITE_CMD_GDMA(1, 0ull, low);
	WRITE_CMD_GDMA(2, 0ull, 0ull);
	WRITE_CMD_GDMA(3, 0ull, 0ull);
	low = src & 0xffffffffff;
	high = dst & 0xffffffffff;
	WRITE_CMD_GDMA(4, high, low);
	WRITE_CMD_GDMA(5, 0ull, 0ull);
	/* wait command done */
	while (gdma_sync_id() != 1)
		;
	// for (i = 0; i < 24; ++i) {
	// 	printf("REG_val, offset 0x%x: 0x%x\n", i * 4, readl((void *)0x26020000 + i * 4));
	// }
#endif
	gdma_start(dst, src, cnt);
	while (gdma_poll_state() != 0);
#if 0
	end_tick = timer_get_tick();
	if (end_tick > start_tick)
		kbs = cal_kbs(end_tick - start_tick, cnt);
	INFO("start -> end tick: %ld -> %ld, gdma_memcpy %ld KBs(%ld MBs)\n",
	       start_tick, end_tick, kbs / 1024, kbs / (1024 * 1024));
#endif
}

//fill data patt for gdam src & dst
uint64_t ddr_fill_fix_patt(uint64_t src, uint64_t dst, uint64_t patt, uint64_t size)
{
	int i;
	uint64_t _src = src;
	uint64_t _dst = dst;

	for (i = 0; i < size * 4; i += 4)
	{
		mmio_write_32((_src + i), patt & 0xffffffff);
		mmio_write_32((_src + i + 4), (patt >> 32) & 0xffffffff);
	}

	for (i = 0; i < size * 4; i += 4)
	{
		mmio_write_32((_dst + i), patt & 0xffffffff);
		mmio_write_32((_dst + i + 4), (patt >> 32) & 0xffffffff);
	}
	return 0;
}

//fill data patt for gdam src & dst
static uint64_t _fill_gdma_patt(uint64_t src, uint64_t dst, uint64_t size)
{
	int i;
	uint64_t _src = src;
	uint64_t _dst = dst;
	uint32_t mask = 0;

	for (i = 0; i < size * 4; i += 4)
	{
		mmio_write_32((_src + i), (_src + i) & 0xffffffff);
	}
	for (i = 0; i < size * 4; i += 4)
	{
		if ((i & 0xffff) < 0x1000){
			mask = 0;
		} else if ((i & 0xffff) < 0x2000){
			mask = 0x1111;
		} else if ((i & 0xffff) < 0x3000){
			mask = 0x2222;
		} else if ((i & 0xffff) < 0x4000){
			mask = 0x3333;
		} else if ((i & 0xffff) < 0x5000){
			mask = 0x4444;
		} else if ((i & 0xffff) < 0x6000){
			mask = 0x5555;
		} else if ((i & 0xffff) < 0x7000){
			mask = 0x6666;
		} else if ((i & 0xffff) < 0x8000){
			mask = 0x7777;
		} else if ((i & 0xffff) < 0x9000){
			mask = 0x8888;
		} else if ((i & 0xffff) < 0xa000){
			mask = 0x9999;
		} else if ((i & 0xffff) < 0xb000){
			mask = 0xaaaa;
		} else if ((i & 0xffff) < 0xc000){
			mask = 0xbbbb;
		} else if ((i & 0xffff) < 0xd000){
			mask = 0xcccc;
		} else if ((i & 0xffff) < 0xe000){
			mask = 0xdddd;
		} else if ((i & 0xffff) < 0xf000){
			mask = 0xeeee;
		} else {
			mask = 0xffff;
		}
		mmio_write_32((_dst + i), ((_src + i) & HIGH_BYTE_CLR) | mask << 24);
	}
	return 0;
}

int ddr_init_pattern(uint64_t src, uint64_t dst, uint64_t size)
{
	//int i;
	NOTICE("%s\n", __func__);
	NOTICE("src:0x%lx\n", src);
	NOTICE("dst:0x%lx\n", dst);
	NOTICE("size:0x%lx\n", size);
#if 0
	for (i = 0; i < size * 4; i += 4)
	{
		mmio_write_32((src + i), (src + i) & 0xffffffff);
	}
	for (i = 0; i < size * 4; i += 4)
	{
		mmio_write_32((dst + i), (src + i) | 0x33300000);
	}
#else
	 _fill_gdma_patt(src, dst, size);
#endif
	return 0;
}

static int ddr_cmp_32bit(uint32_t src, uint32_t dst)
{
	int cnt = 0;
	static int enter = 0;
	for(int i = 0; i < 32; i++){
		if((src & BIT0) != (dst & BIT0)){
			cnt++;
#ifdef  BIT_ERR_PRINT
			tf_printf("%d(%d->%d)", i, (src & BIT0)?1:0, (dst & BIT0)?1:0);
			if(enter++ > 50)
				tf_printf("\n");
#endif
		}
		src = src >> 1;
		dst = dst >> 1;
	}
	INFO("diff cnt(%x)\n", cnt);
	return cnt;
}

uint64_t ddr_gdma_copy(uint64_t src, uint64_t dst, uint64_t size)
{
	int j;
    int	ret;
	uint32_t data;
	uint64_t src_addr, dst_addr;
	uint64_t  abit_err_cnt =0;
	uint64_t  nbit_err_cnt =0;

	NOTICE("%s\n", __func__);
	NOTICE("src:0x%lx\n", src);
	NOTICE("dst:0x%lx\n", dst);
	NOTICE("size:0x%lx\n", size);
	src_addr = src;
	dst_addr = dst;
    INFO("\n!!!axim start:\n");
	axi_enable_monitor();

	mmio_write_32(0x26050000, 0xff);//open gdam clock
	_fill_gdma_patt(src, dst, size);

    INFO("%s src(%p):s(0x%x), e(0x%x) \n", __func__,(void *)src_addr,
			*(char *)src_addr, *(char *)(src_addr + size - 1));
    INFO("!!!axim step1:\n");
	axi_monitor_show();
	// timer_meter_start();
	gdma_memcpy(dst_addr, src_addr, size);
	// ticks = timer_meter_get_tick();
    INFO("!!!axim step2:\n");
	axi_monitor_show();
    INFO("%s dst(%p):s(0x%x), e(0x%x)\n", __func__,(void *)dst_addr,
			*(char *)dst_addr, *(char *)(dst_addr + size * 4 - 1));
#if 0
	kbs = cal_kbs(ticks, size);
	NOTICE("ticks %ld, ddr_gdmacopy %ld KBs(%ld MBs)\n", ticks, kbs / 1024,
	       kbs / (1024 * 1024));
#endif
	// NOTICE("ticks %ld, ddr_gdmacopy %ld Bytes(%ld KBs | %ld MBs)\n", ticks, size / 4);
	data = memcmp((void *)src_addr, (void *)dst_addr, size * 4);
    INFO("!!!axim step3:\n");
	axi_monitor_show();
	if (data) {
		for (j = 0; j < size * 4; j += 4) {
			if (*(uint32_t *)src_addr != *(uint32_t *)dst_addr) {
#if 0
				if(((*(uint32_t *)src_addr ^ *(uint32_t *)dst_addr) != 0) &&
					(*(uint32_t *)dst_addr & 0x33000000) !=  0x33000000 )
#endif
				ret = ddr_cmp_32bit(*(uint32_t *)src_addr, *(uint32_t *)dst_addr);
				if (ret == 2) {
					//tf_printf("*");
					nbit_err_cnt++;
				} else if (ret <= 1) {
					//tf_printf(".");
					abit_err_cnt++;
				} else
					nbit_err_cnt++;
				ERROR("src: %p, dst: %p, data 0x%x : 0x%x\n",
				   (void *)src_addr, (void *)dst_addr,
				   *(uint32_t *)src_addr, *(uint32_t *)dst_addr);
			}
			src_addr += 4;
			dst_addr += 4;
		}
	} else {
		NOTICE("%s success!!!\n", __func__);
	}
	NOTICE("1biterr:%ld\n", abit_err_cnt );
	NOTICE("nbiterr:%ld\n", nbit_err_cnt);
    INFO("!!!axim step4:\n");
	axi_monitor_show();
	return (abit_err_cnt + nbit_err_cnt);
}
