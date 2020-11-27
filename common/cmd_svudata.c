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
#include <sunxi_board.h>

#define MAGIC		"sunxi"
#define PART_NAME	"private"
#define USER_DATA_MAXSIZE				(8 * 1024)
#define USER_DATA_PARAMETER_MAX_COUNT	(30)
#define ONCE_DEAL_LEN (1024*1024)
#define SECTOR_SIZE (512)

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
#ifdef CONFIG_SUNXI_PRIVATE_KEY
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

int erase_all_private_data(void)
{
	u32 flash_start, flash_sectors;
	u32 part_size, len;
	int i, ret = 0, count;
	u8 *fill_zero;

	part_size = sunxi_partition_get_size_byname(PART_NAME);
	if (part_size <= 0)
		return -1;

	flash_start = sunxi_partition_get_offset_byname(PART_NAME);

	if (part_size * SECTOR_SIZE < ONCE_DEAL_LEN)
		len = part_size * SECTOR_SIZE;
	else
		len = ONCE_DEAL_LEN;

	count = (part_size * SECTOR_SIZE) / len;

	fill_zero = (u8 *)malloc(len);
	if(fill_zero == NULL)
		return -1;

	memset(fill_zero , 0x0, len);
	flash_sectors = len / SECTOR_SIZE;
	for(i = 0; i < count ; i++) {
		if(!sunxi_sprite_write(flash_start + i * flash_sectors,
				flash_sectors, (void *)fill_zero)) {
				pr_error("erase private key err: from 0x%x, sectors 0x%x failed\n",
				flash_start + i * flash_sectors, flash_sectors);
			ret = -1;
			goto erase_err;
		}
	}
	pr_msg("erase_private_data success\n");

erase_err:
	free(fill_zero);

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


int do_save_user_data (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{

	if (argc < 3) {
		printf("usage: saveud <name> <data>\n");
		return 0;
	}
	if (argc == 3) {
		save_user_private_data(argv[1], argv[2], strnlen(argv[2], VALUE_SIZE));
	}

	return 0;
}

U_BOOT_CMD(
	save_userdata,	3,	1,	do_save_user_data,
	"save user data",
	"<name> <data>\n"
);


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
	"erase user data",
	"<command>\n"
);
#endif

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

int do_probe_secure_storage(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char *cmd, *name;

	cmd = argv[1];
	name = argv[2];

	if(!strcmp(cmd, "read"))
	{
		if(argc == 2)
		{
			return sunxi_secure_storage_list();
		}
		if(argc == 3)
		{
			char buffer[4096];
			int ret, data_len;

			memset(buffer, 0, 4096);
			ret = sunxi_secure_storage_init();
			if(ret < 0)
			{
				printf("%s secure storage init err\n", __func__);

				return -1;
			}
			ret = sunxi_secure_object_read(name, buffer, 4096, &data_len);
			if(ret < 0)
			{
				printf("private data %s is not exist\n", name);

				return -1;
			}
			printf("private data:\n");
			sunxi_dump(buffer, strlen((const char *)buffer));

			return 0;
		}
	}
	else if(!strcmp(cmd, "erase"))
	{
		int ret;

		ret = sunxi_secure_storage_init();
		if(ret < 0)
		{
			printf("%s secure storage init err\n", __func__);

			return -1;
		}

		if(argc == 2)
		{
			ret = sunxi_secure_storage_erase_all();
			if(ret < 0)
			{
				printf("erase secure storage failed\n");
				return -1;
			}
		}
		else if(argc == 3)
		{
			if(!strcmp(name, "key_burned_flag"))
			{
				if(sunxi_secure_storage_erase_data_only("key_burned_flag"))
				{
					printf("erase key_burned_flag failed\n");
				}
			}
		}
		sunxi_secure_storage_exit();

		return 0;
	}
	else if(!strcmp(cmd, "write"))
	{
		if(argc == 4)
			save_user_data_to_secure_storage( argv[2], argv[3]);
		else
			printf("para error!\n");

		return 0;
	}
	return -1;
}

U_BOOT_CMD(
	pst, CONFIG_SYS_MAXARGS, 1, do_probe_secure_storage,
	"read data from secure storage"
	"erase flag in secure storage",
	"pst read|erase [name]\n"
	"pst read,  then dump all data\n"
	"pst read name,  then dump the dedicate data\n"
	"pst write name, string, write the name = string\n"
	"pst erase,  then erase all secure storage data\n"
	"pst erase key_burned_flag,  then erase the dedicate data\n"
	"NULL"
);
#endif


#ifdef CONFIG_SUNXI_USER_KEY

int update_user_data(void)
{
	if(uboot_spare_head.boot_data.work_mode != WORK_MODE_BOOT)
	{
		return 0;
	}

	check_user_data();											//从env中检测用户的环境变量

#if defined(CONFIG_SUNXI_SECURE_STORAGE)
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
#elif defined(CONFIG_SUNXI_PRIVATE_KEY)

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
#endif
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
	if((get_boot_storage_type() == STORAGE_SD) || (get_boot_storage_type() == STORAGE_EMMC))
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
			for (j = 0; j < sizeof(IGNORE_ENV_VARIABLE) / sizeof(IGNORE_ENV_VARIABLE[0]); j++) {
				if (!strcmp(IGNORE_ENV_VARIABLE[j], temp_name)) {			//查词典库，排除系统的环境变量，得到用户的数据
					break;
				}
			}
			if (j >= sizeof(IGNORE_ENV_VARIABLE) / sizeof(IGNORE_ENV_VARIABLE[0])) {
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

#endif

