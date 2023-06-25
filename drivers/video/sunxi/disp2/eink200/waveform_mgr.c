/*
 * Allwinner SoCs eink driver.
 *
 * Copyright (C) 2019 Allwinner.
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */
#include "include/eink_sys_source.h"
#include "include/eink_driver.h"

#define    C_HEADER_INFO_OFFSET		0
#define    C_HEADER_TYPE_ID_OFFSET		0		//eink type(eink id)
#define    C_HEADER_VERSION_STR_OFFSET		1
#define    C_HEADER_INFO_SIZE			128		//size of awf file header
#define    C_TEMP_TBL_OFFSET (C_HEADER_INFO_OFFSET + C_HEADER_INFO_SIZE)	//temperature table offset from the beginning of the awf file
#define    C_TEMP_TBL_SIZE			32	// temperature table size

#define    C_MODE_ADDR_TBL_OFFSET	       (C_TEMP_TBL_OFFSET + C_TEMP_TBL_SIZE)//mode address table offset
#define    C_MODE_ADDR_TBL_SIZE		64

#define    C_INIT_MODE_ADDR_OFFSET		C_MODE_ADDR_TBL_OFFSET
#define    C_GC16_MODE_ADDR_OFFSET		(C_MODE_ADDR_TBL_OFFSET + 4)
#define    C_GC4_MODE_ADDR_OFFSET		(C_MODE_ADDR_TBL_OFFSET + 8)
#define    C_DU_MODE_ADDR_OFFSET		(C_MODE_ADDR_TBL_OFFSET + 12)
#define    C_A2_MODE_ADDR_OFFSET		(C_MODE_ADDR_TBL_OFFSET + 16)
#define    C_GC16_LOCAL_MODE_ADDR_OFFSET	(C_MODE_ADDR_TBL_OFFSET + 20)
#define    C_GC4_LOCAL_MODE_ADDR_OFFSET	(C_MODE_ADDR_TBL_OFFSET + 24)
#define    C_A2_IN_MODE_ADDR_OFFSET		(C_MODE_ADDR_TBL_OFFSET + 28)
#define    C_A2_OUT_MODE_ADDR_OFFSET		(C_MODE_ADDR_TBL_OFFSET + 32)
#define    C_GL16_MODE_ADDR_OFFSET		(C_MODE_ADDR_TBL_OFFSET + 36)
#define    C_GLR16_MODE_ADDR_OFFSET             (C_MODE_ADDR_TBL_OFFSET + 40)
#define    C_GLD16_MODE_ADDR_OFFSET             (C_MODE_ADDR_TBL_OFFSET + 44)

#define    C_INIT_MODE_OFFSET			(C_MODE_ADDR_TBL_OFFSET + C_MODE_ADDR_TBL_SIZE)

#define    C_REAL_TEMP_AREA_NUM		15              //max temperature range number
#define    WF_MAX_COL				256		// GC16, 16*16 = 256

#define MAX_MODE_CNT	6
#define MAX_TEMP_CNT	32

typedef enum  {
	ED060SC4 = 0x01,
	ED060SC7 = 0x02,
	OPM060A1 = 0x03,
	ED060XD4 = 0x04
} EINK_PANEL_TYPE;

typedef struct {
	u8 load_flag;			//when awf has been loaded, init_flag = 1
	char *p_wf_vaddr;		//virtual address of waveform file
	unsigned long p_wf_paddr;			//phy address of waveform file
	char *rearray_vaddr;		//rearray address of waveform file
	unsigned long rearray_paddr;		//rearray phy address of waveform file
	EINK_PANEL_TYPE eink_panel_type;//eink type, example PVI, OED and etc
	char wavefile_name[128];
	u8 wf_temp_area_tbl[C_TEMP_TBL_SIZE];//temperature table

	/*phy address*/
	unsigned long p_init_wf;			//init mode address (include temperature table address)
	unsigned long p_gc16_wf;		//gc16 mode address (include temperature table address)
	unsigned long p_gc4_wf;		//gc4 mode address (include temperature table address)
	unsigned long p_du_wf;		//du mode address (include temperature table address)
	unsigned long p_A2_wf;		//A2 mode address (include temperature table address)
	unsigned long p_gc16_local_wf;		//gc16 local mode address (include temperature table address)
	unsigned long p_gc4_local_wf;	//gc4 local mode address (include temperature table address)
	unsigned long p_A2_in_wf;		//A2 in mode address (include temperature table address)
	unsigned long p_A2_out_wf;		//A2 out mode address (include temperature table address)
	unsigned long p_gl16_wf;		//GL16 mode address (include temperature table address)
	unsigned long p_glr16_wf;		//GLR16 mode address (include temperature table address)
	unsigned long p_gld16_wf;		//GLD16 mode address (include temperature table address)
} AWF_WAVEFILE;

