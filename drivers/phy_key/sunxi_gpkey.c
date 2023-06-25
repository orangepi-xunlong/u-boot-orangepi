/*
 *  * Copyright 2000-2009
 *   * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *    *
 *     * SPDX-License-Identifier:	GPL-2.0+
 *     */
#include <common.h>
#include <asm/io.h>
#include <physical_key.h>
#include <sys_config.h>
#include <fdt_support.h>
#include <console.h>
#include <sunxi_gpadc.h>

__attribute__((section(".data")))
static int keyen_flag = 1;

__weak int axp_probe_key(void)
{
	return 0;
}

int sunxi_key_init(void)
{
	uint reg_val = 0;

	sunxi_key_clock_open();

	/*choose channel 0*/
	reg_val = readl(GP_CS_EN);
	reg_val |= 1;
	writel(reg_val, GP_CS_EN);

	/*choose continue work mode and enable ADC*/
	reg_val = readl(GP_CTRL);
	reg_val &= ~(1<<18);
	reg_val |= ((1<<19) | (1<<16));
	writel(reg_val, GP_CTRL);

	/* disable all key irq */
	writel(0, GP_DATA_INTC);
	writel(1, GP_DATA_INTS);

	script_parser_fetch("key_detect_en", "keyen_flag", &keyen_flag, 1);

	return 0;
}

int sunxi_key_exit(void)
{
	writel(0, GP_CTRL);
	/* disable all key irq */
	writel(0, GP_DATA_INTC);
	writel(0, GP_DATA_INTS);

	sunxi_key_clock_close();

	return 0;
}

int sunxi_key_read(void)
{
	u32 ints;
	int key = -1;
	int vin;

	if (!keyen_flag)
		return -1;

	ints = readl(GP_DATA_INTS);
	/* clear the pending data */
	writel(readl(GP_DATA_INTS)|(ints & 0x1), GP_DATA_INTS);
	/* if there is already data pending, read it */
	if (ints & GPADC0_DATA_PENDING) {
		vin = readl(GP_CH0_DATA)*18/4095;
		if (vin > 16)
			key = -1;
		else {
			key = readl(GP_CH0_DATA)*63/4095;
			printf("key pressed value=0x%x\n", key);
		}
	}
	return key;
}


int do_adc_key_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	int val;

	sunxi_gpadc_init();
	while (1) {
		val = sunxi_gpadc_read(0);
		printf("gpadc read vol: %d \n", val);
		udelay(1000 * 1000);
		if (tstc()) {
			if (0x03 == getc())	/*ctrl+c exit */
				break;
		}
	}
	return 0;
}

int do_power_key_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	u32 power_key = 0;

	writel(1, GP_DATA_INTS);

	printf(" press a key:\n");
	while (!ctrlc()) {
		sunxi_key_read();
		power_key = axp_probe_key();
		if (power_key > 0) {
			break;
		}
	}

	return 0;

}

static cmd_tbl_t cmd_key_test[] = {
	U_BOOT_CMD_MKENT(power_key, 2, 0, do_power_key_test, "", ""),
	U_BOOT_CMD_MKENT(adc_driver, 2, 0, do_adc_key_test, "", ""),
};

int do_key_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	cmd_tbl_t *cp;
	cp = find_cmd_tbl(argv[1], cmd_key_test, ARRAY_SIZE(cmd_key_test));
	/* Drop the sunxi_ce_test command */
	argc--;
	argv++;

	if (cp)
		return cp->cmd(cmdtp, flag, argc, argv);
	else {
		pr_err("unknown sub command\n");
		return CMD_RET_USAGE;
	}
}

U_BOOT_CMD(
	key_test, CONFIG_SYS_MAXARGS, 0, do_key_test,
	"Test the key value\n", "NULL"
);

