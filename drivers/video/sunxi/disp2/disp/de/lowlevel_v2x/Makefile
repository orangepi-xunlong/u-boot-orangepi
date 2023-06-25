#
# Copyright (c) 2015 Allwinnertech, Inc.
# Tyle <tyle at allwinnertech.com>
#
#

ifeq ($(MIPI_DSI_IP_VERSION),40)
dsi_obj:=$(mydir)/de_dsi.o
else
dsi_obj:=$(mydir)/de_dsi_28.o
endif

ifeq ($(CONFIG_EINK_PANEL_USED),y)
eink_obj:= de_eink.o disp_eink_data.o \
		disp_waveform.o rtmx_eink.o
endif

ifneq ($(CONFIG_EINK200_SUNXI),y)
wb_obj:= de_wb.o
endif
obj-y += de_fcc.o de_gsu.o de_hal.o disp_al.o \
	de_peak.o de_rtmx.o de_scaler_table.o\
	de_vep_table.o de_vsu.o /de_feat.o \
	de_lti.o de_fce.o \
	de_ase.o de_bws.o de_enhance.o \
	de_smbl.o de_ccsc.o de_dcsc.o \
	$(wb_obj) de_clock.o $(dsi_obj) \
	$(eink_obj) \

ifeq ($(CONFIG_MACH_SUN50IW10),y)
obj-y += de_lcd_sun50iw10.o
else ifeq ($(CONFIG_MACH_SUN8IW20),y)
obj-y += de_lcd_sun50iw10.o
else ifeq ($(CONFIG_MACH_SUN20IW1),y)
obj-y += de_lcd_sun50iw10.o
else ifeq ($(CONFIG_MACH_SUN8IW21),y)
obj-y += de_lcd_sun50iw10.o
else
obj-y += de_lcd.o
endif
