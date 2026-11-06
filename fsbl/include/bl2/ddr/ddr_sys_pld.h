#ifndef __DDR_SYS_H__
#define __DDR_SYS_H__

#include <mmio.h>
#include <debug.h>

extern uint32_t  freq_in;
extern uint32_t  tar_freq;
extern uint32_t  mod_freq;
extern uint32_t  dev_freq;
extern uint64_t  reg_set;
extern uint64_t  reg_span;
extern uint64_t  reg_step;

//extern uint64_t  debug_seqnum;
//extern uint64_t  debug_seqnum1;

extern uint32_t rddata;

enum mem_cs_e {
	JEDEC_DDR4_16G_X4_3200AA,
	JEDEC_DDR4_16G_X8_3200AA,
	JEDEC_DDR4_2G_X8_3200AC,
};

enum cl_e {
	CL_18 = 18,
	CL_20 = 20,
	CL_24 = 24,
};

enum bl_e {
	BL_2 = 1,
	BL_4 = 2,
	BL_8 = 3,
};

enum ddr_type_e {
	LP4,
	LP4_2CH_1PHYD,
	DDR4_X8,
	DDR4_X16,
};

enum freq_mode_e {
	MEM_FREQ_4266,
	MEM_FREQ_3733,
	MEM_FREQ_3200,
};

enum dllbp_mode_e {
	ENABLE,
	DISABLE,
};

enum rdimm_mode_e {
	SUPPORT,
	UNSUPPORT,
};

enum low_power_state_e {
	IDLE = 0,
	ACT_POWER_DOWN = 1,
	ACT_POWER_DOWN_MEM_CG = 2,
	PRE_CHARGE_PD = 3,
	PRE_CHARGE_PD_MEM_CG = 4,
	SELF_REFRESH_SHORT = 5,
	SELF_REFRESH_SHORT_MEM_CG = 6,
	SELF_REFRESH_LONG = 8,
	SELF_REFRESH_LONG_MEM_CG = 9,
	SELF_REFRESH_LONG_MC_CG = 10,
};

#ifdef CADENCE_DDRC
	#define DDR_PI_BASE_ADDR 0x2000
	#define DDR_PHY_BASE_ADDR 0x4000
	#define DDR_DFS_BASE_ADDR 0x7000
	#define DENALI_CTL_REG(reg_num) ((reg_num)*4)
	#define DENALI_PI_REG(reg_num) (((reg_num)*4) + DDR_PI_BASE_ADDR)
	#define DENALI_PHY_REG(reg_num) (((reg_num)*4) + DDR_PHY_BASE_ADDR)
	#define DENALI_DFS_REG(reg_num) (((reg_num)*4) + DDR_DFS_BASE_ADDR)
#else
	#define DDR_DRAM_BASE 0x100000000
	#define DDR_DRAM_END  0x4ffffffff
	#define DDR_TOP_CFG_BASE 0x6fff0000
	#define DDR_TOP_CFG_END 0x6fff3fff

	#define DDR_CFG_SYS1_BASE		0x70000000
	#define DDR_CFG_SYS1_END		0x77ffffff
	#define DDR_PHYD2_CH0_SYS1_BASE 0x70000000
	#define DDR_PHYD2_CH0_SYS1_END	0x70003fff
	#define DDR_PHYD_CH0_SYS1_BASE	0x70006000
	#define DDR_PHYD_CH0_SYS1_END   0x70007fff
	#define DDR_CTRL_SYS1_BASE		0x70004000
	#define DDR_CTRL_SYS1_END		0x70005fff
	#define	DDR_AXI_MON_SYS1_BASE	0x70008000
	#define DDR_AXI_MON_SYS1_END	0x70009fff
	#define DDR_TOP_SYS1_BASE		0x7000a000
	#define DDR_TOP_SYS1_END		0x7000ffff
	#define DDR_BIST_CH0_SYS1_BASE  0x70010000
	#define DDR_BIST_CH0_SYS1_END	0x70011fff
	#define DDR_BIST_CH1_SYS1_BASE  0x70030000
	#define DDR_BIST_CH1_SYS1_END	0x70031fff

	#define DDR_CFG_SYS2_BASE		0x78000000
	#define DDR_CFG_SYS2_END		0x7fffffff
	#define DDR_PHYD2_CH0_SYS2_BASE 0x78000000
	#define DDR_PHYD2_CH0_SYS2_END	0x78003fff
	#define DDR_PHYD_CH0_SYS2_BASE	0x78006000
	#define DDR_PHYD_CH0_SYS2_END   0x78007fff
	#define DDR_CTRL_SYS2_BASE		0x78004000
	#define DDR_CTRL_SYS2_END		0x78005fff
	#define	DDR_AXI_MON_SYS2_BASE	0x78008000
	#define DDR_AXI_MON_SYS2_END	0x78009fff
	#define DDR_TOP_SYS2_BASE		0x7800a000
	#define DDR_TOP_SYS2_END		0x7800ffff
	#define DDR_BIST_CH0_SYS2_BASE  0x78010000
	#define DDR_BIST_CH0_SYS2_END	0x78011fff
	#define DDR_BIST_CH1_SYS2_BASE  0x78030000
	#define DDR_BIST_CH1_SYS2_END	0x78031fff

