/*
 * (C) Copyright 2016-2017
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */


#include <common.h>
#include <asm/arch/rsb.h>
#include <sunxi_board.h>

#define  SUNXI_RSB_SLAVE_MAX      2
struct sunxi_rsb_slave_set
{
	u32 m_slave_addr;
	u32 m_rtaddr;
	u32 chip_id;
};


#undef rsb_printk
#define rsb_printk(format,arg...)   pr_msg(format,##arg)

__attribute__((section(".data")))
static struct rsb_info rsbc;

/*runtime addr: 0x2d, 0x3a*/
__attribute__((section(".data")))
static struct sunxi_rsb_slave_set rsb_slave[SUNXI_RSB_SLAVE_MAX] = \
{
	#ifdef CONFIG_ARCH_SUN50IW3P1
	/*AXP81X_ADDR*/
	/*slave_addr, runtime_addr, ID */
	{0x3a3,       0x2d,         0x11},
	{0,           0,            0,}
	#elif defined(CONFIG_ARCH_SUN8IW12P1)
	/*AXP809_CHIP_ID*/
	/*slave_addr, runtime_addr, ID */
	{0x3a3,       0x2d,         0x12},
	{0,           0,            0,}
	#endif
};



static s32 sunxi_rsb_config_null(u32 slave_id, u32 rsb_addr)
{
	return -1;
}

static s32 sunxi_rsb_io_null(u32 slave_id, u32 daddr, u8 *data, u32 len)
{
	return -1;
}

s32 (* sunxi_rsb_config_pt)(u32 slave_id, u32 rsb_addr) = sunxi_rsb_config_null;
s32 (* sunxi_rsb_read_pt)(u32 slave_id, u32 daddr, u8 *data, u32 len) = sunxi_rsb_io_null;
s32 (* sunxi_rsb_write_pt)(u32 slave_id, u32 daddr, u8 *data, u32 len) = sunxi_rsb_io_null;



static void rsb_cfg_io(void)
{
	uint reg,val;

	//PL0,PL1 cfg 2
	reg = SUNXI_RPIO_BASE+0x0;
	val = readl(reg)  & (~0xff);
	val |= 0x22;
	writel(val,reg);

	//PL0,PL1 drv 2
	reg = SUNXI_RPIO_BASE+0x14;
	val = readl(reg)  & (~0xf);
	val |= 0xa;
	writel(val,reg);

	//PL0,PL1 pull up 1
	reg = SUNXI_RPIO_BASE+0x1c;
	val = readl(reg)  & (~0xf);
	val |= 0x5;
	writel(val,reg);

}

static void rsb_bus_gating_reset(void)
{
	u32 reg_value;
	u32 reg_addr;
	//R_RSB Bus Gating Reset Reg
	//bit0: R_RSB Gating Clock
	//bit16: R_RSB Reset
	reg_addr = SUNXI_RPRCM_BASE + 0x1bc;

	//rsb reset
	reg_value = readl(reg_addr);
	reg_value |=  (1<<16);
	writel(reg_value, reg_addr);

	//rsb gating
	reg_value |= (1 << 0);
	writel(reg_value, reg_addr);

}

static void rsb_set_clk(u32 sck)
{
	u32 src_clk = 0;
	u32 div = 0;
	u32 cd_odly = 0;
	u32 rval = 0;

	src_clk = 24000000;

	div = src_clk/sck/2;
	if(0==div){
		div = 1;
		rsb_printk("Source clock is too low\n");
	}else if(div>256){
		div = 256;
		rsb_printk("Source clock is too high\n");
	}
	div--;
	cd_odly = div >> 1;
	//cd_odly = 1;
	if(!cd_odly)
		cd_odly = 1;
	rval = div | (cd_odly << 8);
	writel(rval, RSB_REG_CCR);
}


