#
# Copyright (c) 2017 Allwinnertech, Inc.
#
#

mydir = .

dedir = $(mydir)/de330

tcondir = $(mydir)/tcon

ifeq ($(MIPI_DSI_IP_VERSION),40)
dsi_obj:=$(tcondir)/de_dsi.o
else
dsi_obj:=$(tcondir)/de_dsi_28.o
endif

ifeq ($(CONFIG_MACH_SUN50IW9),y)
obj-y += $(dedir)/de331_feat.o
else
obj-y += $(dedir)/de330_feat.o
endif

obj-y += $(mydir)/disp_al_de.o \
        $(dedir)/de_feat.o $(dedir)/de_rtmx.o \
        $(dedir)/de_top.o $(dedir)/de_csc_table.o $(dedir)/de_ccsc.o \
        $(dedir)/de_ovl.o $(dedir)/de_fbd_atw.o \
        $(dedir)/de_vsu.o $(dedir)/de_scaler_table.o \
        $(dedir)/de_cdc.o \
        $(dedir)/de_bld.o $(dedir)/de_fmt.o \
        $(dedir)/de_enhance.o $(dedir)/de_dns.o $(dedir)/de_fce.o \
        $(dedir)/de_peak.o $(dedir)/de_lti.o \
        $(dedir)/de_bls.o $(dedir)/de_fcc.o  \
        $(dedir)/de_smbl.o \
         $(dsi_obj) \
        $(mydir)/disp_al_tcon.o $(mydir)/tcon_feat.o \
        $(tcondir)/de_lcd.o


obj-$(CONFIG_DISP2_SUNXI_SUPPORT_ENAHNCE) += $(dedir)/de_vep_table.o
obj-$(CONFIG_DISP2_SUNXI_SUPPORT_SNR) += $(dedir)/de_ksc.o
obj-$(CONFIG_DISP2_SUNXI_SUPPORT_KSC) += $(dedir)/de_snr.o
obj-$(CONFIG_DISP2_SUNXI_SUPPORT_WB) += $(dedir)/de_wb.o
obj-$(CONFIG_DISP2_SUNXI_SUPPORT_CDC) += $(dedir)/de_cdc_table.o

