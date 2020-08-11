include $(SPLDIR)/config.mk
LIB	=  $(obj)libdisplay.o

COBJS	+= clk.o
COBJS	+= hdmi.o
COBJS	+= tcon.o
COBJS	+= display.o
COBJS	+= video.o fc_video.o phy.o access.o irq.o main_controller.o packets.o scdc.o
SRCS	:= $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))
all:	 $(LIB)

$(LIB):	$(OBJS)
	$(call cmd_link_o_target, $(OBJS))

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
