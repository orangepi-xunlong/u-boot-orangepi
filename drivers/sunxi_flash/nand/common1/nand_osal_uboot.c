/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *

 */
#include <common.h>
#include <console.h>
#include <malloc.h>
#include <stdarg.h>
#include <asm/io.h>
#include <asm/arch/dma.h>
#include <asm/arch/clock.h>
#include <sys_config.h>
//#include <smc.h>
#include <fdt_support.h>
#include <spare_head.h>
#include <sunxi_board.h>
#include "nand_for_clock.h"
#include <fdtdec.h>

//#include <sys_config_old.h>
DECLARE_GLOBAL_DATA_PTR;

#define SPIC0_BASE                      SUNXI_SPI0_BASE
#define SPI_TX_IO_DATA                  (SPIC0_BASE + 0x200)
#define SPI_RX_IO_DATA                  (SPIC0_BASE + 0x300)

#if defined(CONFIG_MACH_SUN8IW7)
#define  NAND_DRV_VERSION_0		0x03
#define  NAND_DRV_VERSION_1		0x6120
#define  NAND_DRV_DATE			0x20190827
#define  NAND_DRV_TIME			0x16801239

#else
#define  NAND_DRV_VERSION_0		0x03
#define  NAND_DRV_VERSION_1		0x6081
#define  NAND_DRV_DATE			0x20210317
#define  NAND_DRV_TIME			0x18212019
#endif

/*nand common1 version rule vx.ab date time
 * x >= 1 ; 00 <= ab <= 99;*/
#define NAND_COMMON1_DRV_VERSION "v2.05 2022-01-13 16:53"

/*
 *1755--AW1755--A50
 *14--uboot2014
 *49--linux4.9
*/
#if defined(CONFIG_MACH_SUN8IW7)
#define NDFC0_BASE_ADDR                 (0x01c03000)
#define NDFC1_BASE_ADDR                 (0x01c05000)

#define PL_CF0_REG		(0x01f02c00)
#define PL_PUL0_REG		(0x01f02c1c)
#define PL_DRV0_REG		(0x01f02c14)
#define PL_DAT_REG		(0x01f02c10)

#else

#ifndef SUNXI_NFC_BASE
#define SUNXI_NFC_BASE  (0)
#endif

#define NDFC0_BASE_ADDR                 (SUNXI_NFC_BASE)
#define NDFC1_BASE_ADDR                 (NULL)
#endif
#define NAND_PIO_WITHSTAND_VOL_MODE	(0x0340)

#define NAND_STORAGE_TYPE_NULL (0)
#define NAND_STORAGE_TYPE_RAWNAND (1)
#define NAND_STORAGE_TYPE_SPINAND (2)

__u32 boot_mode;
extern __u32 storage_type;

#if defined(CONFIG_MACH_SUN8IW7)
	NAND_ClkRequest_t NAND_ClkRequest = NAND_ClkRequest_v0;
	NAND_ClkRelease_t NAND_ClkRelease = NAND_ClkRelease_v0;
	NAND_SetClk_t NAND_SetClk = NAND_SetClk_v0;
	NAND_GetClk_t NAND_GetClk = NAND_GetClk_v0;

#else
	NAND_ClkRequest_t NAND_ClkRequest = NAND_ClkRequest_v1;
	NAND_ClkRelease_t NAND_ClkRelease = NAND_ClkRelease_v1;
	NAND_SetClk_t NAND_SetClk = NAND_SetClk_v1;
	NAND_GetClk_t NAND_GetClk = NAND_GetClk_v1;

#endif /*CONFIG_MACH_SUN8IW7*/


__u32 get_wvalue(__u32 addr)
{
	return readl((void *)(unsigned long)addr);
}

void put_wvalue(__u32 addr, __u32 v)
{
	writel(v, (void *)(unsigned long)addr);
}


__u32 NAND_GetNdfcVersion(void);
void *NAND_Malloc(unsigned int Size);
void NAND_Free(void *pAddr, unsigned int Size);
int NAND_Get_Version(void);
//static __u32 boot_mode;
/* static __u32 gpio_hdl; */
/*static int nand_nodeoffset;*/
static int fdt_node;

static int nand_fdt_get_path_offset(const char *path)
{
	static char *s_path = "none";
	int fdt_node_tmp;

	if ((strlen(path) != strlen(s_path)) || memcmp((char *)s_path, (char *)path, strlen(path))) {
		fdt_node_tmp = fdt_path_offset(working_fdt, path);
		if (fdt_node_tmp < 0) {
			/* printf("get %s err\n", path); */
			return fdt_node_tmp;
		}
		fdt_node = fdt_node_tmp;
		s_path = (char *)path;
	}
	return fdt_node;
}

