/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Char <yanjianbo@allwinnertech.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <boot_type.h>
#include <sunxi_mbr.h>
#include <malloc.h>
#include <sys_partition.h>
#include <securestorage.h>

#define MAGIC		"sunxi"
#define PART_NAME	"private"
#define USER_DATA_MAXSIZE				(8 * 1024)
#define USER_DATA_PARAMETER_MAX_COUNT	(30)


#define	NAME_SIZE	32
#define VALUE_SIZE	128

typedef struct {
	char magic_name[8];
	int count;
	int reserved[3];
}USER_DATA_HEAR;

typedef struct {
	char name[NAME_SIZE];
	char value[VALUE_SIZE];
	int valid;					//0: 有效  1：无效（不被更新
	int reserved[3];
}USER_PRIVATE_DATA;

char *IGNORE_ENV_VARIABLE[] = { "console",
								"root",
								"init",
								"loglevel",
								"partitions",
								"vmalloc",
								"earlyprintk",
								"ion_reserve",
								"enforcing",
								"cma",
								"initcall_debug",
							};

int USER_DATA_NUM;									//用户的环境变量个擿
char USER_DATA_NAME[10][NAME_SIZE] = {{'\0'}};		//用户的环境变量（从env.fex获取ﺿ

void check_user_data(void);

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
int save_user_private_data(char *name, char *buffer, int length)
{
	int j;
	unsigned int part_offset;					//分区的地址偏移酿
	unsigned int part_size;						//分区的大宿
	unsigned int user_data_offset;				//用户存放数据地址的偏移量
	char user_data_buffer[USER_DATA_MAXSIZE] = {0};	//
	char data[VALUE_SIZE];
	USER_PRIVATE_DATA *user_data_p = NULL;
	USER_DATA_HEAR *user_data_head = NULL;

	printf("save data to private\n");
	if (!name || !buffer) {
		printf("error: the name (data) is null\n");
		return -1;
	}

	if( length > (VALUE_SIZE - 1))
	{
		printf("the data size is large then %d, cant write\n", VALUE_SIZE - 1);
		return 1;
	}

	memset(data, 0, VALUE_SIZE);
	memcpy(data, buffer, length);

	printf("data=%s\n", data);
	part_size = sunxi_partition_get_size_byname(PART_NAME);
	if (part_size > 0)
	{
		part_offset = sunxi_partition_get_offset_byname(PART_NAME);
		user_data_offset = part_offset + part_size - (USER_DATA_MAXSIZE >> 9);		//获得用户存放数据地址的偏移量
		if (!sunxi_flash_read(user_data_offset, USER_DATA_MAXSIZE >> 9, user_data_buffer)) {
			printf("read flash error\n");
			return -1;
		}

		user_data_head = (USER_DATA_HEAR *)user_data_buffer;
		user_data_p = (USER_PRIVATE_DATA *)(user_data_buffer + sizeof(USER_DATA_HEAR));
		if (strncmp(user_data_head->magic_name, MAGIC, 5)) {
			memset(user_data_buffer, 0xff, USER_DATA_MAXSIZE);
			strcpy(user_data_head->magic_name, MAGIC);
			user_data_head->count = 0;
			printf("init the (user) private space\n");
		}

		if (user_data_head->count > 0) {
			for (j = 0; j < user_data_head->count && j < USER_DATA_PARAMETER_MAX_COUNT; j++) {
				if (!strcmp(name, user_data_p->name)) {					//匹配数据项名祿
					strcpy(user_data_p->value, data);					//更新数据
					user_data_p->valid = 1;							    //更新有效便
					printf("Saving Environment to \n");
					break;
				}
				user_data_p++;
			}
			if (j == user_data_head->count) {
					strcpy(user_data_p->name, name);					//在尾巴增加新的数拿
					strcpy(user_data_p->value, data);
					user_data_p->valid = 1;
					user_data_head->count++;
					printf("Saving Environment to \n");
			}
		}
		else {
			strcpy(user_data_p->name, name);					//第一个数拿
			strcpy(user_data_p->value, data);
			user_data_p->valid = 1;
			user_data_head->count++;
			printf("Saving Environment to \n");
		}

		sunxi_flash_write(user_data_offset, USER_DATA_MAXSIZE >> 9, user_data_buffer);	//回写到flash 䶿
		sunxi_flash_flush();

		return 0;
	}
	printf("the part isn't exist\n");
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
int erase_all_private_data(void)
{
	int count = 0;
	unsigned int flash_start = 0, flash_sectors = 0;;					//分区的地址偏移酿
	unsigned int part_size;						//分区的大宿
	int i = 0 , len = 1024 * 1024;
	int ret = 0;
	char *fill_zero = NULL;

	part_size = sunxi_partition_get_size_byname(PART_NAME);
	if (part_size <= 0) {
		return -1;
	}

	flash_start = sunxi_partition_get_offset_byname(PART_NAME);
	count = part_size / 2048;

	fill_zero = (char *)malloc(len);
	if(fill_zero == NULL)
	{
		printf("no enough memory to malloc \n");
		return -1;
	}

	memset(fill_zero , 0x0, len);
	flash_sectors = len / 512;
	for(i = 0; i < count ; i++)
	{
		if(!sunxi_sprite_write(flash_start + i * flash_sectors, flash_sectors, (void *)fill_zero))
		{
			printf("sunxi_sprite_erase_private_key err: write flash from 0x%x, sectors 0x%x failed\n", flash_start + i * flash_sectors, flash_sectors);
			ret = -1;
			goto erase_err;
		}
	}

	sunxi_flash_flush();
	printf("erase_private_data success\n");

erase_err:
	if(fill_zero)
	{
		free(fill_zero);
	}

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
int read_private_key_by_name(const char * name, char *buffer, int buffer_len, int *data_len)
{
	int j;
	unsigned int part_offset;					//分区的地址偏移酿
	unsigned int part_size;						//分区的大宿
	unsigned int user_data_offset;				//用户存放数据地址的偏移量
	char user_data_buffer[USER_DATA_MAXSIZE];	//
	USER_PRIVATE_DATA *user_data_p = NULL;
	USER_DATA_HEAR *user_data_head = NULL;

	printf("read private data\n");
	part_size = sunxi_partition_get_size_byname(PART_NAME);
	if (part_size > 0) {
		part_offset = sunxi_partition_get_offset_byname(PART_NAME);

		user_data_offset = part_offset + part_size - (USER_DATA_MAXSIZE >> 9);		//获得用户存放数据地址的偏移量
		if (!sunxi_flash_read(user_data_offset, USER_DATA_MAXSIZE >> 9, user_data_buffer)) {
			printf("read flash error\n");
			return -1;
		}

		user_data_head = (USER_DATA_HEAR *)user_data_buffer;
		user_data_p = (USER_PRIVATE_DATA *)(user_data_buffer + sizeof(USER_DATA_HEAR));
		
		if (strncmp(user_data_head->magic_name, MAGIC, 5)) {
			printf("private maybe empty\n");
			return -1;
		}

		if (user_data_head->count > 0) {
			printf("count = %d\n", user_data_head->count);
			for (j = 0; j < user_data_head->count; j++) {
				if(!strcmp(user_data_p->name, name))
				{
					strcpy(buffer, user_data_p->value);
					*data_len = strnlen(user_data_p->value, VALUE_SIZE);
					printf("%s = %s\n", user_data_p->name, user_data_p->value);
					return 0;
				}
				user_data_p++;
			}
		}
		printf("have not data\n");
		return -1;

	}
	printf("the part isn't exist\n");
	return -1;
}


#ifdef CONFIG_SUNXI_SECURE_STORAGE
static int save_user_data_to_secure_storage(const char * name, char *data)
{
	char buffer[512];
	int  data_len;
	int  ret;
    memset(buffer,0x00,sizeof(buffer));
	printf("Also save user data %s to secure storage\n", (char*)name);
	if(sunxi_secure_storage_init()){
		printf("secure storage init fail\n");
	}else{
		ret = sunxi_secure_object_read("key_burned_flag", buffer, 512, &data_len);
		if(ret)
		{
			printf("sunxi secure storage has no flag\n");
		}
		else
		{
			if(!strcmp(buffer, "key_burned"))
			{
				printf("find key burned flag\n");
				return 0;
			}
			printf("do not find key burned flag\n");
		}
		sunxi_secure_object_write(name, data, strnlen(data, 512));	
		sunxi_secure_storage_exit();
	}
	return 0 ;
}
#endif

int do_save_user_data (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int old_way = 1;//default old version
	if (argc == 5) {//support livecom 2.1
		int value_length = strlen(argv[2]);
		int value_crc32 = simple_strtol(argv[4],NULL,16);
		int real_crc32 = crc32(0,(unsigned char *)argv[2],value_length);
		printf("real_crc32 = %08x\n",real_crc32);
		printf("orignal crc32 = %08x\n",value_crc32);
		if(value_crc32 != real_crc32)
		{
			printf("save_userdata:crc32 failed\n");
			return -1;
		}
		old_way = 0;//livecom 2.1
	}
	else if(argc ==3)
	{
	   //do nothing
	}
	else
	{
	    printf("wrong usage format\n");
	    printf("old_way: save_userdata key_name key_value\n");
	    printf("new_way: save_userdata key_name key_value flash/efuse crc32\n");
		return -1;
	}
	//livecom 2.1 and old version burn key to flash
	if(old_way == 1 || ((old_way == 0) && strncmp(argv[3],"flash",5)==0))
	{
		#ifdef CONFIG_SUNXI_PRIVATE_KEY
			save_user_private_data(argv[1], argv[2], strnlen(argv[2], VALUE_SIZE));
		#endif

		#ifdef CONFIG_SUNXI_SECURE_STORAGE
			save_user_data_to_secure_storage( argv[1], argv[2]);
		#endif
	}//livecom 2.1 support burn key to efuse
    else if(old_way == 0 && strncmp(argv[3],"efuse",5)==0)
	{
		printf("no item need to burn efuse,this is for future\n");
	}
	return 0;
}

U_BOOT_CMD(
	save_userdata,	5,	1,	do_save_user_data,
	"save user data",
	"<name> <data>\n"
);

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
static int user_data_list(void)
{
	int j;
	unsigned int part_offset;					//分区的地址偏移酿
	unsigned int part_size;						//分区的大宿
	unsigned int user_data_offset;				//用户存放数据地址的偏移量
	char user_data_buffer[USER_DATA_MAXSIZE];	//
	USER_PRIVATE_DATA *user_data_p = NULL;
	USER_DATA_HEAR *user_data_head = NULL;

	printf("user_data_list\n");
	part_size = sunxi_partition_get_size_byname(PART_NAME);
	if (part_size > 0) {
		part_offset = sunxi_partition_get_offset_byname(PART_NAME);

		user_data_offset = part_offset + part_size - (USER_DATA_MAXSIZE >> 9);		//获得用户存放数据地址的偏移量
		if (!sunxi_flash_read(user_data_offset, USER_DATA_MAXSIZE >> 9, user_data_buffer)) {
			printf("read flash error\n");
			return 0;
		}

		user_data_head = (USER_DATA_HEAR *)user_data_buffer;
		user_data_p = (USER_PRIVATE_DATA *)(user_data_buffer + sizeof(USER_DATA_HEAR));
		
		if (strncmp(user_data_head->magic_name, MAGIC, 5)) {
			printf("the (user) private space\n");
			return 0;
		}
		
		if (user_data_head->count > 0) {
			printf("count = %d\n", user_data_head->count);
			for (j = 0; j < user_data_head->count; j++) {
				printf("%s = %s\n", user_data_p->name, user_data_p->value);
				user_data_p++;
			}
			return 0;
		}
		printf("have not data\n");
		return 0;

	}
	printf("the part isn't exist\n");
	return 0;
}

int do_check_userdata (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	if (argc > 1)
	{
		printf("error: <command>\n");
		return 0;
	}
	user_data_list();
	return 0;
}

U_BOOT_CMD(
	check_userdata,	1,	1,	do_check_userdata,
	"check user data",
	"<command>\n"
);

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
int erase_private_data(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	if(argc > 1)
	{
		printf("error: <command>\n");
		return 0;
	}

	erase_all_private_data();
	return 0;
}

U_BOOT_CMD(
	erase_userdata,	1,	1,	erase_private_data,
	"check user data",
	"<command>\n"
);
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
int update_user_data(void)
{
	if(uboot_spare_head.boot_data.work_mode != WORK_MODE_BOOT)
	{
		return 0;
	}

	check_user_data();											//从env中检测用户的环境变量

#ifdef CONFIG_SUNXI_SECURE_STORAGE
	int data_len;
	int ret, k;
	char buffer[512];
	int updata_data_num = 0;

	if(!sunxi_secure_storage_init())
	{	
		memset(buffer, 0, 512);
		for (k = 0; k < USER_DATA_NUM; k++) 
		{
			ret = sunxi_secure_object_read(USER_DATA_NAME[k], buffer, 512, &data_len);
			if(!ret && data_len < 512) 
			{
				//sunxi_dump(buffer, data_len);
				setenv(USER_DATA_NAME[k], buffer);
				printf("update %s = %s\n", USER_DATA_NAME[k], buffer);					
				memset(buffer, 0, 512);
				updata_data_num++;
				strcpy(USER_DATA_NAME[k], "\0");
			}
		}
	}
#endif

	int i, j;
	unsigned int part_offset;					//分区的地址偏移酿
	unsigned int part_size;						//分区的大宿
	unsigned int user_data_offset;				//用户存放数据地址的偏移量
	char user_data_buffer[USER_DATA_MAXSIZE];	//
	USER_PRIVATE_DATA *user_data_p = NULL;
	USER_DATA_HEAR *user_data_head = NULL;

	part_size = sunxi_partition_get_size_byname(PART_NAME);
	printf("check user data form private\n");
	if (part_size > 0) {
		part_offset = sunxi_partition_get_offset_byname(PART_NAME);
		user_data_offset = part_offset + part_size - (USER_DATA_MAXSIZE >> 9);		//获得用户存放数据地址的偏移量
		if (!sunxi_flash_read(user_data_offset, USER_DATA_MAXSIZE >> 9, user_data_buffer)) {
			printf("read flash error\n");
			return 0;
		}
		
		user_data_head = (USER_DATA_HEAR *)user_data_buffer;
		user_data_p = (USER_PRIVATE_DATA *)(user_data_buffer + sizeof(USER_DATA_HEAR));
	
		if (strncmp(user_data_head->magic_name, MAGIC, 5)) { 				//校验数据是否有效
			printf("the user data'magic is bad\n");
			return 0;
		}

		if (user_data_head->count > 0) {
			for (i = 0; i < USER_DATA_NUM; i++) {
				user_data_p = (USER_PRIVATE_DATA *)(user_data_buffer + sizeof(USER_DATA_HEAR));
				for (j = 0; j < user_data_head->count && j < USER_DATA_PARAMETER_MAX_COUNT; j++) {
					if (!strcmp(USER_DATA_NAME[i], user_data_p->name))
					{
						setenv(user_data_p->name, user_data_p->value);
						printf("updataed %s = %s\n", user_data_p->name, user_data_p->value);
					}
					user_data_p++;
				}
			}
			return 0;
		}
		printf("not the user data to updata\n");
		return 0;
	}
	printf("the %s part isn't exist\n", PART_NAME);
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
*    note          :		 得到用户的环境变酿
*
*
************************************************************************************************************
*/
void check_user_data(void)
{
	char *command_p  =NULL;
	char temp_name[32] = {'\0'};
	int i, j;

	//command_p = getenv("boot_base");  //合并nand和mmc启动参数后的方法，暂时不璿
	if((uboot_spare_head.boot_data.storage_type == 1) || (uboot_spare_head.boot_data.storage_type == 2))
	{
		command_p = getenv("setargs_mmc");
	}
	else
	{
		command_p = getenv("setargs_nand");
	}
	//printf("cmd line = %s\n", command_p);
	if (!command_p) {
		printf("cann't get the boot_base from the env\n");
		return ;
	}

	while (*command_p != '\0' && *command_p != ' ') {		//过滤第一个环境变酿
		command_p++;
	}
	command_p++;
	while (*command_p == ' ') {								//过滤多余的空枿
		command_p++;
	}
	while (*command_p != '\0' && *command_p != ' ') { 		//过滤第二个环境变酿
		command_p++;
	}
	command_p++;
	while (*command_p == ' ') {
		command_p++;
	}

	USER_DATA_NUM =0;
	while (*command_p != '\0') {
		i = 0;
		while (*command_p != '=') {
			temp_name[i++] = *command_p;
			command_p++;
		}
		temp_name[i] = '\0';
		if (i != 0) {
			for (j = 0; j < sizeof(IGNORE_ENV_VARIABLE) / sizeof(int); j++) {
				if (!strcmp(IGNORE_ENV_VARIABLE[j], temp_name)) {			//查词典库，排除系统的环境变量，得到用户的数据
					break;
				}
			}
			if (j >= sizeof(IGNORE_ENV_VARIABLE) / sizeof(int)) {
				if (!strcmp(temp_name, "mac_addr")) {						//处理mac_addr和mac不相等的情况（特殊情况）
					strcpy(USER_DATA_NAME[USER_DATA_NUM], "mac");
				}
				else {
					strcpy(USER_DATA_NAME[USER_DATA_NUM], temp_name);
				}
				USER_DATA_NUM++;
			}
		}
		while (*command_p != '\0' && *command_p != ' ') {					//下一个变酿
			command_p++;
		}
		while (*command_p == ' ') {
			command_p++;
		}
	}
/*
	printf("USER_DATA_NUM = %d\n", USER_DATA_NUM);
	for (i = 0; i < USER_DATA_NUM; i++) {
		printf("user data = %s\n", USER_DATA_NAME[i]);
	}
*/
}

