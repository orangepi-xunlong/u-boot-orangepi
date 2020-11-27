#
# (C) Copyright 2002
# Gary Jennejohn, DENX Software Engineering, <garyj@denx.de>
#
# SPDX-License-Identifier:	GPL-2.0+
#

ifneq  ("$(TOPDIR)", "")
    #check gcc tools chain
    tooldir_armv5=$(TOPDIR)/../armv5_toolchain
    toolchain_archive_armv5=$(TOPDIR)/../toolchain/arm-none-linux-gnueabi-toolchain.tar.xz
    armv5_toolchain_check=$(strip $(shell if [  -d $(tooldir_armv5) ];  then  echo yes;  fi))

    ifneq ("$(armv5_toolchain_check)", "yes")
        $(info "gcc tools chain for armv5 not exist")
        $(info "Prepare toolchain...")
        $(shell mkdir -p $(tooldir_armv5))
        $(shell tar --strip-components=1 -xf $(toolchain_archive_armv5) -C $(tooldir_armv5))
    endif

    CROSS_COMPILE :=  $(TOPDIR)/../armv5_toolchain/arm-none-linux-gnueabi-toolchain/arm-2014.05/bin/arm-none-linux-gnueabi-
endif

PLATFORM_CPPFLAGS += -march=armv5te
