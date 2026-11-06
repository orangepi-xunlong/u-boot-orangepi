#ifndef __DDR_SYS_H__
#define __DDR_SYS_H__

#include <mmio.h>
#include <debug.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern uint32_t  freq_in;
//extern uint32_t  tar_freq;
//extern uint32_t  mod_freq;
//extern uint32_t  dev_freq;
//extern uint64_t  reg_set;
//extern uint64_t  reg_span;
//extern uint64_t  reg_step;

extern uint8_t ddr_rank_num;
extern uint8_t ddr_sys_num;
extern uint8_t board_ddr_type;

extern u8  uWdq_Trained;
extern u16 tx_winsize_min, rx_winsize_min;
extern u8  tx_train_cnt, rx_train_cnt;
extern u32 ca_winsize_min;
extern u16 ca_train_cnt, ca_vref_min, ca_vref_max;

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

//enum ddr_type_e {
//	LP4,
//	LP4_2CH_1PHYD,
//	DDR4_X8,
//	DDR4_X16,
//};

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

enum rank_num_e {
	SINGLE_RANK,
	DUAL_RANK,
};

enum ddrsys_num_e {
	SINGLE_SYS,
	DUAL_SYS,
};

enum board_ddr_type_e {
	BOARD_DDR4_X8,
	BOARD_DDR4_X16,
	BOARD_LPDDR4,
	BOARD_LPDDR4X,
};

/********************DDR Feature List***********************/
/*ddr common feature define*/
//#define DDR_EXT_LOOP_BACK_TEST
//#define DBG_BRING_UP
//#define FULL_MEM_BIST
//#define FULL_MEM_BIST_FOREVER

//#define DDR_CLK_SSC_EN
//#define SHOW_DDR_INIT_TIME

//#define DBG_INFO_L1
//#define DBG_INFO_L2
//#define DBG_INFO_L3

//debug log define
//#define DBG_SHMOO_TX_LEGACY
//#define DBG_SHMOO_RX_LEGACY
//#define DDR_SHMOO_LOG_RX
//#define DDR_SHMOO_LOG_TX

#define MASK_SHIFT_FW_1T
//#define AUTO_GATED
//#define HALF_FREQ

#define DDR_OENZ_ALWAY_LOW
#define DDR_CLOSE_SYS_AP
//#define DDR_ODTEN_EXTO_24UI
#define DDR_RDCLK_ALWAY_ON
//#define ODTEN_ENLARGE_4UI

//#define DDR_TRAIN_FIX_VREF //when shmoo need close
#ifdef DDR_TRAIN_FIX_VREF
#define DDR_TRAIN_VREF_VAL	(0x21)// fixed vref to training
#endif

/*DDR4 feature define*/
#if (defined(DDR4_X8) || defined(DDR4_X16) || defined(DDR_AUTO))
#define DDR4_OENZ_DQS_SHIFT2
#define DDR4_ODT_EN_EXTEND2
#define DDR4_NO_DODT

//#define DDR4_CLOSE_DDRC_OPT//disable ddr4 opt function
#define DDR4_CONFIG_CTLE_HIGH //3200,3733
//#define DDR4_CONFIG_CTLE_LOW  //1600,2400
//#define DDR4_CONFIG_ODT48
//#define DDR4_CONFIG_ODT60
//#define DDR4_CONFIG_ODT34
#define DDR4_CONFIG_ODT40

//#define DDR4_CLOSE_SYS_DBI
//#define DDR4_ADDRMAP_N

#define DDR4_USE_LP4RXCFG

//#define DDR4_ADD_FIX_VREF
//#define DDR4_RX_VREF_OFFSET (-11)// fixed offset add when trained

/*shmoo should eanble DBI & disable fix vref */
#if defined(DDR_SHMOO_LOG_RX) || defined(DDR_SHMOO_LOG_TX)
//#undef DDR4_CLOSE_SYS_DBI
#undef DDR_TRAIN_FIX_VREF
#undef DDR4_ADD_FIX_VREF
#endif

#endif//defined(DDR4_X8) || defined(DDR4_X16)

/*LPDDR4/4x feature define*/
#if (defined(LPDDR4) || defined(LPDDR4X) || defined(DDR_AUTO))
#define LP4_4X_ODT_EN_EXTEND0

