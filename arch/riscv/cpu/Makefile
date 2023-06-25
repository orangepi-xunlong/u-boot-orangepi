# SPDX-License-Identifier: GPL-2.0+
#
# Copyright (C) 2018, Bin Meng <bmeng.cn@gmail.com>

extra-y = start.o
ifeq ($(CONFIG_ARCH_SUNXI),y)
extra-y	+= spare_head.o
endif
obj-y += cpu.o mtrap.o
