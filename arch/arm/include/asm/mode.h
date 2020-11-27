/*
 * (C) Copyright 2010
 * Texas Instruments, <www.ti.com>
 * Aneesh V <aneesh@ti.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#ifndef __ARM_MODE_H__
#define __ARM_MODE_H__

//ARMV7
#define  ARMV7_USR_MODE           0x10
#define  ARMV7_FIQ_MODE           0x11
#define  ARMV7_IRQ_MODE           0x12
#define  ARMV7_SVC_MODE           0x13
#define  ARMV7_MON_MODE        	  0x16
#define  ARMV7_ABT_MODE           0x17
#define  ARMV7_UND_MODE           0x1b
#define  ARMV7_SYSTEM_MODE        0x1f
#define  ARMV7_MODE_MASK          0x1f
#define  ARMV7_FIQ_MASK           0x40
#define  ARMV7_IRQ_MASK           0x80

//ARM9
#define ARM9_USR26_MODE           0x00
#define ARM9_FIQ26_MODE           0x01
#define ARM9_IRQ26_MODE           0x02
#define ARM9_SVC26_MODE           0x03
#define ARM9_USR_MODE             0x10
#define ARM9_FIQ_MODE             0x11
#define ARM9_IRQ_MODE             0x12
#define ARM9_SVC_MODE             0x13
#define ARM9_ABT_MODE             0x17
#define ARM9_UND_MODE             0x1b
#define ARM9_SYSTEM_MODE          0x1f
#define ARM9_MODE_MASK            0x1f
#define ARM9_FIQ_MASK             0x40
#define ARM9_IRQ_MASK             0x80
#define ARM9_CC_V_BIT             (1 << 28)
#define ARM9_CC_C_BIT             (1 << 29)
#define ARM9_CC_Z_BIT             (1 << 30)
#define ARM9_CC_N_BIT             (1 << 31)


// coprocessor CP15
// C1
#define C1_M_BIT                  (1 << 0 )
#define C1_A_BIT                  (1 << 1 )
#define C1_C_BIT                  (1 << 2 )
#define C1_W_BIT                  (1 << 3 )
#define C1_P_BIT                  (1 << 4 )
#define C1_D_BIT                  (1 << 5 )
#define C1_L_BIT                  (1 << 6 )
#define C1_B_BIT                  (1 << 7 )
#define C1_S_BIT                  (1 << 8 )
#define C1_R_BIT                  (1 << 9 )
#define C1_F_BIT                  (1 << 10)
#define C1_Z_BIT                  (1 << 11)
#define C1_I_BIT                  (1 << 12)
#define C1_V_BIT                  (1 << 13)
#define C1_RR_BIT                 (1 << 14)
#define C1_L4_BIT                 (1 << 15)

#endif
