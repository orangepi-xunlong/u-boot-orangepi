#
# (C) Copyright 2006
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# SPDX-License-Identifier:	GPL-2.0+
#

obj-$(CONFIG_FSLDMAFEC) += MCD_tasksInit.o MCD_dmaApi.o MCD_tasks.o
obj-$(CONFIG_APBH_DMA) += apbh_dma.o
obj-$(CONFIG_FSL_DMA) += fsl_dma.o
obj-$(CONFIG_OMAP3_DMA) += omap3_dma.o

ifdef CONFIG_ARCH_SUN3IW1P1
obj-y += sunxi_dma_sun3iw1p1.o
else
obj-y += sunxi_dma.o
endif

subdir-ccflags-$(CONFIG_SUNXI_FINS_FUNC_DRIVER_DMA_DIR) += -finstrument-functions
