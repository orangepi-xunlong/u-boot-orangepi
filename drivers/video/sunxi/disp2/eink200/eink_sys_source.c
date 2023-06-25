/*
 * Allwinner SoCs eink driver.
 *
 * Copyright (C) 2019 Allwinner.
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include "asm/io.h"

#include "include/eink_sys_source.h"
#include "include/eink_driver.h"

#define BYTE_ALIGN(x) (((x + (4 * 1024 - 1)) >> 12) << 12)

extern struct eink_driver_info g_eink_drvdata;

typedef struct __eink_node_map {
	char node_name[16];
	int  node_offset;
} eink_fdt_node_map_t;

static eink_fdt_node_map_t g_eink_fdt_node_map[] = {
	{FDT_EINK_PATH, -1},
#ifdef CONFIG_PMIC_TPS65185
	{FDT_TPS65185_PATH, -1},
	{FDT_TPS65185_SLV_PATH, -1},
#endif
	{"", -1}
};

void eink_fdt_init(void)
{
	int i = 0;

	while (strlen(g_eink_fdt_node_map[i].node_name)) {
		g_eink_fdt_node_map[i].node_offset =
			fdt_path_offset(working_fdt, g_eink_fdt_node_map[i].node_name);
		i++;
	}
}

int eink_fdt_nodeoffset(char *main_name)
{
	int i = 0;

	for (i = 0; ; i++) {
		if (strcmp(g_eink_fdt_node_map[i].node_name, main_name) == 0) {
			return g_eink_fdt_node_map[i].node_offset;
		}
		if (strlen(g_eink_fdt_node_map[i].node_name) == 0) {
			return -1;
		}
	}
	return -1;
}

uintptr_t eink_getprop_regbase(char *main_name, char *sub_name, u32 index)
{
	char compat[32];
	u32 len = 0;
	int node;
	int ret = -1;
	int value[32] = {0};
	uintptr_t reg_base = 0;

	len = sprintf(compat, "%s", main_name);
	if (len > 32)
		pr_warn("size of mian_name is out of range\n");

	node = eink_fdt_nodeoffset(compat);
	if (node < 0) {
		pr_warn("fdt_path_offset %s fail\n", compat);
		goto exit;
	}

	ret = fdt_getprop_u32(working_fdt, node, sub_name, (uint32_t *)value);
	if (ret < 0)
		pr_warn("fdt_getprop_u32 %s.%s fail\n", main_name, sub_name);
	else {
		reg_base = value[index * 4] + value[index * 4 + 1];
	}

exit:
	return reg_base;
}

u32 eink_getprop_irq(char *main_name, char *sub_name, u32 index)
{
	char compat[32];
	u32 len = 0;
	int node;
	int ret = -1;
	int value[32] = {0};
	u32 irq = 0;

	len = sprintf(compat, "%s", main_name);
	if (len > 32)
		pr_warn("size of mian_name is out of range\n");

	node = eink_fdt_nodeoffset(compat);
	if (node < 0) {
		pr_warn("fdt_path_offset %s fail\n", compat);
		goto exit;
	}

	ret = fdt_getprop_u32(working_fdt, node, sub_name, (uint32_t *)value);
	if (ret < 0)
		pr_warn("fdt_getprop_u32 %s.%s fail\n", main_name, sub_name);
	else {
		irq = value[index * 3 + 1];
		if (value[index * 3] == 0)
			irq += 32;
	}

exit:
	return irq;
}

bool is_upd_win_zero(struct upd_win update_area)
{
	if ((update_area.left == 0) && (update_area.right == 0) && (update_area.top == 0) && (update_area.bottom == 0))
		return true;
	else
		return false;
}

int eink_sys_pin_set_state(char *dev_name, char *name)
{
	int ret = -1;

	if (!strcmp(name, EINK_PIN_STATE_ACTIVE))
		ret = fdt_set_all_pin("eink", "pinctrl-0");
	else
		ret = fdt_set_all_pin("eink_suspend", "pinctrl-1");

	if (ret != 0)
		printf("%s, fdt_set_all_pin, ret=%d\n", __func__, ret);

	return ret;
}
EXPORT_SYMBOL(eink_sys_pin_set_state);

int eink_sys_power_enable(char *name)
{
	int ret = 0;

	if (strlen(name) == 0) {
		return 0;
	}
#if defined(CONFIG_SUNXI_PMU)
	/*TODO:bmu*/
	ret = pmu_set_voltage(name, 0, 1);
	if (!ret)
		EINK_DEFAULT_MSG("enable eink power %s, ret=%d\n", name, ret);
