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
/* MBR       */
#define     MBR_SIZE            1024
#define     MBR_MAGIC           "softw311"
#define     MBR_START_ADDRESS   0x00000000
#define     MBR_MAX_PART_COUNT  15
#define     MBR_COPY_NUM        4 //number of mbr copy
#define     MBR_RESERVED        (MBR_SIZE - 20 - (MBR_MAX_PART_COUNT * sizeof(PARTITION)))

//partition info, 64byte
typedef struct tag_PARTITION
{
    unsigned  int       addrhi;             //begin addr, sector
    unsigned  int       addrlo;             //
    unsigned  int       lenhi;              //length
    unsigned  int       lenlo;              //
    unsigned  char      classname[12];      //client device
    unsigned  char      name[12];           //host device
    unsigned  int       user_type;          //user type
    unsigned  int       ro;                 //R/W property
    unsigned  char      res[16];            //resever
} __attribute__ ((packed))PARTITION;
//MBR info
typedef struct tag_MBR
{
    unsigned  int       crc32;                      // crc 1k - 4
    unsigned  int       version;                    //version info 0x00000100
    unsigned  char      magic[8];                   //"softw311"
    unsigned  char      copy;
    unsigned  char      index;                      //index of MBR copy
    unsigned  short     PartCount;                  //sector number
    PARTITION           array[MBR_MAX_PART_COUNT];  //
    unsigned  char      res[MBR_RESERVED];
}__attribute__ ((packed)) MBR;

typedef struct tag_one_part_info
{
    unsigned  char      classname[12];      //burnning partition's client device name
    unsigned  char      name[12];           //burnning partition's host device name
    unsigned  int       addrhi;             //burnning partition's high addr sector unit
    unsigned  int       addrlo;             //burnning partition's low addr sector unit
    unsigned  int       lenhi;              //burnning partition's length high 32bit sector unit
    unsigned  int       lenlo;              //burnning partition's lengt, low 32bit sector unit
    unsigned  char      part_name[12];      //burnning partition's name ,relate with classname in MBR
    unsigned  char      dl_filename[16];    //burnning partition's file name fixed length 16 bytes
    unsigned  char      vf_filename[16];    //burnning partition's verify file name, fixed length 16 bytes
    unsigned  int       encrypt;            //encrypt burnning partition or not, 0:encrypt 1:plain
}
dl_one_part_info;
//partition info
typedef struct tag_download_info
{
    unsigned  int       crc32;                              //crc
    unsigned  int       version;                            //version info  0x00000101
    unsigned  char      magic[8];                           //"softw311"
    unsigned  int       download_count;                     //burn sector number
    dl_one_part_info    one_part_info[MBR_MAX_PART_COUNT];  //burn sector info
}
download_info;

#endif  //_NAND_MBR_H_

/* end of _NAND_MBR_H_ */