//#define DBG_SHMOO_CA

//#define DDR_SHMOO_LOG_RX_LP4
//#define DDR_SHMOO_LOG_TX_LP4

#define CSLVL_CALVL  //Cal CA and CS both in CA Leveling

//#define LP4_ADD_FIX_VREF
//#define LP4_RX_VREF_OFFSET (0)
#endif

#ifdef CADENCE_DDRC
	#define DDR_PI_BASE_ADDR 0x2000
	#define DDR_PHY_BASE_ADDR 0x4000
	#define DDR_DFS_BASE_ADDR 0x7000
	#define DENALI_CTL_REG(reg_num) ((reg_num)*4)
	#define DENALI_PI_REG(reg_num)  (((reg_num)*4) + DDR_PI_BASE_ADDR)
	#define DENALI_PHY_REG(reg_num) (((reg_num)*4) + DDR_PHY_BASE_ADDR)
	#define DENALI_DFS_REG(reg_num) (((reg_num)*4) + DDR_DFS_BASE_ADDR)
#else
	#define DDR_DRAM_BASE 			0x100000000
	#define DDR_DRAM_END  			0x4ffffffff
	#define DDR_TOP_CFG_BASE 		0x6fff0000
	#define DDR_TOP_CFG_END 		0x6fff3fff

	#define DDR_CFG_SYS1_BASE		0x70000000
	#define DDR_CFG_SYS1_END		0x77ffffff
	#define DDR_PHYD2_CH0_SYS1_BASE 0x70000000
	//#define DDR_PHYD2_CH0_SYS1_END	0x70003fff
	#define DDR_PHYD_CH0_SYS1_BASE	0x70006000
	//#define DDR_PHYD_CH0_SYS1_END   0x70007fff
	#define DDR_CTRL_SYS1_BASE		0x70004000
	//#define DDR_CTRL_SYS1_END		0x70005fff
	#define	DDR_AXI_MON_SYS1_BASE	0x70008000
	#define DDR_AXI_MON_SYS1_END	0x70009fff
	#define DDR_TOP_SYS1_BASE		0x7000a000
	//#define DDR_TOP_SYS1_END		0x7000ffff
	#define DDR_BIST_CH0_SYS1_BASE  0x70010000
	//#define DDR_BIST_CH0_SYS1_END	0x70011fff
	#define DDR_BIST_CH1_SYS1_BASE  0x70030000
	//#define DDR_BIST_CH1_SYS1_END	0x70031fff

	#define DDR_CFG_SYS2_BASE		0x78000000
	#define DDR_CFG_SYS2_END		0x7fffffff
	#define DDR_PHYD2_CH0_SYS2_BASE 0x78000000
	//#define DDR_PHYD2_CH0_SYS2_END	0x78003fff
	#define DDR_PHYD_CH0_SYS2_BASE	0x78006000
	//#define DDR_PHYD_CH0_SYS2_END   0x78007fff
	#define DDR_CTRL_SYS2_BASE		0x78004000
	//#define DDR_CTRL_SYS2_END		0x78005fff
	#define	DDR_AXI_MON_SYS2_BASE	0x78008000
	#define DDR_AXI_MON_SYS2_END	0x78009fff
	#define DDR_TOP_SYS2_BASE		0x7800a000
	//#define DDR_TOP_SYS2_END		0x7800ffff
	#define DDR_BIST_CH0_SYS2_BASE  0x78010000
	//#define DDR_BIST_CH0_SYS2_END	0x78011fff
	#define DDR_BIST_CH1_SYS2_BASE  0x78030000
	//#define DDR_BIST_CH1_SYS2_END	0x78031fff
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

