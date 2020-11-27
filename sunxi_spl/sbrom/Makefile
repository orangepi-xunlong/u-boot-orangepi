#
# (C) Copyright 2000-2011
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# (C) Copyright 2011
# Daniel Schwierzeck, daniel.schwierzeck@googlemail.com.
#
# (C) Copyright 2011
# Texas Instruments Incorporated - http://www.ti.com/
# Aneesh V <aneesh@ti.com>
#
# This file is released under the terms of GPL v2 and any later version.
# See the file COPYING in the root directory of the source tree for details.
#
# Based on top-level Makefile.
#

include $(SPLDIR)/config.mk
include $(TOPDIR)/include/autoconf.mk
include $(TOPDIR)/include/autoconf.mk.dep

CONFIG_SPL := y
export CONFIG_SPL

TOOLS_DIR  := $(TOPDIR)/tools

SBROM_LDSCRIPT := $(SPLBASE)/sunxi_spl/sbrom/main/sboot.lds

# We want the final binaries in this directory
obj := $(SPLBASE)/sunxi_spl/sbrom/

HEAD  := main/sbromsw_head.o
START := main/brom.o

LIBS-y += sunxi_spl/sbrom/spl/libsource_spl.o
LIBS-y += sunxi_spl/sbrom/main/libmain.o
LIBS-y += sunxi_spl/sbrom/libs/libgeneric.o
LIBS-y += sunxi_spl/sbrom/flash/libflash.o
LIBS-y += sunxi_spl/sbrom/load/libload.o
LIBS-y += sunxi_spl/spl/lib/libgeneric.o
LIBS-y += sunxi_spl/sbrom/openssl/libopenssl.o

ifdef CONFIG_SUPPORT_DDR4
LIB-DDR3 = $(SPLBASE)/sunxi_spl/dram/$(SOC)/dram/libdram_ddr3
LIB-LPDDR3 = $(SPLBASE)/sunxi_spl/dram/$(SOC)/dram/libdram_lpddr3
LIB-DDR4 = $(SPLBASE)/sunxi_spl/dram/$(SOC)/dram/libdram_ddr4
LIB-LPDDR4 = $(SPLBASE)/sunxi_spl/dram/$(SOC)/dram/libdram_lpddr4
else
LIBS-y += sunxi_spl/dram/$(SOC)/dram/libdram.o
LIBS-$(CONFIG_SUNXI_CHIPID) += sunxi_spl/dram/$(SOC)/dram/libchipid.o
endif

LIBS-$(CONFIG_SUNXI_KEY_LADDER) += sunxi_spl/sbrom/keyladder/libkeyladder.o

LIBS := $(addprefix $(SPLBASE)/,$(sort $(LIBS-y)))
__LIBS := $(subst $(obj),,$(LIBS))

FLASH-y += $(TOPDIR)/arch/$(ARCH)/cpu/$(CPU)/$(SOC)/nand/libnand.o
FLASH-y += $(TOPDIR)/arch/$(ARCH)/cpu/$(CPU)/$(SOC)/mmc/libmmc.o

LIBS += $(FLASH-y)

# Special flags for CPP when processing the linker script.
# Pass the version down so we can handle backwards compatibility
# on the fly.
LDPPFLAGS += \
	-include $(TOPDIR)/include/u-boot/u-boot.lds.h \
	-DSBROMSWADDR=$(CONFIG_TOC0_RUN_ADDR)	 \
	$(shell $(LD) --version | \
	  sed -ne 's/GNU ld version \([0-9][0-9]*\)\.\([0-9][0-9]*\).*/-DLD_MAJOR=\1 -DLD_MINOR=\2/p')

ifdef CONFIG_SUPPORT_DDR4
ALL-y  += $(obj)sboot_ddr3.bin $(obj)sboot_lpddr3.bin $(obj)sboot_ddr4.bin $(obj)sboot_lpddr4.bin
else
ALL-y	+= $(obj)sboot.bin
endif

all:	$(ALL-y)


ifdef CONFIG_SUPPORT_DDR4
$(obj)sboot_ddr3.bin:	$(obj)sboot_ddr3.axf $(obj)cur.log
	@$(OBJCOPY) $(OBJCFLAGS) -O binary $< $@
	@$(SPLDIR)/add_hash.sh -f $(SPLDIR)/sbrom/sboot_ddr3.bin -m sboot

$(obj)sboot_ddr3.axf: $(LIBS) $(obj)sboot.lds
	@$(LD) $(LIB-DDR3) $(LIBS) $(PLATFORM_LIBGCC) $(LDFLAGS) -T$(obj)sboot.lds -o sboot_ddr3.axf -Map sboot.map

$(obj)sboot_lpddr3.bin:	$(obj)sboot_lpddr3.axf $(obj)cur.log
	@$(OBJCOPY) $(OBJCFLAGS) -O binary $< $@
	@$(SPLDIR)/add_hash.sh -f $(SPLDIR)/sbrom/sboot_lpddr3.bin -m sboot

$(obj)sboot_lpddr3.axf: $(LIBS) $(obj)sboot.lds
	@$(LD) $(LIB-LPDDR3) $(LIBS) $(PLATFORM_LIBGCC) $(LDFLAGS) -T$(obj)sboot.lds -o sboot_lpddr3.axf -Map sboot.map

$(obj)sboot_ddr4.bin:	$(obj)sboot_ddr4.axf $(obj)cur.log
	@$(OBJCOPY) $(OBJCFLAGS) -O binary $< $@
	@$(SPLDIR)/add_hash.sh -f $(SPLDIR)/sbrom/sboot_ddr4.bin -m sboot

$(obj)sboot_ddr4.axf: $(LIBS) $(obj)sboot.lds
	@$(LD) $(LIB-DDR4) $(LIBS) $(PLATFORM_LIBGCC) $(LDFLAGS) -T$(obj)sboot.lds -o sboot_ddr4.axf -Map sboot.map

$(obj)sboot_lpddr4.bin:	$(obj)sboot_lpddr4.axf $(obj)cur.log
	@$(OBJCOPY) $(OBJCFLAGS) -O binary $< $@
	@$(SPLDIR)/add_hash.sh -f $(SPLDIR)/sbrom/sboot_lpddr4.bin -m sboot

$(obj)sboot_lpddr4.axf: $(LIBS) $(obj)sboot.lds
	@$(LD) $(LIB-LPDDR4) $(LIBS) $(PLATFORM_LIBGCC) $(LDFLAGS) -T$(obj)sboot.lds -o sboot_lpddr4.axf -Map sboot.map
else
$(obj)sboot.bin:	$(obj)sboot.axf $(obj)cur.log
	@$(OBJCOPY) $(OBJCFLAGS) -O binary $< $@
	@$(SPLDIR)/add_hash.sh -f $(SPLDIR)/sbrom/sboot.bin -m sboot

$(obj)sboot.axf: $(LIBS) $(obj)sboot.lds
	@$(LD) $(LIBS) $(PLATFORM_LIBGCC) $(LDFLAGS) -T$(obj)sboot.lds -o sboot.axf -Map sboot.map
endif

$(LIBS): depend
	@$(MAKE) -C $(SRCTREE)$(dir $(subst $(OBJTREE),,$@))

$(obj)sboot.lds: $(SBROM_LDSCRIPT)
	@$(CPP) $(CPPFLAGS) $(LDPPFLAGS) -ansi -D__ASSEMBLY__ -P - <$^ >$@

$(obj)cur.log:
	@git show HEAD --pretty=format:"%H" | head -n 1 > cur.log

depend:.depend

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude .depend

#########################################################################


