/*
 * sunxi mipi bsp interface
 *
 * Copyright (c) 2017 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  wangxuan <wangxuan@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "bsp_mipi_csi.h"
#include "../utility/vin_io.h"

void bsp_mipi_csi_set_version(unsigned int sel, unsigned int ver)
{
	return;
}

int bsp_mipi_csi_set_base_addr(unsigned int sel, unsigned long addr_base)
{
	return 0;
}

int bsp_mipi_dphy_set_base_addr(unsigned int sel, unsigned long addr_base)
{
	return 0;
}

void bsp_mipi_csi_dphy_init(unsigned int sel)
{
	return;
}

void bsp_mipi_csi_dphy_exit(unsigned int sel)
{
	return;
}

void bsp_mipi_csi_dphy_enable(unsigned int sel)
{
	return;
}

void bsp_mipi_csi_dphy_disable(unsigned int sel)
{
	return;
}

void bsp_mipi_csi_protocol_enable(unsigned int sel)
{
	return;
}

void bsp_mipi_csi_protocol_disable(unsigned int sel)
{
	return;
}

void bsp_mipi_csi_set_para(unsigned int sel, struct mipi_para *para)
{
	return;
}
/*
void bsp_mipi_csi_set_fmt(unsigned int sel, unsigned int total_rx_ch,
			  struct mipi_fmt_cfg *fmt)
{
	return;
}
*/
