#
# Copyright (c) 2013-2016, ARM Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
USE_COHERENT_MEM := 1
INCLUDES		+=	-Iinclude/bl32/cvsp \
				-Iservices/spd/opteed \
				-Iinclude/arch/	\
				-Iinclude/drivers/cvitek/$(CHIP_ARCH)/tpu

BL32_SOURCES	+=	bl32/cvsp/cvsp_main.c			\
				bl32/cvsp/aarch64/cvsp_entrypoint.S	\
				bl32/cvsp/aarch64/cvsp_exceptions.S	\
				bl32/cvsp/cvsp_interrupt.c		\
				bl32/cvsp/cvsp_timer.c			\
				drivers/cvitek/timer/timer_dw.c  \
				common/aarch64/early_exceptions.S	\
				lib/locks/exclusive/aarch64/spinlock.S

BL32_SOURCES	+= drivers/delay_timer/${BOOT_CPU}/delay_timer.c		\
				bl32/cvsp/cvsp_private.c			\
				bl32/cvsp/cvsp_debug.c				\

ifeq ($(ENABLE_TPU_SECURITY),1)
BL32_SOURCES	+= lib/cli_simple/strto.c				\
				plat/cvitek/${CHIP_ARCH}/common/board_cv_trusted_boot.c \
				third_party/tinyscheme/scheme.c
endif

BL32_LINKERFILE		:=	bl32/cvsp/cvsp.ld.S

# This flag determines if the BMSPD initializes BL32 in bmspd_init() (synchronous
# method) or configures BL31 to pass control to BL32 instead of BL33
# (asynchronous method).
BMSP_INIT_ASYNC         :=      0

$(eval $(call assert_boolean,BMSP_INIT_ASYNC))
$(eval $(call add_define,BMSP_INIT_ASYNC))

include plat/cvitek/${CHIP_ARCH}/common/cvsp/cv_sp.mk

BL32_CPPFLAGS += \
				${DEFINES} ${INCLUDES} \
				-nostdinc \
				-Wmissing-include-dirs -Werror

BL32_ASFLAGS  += $(BL32_CPPFLAGS) $(ASFLAGS_$(ARCH))			\
				-D__ASSEMBLY__ -ffreestanding 			\
				-Wa,--fatal-warnings

BL32_CFLAGS  +=	$(BL32_CPPFLAGS) $(BL31_CFLAGS_$(ARCH))		\
				-ffreestanding -fno-builtin -Wall -std=gnu99	\
				-Os -ffunction-sections -fdata-sections		\
				-fno-delete-null-pointer-checks

