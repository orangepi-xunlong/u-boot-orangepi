#include <cpu.h>
#include <mmio.h>
#include <debug.h>
#include <assert.h>
#include <errno.h>
#include <bl_common.h>
#include <platform.h>
#include <delay_timer.h>
#include <console.h>
#include <string.h>
#include <rom_api.h>

#include "ddr_pkg_info.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"

#define TPU_BONDING_LMT0 0x1
#define TPU_BONDING_LMT1 0x2

extern struct rom_api *p_rom_api;

void plat_panic_handler(void)
{
	void *ra;

	ATF_ERR = ATF_ERR_PLAT_PANIC;

	ra = __builtin_return_address(0);
	mmio_write_32(ATF_ERR_INFO0, ((uint64_t)ra) & 0xFFFFFFFFUL);

	ERROR("ra=0x%lx\n", (uint64_t)ra);

	__system_reset("panic", -1);
	__builtin_unreachable();
}

void __system_reset(const char *file, unsigned int line)
{
	ATF_ERR = ATF_ERR_PLAT_SYSTEM_RESET;
	ERROR("RESET:%s:%d\n", file, line);

	console_flush();

	ATF_STATE = ATF_STATE_RESET_WAIT;
	mdelay(5000);

	// enable rtc wdt reset
	mmio_write_32(0x050260E0, 0x0001); //enable rtc_core wathdog reset enable
	mmio_write_32(0x050260C8, 0x0001); //enable rtc_core power cycle   enable

	// sw delay 100us
	ATF_STATE = ATF_STATE_RESET_RTC_WAIT;
	udelay(100);

	// mmio_write_32(0x05025018,0x00FFFFFF); //Mercury rtcsys_rstn_src_sel
	mmio_write_32(0x050250AC, 0x00000000); //cv181x rtcsys_rstn_src_sel
	mmio_write_32(0x05025004, 0x0000AB18);
	mmio_write_32(0x05025008, 0x00400040); //enable rtc_ctrl wathdog reset enable

	// printf("Enable TOP_WDT\n");
	// mmio_write_32(0x03010004,0x00000000); //config watch dog 2.6ms
	// mmio_write_32(0x03010004,0x00000022); //config watch dog 166ms
	mmio_write_32(0x03010004, 0x00000066); //config watch dog 166ms
	mmio_write_32(0x0301001c, 0x00000020);
	mmio_write_32(0x0301000c, 0x00000076);
	mmio_write_32(0x03010000, 0x00000011);

	// ROM_PWR_CYC
	if (get_sw_info()->reset_type == 1) {
		ATF_ERR = ATF_ERR_PLAT_SYSTEM_PWR_CYC;
		// printf("Issue RTCSYS_PWR_CYC\n");

		// wait pmu state to ON
		while (mmio_read_32(0x050260D4) != 0x00000003) {
			;
		}

		mmio_write_32(0x05025008, 0x00080008);
	}

	while (1)
		;

	__builtin_unreachable();
}

void reset_c906l(uintptr_t reset_address)
{
	NOTICE("RSC.\n");

	mmio_clrbits_32(0x3003024, 1 << 6);

	mmio_setbits_32(SEC_SYS_BASE + 0x04, 1 << 13);
	mmio_write_32(SEC_SYS_BASE + 0x20, reset_address);
	mmio_write_32(SEC_SYS_BASE + 0x24, reset_address >> 32);

	mmio_setbits_32(0x3003024, 1 << 6);
}

void setup_dl_flag(void)
{
	uint32_t v = p_rom_api->get_boot_src();

	switch (v) {
	case BOOT_SRC_SD:
		mmio_write_32(BOOT_SOURCE_FLAG_ADDR, MAGIC_NUM_SD_DL);
		break;
	case BOOT_SRC_USB:
		mmio_write_32(BOOT_SOURCE_FLAG_ADDR, MAGIC_NUM_USB_DL);
		break;
	default:
		mmio_write_32(BOOT_SOURCE_FLAG_ADDR, v);
		break;
	}
}

void config_core_power(uint32_t low_period)
{
	/*
	 * low_period = 0x42; // 0.90V
	 * low_period = 0x48; // 0.93V
	 * low_period = 0x4F; // 0.96V
	 * low_period = 0x58; // 1.00V
	 * low_period = 0x5C; // 1.02V
	 * low_period = 0x62; // 1.05V
	 * low_period = 0x62; // 1.05V
	 */
	// mmio_write_32(PWM0_BASE + PWM_HLPERIOD0, low_period);
	// mmio_write_32(PWM0_BASE + PWM_PERIOD0, 0x64);
	// mmio_write_32(PINMUX_BASE + 0xEC, 0x0); // set pinmux for pwm0
	// mmio_write_32(PWM0_BASE + PWM_START, 0x1); // enable bit0:pwm0 and bit3:pwm3
	// mmio_write_32(PWM0_BASE + PWM_OE, 0x1); // output enable bit0:pwm0 and bit3:pwm3
	mdelay(10);
}