u32 nand_fdt_get_uint(const char *path, const char *prop_name)
{

	unsigned int val = 0;
	int node;

	node = nand_fdt_get_path_offset(path);
	if (node < 0)
		return 0;

	val = fdtdec_get_uint(working_fdt, node,
			prop_name, 0);
	return val;

}

int nand_fdt_get_int(const char *path, const char *prop_name)
{

	int val = 0;
	int node;

	node = nand_fdt_get_path_offset(path);
	if (node < 0)
		return 0;

	val = fdtdec_get_int(working_fdt, node,
			prop_name, 0);
	return val;

}


const char *nand_fdt_get_string(const char *path, const char *prop_name)
{
	int node;
	const char *str = NULL;

	node = nand_fdt_get_path_offset(path);

	if (node < 0)
		return NULL;


	str = fdt_getprop(working_fdt, node, prop_name, NULL);
	if (str == NULL) {
		return NULL;
	}

	return str;
}

u32 nand_fdt_get_addr(const char *path, const char *prop_name)
{
	struct fdt_resource res;
	int node;
	int index = 0;
	int ret = 0;

	node = nand_fdt_get_path_offset(path);
	if (node < 0)
		return 0;


	ret = fdt_get_resource(working_fdt, node, prop_name, index, &res);
	if (ret < 0) {
		printf("get resource err\n");
		return 0;
	}

	return res.start;
}

__u32 get_storage_type(void)
{
	__u32 ret = NAND_STORAGE_TYPE_NULL;

	if (WORK_MODE_BOOT == get_boot_work_mode()) {
		if (STORAGE_SPI_NAND == get_boot_storage_type_ext())
			ret = NAND_STORAGE_TYPE_SPINAND;
		else if (STORAGE_NAND == get_boot_storage_type_ext())
			ret = NAND_STORAGE_TYPE_RAWNAND;
	} else {
		/* probe and set storage_type in nand lib */
		return storage_type;
	}
	return ret;
}

int NAND_set_boot_mode(__u32 boot)
{
	boot_mode = boot;
	return 0;
}


int NAND_Print(const char *str, ...)
{
	int ret;
	static char _buf[1024];
	va_list args;

	va_start(args, str);
	vsprintf(_buf, str, args);

	ret = tick_printf(_buf);
	va_end(args);

	return ret;
}

int NAND_Print_DBG(const char *str, ...)
{
	int ret;

	static char _buf[1024];
	va_list args;

	va_start(args, str);
	vsprintf(_buf, str, args);

	ret = pr_debug(_buf);
	va_end(args);
	return ret;
}

__s32 NAND_CleanFlushDCacheRegion(void *buff_addr, __u32 len)
{
	flush_cache((ulong)buff_addr, len);
	return 0;
}
#if 1 //181105 DMA
__s32 NAND_InvaildDCacheRegion(__u32 rw, __u32 buff_addr, __u32 len)
{
	if (rw == 0)
		invalidate_dcache_range(buff_addr, buff_addr+len);

	return 0;
}

void *NAND_DMASingleMap(__u32 rw, void *buff_addr, __u32 len)
{
	return buff_addr;
}

void *NAND_DMASingleUnmap(__u32 rw, void *buff_addr, __u32 len)
{
	return buff_addr;
}

__s32 NAND_AllocMemoryForDMADescs(__u32 *cpu_addr, __u32 *phy_addr)
{
	void *p = NULL;
	p = (void *)NAND_Malloc(1024);

	if (p == NULL) {
		NAND_Print("NAND_AllocMemoryForDMADescs(): alloc dma des failed\n");
		return -1;
	} else {
		*cpu_addr = (__u32)((unsigned long)p);
		*phy_addr = (__u32)((unsigned long)p);
		NAND_Print_DBG("NAND_AllocMemoryForDMADescs(): cpu: 0x%x  physic: 0x%x\n",
			*cpu_addr, *phy_addr);
	}

	return 0;
}

__s32 NAND_FreeMemoryForDMADescs(__u32 *cpu_addr, __u32 *phy_addr)
{
	NAND_Free((void *)((unsigned long)(*cpu_addr)), 1024);
	*cpu_addr = 0;
	*phy_addr = 0;

	return 0;
}
#endif


