
##
## Makefile for Sunxi Secure Boot
##


include $(SPLDIR)/config.mk
LIB	:= $(obj)libdram

COBJS-y += mctl_hal.o chip_id_sd.o

COBJS	:= $(COBJS-y)
SRCS	:= $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))

all:	$(LIB)

$(LIB):	$(obj).depend $(OBJS)
	$(call cmd_link_o_target, $(OBJS))
	cp $(LIB) ../

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
