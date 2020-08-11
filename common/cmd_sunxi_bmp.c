/*
 * (C) Copyright 2002
 * Detlev Zundel, DENX Software Engineering, dzu@denx.de.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * BMP handling routines
 */

#include <common.h>
#include <bmp_layout.h>
#include <command.h>
#include <malloc.h>
#include <sunxi_bmp.h>
#include <sunxi_board.h>
#include <sunxi_advert.h>
#include <sys_partition.h>
#include <fdt_support.h>

extern int sunxi_partition_get_partno_byname(const char *part_name);
static int sunxi_bmp_probe_info (uint addr);
static int sunxi_bmp_show(sunxi_bmp_store_t bmp_info);
int sunxi_advert_display(char *fatname, char *filename);
int sunxi_advert_logo_load(char *fatname, char *filename);


DECLARE_GLOBAL_DATA_PTR;

/*
 * Allocate and decompress a BMP image using gunzip().
 *
 * Returns a pointer to the decompressed image data. Must be freed by
 * the caller after use.
 *
 * Returns NULL if decompression failed, or if the decompressed data
 * didn't contain a valid BMP signature.
 */

static int do_sunxi_bmp_info(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	uint addr;

	if(argc == 2)
	{
		/* use argument only*/
		addr = simple_strtoul(argv[1], NULL, 16);
		debug("bmp addr=%x\n", addr);
	}
	else if(argc == 3)
	{
		char  load_addr[8];
		char  filename[32];
		char *const bmp_argv[6] = { "fatload", "sunxi_flash", "0:0", load_addr, filename, NULL };

		addr = simple_strtoul(argv[1], NULL, 16);
		memcpy(load_addr, argv[1], 8);
		memset(filename, 0, 32);
		memcpy(filename, argv[2], strlen(argv[2]));
#ifdef DEBUG
	    int i;

		for(i=0;i<6;i++)
		{
	        printf("argv[%d] = %s\n", i, argv[i]);
		}
#endif
	    if(do_fat_fsload(0, 0, 5, bmp_argv))
		{
		   printf("sunxi bmp info error : unable to open bmp file %s\n", argv[2]);

		   return cmd_usage(cmdtp);
	    }
	}
	else
	{
		return cmd_usage(cmdtp);
	}

	return (sunxi_bmp_probe_info(addr));
}

U_BOOT_CMD(
	sunxi_bmp_info,	3,	1,	do_sunxi_bmp_info,
	"manipulate BMP image data",
	"only one para : the address where the bmp stored\n"
);


static int do_sunxi_bmp_display(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	uint addr;
	uint de_addr;
	sunxi_bmp_store_t bmp_info;

	if(argc == 2)
	{
		/* use argument only*/
		addr = simple_strtoul(argv[1], NULL, 16);
#if defined(CONFIG_SUNXI_LOGBUFFER)
		de_addr = CONFIG_SYS_SDRAM_BASE + gd->ram_size - SUNXI_DISPLAY_FRAME_BUFFER_SIZE;
#else
		de_addr = SUNXI_DISPLAY_FRAME_BUFFER_ADDR;
#endif
	}
	else if(argc == 3)
	{
		addr = simple_strtoul(argv[1], NULL, 16);
		de_addr = simple_strtoul(argv[2], NULL, 16);
	}
	else if(argc == 4)
	{
		char  load_addr[8];
		char  filename[32];
		char *const bmp_argv[6] = { "fatload", "sunxi_flash", "0:0", load_addr, filename, NULL };

		addr = simple_strtoul(argv[1], NULL, 16);
		memcpy(load_addr, argv[1], 8);
		memset(filename, 0, 32);
		memcpy(filename, argv[3], strlen(argv[3]));
		de_addr = simple_strtoul(argv[2], NULL, 16);
#ifdef DEBUG
	    int i;

		for(i=0;i<6;i++)
		{
	        printf("argv[%d] = %s\n", i, argv[i]);
		}
#endif
	    if(do_fat_fsload(0, 0, 5, bmp_argv))
		{
		   printf("sunxi bmp info error : unable to open bmp file %s\n", argv[2]);

		   return cmd_usage(cmdtp);
	    }
	}
	else
	{
		return cmd_usage(cmdtp);
	}
	if(de_addr < CONFIG_SYS_SDRAM_BASE)
	{
#if defined(CONFIG_SUNXI_LOGBUFFER)
		de_addr = CONFIG_SYS_SDRAM_BASE + gd->ram_size - SUNXI_DISPLAY_FRAME_BUFFER_SIZE;
#else
		de_addr = SUNXI_DISPLAY_FRAME_BUFFER_ADDR;
#endif
	}
	debug("bmp addr %x, display addr %x\n", addr, de_addr);
	bmp_info.buffer = (void *)de_addr;
	if(!sunxi_bmp_decode(addr, &bmp_info))
	{
		debug("decode bmp ok\n");

		return sunxi_bmp_show(bmp_info);
	}
	debug("decode bmp error\n");

	return -1;
}