static s32 rsb_send_initseq(u32 slave_addr, u32 reg, u32 data)
{

	while(readl(RSB_REG_STAT)&(RSB_LBSY_INT|RSB_TERR_INT|RSB_TOVER_INT))
	{
		rsb_printk("status err\n");
	}

	rsbc.rsb_busy = 1;
	rsbc.rsb_flag = 0;
	rsbc.rsb_load_busy = 0;
	writel(RSB_PMU_INIT|(slave_addr << 1)				\
					|(reg << PMU_MOD_REG_ADDR_SHIFT)			\
					|(data << PMU_INIT_DAT_SHIFT), 				\
					RSB_REG_PMCR);
	while(readl(RSB_REG_PMCR) & RSB_PMU_INIT){
	}


	while(rsbc.rsb_busy){

		//istat will be optimize?
		u32 istat = readl(RSB_REG_STAT);

		if(istat & RSB_LBSY_INT){
			rsbc.rsb_load_busy = 1;
			writel(istat, RSB_REG_STAT);
			break;
		}

		if (istat & RSB_TERR_INT) {
			rsbc.rsb_flag = (istat >> 8) & 0xffff;
			writel(istat, RSB_REG_STAT);
			break;
		}

		if (istat & RSB_TOVER_INT) {
			rsbc.rsb_busy = 0;
			writel(istat, RSB_REG_STAT);
		}

	}

	if(rsbc.rsb_load_busy){
		rsb_printk("Load busy\n");
		return RET_FAIL;
	}

	if (rsbc.rsb_flag) {
		rsb_printk(	"rsb write failed, flag 0x%x:%s%s%s%s%s !!\n",
					rsbc.rsb_flag,
					rsbc.rsb_flag & ERR_TRANS_1ST_BYTE	? " 1STE "  : "",
					rsbc.rsb_flag & ERR_TRANS_2ND_BYTE	? " 2NDE "  : "",
					rsbc.rsb_flag & ERR_TRANS_3RD_BYTE	? " 3RDE "  : "",
					rsbc.rsb_flag & ERR_TRANS_4TH_BYTE	? " 4THE "  : "",
					rsbc.rsb_flag & ERR_TRANS_RT_NO_ACK	? " NOACK "	: ""
					);
		return -rsbc.rsb_flag;
	}

	return 0;
}


static s32 set_run_time_addr(u32 saddr,u32 rtsaddr)
{
	while(readl(RSB_REG_STAT)&(RSB_LBSY_INT|RSB_TERR_INT|RSB_TOVER_INT))
	{
		rsb_printk("status err\n");
	}

	rsbc.rsb_busy = 1;
	rsbc.rsb_flag = 0;
	rsbc.rsb_load_busy = 0;
	writel((saddr<<RSB_SADDR_SHIFT)						\
					|(rtsaddr<<RSB_RTSADDR_SHIFT),				\
					RSB_REG_SADDR);
	writel(RSB_CMD_SET_RTSADDR,RSB_REG_CMD);
	writel(readl(RSB_REG_CTRL)|RSB_START_TRANS, RSB_REG_CTRL);

	while(rsbc.rsb_busy){

		//istat will be optimize?
		u32 istat = readl(RSB_REG_STAT);

		if(istat & RSB_LBSY_INT){
			rsbc.rsb_load_busy = 1;
			writel(istat, RSB_REG_STAT);
			break;
		}

		if (istat & RSB_TERR_INT) {
			rsbc.rsb_flag = (istat >> 8) & 0xffff;
			writel(istat, RSB_REG_STAT);
			break;
		}

		if (istat & RSB_TOVER_INT) {
			rsbc.rsb_busy = 0;
			writel(istat, RSB_REG_STAT);
		}
	}

	if(rsbc.rsb_load_busy){
		rsb_printk("Load busy\n");
		return RET_FAIL;
	}

	if (rsbc.rsb_flag) {
		rsb_printk(	"rsb set run time address failed, flag 0x%x:%s%s%s%s%s !!\n",
					rsbc.rsb_flag,
					rsbc.rsb_flag & ERR_TRANS_1ST_BYTE	? " 1STE "  : "",
					rsbc.rsb_flag & ERR_TRANS_2ND_BYTE	? " 2NDE "  : "",
					rsbc.rsb_flag & ERR_TRANS_3RD_BYTE	? " 3RDE "  : "",
					rsbc.rsb_flag & ERR_TRANS_4TH_BYTE	? " 4THE "  : "",
					rsbc.rsb_flag & ERR_TRANS_RT_NO_ACK	? " NOACK "	: ""
					);
		return -rsbc.rsb_flag;
	}
	return 0;
}


