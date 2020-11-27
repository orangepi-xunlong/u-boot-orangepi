
obj-y += $(SOC)/osal/
obj-y += nand_interface/

obj-{CONFIG_SUNXI_UBIFS} += sunxi_mtd_ubi/
obj-y += $(shell if [ -d $(obj)/$(SOC)/lib-nand ];  then echo $(SOC)/lib-nand/;  fi )



