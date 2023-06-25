# SPDX-License-Identifier: GPL-2.0+
#
# (C) Copyright 2000-2013
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#########################################################################

# This file is included from ./Makefile and spl/Makefile.
# Clean the state to avoid the same flags added twice.
#
# (Tegra needs different flags for SPL.
#  That's the reason why this file must be included from spl/Makefile too.
#  If we did not have Tegra SoCs, build system would be much simpler...)
PLATFORM_RELFLAGS :=
PLATFORM_CPPFLAGS :=
PLATFORM_LDFLAGS :=
LDFLAGS :=
LDFLAGS_FINAL :=
LDFLAGS_STANDALONE :=
OBJCOPYFLAGS :=
# clear VENDOR for tcsh
VENDOR :=
#########################################################################

ARCH := $(CONFIG_SYS_ARCH:"%"=%)
CPU := $(CONFIG_SYS_CPU:"%"=%)
ifdef CONFIG_SPL_BUILD
ifdef CONFIG_TEGRA
CPU := arm720t
endif
endif
BOARD := $(CONFIG_SYS_BOARD:"%"=%)
ifneq ($(CONFIG_SYS_VENDOR),)
VENDOR := $(CONFIG_SYS_VENDOR:"%"=%)
endif
ifneq ($(CONFIG_SYS_SOC),)
SOC := $(CONFIG_SYS_SOC:"%"=%)
endif

# Some architecture config.mk files need to know what CPUDIR is set to,
# so calculate CPUDIR before including ARCH/SOC/CPU config.mk files.
# Check if arch/$ARCH/cpu/$CPU exists, otherwise assume arch/$ARCH/cpu contains
# CPU-specific code.
CPUDIR=arch/$(ARCH)/cpu$(if $(CPU),/$(CPU),)

sinclude $(srctree)/arch/$(ARCH)/config.mk	# include architecture dependend rules
sinclude $(srctree)/$(CPUDIR)/config.mk		# include  CPU	specific rules

ifdef	SOC
sinclude $(srctree)/$(CPUDIR)/$(SOC)/config.mk	# include  SoC	specific rules
endif
ifneq ($(BOARD),)
ifdef	VENDOR
BOARDDIR = $(VENDOR)/$(BOARD)
else
BOARDDIR = $(BOARD)
endif
endif
ifdef	BOARD
sinclude $(srctree)/board/$(BOARDDIR)/config.mk	# include board specific rules
endif

ifdef FTRACE
PLATFORM_CPPFLAGS += -finstrument-functions -DFTRACE
endif

ifeq ($(CONFIG_SUNXI_TRACE),y)
#PLATFORM_CPPFLAGS += -finstrument-functions -DFTRACE
PLATFORM_CPPFLAGS += -DFTRACE

# cpu related function will freeze cpu, never trace them
PLATFORM_CPPFLAGS += -finstrument-functions-exclude-file-list=arch/arm/cpu/
PLATFORM_CPPFLAGS += -finstrument-functions-exclude-file-list=arch/riscv/cpu/
# serial command line may contaminate trace result, so dont trace those function
PLATFORM_CPPFLAGS += -finstrument-functions-exclude-file-list=common/cli
endif

# Allow use of stdint.h if available
ifneq ($(USE_STDINT),)
PLATFORM_CPPFLAGS += -DCONFIG_USE_STDINT
endif

#########################################################################

RELFLAGS := $(PLATFORM_RELFLAGS)

PLATFORM_CPPFLAGS += $(RELFLAGS)
PLATFORM_CPPFLAGS += -pipe


LDFLAGS += $(PLATFORM_LDFLAGS)
LDFLAGS_FINAL += -Bstatic

export PLATFORM_CPPFLAGS
export RELFLAGS
export LDFLAGS_FINAL
export LDFLAGS_STANDALONE
export CONFIG_STANDALONE_LOAD_ADDR