static s32 rsb_write(u32 rtsaddr,u32 daddr, u8 *data,u32 len)
{
	u32 cmd = 0;
	u32 dat = 0;
	s32 i	= 0;
	if (len > 4 || len==0||len==3) {
		rsb_printk("error length %d\n", len);
		return -1;
	}
	if(NULL==data){
		rsb_printk("data should not be NULL\n");
		return -1;
	}

	while(readl(RSB_REG_STAT)&(RSB_LBSY_INT|RSB_TERR_INT|RSB_TOVER_INT))
	{
		rsb_printk("status err\n");
	}

	rsbc.rsb_flag = 0;
	rsbc.rsb_busy = 1;
	rsbc.rsb_load_busy	= 0;

	writel(rtsaddr<<RSB_RTSADDR_SHIFT,RSB_REG_SADDR);
	writel(daddr, RSB_REG_DADDR0);

	for(i=0;i<len;i++){
		dat |= data[i]<<(i*8);
	}

	writel(dat, RSB_REG_DATA0);

	switch(len)	{
	case 1:
		cmd = RSB_CMD_BYTE_WRITE;
		break;
	case 2:
		cmd = RSB_CMD_HWORD_WRITE;
		break;
	case 4:
		cmd = RSB_CMD_WORD_WRITE;
		break;
	default:
		break;
	}
	writel(cmd,RSB_REG_CMD);

	writel(readl(RSB_REG_CTRL)|RSB_START_TRANS, RSB_REG_CTRL);
	while(rsbc.rsb_busy){

		//istat will be optimize?
		u32 istat = readl(RSB_REG_STAT);

		if(istat & RSB_LBSY_INT){
			rsbc.rsb_load_busy = 1;
			writel(istat, RSB_REG_STAT);
			break;
		}

		if (istat & RSB_TERR_INT) {
			rsbc.rsb_flag = (istat >> 8) & 0xffff;
			writel(istat, RSB_REG_STAT);
			break;
		}

		if (istat & RSB_TOVER_INT) {
			rsbc.rsb_busy = 0;
			writel(istat, RSB_REG_STAT);
		}

	}

	if(rsbc.rsb_load_busy){
		rsb_printk("Load busy\n");
		return RET_FAIL;
	}

	if (rsbc.rsb_flag) {
		rsb_printk(	"rsb write failed, flag 0x%x:%s%s%s%s%s !!\n",
					rsbc.rsb_flag,
					rsbc.rsb_flag & ERR_TRANS_1ST_BYTE	? " 1STE "  : "",
					rsbc.rsb_flag & ERR_TRANS_2ND_BYTE	? " 2NDE "  : "",
					rsbc.rsb_flag & ERR_TRANS_3RD_BYTE	? " 3RDE "  : "",
					rsbc.rsb_flag & ERR_TRANS_4TH_BYTE	? " 4THE "  : "",
					rsbc.rsb_flag & ERR_TRANS_RT_NO_ACK	? " NOACK "	: ""
					);
		return -rsbc.rsb_flag;
	}

	return 0;
}


static s32 rsb_read(u32 rtsaddr,u32 daddr, u8 *data, u32 len)
{
	u32 cmd = 0;
	u32 dat = 0;
	s32 i	= 0;
	if (len > 4 || len==0||len==3) {
		rsb_printk("error length %d\n", len);
		return -1;
	}
	if(NULL==data){
		rsb_printk("data should not be NULL\n");
		return -1;
	}

	while(readl(RSB_REG_STAT)&(RSB_LBSY_INT|RSB_TERR_INT|RSB_TOVER_INT))
	{
		rsb_printk("status err\n");
	}

	rsbc.rsb_flag = 0;
	rsbc.rsb_busy = 1;
	rsbc.rsb_load_busy	= 0;

	writel(rtsaddr<<RSB_RTSADDR_SHIFT,RSB_REG_SADDR);
	writel(daddr, RSB_REG_DADDR0);

	switch(len){
	case 1:
		cmd = RSB_CMD_BYTE_READ;
		break;
	case 2:
		cmd = RSB_CMD_HWORD_READ;
		break;
	case 4:
		cmd = RSB_CMD_WORD_READ;
		break;
	default:
		break;
	}
	writel(cmd,RSB_REG_CMD);

	writel(readl(RSB_REG_CTRL)|RSB_START_TRANS, RSB_REG_CTRL);
	while(rsbc.rsb_busy){

		u32 istat = readl(RSB_REG_STAT);

		if(istat & RSB_LBSY_INT){
			rsbc.rsb_load_busy = 1;
			writel(istat, RSB_REG_STAT);
			break;
		}

		if (istat & RSB_TERR_INT) {
			rsbc.rsb_flag = (istat >> 8) & 0xffff;
			writel(istat, RSB_REG_STAT);
			break;
		}

		if (istat & RSB_TOVER_INT) {
			rsbc.rsb_busy = 0;
			writel(istat, RSB_REG_STAT);
		}

	}

	if(rsbc.rsb_load_busy){
		rsb_printk("Load busy\n");
		return RET_FAIL;
	}

	if (rsbc.rsb_flag) {
		rsb_printk(	"rsb read failed, flag 0x%x:%s%s%s%s%s !!\n",
					rsbc.rsb_flag,
					rsbc.rsb_flag & ERR_TRANS_1ST_BYTE	? " 1STE "  : "",
					rsbc.rsb_flag & ERR_TRANS_2ND_BYTE	? " 2NDE "  : "",
					rsbc.rsb_flag & ERR_TRANS_3RD_BYTE	? " 3RDE "  : "",
					rsbc.rsb_flag & ERR_TRANS_4TH_BYTE	? " 4THE "  : "",
					rsbc.rsb_flag & ERR_TRANS_RT_NO_ACK	? " NOACK "	: ""
					);
		return -rsbc.rsb_flag;
	}

	dat = readl(RSB_REG_DATA0);
	for(i=0;i<len;i++){
		data[i]=(dat>>(i*8))&0xff;
	}


	return 0;
}