__s32 NAND_PIORequest(__u32 nand_index)
{

	if (get_storage_type() == 1) {
		fdt_node = nand_fdt_get_path_offset("/soc/nand0");
		/*fdt_node =  fdt_path_offset(working_fdt, "nand0");*/
		if (fdt_node < 0) {
			NAND_Print("nand0: get node offset error\n");
			return -1;
		}
		if (fdt_set_all_pin("nand0", "pinctrl-0")) {
			NAND_Print("NAND_PIORequest, failed!\n");
			return -1;
		}
	} else if (get_storage_type() == 2) {
		/*fdt_node =  fdt_path_offset(working_fdt, "spi0");*/
		fdt_node = nand_fdt_get_path_offset("spi0");
		if (fdt_node < 0) {
			printf("spi0: get node offset error\n");
			return -1;
		}
		if (fdt_set_all_pin("spi0", "pinctrl-0")) {
			printf("NAND_PIORequest(spi0), failed!\n");
			return -1;
		}
	}

	return 0;
}

__s32 NAND_3DNand_Request(void)
{
	u32 cfg;

	cfg = *(volatile __u32 *)(SUNXI_PIO_BASE + NAND_PIO_WITHSTAND_VOL_MODE);
	cfg ^= 0x4;
	*(volatile __u32 *)(SUNXI_PIO_BASE + NAND_PIO_WITHSTAND_VOL_MODE) = cfg;
	if ((cfg >> 2) == 1)
		printf("Change PC_Power Mode Select to 1.8V\n");
	else
		printf("Change PC_Power Mode Select to 3.3V\n");

	return 0;
}

__s32 NAND_Check_3DNand(void)
{
	u32 cfg;

	cfg = *(volatile __u32 *)(SUNXI_PIO_BASE + NAND_PIO_WITHSTAND_VOL_MODE);
	if ((cfg >> 2) == 0) {
		cfg |= 0x4;
		*(volatile __u32 *)(SUNXI_PIO_BASE + NAND_PIO_WITHSTAND_VOL_MODE) = cfg;
		printf("Change PC_Power Mode Select to 1.8V\n");
	}

	return 0;
}

__s32 NAND_PIOFuncChange_DQSc(__u32 nand_index, __u32 en)
{
	__u32 ndfc_version;
	__u32 cfg;

	ndfc_version = NAND_GetNdfcVersion();
	if (ndfc_version == 1) {
		NAND_Print("NAND_PIOFuncChange_EnDQScREc: invalid ndfc version!\n");
		return 0;
	}

	if (ndfc_version == 2) {

		if (nand_index == 0) {
			cfg = *(volatile __u32 *)(0x06000800 + 0x50);
			cfg &= (~(0x7U<<8));
			cfg |= (0x3U<<8);
			*(volatile __u32 *)(0x06000800 + 0x50) = cfg;
		} else {
			cfg = *(volatile __u32 *)(0x06000800 + 0x128);
			cfg &= (~(0x7U<<8));
			cfg |= (0x3U<<8);
			*(volatile __u32 *)(0x06000800 + 0x128) = cfg;
		}
	}
	return 0;
}
__s32 NAND_PIOFuncChange_REc(__u32 nand_index, __u32 en)
{
	__u32 ndfc_version;
	__u32 cfg;

	ndfc_version = NAND_GetNdfcVersion();
	if (ndfc_version == 1) {
		NAND_Print("NAND_PIOFuncChange_EnDQScREc: invalid ndfc version!\n");
		return 0;
	}

	if (ndfc_version == 2) {

		if (nand_index == 0) {
			cfg = *(volatile __u32 *)(0x06000800 + 0x50);
			cfg &= (~(0x7U<<4));
			cfg |= (0x3U<<4);
			*(volatile __u32 *)(0x06000800 + 0x50) = cfg;
		} else {
			cfg = *(volatile __u32 *)(0x06000800 + 0x128);
			cfg &= (~(0x7U<<4));
			cfg |= (0x3U<<4);
			*(volatile __u32 *)(0x06000800 + 0x128) = cfg;
		}
	}

	return 0;
}

void NAND_PIORelease(__u32 nand_index)
{
	return;
}

void NAND_Bug(void)
{
	BUG();
}

void NAND_Mdelay(unsigned long ms)
{
	mdelay(ms);
}

void NAND_Memset(void *pAddr, unsigned char value, unsigned int len)
{
	memset(pAddr, value, len);
}

void NAND_Memcpy(void *pAddr_dst, void *pAddr_src, unsigned int len)
{
	memcpy(pAddr_dst, pAddr_src, len);
}

int NAND_Memcmp(void *pAddr_dst, void *pAddr_src, unsigned int len)
{
	return memcmp(pAddr_dst, pAddr_src, len);
}

int NAND_Strcmp(const char *s1, const char *s2)
{
	return strcmp(s1, s2);
}


