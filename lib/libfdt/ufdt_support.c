/*
 *  * (C) Copyright 2013-2016
 *   * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *    * weidonghui <weidonghui@allwinnertech.com>
 *     * SPDX-License-Identifier:     GPL-2.0+
 *      */

#include <common.h>
#include <malloc.h>
#include <dtbo_img.h>
#include <fdt_support.h>


#define DTBOIMG_BUF_SIZE	(1024 * 1024 * 2)
#define DTO_PARTION			("dtbo")
#define DTBO_COUNT			(10)
extern int do_sunxi_flash(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[]);
extern void sunxi_dump(void *addr, unsigned int size);
#if defined(CONFIG_SUNXI_MULITCORE_BOOT)
extern uint *p_spin_lock_heap;
#endif
extern int dtbo_idx[DTBO_COUNT];

int check_dtbo_idx(void)
{
	if (dtbo_idx[0] == 0xf5f5f5f5) {
		return -1;
	}

	return 0;
}

int check_dtbo(void *dtboimg_buf, int *dtbo_entry_offset)
{
    struct dt_table_header *dt_table_head = NULL;
    struct dt_table_entry  *dt_table_entry = NULL;
    u32 i = 0;
    u32 entry_count = 0;

	dt_table_head = (struct dt_table_header *) dtboimg_buf;
#if 0
    dto_error("dtboimg magic is:0x%x\n", dt_table_header_magic(dtboimg_buf));
    dto_error("dtboimg total size is:0x%x\n", dt_table_header_total_size(dtboimg_buf));
    dto_error("dtboimg header size:0x%x\n", dt_table_header_header_size(dtboimg_buf));
    dto_error("dtboimg entry size:0x%x\n", dt_table_header_dt_entry_size(dtboimg_buf));
    dto_error("dtboimg entry count:0x%x\n", dt_table_header_dt_entry_count(dtboimg_buf));
    dto_error("dtboimg entry offset:0x%x\n", dt_table_header_dt_entries_offset(dtboimg_buf));
#endif
	if (dt_table_header_magic(dtboimg_buf) != DT_TABLE_MAGIC) {
		dto_error("dtboimg magic is bad:0x%x\n", dt_table_header_magic(dtboimg_buf));
		return -1;
	}
	entry_count = dt_table_header_dt_entry_count(dtboimg_buf);
	if (entry_count == 0) {
		dto_error("dtboimg dt_entry_count is :0x%x\n", entry_count);
		return -2;
	}
	dtbo_entry_offset[0] = dt_table_header_dt_entries_offset(dtboimg_buf);
    for (i = 0; i < entry_count; i++) {
		dt_table_entry = (struct dt_table_entry *) ((char *)dt_table_head + dt_table_header_header_size(dtboimg_buf) + (dt_table_header_dt_entry_size(dtboimg_buf) * i));
#if 0
    dto_error("dt_table_entry_dt_size:0x%x\n", dt_table_entry_dt_size(dt_table_entry));
    dto_error("dt_table_entry_dt_offset:0x%x\n", dt_table_entry_dt_offset(dt_table_entry));
    dto_error("dt_table_entry_id:0x%x\n", dt_table_entry_id(dt_table_entry));
    dto_error("dt_table_entry_rev:0x%x\n", dt_table_entry_rev(dt_table_entry));
    dto_error("dt_table_entry_custom0:0x%x\n", dt_table_entry_custom(dt_table_entry, 0));
    dto_error("dt_table_entry_custom1:0x%x\n", dt_table_entry_custom(dt_table_entry, 1));
    dto_error("dt_table_entry_custom2:0x%x\n", dt_table_entry_custom(dt_table_entry, 2));
    dto_error("dt_table_entry_custom3:0x%x\n\n", dt_table_entry_custom(dt_table_entry, 3));
#endif
     dtbo_entry_offset[i] = dt_table_entry_dt_offset(dt_table_entry);
    }

    return entry_count;

}

int load_dtboimg(void *dtboimg_buf)
{
	char *argv[6];
	char  dtboimg_head[32];
	u32 dto_addr = (u32)dtboimg_buf;

	sprintf(dtboimg_head, "%x", (u32)dto_addr);
	argv[0] = "sunxi_flash";
	argv[1] = "read";
    argv[2] = dtboimg_head;
	argv[3] = DTO_PARTION;
	argv[4] = NULL;

	if (do_sunxi_flash(0, 0, 4, argv)) {
		dto_error("sunxi_flash read dto partion error:%s\n", argv[3]);
		return -1;
	}
    dto_debug("sunxi_flash read dto partion success\n");

    return 0;
}

void *sunxi_support_ufdt(void *dtb_base, u32 dtb_len)
{
	void *dtboimg_buf;
    int ret;
	int i = 0;
	int dt_entry_count;
	int offset;
	int dtbo_entry_offset[DTBO_COUNT] = {0};

	dtboimg_buf = (void *) memalign(CONFIG_SYS_CACHELINE_SIZE, DTBOIMG_BUF_SIZE);
    if (dtboimg_buf == NULL) {
		dto_error("malloc dtboimg_buf fail\n");
		return NULL;
    }
	ret = load_dtboimg(dtboimg_buf);
    if (ret < 0) {
		dto_error("load_dto fail\n");
		free(dtboimg_buf);
		return NULL;
    }
	memset((void *)dtbo_entry_offset, 0x0, sizeof(dtbo_entry_offset));
	dt_entry_count = check_dtbo(dtboimg_buf, dtbo_entry_offset);
	dto_debug("dt_entry_count= %d\n", dt_entry_count);
	if (dt_entry_count < 0) {
		dto_error("don't have match dtbo\n");
		free(dtboimg_buf);
		return NULL;
	}
	while (dtbo_idx[i] != 0xf5f5f5f5) {
		if (dtbo_idx[i] > dt_entry_count) {
			dto_error("androidboot.dtbo_idx is %d > dtboimg %d\n", dtbo_idx[i], dt_entry_count);
			return NULL;
		}
		offset = dtbo_entry_offset[dtbo_idx[i]];
		dto_debug("offset= 0x%x\n", offset);
		dto_debug("dtbo_idx_%d overlay_size=0x%x\n", dtbo_idx[i], fdt_totalsize(dtboimg_buf + offset));
		if (fdt_overlay_apply_verbose(dtb_base, (dtboimg_buf + offset)) < 0) {
			dto_error(" merge fdt fail\n");
			free(dtboimg_buf);
			return NULL;
		}
		dto_debug("dtbo_idx_%d merge sucess, size=0x%x \n", dtbo_idx[i], fdt_totalsize(dtb_base));
		i++;

	}
	return (void *)dtb_base;
}
