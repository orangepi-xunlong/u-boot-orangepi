/*
 *  * (C) Copyright 2013-2016
 *   * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *    * weidonghui <weidonghui@allwinnertech.com>
 *     * SPDX-License-Identifier:     GPL-2.0+
 *      */
#ifndef __DTBO_IMG_H__
#define __DTBO_IMG_H__

#define DT_TABLE_MAGIC	0xd7b7ab1e

struct dt_table_header {
	uint32_t magic;             /*DT_TABLE_MAGIC*/
	uint32_t total_size;        /*includes dt_table_header + all dt_table_entry*/
								/*and all dtb/dtbo*/
	uint32_t header_size;       /*sizeof(dt_table_header)*/
	uint32_t dt_entry_size;     /*sizeof(dt_table_entry)*/
	uint32_t dt_entry_count;    /*number of dt_table_entry*/
	uint32_t dt_entries_offset; /*offset to the first dt_table_entry*/
								/*from head of dt_table_header*/
	uint32_t page_size;         /*flash page size we assume*/
	uint32_t version;			/*must be zero*/

};

struct dt_table_entry {
	uint32_t dt_size;
	uint32_t dt_offset;         /*offset from head of dt_table_header*/
	uint32_t id;                /*optional, must be zero if unused*/
	uint32_t rev;               /*optional, must be zero if unused*/
	uint32_t custom[4];			/*optional, must be zero if unused*/
};

//#define	UFDT_DEBUG
#define dto_error(fmt, args...)				pr_err("[ufdt]: "fmt, ##args)

#ifdef UFDT_DEBUG
#define dto_print(fmt, args...)				pr_msg("[ufdt]: "fmt, ##args)
#define dto_debug(fmt, args...)				pr_msg("[ufdt]: "fmt, ##args)
#else
#define dto_print(fmt, args...)				{}
#define dto_debug(fmt, args...)				{}
#endif

#define dt_table_header_get_header(dtboimg, field)\
	(fdt32_to_cpu(((const struct dt_table_header *)(dtboimg))->field))
#define dt_table_header_magic(dtboimg)			(dt_table_header_get_header(dtboimg, magic))
#define dt_table_header_total_size(dtboimg)			(dt_table_header_get_header(dtboimg, total_size))
#define dt_table_header_header_size(dtboimg)			(dt_table_header_get_header(dtboimg, header_size))
#define dt_table_header_dt_entry_size(dtboimg)			(dt_table_header_get_header(dtboimg, dt_entry_size))
#define dt_table_header_dt_entry_count(dtboimg)			(dt_table_header_get_header(dtboimg, dt_entry_count))
#define dt_table_header_dt_entries_offset(dtboimg)			(dt_table_header_get_header(dtboimg, dt_entries_offset))


#define dt_table_entry_get_header(dtboimg, field)\
	(fdt32_to_cpu(((const struct dt_table_entry *)(dtboimg))->field))
#define dt_table_entry_dt_size(dtboimg)			(dt_table_entry_get_header(dtboimg, dt_size))
#define dt_table_entry_dt_offset(dtboimg)			(dt_table_entry_get_header(dtboimg, dt_offset))
#define dt_table_entry_id(dtboimg)			        (dt_table_entry_get_header(dtboimg, id))
#define dt_table_entry_rev(dtboimg)			(dt_table_entry_get_header(dtboimg, rev))
#define dt_table_entry_custom(dtboimg, n)			(dt_table_entry_get_header(dtboimg, custom[n]))

#endif