U_BOOT_CMD(
	sunxi_bmp_show,	4,	1,	do_sunxi_bmp_display,
	"manipulate BMP image data",
	"sunxi_bmp_display addr [de addr]\n"
	"parameters 1 : the address where the bmp stored\n"
	"parameters 2 : option para, the address where the bmp display\n"
);

#ifdef CONFIG_BOOT_GUI

int show_bmp_on_fb(char *bmp_head_addr, unsigned int fb_id)
{
	bmp_image_t *bmp = (bmp_image_t *)bmp_head_addr;
	struct canvas *cv = NULL;
	char *src_addr;
	int src_width, src_height, src_stride, src_cp_bytes;
	char *dst_addr_b, *dst_addr_e;
	rect_t dst_crop;
	int need_set_bg = 0;

	cv = fb_lock(fb_id);
	if ((NULL == cv) || (NULL == cv->base)) {
		printf("cv=%p, base= %p\n", cv,
			(cv != NULL) ? cv->base : 0x0);
		goto err_out;
	}
	if ((bmp->header.signature[0] != 'B')
		|| (bmp->header.signature[1] != 'M')) {
		printf("this is not a bmp picture\n");
		goto err_out;
	}
	if ((24 != bmp->header.bit_count)
		&& (32 != bmp->header.bit_count)) {
		printf("no support %dbit bmp\n", bmp->header.bit_count);
		goto err_out;
	}

	src_width = bmp->header.width;
	if (bmp->header.height & 0x80000000) {
		src_height = -bmp->header.height;
	} else {
		src_height = bmp->header.height;
	}
	if ((src_width > cv->width)
		|| (src_height > cv->height)) {
		printf("no support big size bmp[%dx%d] on fb[%dx%d]\n",
			src_width, src_height,
			cv->width, cv->height);
		goto err_out;
	}

	src_cp_bytes = src_width * bmp->header.bit_count >> 3;
	src_stride = ((src_width * bmp->header.bit_count + 31) >> 5) << 2;
	src_addr = (char *)(bmp_head_addr + bmp->header.data_offset);
	if (!(bmp->header.height & 0x80000000)) {
		src_addr += (src_stride * (src_height - 1));
		src_stride = -src_stride;
	}

	dst_crop.left = (cv->width - src_width) >> 1;
	dst_crop.right = dst_crop.left + src_width;
	dst_crop.top = (cv->height - src_height) >> 1;
	dst_crop.bottom = dst_crop.top + src_height;
	dst_addr_b = (char *)cv->base + cv->stride * dst_crop.top
		+ (dst_crop.left * cv->bpp >> 3);
	dst_addr_e = dst_addr_b + cv->stride * src_height;

	need_set_bg = cv->set_interest_region(cv, &dst_crop, 1, NULL);
	if (0 != need_set_bg) {
		if (src_width != cv->width) {
			debug("memset full fb\n");
			memset((void *)(cv->base), 0, cv->stride * cv->height);
		} else if (0 != dst_crop.top) {
			debug("memset top fb\n");
			memset((void *)(cv->base), 0, cv->stride * dst_crop.top);
		}
	}
	if (cv->bpp == bmp->header.bit_count) {
		for (; dst_addr_b != dst_addr_e; dst_addr_b += cv->stride) {
			memcpy((void *)dst_addr_b, (void *)src_addr, src_cp_bytes);
			src_addr += src_stride;
		}
	} else {
		if ((24 == bmp->header.bit_count) && (32 == cv->bpp)) {
			for (; dst_addr_b != dst_addr_e; dst_addr_b += cv->stride) {
				int *d = (int *)dst_addr_b;
				char *c_b = src_addr;
				char *c_end = c_b + src_cp_bytes;
				for (; c_b < c_end;) {
					*d++ = 0xFF000000 | ((*(c_b + 2)) << 16)
						| ((*(c_b + 1)) << 8) | (*c_b);
					c_b += 3;
				}
				src_addr += src_stride;
			}
		} else {
			printf("no support %dbit bmp picture on %dbit fb\n",
				bmp->header.bit_count, cv->bpp);
		}
	}
	if (0 != need_set_bg) {
		if ((cv->height != dst_crop.bottom)
			&& (src_width == cv->width)) {
			debug("memset bottom fb\n");
			memset((void *)(cv->base + cv->stride * dst_crop.bottom),
				0, cv->stride * (cv->height - dst_crop.bottom));
		}
	}

	if (32 == cv->bpp) {
		fb_set_alpha_mode(fb_id, FB_GLOBAL_ALPHA_MODE, 0xFF);
	}

	fb_unlock(fb_id, NULL, 1);
	save_disp_cmd();

	return 0;

err_out:
	if (NULL != cv)
		fb_unlock(fb_id, NULL, 0);
	return -1;
}

