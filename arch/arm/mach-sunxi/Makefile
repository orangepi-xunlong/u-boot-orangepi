# SPDX-License-Identifier: GPL-2.0+
#
# (C) Copyright 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
#
# Based on some other Makefile
# (C) Copyright 2000-2003
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.

obj-y	+= board.o
obj-y	+= clock.o

ifeq ($(CONFIG_SUNXI_GIC_V3),y)
obj-y	+= gic_v3.o
else
obj-y	+= gic.o
endif

obj-y	+= cpu_info.o
obj-y	+= pinmux.o
obj-y	+= rtc.o
obj-n	+= usb_phy.o
obj-$(CONFIG_SUN6I_P2WI)	+= p2wi.o
obj-$(CONFIG_SUN6I_PRCM)	+= prcm.o
obj-$(CONFIG_AXP_PMIC_BUS)	+= pmic_bus.o
obj-$(CONFIG_SUN8I_RSB)		+= rsb.o
obj-$(CONFIG_MACH_SUN50IW3)	+= clock_sun50iw3.o
obj-$(CONFIG_MACH_SUN50IW5)	+= clock_sun50iw5.o
obj-$(CONFIG_MACH_SUN50IW9)	+= clock_sun50iw9.o board_sun50iw9.o
obj-$(CONFIG_MACH_SUN50IW10)    += clock_sun50iw10.o board_sun50iw10.o
obj-$(CONFIG_MACH_SUN55IW3)     += clock_sun55iw3.o
obj-$(CONFIG_MACH_SUN50IW11)    += clock_sun50iw11.o
obj-$(CONFIG_MACH_SUN50IW12)    += clock_sun50iw12.o
obj-$(CONFIG_MACH_SUN8IW15)     += clock_sun8iw15.o
obj-$(CONFIG_MACH_SUN8IW16)	+= clock_sun8iw16.o
obj-$(CONFIG_MACH_SUN8IW18)	+= clock_sun8iw18.o board_sun8iw18.o
obj-$(CONFIG_MACH_SUN8IW19)	+= clock_sun8iw19.o
obj-$(CONFIG_MACH_SUN8IW7) += clock_sun8iw7.o
obj-$(CONFIG_MACH_SUN8IW11) += clock_sun8iw11.o
obj-$(CONFIG_MACH_SUN20IW1) += clock_sun20iw1.o
obj-$(CONFIG_MACH_SUN8IW20) += clock_sun8iw20.o
obj-$(CONFIG_MACH_SUN8IW21) += clock_sun8iw21.o

subdir-ccflags-$(CONFIG_SUNXI_TRACE) += -finstrument-functions