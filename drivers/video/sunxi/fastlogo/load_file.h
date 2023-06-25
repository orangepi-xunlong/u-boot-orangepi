/*
 * load_file.h
 *
 * Copyright (c) 2007-2021 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <zhengxiaobin@allwinnertech.com>
 *
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
#ifndef _LOAD_FILE_H
#define _LOAD_FILE_H

#ifdef __cplusplus
extern "C" {
#endif

struct file_info_t;

/**
 * file info
 */
struct file_info_t {
	  char *name;
	  char *path;
	  unsigned int file_size;
	  void *file_addr;
	  int (*unload_file)(struct file_info_t *file);
	  int (*print_file_info)(struct file_info_t *file);
};

struct file_info_t *load_file(char *name, char *part_name);



#ifdef __cplusplus
}
#endif

#endif /*End of file*/
