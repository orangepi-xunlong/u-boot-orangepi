# SPDX-License-Identifier: GPL-2.0+
#
# Copyright (C) 2022 StarFive, Inc

ifeq ($(CONFIG_SPL_BUILD),y)
obj-y += spl.o
else
obj-y += cpu.o
obj-$(CONFIG_STARFIVE_JH7110_L2CC_FLUSH) += cache.o
endif
obj-y += dram.o
obj-y += pll.o