void *NAND_Malloc(unsigned int Size)
{
	void *buf = NULL;

	if (Size == 0) {
		NAND_Print("NAND_Malloc size=0!\n");
		return NULL;
	}

	buf = malloc_align(Size, 64);/*songwj For A50, Buffer Must 64 Bytes align*/
	if (buf == NULL)
		NAND_Print("NAND_Malloc fail!\n");
	return buf;
}

void NAND_Free(void *pAddr, unsigned int Size)
{
	free_align(pAddr);/*songwj For A50, Buffer Must 64 Bytes align*/
}

void *nand_malloc(unsigned int size)
{
	void *buf = NULL;

	if (size == 0) {
		NAND_Print("NAND_Malloc size=0!\n");
		return NULL;
	}

	buf = malloc_align(size, 64);/*songwj For A50, Buffer Must 64 Bytes align*/
	if (buf == NULL)
		NAND_Print("NAND_Malloc fail!\n");
	return buf;
}

void nand_free(void *pAddr)
{
	free_align(pAddr);/*songwj For A50, Buffer Must 64 Bytes align*/
}


void  OSAL_IrqUnLock(unsigned int  p)
{
	;
}
void  OSAL_IrqLock  (unsigned int *p)
{
	;
}

int NAND_WaitRbReady(void)
{
	return 0;
}

void *NAND_IORemap(void *base_addr, unsigned int size)
{
	return (void *)base_addr;
}

void *NAND_VA_TO_PA(void *buff_addr)
{
	return buff_addr;
}

void *RAWNAND_GetIOBaseAddrCH0(void)
{
	return (void *)NDFC0_BASE_ADDR;
}

void *RAWNAND_GetIOBaseAddrCH1(void)
{
	return (void *)NDFC1_BASE_ADDR;
}
void *SPINAND_GetIOBaseAddrCH0(void)
{
	return (void *)SPIC0_BASE;
}

void *SPINAND_GetIOBaseAddrCH1(void)
{
	return NULL;
}

__u32 NAND_GetNdfcVersion(void)
{
	return 1;
}

__u32 NAND_GetNdfcDmaMode(void)
{
	/*
		0: General DMA;
		1: MBUS DMA
	*/
	return 1;
}

int NAND_PhysicLockInit(void)
{
	return 0;
}

int NAND_PhysicLock(void)
{
	return 0;
}

int NAND_PhysicUnLock(void)
{
	return 0;
}

int NAND_PhysicLockExit(void)
{
	return 0;
}

__u32 NAND_GetMaxChannelCnt(void)
{
	return 1;
}

__u32 NAND_GetPlatform(void)
{
	return 64;
}
#if 1
unsigned int dma_chan = 0;

/* request dma channel and set callback function */
int nand_request_dma(void)
{
	dma_chan = sunxi_dma_request(0);
	if (dma_chan == 0) {
		NAND_Print("uboot nand_request_dma: request genernal dma failed!\n");
		return -1;
	} else
		NAND_Print_DBG("uboot nand_request_dma: reqest genernal dma for nand success, 0x%x\n", dma_chan);

	return 0;
}
int NAND_ReleaseDMA(__u32 nand_index)
{
	NAND_Print_DBG("nand release dma:%x\n", dma_chan);
	if (dma_chan) {
		sunxi_dma_release(dma_chan);
		dma_chan = 0;
	}
	return 0;
}


