.PHONY: FORCE
# Default goal is build all images
.DEFAULT_GOAL := all

# Avoid any implicit propagation of command line variable definitions to
# sub-Makefiles, like CFLAGS that we reserved for the firmware images'
# usage. Other command line options like "-s" are still propagated as usual.
MAKEOVERRIDES =

ifeq ($(CHIP_ARCH),SOPHON)
CHIP := cv186x
endif
CHIP_ARCH=${CHIP}

#include $(BUILD_PATH)/.config

S := $(shell printf '\033[1;34;40m  MAKECMDGOALS %s \033[0m\n' '${MAKECMDGOALS}')
$(info ${S})

ARCH ?=

#$(info ===========$(CROSS_COMPILE))
#ifneq ($(origin CROSS_COMPILE),command line)
ifeq ($(ARCH),riscv)
CROSS_COMPILE := ${CROSS_COMPILE_GLIBC_RISCV64}
BOOT_CPU ?= riscv
else
CROSS_COMPILE := ${CROSS_COMPILE_64}
BOOT_CPU := aarch64
ARCH := aarch64
endif
#endif

ifeq (${CHIP_ARCH},)
$(error CHIP_ARCH is undefined)
endif

ifeq (${CROSS_COMPILE},)
$(error CROSS_COMPILE is undefined)
endif


DEFINES += -DCHIP=$(CHIP)
################################################################################
# Default values for build configurations, and their dependencies
################################################################################
MAKE_HELPERS_DIRECTORY := make_helpers/

V ?= 0
DEBUG := 0
LOG_LEVEL := 2
ENABLE_ASSERTIONS := 1
PRINTF_TIMESTAMP := 0
BL2_CLI_SIMPLE := 0
NANDBOOT_V2 := 1

ifeq (${BL2_CLI_SIMPLE},1)
$(eval $(call add_define,BL2_USE_CLI))
endif

# Verbose flag
ifeq (${V},0)
        Q:=@
else
        Q:=
endif
export Q

PRINTABLE	:= abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789
PRINTABLE_TR	:= v1JfTUIeqdsE7P0oiczuVbW9aLnOFySG5YtDQ3lHN4rRpZABwmkCg2XhjM6x8K
CHIP_ARCH	:= $(shell echo '${CHIP_ARCH}' | tr A-Z a-z)
CHIP_ARCH_ALT	:= $(shell echo '${CHIP_ARCH}' | tr '${PRINTABLE}' '${PRINTABLE_TR}')
BUILD_BASE	:= ./build

O ?= ${BUILD_BASE}/${CHIP_ARCH}
BUILD_PLAT	= ${O}

BUILD_STRING := g$(shell git rev-parse --short HEAD 2> /dev/null)
BUILD_STRING := ${BUILD_STRING}$(shell if git diff-index --name-only HEAD | grep -q "."; then echo -dirty; fi)
VERSION_STRING := ${CHIP_ARCH}:${BUILD_STRING}

ifeq ($(CHIP_ARCH),$(filter $(CHIP_ARCH),cv183x ))
DEFINES += -D__CVITEK__
# TPU model decryption
ENABLE_TPU_SECURITY:=1
BOOTFLOW_TYPE:=0
else ifeq ($(CHIP_ARCH),$(filter $(CHIP_ARCH),cv182x  cv181x cv186x))
DEFINES += -D__CVITEK__
ENABLE_TPU_SECURITY:=0
BOOTFLOW_TYPE:=1
endif

ifeq (${CRC}, 1)
	DEFINES += -DADD_CRC
endif

# Add BLD to fip.bin
IMG_BLD?=0
ifeq (${IMG_BLD}, 1)
	ADD_BLD := yes
	DEFINES += -DLOAD_BLD
endif

################################################################################
# Toolchain
################################################################################
HOSTCC			:=	gcc
export HOSTCC

