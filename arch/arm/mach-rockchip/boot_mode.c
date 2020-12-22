// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2016 Rockchip Electronics Co., Ltd
 */

#include <common.h>
#include <adc.h>
#include <command.h>
#include <env.h>
#include <led.h>
#include <log.h>
#include <mmc.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <asm/arch-rockchip/boot_mode.h>
#include <dm/device.h>
#include <dm/uclass.h>

#if (CONFIG_ROCKCHIP_BOOT_MODE_REG == 0)

int setup_boot_mode(void)
{
	return 0;
}

#else

void set_back_to_bootrom_dnl_flag(void)
{
	writel(BOOT_BROM_DOWNLOAD, CONFIG_ROCKCHIP_BOOT_MODE_REG);
}

/*
 * detect download key status by adc, most rockchip
 * based boards use adc sample the download key status,
 * but there are also some use gpio. So it's better to
 * make this a weak function that can be override by
 * some special boards.
 */
#define KEY_DOWN_MIN_VAL	0
#define KEY_DOWN_MAX_VAL	30

__weak int rockchip_dnl_key_pressed(void)
{
	unsigned int val;
	struct udevice *dev;
	struct uclass *uc;
	int ret;

	ret = uclass_get(UCLASS_ADC, &uc);
	if (ret)
		return false;

	ret = -ENODEV;
	uclass_foreach_dev(dev, uc) {
		if (!strncmp(dev->name, "saradc", 6)) {
			ret = adc_channel_single_shot(dev->name, 1, &val);
			break;
		}
	}

	if (ret == -ENODEV) {
		pr_warn("%s: no saradc device found\n", __func__);
		return false;
	} else if (ret) {
		pr_err("%s: adc_channel_single_shot fail!\n", __func__);
		return false;
	}

	if ((val >= KEY_DOWN_MIN_VAL) && (val <= KEY_DOWN_MAX_VAL))
		return true;
	else
		return false;
}

#if defined(CONFIG_ROCKCHIP_ADVANCED_RECOVERY)
#define RECOVERY_LED_BY_LABEL(dev) led_get_by_label(CONFIG_ROCKCHIP_ADVANCED_RECOVERY_LED, dev)
void rockchip_blink_recovery_led(int times)
{
	struct udevice *dev;
	RECOVERY_LED_BY_LABEL(&dev);
	for (int i = 0; i < times; ++i) {
		led_set_state(dev, LEDST_ON);
		mdelay(100);
		led_set_state(dev, LEDST_OFF);
		mdelay(100);
	}
}

int rockchip_dnl_mode(int num_modes)
{
	int mode = 0;
	const char *mode_names[5] = {
		"none",
		"ums",
		"rockusb",
		"fastboot",
		"maskrom"
	};

	const int modes_enabled[5] = {
		1,
#if defined(CONFIG_ROCKCHIP_ADVANCED_RECOVERY_UMS)
		1,
#else
		0,
#endif
#if defined(CONFIG_ROCKCHIP_ADVANCED_RECOVERY_ROCKUSB)
		1,
#else
		0,
#endif
#if defined(CONFIG_ROCKCHIP_ADVANCED_RECOVERY_FASTBOOT)
		1,
#else
		0,
#endif
#if defined(CONFIG_ROCKCHIP_ADVANCED_RECOVERY_MASKROM)
		1,
#else
		0,
#endif
	};

	while(mode < num_modes) {
		++mode;

		if (modes_enabled[mode]) {
			printf("rockchip_dnl_mode = %s mode\n", mode_names[mode]);
			rockchip_blink_recovery_led(mode);

			// return early
	 		if (mode == num_modes) {
	 			goto end;
	 		}

			// wait 2 seconds
			for (int i = 0; i < 100; ++i) {
				if (!rockchip_dnl_key_pressed()) {
					goto end;
				}
				mdelay(20);
			}
		}
	}

end:
	return mode;
}

__weak void rockchip_prepare_download_mode(void)
{
}

int rockchip_has_mmc_device(int devnum)
{
	struct mmc *mmc;
	mmc = find_mmc_device(devnum);
	if (!mmc || mmc_init(mmc))
		return 0;
	else
		return 1;
}
#endif

void rockchip_dnl_mode_check(void)
{
#if defined(CONFIG_ROCKCHIP_ADVANCED_RECOVERY)
	int mmc_device = 0;
	int ret = 0;
	char cmd[32];

	if (!rockchip_dnl_key_pressed()) {
		return 0;
	}

	if (rockchip_has_mmc_device(0)) {
		mmc_device = 0;
	} else if (rockchip_has_mmc_device(1)) {
		mmc_device = 1;
	} else {
		printf("no mmc device suitable for download mode!\n");
		return 0;
	}

	printf("using mmc%d device for download mode\n", mmc_device);

	switch(rockchip_dnl_mode(4)) {
	case 0:
		return;

	case 1:
		printf("entering ums mode...\n");
		rockchip_prepare_download_mode();
		sprintf(cmd, "ums 0 mmc %d", mmc_device);
		cli_simple_run_command(cmd, 0);
		break;

	case 2:
		printf("entering rockusb mode...\n");
		rockchip_prepare_download_mode();
		sprintf(cmd, "rockusb 0 mmc %d", mmc_device);
		cli_simple_run_command(cmd, 0);
		break;

	case 3:
		printf("entering fastboot mode...\n");
		rockchip_prepare_download_mode();
		sprintf(cmd, "mmc dev %d; fastboot usb 0", mmc_device);
		cli_simple_run_command(cmd, 0);
		break;

	case 4:
		printf("entering maskrom mode...\n");
		rockchip_prepare_download_mode();
		break;
	}

	set_back_to_bootrom_dnl_flag();
	do_reset(NULL, 0, 0, NULL);
#else
	if (rockchip_dnl_key_pressed()) {
		printf("download key pressed, entering download mode...");
		set_back_to_bootrom_dnl_flag();
		do_reset(NULL, 0, 0, NULL);
	}
#endif
}

int setup_boot_mode(void)
{
	void *reg = (void *)CONFIG_ROCKCHIP_BOOT_MODE_REG;
	int boot_mode = readl(reg);

	rockchip_dnl_mode_check();

	boot_mode = readl(reg);
	debug("%s: boot mode 0x%08x\n", __func__, boot_mode);

	/* Clear boot mode */
	writel(BOOT_NORMAL, reg);

	switch (boot_mode) {
	case BOOT_FASTBOOT:
		debug("%s: enter fastboot!\n", __func__);
		env_set("preboot", "setenv preboot; fastboot usb0");
		break;
	case BOOT_UMS:
		debug("%s: enter UMS!\n", __func__);
		env_set("preboot", "setenv preboot; ums mmc 0");
		break;
	}

	return 0;
}

#endif