int rawnand_dma_config_start(__u32 write, dma_addr_t addr, __u32 length)
{
	int ret = 0;
	sunxi_dma_set dma_set;

	dma_set.loop_mode = 0;
	dma_set.wait_cyc = 8;
	dma_set.data_block_size = 0;

	if (write) {
		dma_set.channal_cfg.src_drq_type = DMAC_CFG_TYPE_DRAM;
		dma_set.channal_cfg.src_addr_mode = DMAC_CFG_SRC_ADDR_TYPE_LINEAR_MODE;
		dma_set.channal_cfg.src_burst_length = DMAC_CFG_SRC_1_BURST; /* 8 */
		dma_set.channal_cfg.src_data_width = DMAC_CFG_SRC_DATA_WIDTH_32BIT;
		dma_set.channal_cfg.reserved0 = 0;

		dma_set.channal_cfg.dst_drq_type = DMAC_CFG_SRC_TYPE_NAND;
		dma_set.channal_cfg.dst_addr_mode = DMAC_CFG_DEST_ADDR_TYPE_IO_MODE;
		dma_set.channal_cfg.dst_burst_length = DMAC_CFG_DEST_1_BURST; /* 8 */
		dma_set.channal_cfg.dst_data_width = DMAC_CFG_DEST_DATA_WIDTH_32BIT;
		dma_set.channal_cfg.reserved1 = 0;
	} else {
		dma_set.channal_cfg.src_drq_type = DMAC_CFG_SRC_TYPE_NAND;
		dma_set.channal_cfg.src_addr_mode = DMAC_CFG_SRC_ADDR_TYPE_IO_MODE;
		dma_set.channal_cfg.src_burst_length = DMAC_CFG_SRC_1_BURST; /* 8 */
		dma_set.channal_cfg.src_data_width = DMAC_CFG_SRC_DATA_WIDTH_32BIT;
		dma_set.channal_cfg.reserved0 = 0;

		dma_set.channal_cfg.dst_drq_type = DMAC_CFG_TYPE_DRAM;
		dma_set.channal_cfg.dst_addr_mode = DMAC_CFG_DEST_ADDR_TYPE_LINEAR_MODE;
		dma_set.channal_cfg.dst_burst_length = DMAC_CFG_DEST_1_BURST; /* 8 */
		dma_set.channal_cfg.dst_data_width = DMAC_CFG_DEST_DATA_WIDTH_32BIT;
		dma_set.channal_cfg.reserved1 = 0;
	}

	if (sunxi_dma_setting(dma_chan, &dma_set) < 0) {
		NAND_Print("uboot: sunxi_dma_setting for nand faild!\n");
		return -1;
	}

	if (write)
		ret = sunxi_dma_start(dma_chan, addr, (uint)0x01c03300, length);
	else
		ret = sunxi_dma_start(dma_chan, (uint)0x01c03300, addr, length);
	if (ret < 0) {
		NAND_Print("uboot: sunxi_dma_start for nand faild!\n");
		return -1;
	}

	return 0;
}

unsigned int tx_dma_chan;
unsigned int rx_dma_chan;

/* request dma channel and set callback function */
int spinand_request_tx_dma(void)
{
	tx_dma_chan = sunxi_dma_request(0);
	if (tx_dma_chan == 0) {
		printf("uboot nand_request_tx_dma: request genernal dma failed!\n");
		return -1;
	} else
		NAND_Print("uboot nand_request_tx_dma: reqest genernal dma for nand success, 0x%x\n", tx_dma_chan);

	return 0;
}

int spinand_request_rx_dma(void)
{
	rx_dma_chan = sunxi_dma_request(0);
	if (rx_dma_chan == 0) {
		printf("uboot nand_request_tx_dma: request genernal dma failed!\n");
		return -1;
	} else
		NAND_Print("uboot nand_request_tx_dma: reqest genernal dma for nand success, 0x%x\n", rx_dma_chan);

	return 0;
}

int spinand_releasetxdma(void)
{
	printf("nand release dma:%x\n", tx_dma_chan);
	if (tx_dma_chan) {
		sunxi_dma_release(tx_dma_chan);
		tx_dma_chan = 0;
	}

	return 0;
}

int spinand_releaserxdma(void)
{
	printf("nand release dma:%x\n", tx_dma_chan);
	if (rx_dma_chan) {
		sunxi_dma_release(rx_dma_chan);
		rx_dma_chan = 0;
	}

	return 0;
}

