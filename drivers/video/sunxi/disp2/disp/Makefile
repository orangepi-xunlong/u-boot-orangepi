#
# Copyright (c) 2015 Allwinnertech, Inc.
# Tyle <tyle at allwinnertech.com>
#
#

obj-$(CONFIG_DISP2_SUNXI) += disp.o

disp-objs	:= dev_disp.o disp_sys_intf.o
disp-objs	+= de/disp_display.o de/disp_features.o de/disp_device.o de/disp_lcd.o de/disp_manager.o de/disp_private.o \
			   de/disp_smart_backlight.o de/disp_enhance.o de/disp_capture.o de/disp_hdmi.o de/disp_tv.o  de/disp_vga.o de/disp_vdevice.o \
			   de/disp_edp.o

disp-$(CONFIG_EINK_PANEL_USED) += de/disp_eink_manager.o de/eink_pipeline_manager.o

disp-objs += lcd/panels.o lcd/lcd_source.o lcd/default_panel.o


disp-$(CONFIG_LCD_SUPPORT_HE0801A068) += lcd/he0801a068.o
disp-$(CONFIG_EINK_PANEL_USED) += lcd/default_eink.o
disp-$(CONFIG_LCD_SUPPORT_LT070ME05000) += lcd/lt070me05000.o
disp-$(CONFIG_LCD_SUPPORT_WTQ05027D01) += lcd/wtq05027d01.o
disp-$(CONFIG_LCD_SUPPORT_T27P06) += lcd/t27p06.o
disp-$(CONFIG_LCD_SUPPORT_DX0960BE40A1) += lcd/dx0960be40a1.o
disp-$(CONFIG_LCD_SUPPORT_TFT720X1280) += lcd/tft720x1280.o
disp-$(CONFIG_LCD_SUPPORT_S6D7AA0X01) += lcd/S6D7AA0X01.o
disp-$(CONFIG_LCD_SUPPORT_GG1P4062UTSW) += lcd/cpu_gg1p4062utsw.o
disp-$(CONFIG_LCD_SUPPORT_LS029B3SX02) +=  lcd/ls029b3sx02.o
disp-$(CONFIG_LCD_SUPPORT_FD055HD003S) += lcd/fd055hd003s.o
disp-$(CONFIG_LCD_SUPPORT_FRD450H40014) += lcd/frd450h40014.o
disp-$(CONFIG_LCD_SUPPORT_H245QBN02) += lcd/h245qbn02.o
disp-$(CONFIG_LCD_SUPPORT_ILI9341) += lcd/ili9341.o
disp-$(CONFIG_LCD_SUPPORT_LH219WQ1) += lcd/lh219wq1.o
disp-$(CONFIG_LCD_SUPPORT_ST7789V) += lcd/st7789v.o
disp-$(CONFIG_LCD_SUPPORT_ST7796S) += lcd/st7796s.o
disp-$(CONFIG_LCD_SUPPORT_ST7701S) += lcd/st7701s.o
disp-$(CONFIG_LCD_SUPPORT_WTL096601G03) += lcd/wtl096601g03.o
disp-$(CONFIG_LCD_SUPPORT_T30P106) += lcd/t30p106.o
disp-$(CONFIG_LCD_SUPPORT_TO20T20000) += lcd/to20t20000.o
disp-$(CONFIG_LCD_SUPPORT_S2003T46G) += lcd/s2003t46g.o
disp-$(CONFIG_LCD_SUPPORT_WILLIAMLCD) += lcd/WilliamLcd.o
disp-$(CONFIG_LCD_SUPPORT_LQ101R1SX03) += lcd/lq101r1sx03.o
disp-$(CONFIG_LCD_SUPPORT_INET_DSI_PANEL) += lcd/inet_dsi_panel.o
disp-$(CONFIG_LCD_SUPPORT_VR_SHARP) += lcd/vr_sharp.o
disp-$(CONFIG_LCD_SUPPORT_SL008PN21D) += lcd/sl008pn21d.o
disp-$(CONFIG_LCD_SUPPORT_VR_LS055T1SX01) += lcd/vr_ls055t1sx01.o
disp-$(CONFIG_LCD_SUPPORT_BP101WX1) += lcd/bp101wx1-206.o
disp-$(CONFIG_LCD_SUPPORT_K101IM2QA04) += lcd/k101im2qa04.o
disp-$(CONFIG_LCD_SUPPORT_K101IM2BYL02L) += lcd/k101im2byl02l.o
disp-$(CONFIG_LCD_SUPPORT_M133X56) += lcd/m133x56-105.o
disp-$(CONFIG_LCD_SUPPORT_FX070) += lcd/fx070.o
disp-$(CONFIG_LCD_SUPPORT_ST7789V_CPU) += lcd/st7789v_cpu.o
disp-$(CONFIG_LCD_SUPPORT_M101B31) += lcd/M101B31.o
disp-$(CONFIG_LCD_SUPPORT_CC08021801_310_800X1280) += lcd/CC08021801_310_800X1280.o
disp-$(CONFIG_LCD_SUPPORT_K080_IM2HYL802R_800X1280) += lcd/K080_IM2HYL802R_800X1280.o
disp-$(CONFIG_LCD_SUPPORT_KD101NA5) += lcd/kd101na5.o
disp-$(CONFIG_LCD_SUPPORT_KD070D57) += lcd/kd070d57.o
disp-$(CONFIG_LCD_SUPPORT_ZS080NI4003E7H3H_A) += lcd/zs080ni4003e7h3h_a.o
disp-$(CONFIG_LCD_SUPPORT_K101_IM2BYL02_L_800X1280) += lcd/K101_IM2BYL02_L_800X1280.o
disp-$(CONFIG_LCD_SUPPORT_JD9366AB_3) += lcd/jd9366ab_3.o
disp-$(CONFIG_LCD_SUPPORT_TFT08006) += lcd/tft08006.o
disp-$(CONFIG_LCD_SUPPORT_T050K589) += lcd/t050k589.o
disp-$(CONFIG_LCD_SUPPORT_K101_MM2QA01_A) += lcd/K101_MM2QA01_A.o