int sunxi_bmp_display(char *name)
{
	int ret = 0;
	char *argv[6];
	char bmp_head[32];
	char bmp_name[32];
	char *bmp_head_addr = (char *)CONFIG_SYS_SDRAM_BASE;

#ifdef ENABLE_ADVERT_PICTURE
	if (!strcmp(name, "bootlogo.bmp"))
		if (!sunxi_advert_display("Reserve0", "advert.bmp"))
			return 0;
#endif

	if (NULL != bmp_head_addr) {
		sprintf(bmp_head, "%lx", (ulong)bmp_head_addr);
	} else {
		printf("sunxi bmp: alloc buffer for %s fail\n", name);
		return -1;
	}
	strncpy(bmp_name, name, sizeof(bmp_name));
	printf("bmp_name=%s\n", bmp_name);

	argv[0] = "fatload";
	argv[1] = "sunxi_flash";
#ifdef CONFIG_GPT_SUPPORT
	argv[2] = "1:0";
#else
	argv[2] = "0:0";
#endif
	argv[3] = bmp_head;
	argv[4] = bmp_name;
	argv[5] = NULL;
	if (do_fat_fsload(0, 0, 5, argv)) {
		printf("sunxi bmp info error : unable to open logo file %s\n", argv[4]);
		return -1;
	}

	ret = show_bmp_on_fb(bmp_head_addr, FB_ID_0);
	if (0 != ret)
		printf("show bmp on fb failed !\n");
	tick_printf("show bmp on ok\n");

	return ret;
}

int sunxi_bmp_dipslay_screen(sunxi_bmp_store_t bmp_info)
{
	return show_bmp_on_fb(bmp_info.buffer, FB_ID_0);
}

#else

int sunxi_bmp_display(char *name)
{
	sunxi_bmp_store_t bmp_info;
	char *argv[6];
	char  bmp_head[32];
	char  bmp_name[32] = {0};
	ulong bmp_buff = CONFIG_SYS_SDRAM_BASE;
	int  ret = -1;

	/* set bmp decode addr is CONFIG_SYS_SDRAM_BASE */
	sprintf(bmp_head, "%lx", (ulong)bmp_buff);
	strncpy(bmp_name, name, sizeof(bmp_name));

	argv[0] = "fatload";
	argv[1] = "sunxi_flash";
#ifdef CONFIG_GPT_SUPPORT
	argv[2] = "1:0";
#else
	argv[2] = "0:0";
#endif
	argv[3] = bmp_head;
	argv[4] = bmp_name;
	argv[5] = NULL;

	if (do_fat_fsload(0, 0, 5, argv))
	{
		pr_error("unable to open logo file %s\n", argv[4]);
		return -1;
	}

#if defined(CONFIG_SUNXI_LOGBUFFER)
	bmp_info.buffer = (void *)(CONFIG_SYS_SDRAM_BASE + gd->ram_size - SUNXI_DISPLAY_FRAME_BUFFER_SIZE);
#else
	bmp_info.buffer = (void *)(SUNXI_DISPLAY_FRAME_BUFFER_ADDR);
#endif
	pr_msg("bmp file buffer: 0x%lx, bmp_info.buffer: %lx\n",
		bmp_buff, (ulong)bmp_info.buffer);
	if (sunxi_bmp_decode(bmp_buff, &bmp_info))
		return -1;

	ret = sunxi_bmp_show(bmp_info);
	return ret;

}