enum ddr_pinmux_type_enum { 
	DDR_DEFAULT,
	DDR3_1G,
	DDR3_2G,
	DDR3_4G,
	DDR4_PINMUX,
	DDR_DBG,
	LP4_PINMUX,
	N25_DDR2_512,
	DDR2_PINMUX
};
enum ddr_type_enum { 
	DDR2_0P5G,
	DDR2,
	DDR3,
	DDR4,
	DDR4_2PHYD,
	LP4,
	DDR3_MICRON,
	LP4_MICRON,
	LP4X,
	LP4_2CH_2PHYD,
	LP4_2CH_1PHYD,
	LP4_4CH_2PHYD
};
enum ddr_ssc_enum { 
	SSC_EN,
	SSC_BYPASS,
	SSC_OFF
};
enum ddr_mode_enum {
	X8_MODE,
	X16_MODE,
	X32_MODE,
	X64_MODE
};
enum ddr_freq_enum { 
	_mem_freq_3200,
	_mem_freq_2400,
	_mem_freq_2133,
	_mem_freq_1866,
	_mem_freq_1600,
	_mem_freq_1333,
	_mem_freq_1066,
	_mem_freq_2666,
	_mem_freq_2800,
	_mem_freq_4266,
	_mem_freq_4000,
	_mem_freq_4200,
	_mem_freq_4300,
	_mem_freq_3733 
};
typedef struct {
	uint8_t channel_number;
	enum ddr_type_enum ddr_type;
	enum ddr_pinmux_type_enum ddr_pinmux_type;
	enum ddr_ssc_enum  ddr_ssc_type;
	enum ddr_mode_enum ddr_mode;
	enum ddr_freq_enum ddr_freq_mode;
	uint64_t ddr_top_base;
	uint64_t phy_base_addr;
	uint64_t ddr_ctrl;
	uint64_t phyd_base_addr;
	uint64_t cv_ddr_phyd_apb;

	uint64_t ddr_bist_base;
	uint64_t ddr_bist_sram_dq_base;
	uint64_t ddr_bist_sram_dm_base;

	uint8_t wdqlvl_sw_mode_en;
	uint8_t wrlvl_sw_mode_en;
} ddr_define_struct;

extern ddr_define_struct ddr_struct_h;

#define CEILING_POS(X) ((X - (int)(X)) > 0 ? (int)(X + 1) : (int)(X))
#define CEILING_NEG(X) ((X - (int)(X)) < 0 ? (int)(X - 1) : (int)(X))
#define ceil(X) (((X) > 0) ? CEILING_POS(X) : CEILING_NEG(X))

#define mmio_wr32	mmio_write_32
#define mmio_rd32	mmio_read_32

// #define ddr_mmio_rd32(a, b)	do { if (1) b = mmio_rd32(a); } while (0)
// #define ddr_sram_rd32(a, b)	do { if (1) b = mmio_rd32(a); } while (0)

#define ddr_sram_wr32(a, b)	mmio_wr32(a, b)
#define ddr_debug_wr32(b)



#ifdef DBG_INFO_L1
#define DBG1(...)		tf_printf(__VA_ARGS__)
#else
#define DBG1(...)
#endif

#ifdef DBG_INFO_L2
#define ZQ_MSG(...)		tf_printf("[ZQ]: " __VA_ARGS__)
#define DLL_MSG(...)	tf_printf("[DLL]: " __VA_ARGS__)
#define RX_MSG(...)		tf_printf("[RX]: " __VA_ARGS__)
#define TX_MSG(...)		tf_printf("[TX]: " __VA_ARGS__)
#define RDG_MSG(...)	tf_printf("[RDG]: " __VA_ARGS__)
#define WR_MSG(...)		tf_printf("[WR]: " __VA_ARGS__)
#else
#define ZQ_MSG(...)
#define DLL_MSG(...)
#define RX_MSG(...)
#define TX_MSG(...)
#define RDG_MSG(...)
#define WR_MSG(...)
#endif

#ifdef DBG_INFO_L3
#define uartlog(...)	tf_printf("U: " __VA_ARGS__)
#define KC_MSG(...)		tf_printf("[KC_DBG] " __VA_ARGS__)
#define KC_MSG_TR(...)	tf_printf("[KC_DBG_training]" __VA_ARGS__)
#define TJ_MSG(...)		tf_printf("[TJ Info] : " __VA_ARGS__)
#define DBG_BRI(...)	tf_printf(__VA_ARGS__)
#else
#define uartlog(...)
#define KC_MSG(...)
#define KC_MSG_TR(...)
#define TJ_MSG(...)
#define DBG_BRI(...)
#endif


#ifdef DBG_SHMOO
#define SHMOO_MSG(...)	tf_printf(__VA_ARGS__)
#else
#define SHMOO_MSG(...)
#endif

