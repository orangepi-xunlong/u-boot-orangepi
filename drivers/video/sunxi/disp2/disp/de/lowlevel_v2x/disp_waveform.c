/*
 * drivers/video/sunxi/disp2/disp/de/lowlevel_v2x/disp_waveform.c
 *
 * Copyright (c) 2007-2019 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <zhengxiaobin@allwinnertech.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "disp_waveform.h"

#define DEBUG_WAVEFILE
#ifdef DEBUG_WAVEFILE
#define WF_DBG(msg, fmt...)		printf(msg, ##fmt)
#define WF_INFO(msg, fmt...)		printf(msg, ##fmt)
#define WF_WRN(msg, fmt...)		printf(msg, ##fmt)
#define WF_ERR(msg, fmt...)		printf(msg, ##fmt)
#else
#define WF_DBG(msg, fmt...)
#define WF_INFO(msg, fmt...)
#define WF_WRN(msg, fmt...)		printf(msg, ##fmt)
#define WF_ERR(msg, fmt...)		printf(msg, ##fmt)
#endif

#define    C_HEADER_INFO_OFFSET			0
#define    C_HEADER_TYPE_ID_OFFSET		0 /*eink type(eink id)*/
#define    C_HEADER_VERSION_STR_OFFSET		1
#define    C_HEADER_INFO_SIZE			128 /*size of awf file header*/
#define    C_TEMP_TBL_OFFSET  (C_HEADER_INFO_OFFSET+C_HEADER_INFO_SIZE)	/*temperature table offset from the beginning of the awf file*/
#define    C_TEMP_TBL_SIZE			32 /* temperature table size*/

#define    C_MODE_ADDR_TBL_OFFSET	(C_TEMP_TBL_OFFSET+C_TEMP_TBL_SIZE)	/*mode address table offset*/
#define    C_MODE_ADDR_TBL_SIZE			64

#define    C_INIT_MODE_ADDR_OFFSET			C_MODE_ADDR_TBL_OFFSET
#define    C_GC16_MODE_ADDR_OFFSET		(C_MODE_ADDR_TBL_OFFSET+4)
#define    C_GC4_MODE_ADDR_OFFSET			(C_MODE_ADDR_TBL_OFFSET+8)
#define    C_DU_MODE_ADDR_OFFSET			(C_MODE_ADDR_TBL_OFFSET+12)
#define    C_A2_MODE_ADDR_OFFSET			(C_MODE_ADDR_TBL_OFFSET+16)
#define    C_GC16_LOCAL_MODE_ADDR_OFFSET	(C_MODE_ADDR_TBL_OFFSET+20)
#define    C_GC4_LOCAL_MODE_ADDR_OFFSET	(C_MODE_ADDR_TBL_OFFSET+24)
#define    C_A2_IN_MODE_ADDR_OFFSET			(C_MODE_ADDR_TBL_OFFSET+28)
#define    C_A2_OUT_MODE_ADDR_OFFSET		(C_MODE_ADDR_TBL_OFFSET+32)

#define    C_INIT_MODE_OFFSET					(C_MODE_ADDR_TBL_OFFSET+C_MODE_ADDR_TBL_SIZE)

#define    C_REAL_TEMP_AREA_NUM			15      /*max temperature range number*/
#define    WF_MAX_COL				256		/* GC16, 16*16 = 256*/


typedef struct {
	u8 load_flag;			/*when awf has been loaded, init_flag = 1*/
	char *p_wf_vaddr;		/*virtual address of waveform file*/
	u32 p_wf_paddr;			/*phy address of waveform file*/
	EINK_PANEL_TYPE eink_panel_type;		/*eink type, example PVI, OED and etc*/
	u8 wf_temp_area_tbl[C_TEMP_TBL_SIZE];	/*temperature table*/

	/*phy address*/
	u32 p_init_wf;			/*init mode address (include temperature table address)*/
	u32 p_gc16_wf;		/*gc16 mode address (include temperature table address)*/
	u32 p_gc4_wf;		/*gc4 mode address (include temperature table address)*/
	u32 p_du_wf;		/*du mode address (include temperature table address)*/
	u32 p_A2_wf;		/*A2 mode address (include temperature table address)*/
	u32 p_gc16_local_wf;	/*gc16 local mode address (include temperature table address)*/
	u32 p_gc4_local_wf;	/*gc4 local mode address (include temperature table address)*/
	u32 p_A2_in_wf;		/*A2 in mode address (include temperature table address)*/
	u32 p_A2_out_wf;	/*A2 out mode address (include temperature table address)*/
} AWF_WAVEFILE;