#endif

	return ret;
}
EXPORT_SYMBOL(eink_sys_power_enable);

int eink_sys_power_disable(char *name)
{
	int ret = 0;

	/*TODO:bmu*/
#if defined(CONFIG_SUNXI_PMU)
	ret = pmu_set_voltage(name, 0, 0);
	if (!ret)
		EINK_DEFAULT_MSG("disable eink power %s, ret=%d\n", name, ret);
#endif
	return ret;
}
EXPORT_SYMBOL(eink_sys_power_disable);

s32 eink_panel_pin_cfg(u32 en)
{
	struct eink_manager *eink_mgr = get_eink_manager();
	char dev_name[25];

	if (!eink_mgr) {
		pr_err("NULL hdl!\n");
		return -1;
	}
	EINK_INFO_MSG("eink pin config, state %s, %d\n", (en) ? "on" : "off", en);

	/* io-pad */
	if (en == 1) {
		if (!((!strcmp(eink_mgr->eink_pin_power, "")) ||
		      (!strcmp(eink_mgr->eink_pin_power, "none"))))
			eink_sys_power_enable(eink_mgr->eink_pin_power);
	}

	sprintf(dev_name, "eink");
	eink_sys_pin_set_state(dev_name, (en == 1) ?
			EINK_PIN_STATE_ACTIVE : EINK_PIN_STATE_SLEEP);

	if (en == 0) {
		if (!((!strcmp(eink_mgr->eink_pin_power, "")) ||
		      (!strcmp(eink_mgr->eink_pin_power, "none"))))
			eink_sys_power_disable(eink_mgr->eink_pin_power);
	}

	return 0;
}

int eink_sys_gpio_request(struct eink_gpio_cfg *gpio_list,
			  u32 group_count_max)
{
	user_gpio_set_t gpio_info;

	gpio_info.port = gpio_list->port;
	gpio_info.port_num = gpio_list->port_num;
	gpio_info.mul_sel = gpio_list->mul_sel;
	gpio_info.drv_level = gpio_list->drv_level;
	gpio_info.data = gpio_list->data;

	EINK_INFO_MSG("eink_sys_gpio_request, port:%d, port_num:%d, mul_sel:%d, pull:%d, drv_level:%d, data:%d\n",
		      gpio_list->port, gpio_list->port_num, gpio_list->mul_sel,
	      gpio_list->pull, gpio_list->drv_level, gpio_list->data);

	return sunxi_gpio_request(&gpio_info, group_count_max);
}
EXPORT_SYMBOL(eink_sys_gpio_request);

int eink_sys_gpio_request_simple(struct eink_gpio_cfg *gpio_list, u32 group_count_max)
{
	int ret = 0;
	user_gpio_set_t gpio_info;

	gpio_info.port = gpio_list->port;
	gpio_info.port_num = gpio_list->port_num;
	gpio_info.mul_sel = gpio_list->mul_sel;
	gpio_info.drv_level = gpio_list->drv_level;
	gpio_info.data = gpio_list->data;

	pr_info("[%s]:GPIO_Request, port:%d, port_num:%d, mul_sel:%d, "\
		"pull:%d, drv_level:%d, data:%d\n", __func__,
		gpio_list->port, gpio_list->port_num, gpio_list->mul_sel,
		gpio_list->pull, gpio_list->drv_level, gpio_list->data);

	ret = gpio_request_early(&gpio_info, group_count_max, 1);
	return ret;
}

int eink_sys_gpio_release(int p_handler, s32 if_release_to_default_status)
{
	if (p_handler != 0xffff) {
		gpio_release(p_handler, if_release_to_default_status);
	}
	return 0;
}
EXPORT_SYMBOL(eink_sys_gpio_release);

/* direction: 0:input, 1:output */
int eink_sys_gpio_set_direction(u32 p_handler, u32 direction, const char *gpio_name)
{
	return gpio_set_one_pin_io_status(p_handler, direction, gpio_name);
}
EXPORT_SYMBOL(eink_sys_gpio_set_direction);

int eink_sys_gpio_set_value(u32 p_handler, u32 value_to_gpio,
			    const char *gpio_name)
{
	return gpio_write_one_pin_value(p_handler, value_to_gpio, gpio_name);
}

