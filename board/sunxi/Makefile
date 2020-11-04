# SPDX-License-Identifier: GPL-2.0+
#
# (C) Copyright 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
#
# Based on some other board Makefile
#
# (C) Copyright 2000-2003
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
obj-y	+= board.o
obj-y	+= board_common.o board_helper.o
obj-y	+= sys_config.o
obj-y	+= sys_partition.o
obj-$(CONFIG_SUNXI_MAC)	+= sunxi_mac.o
obj-$(CONFIG_SUN7I_GMAC)	+= gmac.o
ifndef CONFIG_SPL_BUILD
obj-$(CONFIG_SUNXI_AHCI)	+= ahci.o
endif
obj-$(CONFIG_MACH_SUN4I)	+= dram_sun4i_auto.o
obj-$(CONFIG_MACH_SUN5I)	+= dram_sun5i_auto.o
obj-$(CONFIG_MACH_SUN7I)	+= dram_sun5i_auto.o

obj-$(CONFIG_SUNXI_IMAGE_VERIFIER)	+= sunxi_image_verifier.o
obj-$(CONFIG_SUNXI_BURN)	+= key_burn.o
obj-$(CONFIG_CMD_SUNXI_AUTO_FEL)	+= sunxi_auto_fel.o
obj-$(CONFIG_SUNXI_SERIAL)	+= sunxi_serial.o
obj-$(CONFIG_SUNXI_KEYBOX)	+= sunxi_keybox.o
obj-y += sunxi_bootargs.o
obj-y += android/
obj-y += homlet/
obj-$(CONFIG_SUNXI_POWER)     += power_manage.o
