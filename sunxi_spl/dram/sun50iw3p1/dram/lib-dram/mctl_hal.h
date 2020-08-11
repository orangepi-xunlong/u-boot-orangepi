#ifndef _MCTL_HAL_H
#define  _MCTL_HAL_H
/*****************************************
Verify Platform
1.FPGA_VERIFY --- FPGA Verify
2.SIM_VERIFY --- Simulation
*****************************************/
#define FPGA_VERIFY
//#define SIM_VERIFY

#ifdef SIM_VERIFY
/*****************************************
 Function: DRAM Type Choose
*****************************************/
#define   DRAM_TYPE_DDR3
//#define   DRAM_TYPE_LPDDR2
//#define   DRAM_TYPE_LPDDR3
//#define   DRAM_TYPE_DDR2
/*****************************************
 Function: CLK Source Choose
*****************************************/
#define SOURCE_CLK_USE_DDR_PLL1
//#define SOURCE_CLK_USE_DDR_PLL0
//#define OPEN_ANOTHER_SOURCE
/*****************************************
 Function: DQS Mode Choose
*****************************************/
//#define DRAMC_USE_AUTO_DQS_GATE_PD_MODE
//#define DRAMC_USE_DQS_GATING_MODE
#define DRAMC_USE_AUTO_DQS_GATE_PU_MODE

/*****************************************
 Function: No Care
*****************************************/
#if defined DRAM_TYPE_LPDDR2 || defined DRAM_TYPE_LPDDR3
#define DRAMC_USE_1T_MODE
#else
#define DRAMC_USE_2T_MODE
#endif
/*****************************************
 Function: Close Print
*****************************************/
#define dram_dbg(fmt,args...)
#else
/*****************************************
 Function: Open Print
*****************************************/
#define dram_dbg(fmt,args...)	printk(fmt ,##args)
#endif

typedef struct __DRAM_PARA
{
	unsigned int        dram_clk;
	unsigned int        dram_type;
	unsigned int        dram_zq;
	unsigned int		dram_odt_en;
	unsigned int		dram_para1;
	unsigned int		dram_para2;
	unsigned int		dram_mr0;
	unsigned int		dram_mr1;
	unsigned int		dram_mr2;
	unsigned int		dram_mr3;
	unsigned int		dram_tpr0;
	unsigned int		dram_tpr1;
	unsigned int		dram_tpr2;
	unsigned int		dram_tpr3;
	unsigned int		dram_tpr4;
	unsigned int		dram_tpr5;
	unsigned int		dram_tpr6;
	unsigned int		dram_tpr7;
	unsigned int		dram_tpr8;
	unsigned int		dram_tpr9;
	unsigned int		dram_tpr10;
	unsigned int		dram_tpr11;
	unsigned int		dram_tpr12;
	unsigned int		dram_tpr13;
	unsigned int		dram_tpr14;
	unsigned int		dram_tpr15;
	unsigned int		dram_tpr16;
	unsigned int		dram_tpr17;
	unsigned int		dram_tpr18;
	unsigned int		dram_tpr19;
	unsigned int		dram_tpr20;
	unsigned int		dram_tpr21;
}__dram_para_t;


extern unsigned int mctl_init(void *para);
extern signed int init_DRAM(int type, __dram_para_t *para);
extern void dram_udelay(unsigned int n);
#endif  //_MCTL_HAL_H

