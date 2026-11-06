#
# Copyright (c) 2013-2017, ARM Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

# Include the CPU specific operations makefile
include lib/cpus/${BOOT_CPU}/cpu.mk

BL2_INCLUDES += \
	-Ibuild \
	-Iinclude/ \
	-Iinclude/arch \
	-Iinclude/arch/${ARCH} \
	-Iinclude/lib \
	-Iinclude/bl2 \
	-Iinclude/common \
	-Iinclude/drivers \
	-Iinclude/drivers/cvitek/${CHIP_ARCH} \
	${CPU_INCLUDES} \
	-Iinclude/bl2/uart \
	-Iplat/cvitek/${CHIP_ARCH}/${SUBTYPE}/include \
	-Iplat/cvitek/${CHIP_ARCH}/common/include \
	-Iinclude/bl2/emmc \
	-Iinclude/bl2/${BOOT_CPU} \
	-Ilib/ \
	-Ilib/utils \
	-Ilib/lzma \
	-Ilib/lz4 \
	-Ilib/cli_simple \
	${STDLIB_INCLUDES_V2} \
	${CRYPT_INCLUDES}

BL2_COMMON_SOURCES = \
	${CPU_SOURCES} \
	common/tf_printf.c \
	plat/cvitek/${CHIP_ARCH}/common/cv_bl2_setup.c \
	plat/cvitek/${CHIP_ARCH}/common/bl2_load.c \
	drivers/cvitek/security/security.c \
	drivers/cvitek/emmc/emmc.c \
	drivers/cvitek/emmc/cv_emmc.c \
	${STDLIB_SRCS_v2} \
	${CRYPT_SOURCES}

ifeq (${BL2_CLI_SIMPLE},1)
BL2_COMMON_SOURCES += \
	lib/cli_simple/cli_readline.c \
	lib/cli_simple/cli_simple.c \
	lib/cli_simple/strto.c \
	lib/cli_simple/bm_ddr.c \
	lib/cli_simple/memtest.c \
	lib/cli_simple/tempsen.c \
	lib/cli_simple/a2_ddr_test.c

endif

DECOMPRESSION_SOURCES = \
	lib/utils/decompress.c \
	lib/lzma/LzmaDec.c \
	lib/lz4/lz4_all.c \
	lib/lz4/xxhash.c

BL2_SRCS = \
	${BL2_COMMON_SOURCES} \
	${DECOMPRESSION_SOURCES}


#don't edit BL2_SOURCES
BL2_SOURCES = \
	${BL2_CPU_SOURCES} \
	${BL2_SRCS} \
	bl2/bl2_main.c

include drivers/cvitek/ddr/ddr.mk

BL2_LINKERFILE := bl2/bl2.ld.S

################################################################################
# BL2 Compilation Options
################################################################################
BL2_CPPFLAGS += \
	${DEFINES} ${BL2_INCLUDES} \
	-nostdinc \
	-Wmissing-include-dirs -Werror

BL2_CFLAGS += -ggdb3 -gdwarf-2 -fno-stack-protector
BL2_ASFLAGS += -g -Wa,--gdwarf-2