#endif

enum bist_mode {
	E_PRBS,
	E_SRAM,
};

enum train_mode {
	E_WRLVL,
	E_RDGLVL,
	E_WDQLVL,
	E_RDLVL,
	E_WDQLVL_SW,
	E_RDLVL_SW,
};

#define CEILING_POS(X) ((X - (int)(X)) > 0 ? (int)(X + 1) : (int)(X))
#define CEILING_NEG(X) ((X - (int)(X)) < 0 ? (int)(X - 1) : (int)(X))
#define ceil(X) (((X) > 0) ? CEILING_POS(X) : CEILING_NEG(X))

//#define PHY_BASE_ADDR		2048
//#define PI_BASE_ADDR		0
//#define CADENCE_PHYD		0x08000000
//#define CADENCE_PHYD_APB	0x08006000
//#define cfg_base		0x08004000

//#define DDR_SYS_BASE		0x08000000
//#define PI_BASE			(DDR_SYS_BASE + 0x0000)
//#define PHY_BASE		(DDR_SYS_BASE + 0x2000)
//#define DDRC_BASE		(DDR_SYS_BASE + 0x4000)
//#define PHYD_BASE		(DDR_SYS_BASE + 0x6000)
//#define CV_DDR_PHYD_APB	(DDR_SYS_BASE + 0x6000)
//#define AXI_MON_BASE		(DDR_SYS_BASE + 0x8000)
//#define TOP_BASE		(DDR_SYS_BASE + 0xa000)
//#define DDR_TOP_BASE		(DDR_SYS_BASE + 0xa000)
//#define PHYD_BASE_ADDR		(DDR_SYS_BASE)
//#define DDR_BIST_BASE		0x08010000
//#define DDR_BIST_SRAM_DQ_BASE	0x08011000
//#define DDR_BIST_SRAM_DM_BASE	0x08011800

#define mmio_wr32	mmio_write_32
#define mmio_rd32	mmio_read_32

// #define ddr_mmio_rd32(a, b)	do { if (1) b = mmio_rd32(a); } while (0)
// #define ddr_sram_rd32(a, b)	do { if (1) b = mmio_rd32(a); } while (0)

#define ddr_sram_wr32(a, b)	mmio_wr32(a, b)
#define ddr_debug_wr32(b)

// #define uartlog(...) tf_printf(MSG_NOTICE "U: " __VA_ARGS__)
// #define KC_MSG(...) tf_printf(MSG_NOTICE "[KC_DBG] " __VA_ARGS__)
// #define KC_MSG_TR(...) tf_printf(MSG_NOTICE "[KC_DBG_training]" __VA_ARGS__)
// #define TJ_MSG(...) tf_printf(MSG_NOTICE "[TJ Info] : " __VA_ARGS__)

#define uartlog(...)
#define KC_MSG(...)
#define KC_MSG_TR(...)
#define TJ_MSG(...)

#ifdef DBG_SHMOO
#define SHMOO_MSG(...)	tf_printf(MSG_NOTICE __VA_ARGS__)
#else
#define SHMOO_MSG(...)
#endif

#ifdef DBG_SHMOO_CA
#define SHMOO_MSG_CA(...)	tf_printf(MSG_NOTICE __VA_ARGS__)
#else
#define SHMOO_MSG_CA(...)
#endif