/* type: 0:invalid, 1: int; 2:str, 3: gpio */
int eink_sys_script_get_item(char *main_name, char *sub_name, int value[],
			     int type)
{
	int node;
	int ret = 0;
	user_gpio_set_t  gpio_info;
	struct eink_gpio_cfg  *gpio_list;

	node = eink_fdt_nodeoffset(main_name);
	if (node < 0) {
		printf("fdt get node offset fail: %s\n", main_name);
		return ret;
	}

	if (type == 1) {
		if (fdt_getprop_u32(working_fdt, node, sub_name, (uint32_t *)value) >=
				0)
			ret = type;
	} else if (type == 2) {
		const char *str;

		if (fdt_getprop_string(working_fdt, node, sub_name,
				       (char **)&str) >= 0) {
		  ret = type;
		  memcpy((void *)value, str, strlen(str) + 1);
		}
	} else if (type == 3) {
		if (fdt_get_one_gpio_by_offset(node, sub_name, &gpio_info) >= 0) {
			gpio_list = (struct eink_gpio_cfg *)value;
			gpio_list->port = gpio_info.port;
			gpio_list->port_num = gpio_info.port_num;
			gpio_list->mul_sel = gpio_info.mul_sel;
			gpio_list->drv_level = gpio_info.drv_level;
			gpio_list->pull = gpio_info.pull;
			gpio_list->data = gpio_info.data;

			memcpy(gpio_info.gpio_name, sub_name, strlen(sub_name) + 1);
			debug("%s.%s gpio=%d, mul_sel=%d, data:%d\n",
			      main_name, sub_name, gpio_list->gpio,
					gpio_list->mul_sel, gpio_list->data);
			ret = type;
		}
	}

	return ret;
}
EXPORT_SYMBOL(eink_sys_script_get_item);

#ifdef REGISTER_PRINT
void eink_print_register(unsigned long start_addr, unsigned long end_addr)
{
	unsigned long start_addr_align = ALIGN(start_addr, 4);
	unsigned long end_addr_align = ALIGN(end_addr, 4);
	unsigned long addr = 0;
	char tmp_buf[50] = {0};
	char buf[256] = {0};

	pr_info("print reg: 0x%08x ~ 0x%08x\n", (unsigned int)start_addr_align, (unsigned int)end_addr_align);
	for (addr = start_addr_align; addr <= end_addr_align; addr += 4) {
		if (0 == (addr & 0xf)) {
			memset(tmp_buf, 0, sizeof(tmp_buf));
			memset(buf, 0, sizeof(buf));
			snprintf(tmp_buf, sizeof(tmp_buf), "0x%08x: ", (unsigned int)addr);
			strncat(buf, tmp_buf, sizeof(tmp_buf));
		}

		memset(tmp_buf, 0, sizeof(tmp_buf));
		snprintf(tmp_buf, sizeof(tmp_buf), "0x%08x ", readl(addr));
		strncat(buf, tmp_buf, sizeof(tmp_buf));
		if (0 == ((addr + 4) & 0xf)) {
			pr_info("%s\n", buf);
			mdelay(5);
		}
	}
}
#endif

#ifdef PIPELINE_DEBUG
void print_free_pipe_list(struct pipe_manager *mgr)
{
	struct pipe_info_node *pipe, *tpipe;
	unsigned long flags = 0;

	spin_lock_irqsave(&mgr->list_lock, flags);
	if (list_empty(&mgr->pipe_free_list)) {
		EINK_INFO_MSG("FREE_LIST is empty\n");
		spin_unlock_irqrestore(&mgr->list_lock, flags);
		return;
	}

	list_for_each_entry_safe(pipe, tpipe, &mgr->pipe_free_list, node) {
		if (pipe->pipe_id < 10) {
			EINK_INFO_MSG("FREE_LIST: pipe %02d is free\n", pipe->pipe_id);
		}
	}
	spin_unlock_irqrestore(&mgr->list_lock, flags);
}

void print_used_pipe_list(struct pipe_manager *mgr)
{
	struct pipe_info_node *pipe, *tpipe;
	unsigned long flags = 0;

	spin_lock_irqsave(&mgr->list_lock, flags);
	if (list_empty(&mgr->pipe_used_list)) {
		EINK_INFO_MSG("USED_LIST is empty\n");
		spin_unlock_irqrestore(&mgr->list_lock, flags);
		return;
	}

	list_for_each_entry_safe(pipe, tpipe, &mgr->pipe_used_list, node) {
		if (pipe->pipe_id < 10) {
			EINK_INFO_MSG("USED_LIST: pipe %02d is used\n", pipe->pipe_id);
		}
	}
	spin_unlock_irqrestore(&mgr->list_lock, flags);
}
#endif