#ifdef DBG_SHMOO_TX
#define SHMOO_MSG_TX(...) tf_printf("[TX_SHMOO] : " __VA_ARGS__)
#else
#define SHMOO_MSG_TX(...)
#endif

#ifdef DBG_SHMOO_RX
#define SHMOO_MSG_RX(...) tf_printf("[RX_SHMOO] : " __VA_ARGS__)
#else
#define SHMOO_MSG_RX(...)
#endif

#ifdef DBG_SHMOO_CA
#define SHMOO_MSG_CA(...)	tf_printf("[CA_SHMOO] : " __VA_ARGS__)
#else
#define SHMOO_MSG_CA(...)
#endif

#ifdef DBG_SHMOO_CS
#define SHMOO_MSG_CS(...)	tf_printf(__VA_ARGS__)
#else
#define SHMOO_MSG_CS(...)
#endif

extern uint64_t    DDR_TOP_BASE;
extern uint64_t    PHY_BASE_ADDR;
extern uint64_t    DDR_CTRL;
extern uint64_t    PHYD_BASE_ADDR;
extern uint64_t    CV_DDR_PHYD_APB;

extern uint64_t		DDR_BIST_BASE;
extern uint64_t		DDR_BIST_SRAM_DQ_BASE;
extern uint64_t		DDR_BIST_SRAM_DM_BASE;

extern uint32_t    REG_DDRPLL_MAS_STEP;
extern uint8_t     REG_DDRPLL_MAS_ICTRL;
extern uint64_t    REG_DDRPLL_MAS_SET;
extern uint32_t    REG_DDRPLL_MAS_SET33;
extern uint32_t    REG_DDRPLL_MAS_SET32_0;
extern uint8_t     REG_TX_MPLL_ICTRL;   //[7:4]
extern uint8_t     REG_TX_MPLL_DIV_SEL; //[14:8]
extern uint8_t     REG_TX_MPLL_POST_DIV;
extern uint8_t     REG_TX_MPLL_POST_DIV_SEL; //[21:15]
extern uint16_t    REG_DDRPLL_MAS_SPAN; //[15:0]
extern uint8_t     REG_DDRPLL_MAS_SW_UP; //[0:0]
extern uint8_t     REG_DDRPLL_MAS_SYN_MODE; //[1:1]
extern uint8_t     REG_DDRPLL_MAS_SYN_RST; //[2:2]
extern uint8_t     REG_TX_MPLL_EN_LCKDET; //22:22
extern uint8_t     REG_VPROT_H_LEVEL_SEL;//25:23

extern uint32_t    REG_DDRPLL_MAS_N;
extern uint32_t    REG_DDRPLL_MAS_F;
extern uint32_t    REG_DDRPLL_MAS_DIV2N;
extern uint32_t    REG_DDRPLL_MAS_KPD;
extern uint32_t    DDR_SSO_PERIOD;  // mem_freq/200