int sunxi_bmp_display_mem(unsigned char *source, sunxi_bmp_store_t *bmp_info)
{
	int  ret = -1;

#if defined(CONFIG_SUNXI_LOGBUFFER)
	bmp_info->buffer = (void *)(CONFIG_SYS_SDRAM_BASE + gd->ram_size - SUNXI_DISPLAY_FRAME_BUFFER_SIZE);
#else
	bmp_info->buffer = (void *)(SUNXI_DISPLAY_FRAME_BUFFER_ADDR);
#endif
	pr_notice("bmp file buffer: 0x%lx, bmp_info.buffer: %lx\n",(ulong)source,(ulong)bmp_info->buffer);
	ret = sunxi_bmp_decode((ulong)source, bmp_info);
	if (!ret)
		debug("decode bmp ok\n");

	return ret;
}

int sunxi_bmp_dipslay_screen(sunxi_bmp_store_t bmp_info)
{
	return sunxi_bmp_show(bmp_info);
}


#endif

int sunxi_bmp_load(char *name)
{
	int ret = 0;
	char *argv[6];
	char bmp_head[32];
	char bmp_name[32];
	char *bmp_head_addr = (char *)CONFIG_SYS_SDRAM_BASE;

#ifdef ENABLE_ADVERT_PICTURE
	if (!strcmp(name, "bootlogo.bmp")) {
		if (!sunxi_advert_logo_load("Reserve0", "advert.bmp")) {
			return 0;
                } else {            //if no advert.bmp in Reserve0, will not search bootlogo.bmp in bootloader partition
                        return -1;  //return immediately
                }
        }
#endif

	if (NULL != bmp_head_addr) {
		sprintf(bmp_head, "%lx", (ulong)bmp_head_addr);
	} else {
		printf("sunxi bmp: alloc buffer for %s fail\n", name);
		return -1;
	}
	strncpy(bmp_name, name, sizeof(bmp_name));
	printf("bmp_name=%s\n", bmp_name);

	argv[0] = "fatload";
	argv[1] = "sunxi_flash";
	argv[2] = "0:0";
	argv[3] = bmp_head;
	argv[4] = bmp_name;
	argv[5] = NULL;
	if (do_fat_fsload(0, 0, 5, argv)) {
		printf("sunxi bmp info error : unable to open logo file %s\n", argv[4]);
		return -1;
	}
	return ret;
}


/*
 * Subroutine:  bmp_info
 *
 * Description: Show information about bmp file in memory
 *
 * Inputs:	addr		address of the bmp file
 *
 * Return:      None
 *
 */
static int sunxi_bmp_probe_info(uint addr)
{
	bmp_image_t *bmp=(bmp_image_t *)addr;

	if((bmp->header.signature[0]!='B') || (bmp->header.signature[1]!='M'))
	{
		printf("this is not a bmp picture\n");

		return -1;
	}
	debug("bmp picture dectede\n");

	printf("Image size    : %d x %d\n", bmp->header.width, (bmp->header.height & 0x80000000) ? (-bmp->header.height):(bmp->header.height));
	printf("Bits per pixel: %d\n", bmp->header.bit_count);

	return(0);
}

/*
 * Subroutine:  bmp_display
 *
 * Description: Display bmp file located in memory
 *
 * Inputs:	addr		address of the bmp file
 *
 * Return:      None
 *
 */