static AWF_WAVEFILE g_waveform_file;
static unsigned long wf_vaddr_array[MAX_MODE_CNT][MAX_TEMP_CNT];
static unsigned long wf_paddr_array[MAX_MODE_CNT][MAX_TEMP_CNT];
static u32 tframes_array[MAX_MODE_CNT][MAX_TEMP_CNT];

/*
*Description	: get temperature range index from temperature table,
*		  if temperature match TBL[id] <= temperature < TBL[id + 1]
*		  condition, then temp range index = id
*Input		: temperature -- temperature of eink panel
*Output		: None
*Return		: 0 & positive -- temperature range index, negative -- fail
*/
static __s32 get_temp_range_index(int temperature)
{
	__s32 index = -EINVAL;

	for (index = 0; index < C_TEMP_TBL_SIZE; index++) {
		if (((g_waveform_file.wf_temp_area_tbl[index] == 0)) && (index > 0)) {
			break;
		}

		if (temperature < g_waveform_file.wf_temp_area_tbl[index]) {
			if (index > 0) {
				index -= 1;
			}
			break;
		}
		if ((temperature == g_waveform_file.wf_temp_area_tbl[index]) && (temperature > 0)) {
			return index;
		}
	}

	return index;
}

int get_index_from_upd_mode(enum upd_mode upd_mode)
{
	int index = 0;
	u32 mode;

	mode = upd_mode & 0xff;
	EINK_INFO_MSG("mode = %d, upd_mode = 0x%x\n", mode, upd_mode);
	switch (mode) {
	case EINK_INIT_MODE:
		index = 0;
		break;
	case EINK_DU_MODE:
		index = 1;
		break;
	case EINK_GC16_MODE:
		index = 2;
		break;
	case EINK_GC4_MODE:
		index = 3;
		break;
	case EINK_A2_MODE:
		index = 4;
		break;
	case EINK_GU16_MODE:
		index = 5;
		break;
	case EINK_GLR16_MODE:
		index = 6;
		break;
	case EINK_GLD16_MODE:
		index = 7;
		break;
	case EINK_GL16_MODE:
		index = 8;
		break;
	default:
		index = -1;
		break;
	}
	return index;
}

int get_upd_mode_from_index(u32 mode)
{
	enum upd_mode upd_mode = EINK_GC16_MODE;

	switch (mode) {
	case 0:
		upd_mode = EINK_INIT_MODE;
		break;
	case 1:
		upd_mode = EINK_DU_MODE;
		break;
	case 2:
		upd_mode = EINK_GC16_MODE;
		break;
	case 3:
		upd_mode = EINK_GC4_MODE;
		break;
	case 4:
		upd_mode = EINK_A2_MODE;
		break;
	case 5:
		upd_mode = EINK_GU16_MODE;
		break;
	case 6:
		upd_mode = EINK_GLR16_MODE;
		break;
	case 7:
		upd_mode = EINK_GLD16_MODE;
		break;
	case 8:
		upd_mode = EINK_GL16_MODE;
		break;
	default:
		pr_err("%s:mode is err\n", __func__);
		break;
	}
	return upd_mode;
}

