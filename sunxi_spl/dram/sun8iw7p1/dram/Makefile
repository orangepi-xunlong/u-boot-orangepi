
##
## Makefile for Sunxi Secure Boot
##



include $(TOPDIR)/config.mk

all:
ifeq ($(notdir $(shell find ./ -name lib-dram)), lib-dram)

	make -C lib-dram
else
	@echo "libdram exist"
endif
ifeq ($(notdir $(shell find ./ -name lib-chipid)), lib-chipid)

	make -C lib-chipid
	
else
	@echo "lib-chipid exist"

endif
	cp ./libdram ./libdram.o
	cp ./libchipid ./libchipid.o
#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################

