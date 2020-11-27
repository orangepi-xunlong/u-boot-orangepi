
##
## Makefile for Sunxi Secure Boot
##



include $(SPLDIR)/config.mk

LIB	:= $(lib)libgeneric.o

COBJS += check.o
COBJS += nand_misc.o
COBJS += eabi_compat.o
COBJS += common.o

ifdef CONFIG_ARCH_SUN3IW1P1
COBJS += mmu_sun3i.o
SOBJS += jmpto64_sun3i.o memcpy_align16.o
else
COBJS += mmu.o
SOBJS += jmpto64.o memcpy_align16.o
endif


SOBJS   := $(SOBJS)
COBJS	:= $(COBJS)
SRCS	:= $(SOBJS:.o=.S) $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS) $(SOBJS))

all:	$(obj).depend $(LIB)

$(LIB):	 $(OBJS)
	$(call cmd_link_o_target, $(OBJS))

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
