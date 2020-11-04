/*
 * (C) Copyright 2018 allwinnertech  <wangwei@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <config.h>
#include <common.h>
#include <malloc.h>
//#include "sprite_storage_crypt.h"
#include <sunxi_board.h>
#include <sunxi_flash.h>
#include <memalign.h>

int sunxi_secure_storage_erase(const char *item_name);
int sunxi_secure_storage_erase_data_only(const char *item_name);

static unsigned int secure_storage_inited = 0;

static unsigned int map_dirty;

static inline void set_map_dirty(void)
{
	map_dirty = 1;
}
static inline void clear_map_dirty(void)
{
	map_dirty = 0;
}
static inline int try_map_dirty(void)
{
	return map_dirty;
}

static struct map_info secure_storage_map = { { 0 } };

/*
************************************************************************************************************
*
*                                             function
*
*    name          :  __probe_name_in_map 在secure storage map中查找指定项
*
*parmeters     :  buffer :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
static int __probe_name_in_map(unsigned char *buffer, const char *item_name,
			       int *len)
{
	unsigned char *buf_start = buffer;
	int index		 = 1;
	ALLOC_CACHE_ALIGN_BUFFER(unsigned char, name, MAP_KEY_NAME_SIZE);
	ALLOC_CACHE_ALIGN_BUFFER(unsigned char, length, MAP_KEY_DATA_SIZE);
	int i, j;

	while (*buf_start != '\0' && (buf_start - buffer) < SEC_BLK_SIZE) {
		memset(name, 0, 64);
		memset(length, 0, 32);
		i = j = 0;
		while (buf_start[i] != ':' && (buf_start[i] != '\0') &&
		       (&buf_start[i] - buffer) < SEC_BLK_SIZE &&
		       j < MAP_KEY_NAME_SIZE) {
			name[j] = buf_start[i];
			i++;
			j++;
		}

		if (j >= MAP_KEY_NAME_SIZE)
			return -1;

		i++;
		j = 0;
		while ((buf_start[i] != ' ') && (buf_start[i] != '\0') &&
		       (&buf_start[i] - buffer) < SEC_BLK_SIZE &&
		       j < MAP_KEY_DATA_SIZE) {
			length[j] = buf_start[i];
			i++;
			j++;
		}

		/* deal dirty data */
		if ((&buf_start[i] - buffer) >= SEC_BLK_SIZE ||
		    j >= MAP_KEY_DATA_SIZE) {
			return -1;
		}

		if (!strcmp(item_name, (const char *)name)) {
			buf_start += strlen(item_name) + 1;
			*len = simple_strtoul((const char *)length, NULL, 10);
			pr_force("name in map %s\n", name);
			return index;
		}
		index++;
		buf_start += strlen((const char *)buf_start) + 1;
	}

	return -1;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
