#
# Copyright (c) 2009 Wind River Systems, Inc.
# Tom Rix <Tom.Rix at windriver.com>
#
# SPDX-License-Identifier:	GPL-2.0+
#

obj-$(CONFIG_EXYNOS_TMU)	+= exynos-tmu.o
obj-$(CONFIG_FTPMU010_POWER)	+= ftpmu010.o
obj-$(CONFIG_TPS6586X_POWER)	+= tps6586x.o
obj-$(CONFIG_TWL4030_POWER)	+= twl4030.o
obj-$(CONFIG_TWL6030_POWER)	+= twl6030.o
obj-$(CONFIG_PALMAS_POWER)	+= palmas.o

obj-$(CONFIG_POWER) += power_core.o
obj-$(CONFIG_DIALOG_POWER) += power_dialog.o
obj-$(CONFIG_POWER_FSL) += power_fsl.o
obj-$(CONFIG_POWER_I2C) += power_i2c.o
obj-$(CONFIG_POWER_SPI) += power_spi.o

obj-y += sunxi/axp_null.o sunxi/axp_supply_null.o

obj-y += sunxi/axp.o
ifdef CONFIG_SUNXI_MODULE_AXP
obj-$(CONFIG_SUNXI_AXP81X) += sunxi/axp81X.o
obj-$(CONFIG_SUNXI_AXP81X) += sunxi/axp81X_supply.o
obj-$(CONFIG_SUNXI_AXP15) += sunxi/axp15.o
obj-$(CONFIG_SUNXI_AXP15) += sunxi/axp15_supply.o
obj-$(CONFIG_SUNXI_AXP20) += sunxi/axp20.o
obj-$(CONFIG_SUNXI_AXP20) += sunxi/axp20_supply.o
obj-$(CONFIG_SUNXI_AXP22) += sunxi/axp22.o
obj-$(CONFIG_SUNXI_AXP22) += sunxi/axp22_supply.o
obj-$(CONFIG_SUNXI_AXP806) += sunxi/axp806.o
obj-$(CONFIG_SUNXI_AXP806) += sunxi/axp806_supply.o
obj-$(CONFIG_SUNXI_AXP809) += sunxi/axp809.o
obj-$(CONFIG_SUNXI_AXP809) += sunxi/axp809_supply.o
obj-$(CONFIG_SUNXI_AXP259) += sunxi/axp259.o
obj-$(CONFIG_SUNXI_AXP259) += sunxi/axp259_supply.o
obj-$(CONFIG_SUNXI_AXP858) += sunxi/axp858.o
obj-$(CONFIG_SUNXI_AXP858) += sunxi/axp858_supply.o
obj-$(CONFIG_SUNXI_AXP2585) += sunxi/axp2585.o
obj-$(CONFIG_SUNXI_AXP2585) += sunxi/axp2585_supply.o
endif

subdir-ccflags-$(CONFIG_SUNXI_FINS_FUNC_DRIVER_POWER_DIR) += -finstrument-functions