ifeq ($(CONFIG_MACH_SUN8IW6),y)
    export MIPI_DSI_IP_VERSION := 28
    obj-y += de/lowlevel_v2x/
endif

ifeq ($(CONFIG_MACH_SUN8IW15),y)
    export MIPI_DSI_IP_VERSION := 28
    obj-y += de/lowlevel_v2x/
endif

ifeq ($(CONFIG_MACH_SUN50IW1),y)
    export MIPI_DSI_IP_VERSION := 40
    obj-y += de/lowlevel_sun50iw1/
endif

ifeq ($(CONFIG_MACH_SUN50IW2),y)
    export MIPI_DSI_IP_VERSION := 40
    obj-y += de/lowlevel_v2x/
endif

ifeq ($(CONFIG_MACH_SUN50IW10),y)
    export MIPI_DSI_IP_VERSION := 40
    obj-y += de/lowlevel_v2x/
endif

ifeq ($(CONFIG_MACH_SUN8IW7),y)
    export MIPI_DSI_IP_VERSION := 40
    obj-y += de/lowlevel_v2x/
endif

ifeq ($(CONFIG_MACH_SUN8IW11),y)
    export MIPI_DSI_IP_VERSION := 40
    obj-y += de/lowlevel_v2x/
endif

ifeq ($(CONFIG_MACH_SUN8IW17),y)
    export MIPI_DSI_IP_VERSION := 40
    obj-y += de/lowlevel_v2x/
endif

ifeq ($(CONFIG_MACH_SUN8IW12),y)
    export MIPI_DSI_IP_VERSION := 28
    obj-y += de/lowlevel_v2x/
endif

ifeq ($(CONFIG_MACH_SUN8IW16),y)
    export MIPI_DSI_IP_VERSION := 28
    obj-y += de/lowlevel_v2x/
endif

ifeq ($(CONFIG_MACH_SUN50IW3),y)
    export MIPI_DSI_IP_VERSION := 28
    obj-y += de/lowlevel_v3x/
endif

ifeq ($(CONFIG_MACH_SUN50IW6),y)
    export MIPI_DSI_IP_VERSION := 28
    obj-y += de/lowlevel_v3x/
endif


ifeq ($(CONFIG_MACH_SUN50IW5T),y)
export MIPI_DSI_IP_VERSION := 28
obj-y += = de/lowlevel_v33x/
endif

ifeq ($(CONFIG_MACH_SUN50IW9),y)
    export MIPI_DSI_IP_VERSION := 28
    obj-y += de/lowlevel_v33x/
endif

ifeq ($(CONFIG_MACH_SUN8IW19),y)
    export MIPI_DSI_IP_VERSION := 28
    obj-y += de/lowlevel_v2x/
endif

ifeq ($(CONFIG_MACH_SUN8IW21),y)
    export MIPI_DSI_IP_VERSION := 40
    obj-y += de/lowlevel_v2x/
endif


ifeq ($(CONFIG_MACH_SUN8IW20),y)
    export MIPI_DSI_IP_VERSION := 40
    obj-y += de/lowlevel_v2x/
endif

ifeq ($(CONFIG_MACH_SUN20IW1),y)
    export MIPI_DSI_IP_VERSION := 40
    obj-y += de/lowlevel_v2x/
endif