/*
Description	: get mode address according to flush mode
Input		: mode -- flush mode
Output		: None
Return		: NULL -- get mode address fail, others -- get mode address successfully
*/
static u8 *get_mode_virt_address(enum upd_mode mode)
{
	u8 *p_wf_file = NULL;
	u32 offset = 0;

	switch (mode & 0xff) {
	case EINK_INIT_MODE:
		{
			offset = g_waveform_file.p_init_wf - g_waveform_file.p_wf_paddr;
			EINK_DEFAULT_MSG("offset = 0x%x\n", offset);
			p_wf_file = (u8 *)g_waveform_file.p_wf_vaddr + offset;
			break;
		}

	case EINK_DU_MODE:
		{
			offset = g_waveform_file.p_du_wf - g_waveform_file.p_wf_paddr;
			EINK_DEFAULT_MSG("offset = 0x%x\n", offset);
			p_wf_file = (u8 *)g_waveform_file.p_wf_vaddr + offset;
			break;
		}

	case EINK_GC16_MODE:
		{
			offset = g_waveform_file.p_gc16_wf - g_waveform_file.p_wf_paddr;
			p_wf_file = (u8 *)g_waveform_file.p_wf_vaddr + offset;
			break;
		}

	case EINK_GC4_MODE:
		{
			offset = g_waveform_file.p_gc4_wf - g_waveform_file.p_wf_paddr;
			p_wf_file = (u8 *)g_waveform_file.p_wf_vaddr + offset;
			break;
		}

	case EINK_A2_MODE:
		{
			offset = g_waveform_file.p_A2_wf - g_waveform_file.p_wf_paddr;
			p_wf_file = (u8 *)g_waveform_file.p_wf_vaddr + offset;
			break;
		}

	case EINK_GU16_MODE:
		{
			offset = g_waveform_file.p_gc16_local_wf - g_waveform_file.p_wf_paddr;
			p_wf_file = (u8 *)g_waveform_file.p_wf_vaddr + offset;
			break;
		}
/*
	case EINK_GL16_MODE:
		{
			offset = (u32)g_waveform_file.p_gl16_wf - g_waveform_file.p_wf_paddr;
			p_wf_file = (u8 *)g_waveform_file.p_wf_vaddr + offset;
			break;
		}

	case EINK_GLR16_MODE:
		{
			offset = (u32)g_waveform_file.p_glr16_wf - g_waveform_file.p_wf_paddr;
			pr_info("glr16 offset = 0x%x\n", offset);
			p_wf_file = (u8 *)g_waveform_file.p_wf_vaddr + offset;
			break;
		}

	case EINK_GLD16_MODE:
		{
			offset = (u32)g_waveform_file.p_gld16_wf - g_waveform_file.p_wf_paddr;
			p_wf_file = (u8 *)g_waveform_file.p_wf_vaddr + offset;
			break;
		}
*/
	default:
		{
			pr_err("unknown mode(0x%x)\n", mode);
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
static unsigned long get_mode_phy_address(enum upd_mode mode)
{
	unsigned long phy_addr = 0;

	switch (mode & 0xff) {
	case EINK_INIT_MODE:
		{
			phy_addr = g_waveform_file.p_init_wf;
			break;
		}

	case EINK_DU_MODE:
		{
			phy_addr = g_waveform_file.p_du_wf;
			break;
		}

	case EINK_GC16_MODE:
		{
			phy_addr = g_waveform_file.p_gc16_wf;
			break;
		}

	case EINK_GC4_MODE:
		{
			phy_addr = g_waveform_file.p_gc4_wf;
			break;
		}

	case EINK_A2_MODE:
		{
			phy_addr = g_waveform_file.p_A2_wf;
			break;
		}

	case EINK_GU16_MODE:
		{
			phy_addr = g_waveform_file.p_gc16_local_wf;
			break;
		}
/*
	case EINK_GL16_MODE:
		{
			phy_addr = g_waveform_file.p_gl16_wf;
			break;
		}

	case EINK_GLR16_MODE:
		{
			phy_addr = g_waveform_file.p_glr16_wf;
			break;
		}

	case EINK_GLD16_MODE:
		{
			phy_addr = g_waveform_file.p_gld16_wf;
			break;
		}
*/
	default:
		{
			pr_err("unknown mode(0x%x)\n", mode);
			phy_addr = 0;
			break;
		}
	}

	return phy_addr;
}

/*
Description: get waveform data address according to mode and temperature
Input: None
Output: type -- save the type of eink panel
Return: 0 -- get eink panel type successfully, others -- fail
*/
int get_waveform_data(enum upd_mode mode, u32 temp, u32 *total_frames, unsigned long *wf_paddr, unsigned long *wf_vaddr)
{
	unsigned long p_mode_phy_addr = 0;
	u8 *p_mode_virt_addr = NULL;
	u32 mode_temp_offset = 0;
	u8 *p_mode_temp_addr = NULL;
	//u16 *p_pointer = NULL;
	u32 temp_range_id = 0;

	if ((!total_frames) || (!wf_paddr) || (!wf_vaddr)) {
		pr_err("input param is null\n");
		return -EINVAL;
	}

	p_mode_virt_addr = get_mode_virt_address(mode);
	if (!p_mode_virt_addr) {
		pr_err("get mode virturl address fail, mode=0x%x\n", mode);
		return -EINVAL;
	}

	p_mode_phy_addr = get_mode_phy_address(mode);
	if (p_mode_phy_addr == 0) {
		pr_err("get mode phy address fail, mode=0x%x\n", mode);
		return -EINVAL;
	}

	temp_range_id = get_temp_range_index(temp);
	if (temp_range_id < 0) {
		pr_err("get temp range index fail, temp=0x%x\n", temp);
		return -EINVAL;
	}

	mode_temp_offset =  *((u32 *)p_mode_virt_addr + temp_range_id);

	p_mode_temp_addr = (u8 *)p_mode_virt_addr + mode_temp_offset;

	*total_frames = *((u16 *)p_mode_temp_addr);
	mode_temp_offset = mode_temp_offset + 16; //total frame(2 Byte) and dividor(2 Byte) wav paddr must 16byte align

	*wf_paddr = p_mode_phy_addr + mode_temp_offset;
	*wf_vaddr = (unsigned long)(p_mode_virt_addr + mode_temp_offset);

	EINK_DEFAULT_MSG("mode=0x%x, temp=%d, temp_id=%d, temp_offset=0x%x, total=%d, mode_offset=0x%x\n",\
		mode, temp, temp_range_id, (mode_temp_offset - 4), *total_frames, (unsigned int)(*wf_paddr - g_waveform_file.p_wf_paddr));

	return 0;
}

/*
 *re array wav because 8bit data only the low 2bit valid, close them.
 */

int eink_set_rearray_wavedata(u32 bit_num)
{
	int ret = 0, index = 0, temp = 0, mode = 0;
	enum upd_mode upd_mode;
	u32 total_frames = 0, frame_size = 0, per_size = 0, i = 0;
	unsigned long wf_vaddr = 0;
	unsigned long wf_paddr = 0;

	char *vaddr = NULL;
	unsigned long paddr = 0;

	if (bit_num == 5)
		per_size = 1024;
	else
		per_size = 256;

	vaddr = g_waveform_file.rearray_vaddr;
	paddr = g_waveform_file.rearray_paddr;

	for (mode = 0; mode < MAX_MODE_CNT; mode++) {
		for (index = 0; index < C_TEMP_TBL_SIZE; index++) {
			tframes_array[mode][index] = total_frames;
			wf_vaddr_array[mode][index] = (unsigned long)vaddr;
			wf_paddr_array[mode][index] = (unsigned long)paddr;

			temp = g_waveform_file.wf_temp_area_tbl[index];
			EINK_DEFAULT_MSG("temp = %d, index = %d\n", temp, index);

			upd_mode = get_upd_mode_from_index(mode);
			get_waveform_data(upd_mode, temp, &total_frames, &wf_paddr, &wf_vaddr);
			/* rearray */
			frame_size = total_frames * per_size;/* 4bit panel 256 5bit panel 1024 */
			for (i = 0; i < frame_size / 4; i++) {
				*vaddr = (*((char *)wf_vaddr) & 0x3) |
					((*((char *)wf_vaddr + 1) & 0x3) << 2) |
					((*((char *)wf_vaddr + 2) & 0x3) << 4) |
					((*((char *)wf_vaddr + 3) & 0x3) << 6);
				vaddr++;
				wf_vaddr += 4;
			}
			/*
			   for (i = 0, j = 0; j < frame_size; i++, j += 4) {
			   g_waveform_file.rearray_vaddr[i] = (wf_vaddr[j] & 0x3) |
			   ((wf_vaddr[j + 1] & 0x3) << 2) |
			   ((wf_vaddr[j + 2] & 0x3) << 4) |
			   ((wf_vaddr[j + 3] & 0x3) << 6);
			   }
			   */

			EINK_DEFAULT_MSG("rearray ([%d, %d]) vaddr = 0x%lx, paddr = 0x%lx\n",
					 mode, index,
					wf_vaddr_array[mode][index],
					wf_paddr_array[mode][index]);

		/*	g_waveform_file.rearray_vaddr += (frame_size / 4); */
			paddr += (frame_size / 4);
		}
	}
#ifdef WAVEDATA_DEBUG
	EINK_INFO_MSG("rearray_vaddr = 0x%p, paddr = 0x%lx\n", g_waveform_file.rearray_vaddr, g_waveform_file.rearray_paddr);
	//save_rearray_waveform_to_mem(g_waveform_file.rearray_vaddr, file_len);
#endif
	return ret;
}

unsigned int memory_addr_to_wavefile_addr(unsigned int paddr)
{
	return (unsigned int)(paddr - g_waveform_file.p_wf_paddr);
}

static void print_wavefile_mode_mapping(AWF_WAVEFILE wf)
{
	EINK_DEFAULT_MSG("INIT mode wavefile offset = 0x%08x\n", (unsigned int)(wf.p_init_wf - wf.p_wf_paddr));
	EINK_DEFAULT_MSG("GC16 mode wavefile offset = 0x%08x\n", (unsigned int)(wf.p_gc16_wf - wf.p_wf_paddr));
	EINK_DEFAULT_MSG("GC4 mode wavefile offset = 0x%08x\n", (unsigned int)(wf.p_gc4_wf - wf.p_wf_paddr));
	EINK_DEFAULT_MSG("DU mode wavefile offset = 0x%08x\n", (unsigned int)(wf.p_du_wf - wf.p_wf_paddr));
	EINK_DEFAULT_MSG("A2 mode wavefile offset = 0x%08x\n", (unsigned int)(wf.p_A2_wf - wf.p_wf_paddr));
	EINK_DEFAULT_MSG("GC16_LOCAL mode wavefile offset = 0x%08x\n", (unsigned int)(wf.p_gc16_local_wf - wf.p_wf_paddr));
	EINK_DEFAULT_MSG("GC4_LOCAL mode wavefile offset = 0x%08x\n", (unsigned int)(wf.p_gc4_local_wf - wf.p_wf_paddr));
	EINK_DEFAULT_MSG("A2_IN mode wavefile offset = 0x%08x\n", (unsigned int)(wf.p_A2_in_wf - wf.p_wf_paddr));
	EINK_DEFAULT_MSG("A2_OUT mode wavefile offset = 0x%08x\n", (unsigned int)(wf.p_A2_out_wf - wf.p_wf_paddr));
	EINK_DEFAULT_MSG("GL16 mode wavefile offset = 0x%08x\n", (unsigned int)(wf.p_gl16_wf - wf.p_wf_paddr));
	EINK_DEFAULT_MSG("GLR16 mode wavefile offset = 0x%08x\n", (unsigned int)(wf.p_glr16_wf - wf.p_wf_paddr));
	EINK_DEFAULT_MSG("GLD16 mode wavefile offset = 0x%08x\n", (unsigned int)(wf.p_gld16_wf - wf.p_wf_paddr));
}

int init_waveform(const char *path, u32 bit_num)
{
	char  waveform_path[32];
	char  waveform_addr[32] = {0};
	char *waveform_buff = NULL;
	int partno = -1;
	int ret = 0;

	u32 *pAddr = NULL;

	if (!path) {
		pr_err("path is null\n");
		return -1;
	}

	size_t waveform_buff_len = 10 << 20;/* 10M */
	char *waveform_argv[6] = { "fatload", "sunxi_flash", "0:0", "00000000", waveform_path, NULL };

	partno = sunxi_partition_get_partno_byname("bootloader");
	if (partno < 0) {
		partno = sunxi_partition_get_partno_byname("boot-resource");
		if (partno < 0) {
			printf("[%s]:Get bootloader or boot-resource partition number fail!\n", __func__);
			return -1;
		}
	}

	EINK_INFO_MSG("partno = %d\n", partno);
	sprintf(waveform_argv[2], "0:%x", partno);

	memset(&g_waveform_file, 0, sizeof(g_waveform_file));
	EINK_INFO_MSG("starting to load awf waveform file(%s)\n", path);
	g_waveform_file.p_wf_vaddr = (char *)malloc_aligned(waveform_buff_len, ARCH_DMA_MINALIGN);

	if (!g_waveform_file.p_wf_vaddr) {
		pr_err("%s:fail to alloc memory for waveform file\n", __func__);
		ret = -ENOMEM;
		goto error;
	}
	EINK_INFO_MSG("g_waveform_file.p_wf_vaddr %lx\n", (ulong)g_waveform_file.p_wf_vaddr);

	g_waveform_file.p_wf_paddr = (unsigned long)g_waveform_file.p_wf_vaddr;

#ifdef DRIVER_REMAP_WAVEFILE
	g_waveform_file.rearray_vaddr = (char *)malloc_aligned(waveform_buff_len, ARCH_DMA_MINALIGN);
	EINK_INFO_MSG("rearray_vaddr = 0x%p\n", g_waveform_file.rearray_vaddr);
	if (!g_waveform_file.rearray_vaddr) {
		pr_err("%s:fail to alloc mem for rearray waveform file\n", __func__);
		ret = -ENOMEM;
		goto error;
	}
	g_waveform_file.rearray_paddr = (unsigned long)g_waveform_file.rearray_vaddr;

	memset(g_waveform_file.rearray_vaddr, 0, waveform_buff_len);
#endif
	/*set wav decode addr is CONFIG_SYS_SDRAM_BASE*/
	sprintf(waveform_addr, "%lx", (ulong)g_waveform_file.p_wf_vaddr);
	waveform_argv[3] = waveform_addr;

	memset(waveform_path, 0, 32);
	strcpy(waveform_path, path);

	if (do_fat_fsload(0, 0, 5, waveform_argv)) {
		printf("sunxi loat wavfile error : unable to open wavfile %s\n", waveform_argv[4]);
		free(waveform_buff);
		return -1;
	}

	g_waveform_file.eink_panel_type = *((u8 *)g_waveform_file.p_wf_vaddr);
	EINK_INFO_MSG("eink type=0x%x\n", g_waveform_file.eink_panel_type);

	memset(g_waveform_file.wavefile_name, 0, sizeof(g_waveform_file.wavefile_name));
	memcpy(g_waveform_file.wavefile_name, (g_waveform_file.p_wf_vaddr + 1), (sizeof(g_waveform_file.wavefile_name) - 1));
	EINK_INFO_MSG("wavefile info: %s\n", g_waveform_file.wavefile_name);

	/* starting to load data */
	memcpy(g_waveform_file.wf_temp_area_tbl, (g_waveform_file.p_wf_vaddr + C_TEMP_TBL_OFFSET), C_TEMP_TBL_SIZE);

	pAddr = (u32 *)(g_waveform_file.p_wf_vaddr + C_INIT_MODE_ADDR_OFFSET);
	g_waveform_file.p_init_wf = (unsigned long)(g_waveform_file.p_wf_paddr + *pAddr);//unsigned long吧？

	pAddr = (u32 *)(g_waveform_file.p_wf_vaddr + C_GC16_MODE_ADDR_OFFSET);
	g_waveform_file.p_gc16_wf = (unsigned long)(g_waveform_file.p_wf_paddr + *pAddr);

	pAddr = (u32 *)(g_waveform_file.p_wf_vaddr + C_GC4_MODE_ADDR_OFFSET);
	g_waveform_file.p_gc4_wf = (unsigned long)(g_waveform_file.p_wf_paddr + *pAddr);

	pAddr = (u32 *)(g_waveform_file.p_wf_vaddr + C_DU_MODE_ADDR_OFFSET);
	g_waveform_file.p_du_wf = (unsigned long)(g_waveform_file.p_wf_paddr + *pAddr);

	pAddr = (u32 *)(g_waveform_file.p_wf_vaddr + C_A2_MODE_ADDR_OFFSET);
	g_waveform_file.p_A2_wf = (unsigned long)(g_waveform_file.p_wf_paddr + *pAddr);

	pAddr = (u32 *)(g_waveform_file.p_wf_vaddr + C_GC16_LOCAL_MODE_ADDR_OFFSET);
	g_waveform_file.p_gc16_local_wf = (unsigned long)(g_waveform_file.p_wf_paddr + *pAddr);

	pAddr = (u32 *)(g_waveform_file.p_wf_vaddr + C_GC4_LOCAL_MODE_ADDR_OFFSET);
	g_waveform_file.p_gc4_local_wf = (unsigned long)(g_waveform_file.p_wf_paddr + *pAddr);

	pAddr = (u32 *)(g_waveform_file.p_wf_vaddr + C_A2_IN_MODE_ADDR_OFFSET);
	g_waveform_file.p_A2_in_wf = (unsigned long)(g_waveform_file.p_wf_paddr + *pAddr);

	pAddr = (u32 *)(g_waveform_file.p_wf_vaddr + C_A2_OUT_MODE_ADDR_OFFSET);
	g_waveform_file.p_A2_out_wf = (unsigned long)(g_waveform_file.p_wf_paddr + *pAddr);

	pAddr = (u32 *)(g_waveform_file.p_wf_vaddr + C_GL16_MODE_ADDR_OFFSET);
	g_waveform_file.p_gl16_wf = (unsigned long)(g_waveform_file.p_wf_paddr + *pAddr);

	pAddr = (u32 *)(g_waveform_file.p_wf_vaddr + C_GLR16_MODE_ADDR_OFFSET);
	g_waveform_file.p_glr16_wf = (unsigned long)(g_waveform_file.p_wf_paddr + *pAddr);

	pAddr = (u32 *)(g_waveform_file.p_wf_vaddr + C_GLD16_MODE_ADDR_OFFSET);
	g_waveform_file.p_gld16_wf = (unsigned long)(g_waveform_file.p_wf_paddr + *pAddr);

	print_wavefile_mode_mapping(g_waveform_file);
 #ifdef DRIVER_REMAP_WAVEFILE
	eink_set_rearray_wavedata(bit_num);
#endif
	g_waveform_file.load_flag = 1;

	pr_info("load waveform file(%s) successfully\n", path);
	return 0;

error:
	g_waveform_file.load_flag = 0;
#ifdef DRIVER_REMAP_WAVEFILE
	if (g_waveform_file.rearray_vaddr)   {
		free_aligned(g_waveform_file.rearray_vaddr);
		g_waveform_file.rearray_vaddr = NULL;
	}
#endif
	if (g_waveform_file.p_wf_vaddr)   {
		free_aligned(g_waveform_file.p_wf_vaddr);
		g_waveform_file.p_wf_vaddr = NULL;
	}

	return ret;
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
	if (g_waveform_file.rearray_vaddr)   {
		free_aligned(g_waveform_file.rearray_vaddr);
		g_waveform_file.rearray_vaddr = NULL;
	}

	if (g_waveform_file.p_wf_vaddr)   {
		free_aligned(g_waveform_file.p_wf_vaddr);
		g_waveform_file.p_wf_vaddr = NULL;
	}

	g_waveform_file.load_flag = 0;
	return;
}

int eink_get_wf_data(enum upd_mode mode, u32 temp, u32 *total_frames, unsigned long *wf_paddr, unsigned long *wf_vaddr)
{
	u32 index = 0;
	s32 temp_range_id = 0;

	if (g_waveform_file.load_flag != 1) {
		pr_err("waveform hasn't init yet, pls init first\n");
		return -EAGAIN;
	}

	if ((!total_frames) || (!wf_paddr) || (!wf_vaddr)) {
		pr_err("input param is null\n");
		return -EINVAL;
	}

	temp_range_id = get_temp_range_index(temp);
	if (temp_range_id < 0) {
		pr_err("%s:get temp range index fail, temp=0x%x\n", __func__, temp);
		return -EINVAL;
	}

	index = get_index_from_upd_mode(mode);
	if (index < 0) {
		pr_err("%s:index not invalid", __func__);
		return -EINVAL;
	}

	EINK_INFO_MSG("temp = %d, range_id = %d, index = %d\n",
		      temp, temp_range_id, index);
	*total_frames = tframes_array[index][temp_range_id];
	*wf_paddr = wf_paddr_array[index][temp_range_id];
	*wf_vaddr = wf_vaddr_array[index][temp_range_id];

	return 0;
}

int waveform_mgr_init(const char *path, u32 bit_num)
{
	int ret =  0;

	ret = init_waveform(path, bit_num);
	if (ret) {
		pr_err("fail to load setting wavefile(%s),try default wavefile(%s)\n", path, DEFAULT_WAVEFORM_PATH);
		ret = init_waveform(DEFAULT_WAVEFORM_PATH, bit_num);
		if (ret) {
			pr_err("both wavefile(%s) and default wavefile(%s) cannot found\n", path, DEFAULT_WAVEFORM_PATH);
			free_waveform();
		}
		return ret;
	}

	return ret;
}
