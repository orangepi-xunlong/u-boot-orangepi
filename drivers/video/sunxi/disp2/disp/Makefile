#
# Copyright (c) 2015 Allwinnertech, Inc.
# Tyle <tyle at allwinnertech.com>
#
#

obj-$(CONFIG_SUNXI_DISPLAY) += disp.o

disp-objs	:= dev_disp.o disp_sys_intf.o
disp-objs	+= de/disp_display.o de/disp_features.o de/disp_device.o de/disp_lcd.o de/disp_manager.o de/disp_private.o \
			   de/disp_smart_backlight.o de/disp_enhance.o de/disp_capture.o de/disp_hdmi.o de/disp_tv.o  de/disp_vga.o de/disp_vdevice.o \
			   de/disp_edp.o de/disp_eink_manager.o de/eink_pipeline_manager.o

disp-objs += lcd/panels.o lcd/lcd_source.o

export MIPI_DSI_IP_VERSION ?= 40

ifeq ($(CONFIG_ARCH_SUN50IW3P1),y)
    export MIPI_DSI_IP_VERSION := 28
    disp-objs += lcd/ls029b3sx02.o lcd/vr_ls055t1sx01.o lcd/lq101r1sx03.o lcd/he0801a068.o lcd/sl008pn21d.o
else
ifeq ($(CONFIG_ARCH_SUN8IW12P1),y)
    export MIPI_DSI_IP_VERSION := 28
    disp-objs += lcd/fd055hd003s.o lcd/ili9341.o lcd/default_panel.o
else
disp-objs += lcd/default_panel.o \
	lcd/lt070me05000.o lcd/wtq05027d01.o lcd/t27p06.o \
	lcd/dx0960be40a1.o lcd/tft720x1280.o lcd/S6D7AA0X01.o \
	lcd/inet_dsi_panel.o lcd/cpu_gg1p4062utsw.o lcd/vr_sharp.o \
	lcd/he0801a068.o lcd/WilliamLcd.o lcd/default_eink.o lcd/fx070.o
endif
endif

obj-$(CONFIG_ARCH_SUN50IW1P1) += de/lowlevel_sun50iw1/

obj-$(CONFIG_ARCH_SUN8IW10P1) += de/lowlevel_sun8iw10/

obj-$(CONFIG_ARCH_SUN8IW11P1) += de/lowlevel_v2x/

obj-$(CONFIG_ARCH_SUN8IW12P1) += de/lowlevel_v2x/

obj-$(CONFIG_ARCH_SUN50IW2P1) += de/lowlevel_v2x/

obj-$(CONFIG_ARCH_SUN8IW7P1) += de/lowlevel_v2x/

obj-$(CONFIG_ARCH_SUN50IW6P1) += de/lowlevel_v3x/

obj-$(CONFIG_ARCH_SUN50IW3P1) += de/lowlevel_v3x/

obj-$(CONFIG_ARCH_SUN8IW17P1) += de/lowlevel_v2x/

obj-$(CONFIG_ARCH_SUN8IW6P1) += de/lowlevel_v2x/

obj-$(CONFIG_ARCH_SUN8IW15P1) += de/lowlevel_v2x/

ifeq ($(CONFIG_ARCH_SUN8IW6P1),y)
    export MIPI_DSI_IP_VERSION := 28
endif

ifeq ($(CONFIG_ARCH_SUN8IW15P1),y)
    export MIPI_DSI_IP_VERSION := 28
endif
