#ifndef SYS_CONFIG_OLD_H_
#define SYS_CONFIG_OLD_H_

#include <sunxi_cfg.h>

#define   SCRIPT_PARSER_OK                   (0)
#define   SCRIPT_PARSER_EMPTY_BUFFER         (-1)
#define   SCRIPT_PARSER_KEYNAME_NULL         (-2)
#define   SCRIPT_PARSER_DATA_VALUE_NULL      (-3)
#define   SCRIPT_PARSER_KEY_NOT_FIND         (-4)
#define   SCRIPT_PARSER_BUFFER_NOT_ENOUGH    (-5)
#define   SCRIPT_PARSER_PARA_INVALID         (-6)

typedef enum
{
	SCIRPT_PARSER_VALUE_TYPE_INVALID = 0,
	SCIRPT_PARSER_VALUE_TYPE_SINGLE_WORD,
	SCIRPT_PARSER_VALUE_TYPE_STRING,
	SCIRPT_PARSER_VALUE_TYPE_MULTI_WORD,
	SCIRPT_PARSER_VALUE_TYPE_GPIO_WORD
} script_parser_value_type_t;

typedef struct
{
	unsigned  main_key_count;
	unsigned  length;
	unsigned  version[2];
} script_head_t;

typedef struct
{
	char main_name[32];
	int  lenth;
	int  offset;
} script_main_key_t;

typedef struct
{
	char sub_name[32];
	int  offset;
	int  pattern;
} script_sub_key_t;

typedef struct
{
	char *script_mod_buf;
	ulong script_main_key_count;
	ulong script_reloc_buf;
	ulong script_reloc_size;

	ulong (*parser_subkey)( char *script_mod_buf,ulong script_main_key_count, script_main_key_t* main_name,char *subkey_name , uint *pattern);
	ulong (*parser_fetch_subkey_start)(char *script_mod_buf,ulong script_main_key_count, char *main_name);
	int (*parser_fetch_subkey_next)(char *script_mod_buf, ulong hd, char *sub_name, int value[], int *index);
	int (*parser_fetch)(char *script_mod_buf, ulong  script_main_key_count,char *main_name, char *sub_name, int value[], int count);
	int (*parser_fetch_ex)(char *script_mod_buf,ulong script_main_key_count,char *main_name,
								char *sub_name, int value[], script_parser_value_type_t *type, int count);
	int (*parser_patch_all)(char *script_mod_buf,ulong script_main_key_count,char *main_name, void *str, uint data_count);
	int(*parser_patch)(char *script_mod_buf,ulong script_main_key_count,char *main_name, char *sub_name, void *str, int str_size);
	int (*parser_mainkey_get_gpio_count)(char *script_mod_buf,ulong script_main_key_count,char *main_name);
	int (*parser_mainkey_get_gpio_cfg)(char *script_mod_buf,ulong script_main_key_count,char *main_name, void *gpio_cfg, int gpio_count);
	ulong (*parser_offset)(char *script_mod_buf, ulong script_main_key_count,char *main_name);
	int (*parser_fetch_by_offset)(char * script_mod_buf,ulong script_main_key_count,ulong offset, char *sub_name, uint32_t value[]);
	int (*probe_gpio_function)(char *script_mod_buf,ulong script_main_key_count,char *main_name, void *gpio_cfg, int gpio_count);
} script_op_t;

/* functions for early boot */
extern int sw_cfg_get_int(const char *script_buf, const char *main_key, const char *sub_key);
extern char *sw_cfg_get_str(const char *script_buf, const char *main_key, const char *sub_key, char *buf);

/* script operations */
extern int script_parser_init(char *soc_script_buf,char *board_script_buf);
extern int script_parser_exit(void);
extern unsigned script_get_length(char *buffer);
extern ulong script_parser_fetch_subkey_start(char *main_name);
extern int script_parser_fetch_subkey_next(char *main_name, char *sub_name, int value[], int *index1, int *index2);
extern int script_parser_fetch(char *main_name, char *sub_name, int value[], int count);
extern int script_parser_fetch_ex(char *main_name, char *sub_name, int value[],
               script_parser_value_type_t *type, int count);
extern int script_parser_patch(char *main_name, char *sub_name, void *str, int str_size);
extern int script_parser_subkey_count(int script_no,char *main_name);
extern int script_parser_mainkey_count(int script_no);
extern int script_parser_mainkey_get_gpio_count(char *main_name);
extern int script_parser_mainkey_get_gpio_cfg(char *main_name, void *gpio_cfg, int gpio_count);

extern ulong script_parser_subkey( script_main_key_t* main_name,char *subkey_name , uint *pattern);

extern int script_parser_patch_all(char *main_name, void *str, uint data_count);

extern ulong script_parser_offset(char *main_name);
extern int script_parser_fetch_by_offset(int offset, char *sub_name, uint32_t value[]);

extern void script_parser_func_init(void);
extern void set_script_reloc_buf(int script_no,ulong addr);
extern void set_script_reloc_size(int script_no,ulong size);
extern ulong get_script_reloc_buf(int script_no);
extern ulong get_script_reloc_size(int script_no);
extern int soc_script_parser_init(char *script_buf);
extern int bd_script_parser_init(char *script_buf);

extern int gpio_request_by_function(char *main_name, const char *sub_name);

#endif