static int rsb_init(void)
{
	int ret;

	rsbc.rsb_flag = 0;
	rsbc.rsb_busy = 0;
	rsbc.rsb_load_busy	= 0;

	rsb_cfg_io();
	rsb_bus_gating_reset();

	writel(RSB_SOFT_RST, RSB_REG_CTRL);

	// rsb clk = 400Khz
	rsb_set_clk(400000);
	ret = rsb_send_initseq(0x00, 0x3e, 0x7c);
	if(ret)
	{
		return ret;
	}
	// rsb clk = 3Mhz
	rsb_set_clk(RSB_SCK);
	pr_msg("rsb_send_initseq: rsb clk 400Khz -> 3Mhz\n");

	return ret;
}

static int rsb_exit(void)
{
	return 0;
}


static int rsb_get_rtaaddr(u32 slave_id)
{
	u32 tmp_slave_id;
	int i;
	for(i=0; i< SUNXI_RSB_SLAVE_MAX; i++)
	{
		tmp_slave_id = rsb_slave[i].chip_id;
		if(tmp_slave_id == slave_id)
		{
			break;
		}
		else if(!tmp_slave_id)
		{
			printf("sunxi_rsb_write err: bad id\n");
			return 0;
		}
		i++;
	}

	return rsb_slave[i].m_rtaddr;
}

/* for cpux config*/
static int rsb_config_by_cpux(u32 slave_id, u32 saddr)
{
	int rtaddr = 0;
	rtaddr = rsb_get_rtaaddr(slave_id);
	if(!rtaddr)
	{
		return -1;
	}
	return set_run_time_addr(saddr, rtaddr);
}

/* for cpux read*/
static int rsb_read_by_cpux(u32 slave_id, u32 daddr, u8 *data, u32 len)
{
	int rtaddr = 0;
	rtaddr = rsb_get_rtaaddr(slave_id);
	if(!rtaddr)
	{
		return -1;
	}
	return rsb_read(rtaddr, daddr, data, len);
}

/* for cpux write*/
static int rsb_write_by_cpux(u32 slave_id, u32 daddr, u8 *data, u32 len)
{
	int rtaddr = 0;
	rtaddr = rsb_get_rtaaddr(slave_id);
	if(!rtaddr)
	{
		return -1;
	}
	return rsb_write(rtaddr, daddr, data, len);
}

/* for cpus config*/
__maybe_unused static int rsb_config_by_cpus(u32 slave_id, u32 saddr)
{
	return 0;
}

/* for cpus read*/
__maybe_unused static int rsb_read_by_cpus(u32 slave_id, u32 daddr, u8 *data, u32 len)
{
	int ret = 0;
	ret = (u8)(arm_svc_arisc_read_pmu((ulong)daddr));
	if(ret < 0 )
	{
		return -1;
	}
	*data = ret&0xff;
	return 0;
}

/* for cpus write*/
__maybe_unused static int rsb_write_by_cpus(u32 slave_id, u32 daddr, u8 *data, u32 len)
{
	return arm_svc_arisc_write_pmu((ulong)daddr,(u32)(*data));
}


int sunxi_rsb_config(u32 slave_id, u32 rsb_addr)
{
	return sunxi_rsb_config_pt(slave_id, rsb_addr);
}

int sunxi_rsb_read(u32 slave_id, u32 daddr, u8 *data, u32 len)
{
	return sunxi_rsb_read_pt(slave_id, daddr, data, len);
}


int sunxi_rsb_write(u32 slave_id, u32 daddr, u8 *data, u32 len)
{
	return sunxi_rsb_write_pt(slave_id, daddr, data, len);
}

int sunxi_rsb_init(u32 uused)
{
	if(sunxi_probe_secure_monitor())
	{
		pr_msg("rsb init by cpus\n");
		sunxi_rsb_config_pt = rsb_config_by_cpus;
		sunxi_rsb_read_pt   = rsb_read_by_cpux;
		sunxi_rsb_write_pt  = rsb_write_by_cpux;
	}
	else
	{
		pr_msg("rsb init by cpux\n");
		rsb_init();
		sunxi_rsb_config_pt = rsb_config_by_cpux;
		sunxi_rsb_read_pt   = rsb_read_by_cpux;
		sunxi_rsb_write_pt  = rsb_write_by_cpux;
	}

	return 0;
}

int sunxi_rsb_exit(u32 uused)
{
	rsb_exit();
	return 0;
}
