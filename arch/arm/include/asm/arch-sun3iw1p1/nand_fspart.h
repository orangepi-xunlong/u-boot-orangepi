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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef _NAND_MBR_H_
#define _NAND_MBR_H_

#define     DOWNLOAD_MAP_NAME   "dlinfo.fex"
#define     MBR_SIZE            1024
#define     MBR_MAGIC           "softw311"
#define     MBR_START_ADDRESS   0x00000000
#define     MBR_MAX_PART_COUNT  15
#define     MBR_COPY_NUM        4
#define     MBR_RESERVED        (MBR_SIZE - 20 - (MBR_MAX_PART_COUNT * sizeof(PARTITION)))

typedef struct tag_PARTITION
{
    unsigned  int       addrhi;
    unsigned  int       addrlo;
    unsigned  int       lenhi;
    unsigned  int       lenlo;
    unsigned  char      classname[12];
    unsigned  char      name[12];
    unsigned  int       user_type;
    unsigned  int       ro;
    unsigned  char      res[16];
} __attribute__ ((packed))PARTITION;

typedef struct tag_MBR
{
    unsigned  int       crc32;
    unsigned  int       version;
    unsigned  char      magic[8];
    unsigned  char      copy;
    unsigned  char      index;
    unsigned  short     PartCount;
    PARTITION           array[MBR_MAX_PART_COUNT];
    unsigned  char      res[MBR_RESERVED];
}__attribute__ ((packed)) MBR;

typedef struct tag_one_part_info
{
    unsigned  char      classname[12];
    unsigned  char      name[12];
    unsigned  int       addrhi;
    unsigned  int       addrlo;
    unsigned  int       lenhi;
    unsigned  int       lenlo;
    unsigned  char      part_name[12];
    unsigned  char      dl_filename[16];
    unsigned  char      vf_filename[16];
    unsigned  int       encrypt;
}
dl_one_part_info;

typedef struct tag_download_info
{
    unsigned  int       crc32;
    unsigned  int       version;
    unsigned  char      magic[8];
    unsigned  int       download_count;
    dl_one_part_info    one_part_info[MBR_MAX_PART_COUNT];
}
download_info;

#endif  //_NAND_MBR_H_

/* end of _NAND_MBR_H_ */