void ddr_define_struct_init(uint8_t sys_id);
void ddr_global_para_init(void);
void cvx32_top_fab_reg(uint8_t sys_id);
void cvx32_pll_init(void);
void cvx32_ctrl_init(uintptr_t base_addr_ctrl);
void cvx32_phy_init(uintptr_t base_addr_phyd);
void cvx32_setting_check(void);
void cvx32_bist_wr_prbs_init(uint32_t rank);
void cvx32_bist_rd_only(uint32_t rank);
void cvx32_bist_rd_only_prbs_init(uint32_t rank);
void cvx32_bist_wr_sram_init(uint32_t rank);
void cvx32_bist_wrlvl_init(void);
void cvx32_bist_rdglvl_init(void);
void cvx32_bist_rdlvl_init(uint32_t mode, uint32_t sso_period, uint32_t rank);
void cvx32_bist_wdqlvl_init(uint32_t mode, uint32_t sso_period, uint32_t rank);
void cvx32_bist_wdmlvl_init(uint32_t mode, uint32_t sso_period, uint32_t rank);
void cvx32_bist_start_check(uint32_t *bist_result, uint64_t *err_data_odd, uint64_t *err_data_even);
void cvx32_bist_tx_shift_delay(uint32_t shift_delay);
void cvx32_bist_rx_delay(uint32_t delay);
void cvx32_bist_rx_deskew_delay(uint32_t delay);
void cvx32_synp_mrw(uint32_t addr, uint32_t data, uint8_t mpr_en);
void cvx32_synp_mrr(uint32_t addr, uint8_t mpr_en);
void cvx32_synp_mrr_lp4(uint32_t addr, uint32_t rank);
void cvx32_synp_mrw_lp4 (uint32_t addr, uint32_t data, uint32_t rank);
void cvx32_dfi_sw_mrw(uint32_t addr, uint32_t data, uint32_t rank, uint32_t channel_sel);
void cvx32_dfi_sw_cmd(uint32_t cmd_word, uint32_t cmd_addr_data);
void cvx32_ddrck_ctrl(uint32_t stop);
void cvx32_chg_pll_freq(void);
void cvx32_dll_cal_phyd_hw(void);
void cvx32_dll_cal(void);
void cvx32_ddr_zqcal(uint8_t ratio_down, uint8_t ratio_up, uint8_t dram_class);
void cvx32_clk_normal(void);
void cvx32_clk_div2(void);
void cvx32_INT_ISR_08(void);
void cvx32_clk_div40(void);
void cvx32_ddr_phy_power_on_seq1(void);
void cvx32_ddr_phy_power_on_seq2(void);
void cvx32_ddr_phy_power_on_seq3(void);
void cvx32_wait_for_dfi_init_complete(void);
void cvx32_ctrlupd_short(void);
void cvx32_polling_dfi_init_start(void);
void cvx32_set_dfi_init_complete(void);
void cvx32_polling_synp_normal_mode(void);
void cvx32_wrlvl_req(uint32_t rank);
void cvx32_rdglvl_req(uint32_t rank);
void cvx32_rdlvl_req(uint32_t mode, uint32_t sso_period, uint32_t rank, uint8_t vref_start, uint8_t vref_end, uint8_t vref_step);
void cvx32_wdqlvl_req(uint8_t vref_start, uint8_t vref_end, uint8_t vref_step, uint32_t data_mode, uint32_t lvl_mode, uint32_t sso_period, uint32_t rank);
void cvx32_wrlvl_status(void);
void cvx32_rdglvl_status(void);
void cvx32_rdlvl_status(void);
void cvx32_wdqlvl_status(void);
void cvx32_zqcal_status(void);
void cvx32_dll_cal_status(void);
void cvx32_set_dfi_init_start(void);
void cvx32_dll_sw_upd(void);
void cvx32_bist_mask_shift_delay(uint32_t shift_delay, uint32_t  en_lead);
void cvx32_lb_0_external(void);
void cvx32_clk_gating_disable(void);
void cvx32_clk_gating_enable(void);
void cvx32_pinmux(void);
void cvx32_pinmux_sys2(void);
void cvx32_dfi_phyupd_req(void);
void cvx32_dfi_phyupd_req_clr(void);
void cvx32_dfi_phymstr_req(void);
void cvx32_dfi_phymstr_req_clr(void);
void cvx32_dll_sw_clr(void);
void ctrl_init_detect_dram_size(uint32_t *cap_in_mbyte);
void cvx32_change_to_calvl_freq(void);
void cvx32_ddr_phy_auto_mask(void);

void cvx32_calvl_base_loop(uint8_t cslvl_mode, uint8_t vref, uint16_t cur_cs_center_code_cha, uint16_t cur_cs_center_code_chb,
                            uint16_t param_ca_dline_start, uint16_t param_ca_dline_end,
                            uint32_t MR13, uint32_t rank, uint8_t param_phyd_dfi_calvl_vref_start);
void cvx32_calvl_req_base_sw(uint8_t rank_sel, uint32_t rank, uint8_t param_phyd_dfi_calvl_vref_start, uint8_t param_phyd_dfi_calvl_vref_end,
                            uint8_t param_phyd_dfi_calvl_vref_step, uint8_t param_phyd_calvl_dly_inc_step,
                            uint16_t better_ca_cs_min_pass_code_cha[7], uint16_t better_ca_cs_max_pass_code_cha[7], uint8_t * better_vrf_cha,
                            uint16_t better_ca_cs_min_pass_code_chb[7], uint16_t better_ca_cs_max_pass_code_chb[7], uint8_t * better_vrf_chb);