CC			:=	${CROSS_COMPILE}gcc
CPP			:=	${CROSS_COMPILE}cpp
AS			:=	${CROSS_COMPILE}gcc
AR			:=	${CROSS_COMPILE}ar
LD			:=	${CROSS_COMPILE}ld
OC			:=	${CROSS_COMPILE}objcopy
OD			:=	${CROSS_COMPILE}objdump
NM			:=	${CROSS_COMPILE}nm
PP			:=	${CROSS_COMPILE}gcc -E
READELF			:=	${CROSS_COMPILE}readelf
GDB			:=	${CROSS_COMPILE}gdb

################################################################################
# Generic definitions
################################################################################
include ${MAKE_HELPERS_DIRECTORY}unix.mk
include ${MAKE_HELPERS_DIRECTORY}build_macros.mk
include ${MAKE_HELPERS_DIRECTORY}defaults.mk

# Common includes #
INCLUDES		+= \
				-Iinclude/common			\
				-Iinclude/common/${ARCH}		\
				-Iinclude/drivers			\
				-Iinclude/drivers/arm			\
				-Iinclude/drivers/auth			\
				-Iinclude/drivers/io			\
				-Iinclude/drivers/ti/uart		\
				-Iinclude/lib				\
				-Iinclude/lib/${ARCH}			\
				-Iinclude/lib/cpus			\
				-Iinclude/lib/cpus/${ARCH}		\
				-Iinclude/lib/el3_runtime		\
				-Iinclude/lib/el3_runtime/${ARCH}	\
				-Iinclude/lib/pmf			\
				-Iinclude/lib/psci			\
				-Iinclude/lib/xlat_tables		\
				-Iinclude/plat/common			\
				-Iinclude/services			\
				-Ilib/lzma                  \
				-Ilib/lz4                   \
				${PLAT_INCLUDES}			\
				${SPD_INCLUDES}				\
				-Iinclude/tools_share

################################################################################
# For AArch64, BL31 is  supported.
################################################################################
ifeq (${BOOT_CPU},aarch64)
# When booting an EL3 payload, there is no need to compile the BL31 image nor
# put it in the FIP.
ifndef EL3_PAYLOAD_BASE
NEED_BL31 := yes
include bl31/bl31.mk
endif
else
NEED_BL31 := no
endif

################################################################################
# Generic definitions
################################################################################

include ${MAKE_HELPERS_DIRECTORY}plat_helpers.mk

PLAT                    :=      ${CHIP}_${SUBTYPE}