void config_rgmii_power(void)
{
	mmio_write_32(0x281051e0, mmio_read_32(0x281051e0) | 0x300); //RGMII 1.8V
	mmio_write_32(0x281001cc, mmio_read_32(0x281001cc) | 0x101); //rx delay 2ns
}

void sys_switch_all_to_pll(void)
{
	// Switch all clocks to PLL
	mmio_write_32(0x28102198, 0);
	mmio_write_32(0x2810219c, 0);
	mmio_write_32(0x281021a0, 0);
	//skip uart0
	//mmio_write_32(0x281021a4, 0);
	mmio_clrbits_32(0x281021a4, ~(1 << 25));
	mmio_write_32(0x281021a8, 0);
}

void sys_pll_init(void)
{
	uint32_t value;
	NOTICE("PLLS.\n");
	//disppll0 sw update
	mmio_setbits_32(0x28102460, 0x1);
	//disppll1 sw update
	mmio_setbits_32(0x28102560, 0x1);
	//a0pll sw update
	mmio_setbits_32(0x28102760, 0x1);

	value = (mmio_read_32(0x33052014) >> 3) & 0x7;
	switch (value) {
	case TPU_BONDING_LMT0:
		mmio_write_32(0x281029a0, 0x02360008);
		break;
	case TPU_BONDING_LMT1:
		mmio_write_32(0x281029a0, 0x02240008);
		break;
	default:
		break;
	}
	NOTICE("PLLE.\n");
}

#if 0
void switch_rtc_mode_1st_stage(void)
{
	uint32_t read_data;
	uint32_t write_data;
	uint32_t rtc_mode;

#ifdef CV181X_SUPPORT_SUSPEND_RESUME
	void (*warmboot_entry)(void) = get_warmboot_entry();

	if (warmboot_entry == (void *)BL31_WARMBOOT_ENTRY)
		return;
#endif

	// reg_rtc_mode = rtc_ctrl0[10]
	read_data = mmio_read_32(REG_RTC_CTRL_BASE + RTC_CTRL0);
	rtc_mode = (read_data >> 10) & 0x1;
	if (rtc_mode == 0x1) {
		NOTICE("By pass rtc mode switch\n");
		return;
	}

	mmio_write_32(REG_RTC_CTRL_BASE + RTC_CTRL0_UNLOCKKEY, 0xAB18);
	read_data = mmio_read_32(REG_RTC_CTRL_BASE + RTC_CTRL0);

	// reg_clk32k_cg_en = rtc_ctrl0[11] -> 0
	write_data = 0x08000000 | (read_data & 0xfffff7ff);
	mmio_write_32(REG_RTC_CTRL_BASE + RTC_CTRL0, write_data);

	//cg_en_out_clk_32k = rtc_ctrl_status0[25]
	read_data = mmio_read_32(REG_RTC_CTRL_BASE + RTC_CTRL0_STATUS0);
	while ((read_data & 0x02000000) != 0x00)
		read_data = mmio_read_32(REG_RTC_CTRL_BASE + RTC_CTRL0_STATUS0);

	read_data = mmio_read_32(REG_RTC_CTRL_BASE + RTC_CTRL0);
	//r eg_rtc_mode = rtc_ctrl0[10];
	write_data = 0x04000000 | (read_data & 0xfffffbff) | (0x1 << 10);
	mmio_write_32(REG_RTC_CTRL_BASE + RTC_CTRL0, write_data);

	// DA_SOC_READY = 1
	mmio_write_32(RTC_MACRO_BASE + 0x8C, 0x01);
	// DA_SOC_READY = 0
	mmio_write_32(RTC_MACRO_BASE + 0x8C, 0x0);

	udelay(200); // delay ~200us

	read_data = mmio_read_32(REG_RTC_CTRL_BASE + RTC_CTRL0);
	// reg_clk32k_cg_en = rtc_ctrl0[11] -> 1
	write_data = 0x0C000000 | (read_data & 0xffffffff) | (0x1 << 11);
	mmio_write_32(REG_RTC_CTRL_BASE + RTC_CTRL0, write_data); //rtc_ctrl0
}

