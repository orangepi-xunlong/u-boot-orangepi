

#obj-y += smc-asm.o
ifdef CONFIG_OPTEE25
obj-y += sunxi_smc_v2.o
else
obj-y += sunxi_smc.o
endif

obj-y += smc_helps.o

subdir-ccflags-$(CONFIG_SUNXI_FINS_FUNC_DRIVER_SMC_DIR) += -finstrument-functions
