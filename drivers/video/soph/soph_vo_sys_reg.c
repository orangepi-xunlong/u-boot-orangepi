#include "soph_vo_sys_reg.h"
#include <mmio.h>

u32  vo_sys_f_base;
u32  vo_sys_b_base;
u32  top_pll_base;

u32 _reg_read(uintptr_t addr)
{
	u32 value;

	value = mmio_read_32(addr);
	return value;
}

void _reg_write(uintptr_t addr, u32 value)
{
	mmio_write_32(addr, value);
}

void _reg_write_mask(uintptr_t addr, u32 mask, u32 data)
{
	u32 value;

	value = mmio_read_32(addr) & ~mask;
	value |= (data & mask);
	mmio_write_32(addr, value);
}

void extend_axi_to_36bit(u32 high_bit, enum drm_intf intf)
{
	switch (intf) {
	case DRM_INTF_DISP0:
		_reg_write_mask(REG_VO_SYS_AXI_ADDR_EXT_OW, 0xff000, 0xff000);
		_reg_write(REG_VO_SYS_AXI_ADDR_EXT2, (high_bit) | (high_bit << 4) | (high_bit << 8) | (high_bit << 12) |
									     (high_bit << 16) | (high_bit << 20) | (high_bit << 24) | (high_bit << 28));
		break;
	case DRM_INTF_DISP1:
		_reg_write_mask(REG_VO_SYS_AXI_ADDR_EXT_OW, 0xff00000, 0xff00000);
		_reg_write(REG_VO_SYS_AXI_ADDR_EXT3, (high_bit) | (high_bit << 4) | (high_bit << 8) | (high_bit << 12) |
									     (high_bit << 16) | (high_bit << 20) | (high_bit << 24) | (high_bit << 28));
		break;
	default:
		printf("drm intf %d is not support\n", intf);
		break;
	}
}
