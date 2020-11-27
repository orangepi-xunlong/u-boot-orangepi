/*
 * Copyright (C) 2013 Allwinnertech, kevin.z.m <kevin@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Adjustable factor-based clock implementation
 */
#ifndef __MACH_SUNXI_CLK_SUN3IW1_H
#define __MACH_SUNXI_CLK_SUN3IW1_H

/* register list */
#define PLL_CPU             0x0000
#define PLL_AUDIO           0x0008
#define PLL_VIDEO           0x0010
#define PLL_VE              0x0018
#define PLL_DDR             0x0020
#define PLL_PERIPH          0x0028

#define CPU_CFG             0x0050
#define AHB1_CFG            0x0054

#define BUS_GATE0           0x0060
#define BUS_GATE1           0x0064
#define BUS_GATE2           0x0068


#define SD0_CFG             0x0088
#define SD1_CFG             0x008c

#define SPI0_CFG            0x00A0
#define SPI1_CFG            0x00A4


#define AUDIO_CFG           0x00B0
#define SPDIF_CFG           0x00B4
#define CIR_CFG             0x00B8

#define USB_CFG             0x00CC

#define DRAM_GATE           0x0100

#define BE_CFG              0x0104
#define FE_CFG              0x010C

#define TCON_CFG            0x0118
#define DEINTERLACE_CFG     0x011C
#define TVE_CFG             0x0120
#define TVD_CFG             0x0124

#define CSI_CFG             0x0134
#define VE_CFG              0x013C


#define CODEC_CFG           0x0140
#define AVS_CFG             0x0144

#define PLL_LOCK            0x0200
#define CPU_LOCK            0x0204

#define PLL_AUDIOPAT        0x0284
#define PLL_VIDEOPAT        0x0288
#define PLL_DDRPAT          0x0290
#define PLL_CLK_CTRL        0x0320

#define BUS_RST0            0x02C0
#define BUS_RST1            0x02C4
#define BUS_RST2            0x02D0

#define SUNXI_CLK_MAX_REG   0x02D0

//#define LOSC_OUT_GATE       0x01C20460

#define F_N8X7_M0X4(nv,mv) FACTOR_ALL(nv,8,7,0,0,0,mv,0,4,0,0,0,0,0,0,0,0,0)
#define F_N8X5_K4X2(nv,kv) FACTOR_ALL(nv,8,5,kv,4,2,0,0,0,0,0,0,0,0,0,0,0,0)
#define F_N8X7_M0X2(nv,mv) FACTOR_ALL(nv,8,7,0,0,0,mv,0,2,0,0,0,0,0,0,0,0,0)
#define F_N8X5_K4X2_M0X2(nv,kv,mv) FACTOR_ALL(nv,8,5,kv,4,2,mv,0,2,0,0,0,0,0,0,0,0,0)
#define F_N8X5_K4X2_M0X2_P16x2(nv,kv,mv,pv) \
               FACTOR_ALL(nv,8,5, \
                          kv,4,2, \
                          mv,0,2, \
                          pv,16,2, \
                          0,0,0,0,0,0)
#define F_N8X7_N116X5_M0X2_M14x4(nv,kv,mv,pv) \
               FACTOR_ALL(nv,8,7, \
                          kv,16,5, \
                          mv,0,2, \
                          pv,4,4, \
                          0,0,0,0,0,0)


#define PLLCPU(n,k,m,p,freq)   {F_N8X5_K4X2_M0X2_P16x2(n, k, m, p),  freq}
#define PLLAUDIO(n,m,freq)     {F_N8X7_M0X4( n, m),  freq}
#define PLLVIDEO(n,m,freq)     {F_N8X7_M0X4( n, m),  freq}
#define PLLVE(n,m,freq)        {F_N8X7_M0X4( n, m),  freq}
#define PLLDDR(n,k,m,freq)     {F_N8X5_K4X2_M0X2( n, k, m),  freq}
#define PLLPERIPH(n,k,freq)    {F_N8X5_K4X2( n, k),  freq}

#endif