void spinand_dma_callback(void *arg)
{

}
#define ALIGN_6BITS (6)
#define ALIGN_6BITS_SIZE (1<<ALIGN_6BITS)
#define ALIGN_6BITS_MASK (~(ALIGN_6BITS_SIZE-1))
#define ALIGN_TO_64BYTES(len) (((len) +63)&ALIGN_6BITS_MASK)
int spinand_dma_config_start(__u32 tx_mode, __u32 addr, __u32 length)
{
	int ret = 0;
	sunxi_dma_set dma_set;

	dma_set.loop_mode = 0;
	dma_set.wait_cyc = 32;
	dma_set.data_block_size = 0;

	if (tx_mode) {
		dma_set.channal_cfg.src_drq_type = DMAC_CFG_TYPE_DRAM;
		dma_set.channal_cfg.src_addr_mode = DMAC_CFG_SRC_ADDR_TYPE_LINEAR_MODE;
		dma_set.channal_cfg.src_burst_length = DMAC_CFG_SRC_8_BURST;
		dma_set.channal_cfg.src_data_width = DMAC_CFG_SRC_DATA_WIDTH_32BIT;
		dma_set.channal_cfg.reserved0 = 0;

		dma_set.channal_cfg.dst_drq_type = DMAC_CFG_TYPE_SPI0;
		dma_set.channal_cfg.dst_addr_mode = DMAC_CFG_DEST_ADDR_TYPE_IO_MODE;
		dma_set.channal_cfg.dst_burst_length = DMAC_CFG_DEST_8_BURST;
		dma_set.channal_cfg.dst_data_width = DMAC_CFG_DEST_DATA_WIDTH_32BIT;
		dma_set.channal_cfg.reserved1 = 0;
	} else {
		dma_set.channal_cfg.src_drq_type = DMAC_CFG_TYPE_SPI0;
		dma_set.channal_cfg.src_addr_mode = DMAC_CFG_SRC_ADDR_TYPE_IO_MODE;
		dma_set.channal_cfg.src_burst_length = DMAC_CFG_SRC_8_BURST;
		dma_set.channal_cfg.src_data_width = DMAC_CFG_SRC_DATA_WIDTH_32BIT;
		dma_set.channal_cfg.reserved0 = 0;

		dma_set.channal_cfg.dst_drq_type = DMAC_CFG_TYPE_DRAM;
		dma_set.channal_cfg.dst_addr_mode = DMAC_CFG_DEST_ADDR_TYPE_LINEAR_MODE;
		dma_set.channal_cfg.dst_burst_length = DMAC_CFG_DEST_8_BURST;
		dma_set.channal_cfg.dst_data_width = DMAC_CFG_DEST_DATA_WIDTH_32BIT;
		dma_set.channal_cfg.reserved1 = 0;
	}

	if (tx_mode) {
		if (sunxi_dma_setting(tx_dma_chan, &dma_set) < 0) {
			printf("uboot: sunxi_dma_setting for tx faild!\n");
			return -1;
		}
	} else {
		if (sunxi_dma_setting(rx_dma_chan, &dma_set) < 0) {
			printf("uboot: sunxi_dma_setting for rx faild!\n");
			return -1;
		}
	}

	if (tx_mode) {
		flush_cache((uint)addr, ALIGN_TO_64BYTES(length));
		ret = sunxi_dma_start(tx_dma_chan, addr, (__u32)SPI_TX_IO_DATA, length);
	} else {
		flush_cache((uint)addr, ALIGN_TO_64BYTES(length));
		ret = sunxi_dma_start(rx_dma_chan, (__u32)SPI_RX_IO_DATA, addr, length);
	}
	if (ret < 0) {
		printf("uboot: sunxi_dma_start for spi nand faild!\n");
		return -1;
	}

	return 0;

}

int NAND_WaitDmaFinish(__u32 tx_flag, __u32 rx_flag)
{
	__u32 timeout = 0xffffff;

	if (tx_flag) {
		timeout = 0xffffff;
		while (sunxi_dma_querystatus(tx_dma_chan)) {
			timeout--;
			if (!timeout)
				break;
		}

		if (timeout <= 0) {
			NAND_Print("TX DMA wait status timeout!\n");
			return -1;
		}
	}

	if (rx_flag) {
		timeout = 0xffffff;
		while (sunxi_dma_querystatus(rx_dma_chan)) {
			timeout--;
			if (!timeout)
				break;
		}

		if (timeout <= 0) {
			NAND_Print("RX DMA wait status timeout!\n");
			return -1;
		}
	}

	return 0;
}

int Nand_Dma_End(__u32 rw, __u32 addr, __u32 length)
{
	if (rw == 0)
		NAND_InvaildDCacheRegion(rw, addr, length);
	return 0;
}

int nand_dma_config_start(__u32 rw, __u32 addr, __u32 length)
{
	int ret = 0;

	if (get_storage_type() == 1)
		ret = rawnand_dma_config_start(rw, addr, length);
	else if (get_storage_type() == 2)
		ret = spinand_dma_config_start(rw, addr, length);

	return ret;
}
#endif

__u32 NAND_GetNandExtPara(__u32 para_num)
{
	unsigned int nand_para;
	/*unsigned int nand0_offset = 0;*/
	char str[9];

	str[0] = 'n';
	str[1] = 'a';
	str[2] = 'n';
	str[3] = 'd';
	str[4] = '0';
	str[5] = '_';
	str[6] = 'p';
	str[7] = '0';
	str[8] = '\0';

	if (para_num == 0) {
		/* frequency */
		str[7] = '0';
	} else if (para_num == 1) {

		/* SUPPORT_TWO_PLANE */
		str[7] = '1';
	} else if (para_num == 2) {
		/* SUPPORT_VERTICAL_INTERLEAVE */
		str[7] = '2';
	} else if (para_num == 3) {
		/* SUPPORT_DUAL_CHANNEL */
		str[7] = '3';
	} else if (para_num == 4) {
		/*frequency change */
		str[7] = '4';
	} else if (para_num == 5) {
		str[7] = '5';
	} else {
		NAND_Print("NAND GetNandExtPara: wrong para num: %d\n", para_num);
		return 0xffffffff;
	}

	/*nand0_offset = fdt_path_offset(working_fdt, "nand0");*/
	/*
	 *if (nand0_offset < 0) {
	 *        printf("get nand0 offset error\n");
	 *        return 0xffffffff;
	 *}
	 */

	/*nand_para = fdtdec_get_uint(working_fdt, fdt_node, str, 0);*/
	nand_para = nand_fdt_get_uint("/soc/nand0", str);
	if (nand_para == 0x55aaaa55)
		return 0xffffffff;
	else
		return nand_para;

}

