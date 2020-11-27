#ifndef __KEY_H__
#define __KEY_H__

#include "sid_reg.h"

typedef struct EFUSE_KEY
{
	char name[32];							// key名称
	int key_index;							// 地址索引
	int store_max_bit;					// 允许被烧录的最大bit
	int show_bit_offset;				// key是否允许读
	int burned_bit_offset;			// key是否已经烧录了
	int reserve[4];
}
efuse_key_map_t;


efuse_key_map_t key_imformatiom[] =
{
	{"rssk", EFUSE_RSSK, SID_RSSK_SIZE, -1, SCC_RSSK_BURNED_FLAG, {0}},
	{"ssk", EFUSE_SSK, SID_SSK_SIZE, SCC_SSK_DONTSHOW_FLAG, SCC_SSK_BURNED_FLAG, {0}},
	{"rotpk", EFUSE_ROTPK, SID_ROTPK_SIZE, SCC_ROTPK_DONTSHOW_FLAG, SCC_ROTPK_BURNED_FLAG, {0}},
	{"hdcphash", EFUSE_HDCP_HASH, SID_HDCP_HASH_SIZE, -1, SCC_HDCP_HASH_BURNED_FLAG, {0}},
	{{0} , 0, 0, 0, 0,{0}}
};


#endif

