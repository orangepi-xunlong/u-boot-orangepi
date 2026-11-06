#
# Copyright (c) 2013-2017, ARM Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

include lib/psci/psci_lib.mk

INCLUDES		+=	-Ithird_party/tinyscheme \
					-Iinclude/bl31 \
					-Iinclude/arch/ \
					${STDLIB_INCLUDES_V2}

BL31_SOURCES		+=	common/bl_common.c	\
				bl31/bl31_main.c				\
				bl31/interrupt_mgmt.c				\
				bl31/aarch64/bl31_entrypoint.S			\
				bl31/aarch64/runtime_exceptions.S		\
				bl31/aarch64/crash_reporting.S			\
				bl31/bl31_context_mgmt.c			\
				common/runtime_svc.c				\
				plat/common/aarch64/platform_mp_stack.S		\
				services/std_svc/std_svc_setup.c		\
				${PSCI_LIB_SOURCES}

BL31_SOURCES		+=	$(addprefix ${MBEDTLS_DIR}/library/,	\
				memory_buffer_alloc.c 			\
				platform.c 				\
				platform_util.c                         \
				)

BL31_SOURCES		+=	third_party/tinyscheme/scheme.c \
				bl31/bl31_license_file.c

ifeq (${ENABLE_PMF}, 1)
BL31_SOURCES		+=	lib/pmf/pmf_main.c
endif

BL31_LINKERFILE		:=	bl31/bl31.ld.S

# Flag used to indicate if Crash reporting via console should be included
# in BL31. This defaults to being present in DEBUG builds only
ifndef CRASH_REPORTING
CRASH_REPORTING		:=	$(DEBUG)
endif

$(eval $(call assert_boolean,CRASH_REPORTING))
$(eval $(call add_define,CRASH_REPORTING))

################################################################################
# Common sources and include directories
################################################################################

BL31_COMMON_SOURCES	+= plat/cvitek/${CHIP_ARCH}/common/cv_common.c	\
				common/tf_printf.c			\
				common/tf_snprintf.c			\
				common/${ARCH}/debug.S			\
				lib/${ARCH}/cache_helpers.S		\
				lib/${ARCH}/misc_helpers.S		\
				plat/common/${ARCH}/plat_common.c	\
				plat/common/${ARCH}/platform_helpers.S	\
				${STDLIB_SRCS_v2}

################################################################################
# BL31 Compilation Options
################################################################################

BL31_ASFLAGS += -g -Wa,--gdwarf-2

ifeq ($(notdir $(CC)),armclang)
BL31_CFLAGS_aarch32	=	-target arm-arm-none-eabi -march=armv8-a
BL31_CFLAGS_aarch64	=	-target aarch64-arm-none-eabi -march=armv8-a
else ifneq ($(findstring clang,$(notdir $(CC))),)
BL31_CFLAGS_aarch32	=	-target armv8a-none-eabi
BL31_CFLAGS_aarch64	=	-target aarch64-elf
else
BL31_CFLAGS_aarch32	=	-march=armv8-a
BL31_CFLAGS_aarch64	=	-march=armv8-a
endif

BL31_CFLAGS_aarch64	+=	-mstrict-align
BL31_CFLAGS_aarch64	+=	-fno-stack-protector -Wno-error=array-bounds

ASFLAGS_aarch32		=	-march=armv8-a
ASFLAGS_aarch64		=	-march=armv8-a

BL31_CPPFLAGS += \
				${DEFINES} ${INCLUDES} \
				-nostdinc \
				-Wmissing-include-dirs -Werror

BL31_ASFLAGS	+=	$(BL31_CPPFLAGS) $(ASFLAGS_$(ARCH))			\
				-D__ASSEMBLY__ -ffreestanding 			\
				-Wa,--fatal-warnings

BL31_CFLAGS		+=	$(BL31_CPPFLAGS) $(BL31_CFLAGS_$(ARCH))		\
				-ffreestanding -fno-builtin -Wall -std=gnu99	\
				-Os -ffunction-sections -fdata-sections		\
				-fno-delete-null-pointer-checks

ifeq (${ERROR_DEPRECATED},0)
    BL31_CFLAGS		+= 	-Wno-error=deprecated-declarations
endif