__u32 NAND_GetNandIDNumCtrl(void)
{
/*
 *        unsigned int nand0_offset = 0;
 *        unsigned int id_number_ctl = 0;
 *
 *        nand0_offset = fdt_path_offset(working_fdt, "nand0");
 *        if (!nand0_offset) {
 *                printf("get nand0 offset error\n");
 *                return 0;
 *        }
 *
 *        id_number_ctl = fdtdec_get_uint(working_fdt, nand0_offset,
 *                        "nand0_id_number_ctl", 0);
 */
	unsigned int id_number_ctl;

	id_number_ctl = nand_fdt_get_uint("/soc/nand0", "nand0_id_number_ctl");
	if (id_number_ctl == 0x55aaaa55)
		return 0;
	else
		return id_number_ctl;

}

void nand_cond_resched(void)
{
	;
}

__u32 NAND_GetNandCapacityLevel(void)
{

/*
 *        unsigned int nand0_offset = 0;
 *        unsigned int capacitylevel = 0;
 *
 *        nand0_offset = fdt_path_offset(working_fdt, "nand0");
 *        if (!nand0_offset) {
 *                printf("get nand0 offset error\n");
 *                return 0;
 *        }
 *
 *        capacitylevel = fdtdec_get_uint(working_fdt, nand0_offset,
 *                        "nand0_capacity_level", 0);
 *        if (capacitylevel == 0x55aaaa55)
 *                return 0;
 *        else
 *                return capacitylevel;
 *
 */
	unsigned int capacitylevel = 0;

	capacitylevel = nand_fdt_get_uint("/soc/nand0", "nand0_capacity_level");
	if (capacitylevel == 0x55aaaa55)
		return 0;
	else
		return capacitylevel;

}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       :
*Note         : wait rb
*****************************************************************************/
__s32 nand_rb_wait_time_out(__u32 no, __u32 *flag)
{
	return 1;
}