int sunxi_bmp_decode(unsigned long addr, sunxi_bmp_store_t *bmp_info)
{
	char *tmp_buffer;
	char *bmp_data;
	int zero_num = 0;
	bmp_image_t *bmp = (bmp_image_t *)addr;
	int x, y, bmp_bpix;
	int tmp;

	if((bmp->header.signature[0]!='B') || (bmp->header.signature[1] !='M'))
	{
		printf("this is not a bmp picture\n");

		return -1;
	}
	debug("bmp dectece\n");

	bmp_bpix = bmp->header.bit_count/8;
	if((bmp_bpix != 3) && (bmp_bpix != 4))
	{
		printf("no support bmp picture without bpix 24 or 32\n");

		return -1;
	}
	if(bmp_bpix ==3)
	{		
		zero_num = (4 - ((3*bmp->header.width) % 4))&3;
	}
	debug("bmp bitcount %d\n", bmp->header.bit_count);
	x = bmp->header.width;
	y = (bmp->header.height & 0x80000000) ? (-bmp->header.height):(bmp->header.height);
	debug("bmp x = %x, bmp y = %x\n", x, y);

	tmp = bmp->header.height;
	if (0 == (bmp->header.height & 0x80000000))
		bmp->header.height = (-bmp->header.height);
	memcpy(bmp_info->buffer, bmp, sizeof(bmp_header_t));
	bmp_info->buffer += sizeof(bmp_header_t);
	bmp->header.height = tmp;

	tmp_buffer = (char *)bmp_info->buffer;
	bmp_data = (char *)(addr + bmp->header.data_offset);
	if(bmp->header.height & 0x80000000)
    {
	      if(zero_num == 0)
                {
                    memcpy(tmp_buffer,bmp_data,x*y*bmp_bpix);
                }
                else
                {
                    int i, line_bytes, real_line_byte;	
	            char *src;
	            line_bytes = (x * bmp_bpix) + zero_num;
		    real_line_byte = x * bmp_bpix;
		    for(i=0; i<y; i++)
                   {
             	    src = bmp_data + i*line_bytes;
                     memcpy(tmp_buffer, src, real_line_byte);
                    tmp_buffer += real_line_byte;
                    }
                }
    }
    else
    {
    	uint i, line_bytes, real_line_byte;
        char *src;

		line_bytes = (x * bmp_bpix) + zero_num;
		real_line_byte = x * bmp_bpix;
		for(i=0; i<y; i++)
        {
        	src = bmp_data + (y - i - 1) * line_bytes;
        	memcpy(tmp_buffer, src, real_line_byte);
            tmp_buffer += real_line_byte;
        }
    }
    bmp_info->x = x;
    bmp_info->y = y;
    bmp_info->bit = bmp->header.bit_count;
	//flush_cache((uint)bmp_info->buffer, x * y * bmp_bpix);
	flush_cache((uint)bmp_info->buffer-sizeof(bmp_header_t) , x * y * bmp_bpix+sizeof(bmp_header_t));

	return 0;
}

static int sunxi_bmp_show(sunxi_bmp_store_t bmp_info)
{
	debug("begin to set framebuffer\n");
	if(board_display_framebuffer_set(bmp_info.x, bmp_info.y, bmp_info.bit, (void *)bmp_info.buffer))
	{
		printf("sunxi bmp display error : set frame buffer error\n");

		return -2;
	}
	debug("begin to show layer\n");
	board_display_show(0);
	debug("bmp display finish\n");

	return 0;
}

static int fat_read_file_ex(char *fatname, char *filename, char *addr)
{
	char file_name[32];
	char fat_name[32];
	char partition[32];
	int partition_num = -1;
	char *bmp_buff = NULL;
	char bmp_addr[32]={0};

	memset(file_name, 0, 32);
	strcpy(file_name, filename);

	memset(fat_name, 0, 32);
	strcpy(fat_name, fatname);
	
	partition_num = sunxi_partition_get_partno_byname(fat_name);
	if(partition_num < 0)
	{
		printf("[boot disp] can not find the partition %s\n",fat_name);
		return -1;
	}
	sprintf(partition,"%x:0",partition_num);
	bmp_buff = addr;
	if(bmp_buff == NULL)
	{
		printf("sunxi bmp: alloc buffer fail\n");
		return -1;
	}
	char * bmp_argv[6] = {"fatload", "sunxi_flash", "0:0", "00000000", file_name, NULL };
	bmp_argv[2] = partition;
    sprintf(bmp_addr,"%lx", (ulong)bmp_buff);
	bmp_argv[3] = bmp_addr;
	if(do_fat_fsload(0, 0, 5, bmp_argv))
	{
		printf("sunxi bmp info error : unable to open logo file %s\n", bmp_argv[1]);
		return -1;
	}
	return 0;
}

