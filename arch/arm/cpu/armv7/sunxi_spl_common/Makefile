
##
## Makefile for dma support
##



include $(SPLDIR)/config.mk

LIB	:= $(obj)libdma.o

COBJS-y += sunxi_dma_spl.o
COBJS-y += sunxi_gic_spl.o
COBJS-y += sunxi_mem_alloc_spl.o
COBJS-y += interrupts_spl.o

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