void cvx32_calvl_req_loop_sw (uint32_t rank, uint8_t param_phyd_dfi_calvl_vref_start, uint8_t param_phyd_dfi_calvl_vref_end,
                                            uint8_t param_phyd_dfi_calvl_vref_step, uint8_t param_phyd_calvl_dly_inc_step);
void cvx32_dll_sw_clr_calvl();
void cvx32_cs_calvl_req_sw (uint8_t channel_sel, uint32_t vref, uint16_t better_ca_cs_min_pass_code_cha[7], uint16_t better_ca_cs_max_pass_code_cha[7],
                                uint16_t better_ca_cs_min_pass_code_chb[7], uint16_t better_ca_cs_max_pass_code_chb[7],
                                uint16_t * cs0_window, uint16_t * cs1_window, uint32_t MR13, uint32_t rank, uint8_t param_phyd_dfi_calvl_vref_start);
void cvx32_rdglvl_req_rank2(void);
void cvx32_rdglvl_req_rank2_auto_mask(void);
void cvx32_chg_pll_freq_div2(uint8_t div2_en);
void cvx32_rdlvl_req_lp4_2rank(uint32_t mode, uint32_t sso_period, uint8_t vref_start, uint8_t vref_end, uint8_t vref_step);
void cvx32_rdlvl_req_shmoo(uint32_t mode, uint32_t sso_period, uint32_t rank, uint8_t vref_start, uint8_t vref_end, uint8_t vref_step);
void cvx32_wdqlvl_req_shmoo(uint8_t vref_start, uint8_t vref_end, uint8_t vref_step, uint32_t data_mode, uint32_t lvl_mode, uint32_t sso_period, uint32_t rank);
//void ddr_bist_all(uint32_t capacity, uint32_t x16_mode);
void cvx32_rdglvl_training_check(void);
void cvx32_rdlvl_sw_patch(int rank);//for corner case
void ddr_phya_multi_rank(void);
void disable_low_power_function(void);
void ddr_config_ctle_rsel_csel_hs(uint8_t mode);
void ddr4_customed_setting(void);
void cvx32_wrlvl_req_sw(uint32_t rank);
void calvl_pre_set(void);
void cvx32_wdqlvl_req_sw1d(uint32_t mode, uint32_t sso_period, uint32_t rank, uint32_t en_vref_sel, uint32_t vref,
			   uint32_t dq_lvl_mode, uint32_t dm_lvl_mode, uint32_t dm_as_dbi);
void cvx32_wdqlvl_req_sw2d(uint32_t mode, uint32_t sso_period, uint32_t rank, uint32_t vref_start, uint32_t vref_end,
			   uint32_t vref_step, uint32_t dm_lvl_mode, uint32_t dm_as_dbi);
void cvx32_le_te_detect_tx(void);
//void cvx32_bist_wr_only_infinite_prbs_init_pulse(uint32_t rank);
#ifdef DDR_ODTEN_EXTO_24UI
void rdodten_rxmask_backward(void);
#endif
void cvx32_bist_write_read_init(int rank, int sso_period, int pattern_sel);
void ddr_ca_ctrl_upd_disable(void);
#ifdef DDR_EXT_LOOP_BACK_TEST
void DDR_EXT_LOOP_BACK_TEST(void);
#endif
#ifdef FULL_MEM_BIST
uint32_t bist_full_mem(uint64_t uSys0_cap_in_mbyte, uint64_t uSys1_cap_in_mbyte);
#endif
#ifdef FULL_MEM_BIST_FOREVER
void bist_full_mem_forever(uint64_t uSys0_cap_in_mbyte, uint64_t uSys1_cap_in_mbyte);
#endif
#ifdef DDR_SHMOO_LOG_RX_LP4
void cvx32_rdlvl_req_shmoo_real_LP4(uint8_t range, int mode, int sso_period, int rank, int vref_start, int vref_end, int vref_step, int dcode_start, int dcode_end, int dcode_step);
#endif
#ifdef DDR_SHMOO_LOG_TX_LP4
void cvx32_wdqlvl_req_shmoo_real_LP4(int mode, int sso_period, int rank, int vref_start, int vref_end, int vref_step,int  dline_start, int  dline_end, int dline_step, int en_dmdbi);
#endif
#endif /* __DDR_SYS_H__ */
