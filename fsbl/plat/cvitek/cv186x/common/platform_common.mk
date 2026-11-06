#
# Copyright (c) 2013-2017, ARM Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

ENABLE_COMPRESSION:=1
NANDBOOT_V2:=1
BL1_SEPARATE_CODE_AND_RODATA:=1
ARM_CRYPTOCELL_INTEG:=0
MBEDTLS_DIR:=third_party/mbedtls-mbedtls-2.12.0
CRYPTO_LIB_MK := drivers/auth/mbedtls/mbedtls_crypto.mk
$(info Including ${CRYPTO_LIB_MK})
include ${CRYPTO_LIB_MK}

TEST_FROM_SPINOR1 := 0

DEFINES += -DLZ4_USER_MEMORY_FUNCTIONS=1

ifeq ($(FSBL_SECURE_BOOT_SUPPORT),1)
DEFINES += \
	-DNO_ALLOCS \
	-DARGTYPE=3 \
	-DLTC_NO_FILE \
	-DLTM_NO_FILE \
	-DLTM_DESC \
	-DLTC_SOURCE

CRYPT_INCLUDES := \
	-Ilib/libtommath \
	-Ilib/libtomcrypt/src/headers \
	-Ilib/BigDigits

CRYPT_SOURCES := \
	lib/BigDigits/bigdigits.c \
	lib/libtomcrypt/src/hashes/sha2/sha256.c
endif

include lib/stdlib_v2/stdlib_v2.mk
include bl2/bl2.mk

PLAT_INCLUDES		:=	-Iinclude/plat/arm/common/		\
				-Ibuild \
				-Iinclude/plat/arm/common/aarch64/	\
				-Iinclude/drivers/cvitek/$(CHIP_ARCH)/emmc		\
				-Iinclude/drivers/cvitek/$(CHIP_ARCH)/usb		\
				-Iinclude/drivers/cvitek/$(CHIP_ARCH)/spinor	\
				-Iinclude/drivers/cvitek/$(CHIP_ARCH)/i2c		\
				-Iinclude/drivers/cvitek/$(CHIP_ARCH)/ddr		\
				-Iinclude/drivers/cvitek/$(CHIP_ARCH)/spinand		\
				-Iinclude/common/tbbr			\
				-Ilib/crc				\
				-Iplat/cvitek/${CHIP_ARCH}/common/include	\
				-Iplat/cvitek/${CHIP_ARCH}/${SUBTYPE}/include

PLAT_BL_COMMON_SOURCES	:=	\
				plat/cvitek/${CHIP_ARCH}/common/board_cv_trusted_boot.c	\
				drivers/cvitek/uart/uart_dw.c		\
				lib/xlat_tables/xlat_tables_common.c		\
				lib/xlat_tables/aarch64/xlat_tables.c


BL31_SOURCES		+=	lib/cpus/aarch64/aem_generic.S		\
				lib/cpus/aarch64/cortex_a53.S		\
				drivers/arm/gic/v2/gicv2_helpers.c	\
				drivers/arm/gic/v2/gicv2_main.c		\
				drivers/arm/gic/common/gic_common.c	\
				drivers/cvitek/timer/timer_dw.c  \
				plat/common/aarch64/plat_psci_common.c	\
				plat/common/plat_gicv2.c		\
				plat/arm/common/arm_gicv2.c		\
				drivers/delay_timer/${BOOT_CPU}/delay_timer.c	\
				plat/cvitek/${CHIP_ARCH}/common/cv_pm.c	\
				plat/cvitek/${CHIP_ARCH}/common/topology.c	\
				plat/cvitek/${CHIP_ARCH}/common/aarch64/plat_helpers.S	\
				plat/cvitek/${CHIP_ARCH}/${SUBTYPE}/aarch64/plat_suspend.S	\
				plat/cvitek/${CHIP_ARCH}/common/cv_bl31_setup.c

CV_LF_EK            = plat/cvitek/$(CHIP_ARCH)/common/keys/cv_license_file_key.bin
    $(eval $(call add_define_val,CV_LF_EK,'"$(CV_LF_EK)"'))

# BL32
BL32_ROOT		:=	bl32/cvsp
include ${BL32_ROOT}/cvsp.mk
#NEED_BL32		:=	yes
BMSP_NS_INTR_ASYNC_PREEMPT	:=	0
BL32_SOURCES		+=	plat/cvitek/${CHIP_ARCH}/common/cv_bl32_setup.c

# Disable the PSCI platform compatibility layer
ENABLE_PLAT_COMPAT	:= 	0

# Enable workarounds for selected Cortex-A53 errata
ERRATA_A53_835769	:=	1
ERRATA_A53_843419	:=	1
ERRATA_A53_855873	:=	1
