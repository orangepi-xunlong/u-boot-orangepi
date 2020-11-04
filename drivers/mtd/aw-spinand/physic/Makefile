
obj-y += spinand-phy.o

spinand-phy-objs += core.o ecc.o id.o bbt.o

ifeq (CONFIG_AW_SPINAND_NONSTANDARD_SPI_DRIVER,)
spinand-phy-objs += ops.o cache.o
else
spinand-phy-objs += non-ops.o non-cache.o spic_op.o
endif
