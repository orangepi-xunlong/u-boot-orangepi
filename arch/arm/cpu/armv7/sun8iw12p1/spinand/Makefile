
##
## Makefile for spi nand
##



include $(SPLDIR)/config.mk

LIB	:= $(obj)libspinand.o

COBJS-y += spic_op.o
COBJS-y += spinand.o
COBJS-y += spinand_basic_op.o
COBJS-y += spinand_osal_boot0.o

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
