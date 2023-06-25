
#put following config to platform's defconfig
#CONFIG_SUNXI_TV_FASTLOGO=y
#To enable jpeg decode add following 3 lines
#CONFIG_SUNXI_FASTLOGO_JPEG=y
#CONFIG_USE_PRIVATE_LIBGCC=n
#CONFIG_HAVE_PRIVATE_LIBGCC=n

obj-y += fastlogo.o
obj-y += decode_pic.o load_file.o parse_reg.o
obj-$(CONFIG_SUNXI_FASTLOGO_JPEG) += tinyjpegdecoder/
