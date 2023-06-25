#
# (C) Copyright 2013-2016
# Allwinner Technology Co., Ltd. <www.allwinnertech.com>
#
# SPDX-License-Identifier:     GPL-2.0+
#
ifdef CONFIG_SUNXI_CE_21
obj-$(CONFIG_SUNXI_CE_DRIVER)   += sunxi_crypto_2.1.o
obj-$(CONFIG_SUNXI_CE_DRIVER)	+= ss_op_2.1.o
else ifdef CONFIG_SUNXI_CE_23
obj-$(CONFIG_SUNXI_CE_DRIVER)   += sunxi_crypto_2.3.o
obj-$(CONFIG_SUNXI_CE_DRIVER)	+= ss_op_2.3.o
else
obj-$(CONFIG_SUNXI_CE_DRIVER)	+= sunxi_crypto.o
obj-$(CONFIG_SUNXI_CE_DRIVER)	+= ss_op.o
endif

obj-y	+= sunxi_efuse.o
