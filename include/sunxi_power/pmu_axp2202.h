/*
 * Copyright (C) 2016 Allwinner.
 * weidonghui <weidonghui@allwinnertech.com>
 *
 * SUNXI AXP2202  Driver
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#ifndef __AXP2202_H__
#define __AXP2202_H__

//PMIC chip id reg03:bit7-6  bit3-
#define   AXP2202_CHIP_VER              (0x00)


#define AXP2202_DEVICE_ADDR			(0x3A3)
#ifndef CONFIG_SYS_SUNXI_R_I2C0_SLAVE
#define AXP2202_RUNTIME_ADDR			(0x2d)
#else
#ifndef CONFIG_AXP2202_SUNXI_I2C_SLAVE
#define AXP2202_RUNTIME_ADDR			CONFIG_SYS_SUNXI_R_I2C0_SLAVE
#else
#define AXP2202_RUNTIME_ADDR			CONFIG_AXP2202_SUNXI_I2C_SLAVE
#endif
#endif

/* define AXP21 REGISTER */
#define   AXP2202_MODE_CHGSTATUS		(0x01)
#define   AXP2202_CHIP_ID         	 	(0x03)
#define   AXP2202_VERSION			(0x04)
#define   AXP2202_CLK_EN			(0x0b)
#define   AXP2202_CHIP_ID_EXT			(0x0e)

#define   AXP2202_IIN_LIM			(0x17)

#define   AXP2202_DCDC_CFG0    	   		(0x80)
#define   AXP2202_DCDC_CFG1    	 	 	(0x81)
#define   AXP2202_DCDC_CFG2    	 	 	(0x82)
#define   AXP2202_LDO_CFG0     			(0x90)
#define   AXP2202_LDO_CFG1     			(0x91)

#define   AXP2202_DCDC1_CFG			(0x83)
#define   AXP2202_DCDC2_CFG        	  	(0x84)
#define   AXP2202_DCDC3_CFG          		(0x85)
#define   AXP2202_DCDC4_CFG       	   	(0x86)

#define   AXP2202_ALDO1_CFG				(0x93)
#define   AXP2202_ALDO2_CFG				(0x94)
#define   AXP2202_ALDO3_CFG				(0x95)
#define   AXP2202_ALDO4_CFG				(0x96)
#define   AXP2202_BLDO1_CFG				(0x97)
#define   AXP2202_BLDO2_CFG				(0x98)
#define   AXP2202_BLDO3_CFG				(0x99)
#define   AXP2202_BLDO4_CFG				(0x9a)
#define   AXP2202_CLDO1_CFG				(0x9b)
#define   AXP2202_CLDO2_CFG				(0x9c)
#define   AXP2202_CLDO3_CFG				(0x9d)
#define   AXP2202_CLDO4_CFG				(0x9e)
#define   AXP2202_CPUSLDO_CFG				(0x9f)


#define   AXP2202_COMM_CFG	        	(0x10)
#define   AXP2202_VBUS_VOL_SET         	(0x16)
#define   AXP2202_VBUS_CUR_SET          	(0x17)
#define   AXP2202_PWRON_STATUS           	(0x20)
#define   AXP2202_VOFF_THLD            	(0x24)
#define   AXP2202_OFF_CTL             	(0x27)
#define   AXP2202_CHARGE1             	(0x62)
#define   AXP2202_CHGLED_SET             	(0x70)




#define   AXP2202_INTEN0              	(0x40)
#define   AXP2202_INTEN1              	(0x41)
#define   AXP2202_INTEN2              	(0x42)

#define   AXP2202_INTSTS0             	(0x48)
#define   AXP2202_INTSTS1             	(0x49)
#define   AXP2202_INTSTS2             	(0x4a)

#define   AXP2202_DATA_BUFFER3				 (0xf0)

#define   AXP2202_DCDC_MODESET				(0x81)
#define   AXP2202_DCDC1_PWM_BIT				(2)
#define   AXP2202_DCDC2_PWM_BIT				(3)
#define   AXP2202_DCDC3_PWM_BIT				(4)

#define   AXP2202_bc12_CLK_EN				(4)
#define   AXP2202_bc12_EN				(7)

#endif /* __AXP2202_REGS_H__ */