/*__attribute__((section(".data")))*/
static AWF_WAVEFILE g_waveform_file;

static inline void *malloc_aligned(u32 size, u32 alignment)
{
     void *ptr = (void *)malloc(size + alignment);
      if (ptr) {
	    void *aligned = (void *)(((long)ptr + alignment) & (~(alignment - 1)));

	    /* Store the original pointer just before aligned pointer*/
	    ((void **) aligned)[-1] = ptr;
	return aligned;
      }

      return NULL;
}

static inline void free_aligned(void *aligned_ptr)
{
     if (aligned_ptr)
	free(((void **) aligned_ptr)[-1]);
}

/*
Description	: get temperature range index from temperature table, if temperature match
			  TBL[id] <= temperature < TBL[id + 1] condition, then temp range index = id
Input		: temperature -- temperature of eink panel
Output		: None
Return		: 0 & positive -- temperature range index, negative -- fail
*/
static __s32 get_temp_range_index(int temperature)
{
	__s32 index = -EINVAL;

	for (index = 0; index < C_TEMP_TBL_SIZE; index++) {
		if ((g_waveform_file.wf_temp_area_tbl[index] == 0) && (index > 0)) {
			break;
		}

		if (temperature < g_waveform_file.wf_temp_area_tbl[index]) {
			break;
		}
	}

	if (index > 0) {
		index -= 1;
	}

	return index;
}

/*
Description	: get mode address according to flush mode
Input		: mode -- flush mode
Output		: None
Return		: NULL -- get mode address fail, others -- get mode address successfully
*/
static u8 *get_mode_virt_address(enum eink_update_mode mode)
{
	u8 *p_wf_file = NULL;
	u32 offset = 0;

	switch (mode) {
	case EINK_INIT_MODE:
	{
		offset = (u32)g_waveform_file.p_init_wf - g_waveform_file.p_wf_paddr;
		p_wf_file = (u8 *)g_waveform_file.p_wf_vaddr + offset;
		break;
	}

	case EINK_DU_MODE:
	case EINK_DU_RECT_MODE:
	{
		offset = (u32)g_waveform_file.p_du_wf - g_waveform_file.p_wf_paddr;
		p_wf_file = (u8 *)g_waveform_file.p_wf_vaddr + offset;
		break;
	}

	case EINK_GC16_MODE:
	case EINK_GC16_RECT_MODE:
	{
		offset = (u32)g_waveform_file.p_gc16_wf - g_waveform_file.p_wf_paddr;
		p_wf_file = (u8 *)g_waveform_file.p_wf_vaddr + offset;
		break;
	}

	case EINK_A2_MODE:
	case EINK_A2_RECT_MODE:
	{
		offset = (u32)g_waveform_file.p_A2_wf - g_waveform_file.p_wf_paddr;
		p_wf_file = (u8 *)g_waveform_file.p_wf_vaddr + offset;
		break;
	}

	case EINK_GC16_LOCAL_MODE:
	case EINK_GC16_LOCAL_RECT_MODE:
	{
		offset = (u32)g_waveform_file.p_gc16_local_wf - g_waveform_file.p_wf_paddr;
		p_wf_file = (u8 *)g_waveform_file.p_wf_vaddr + offset;
		break;
	}

	default:
	{
		WF_WRN("unkown mode()\n");
		p_wf_file = NULL;
		break;
	}
	}

	return p_wf_file;
}

/*
Description	: get mode address according to flush mode
Input		: mode -- flush mode
Output		: None
Return		: NULL -- get mode address fail, others -- get mode address successfully
*/
static u32 get_mode_phy_address(enum eink_update_mode mode)
{
	u32 phy_addr = 0;

	switch (mode) {
	case EINK_INIT_MODE:
	{
		phy_addr = g_waveform_file.p_init_wf;
		break;
	}

	case EINK_DU_MODE:
	case EINK_DU_RECT_MODE:
	{
		phy_addr = g_waveform_file.p_du_wf;
		break;
	}

	case EINK_GC16_MODE:
	case EINK_GC16_RECT_MODE:
	{
		phy_addr = g_waveform_file.p_gc16_wf;
		break;
	}

	case EINK_A2_MODE:
	case EINK_A2_RECT_MODE:
	{
		phy_addr = g_waveform_file.p_A2_wf;
		break;
	}

	case EINK_GC16_LOCAL_MODE:
	case EINK_GC16_LOCAL_RECT_MODE:
	{
		phy_addr = g_waveform_file.p_gc16_local_wf;
		break;
	}

	default:
	{
		WF_WRN("unkown mode(0x%x)\n", mode);
		phy_addr = 0;
		break;
	}
	}

	return phy_addr;
}


