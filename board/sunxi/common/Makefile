#
# (C) Copyright 2007-2015
# Allwinner Technology Co., Ltd. <www.allwinnertech.com>
# Jerry Wang <wangflord@allwinnertech.com>
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
# GNU General Public License for more details.
#
# SPDX-License-Identifier:	GPL-2.0
#


obj-y	+= sunxi_debug.o
obj-y	+= sys_config.o
obj-y	+= sys_config_old.o
ifdef CONFIG_SUNXI_GPT
obj-y	+= sys_partition_gpt.o
else
obj-y	+= sys_partition.o
endif

obj-y   += sunxi_string.o
obj-$(CONFIG_SUNXI_MAC)          += sunxi_mac.o
obj-$(CONFIG_SUNXI_MODULE_AXP)   += power_probe.o power_check.o
obj-y   += sunxi_debug.o
obj-y   += check_key_uart.o
obj-y   += debug_mode.o
obj-$(CONFIG_SUNXI_KEY_BURN)   += key_burn.o
obj-$(CONFIG_SUNXI_SERIAL)     += sunxi_serial.o
obj-$(CONFIG_BOX_STANDBY)      += box_standby.o
obj-$(CONFIG_BOOT_PARAMETER)   += bootparam.o

obj-y	  +=de_weak.o
obj-$(CONFIG_VIDEO_SUNXI_V1)   += de_v1.o
obj-$(CONFIG_VIDEO_SUNXI_V2)   += de_v2.o
obj-$(CONFIG_VIDEO_SUNXI_V3)   += de_v3.o

obj-y += grallocator.o
obj-$(CONFIG_BOOT_GUI)   += bootGUI/

ifdef CONFIG_SUNXI_MULITCORE_BOOT
obj-y   += secondary_main.o
endif

obj-y   += board_common.o

subdir-ccflags-$(CONFIG_SUNXI_FINS_FUNC_BOARD_DIR) += -finstrument-functions