static __s32 check_sum(void *mem_base, __u32 size, __u32 src_sum)
{
	__u32 *buf = (__u32 *)mem_base;
	__u32 count = 0;
	__u32 sum = 0;
	__u32 last = 0;
	__u32 curlen = 0;
	__s32 i = 0;

	count = size >> 2;
	do {
		sum += *buf++;
		sum += *buf++;
		sum += *buf++;
		sum += *buf++;
	} while ((count -= 4) > (4 - 1));
	for (i = 0; i < count; i++) {
		sum += *buf++;
	}
	curlen = size % 4;
	if ((size & 0x03) != 0) {
		memcpy(&last, mem_base + size - curlen, curlen);
		sum += last;
	}

	if (sum == src_sum) {
		return 0;
	} else {
		printf("err: sum=%x; src_sum=%x\n", sum, src_sum);
		return -1;
	}
}

static int sunxi_advert_verify_head(struct __advert_head *adv_head)
{    
	char *addr = (char *)CONFIG_SYS_SDRAM_BASE;

	if ((0 > fat_read_file_ex("Reserve0", "advert.crc", addr))) {
		return -1;
	};

	memcpy((u32 *)adv_head, (u32 *)addr, sizeof(*adv_head));

	if (memcmp((char *)(adv_head->magic), ADVERT_MAGIC, strlen(ADVERT_MAGIC))) {
		printf("advert magic not equal,%s\n", (char *)(adv_head->magic));
		return -1;
	}

	if ((SUNXI_DISPLAY_FRAME_BUFFER_SIZE < adv_head->length)
		|| (0 == adv_head->length)) {
		printf("advert length=%d to big or to short\n",
			adv_head->length);
		return -1;
	}

	return 0;
}

int sunxi_advert_logo_load(char *fatname, char *filename)
{
	struct __advert_head advert_head;

	if (0 > sunxi_advert_verify_head(&advert_head)) {
		return -1;
	}

	if ((0 > fat_read_file_ex("Reserve0", "advert.bmp",
			(char *)CONFIG_SYS_SDRAM_BASE))
		|| (0 > check_sum((u32 *)CONFIG_SYS_SDRAM_BASE,
			advert_head.length, advert_head.check_sum))) {
		return -1;
	}
	return 0;
}


int sunxi_advert_display(char *fatname, char *filename)
{
	struct __advert_head advert_head;

	if (0 > sunxi_advert_verify_head(&advert_head)) {
		return -1;
	}

	if ((0 > fat_read_file_ex("Reserve0", "advert.bmp",
			(char *)CONFIG_SYS_SDRAM_BASE))
		|| (0 > check_sum((u32 *)CONFIG_SYS_SDRAM_BASE,
			advert_head.length, advert_head.check_sum))) {
		return -1;
	}

#ifdef CONFIG_BOOT_GUI
	return show_bmp_on_fb((char *)CONFIG_SYS_SDRAM_BASE, FB_ID_0);
#else

	sunxi_bmp_store_t bmp_info;

#if defined(CONFIG_SUNXI_LOGBUFFER)
	bmp_info.buffer = (void *)(CONFIG_SYS_SDRAM_BASE + gd->ram_size - SUNXI_DISPLAY_FRAME_BUFFER_SIZE);
#else
	bmp_info.buffer = (void *)(SUNXI_DISPLAY_FRAME_BUFFER_ADDR);
#endif

	debug("check_sum advert bmp ok\n");
	if (!sunxi_bmp_decode(CONFIG_SYS_SDRAM_BASE, &bmp_info)) {
		debug("decode bmp ok\n");
		return sunxi_bmp_show(bmp_info);
	}
	return -1;

#endif /*#ifdef CONFIG_BOOT_GUI*/
}

int do_sunxi_logo(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	return sunxi_bmp_display("bootlogo.bmp");
}


U_BOOT_CMD(
	logo,	1,	0,	do_sunxi_logo,
	"show default logo",
	"no args\n"
);