__s32 nand_rb_wake_up(__u32 no)
{
	return 0;
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       :
*Note         : wait dma
*****************************************************************************/
__s32 nand_dma_wait_time_out(__u32 no, __u32 *flag)
{
	return 1;
}

__s32 nand_dma_wake_up(__u32 no)
{
	return 0;
}


/* int sunxi_get_securemode(void) */
/* return 0:normal mode */
/* rerurn 1:secure mode ,but could change clock */
/* return !0 && !1: secure mode ,and couldn't change clock */
int NAND_IS_Secure_sys(void)
{
	int mode = 0;
	int toc_file = (gd->bootfile_mode == SUNXI_BOOT_FILE_TOC);
	mode = sunxi_get_securemode() || toc_file;
	if (mode == 0)
		/* normal mode */
		return 0;
	else if (mode == 1)
		/* secure */
		return 1;
	return 0;
}

int NAND_IS_Secure_Chip(void)
{
	return sunxi_get_secureboard();
}

/* int NAND_IS_Burn_Mode(void) */
/* return 0:uboot is in boot mode */
/* rerurn 1:uboot is in [usb,card...]_product mode */
int NAND_IS_Burn_Mode(void)
{
	int mode = get_boot_work_mode();

	if (WORK_MODE_BOOT == mode)
		return 0;
	else
		return 1;
}

int NAND_GetVoltage(void)
{
	int ret = 0;
	return ret;
}

int NAND_ReleaseVoltage(void)
{
	int ret = 0;
	return ret;
}

void NAND_Print_Version(void)
{
	int val[4] = {0};

	val[0] = NAND_DRV_VERSION_0;
	val[1] = NAND_DRV_VERSION_1;
	val[2] = NAND_DRV_DATE;
	val[3] = NAND_DRV_TIME;
	NAND_Print("uboot: nand version: %x %x %x %x\n", val[0], val[1], val[2], val[3]);
}

int NAND_Get_Version(void)
{
	return NAND_DRV_DATE;
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       :
*Note         :
*****************************************************************************/

int nand_get_bsp_code(char *chip_code)
{
	/*unsigned int nand0_offset = 0;*/
	const char *pchip_code = NULL;

/*
 *        nand0_offset = fdt_path_offset(working_fdt, "nand0");
 *        if (nand0_offset == 0) {
 *                //PHY_ERR("nand0: get node offset error!\n");
 *                printf("nand0: get node offset error!\n");
 *                return -1;
 *        }
 *
 *        pchip_code = fdt_getprop(working_fdt, nand0_offset, "chip_code", NULL);
 *        if (pchip_code == NULL) {
 *                //PHY_ERR("nand0: get bsp_code error!\n");
 *                printf("nand0: get chip_code error!\n");
 *                return -1;
 *        }
 */

	pchip_code = nand_fdt_get_string("/soc/nand0", "chip_code");
	if (pchip_code == NULL) {
		printf("get chip_code err\n");
		return -1;
	}

	if (chip_code)
		memcpy(chip_code, pchip_code, strlen(pchip_code));

	return 0;
}
/**
*Name         : nand_get_support_boot_check_crc
*Description  :
*Parameter    :
*Return       :
*Note         :
**/

int nand_get_support_boot_check_crc(void)
{
	const char *boot_crc = NULL;

/*
 *	unsigned int nand0_offset = 0;
 *        nand0_offset = fdt_path_offset(working_fdt, "nand0");
 *        if (nand0_offset == 0) {
 *                //PHY_ERR("nand0: get node offset error!\n");
 *                printf("nand0: get node offset error!\n");
 *                return -1;
 *        }
 *
 *        boot_crc = fdt_getprop(working_fdt, nand0_offset, "boot_crc", NULL);
 *        if (boot_crc == NULL) {
 *                return 1; [>default enable<]
 *        }
 */

	boot_crc = nand_fdt_get_string("/soc/nand0", "boot_crc");
	if (boot_crc == NULL) {
		return 1; /*default enable*/
	}

	if (!memcmp(boot_crc, "disabled", sizeof("disabled")) ||
		!memcmp(boot_crc, "disable", sizeof("disable")))
		return 0;

	return 1;
}
#if defined(CONFIG_MACH_SUN8IW7)
//void NAND_GetVccq(void)
void nand_enable_vcc_3v(void)
{
	u32 reg_val = 0;

	reg_val = readl(PL_PUL0_REG);
	reg_val &= ~(3 << 6); //pl3
	reg_val |= (1 << 6);
	writel(reg_val, PL_PUL0_REG);

	reg_val = readl(PL_CF0_REG);
	reg_val &= ~(7 << 12); //pl3
	reg_val |= (1 << 12);
	writel(reg_val, PL_CF0_REG);

	reg_val = readl(PL_DAT_REG);
	reg_val |= (1 << 3); //pl3
	writel(reg_val, PL_DAT_REG);

	printf("PL_PUL0_REG = 0x%.8x, PL_CF0_REG = 0x%.8x, \
			PL_DAT_REG = 0x%.8x\n", readl(PL_PUL0_REG), \
				readl(PL_CF0_REG), readl(PL_DAT_REG));
}

void nand_enable_vccq_1p8v(void)
{
	u32 reg_val = 0;

	reg_val = readl(PL_PUL0_REG);
	reg_val &= ~(3 << 4); //pl2
	reg_val |= (1 << 4);
	writel(reg_val, PL_PUL0_REG);

	reg_val = readl(PL_CF0_REG);
	reg_val &= ~(7 << 8); //pl2
	reg_val |= (1 << 8);
	writel(reg_val, PL_CF0_REG);

	reg_val = readl(PL_DAT_REG);
	reg_val |= (1 << 2); //pl2
	writel(reg_val, PL_DAT_REG);
}
#else
#define NAND_SIGNAL_VOLTAGE_330	0
#define NAND_SIGNAL_VOLTAGE_180	1
void nand_enable_vccq_3p3v(void)
{
	u32 reg_val = 0;

	reg_val = readl((void *)(SUNXI_PIO_BASE + NAND_PIO_WITHSTAND_VOL_MODE));
	/*bit2: PC_POWER MODE SELECT*/
	reg_val &= (~(1 << 2));
	writel(reg_val, (void *)(SUNXI_PIO_BASE + NAND_PIO_WITHSTAND_VOL_MODE));
}

void nand_enable_vccq_1p8v(void)
{
	u32 reg_val = 0;

	reg_val = readl((void *)(SUNXI_PIO_BASE + NAND_PIO_WITHSTAND_VOL_MODE));
	/*bit2: PC_POWER MODE SELECT*/
	reg_val |= 0x04;
	writel(reg_val, (void *)(SUNXI_PIO_BASE + NAND_PIO_WITHSTAND_VOL_MODE));
}


#endif

/*record changes under common1*/
void nand_common1_show_version(void)
{
	static int flag;

	if (flag)
		return;

	printf("nand common1 version: %s\n", NAND_COMMON1_DRV_VERSION);
	flag = 1;
}
