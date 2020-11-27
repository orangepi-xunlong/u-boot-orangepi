#ifndef __DTBFAST_H_
#define __DTBFAST_H_

#include <linux/types.h>
#include <sys_config.h>

#define  DTBFAST_HEAD_MAX_DEPTH       (16)


struct dtbfast_header {
	uint32_t magic;			 /* magic word FDT_MAGIC */
	uint32_t totalsize;		 /* total file size */
	uint32_t level0_count;	 /* the count of level0 head */
	uint32_t off_head;    	 /* offset to head */
	uint32_t head_count;		 /* total head */
	uint32_t off_prop;		 /* offset to prop */
	uint32_t prop_count;		 /* total prop */
	uint32_t reserved[9];

};


struct head_node {
	uint32_t  name_sum;		//完整名称的每个字符的累加和
	uint32_t  name_sum_short; //不计算@之后字符的累加和
	uint32_t  name_offset;	//具体名称的偏移量，从dtb中寻找
	uint32_t  name_bytes;		//完整名称的长度
	uint32_t  name_bytes_short;//@之前名称的长度
	uint32_t  repeate_count;
	uint32_t  head_offset;	//指向第一个head的offset
	uint32_t  head_count;		//head总的个数
	uint32_t  data_offset;	//指向第一个prop的offset
	uint32_t  data_count;		//prop总的个数
	uint32_t  reserved[2];
};

struct prop_node {
	uint32_t  name_sum;		//名称的每个字符的累加和
	uint32_t  name_offset;	//具体名称的偏移量，dtb中寻找
	uint32_t  name_bytes;		//具体名称的长度
	uint32_t  offset;			//具体prop的偏移量，dtb中寻找
};


int fdtfast_path_offset(const void *fdt, const char *path);

int fdtfast_set_node_status(void *fdt, int nodeoffset, enum fdt_status status, unsigned int error_code);

int fdtfast_setprop_u32(void *fdt, int nodeoffset, const char *name,
				  uint32_t val);

int fdtfast_getprop_u32(const void *fdt, int nodeoffset,
			const char *name, uint32_t *val);

int fdtfast_getprop_string(const void *fdt, int nodeoffset,
			const char *name, char **val);

int fdtfast_getprop_gpio(const void *fdt, int nodeoffset,
		const char* prop_name,	user_gpio_set_t* gpio_list);

#endif /* __DTBFAST_H_ */