#ifdef DBG_SHMOO_CS
#define SHMOO_MSG_CS(...)	tf_printf(MSG_NOTICE __VA_ARGS__)
#else
#define SHMOO_MSG_CS(...)
#endif


void cvx32_setting_check(uintptr_t base_addr_ctrl, uintptr_t base_addr_phyd);
void cvx32_pinmux(uintptr_t base_addr_phyd);
void cvx32_clk_div40(uintptr_t cv_base_addr_phyd);
void cvx32_clk_normal(uintptr_t cv_base_addr_phyd);
void cvx32_clk_div2(uintptr_t cv_base_addr_phyd);
void cvx32_dll_sw_clr(uintptr_t base_addr_ctrl, uintptr_t base_addr_phyd);
void cvx32_bist_rdlvl_init(uint32_t mode, uint32_t ddr_freq_mode, uint32_t rank,
			uintptr_t base_addr_bist, uintptr_t base_addr_phyd, int ddr_type);
void cvx32_dll_cal_status(uintptr_t base_addr_phyd, uint32_t ddr_freq_mode);
void cvx32_synp_mrr_lp4(uint32_t rank, uint32_t addr, uintptr_t base_addr_ctrl);
void cvx32_polling_synp_normal_mode(uintptr_t base_addr_ctrl);
void cvx32_wait_for_dfi_init_complete(uintptr_t base_addr_ctrl);
void cvx32_set_dfi_init_complete(uintptr_t base_addr_phyd);
void cvx32_ddr_phy_power_on_seq3(uintptr_t base_addr_phyd);
void cvx32_ddr_phy_power_on_seq2(uintptr_t base_addr_phyd, uintptr_t cv_base_addr_phyd, uint32_t ddr_freq_mode);
void cvx32_INT_ISR_08(uintptr_t base_addr_phyd, uintptr_t cv_base_addr_phyd, uint32_t ddr_freq_mode);
void cvx32_polling_dfi_init_start(uintptr_t base_addr_phyd);
void cvx32_ddr_phy_power_on_seq1(uintptr_t cv_base_addr_phyd);
void cvx32_set_dfi_init_start(uintptr_t base_addr_ctrl);
//void cvx32_setting_check(uintptr_t base_addr_ctrl, uintptr_t base_addr_phyd);
void ctrl_init_low_patch(uintptr_t base_addr_ctrl, uintptr_t ddr_top_base_addr);
void ctrl_init_high_patch(uintptr_t base_addr_ctrl, uintptr_t ddr_top_base_addr);
void cvx32_ctrlupd_short(uintptr_t base_addr_ctrl);
void cvx32_pll_init(uintptr_t cv_base_addr_phyd, int ddr_freq_mode);
void cvx32_ctrl_init(uintptr_t base_addr_ctrl, int ddr_type);
void cvx32_phy_init(uintptr_t base_addr_phyd, int ddr_type);
void cvx32_chg_pll_freq(uintptr_t cv_base_addr_phyd);
void cvx32_dll_cal(uintptr_t cv_base_addr_phyd, uintptr_t base_addr_phyd, uint32_t ddr_freq_mode);
void cvx32_bist_wr_sram_init(uintptr_t rank, uintptr_t base_addr_bist, int ddr_type, uint32_t ddr_freq_mode);
void cvx32_bist_start_check(uintptr_t base_addr_bist, uint32_t bist_result,	uint64_t err_data_odd, uint64_t err_data_even, int ddr_type);

