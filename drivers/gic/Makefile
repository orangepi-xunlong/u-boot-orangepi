#
# Copyright (c) 2013 Samsung Electronics Co., Ltd.
# 	http://www.samsung.com
#
# SPDX-License-Identifier:	GPL-2.0+
#


ifdef CONFIG_ARCH_SUN3IW1P1
obj-y	+= sunxi_gic_sun3iw1p1.o
else
obj-y	+= sunxi_gic.o
endif

subdir-ccflags-$(CONFIG_SUNXI_FINS_FUNC_DRIVER_GIC_DIR) += -finstrument-functions