int save_as_bin_file(__u8 *buf, char *file_name, __u32 length, loff_t pos)
{
#if 0
	struct file *fp = NULL;
	mm_segment_t old_fs;
	ssize_t ret = 0;

	if ((!buf) || (!file_name)) {
		pr_err("%s: buf or file_name is null\n", __func__);
		return -1;
	}

	fp = filp_open(file_name, O_RDWR | O_CREAT, 0644);
	if (IS_ERR(fp)) {
		pr_err("%s: fail to open file(%s), ret=%d\n",
		       __func__, file_name, *((u32 *)fp));
		return -1;
	}

	old_fs = get_fs();
	set_fs(KERNEL_DS);

	ret = vfs_write(fp, buf, length, &pos);
	EINK_INFO_MSG("%s: save %s done, len=%u, pos=%lld, ret=%d\n",
		      __func__, file_name, length, pos, (unsigned int)ret);

	set_fs(old_fs);
	filp_close(fp, NULL);
#endif
	return 0;
}

//#ifdef SAVE_DE_WB_BUF
s32 save_gray_image_as_bmp(u8 *buf, char *file_name, u32 scn_w, u32 scn_h)
{
#if 0
	int ret = -1;
	ES_FILE *fd = NULL;
	mm_segment_t old_fs;
	loff_t pos = 0;
	BMP_FILE_HEADER st_file_header;
	BMP_INFO_HEADER st_info_header;
	char *src_buf = buf;
	char *path = file_name;
	BMP_INFO dest_info;
	u32 bit_count = 0;
	ST_RGB *dest_buf = NULL;
	ST_ARGB color_table[256];
	u32 i  = 0;

	if ((!path) || (!src_buf)) {
		pr_err("%s: input param is null\n", __func__);
		return -1;
	}

	fd = filp_open(path, O_RDWR | O_CREAT, 0644);
	if (IS_ERR(fd)) {
		pr_err("%s: create bmp file(%s) error\n", __func__, path);
		return -2;
	}

	bit_count = 8;
	memset(&dest_info, 0, sizeof(BMP_INFO));
	dest_info.width = scn_w;
	dest_info.height = scn_h;
	dest_info.bit_count = 8;
	dest_info.color_tbl_size = 0;
	dest_info.image_size = (dest_info.width * dest_info.height * dest_info.bit_count) / 8;

	st_file_header.bfType[0] = 'B';
	st_file_header.bfType[1] = 'M';
	st_file_header.bfOffBits = BMP_IMAGE_DATA_OFFSET;
	st_file_header.bfSize = st_file_header.bfOffBits + dest_info.image_size;
	st_file_header.bfReserved1 = 0;
	st_file_header.bfReserved2 = 0;

	st_info_header.biSize = sizeof(BMP_INFO_HEADER);
	st_info_header.biWidth = dest_info.width;
	st_info_header.biHeight = dest_info.height;
	st_info_header.biPlanes = 1;
	st_info_header.biBitCount = dest_info.bit_count;
	st_info_header.biCompress = 0;
	st_info_header.biSizeImage = dest_info.image_size;
	st_info_header.biXPelsPerMeter = 0;
	st_info_header.biYPelsPerMeter = 0;
	st_info_header.biClrUsed = 0;
	st_info_header.biClrImportant = 0;

	for (i = 0; i < 256; i++) {
		color_table[i].reserved = 0;
		color_table[i].r = i;
		color_table[i].g = i;
		color_table[i].b = i;
	}

	dest_buf = (ST_RGB *)src_buf;

	old_fs = get_fs();
	set_fs(KERNEL_DS);

	ret = vfs_write(fd, (u8 *)(&st_file_header), BMP_FILE_HEADER_SIZE, &pos);
	EINK_INFO_MSG("save file header, len=%u, pos=%d, ret=%d\n", (unsigned int)BMP_FILE_HEADER_SIZE, (int)pos, ret);

	ret = vfs_write(fd, (u8 *)(&st_info_header), BMP_INFO_HEADER_SIZE, &pos);
	EINK_INFO_MSG("save file header, len=%u, pos=%d, ret=%d\n", (unsigned int)BMP_INFO_HEADER_SIZE, (int)pos, ret);

	ret = vfs_write(fd, (u8 *)(color_table), BMP_COLOR_SIZE, &pos);
	EINK_INFO_MSG("save file header, len=%u, pos=%d, ret=%d\n", (unsigned int)BMP_COLOR_SIZE, (int)pos, ret);

	ret = vfs_write(fd, (u8 *)(dest_buf), dest_info.image_size, &pos);
	EINK_INFO_MSG("save file header, len=%u, pos=%d, ret=%d\n", dest_info.image_size, (int)pos, ret);

	set_fs(old_fs);

	ret = 0;

	if (!IS_ERR(fd)) {
		filp_close(fd, NULL);
	}
#endif
	return 0;
}