//void check_rd32(uintptr_t addr, uint32_t expected); //unused
void ddr_debug_num_write(void);
//void cvx16_rdvld_train(void);
void ddr_sys_suspend(void);
void ddr_sys_resume(void);
//void cvx16_ddr_sub_resume2(void);
//void cvx16_ddr_sub_resume3(void);
//void cvx16_ddrc_suspend(void);
//void cvx16_bist_wr_prbs_init(void);
//void cvx16_bist_wr_sram_init(void);
//void cvx16_bist_wrlvl_init(void);
//void cvx16_bist_rdglvl_init(void);
//void cvx16_bist_rdlvl_init(uint32_t mode);
//void cvx16_bist_wdqlvl_init(uint32_t mode);
//void cvx16_bist_wdmlvl_init(void);
//void cvx16_bist_start_check(uint32_t *bist_result, uint64_t *err_data_odd, uint64_t *err_data_even);
//void cvx16_bist_tx_shift_delay(uint32_t shift_delay);
//void cvx16_bist_rx_delay(uint32_t delay);
//void cvx16_bist_rx_deskew_delay(uint32_t delay);
//void cvx16_ca_shift_delay(uint32_t shift_delay);
//void cvx16_cs_shift_delay(uint32_t shift_delay);
//void cvx16_synp_mrw(uint32_t addr, uint32_t data);
//void cvx16_chg_pll_freq(void);
//void cvx16_dll_cal(void);
//void cvx16_dll_cal_phyd_hw(void);
//void cvx16_dll_cal_phya_enautok(void);
//void cvx16_ddr_zqcal_isr8(void);
//void cvx16_ddr_zqcal_hw_isr8(uint32_t hw_mode);
//void cvx16_clk_normal(void);
//void cvx16_clk_div2(void);
//void cvx16_INT_ISR_08(void);
//void cvx16_clk_div40(void);
//void cvx16_ddr_phy_power_on_seq1(void);
//void cvx16_ddr_phy_power_on_seq2(void);
//void cvx16_ddr_phy_power_on_seq3(void);
//void cvx16_wait_for_dfi_init_complete(void);
//void cvx16_ctrlupd_short(void);
//void cvx16_polling_dfi_init_start(void);
//void cvx16_set_dfi_init_complete(void);
//void cvx16_polling_synp_normal_mode(void);
//void cvx16_dfi_ca_park_prbs(uint32_t cap_enable);
//void cvx16_wrlvl_req(void);
//void cvx16_rdglvl_req(void);
//void cvx16_rdlvl_req(uint32_t mode);
//void cvx16_rdlvl_sw_req(uint32_t mode);
//void cvx16_wdqlvl_req(uint32_t data_mode, uint32_t lvl_mode);
//void cvx16_wdqlvl_sw_req(uint32_t data_mode, uint32_t lvl_mode);
//void cvx16_wrlvl_status(void);
//void cvx16_rdglvl_status(void);
//void cvx16_rdlvl_status(void);
//void cvx16_wdqlvl_status(void);
//void cvx16_dll_cal_status(void);
//void cvx16_zqcal_status(void);
//void cvx16_training_status(void);
//void cvx16_setting_check(void);
//void cvx16_ddr_freq_change_htol(void);
//void cvx16_ddr_freq_change_ltoh(void);
//void cvx16_set_dq_vref(uint32_t vref);
//void cvx16_set_dfi_init_start(void);
//void cvx16_ddr_phya_pd(void);
//void cvx16_ddr_phyd_save(uint32_t sram_base_addr);
//void cvx16_ddr_phyd_restore(uint32_t sram_base_addr);
//void cvx16_dll_sw_upd(void);
//void cvx16_bist_mask_shift_delay(uint32_t shift_delay, uint32_t en_lead);
//void cvx16_set_dq_trig_lvl(uint32_t trig_lvl);
//void cvx16_pll_init(void);
//void cvx16_lb_0_phase40(void);
//void cvx16_lb_0_external(void);
//void cvx16_lb_1_dq_set_highlow(void);
//void cvx16_lb_2_mux_demux(void);
//void cvx16_lb_3_ca_set_highlow(void);
//void cvx16_lb_4_ca_clk_pat(void);
//void cvx16_clk_gating_disable(void);
//void cvx16_clk_gating_enable(void);
//void cvx16_dfi_phyupd_req(void);
//oid cvx16_en_rec_vol_mode(void);
//void cvx16_dll_sw_clr(void);
//void cvx16_reg_toggle(void);
//void cvx16_ana_test(void);
//void cvx16_ddr_zq240(void);
//void cvx16_ddr_zq240_ate(void);
//void cvx16_ddr_zq240_cal(void);
void ctrl_init_detect_dram_size(uint8_t *dram_cap_in_mbyte);
uint32_t ddr_bist_all(uint32_t mode, uint32_t capacity, uint32_t x16_mode);
uint32_t bist_all_dram(uint32_t mode, uint32_t capacity);
void bist_all_dram_forever(uint32_t capacity);
void calvl_req(uint32_t capacity);
void cslvl_req(uint32_t capacity);
void ddr_training(enum train_mode t_mode);
#endif /* __DDR_SYS_H__ */
