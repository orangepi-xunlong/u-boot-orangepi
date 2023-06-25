obj-$(CONFIG_EINK200_SUNXI) += sunxi_eink.o

sunxi_eink-objs := eink_driver.o
sunxi_eink-objs += eink_sys_source.o
sunxi_eink-objs += eink_manager.o pipeline_mgr.o index_mgr.o \
	timing_ctrl_mgr.o  fmt_convert_mgr.o waveform_mgr.o

sunxi_eink-objs += panels/panel_ctrl.o panels/default_eink.o \
			panels/ed060xd4u2_tc.o panels/ed068og1.o panels/es103tc1c1.o

sunxi_eink-objs += lowlevel/eink200.o lowlevel/de_wb.o
