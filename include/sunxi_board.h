/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
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

#ifndef _SUNXI_BOARD_H_
#define _SUNXI_BOARD_H_

#include <asm/u-boot.h>
#include <sunxi_flash.h>
#include <spare_head.h>
#include <boot_gui.h>

extern int sunxi_oem_op_lock(int lock_flag, char *info);
extern int sunxi_fastboot_status_read(void);

extern void sunxi_board_close_source(void);
extern int sunxi_board_restart(int next_mode);
extern int sunxi_board_shutdown(void);
extern int sunxi_board_prepare_kernel(void);
extern int sunxi_board_run_fel(void);
extern int sunxi_board_run_fel_eraly(void);

extern void sunxi_update_subsequent_processing(int next_work);
extern void fastboot_partition_init(void);
extern int check_android_misc(void);
extern int board_late_init(void);

extern int check_uart_input(void);
extern int check_update_key(void);
extern int gpio_control(void);

extern int battery_charge_cartoon_init(int rate);
extern int battery_charge_cartoon_exit(void);
extern int battery_charge_cartoon_rate(int rate);
extern int battery_charge_cartoon_reset(void);
extern int battery_charge_cartoon_degrade(int alpha_step);

extern int board_display_layer_request(void);
extern int board_display_layer_release(void);
extern int board_display_wait_lcd_open(void);
extern int board_display_wait_lcd_close(void);
extern int board_display_set_exit_mode(int lcd_off_only);
extern int board_display_layer_open(void);
extern int board_display_layer_close(void);
extern int board_display_layer_para_set(void);
extern int board_display_show_until_lcd_open(int display_source);
extern int board_display_show(int display_source);
extern int board_display_framebuffer_set(int width, int height, int bitcount, void *buffer);
extern int board_display_framebuffer_change(void *buffer);
extern void board_display_set_alpha_mode(int mode);
extern int board_display_device_open(void);
extern int board_display_eink_update(char *name, __u32 update_mode);
/* extern int board_display_eink_panel_release(void); */
extern int borad_display_get_screen_width(void);
extern int borad_display_get_screen_height(void);
extern void board_display_setenv(char *data);

extern void board_status_probe(int standby_mode);

extern void power_limit_detect_enter(void);
extern void power_limit_detect_exit(void);
extern void power_limit_init(void);
extern void power_limit_for_vbus(int battery_exist,int power_type);

extern int usb_detect_enter(void);
extern int usb_detect_exit(void);
extern void usb_detect_for_charge(int detect_time);

extern int sunxi_flash_handle_init(void);

extern int sunxi_bmp_display(char *name);
extern int sunxi_Eink_Get_bmp_buffer(char *name, char *bmp_gray_buf);

extern int drv_disp_init(void);
extern int drv_disp_exit(void);
extern int drv_disp_standby(unsigned int cmd, void *pArg);
extern long disp_ioctl(void *hd, unsigned int cmd, void *arg);

extern int board_init(void);
extern void dram_init_banksize(void);
extern int dram_init(void);
extern int sunxi_bmp_load(char *name);

extern int change_to_debug_mode(void);
#ifdef CONFIG_GENERIC_MMC
extern int board_mmc_init(bd_t *bis);
extern void board_mmc_pre_init(int card_num);
extern int board_mmc_get_num(void);
extern void board_mmc_set_num(int num);
//extern int mmc_get_env_addr(struct mmc *mmc, u32 *env_addr);
#endif

#ifdef	TIMESTAMP
extern void clean_timestamp_counter(void);
#endif
#ifdef CONFIG_DISPLAY_BOARDINFO
extern int checkboard(void);
#endif

extern int sprite_uichar_init(int char_size);
extern void sprite_uichar_printf(const char * str, ...);

#if defined(CONFIG_USE_NEON_SIMD)
extern int  arm_neon_init(void);
extern uint add_sum_neon(void *buffer, uint length);
#endif

extern void respond_physical_key_action(void);
extern int check_physical_key_early(void);

extern void sunxi_set_fel_flag(void);
extern void sunxi_clear_fel_flag(void);

extern int sunxi_verify_embed_signature(void *buff, unsigned int len, const char *cert_name,\
										void *cert, unsigned cert_len);
extern int sunxi_verify_signature(void *buff, uint len, const char *cert_name);
extern int sunxi_verify_rotpk_hash(void *input_hash_buf, int len);

extern void sunxi_dump(void *addr, unsigned int size);
extern char *board_hardware_info(void);

#ifdef CONFIG_DETECT_RTC_BOOT_MODE
extern int set_bootcmd_from_rtc(int mode, char *bootcmd);
extern int sunxi_get_bootmode_flag(void);
extern int sunxi_set_bootmode_flag(u8 flag);
#endif
extern char *set_bootcmd_from_misc(int mode, char *bootcmd);

extern int get_boot_work_mode(void);
extern int get_boot_storage_type_ext(void);
extern int get_boot_storage_type(void);
extern void set_boot_storage_type(int);

extern u32 get_boot_dram_para_addr(void);
extern u32 get_boot_dram_para_size(void);
extern u32 get_boot_dram_update_flag(void);
extern void set_boot_dram_update_flag(u32 *dram_para);
extern u32 get_pmu_byte_from_boot0(void);


extern int mmc_request_update_boot0(int dev_num);
extern int mmc_write_info(int dev_num,void *buffer,u32 buffer_size);

extern int get_debugmode_flag(void);

extern int sunxi_probe_securemode(void);
extern int sunxi_get_securemode(void);
extern int sunxi_probe_secure_monitor(void);
extern int sunxi_probe_secure_os(void);

extern int smc_init(void);


extern int erase_all_private_data(void);
extern int read_private_key_by_name(const char * name, char *buffer, int buffer_len, int *data_len);
extern int save_user_private_data(char *name, char *data, int length);

extern int sunxi_download_boot0_atfter_ota(void *buffer, int production_media);

extern int get_core_pos(void);

extern void sunxi_store_gp_status(void);
extern void sunxi_set_gp_status(void);
extern void sunxi_restore_gp_status(void);
extern int  sunxi_set_cpu_on(int cpu, uint entry);
extern int  sunxi_set_cpu_off(void);

extern int  cleanup_before_powerdown(void);
void sunxi_dump(void *addr, unsigned int size);

extern uint sunxi_generate_checksum(void *buffer, uint length, uint src_sum);
extern int sunxi_verify_checksum(void *buffer, uint length, uint src_sum);

#define BIT(x)				(1<<(x))
#define sunxi_set_bit(addr, val) writel((readl(addr) | val), addr)
#define sunxi_clear_bit(addr, val) writel((readl(addr) & ~val), addr)

#endif /*_SUNXI_BOARD_H_ */
