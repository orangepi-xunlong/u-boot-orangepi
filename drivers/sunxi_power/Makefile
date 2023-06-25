# SPDX-License-Identifier: GPL-2.0+
#
# Copyright (c) 2009 Wind River Systems, Inc.
# Tom Rix <Tom.Rix at windriver.com>
obj-$(CONFIG_SUNXI_POWER)   	+= axp.o
obj-$(CONFIG_SUNXI_PMU)		+= pmu.o
obj-$(CONFIG_SUNXI_BMU)		+= bmu.o
#PMU
ifdef CONFIG_SUNXI_PMU
obj-$(CONFIG_AXPNULL_POWER)	+= pmu_axpnull.o
obj-$(CONFIG_AXP858_POWER)      += pmu_axp858.o
obj-$(CONFIG_AXP2101_POWER)      += pmu_axp2101.o
obj-$(CONFIG_AXP2202_POWER)      += pmu_axp2202.o
obj-$(CONFIG_SUNXI_AXP152_POWER)      += pmu_axp152.o
obj-$(CONFIG_AXP1530_POWER)      += pmu_axp1530.o
obj-$(CONFIG_AXP81X_POWER)	+= pmu_axp81X.o
obj-$(CONFIG_AXP806_POWER)      += pmu_axp806.o
obj-$(CONFIG_AXP221_POWER_SUNXI)      += pmu_axp221.o
endif
#BMU
ifdef CONFIG_SUNXI_BMU
obj-$(CONFIG_AXP2585_POWER)     += bmu_axp2585.o
obj-$(CONFIG_AXP2101_POWER)      += bmu_axp2101.o
obj-$(CONFIG_AXP2202_POWER)      += bmu_axp2202.o
obj-$(CONFIG_AXP81X_POWER)      += bmu_axp81X.o
obj-$(CONFIG_SUNXI_AXP152_POWER)	+= bmu_axp152.o
obj-$(CONFIG_AXPNULL_POWER)     += bmu_axpnull.o
obj-$(CONFIG_AXP221_POWER_SUNXI)      += bmu_axp221.o
endif