/*
Description	: load waveform file to memory, and save each flush mode of address. This function
			  should be called before waveform data has been used.
Input		: path -- path of waveform file
Output		: None
Return		: 0 -- load waveform ok, others -- fail
*/

__s32 init_waveform(const char *path)
{
	char  waveform_path[32];
	char  waveform_addr[32] = {0};
	char *waveform_buff = NULL;

	u32 *pAddr = NULL;

	size_t waveform_buff_len = 2<<20;/*2M*/

	char *waveform_argv[6] = { "fatload", "sunxi_flash", "0:0", "00000000", waveform_path, NULL };

#ifdef CONFIG_GPT_SUPPORT
	waveform_argv[2] = "1:0";
#endif

	if (path == NULL) {
		WF_ERR("path is null\n");
		return -1;
	}

	memset(&g_waveform_file, 0, sizeof(g_waveform_file));

	g_waveform_file.p_wf_vaddr = (char *)malloc_aligned(waveform_buff_len, ARCH_DMA_MINALIGN);
	printf("g_waveform_file.p_wf_vaddr %lx\n", (ulong)g_waveform_file.p_wf_vaddr);

	if (g_waveform_file.p_wf_vaddr == NULL) {
		printf("sunxi bmp: alloc buffer for %s fail\n", path);
		return -1;
	}

	g_waveform_file.p_wf_paddr = (u32)g_waveform_file.p_wf_vaddr;

	/*set bmp decode addr is CONFIG_SYS_SDRAM_BASE*/
	sprintf(waveform_addr, "%lx", (ulong)g_waveform_file.p_wf_vaddr);
	waveform_argv[3] = waveform_addr;

	memset(waveform_path, 0, 32);
	strcpy(waveform_path, path);
	if (do_fat_fsload(0, 0, 5, waveform_argv)) {
		printf("sunxi bmp info error : unable to open logo file %s\n", waveform_argv[4]);
		free(waveform_buff);
		return -1;
	}

	g_waveform_file.eink_panel_type = *((u8 *)g_waveform_file.p_wf_vaddr);

	memcpy(g_waveform_file.wf_temp_area_tbl, (g_waveform_file.p_wf_vaddr+C_TEMP_TBL_OFFSET), C_TEMP_TBL_SIZE);

	pAddr = (u32 *)(g_waveform_file.p_wf_vaddr + C_INIT_MODE_ADDR_OFFSET);
	g_waveform_file.p_init_wf = (u32)(g_waveform_file.p_wf_paddr + *pAddr);

	pAddr = (u32 *)(g_waveform_file.p_wf_vaddr + C_GC16_MODE_ADDR_OFFSET);
	g_waveform_file.p_gc16_wf = (u32)(g_waveform_file.p_wf_paddr + *pAddr);

	pAddr = (u32 *)(g_waveform_file.p_wf_vaddr + C_GC4_MODE_ADDR_OFFSET);
	g_waveform_file.p_gc4_wf = (u32)(g_waveform_file.p_wf_paddr + *pAddr);

	pAddr = (u32 *)(g_waveform_file.p_wf_vaddr + C_DU_MODE_ADDR_OFFSET);
	g_waveform_file.p_du_wf = (u32)(g_waveform_file.p_wf_paddr + *pAddr);

	pAddr = (u32 *)(g_waveform_file.p_wf_vaddr + C_A2_MODE_ADDR_OFFSET);
	g_waveform_file.p_A2_wf = (u32)(g_waveform_file.p_wf_paddr + *pAddr);

	pAddr = (u32 *)(g_waveform_file.p_wf_vaddr + C_GC16_LOCAL_MODE_ADDR_OFFSET);
	g_waveform_file.p_gc16_local_wf = (u32)(g_waveform_file.p_wf_paddr + *pAddr);

	pAddr = (u32 *)(g_waveform_file.p_wf_vaddr + C_GC4_LOCAL_MODE_ADDR_OFFSET);
	g_waveform_file.p_gc4_local_wf = (u32)(g_waveform_file.p_wf_paddr + *pAddr);

	pAddr = (u32 *)(g_waveform_file.p_wf_vaddr + C_A2_IN_MODE_ADDR_OFFSET);
	g_waveform_file.p_A2_in_wf = (u32)(g_waveform_file.p_wf_paddr + *pAddr);

	pAddr = (u32 *)(g_waveform_file.p_wf_vaddr + C_A2_OUT_MODE_ADDR_OFFSET);
	g_waveform_file.p_A2_out_wf = (u32)(g_waveform_file.p_wf_paddr + *pAddr);

	g_waveform_file.load_flag = 1;

	return 0;
}