SPDS			:=	$(sort $(filter-out none, $(patsubst services/spd/%,%,$(wildcard services/spd/*))))

# Platforms providing their own TBB makefile may override this value
INCLUDE_TBBR_MK		:=	1               

################################################################################
# Include SPD Makefile if one has been specified
################################################################################
ifneq (${ARCH},riscv)
SPD := opteed
endif
ifneq (${SPD},none)
ifeq (${ARCH},aarch32)
	$(error "Error: SPD is incompatible with AArch32.")
endif
ifdef EL3_PAYLOAD_BASE
        $(warning "SPD and EL3_PAYLOAD_BASE are incompatible build options.")
        $(warning "The SPD and its BL32 companion will be present but ignored.")
endif
        # We expect to locate an spd.mk under the specified SPD directory
        SPD_MAKE	:=	$(wildcard services/spd/${SPD}/${SPD}.mk)

        ifeq (${SPD_MAKE},)
                $(error Error: No services/spd/${SPD}/${SPD}.mk located)
        endif
        $(info Including ${SPD_MAKE})
        include ${SPD_MAKE}
        # If there's BL32 companion for the chosen SPD, we expect that the SPD's
        # Makefile would set NEED_BL32 to "yes". In this case, the build system
        # supports two mutually exclusive options:
        # * BL32 is built from source: then BL32_SOURCES must contain the list
        #   of source files to build BL32
        # * BL32 is a prebuilt binary: then BL32 must point to the image file
        #   that will be included in the FIP
        # If both BL32_SOURCES and BL32 are defined, the binary takes precedence
        # over the sources.
endif

################################################################################
# Include libraries' Makefile that are used in all BL
################################################################################

include lib/stack_protector/stack_protector.mk


# Include the CPU specific operations makefile, which provides default
# values for all CPU errata workarounds and CPU specific optimisations.
# This can be overridden by the platform.

include lib/cpus/cpu-ops.mk
ifeq (${ARCH},aarch32)
NEED_BL32 := yes
################################################################################
# Build `AARCH32_SP` as BL32 image for AArch32
################################################################################
ifneq (${AARCH32_SP},none)
# We expect to locate an sp.mk under the specified AARCH32_SP directory
AARCH32_SP_MAKE	:=	$(wildcard bl32/${AARCH32_SP}/${AARCH32_SP}.mk)

ifeq (${AARCH32_SP_MAKE},)
  $(error Error: No bl32/${AARCH32_SP}/${AARCH32_SP}.mk located)
endif

$(info Including ${AARCH32_SP_MAKE})
include ${AARCH32_SP_MAKE}
endif

endif


# Process TBB related flags
ifneq (${GENERATE_COT},0)
        FIP_DEPS += certificates

        # Common cert_create options
        ifneq (${CREATE_KEYS},0)
                $(eval CRT_ARGS += -n)
                $(eval FWU_CRT_ARGS += -n)
                ifneq (${SAVE_KEYS},0)
                        $(eval CRT_ARGS += -k)
                        $(eval FWU_CRT_ARGS += -k)
                endif
        endif
        # Include TBBR makefile (unless the platform indicates otherwise)
        ifeq (${INCLUDE_TBBR_MK},1)
                include make_helpers/tbbr/tbbr_tools.mk
        endif
endif

ifneq (${FIP_ALIGN},0)
FIP_ARGS += --align ${FIP_ALIGN}
endif

################################################################################
# Auxiliary tools (fiptool, cert_create, etc)
################################################################################

# Variables for use with Certificate Generation Tool
CRTTOOLPATH		?=	tools/cert_create
CRTTOOL			?=	${CRTTOOLPATH}/cert_create${BIN_EXT}

################################################################################
# bl31/bl32 secure boot
################################################################################
FSBL_SECURE_BOOT_SUPPORT := 0

################################################################################
# Convert building option
################################################################################
FSBL_SECURE_BOOT_SUPPORT := $(call yn10,${FSBL_SECURE_BOOT_SUPPORT})

################################################################################
# CPU and platform
################################################################################
$(call print_var,CHIP_ARCH)
$(call print_var,BOOT_CPU)

include ${PLAT_MAKEFILE_FULL}

# Check CPU define
$(eval $(call add_define,$(shell echo '${BOOT_CPU}' | tr a-z A-Z)))
ifeq (${BOOT_CPU},aarch64)

else ifeq (${BOOT_CPU},riscv)
else
$(error "BOOT_CPU=${BOOT_CPU} is not supported")
endif

ifeq (${STORAGE_TYPE},emmc)
$(eval $(call add_define,BOOT_FROM_EMMC))
endif

################################################################################
# Build options checks
################################################################################

$(eval $(call assert_boolean,COLD_BOOT_SINGLE_CPU))
$(eval $(call assert_boolean,CREATE_KEYS))
$(eval $(call assert_boolean,CTX_INCLUDE_AARCH32_REGS))
$(eval $(call assert_boolean,CTX_INCLUDE_FPREGS))
$(eval $(call assert_boolean,DEBUG))
$(eval $(call assert_boolean,DISABLE_PEDANTIC))
$(eval $(call assert_boolean,ENABLE_ASSERTIONS))
$(eval $(call assert_boolean,ENABLE_PLAT_COMPAT))
$(eval $(call assert_boolean,ENABLE_PMF))
$(eval $(call assert_boolean,ENABLE_PSCI_STAT))
$(eval $(call assert_boolean,ENABLE_RUNTIME_INSTRUMENTATION))
$(eval $(call assert_boolean,ERROR_DEPRECATED))
$(eval $(call assert_boolean,GENERATE_COT))
$(eval $(call assert_boolean,HW_ASSISTED_COHERENCY))
$(eval $(call assert_boolean,LOAD_IMAGE_V2))
$(eval $(call assert_boolean,NS_TIMER_SWITCH))
$(eval $(call assert_boolean,PL011_GENERIC_UART))
$(eval $(call assert_boolean,PROGRAMMABLE_RESET_ADDRESS))
$(eval $(call assert_boolean,PSCI_EXTENDED_STATE_ID))
$(eval $(call assert_boolean,RESET_TO_BL31))
$(eval $(call assert_boolean,SAVE_KEYS))
$(eval $(call assert_boolean,SEPARATE_CODE_AND_RODATA))
$(eval $(call assert_boolean,BL1_SEPARATE_CODE_AND_RODATA))
$(eval $(call assert_boolean,SPIN_ON_BL1_EXIT))
$(eval $(call assert_boolean,TRUSTED_BOARD_BOOT))
$(eval $(call assert_boolean,USE_COHERENT_MEM))
$(eval $(call assert_boolean,USE_TBBR_DEFS))
$(eval $(call assert_boolean,WARMBOOT_ENABLE_DCACHE_EARLY))
$(eval $(call assert_boolean,ENABLE_SPE_FOR_LOWER_ELS))
$(eval $(call assert_boolean,ENABLE_COMPRESSION))
# $(eval $(call assert_boolean,ENABLE_TPU_SECURITY))
$(eval $(call assert_numeric,ARM_ARCH_MAJOR))
$(eval $(call assert_numeric,ARM_ARCH_MINOR))

################################################################################
# Add definitions to the cpp preprocessor based on the current build options.
# This is done after including the platform specific makefile to allow the
# platform to overwrite the default options
################################################################################

$(eval $(call add_define,ARM_CCI_PRODUCT_ID))
$(eval $(call add_define,ARM_ARCH_MAJOR))
$(eval $(call add_define,ARM_ARCH_MINOR))
$(eval $(call add_define,ARM_GIC_ARCH))
$(eval $(call add_define,COLD_BOOT_SINGLE_CPU))
$(eval $(call add_define,CTX_INCLUDE_AARCH32_REGS))
$(eval $(call add_define,CTX_INCLUDE_FPREGS))
$(eval $(call add_define,ENABLE_ASSERTIONS))
$(eval $(call add_define,ENABLE_PLAT_COMPAT))
$(eval $(call add_define,ENABLE_PMF))
$(eval $(call add_define,ENABLE_PSCI_STAT))
$(eval $(call add_define,ENABLE_RUNTIME_INSTRUMENTATION))
$(eval $(call add_define,ERROR_DEPRECATED))
$(eval $(call add_define,HW_ASSISTED_COHERENCY))
$(eval $(call add_define,LOAD_IMAGE_V2))
$(eval $(call add_define,LOG_LEVEL))
$(eval $(call add_define,NS_TIMER_SWITCH))
$(eval $(call add_define,PL011_GENERIC_UART))
$(eval $(call add_define,PLAT_${PLAT}))
$(eval $(call add_define,PROGRAMMABLE_RESET_ADDRESS))
$(eval $(call add_define,PSCI_EXTENDED_STATE_ID))
$(eval $(call add_define,RESET_TO_BL31))
$(eval $(call add_define,SEPARATE_CODE_AND_RODATA))
$(eval $(call add_define,BL1_SEPARATE_CODE_AND_RODATA))
$(eval $(call add_define,SPD_${SPD}))
$(eval $(call add_define,SPIN_ON_BL1_EXIT))
$(eval $(call add_define,TRUSTED_BOARD_BOOT))
$(eval $(call add_define,USE_COHERENT_MEM))
$(eval $(call add_define,USE_TBBR_DEFS))
$(eval $(call add_define,WARMBOOT_ENABLE_DCACHE_EARLY))
$(eval $(call add_define,ENABLE_SPE_FOR_LOWER_ELS))
$(eval $(call add_define,ENABLE_COMPRESSION))
# $(eval $(call add_define,ENABLE_TPU_SECURITY))
$(eval $(call add_define,NANDBOOT_V2))
$(eval $(call add_define,BOOTFLOW_TYPE))


ifeq ($(CONFIG_BOARD),"fpga")
$(eval $(call add_define,CONFIG_BOARD_fpga))
endif

################################################################################
# Build options checks
################################################################################
$(eval $(call assert_boolean,DEBUG))
$(eval $(call assert_boolean,ENABLE_ASSERTIONS))
$(eval $(call assert_boolean,NANDBOOT_V2))
$(eval $(call assert_boolean,NANDBOOT_V2))
$(eval $(call assert_boolean,PAGE_SIZE_64KB))
$(eval $(call assert_boolean,TEST_FROM_SPINOR1))
$(eval $(call assert_boolean,PRINTF_TIMESTAMP))

################################################################################
# Add definitions to the cpp preprocessor based on the current build options.
# This is done after including the platform specific makefile to allow the
# platform to overwrite the default options
################################################################################
$(call print_var,TEST_FROM_SPINOR1)
$(call print_var,PAGE_SIZE_64KB)

$(eval $(call add_define,TEST_FROM_SPINOR1))
$(eval $(call add_define,PAGE_SIZE_64KB))
$(eval $(call add_define,PRINTF_TIMESTAMP))

$(eval $(call add_define,ENABLE_ASSERTIONS))
$(eval $(call add_define,LOG_LEVEL))
$(eval $(call add_define,__CVITEK__))
$(eval $(call add_define,NANDBOOT_V2))


ifeq (${BOOT_CPU},riscv)
$(eval $(call add_define_val,TOC_HEADER_NAME,0xC906B001))
else
$(eval $(call add_define_val,TOC_HEADER_NAME,0xAA640001))
endif

ifeq (${RTOS_ENABLE_FREERTOS},y)
$(eval $(call add_define,RTOS_ENABLE_FREERTOS))
$(eval $(call add_define_val,RTOS_DUMP_PRINT_SZ_IDX,${RTOS_DUMP_PRINT_SZ_IDX}))
$(eval $(call add_define_val,RTOS_FAST_IMAGE_TYPE,${RTOS_FAST_IMAGE_TYPE}))
$(eval $(call add_define_val,RTOS_DUMP_PRINT_ENABLE,$(call yn10,${RTOS_DUMP_PRINT_ENABLE})))
endif

$(eval $(call add_define,FSBL_SECURE_BOOT_SUPPORT))
$(eval $(call add_define, USB_DL_BY_FSBL))

# ifeq (${NEED_BL31},yes)
# # Add RTC_CORE_SRAM_BIN_PATH into cv_pm.c
# $(info RTC_CORE_SRAM_BIN_PATH is '${RTC_CORE_SRAM_BIN_PATH}')
# ifeq ($(filter clean %clean clean%,$(MAKECMDGOALS)),)
# ifeq (,$(wildcard ${RTC_CORE_SRAM_BIN_PATH}))
# $(error RTC_CORE_SRAM_BIN_PATH is not existed)
# else
# $(shell touch -c plat/cvitek/${CHIP_ARCH}/common/cv_pm.c > /dev/null)
# endif
# endif
# $(eval $(call add_define_val,RTC_CORE_SRAM_BIN_PATH,'"${RTC_CORE_SRAM_BIN_PATH}"'))
# endif

################################################################################
# Build targets
################################################################################
.PHONY: all fip clean bl-check bl-build fake-blcp
.SUFFIXES:

################################################################################
# Build BL31
################################################################################

ifeq (${NEED_BL31},yes)
all: bl31
BL31_SOURCES += ${SPD_SOURCES}
$(eval $(call MAKE_BL,31,soc-fw))
endif

ifeq (${NEED_BL32},yes)
all: bl32
$(eval $(call MAKE_BL,32,tos-fw))
endif

export BUILD_PLAT NM


all: bl2 fip blmacros

include ${MAKE_HELPERS_DIRECTORY}fip.mk

$(eval $(call MAKE_BL,2))

# Convert '#define ...' to ELF symbols
BLMACROS_LINKERFILE := make_helpers/get_macros.ld.S
BLMACROS_ELF := ${BUILD_PLAT}/blmacros/blmacros.elf

BLmacros_CPPFLAGS := $(BL2_CPPFLAGS)
BLmacros_ASFLAGS := $(BL2_ASFLAGS)
BLmacros_CFLAGS := $(BL2_CFLAGS)

$(eval $(call MAKE_BL,macros))

clean:
	$(print_target)
	$(call SHELL_REMOVE_DIR,${BUILD_PLAT})
