$(call print_var,DDR_CFG)

ifeq (${DDR_CFG},none)
DDR_CFG =
endif

ifeq ($(DDR_CFG), )
BL2_INCLUDES += \
	-Iinclude/bl2/ddr

BL2_SOURCES += \
	drivers/cvitek/ddr/ddr.o

$(eval $(call add_define,NO_DDR_CFG))

else ifeq (${BOARD},palladium)

$(eval $(call add_define,PLD_DDR_CFG))

$(eval $(call add_define,DDR_2_RANK))

BL2_INCLUDES += \
	-Iinclude/bl2/ddr \
	-Iinclude/bl2/ddr/ddr_config/${DDR_CFG}

BL2_SOURCES += \
	drivers/cvitek/ddr/ddr.o \
	drivers/cvitek/ddr/ddr_pkg_info.o \
	drivers/cvitek/ddr/ddr_sys_pld.o \
	drivers/cvitek/ddr/ddrc_init.o \
	drivers/cvitek/ddr/phy_init.o \
	drivers/cvitek/ddr/ddr_patch_regs.o

ifneq ($(findstring ddr4, ${DDR_CFG}),)
    $(eval $(call add_define,DDR4))
else ifneq ($(findstring lp4, ${DDR_CFG}),)
    $(eval $(call add_define,LPDDR4))
endif
#$(eval $(call add_define,REAL_DDRPHY))
#$(eval $(call add_define,REAL_LOCK))
#$(eval $(call add_define,X16_MODE))
else
BL2_INCLUDES += \
	-Iinclude/bl2/ddr \
	-Iinclude/bl2/ddr/ddr_config/${DDR_CFG}

BL2_SOURCES += \
	drivers/cvitek/ddr/ddr.o \
        drivers/cvitek/ddr/ddr_pkg_info.o \
        drivers/cvitek/ddr/ddr_sys_bring_up.o \
        drivers/cvitek/ddr/ddr_sys.o \
        drivers/cvitek/ddr/ddr_shmoo.o \
        drivers/cvitek/ddr/ddrc_init.o \
        drivers/cvitek/ddr/phy_init.o \
        drivers/cvitek/ddr/ddr_patch_regs.o

ifneq ($(findstring ddr4_3200_x8, ${DDR_CFG}),)
    $(eval $(call add_define,DDR4_X8))
else ifneq ($(findstring ddr4_3200_x16, ${DDR_CFG}),)
    $(eval $(call add_define,DDR4_X16))
else ifneq ($(findstring lp4x, ${DDR_CFG}),)
    $(eval $(call add_define,LPDDR4X))
else ifneq ($(findstring lp4, ${DDR_CFG}),)
    $(eval $(call add_define,LPDDR4))
endif

ifneq ($(findstring 2r, ${DDR_CFG}),)
    $(eval $(call add_define,DDR_2_RANK))
endif
ifneq ($(findstring 2s, ${DDR_CFG}),)
    $(eval $(call add_define,DDR_2SYS))
endif

ifneq ($(findstring ddr_auto, ${DDR_CFG}),)
    $(eval $(call add_define,DDR_AUTO))
endif

#$(eval $(call add_define,REAL_DDRPHY))
# $(eval $(call add_define,SSC_EN))
$(eval $(call add_define,REAL_LOCK))
#$(eval $(call add_define,X16_MODE))

# full mem bist
# $(eval $(call add_define,DBG_SHMOO))
# $(eval $(call add_define,DBG_SHMOO_CA))
# $(eval $(call add_define,DBG_SHMOO_CS))
# $(eval $(call add_define,FULL_MEM_BIST))
# $(eval $(call add_define,FULL_MEM_BIST_FOREVER))

endif