/*
Description: get eink panel type, for example, 3 stands for ED060XC3(4-bit), 4 stands for ED060XD4(5-bit)
Input: None
Output: type -- save the type of eink panel
Return: 0 -- get eink panel type successfully, others -- fail
*/
int get_eink_panel_type(EINK_PANEL_TYPE *type)
{
	if (g_waveform_file.load_flag != 1) {
		WF_ERR("waveform hasn't init yet, pls init first\n");
		return -EINVAL;
	}

	if (type == NULL) {
		WF_ERR("input param is null\n");
		return -EINVAL;
	}

	*type = g_waveform_file.eink_panel_type;
	return 0;
}

/*
Description: get bit number of eink panel, 4-bit stands for 16 grayscale, 5-bit stands for 32 grayscale
Input: None
Output: bit_num -- save the bit number of eink panel
Return: 0 -- get eink panel type successfully, others -- fail
*/
int get_eink_panel_bit_num(enum  eink_bit_num *bit_num)
{
	if (g_waveform_file.load_flag != 1) {
		WF_ERR("waveform hasn't init yet, pls init first\n");
		return -EINVAL;
	}

	if (bit_num == NULL) {
		WF_ERR("input param is null\n");
		return -EINVAL;
	}

	if (g_waveform_file.eink_panel_type == ED060XD4) {
		*bit_num = EINK_BIT_5;
	} else {
		*bit_num = EINK_BIT_4;
	}

	return 0;
}


/*
Description: get waveform data address according to mode and temperature
Input: None
Output: type -- save the type of eink panel
Return: 0 -- get eink panel type successfully, others -- fail
*/
int get_waveform_data(enum eink_update_mode mode, u32 temp, u32 *total_frames, u32 *wf_buf)
{
	u32 p_mode_phy_addr = 0;
	u8 *p_mode_virt_addr = NULL;
	u32 mode_temp_offset = 0;
	u8 *p_mode_temp_addr = NULL;
	u32 temp_range_id = 0;

	if (g_waveform_file.load_flag != 1) {
		WF_ERR("waveform hasn't init yet, pls init first\n");
		return -EINVAL;
	}

	if ((total_frames == NULL) || (wf_buf == NULL)) {
		WF_ERR("input param is null\n");
		return -EINVAL;
	}

	p_mode_virt_addr = get_mode_virt_address(mode);
	if (p_mode_virt_addr == NULL) {
		WF_ERR("get mode virturl address fail, mode=0x%x\n", mode);
		return -EINVAL;
	}

	p_mode_phy_addr = get_mode_phy_address(mode);
	if (p_mode_phy_addr == 0) {
		WF_ERR("get mode phy address fail, mode=0x%x\n", mode);
		return -EINVAL;
	}

	temp_range_id = get_temp_range_index(temp);
	if (temp_range_id < 0) {
		WF_ERR("get temp range index fail, temp=0x%x\n", temp);
		return -EINVAL;
	}

	mode_temp_offset =  *((u32 *)p_mode_virt_addr + temp_range_id);
	p_mode_temp_addr = (u8 *)p_mode_virt_addr + mode_temp_offset;

	*total_frames = *((u16 *)p_mode_temp_addr);
	mode_temp_offset = mode_temp_offset + 4; /*skip total frame(2 Byte) and dividor(2 Byte)*/

	*wf_buf = p_mode_phy_addr + mode_temp_offset;

	return 0;
}


/*
Description	: free memory that used by waveform file, after that, waveform data cannot
			  be used until init_waveform function has been called. This function should
			  be called when unload module.
Input		: None
Output		: None
Return		: None
*/
void free_waveform(void)
{
	if (g_waveform_file.p_wf_vaddr != NULL)   {
		free_aligned(g_waveform_file.p_wf_vaddr);
		g_waveform_file.p_wf_vaddr = NULL;
	}

	g_waveform_file.load_flag = 0;
	return;
}



