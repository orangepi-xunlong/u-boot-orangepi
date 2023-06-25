# SPDX-License-Identifier: GPL-2.0+
#
# (C) Copyright 2018 Allwinner Technology.
# wangwei <wangwei@allwinnertech.com>
obj-$(CONFIG_SUNXI_UBIFS) += nand_for_ubi.o

obj-$(CONFIG_SUNXI_NAND) += nand.o nand_for_uboot.o
obj-$(CONFIG_SUNXI_COMM_NAND) += common0/nand_osal_uboot.o common0/nand_uboot_fun.o common0/nand_lib_cfg.o

ifeq ($(CONFIG_MACH_SUN8IW18),y)
obj-$(CONFIG_SUNXI_COMM_NAND_V1) += common1/nand_osal_uboot.o common1/nand_uboot_fun.o common1/nand_lib_cfg.o common1/nand_for_clock_v0.o common1/nand_for_clock_v1.o
obj-$(CONFIG_SUNXI_COMM_NAND_V1) += common1/spinand/

else ifeq ($(CONFIG_MACH_SUN8IW7),y)
obj-$(CONFIG_SUNXI_COMM_NAND_V1) += common1/nand_osal_uboot.o common1/nand_uboot_fun.o common1/nand_lib_cfg.o common1/nand_for_clock_v0.o common1/nand_for_clock_v1.o
obj-$(CONFIG_MACH_SUN8IW7) += sun8iw7p1/

else ifeq ($(CONFIG_SUNXI_RTOS),y)
obj-$(CONFIG_SUNXI_COMM_NAND_V1) += common1/nand_osal_uboot.o common1/nand_uboot_fun.o common1/nand_lib_cfg.o common1/nand_for_clock_v0.o common1/nand_for_clock_v1.o
else
obj-$(CONFIG_SUNXI_COMM_NAND_V1) += common1/
endif