void switch_rtc_mode_2nd_stage(void)
{
	uint32_t read_data;
	uint32_t write_data;

	// mdelay(50);
	read_data = mmio_read_32(REG_RTC_CTRL_BASE + RTC_CTRL0_STATUS0);
#ifndef NO_DDR_CFG
	if (get_pkg() == PKG_QFN || (read_data & 0x02000000) == 0x00) {
#else
	if ((read_data & 0x02000000) == 0x00) {
#endif
		read_data = mmio_read_32(REG_RTC_CTRL_BASE + RTC_CTRL0);
		// reg_rtc_mode = rtc_ctrl0[10]
		write_data = 0x0C000000 | (read_data & 0xfffffbff);
		mmio_write_32(REG_RTC_CTRL_BASE + RTC_CTRL0, write_data);
		//DA_SOC_READY = 1
		mmio_write_32(RTC_MACRO_BASE + 0x8C, 0x01);
		//DA_SOC_READY = 0
		mmio_write_32(RTC_MACRO_BASE + 0x8C, 0x00);
		NOTICE("Use internal 32k\n");
	} else
		NOTICE("Switch RTC mode to xtal32k\n");
}
#endif

void set_rtc_en_registers(void)
{
	uint32_t write_data;
	uint32_t read_data;

	read_data = mmio_read_32(REG_RTC_BASE + RTC_ST_ON_REASON);
	NOTICE("st_on_reason=%x\n", read_data);
	read_data = mmio_read_32(REG_RTC_BASE + RTC_ST_OFF_REASON);
	NOTICE("st_off_reason=%x\n", read_data);

	mmio_write_32(REG_RTC_BASE + RTC_EN_SHDN_REQ, 0x01);
	while (mmio_read_32(REG_RTC_BASE + RTC_EN_SHDN_REQ) != 0x01)
		;

	mmio_write_32(REG_RTC_BASE + RTC_EN_WARM_RST_REQ, 0x01);
	while (mmio_read_32(REG_RTC_BASE + RTC_EN_WARM_RST_REQ) != 0x01)
		;

	mmio_write_32(REG_RTC_BASE + RTC_EN_PWR_CYC_REQ, 0x01);
	while (mmio_read_32(REG_RTC_BASE + RTC_EN_PWR_CYC_REQ) != 0x01)
		;

	mmio_write_32(REG_RTC_BASE + RTC_EN_WDT_RST_REQ, 0x01);
	while (mmio_read_32(REG_RTC_BASE + RTC_EN_WDT_RST_REQ) != 0x01)
		;

	// Set rtcsys_rst_ctrl[24] = 1; bit 24 is reg_rtcsys_reset_en
	mmio_setbits_32(REG_RTC_CTRL_BASE + RTC_POR_RST_CTRL, 0X1);

	// rtc_ctrl0_unlockkey
	mmio_write_32(REG_RTC_CTRL_BASE + RTC_CTRL0_UNLOCKKEY, 0xAB18);

	// Enable hw_wdg_rst_en
	write_data = mmio_read_32(REG_RTC_CTRL_BASE + RTC_CTRL0);
	write_data = 0xffff0000 | write_data | (0x1 << 11) | (0x01 << 6);
	mmio_write_32(REG_RTC_CTRL_BASE + RTC_CTRL0, write_data);

	// Avoid power up again after poweroff
	mmio_clrbits_32(REG_RTC_BASE + RTC_EN_PWR_VBAT_DET, BIT(2));
}

void init_comm_info(void)
{
#ifdef RTOS_ENABLE_FREERTOS
	struct transfer_config_t *transfer_config = (struct transfer_config_t *)MAILBOX_FIELD;
	struct transfer_config_t transfer_config_s;
	unsigned char *ptr = (unsigned char *)&transfer_config_s;
	unsigned short checksum = 0;
	/* mailbox field is 4 byte write access, and can not access byte by byte.
	 * so init parameters and copy all to mailbox field together.
	 */
	transfer_config_s.conf_magic = RTOS_MAGIC_HEADER;
	transfer_config_s.conf_size = ((uint64_t)&transfer_config_s.checksum - (uint64_t)&transfer_config_s.conf_magic);
	transfer_config_s.isp_buffer_addr = CVIMMAP_ISP_MEM_BASE_ADDR;
	transfer_config_s.isp_buffer_size = CVIMMAP_ISP_MEM_BASE_SIZE;
	transfer_config_s.encode_img_addr = CVIMMAP_H26X_BITSTREAM_ADDR;
	transfer_config_s.encode_img_size = CVIMMAP_H26X_BITSTREAM_SIZE;
	transfer_config_s.encode_buf_addr = CVIMMAP_H26X_ENC_BUFF_ADDR;
	transfer_config_s.encode_buf_size = CVIMMAP_H26X_ENC_BUFF_SIZE;
	transfer_config_s.dump_print_enable = RTOS_DUMP_PRINT_ENABLE;
	transfer_config_s.dump_print_size_idx = RTOS_DUMP_PRINT_SZ_IDX;
	transfer_config_s.image_type = RTOS_FAST_IMAGE_TYPE;
	transfer_config_s.mcu_status = MCU_STATUS_NONOS_DONE;
	for (int i = 0; i < transfer_config_s.conf_size; i++) {
		checksum += ptr[i];
	}

	transfer_config_s.checksum = checksum;
	memcpy(transfer_config, &transfer_config_s, sizeof(struct transfer_config_t));
#endif
}

void apply_analog_trimming_data(void)
{
}

void bm_storage_boot_loader_version(uint32_t addr)
{
	int size_version, size_time;

	size_version = strlen(version_string);
	for (int i = 0; i < size_version; i++) {
		mmio_write_8(addr + i, version_string[i]);
	}

	mmio_write_8(addr + size_version, ' ');

	size_time = strlen(build_message);
	for (int i = 0; i < size_time; i++) {
		mmio_write_8(addr + size_version + 1 + i, build_message[i]);
	}

	mmio_write_8(addr + size_version + 1 + size_time, '\0');
}

void enable_top_wdt(void)
{
	// config top wdt 30s
	mmio_write_32((PERI_WDT0_BASE + 0x4), 0xEE);
	mmio_write_32((PERI_WDT0_BASE + 0x1c), 0x5969);
	mmio_write_32(PERI_WDT0_BASE, 0xD1);
}

void vddc_pwm_init(void)
{
	/*
	 * low_period = 0x55; // 0.98V
	 * low_period = 0x50; // 0.96V
	 * low_period = 0x4B; // 0.94V
	 * low_period = 0x46; // 0.92V
         * low_period = 0x42; // 0.90V
	 * low_period = 0x41; // 0.89V
	 * low_period = 0x3c; // 0.87V
         */
	int low_period = 0x42;

        // set pin/func mux GPIO0 to PWM8
        mmio_clrsetbits_32(G12_PINMUX_BASE + 0x64, 0xF << 4, 0x7 << 4);
        mmio_clrsetbits_32(CORE_IPMUX_BASE + 0xc4, 0xF, 0xC);

        // init PWM8 with 34% duty cycle
        mmio_clrsetbits_32(PWM2_BASE + HLPERIOD0, 0x3FFFFFFF, low_period);
        mmio_clrsetbits_32(PWM2_BASE + PERIOD0, 0x3FFFFFFF, 0x64);
        mmio_clrsetbits_32(PWM2_BASE + POLARITY, 0xF << 8, 0);
        mmio_clrsetbits_32(PWM2_BASE + PWMSTART, 0xF, 0x1);
        mmio_clrsetbits_32(PWM2_BASE + PWM_OE, 0xF, 0x1);
}

void platform_setup(void)
{
	time_records->bl2_start = read_time_ms();
	VERBOSE("\n#bl2 start at: %d ms#\n", time_records->bl2_start);
	// set emmc, sd0 bypass
	mmio_setbits_32(0x281021a4, 1 << 8 | 1 << 7 | 1 << 2 | 1 << 1);

	// set uart0 clk to pll
	mmio_clrbits_32(0x281021a4, 1 << 25);

	console_init(PLAT_BOOT_UART_BASE, PLAT_BM_BOOT_UART_CLK_IN_HZ,
		     PLAT_BM_CONSOLE_BAUDRATE);

	vddc_pwm_init();

	config_rgmii_power();
	NOTICE("\nFSBL %s:%s\n", version_string, build_message);
	INFO("sw_info=0x%x\n", get_sw_info()->value);

	INFO("fip_param1: param_cksum=0x%x param2_loadaddr=0x%x\n",
	     fip_param1->param_cksum, fip_param1->param2_loadaddr);

	INFO("CP_STATE_REG=0x%x\n", mmio_read_32(0x25047018));

	print_sram_log();

	rom_api_redirect();

	setup_dl_flag();

	//switch_rtc_mode_1st_stage();

	set_rtc_en_registers();
	bm_storage_boot_loader_version(BL2_VERSION_BASE);

	enable_top_wdt();
}

#pragma GCC diagnostic pop
