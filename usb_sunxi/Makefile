#
# (C) Copyright 2006
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#


obj-$(CONFIG_CMD_USBMASS)    += usb_mass.o
obj-$(CONFIG_CMD_FASTBOOT)   += usb_fastboot.o
obj-$(CONFIG_CMD_SUNXI_BURN) += usb_burn.o
obj-$(CONFIG_CMD_SUNXI_EFEX) += usb_efex.o
obj-$(CONFIG_CMD_SUNXI_EFEX) += buf_queue.o
obj-$(CONFIG_CMD_SUNXI_EFEX) += efex_queue.o
obj-$(CONFIG_SUNXI_HDCP_IN_SECURESTORAGE) += key_deal.o
obj-y += usb_base_common.o

obj-y   += usb_base_platform.o
obj-y   += usb_dma_control.o

#########################################################################