void save_upd_rmi_buffer(u32 order, u8 *buf, u32 len)
{
	char file_name[256] = {0};

	if (!buf) {
		pr_err("%s:input param is null\n", __func__);
		return;
	}

	memset(file_name, 0, sizeof(file_name));
	snprintf(file_name, sizeof(file_name), "/tmp/rmi_buf%d.bin", order);
	save_as_bin_file(buf, file_name, len, 0);
}

void eink_put_gray_to_mem(u32 order, char *buf, u32 width, u32 height)
{
	char file_name[256] = {0};

	if (!buf) {
		pr_err("input param is null\n");
		return;
	}

	memset(file_name, 0, sizeof(file_name));
	snprintf(file_name, sizeof(file_name), "/tmp/eink_image%d.bmp", order);
	save_gray_image_as_bmp((u8 *)buf, file_name, width, height);
}

//#endif

#if (defined DE_WB_DEBUG) || (defined WAVEDATA_DEBUG)
int eink_get_gray_from_mem(__u8 *buf, char *file_name, __u32 length, loff_t pos)
{
	struct file *fp = NULL;
	mm_segment_t fs;
	__s32 read_len = 0;
	ssize_t ret = 0;

	if ((!buf) || (!file_name)) {
		pr_err("%s: buf or file_name is null\n", __func__);
		return -1;
	}

	EINK_INFO_MSG("\n");
	fp = filp_open(file_name, O_RDONLY, 0);
	if (IS_ERR(fp)) {
		pr_err("%s: fail to open file(%s), ret=0x%p\n",
		       __func__, file_name, fp);
		return -1;
	}
	fs = get_fs();
	set_fs(KERNEL_DS);

	read_len = vfs_read(fp, buf, length, &pos);
	if (read_len != length) {
		pr_err("maybe miss some data(read=%d byte, file=%d byte)\n",
		       read_len, length);
		ret = -EAGAIN;
	}
	set_fs(fs);
	filp_close(fp, NULL);

	return ret;
}
#endif

#ifdef WAVEDATA_DEBUG
void save_waveform_to_mem(u32 order, u8 *buf, u32 frames, u32 bit_num)
{
	char file_name[256] = {0};
	u32 len = 0, per_size = 0;

	if (!buf) {
		pr_err("%s:input param is null\n", __func__);
		return;
	}
	if (bit_num == 5)
		per_size = 1024;
	else
		per_size = 256;

	len = frames * per_size;
	memset(file_name, 0, sizeof(file_name));
	snprintf(file_name, sizeof(file_name), "/tmp/waveform%d.bin", order);
	save_as_bin_file(buf, file_name, len, 0);
}

void save_rearray_waveform_to_mem(u8 *buf, u32 len)
{
	char file_name[256] = {0};

	if (!buf) {
		pr_err("%s:input param is null\n", __func__);
		return;
	}
	pr_info("%s:len is %d\n", __func__, len);

	memset(file_name, 0, sizeof(file_name));
	snprintf(file_name, sizeof(file_name), "/tmp/waveform_rearray.bin");
	save_as_bin_file(buf, file_name, len, 0);
}
#endif

void *malloc_aligned(u32 size, u32 alignment)
{
     void *ptr = (void *)malloc(size + alignment);
      if (ptr) {
	    void *aligned = (void *)(((long)ptr + alignment) & (~(alignment - 1)));

	    /* Store the original pointer just before aligned pointer*/
	    ((void **)aligned)[-1] = ptr;
	return aligned;
      }

      return NULL;
}

void free_aligned(void *aligned_ptr)
{
     if (aligned_ptr)
	free(((void **)aligned_ptr)[-1]);
}

