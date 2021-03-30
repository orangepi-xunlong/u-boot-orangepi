
##
## Makefile for Sunxi Secure Boot
##


include $(SPLDIR)/config.mk
LIB	:= $(obj)libmmc.o

COBJS-y += mmc_host_0_3_v4px.o
COBJS-y += mmc_host_2_v5p1.o
COBJS-y += mmc_bsp.o
COBJS-y += mmc.o

COBJS	:= $(COBJS-y)
SRCS	:= $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))

all:	$(LIB)

$(LIB):	$(obj).depend $(OBJS)
	$(call cmd_link_o_target, $(OBJS))

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
