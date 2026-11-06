#
# Copyright (c) 2015, ARM Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
INCLUDES		+=	-Ibl32/cvsp

BL32_SOURCES		+=	plat/common/plat_gicv2.c		\
				plat/common/aarch64/platform_mp_stack.S	\
				plat/arm/common/arm_gicv2.c		\
				drivers/arm/gic/v2/gicv2_helpers.c	\
				drivers/arm/gic/v2/gicv2_main.c		\
				drivers/arm/gic/common/gic_common.c	\
				plat/cvitek/${CHIP_ARCH}/common/topology.c			\
				plat/cvitek/${CHIP_ARCH}/common/aarch64/plat_helpers.S	\
				plat/cvitek/${CHIP_ARCH}/common/cvsp/cv_sp_setup.c

ifeq ($(ENABLE_TPU_SECURITY),1)
BL32_SOURCES		+=	drivers/cvitek/$(CHIP_ARCH)/tpu/tpu_security.c	\
				drivers/cvitek/$(CHIP_ARCH)/tpu/tpu_driver.c		\
				drivers/cvitek/$(CHIP_ARCH)/tpu/tpu_dmabuf.c
endif

BL32_SOURCES		+=	${MBEDTLS_CRYPTO_SOURCES}
BL32_SOURCES		+=	${MBEDTLS_COMMON_SOURCES}

BL32_SOURCES		+= $(addprefix ${MBEDTLS_DIR}/library/,	\
					gcm.c				\
					cipher.c            \
					cipher_wrap.c       \
					)
ifeq ($(ENABLE_TPU_SECURITY),1)
BL32_SOURCES		+= $(addprefix ${MBEDTLS_DIR}/library/,	hkdf.c)
endif
