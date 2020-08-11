#ifndef __VIDEO_MISC_HAL_H__
#define __VIDEO_MISC_HAL_H__

int get_disp_fdt_node(void);
void disp_getprop_by_name(int node, const char *name,
	unsigned int *value, unsigned int defval);
int hal_save_int_to_kernel(char *name, int value);
int hal_save_string_to_kernel(char *name, char *str);
int hal_get_disp_device_config(int type, void *config);
int hal_save_disp_device_config_to_kernel(int disp, void *from);

int hal_fat_fsload(char *part_name, char *file_name,
	char *load_addr, unsigned long length);

#endif /* #ifndef __VIDEO_MISC_HAL_H__ */