static int __fill_name_in_map(unsigned char *buffer, const char *item_name,
			      int length)
{
	unsigned char *buf_start = buffer;
	int index		 = 1;
	int name_len;

	while (*buf_start != '\0' && (buf_start - buffer) < SEC_BLK_SIZE) {
		name_len = 0;
		while (buf_start[name_len] != ':' &&
		       (&buf_start[name_len] - buffer) < SEC_BLK_SIZE &&
		       name_len < MAP_KEY_NAME_SIZE)
			name_len++;

		/* deal dirty data */
		if ((&buf_start[name_len] - buffer) >= SEC_BLK_SIZE ||
		    name_len >= MAP_KEY_NAME_SIZE) {
			pr_msg("__fill_name_in_map: dirty map, memset 0\n");
			memset(buffer, 0x0, SEC_BLK_SIZE);
			buf_start = buffer;
			index     = 1;
			break;
		}

		if (!memcmp((const char *)buf_start, item_name, name_len)) {
			pr_force("name in map %s\n", buf_start);
			return index;
		}
		index++;
		buf_start += strlen((const char *)buf_start) + 1;
	}
	if (index >= 32)
		return -1;
	sprintf((char *)buf_start, "%s:%d", item_name, length);

	return index;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
static int __discard_name_in_map(unsigned char *buffer, const char *item_name)
{
	unsigned char *buf_start = buffer, *last_start;
	int index		 = 1;
	int name_len;

	while (*buf_start != '\0' && (buf_start - buffer) < SEC_BLK_SIZE) {
		name_len = 0;
		while (buf_start[name_len] != ':' &&
		       (&buf_start[name_len] - buffer) < SEC_BLK_SIZE &&
		       name_len < MAP_KEY_NAME_SIZE)
			name_len++;

		/* deal dirty data */
		if ((&buf_start[name_len] - buffer) >= SEC_BLK_SIZE ||
		    name_len >= MAP_KEY_NAME_SIZE) {
			return -1;
		}

		if (!memcmp((const char *)buf_start, item_name, name_len)) {
			last_start =
				buf_start + strlen((const char *)buf_start) + 1;
			if (*last_start == '\0') {
				memset(buf_start, 0,
				       strlen((const char *)buf_start));
			} else {
				memcpy(buf_start, last_start,
				       4096 - (last_start - buffer));
			}

			return index;
		}
		index++;
		buf_start += strlen((const char *)buf_start) + 1;
	}

	return -1;
}

int check_secure_storage_map(void *buffer)
{
	struct map_info *map_buf = (struct map_info *)buffer;

	if (map_buf->magic != STORE_OBJECT_MAGIC) {
		pr_err("Item0 (Map) magic is bad\n");
		return 2;
	}
	if (map_buf->crc !=
	    crc32(0, (void *)map_buf, sizeof(struct map_info) - 4)) {
		pr_err("Item0 (Map) crc is fail [0x%x]\n", map_buf->crc);
		return -1;
	}
	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int sunxi_secure_storage_init(void)
{
	int ret;

	if (!secure_storage_inited) {
		ret = sunxi_secstorage_read(
			0, (unsigned char *)&secure_storage_map, 4096);
		if (ret < 0) {
			pr_err("get secure storage map err\n");

			return -1;
		}

		ret = check_secure_storage_map(&secure_storage_map);
		if (ret == -1) {
			if ((secure_storage_map.data[0] == 0xff) ||
			    (secure_storage_map.data[0] == 0x0))
				memset(&secure_storage_map, 0, SEC_BLK_SIZE);
		} else if (ret == 2) {
			if ((secure_storage_map.data[0] == 0xff) ||
			    (secure_storage_map.data[0] == 0x00)) {
				pr_msg("the secure storage map is empty\n");
				memset(&secure_storage_map, 0, SEC_BLK_SIZE);
			} else {
				/* no things */
			}
		}
	}
	secure_storage_inited = 1;

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int sunxi_secure_storage_exit(void)
{
	int ret;

	if (!secure_storage_inited) {
		pr_err("%s err: secure storage has not been inited\n",
		       __func__);

		return -1;
	}
	if (try_map_dirty()) {
		secure_storage_map.magic = STORE_OBJECT_MAGIC;
		secure_storage_map.crc   = crc32(0, (void *)&secure_storage_map,
					       sizeof(struct map_info) - 4);
		ret = sunxi_secstorage_write(
			0, (unsigned char *)&secure_storage_map, 4096);
		if (ret < 0) {
			pr_err("write secure storage map\n");

			return -1;
		}
		clear_map_dirty();
	}
	secure_storage_inited = 0;

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int sunxi_secure_storage_list(void)
{
	int	    ret, index = 1;
	unsigned char *buf_start = (unsigned char *)&secure_storage_map;
	ALLOC_CACHE_ALIGN_BUFFER(unsigned char, buffer, 4096);

	if (sunxi_secure_storage_init()) {
		pr_err("%s secure storage init err\n", __func__);

		return -1;
	}

	char name[64], length[32];
	int  i, j, len;

	while (*buf_start != '\0') {
		memset(name, 0, 64);
		memset(length, 0, 32);
		i = 0;
		while (buf_start[i] != ':') {
			name[i] = buf_start[i];
			i++;
		}
		i++;
		j = 0;
		while ((buf_start[i] != ' ') && (buf_start[i] != '\0')) {
			length[j] = buf_start[i];
			i++;
			j++;
		}

		pr_force("name in map %s\n", name);
		len = simple_strtoul((const char *)length, NULL, 10);

		ret = sunxi_secstorage_read(index, buffer, 4096);
		if (ret < 0) {
			pr_err("get secure storage index %d err\n", index);

			return -1;
		} else if (ret > 0) {
			pr_msg("the secure storage index %d is empty\n", index);

			return -1;
		} else {
			pr_force("%d data:\n", index);
			sunxi_dump(buffer, len);
		}
		index++;
		buf_start += strlen((const char *)buf_start) + 1;
	}

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int sunxi_secure_storage_probe(const char *item_name)
{
	int ret;
	int len;

	if (!secure_storage_inited) {
		pr_err("%s err: secure storage has not been inited\n",
		       __func__);

		return -1;
	}
	ret = __probe_name_in_map((unsigned char *)&secure_storage_map,
				  item_name, &len);
	if (ret < 0) {
		pr_err("no item name %s in the map\n", item_name);

		return -1;
	}

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int sunxi_secure_storage_read(const char *item_name, char *buffer,
			      int buffer_len, int *data_len)
{
	int	   ret, index;
	int	   len_in_store;
	ALLOC_CACHE_ALIGN_BUFFER(unsigned char, buffer_to_sec, 4096);

	if (!secure_storage_inited) {
		pr_err("%s err: secure storage has not been inited\n",
		       __func__);

		return -1;
	}
	index = __probe_name_in_map((unsigned char *)&secure_storage_map,
				    item_name, &len_in_store);
	if (index < 0) {
		pr_msg("no item name %s in the map\n", item_name);

		return -2;
	}
	memset(buffer_to_sec, 0, 4096);
	ret = sunxi_secstorage_read(index, buffer_to_sec, 4096);
	if (ret < 0) {
		pr_err("read secure storage block %d name %s err\n", index,
		       item_name);

		return -3;
	}
	if (len_in_store > buffer_len) {
		memcpy(buffer, buffer_to_sec, buffer_len);
	} else {
		memcpy(buffer, buffer_to_sec, len_in_store);
	}
	*data_len = len_in_store;

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/

int sunxi_secure_storage_write(const char *item_name, char *buffer, int length)
{
	int  ret, index;
	int  len = 0;
	ALLOC_CACHE_ALIGN_BUFFER(char, tmp_buf, 4096);

	if (!secure_storage_inited) {
		pr_err("%s err: secure storage has not been inited\n",
		       __func__);

		return -1;
	}

	index = __probe_name_in_map((unsigned char *)&secure_storage_map,
				    item_name, &len);
	if (index < 0) {
		index = __fill_name_in_map((unsigned char *)&secure_storage_map,
					   item_name, length);
		if (index < 0) {
			pr_err("write secure storage block %d name %s overrage\n",
			       index, item_name);
			return -1;
		}
	} else {
		pr_force("There is the same name in the secure storage, try to erase it\n");
		if (len != length) {
			pr_err("the length is not match with key has store in secure storage\n");
			return -1;
		} else {
			if (sunxi_secure_storage_erase_data_only(item_name) <
			    0) {
				pr_err("Erase item %s fail\n", item_name);
				return -1;
			}
		}
	}
	memset(tmp_buf, 0x0, 4096);
	memcpy(tmp_buf, buffer, length);
	ret = sunxi_secstorage_write(index, (unsigned char *)tmp_buf, 4096);
	if (ret < 0) {
		pr_err("write secure storage block %d name %s err\n", index,
		       item_name);

		return -1;
	}
	set_map_dirty();
	pr_force("write secure storage: %d ok\n", index);

	return 0;
}

/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/

int sunxi_secure_storage_erase_data_only(const char *item_name)
{
	int	   ret, index, len;
	ALLOC_CACHE_ALIGN_BUFFER(unsigned char, buffer, 4096);

	if (!secure_storage_inited) {
		pr_err("%s err: secure storage has not been inited\n",
		       __func__);

		return -1;
	}
	index = __probe_name_in_map((unsigned char *)&secure_storage_map,
				    item_name, &len);
	if (index < 0) {
		pr_err("no item name %s in the map\n", item_name);

		return -2;
	}
	memset(buffer, 0xff, 4096);
	ret = sunxi_secstorage_write(index, buffer, 4096);
	if (ret < 0) {
		pr_err("erase secure storage block %d name %s err\n", index,
		       item_name);

		return -1;
	}
	set_map_dirty();
	pr_force("erase secure storage: %d data only ok\n", index);

	return 0;
}

/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/

int sunxi_secure_storage_erase(const char *item_name)
{
	int	   ret, index;
	ALLOC_CACHE_ALIGN_BUFFER(unsigned char, buffer, 4096);

	if (!secure_storage_inited) {
		pr_err("%s err: secure storage has not been inited\n",
		       __func__);

		return -1;
	}
	index = __discard_name_in_map((unsigned char *)&secure_storage_map,
				      item_name);
	if (index < 0) {
		pr_err("no item name %s in the map\n", item_name);

		return -2;
	}
	memset(buffer, 0xff, 4096);
	ret = sunxi_secstorage_write(index, buffer, 4096);
	if (ret < 0) {
		pr_err("erase secure storage block %d name %s err\n", index,
		       item_name);

		return -1;
	}
	set_map_dirty();
	pr_force("erase secure storage: %d ok\n", index);

	return 0;
}

/*
************************************************************************************************************
*                                          function
*    erase all the secure storage data
*    name          :
*    parmeters     :
*    return        :
*    note          :
*
************************************************************************************************************
*/
int sunxi_secure_storage_erase_all(void)
{
	int ret;
	if (!secure_storage_inited) {
		sunxi_secure_storage_init();
	}

	memset(&secure_storage_map, 0x00, 4096);
	ret = sunxi_secstorage_write(0, (unsigned char *)&secure_storage_map,
				     4096);
	if (ret < 0) {
		pr_err("erase secure storage block 0 err\n");

		return -1;
	}
	pr_force("erase secure storage: 0 ok\n");

	return 0;
}
