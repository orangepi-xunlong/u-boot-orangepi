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
#include <securestorage.h>

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

			if (strlen(item_name) ==
				    strlen(SECURE_STORAGE_DUMMY_KEY_NAME) &&
			    !memcmp(item_name, SECURE_STORAGE_DUMMY_KEY_NAME,
				    strlen(SECURE_STORAGE_DUMMY_KEY_NAME)) &&
			    *len == 0) {
				/*
				 * if *len == 0 it is a actual DUMMY_KEY,
				 * a key happen to has same name should have a non-zero len
				 */
			} else {
				pr_msg("name in map %s\n", name);
				return index;
			}
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
	uint8_t write_back_buf[sizeof(struct map_info)];
	int dummy_key_index		= -1;
	unsigned char *dummy_key_start	= NULL;
	unsigned char *write_back_start = NULL;

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

		if (!memcmp((const char *)buf_start, item_name, name_len) &&
		    strlen(item_name) == name_len) {
			pr_msg("name in map %s\n", buf_start);
			return index;
		}

		if (name_len == strlen(SECURE_STORAGE_DUMMY_KEY_NAME) &&
		    !memcmp((const char *)buf_start,
			    SECURE_STORAGE_DUMMY_KEY_NAME, name_len) &&
		    dummy_key_index == -1) {
			/*
			 * DUMMY_KEY could be replaced with the input key,
			 * but we dont know whether to do so at this point,
			 * save relate info first
			 */
			pr_msg("found dummy_key %s\n", buf_start);
			dummy_key_index	 = index;
			dummy_key_start	 = buf_start;
			write_back_start = dummy_key_start +
					   strlen((const char *)buf_start) + 1;
			memset(write_back_buf, 0, sizeof(write_back_buf));
			memcpy((char *)write_back_buf, write_back_start,
			       4096 - (write_back_start - buffer));
		}
		index++;
		buf_start += strlen((const char *)buf_start) + 1;
	}
	if ((index >= 32) && (dummy_key_index == -1))
		return -1;

	if (dummy_key_index != -1) {
		/*use index reserved by DUMMY_KEY*/
		sprintf((char *)dummy_key_start, "%s:%d", item_name, length);
		write_back_start = dummy_key_start +
				   strlen((const char *)dummy_key_start) + 1;
		memcpy(write_back_start, write_back_buf,
		       4096 - (write_back_start - buffer));
		return dummy_key_index;
	} else {
		/* add new index */
		sprintf((char *)buf_start, "%s:%d", item_name, length);
		return index;
	}
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
	uint8_t write_back_buf[sizeof(struct map_info)];

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

		if (!memcmp((const char *)buf_start, item_name, name_len) &&
		    strlen(item_name) == name_len) {
			/*
			 * replace discarded key with DUMMY_KEY, so following
			 * keys do not need to change their index
			 */
			memset(write_back_buf, 0, sizeof(write_back_buf));
			last_start =
				buf_start + strlen((const char *)buf_start) + 1;
			memcpy(write_back_buf, last_start,
			       4096 - (last_start - buffer));
			sprintf((char *)buf_start, "%s:%d",
				SECURE_STORAGE_DUMMY_KEY_NAME, 0);

			last_start =
				buf_start + strlen((const char *)buf_start) + 1;
			memcpy((char *)last_start, (char *)write_back_buf,
			       4096 - (last_start - buffer));
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
	ret = sunxi_secstorage_flush();
	secure_storage_inited = 0;

	return ret;
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

		pr_msg("name in map %s\n", name);
		len = simple_strtoul((const char *)length, NULL, 10);

		if (strlen(name) == strlen(SECURE_STORAGE_DUMMY_KEY_NAME) &&
		    !memcmp(name, SECURE_STORAGE_DUMMY_KEY_NAME,
			    strlen(SECURE_STORAGE_DUMMY_KEY_NAME)) &&
		    len == 0) {
			/*dummy key, not used, goto next key*/
			goto next_key;
		}

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

next_key:
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
