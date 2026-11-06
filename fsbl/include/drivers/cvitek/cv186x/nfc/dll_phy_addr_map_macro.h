 /*
 * Copyright (c) 2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __REG_DLL_PHY_ADDR_MAP_MACRO_H__
#define __REG_DLL_PHY_ADDR_MAP_MACRO_H__


/* macros for BlueprintGlobalNameSpace::dataslice_Rfile::dq_timing_reg */
#ifndef __DATASLICE_RFILE__DQ_TIMING_REG_MACRO__
#define __DATASLICE_RFILE__DQ_TIMING_REG_MACRO__

/* macros for field data_select_oe_end */
#define DATASLICE_RFILE__DQ_TIMING_REG__DATA_SELECT_OE_END__SHIFT             0
#define DATASLICE_RFILE__DQ_TIMING_REG__DATA_SELECT_OE_END__WIDTH             3
#define DATASLICE_RFILE__DQ_TIMING_REG__DATA_SELECT_OE_END__MASK    0x00000007U
#define DATASLICE_RFILE__DQ_TIMING_REG__DATA_SELECT_OE_END__READ(src) \
					((uint32_t)(src)\
					& 0x00000007U)
#define DATASLICE_RFILE__DQ_TIMING_REG__DATA_SELECT_OE_END__WRITE(src) \
					((uint32_t)(src)\
					& 0x00000007U)
#define DATASLICE_RFILE__DQ_TIMING_REG__DATA_SELECT_OE_END__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00000007U) | ((uint32_t)(src) &\
					0x00000007U)
#define DATASLICE_RFILE__DQ_TIMING_REG__DATA_SELECT_OE_END__VERIFY(src) \
					(!(((uint32_t)(src)\
					& ~0x00000007U)))

/* macros for field data_select_oe_start */
#define DATASLICE_RFILE__DQ_TIMING_REG__DATA_SELECT_OE_START__SHIFT           4
#define DATASLICE_RFILE__DQ_TIMING_REG__DATA_SELECT_OE_START__WIDTH           3
#define DATASLICE_RFILE__DQ_TIMING_REG__DATA_SELECT_OE_START__MASK  0x00000070U
#define DATASLICE_RFILE__DQ_TIMING_REG__DATA_SELECT_OE_START__READ(src) \
					(((uint32_t)(src)\
					& 0x00000070U) >> 4)
#define DATASLICE_RFILE__DQ_TIMING_REG__DATA_SELECT_OE_START__WRITE(src) \
					(((uint32_t)(src)\
					<< 4) & 0x00000070U)
#define DATASLICE_RFILE__DQ_TIMING_REG__DATA_SELECT_OE_START__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00000070U) | (((uint32_t)(src) <<\
					4) & 0x00000070U)
#define DATASLICE_RFILE__DQ_TIMING_REG__DATA_SELECT_OE_START__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 4) & ~0x00000070U)))

/* macros for field data_select_tsel_end */
#define DATASLICE_RFILE__DQ_TIMING_REG__DATA_SELECT_TSEL_END__SHIFT           8
#define DATASLICE_RFILE__DQ_TIMING_REG__DATA_SELECT_TSEL_END__WIDTH           4
#define DATASLICE_RFILE__DQ_TIMING_REG__DATA_SELECT_TSEL_END__MASK  0x00000f00U
#define DATASLICE_RFILE__DQ_TIMING_REG__DATA_SELECT_TSEL_END__READ(src) \
					(((uint32_t)(src)\
					& 0x00000f00U) >> 8)
#define DATASLICE_RFILE__DQ_TIMING_REG__DATA_SELECT_TSEL_END__WRITE(src) \
					(((uint32_t)(src)\
					<< 8) & 0x00000f00U)
#define DATASLICE_RFILE__DQ_TIMING_REG__DATA_SELECT_TSEL_END__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00000f00U) | (((uint32_t)(src) <<\
					8) & 0x00000f00U)
#define DATASLICE_RFILE__DQ_TIMING_REG__DATA_SELECT_TSEL_END__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 8) & ~0x00000f00U)))

/* macros for field data_select_tsel_start */
#define DATASLICE_RFILE__DQ_TIMING_REG__DATA_SELECT_TSEL_START__SHIFT        12
#define DATASLICE_RFILE__DQ_TIMING_REG__DATA_SELECT_TSEL_START__WIDTH         4
#define DATASLICE_RFILE__DQ_TIMING_REG__DATA_SELECT_TSEL_START__MASK \
					0x0000f000U
#define DATASLICE_RFILE__DQ_TIMING_REG__DATA_SELECT_TSEL_START__READ(src) \
					(((uint32_t)(src)\
					& 0x0000f000U) >> 12)
#define DATASLICE_RFILE__DQ_TIMING_REG__DATA_SELECT_TSEL_START__WRITE(src) \
					(((uint32_t)(src)\
					<< 12) & 0x0000f000U)
#define DATASLICE_RFILE__DQ_TIMING_REG__DATA_SELECT_TSEL_START__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x0000f000U) | (((uint32_t)(src) <<\
					12) & 0x0000f000U)
#define DATASLICE_RFILE__DQ_TIMING_REG__DATA_SELECT_TSEL_START__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 12) & ~0x0000f000U)))
#define DATASLICE_RFILE__DQ_TIMING_REG__TYPE                           uint32_t
#define DATASLICE_RFILE__DQ_TIMING_REG__READ                        0x0000ff77U
#define DATASLICE_RFILE__DQ_TIMING_REG__WRITE                       0x0000ff77U

#endif /* __DATASLICE_RFILE__DQ_TIMING_REG_MACRO__ */


/* macros for phy_dataslice.phy_dq_timing_reg */
#define INST_PHY_DATASLICE__PHY_DQ_TIMING_REG__NUM                            1

/* macros for BlueprintGlobalNameSpace::dataslice_Rfile::dqs_timing_reg */
#ifndef __DATASLICE_RFILE__DQS_TIMING_REG_MACRO__
#define __DATASLICE_RFILE__DQS_TIMING_REG_MACRO__

/* macros for field dqs_select_oe_end */
#define DATASLICE_RFILE__DQS_TIMING_REG__DQS_SELECT_OE_END__SHIFT             0
#define DATASLICE_RFILE__DQS_TIMING_REG__DQS_SELECT_OE_END__WIDTH             4
#define DATASLICE_RFILE__DQS_TIMING_REG__DQS_SELECT_OE_END__MASK    0x0000000fU
#define DATASLICE_RFILE__DQS_TIMING_REG__DQS_SELECT_OE_END__READ(src) \
					((uint32_t)(src)\
					& 0x0000000fU)
#define DATASLICE_RFILE__DQS_TIMING_REG__DQS_SELECT_OE_END__WRITE(src) \
					((uint32_t)(src)\
					& 0x0000000fU)
#define DATASLICE_RFILE__DQS_TIMING_REG__DQS_SELECT_OE_END__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x0000000fU) | ((uint32_t)(src) &\
					0x0000000fU)
#define DATASLICE_RFILE__DQS_TIMING_REG__DQS_SELECT_OE_END__VERIFY(src) \
					(!(((uint32_t)(src)\
					& ~0x0000000fU)))

/* macros for field dqs_select_oe_start */
#define DATASLICE_RFILE__DQS_TIMING_REG__DQS_SELECT_OE_START__SHIFT           4
#define DATASLICE_RFILE__DQS_TIMING_REG__DQS_SELECT_OE_START__WIDTH           4
#define DATASLICE_RFILE__DQS_TIMING_REG__DQS_SELECT_OE_START__MASK  0x000000f0U
#define DATASLICE_RFILE__DQS_TIMING_REG__DQS_SELECT_OE_START__READ(src) \
					(((uint32_t)(src)\
					& 0x000000f0U) >> 4)
#define DATASLICE_RFILE__DQS_TIMING_REG__DQS_SELECT_OE_START__WRITE(src) \
					(((uint32_t)(src)\
					<< 4) & 0x000000f0U)
#define DATASLICE_RFILE__DQS_TIMING_REG__DQS_SELECT_OE_START__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x000000f0U) | (((uint32_t)(src) <<\
					4) & 0x000000f0U)
#define DATASLICE_RFILE__DQS_TIMING_REG__DQS_SELECT_OE_START__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 4) & ~0x000000f0U)))

/* macros for field dqs_select_tsel_end */
#define DATASLICE_RFILE__DQS_TIMING_REG__DQS_SELECT_TSEL_END__SHIFT           8
#define DATASLICE_RFILE__DQS_TIMING_REG__DQS_SELECT_TSEL_END__WIDTH           4
#define DATASLICE_RFILE__DQS_TIMING_REG__DQS_SELECT_TSEL_END__MASK  0x00000f00U
#define DATASLICE_RFILE__DQS_TIMING_REG__DQS_SELECT_TSEL_END__READ(src) \
					(((uint32_t)(src)\
					& 0x00000f00U) >> 8)
#define DATASLICE_RFILE__DQS_TIMING_REG__DQS_SELECT_TSEL_END__WRITE(src) \
					(((uint32_t)(src)\
					<< 8) & 0x00000f00U)
#define DATASLICE_RFILE__DQS_TIMING_REG__DQS_SELECT_TSEL_END__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00000f00U) | (((uint32_t)(src) <<\
					8) & 0x00000f00U)
#define DATASLICE_RFILE__DQS_TIMING_REG__DQS_SELECT_TSEL_END__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 8) & ~0x00000f00U)))

/* macros for field dqs_select_tsel_start */
#define DATASLICE_RFILE__DQS_TIMING_REG__DQS_SELECT_TSEL_START__SHIFT        12
#define DATASLICE_RFILE__DQS_TIMING_REG__DQS_SELECT_TSEL_START__WIDTH         4
#define DATASLICE_RFILE__DQS_TIMING_REG__DQS_SELECT_TSEL_START__MASK \
					0x0000f000U
#define DATASLICE_RFILE__DQS_TIMING_REG__DQS_SELECT_TSEL_START__READ(src) \
					(((uint32_t)(src)\
					& 0x0000f000U) >> 12)
#define DATASLICE_RFILE__DQS_TIMING_REG__DQS_SELECT_TSEL_START__WRITE(src) \
					(((uint32_t)(src)\
					<< 12) & 0x0000f000U)
#define DATASLICE_RFILE__DQS_TIMING_REG__DQS_SELECT_TSEL_START__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x0000f000U) | (((uint32_t)(src) <<\
					12) & 0x0000f000U)
#define DATASLICE_RFILE__DQS_TIMING_REG__DQS_SELECT_TSEL_START__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 12) & ~0x0000f000U)))

/* macros for field phony_dqs_sel */
#define DATASLICE_RFILE__DQS_TIMING_REG__PHONY_DQS_SEL__SHIFT                16
#define DATASLICE_RFILE__DQS_TIMING_REG__PHONY_DQS_SEL__WIDTH                 1
#define DATASLICE_RFILE__DQS_TIMING_REG__PHONY_DQS_SEL__MASK        0x00010000U
#define DATASLICE_RFILE__DQS_TIMING_REG__PHONY_DQS_SEL__READ(src) \
					(((uint32_t)(src)\
					& 0x00010000U) >> 16)
#define DATASLICE_RFILE__DQS_TIMING_REG__PHONY_DQS_SEL__WRITE(src) \
					(((uint32_t)(src)\
					<< 16) & 0x00010000U)
#define DATASLICE_RFILE__DQS_TIMING_REG__PHONY_DQS_SEL__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00010000U) | (((uint32_t)(src) <<\
					16) & 0x00010000U)
#define DATASLICE_RFILE__DQS_TIMING_REG__PHONY_DQS_SEL__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 16) & ~0x00010000U)))
#define DATASLICE_RFILE__DQS_TIMING_REG__PHONY_DQS_SEL__SET(dst) \
					(dst) = ((dst) &\
					~0x00010000U) | ((uint32_t)(1) << 16)
#define DATASLICE_RFILE__DQS_TIMING_REG__PHONY_DQS_SEL__CLR(dst) \
					(dst) = ((dst) &\
					~0x00010000U) | ((uint32_t)(0) << 16)

/* macros for field use_phony_dqs */
#define DATASLICE_RFILE__DQS_TIMING_REG__USE_PHONY_DQS__SHIFT                20
#define DATASLICE_RFILE__DQS_TIMING_REG__USE_PHONY_DQS__WIDTH                 1
#define DATASLICE_RFILE__DQS_TIMING_REG__USE_PHONY_DQS__MASK        0x00100000U
#define DATASLICE_RFILE__DQS_TIMING_REG__USE_PHONY_DQS__READ(src) \
					(((uint32_t)(src)\
					& 0x00100000U) >> 20)
#define DATASLICE_RFILE__DQS_TIMING_REG__USE_PHONY_DQS__WRITE(src) \
					(((uint32_t)(src)\
					<< 20) & 0x00100000U)
#define DATASLICE_RFILE__DQS_TIMING_REG__USE_PHONY_DQS__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00100000U) | (((uint32_t)(src) <<\
					20) & 0x00100000U)
#define DATASLICE_RFILE__DQS_TIMING_REG__USE_PHONY_DQS__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 20) & ~0x00100000U)))
#define DATASLICE_RFILE__DQS_TIMING_REG__USE_PHONY_DQS__SET(dst) \
					(dst) = ((dst) &\
					~0x00100000U) | ((uint32_t)(1) << 20)
#define DATASLICE_RFILE__DQS_TIMING_REG__USE_PHONY_DQS__CLR(dst) \
					(dst) = ((dst) &\
					~0x00100000U) | ((uint32_t)(0) << 20)
#define DATASLICE_RFILE__DQS_TIMING_REG__TYPE                          uint32_t
#define DATASLICE_RFILE__DQS_TIMING_REG__READ                       0x0011ffffU
#define DATASLICE_RFILE__DQS_TIMING_REG__WRITE                      0x0011ffffU

#endif /* __DATASLICE_RFILE__DQS_TIMING_REG_MACRO__ */


/* macros for phy_dataslice.phy_dqs_timing_reg */
#define INST_PHY_DATASLICE__PHY_DQS_TIMING_REG__NUM                           1

/* macros for BlueprintGlobalNameSpace::dataslice_Rfile::gate_lpbk_ctrl_reg */
#ifndef __DATASLICE_RFILE__GATE_LPBK_CTRL_REG_MACRO__
#define __DATASLICE_RFILE__GATE_LPBK_CTRL_REG_MACRO__

/* macros for field gate_cfg */
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__GATE_CFG__SHIFT                  0
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__GATE_CFG__WIDTH                  4
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__GATE_CFG__MASK         0x0000000fU
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__GATE_CFG__READ(src) \
					((uint32_t)(src)\
					& 0x0000000fU)
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__GATE_CFG__WRITE(src) \
					((uint32_t)(src)\
					& 0x0000000fU)
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__GATE_CFG__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x0000000fU) | ((uint32_t)(src) &\
					0x0000000fU)
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__GATE_CFG__VERIFY(src) \
					(!(((uint32_t)(src)\
					& ~0x0000000fU)))

/* macros for field gate_cfg_close */
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__GATE_CFG_CLOSE__SHIFT            4
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__GATE_CFG_CLOSE__WIDTH            2
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__GATE_CFG_CLOSE__MASK   0x00000030U
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__GATE_CFG_CLOSE__READ(src) \
					(((uint32_t)(src)\
					& 0x00000030U) >> 4)
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__GATE_CFG_CLOSE__WRITE(src) \
					(((uint32_t)(src)\
					<< 4) & 0x00000030U)
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__GATE_CFG_CLOSE__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00000030U) | (((uint32_t)(src) <<\
					4) & 0x00000030U)
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__GATE_CFG_CLOSE__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 4) & ~0x00000030U)))

/* macros for field lpbk_en */
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_EN__SHIFT                   8
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_EN__WIDTH                   1
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_EN__MASK          0x00000100U
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_EN__READ(src) \
					(((uint32_t)(src)\
					& 0x00000100U) >> 8)
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_EN__WRITE(src) \
					(((uint32_t)(src)\
					<< 8) & 0x00000100U)
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_EN__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00000100U) | (((uint32_t)(src) <<\
					8) & 0x00000100U)
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_EN__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 8) & ~0x00000100U)))
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_EN__SET(dst) \
					(dst) = ((dst) &\
					~0x00000100U) | ((uint32_t)(1) << 8)
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_EN__CLR(dst) \
					(dst) = ((dst) &\
					~0x00000100U) | ((uint32_t)(0) << 8)

/* macros for field lpbk_internal */
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_INTERNAL__SHIFT             9
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_INTERNAL__WIDTH             1
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_INTERNAL__MASK    0x00000200U
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_INTERNAL__READ(src) \
					(((uint32_t)(src)\
					& 0x00000200U) >> 9)
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_INTERNAL__WRITE(src) \
					(((uint32_t)(src)\
					<< 9) & 0x00000200U)
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_INTERNAL__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00000200U) | (((uint32_t)(src) <<\
					9) & 0x00000200U)
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_INTERNAL__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 9) & ~0x00000200U)))
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_INTERNAL__SET(dst) \
					(dst) = ((dst) &\
					~0x00000200U) | ((uint32_t)(1) << 9)
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_INTERNAL__CLR(dst) \
					(dst) = ((dst) &\
					~0x00000200U) | ((uint32_t)(0) << 9)

/* macros for field loopback_control */
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LOOPBACK_CONTROL__SHIFT         10
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LOOPBACK_CONTROL__WIDTH          2
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LOOPBACK_CONTROL__MASK 0x00000c00U
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LOOPBACK_CONTROL__READ(src) \
					(((uint32_t)(src)\
					& 0x00000c00U) >> 10)
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LOOPBACK_CONTROL__WRITE(src) \
					(((uint32_t)(src)\
					<< 10) & 0x00000c00U)
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LOOPBACK_CONTROL__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00000c00U) | (((uint32_t)(src) <<\
					10) & 0x00000c00U)
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LOOPBACK_CONTROL__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 10) & ~0x00000c00U)))

/* macros for field lpbk_fail_muxsel */
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_FAIL_MUXSEL__SHIFT         12
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_FAIL_MUXSEL__WIDTH          1
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_FAIL_MUXSEL__MASK 0x00001000U
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_FAIL_MUXSEL__READ(src) \
					(((uint32_t)(src)\
					& 0x00001000U) >> 12)
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_FAIL_MUXSEL__WRITE(src) \
					(((uint32_t)(src)\
					<< 12) & 0x00001000U)
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_FAIL_MUXSEL__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00001000U) | (((uint32_t)(src) <<\
					12) & 0x00001000U)
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_FAIL_MUXSEL__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 12) & ~0x00001000U)))
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_FAIL_MUXSEL__SET(dst) \
					(dst) = ((dst) &\
					~0x00001000U) | ((uint32_t)(1) << 12)
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_FAIL_MUXSEL__CLR(dst) \
					(dst) = ((dst) &\
					~0x00001000U) | ((uint32_t)(0) << 12)

/* macros for field lpbk_err_check_timing */
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_ERR_CHECK_TIMING__SHIFT    13
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_ERR_CHECK_TIMING__WIDTH     3
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_ERR_CHECK_TIMING__MASK \
					0x0000e000U
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_ERR_CHECK_TIMING__READ(src) \
					(((uint32_t)(src)\
					& 0x0000e000U) >> 13)
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_ERR_CHECK_TIMING__WRITE(src) \
					(((uint32_t)(src)\
					<< 13) & 0x0000e000U)
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_ERR_CHECK_TIMING__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x0000e000U) | (((uint32_t)(src) <<\
					13) & 0x0000e000U)
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__LPBK_ERR_CHECK_TIMING__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 13) & ~0x0000e000U)))

/* macros for field rd_del_sel */
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__RD_DEL_SEL__SHIFT               19
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__RD_DEL_SEL__WIDTH                5
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__RD_DEL_SEL__MASK       0x00f80000U
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__RD_DEL_SEL__READ(src) \
					(((uint32_t)(src)\
					& 0x00f80000U) >> 19)
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__RD_DEL_SEL__WRITE(src) \
					(((uint32_t)(src)\
					<< 19) & 0x00f80000U)
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__RD_DEL_SEL__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00f80000U) | (((uint32_t)(src) <<\
					19) & 0x00f80000U)
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__RD_DEL_SEL__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 19) & ~0x00f80000U)))
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__TYPE                      uint32_t
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__READ                   0x00f8ff3fU
#define DATASLICE_RFILE__GATE_LPBK_CTRL_REG__WRITE                  0x00f8ff3fU

#endif /* __DATASLICE_RFILE__GATE_LPBK_CTRL_REG_MACRO__ */


/* macros for phy_dataslice.phy_gate_lpbk_ctrl_reg */
#define INST_PHY_DATASLICE__PHY_GATE_LPBK_CTRL_REG__NUM                       1

/* macros for BlueprintGlobalNameSpace::dataslice_Rfile::dll_master_ctrl_reg */
#ifndef __DATASLICE_RFILE__DLL_MASTER_CTRL_REG_MACRO__
#define __DATASLICE_RFILE__DLL_MASTER_CTRL_REG_MACRO__

/* macros for field param_dll_start_point */
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_DLL_START_POINT__SHIFT    0
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_DLL_START_POINT__WIDTH    8
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_DLL_START_POINT__MASK \
					0x000000ffU
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_DLL_START_POINT__READ(src) \
					((uint32_t)(src)\
					& 0x000000ffU)
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_DLL_START_POINT__WRITE(src) \
					((uint32_t)(src)\
					& 0x000000ffU)
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_DLL_START_POINT__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x000000ffU) | ((uint32_t)(src) &\
					0x000000ffU)
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_DLL_START_POINT__VERIFY(src) \
					(!(((uint32_t)(src)\
					& ~0x000000ffU)))

/* macros for field param_dll_lock_num */
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_DLL_LOCK_NUM__SHIFT      16
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_DLL_LOCK_NUM__WIDTH       3
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_DLL_LOCK_NUM__MASK \
					0x00070000U
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_DLL_LOCK_NUM__READ(src) \
					(((uint32_t)(src)\
					& 0x00070000U) >> 16)
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_DLL_LOCK_NUM__WRITE(src) \
					(((uint32_t)(src)\
					<< 16) & 0x00070000U)
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_DLL_LOCK_NUM__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00070000U) | (((uint32_t)(src) <<\
					16) & 0x00070000U)
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_DLL_LOCK_NUM__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 16) & ~0x00070000U)))

/* macros for field param_phase_detect_sel */
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_PHASE_DETECT_SEL__SHIFT  19
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_PHASE_DETECT_SEL__WIDTH   4
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_PHASE_DETECT_SEL__MASK \
					0x00780000U
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_PHASE_DETECT_SEL__READ(src) \
					(((uint32_t)(src)\
					& 0x00780000U) >> 19)
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_PHASE_DETECT_SEL__WRITE(src) \
					(((uint32_t)(src)\
					<< 19) & 0x00780000U)
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_PHASE_DETECT_SEL__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00780000U) | (((uint32_t)(src) <<\
					19) & 0x00780000U)
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_PHASE_DETECT_SEL__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 19) & ~0x00780000U)))

/* macros for field param_dll_bypass_mode */
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_DLL_BYPASS_MODE__SHIFT   23
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_DLL_BYPASS_MODE__WIDTH    1
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_DLL_BYPASS_MODE__MASK \
					0x00800000U
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_DLL_BYPASS_MODE__READ(src) \
					(((uint32_t)(src)\
					& 0x00800000U) >> 23)
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_DLL_BYPASS_MODE__WRITE(src) \
					(((uint32_t)(src)\
					<< 23) & 0x00800000U)
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_DLL_BYPASS_MODE__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00800000U) | (((uint32_t)(src) <<\
					23) & 0x00800000U)
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_DLL_BYPASS_MODE__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 23) & ~0x00800000U)))
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_DLL_BYPASS_MODE__SET(dst) \
					(dst) = ((dst) &\
					~0x00800000U) | ((uint32_t)(1) << 23)
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_DLL_BYPASS_MODE__CLR(dst) \
					(dst) = ((dst) &\
					~0x00800000U) | ((uint32_t)(0) << 23)

/* macros for field param_half_clock_mode */
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_HALF_CLOCK_MODE__SHIFT   24
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_HALF_CLOCK_MODE__WIDTH    1
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_HALF_CLOCK_MODE__MASK \
					0x01000000U
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_HALF_CLOCK_MODE__READ(src) \
					(((uint32_t)(src)\
					& 0x01000000U) >> 24)
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_HALF_CLOCK_MODE__WRITE(src) \
					(((uint32_t)(src)\
					<< 24) & 0x01000000U)
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_HALF_CLOCK_MODE__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x01000000U) | (((uint32_t)(src) <<\
					24) & 0x01000000U)
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_HALF_CLOCK_MODE__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 24) & ~0x01000000U)))
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_HALF_CLOCK_MODE__SET(dst) \
					(dst) = ((dst) &\
					~0x01000000U) | ((uint32_t)(1) << 24)
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__PARAM_HALF_CLOCK_MODE__CLR(dst) \
					(dst) = ((dst) &\
					~0x01000000U) | ((uint32_t)(0) << 24)
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__TYPE                     uint32_t
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__READ                  0x01ff00ffU
#define DATASLICE_RFILE__DLL_MASTER_CTRL_REG__WRITE                 0x01ff00ffU

#endif /* __DATASLICE_RFILE__DLL_MASTER_CTRL_REG_MACRO__ */


/* macros for phy_dataslice.phy_dll_master_ctrl_reg */
#define INST_PHY_DATASLICE__PHY_DLL_MASTER_CTRL_REG__NUM                      1

/* macros for BlueprintGlobalNameSpace::dataslice_Rfile::dll_slave_ctrl_reg */
#ifndef __DATASLICE_RFILE__DLL_SLAVE_CTRL_REG_MACRO__
#define __DATASLICE_RFILE__DLL_SLAVE_CTRL_REG_MACRO__

/* macros for field read_dqs_delay */
#define DATASLICE_RFILE__DLL_SLAVE_CTRL_REG__READ_DQS_DELAY__SHIFT            0
#define DATASLICE_RFILE__DLL_SLAVE_CTRL_REG__READ_DQS_DELAY__WIDTH            8
#define DATASLICE_RFILE__DLL_SLAVE_CTRL_REG__READ_DQS_DELAY__MASK   0x000000ffU
#define DATASLICE_RFILE__DLL_SLAVE_CTRL_REG__READ_DQS_DELAY__READ(src) \
					((uint32_t)(src)\
					& 0x000000ffU)
#define DATASLICE_RFILE__DLL_SLAVE_CTRL_REG__READ_DQS_DELAY__WRITE(src) \
					((uint32_t)(src)\
					& 0x000000ffU)
#define DATASLICE_RFILE__DLL_SLAVE_CTRL_REG__READ_DQS_DELAY__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x000000ffU) | ((uint32_t)(src) &\
					0x000000ffU)
#define DATASLICE_RFILE__DLL_SLAVE_CTRL_REG__READ_DQS_DELAY__VERIFY(src) \
					(!(((uint32_t)(src)\
					& ~0x000000ffU)))

/* macros for field clk_wr_delay */
#define DATASLICE_RFILE__DLL_SLAVE_CTRL_REG__CLK_WR_DELAY__SHIFT              8
#define DATASLICE_RFILE__DLL_SLAVE_CTRL_REG__CLK_WR_DELAY__WIDTH              8
#define DATASLICE_RFILE__DLL_SLAVE_CTRL_REG__CLK_WR_DELAY__MASK     0x0000ff00U
#define DATASLICE_RFILE__DLL_SLAVE_CTRL_REG__CLK_WR_DELAY__READ(src) \
					(((uint32_t)(src)\
					& 0x0000ff00U) >> 8)
#define DATASLICE_RFILE__DLL_SLAVE_CTRL_REG__CLK_WR_DELAY__WRITE(src) \
					(((uint32_t)(src)\
					<< 8) & 0x0000ff00U)
#define DATASLICE_RFILE__DLL_SLAVE_CTRL_REG__CLK_WR_DELAY__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x0000ff00U) | (((uint32_t)(src) <<\
					8) & 0x0000ff00U)
#define DATASLICE_RFILE__DLL_SLAVE_CTRL_REG__CLK_WR_DELAY__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 8) & ~0x0000ff00U)))
#define DATASLICE_RFILE__DLL_SLAVE_CTRL_REG__TYPE                      uint32_t
#define DATASLICE_RFILE__DLL_SLAVE_CTRL_REG__READ                   0x0000ffffU
#define DATASLICE_RFILE__DLL_SLAVE_CTRL_REG__WRITE                  0x0000ffffU

#endif /* __DATASLICE_RFILE__DLL_SLAVE_CTRL_REG_MACRO__ */


/* macros for phy_dataslice.phy_dll_slave_ctrl_reg */
#define INST_PHY_DATASLICE__PHY_DLL_SLAVE_CTRL_REG__NUM                       1

/* macros for BlueprintGlobalNameSpace::dataslice_Rfile::obs_reg_0 */
#ifndef __DATASLICE_RFILE__OBS_REG_0_MACRO__
#define __DATASLICE_RFILE__OBS_REG_0_MACRO__

/* macros for field lpbk_status */
#define DATASLICE_RFILE__OBS_REG_0__LPBK_STATUS__SHIFT                        0
#define DATASLICE_RFILE__OBS_REG_0__LPBK_STATUS__WIDTH                        2
#define DATASLICE_RFILE__OBS_REG_0__LPBK_STATUS__MASK               0x00000003U
#define DATASLICE_RFILE__OBS_REG_0__LPBK_STATUS__READ(src) \
					((uint32_t)(src)\
					& 0x00000003U)

/* macros for field lpbk_dq_data */
#define DATASLICE_RFILE__OBS_REG_0__LPBK_DQ_DATA__SHIFT                       8
#define DATASLICE_RFILE__OBS_REG_0__LPBK_DQ_DATA__WIDTH                      16
#define DATASLICE_RFILE__OBS_REG_0__LPBK_DQ_DATA__MASK              0x00ffff00U
#define DATASLICE_RFILE__OBS_REG_0__LPBK_DQ_DATA__READ(src) \
					(((uint32_t)(src)\
					& 0x00ffff00U) >> 8)
#define DATASLICE_RFILE__OBS_REG_0__TYPE                               uint32_t
#define DATASLICE_RFILE__OBS_REG_0__READ                            0x00ffff03U

#endif /* __DATASLICE_RFILE__OBS_REG_0_MACRO__ */


/* macros for phy_dataslice.phy_obs_reg_0 */
#define INST_PHY_DATASLICE__PHY_OBS_REG_0__NUM                                1

/* macros for BlueprintGlobalNameSpace::dataslice_Rfile::dll_obs_reg_0 */
#ifndef __DATASLICE_RFILE__DLL_OBS_REG_0_MACRO__
#define __DATASLICE_RFILE__DLL_OBS_REG_0_MACRO__

/* macros for field dll_lock */
#define DATASLICE_RFILE__DLL_OBS_REG_0__DLL_LOCK__SHIFT                       0
#define DATASLICE_RFILE__DLL_OBS_REG_0__DLL_LOCK__WIDTH                       1
#define DATASLICE_RFILE__DLL_OBS_REG_0__DLL_LOCK__MASK              0x00000001U
#define DATASLICE_RFILE__DLL_OBS_REG_0__DLL_LOCK__READ(src) \
					((uint32_t)(src)\
					& 0x00000001U)
#define DATASLICE_RFILE__DLL_OBS_REG_0__DLL_LOCK__SET(dst) \
					(dst) = ((dst) &\
					~0x00000001U) | (uint32_t)(1)
#define DATASLICE_RFILE__DLL_OBS_REG_0__DLL_LOCK__CLR(dst) \
					(dst) = ((dst) &\
					~0x00000001U) | (uint32_t)(0)

/* macros for field dll_locked_mode */
#define DATASLICE_RFILE__DLL_OBS_REG_0__DLL_LOCKED_MODE__SHIFT                1
#define DATASLICE_RFILE__DLL_OBS_REG_0__DLL_LOCKED_MODE__WIDTH                2
#define DATASLICE_RFILE__DLL_OBS_REG_0__DLL_LOCKED_MODE__MASK       0x00000006U
#define DATASLICE_RFILE__DLL_OBS_REG_0__DLL_LOCKED_MODE__READ(src) \
					(((uint32_t)(src)\
					& 0x00000006U) >> 1)

/* macros for field dll_unlock_count */
#define DATASLICE_RFILE__DLL_OBS_REG_0__DLL_UNLOCK_COUNT__SHIFT               3
#define DATASLICE_RFILE__DLL_OBS_REG_0__DLL_UNLOCK_COUNT__WIDTH               5
#define DATASLICE_RFILE__DLL_OBS_REG_0__DLL_UNLOCK_COUNT__MASK      0x000000f8U
#define DATASLICE_RFILE__DLL_OBS_REG_0__DLL_UNLOCK_COUNT__READ(src) \
					(((uint32_t)(src)\
					& 0x000000f8U) >> 3)

/* macros for field dll_lock_value */
#define DATASLICE_RFILE__DLL_OBS_REG_0__DLL_LOCK_VALUE__SHIFT                 8
#define DATASLICE_RFILE__DLL_OBS_REG_0__DLL_LOCK_VALUE__WIDTH                 8
#define DATASLICE_RFILE__DLL_OBS_REG_0__DLL_LOCK_VALUE__MASK        0x0000ff00U
#define DATASLICE_RFILE__DLL_OBS_REG_0__DLL_LOCK_VALUE__READ(src) \
					(((uint32_t)(src)\
					& 0x0000ff00U) >> 8)

/* macros for field lock_dec_dbg */
#define DATASLICE_RFILE__DLL_OBS_REG_0__LOCK_DEC_DBG__SHIFT                  16
#define DATASLICE_RFILE__DLL_OBS_REG_0__LOCK_DEC_DBG__WIDTH                   8
#define DATASLICE_RFILE__DLL_OBS_REG_0__LOCK_DEC_DBG__MASK          0x00ff0000U
#define DATASLICE_RFILE__DLL_OBS_REG_0__LOCK_DEC_DBG__READ(src) \
					(((uint32_t)(src)\
					& 0x00ff0000U) >> 16)

/* macros for field lock_inc_dbg */
#define DATASLICE_RFILE__DLL_OBS_REG_0__LOCK_INC_DBG__SHIFT                  24
#define DATASLICE_RFILE__DLL_OBS_REG_0__LOCK_INC_DBG__WIDTH                   8
#define DATASLICE_RFILE__DLL_OBS_REG_0__LOCK_INC_DBG__MASK          0xff000000U
#define DATASLICE_RFILE__DLL_OBS_REG_0__LOCK_INC_DBG__READ(src) \
					(((uint32_t)(src)\
					& 0xff000000U) >> 24)
#define DATASLICE_RFILE__DLL_OBS_REG_0__TYPE                           uint32_t
#define DATASLICE_RFILE__DLL_OBS_REG_0__READ                        0xffffffffU

#endif /* __DATASLICE_RFILE__DLL_OBS_REG_0_MACRO__ */


/* macros for phy_dataslice.phy_dll_obs_reg_0 */
#define INST_PHY_DATASLICE__PHY_DLL_OBS_REG_0__NUM                            1

/* macros for BlueprintGlobalNameSpace::dataslice_Rfile::dll_obs_reg_1 */
#ifndef __DATASLICE_RFILE__DLL_OBS_REG_1_MACRO__
#define __DATASLICE_RFILE__DLL_OBS_REG_1_MACRO__

/* macros for field decoder_out_rd */
#define DATASLICE_RFILE__DLL_OBS_REG_1__DECODER_OUT_RD__SHIFT                 0
#define DATASLICE_RFILE__DLL_OBS_REG_1__DECODER_OUT_RD__WIDTH                 8
#define DATASLICE_RFILE__DLL_OBS_REG_1__DECODER_OUT_RD__MASK        0x000000ffU
#define DATASLICE_RFILE__DLL_OBS_REG_1__DECODER_OUT_RD__READ(src) \
					((uint32_t)(src)\
					& 0x000000ffU)

/* macros for field decoder_out_wr */
#define DATASLICE_RFILE__DLL_OBS_REG_1__DECODER_OUT_WR__SHIFT                16
#define DATASLICE_RFILE__DLL_OBS_REG_1__DECODER_OUT_WR__WIDTH                 8
#define DATASLICE_RFILE__DLL_OBS_REG_1__DECODER_OUT_WR__MASK        0x00ff0000U
#define DATASLICE_RFILE__DLL_OBS_REG_1__DECODER_OUT_WR__READ(src) \
					(((uint32_t)(src)\
					& 0x00ff0000U) >> 16)
#define DATASLICE_RFILE__DLL_OBS_REG_1__TYPE                           uint32_t
#define DATASLICE_RFILE__DLL_OBS_REG_1__READ                        0x00ff00ffU

#endif /* __DATASLICE_RFILE__DLL_OBS_REG_1_MACRO__ */


/* macros for phy_dataslice.phy_dll_obs_reg_1 */
#define INST_PHY_DATASLICE__PHY_DLL_OBS_REG_1__NUM                            1

/* macros for BlueprintGlobalNameSpace::dataslice_Rfile::static_togg_ctrl_0 */
#ifndef __DATASLICE_RFILE__STATIC_TOGG_CTRL_0_MACRO__
#define __DATASLICE_RFILE__STATIC_TOGG_CTRL_0_MACRO__

/* macros for field static_tog_clk_div */
#define DATASLICE_RFILE__STATIC_TOGG_CTRL_0__STATIC_TOG_CLK_DIV__SHIFT        0
#define DATASLICE_RFILE__STATIC_TOGG_CTRL_0__STATIC_TOG_CLK_DIV__WIDTH       16
#define DATASLICE_RFILE__STATIC_TOGG_CTRL_0__STATIC_TOG_CLK_DIV__MASK \
					0x0000ffffU
#define DATASLICE_RFILE__STATIC_TOGG_CTRL_0__STATIC_TOG_CLK_DIV__READ(src) \
					((uint32_t)(src)\
					& 0x0000ffffU)
#define DATASLICE_RFILE__STATIC_TOGG_CTRL_0__STATIC_TOG_CLK_DIV__WRITE(src) \
					((uint32_t)(src)\
					& 0x0000ffffU)
#define DATASLICE_RFILE__STATIC_TOGG_CTRL_0__STATIC_TOG_CLK_DIV__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x0000ffffU) | ((uint32_t)(src) &\
					0x0000ffffU)
#define DATASLICE_RFILE__STATIC_TOGG_CTRL_0__STATIC_TOG_CLK_DIV__VERIFY(src) \
					(!(((uint32_t)(src)\
					& ~0x0000ffffU)))

/* macros for field static_togg_global_enable */
#define DATASLICE_RFILE__STATIC_TOGG_CTRL_0__STATIC_TOGG_GLOBAL_ENABLE__SHIFT \
					16
#define DATASLICE_RFILE__STATIC_TOGG_CTRL_0__STATIC_TOGG_GLOBAL_ENABLE__WIDTH 1
#define DATASLICE_RFILE__STATIC_TOGG_CTRL_0__STATIC_TOGG_GLOBAL_ENABLE__MASK \
					0x00010000U
#define DATASLICE_RFILE__STATIC_TOGG_CTRL_0__STATIC_TOGG_GLOBAL_ENABLE__READ(src) \
					(((uint32_t)(src)\
					& 0x00010000U) >> 16)
#define DATASLICE_RFILE__STATIC_TOGG_CTRL_0__STATIC_TOGG_GLOBAL_ENABLE__WRITE(src) \
					(((uint32_t)(src)\
					<< 16) & 0x00010000U)
#define DATASLICE_RFILE__STATIC_TOGG_CTRL_0__STATIC_TOGG_GLOBAL_ENABLE__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00010000U) | (((uint32_t)(src) <<\
					16) & 0x00010000U)
#define DATASLICE_RFILE__STATIC_TOGG_CTRL_0__STATIC_TOGG_GLOBAL_ENABLE__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 16) & ~0x00010000U)))
#define DATASLICE_RFILE__STATIC_TOGG_CTRL_0__STATIC_TOGG_GLOBAL_ENABLE__SET(dst) \
					(dst) = ((dst) &\
					~0x00010000U) | ((uint32_t)(1) << 16)
#define DATASLICE_RFILE__STATIC_TOGG_CTRL_0__STATIC_TOGG_GLOBAL_ENABLE__CLR(dst) \
					(dst) = ((dst) &\
					~0x00010000U) | ((uint32_t)(0) << 16)

/* macros for field static_togg_enable */
#define DATASLICE_RFILE__STATIC_TOGG_CTRL_0__STATIC_TOGG_ENABLE__SHIFT       20
#define DATASLICE_RFILE__STATIC_TOGG_CTRL_0__STATIC_TOGG_ENABLE__WIDTH        3
#define DATASLICE_RFILE__STATIC_TOGG_CTRL_0__STATIC_TOGG_ENABLE__MASK \
					0x00700000U
#define DATASLICE_RFILE__STATIC_TOGG_CTRL_0__STATIC_TOGG_ENABLE__READ(src) \
					(((uint32_t)(src)\
					& 0x00700000U) >> 20)
#define DATASLICE_RFILE__STATIC_TOGG_CTRL_0__STATIC_TOGG_ENABLE__WRITE(src) \
					(((uint32_t)(src)\
					<< 20) & 0x00700000U)
#define DATASLICE_RFILE__STATIC_TOGG_CTRL_0__STATIC_TOGG_ENABLE__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00700000U) | (((uint32_t)(src) <<\
					20) & 0x00700000U)
#define DATASLICE_RFILE__STATIC_TOGG_CTRL_0__STATIC_TOGG_ENABLE__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 20) & ~0x00700000U)))
#define DATASLICE_RFILE__STATIC_TOGG_CTRL_0__TYPE                      uint32_t
#define DATASLICE_RFILE__STATIC_TOGG_CTRL_0__READ                   0x0071ffffU
#define DATASLICE_RFILE__STATIC_TOGG_CTRL_0__WRITE                  0x0071ffffU

#endif /* __DATASLICE_RFILE__STATIC_TOGG_CTRL_0_MACRO__ */


/* macros for phy_dataslice.phy_static_togg_reg */
#define INST_PHY_DATASLICE__PHY_STATIC_TOGG_REG__NUM                          1

/* macros for BlueprintGlobalNameSpace::ctb_Rfile::ctrl_reg */
#ifndef __CTB_RFILE__CTRL_REG_MACRO__
#define __CTB_RFILE__CTRL_REG_MACRO__

/* macros for field phony_dqs_timing */
#define CTB_RFILE__CTRL_REG__PHONY_DQS_TIMING__SHIFT                          4
#define CTB_RFILE__CTRL_REG__PHONY_DQS_TIMING__WIDTH                          5
#define CTB_RFILE__CTRL_REG__PHONY_DQS_TIMING__MASK                 0x000001f0U
#define CTB_RFILE__CTRL_REG__PHONY_DQS_TIMING__READ(src) \
					(((uint32_t)(src)\
					& 0x000001f0U) >> 4)
#define CTB_RFILE__CTRL_REG__PHONY_DQS_TIMING__WRITE(src) \
					(((uint32_t)(src)\
					<< 4) & 0x000001f0U)
#define CTB_RFILE__CTRL_REG__PHONY_DQS_TIMING__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x000001f0U) | (((uint32_t)(src) <<\
					4) & 0x000001f0U)
#define CTB_RFILE__CTRL_REG__PHONY_DQS_TIMING__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 4) & ~0x000001f0U)))

/* macros for field sdr_dqs_value */
#define CTB_RFILE__CTRL_REG__SDR_DQS_VALUE__SHIFT                            14
#define CTB_RFILE__CTRL_REG__SDR_DQS_VALUE__WIDTH                             1
#define CTB_RFILE__CTRL_REG__SDR_DQS_VALUE__MASK                    0x00004000U
#define CTB_RFILE__CTRL_REG__SDR_DQS_VALUE__READ(src) \
					(((uint32_t)(src)\
					& 0x00004000U) >> 14)
#define CTB_RFILE__CTRL_REG__SDR_DQS_VALUE__WRITE(src) \
					(((uint32_t)(src)\
					<< 14) & 0x00004000U)
#define CTB_RFILE__CTRL_REG__SDR_DQS_VALUE__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00004000U) | (((uint32_t)(src) <<\
					14) & 0x00004000U)
#define CTB_RFILE__CTRL_REG__SDR_DQS_VALUE__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 14) & ~0x00004000U)))
#define CTB_RFILE__CTRL_REG__SDR_DQS_VALUE__SET(dst) \
					(dst) = ((dst) &\
					~0x00004000U) | ((uint32_t)(1) << 14)
#define CTB_RFILE__CTRL_REG__SDR_DQS_VALUE__CLR(dst) \
					(dst) = ((dst) &\
					~0x00004000U) | ((uint32_t)(0) << 14)
#define CTB_RFILE__CTRL_REG__TYPE                                      uint32_t
#define CTB_RFILE__CTRL_REG__READ                                   0x000041f0U
#define CTB_RFILE__CTRL_REG__WRITE                                  0x000041f0U

#endif /* __CTB_RFILE__CTRL_REG_MACRO__ */


/* macros for phy_ctb.phy_ctrl_reg */
#define INST_PHY_CTB__PHY_CTRL_REG__NUM                                       1

/* macros for BlueprintGlobalNameSpace::ctb_Rfile::tsel_reg */
#ifndef __CTB_RFILE__TSEL_REG_MACRO__
#define __CTB_RFILE__TSEL_REG_MACRO__

/* macros for field tsel_rd_value_dqs */
#define CTB_RFILE__TSEL_REG__TSEL_RD_VALUE_DQS__SHIFT                         8
#define CTB_RFILE__TSEL_REG__TSEL_RD_VALUE_DQS__WIDTH                         4
#define CTB_RFILE__TSEL_REG__TSEL_RD_VALUE_DQS__MASK                0x00000f00U
#define CTB_RFILE__TSEL_REG__TSEL_RD_VALUE_DQS__READ(src) \
					(((uint32_t)(src)\
					& 0x00000f00U) >> 8)
#define CTB_RFILE__TSEL_REG__TSEL_RD_VALUE_DQS__WRITE(src) \
					(((uint32_t)(src)\
					<< 8) & 0x00000f00U)
#define CTB_RFILE__TSEL_REG__TSEL_RD_VALUE_DQS__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00000f00U) | (((uint32_t)(src) <<\
					8) & 0x00000f00U)
#define CTB_RFILE__TSEL_REG__TSEL_RD_VALUE_DQS__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 8) & ~0x00000f00U)))

/* macros for field tsel_off_value_dqs */
#define CTB_RFILE__TSEL_REG__TSEL_OFF_VALUE_DQS__SHIFT                       12
#define CTB_RFILE__TSEL_REG__TSEL_OFF_VALUE_DQS__WIDTH                        4
#define CTB_RFILE__TSEL_REG__TSEL_OFF_VALUE_DQS__MASK               0x0000f000U
#define CTB_RFILE__TSEL_REG__TSEL_OFF_VALUE_DQS__READ(src) \
					(((uint32_t)(src)\
					& 0x0000f000U) >> 12)
#define CTB_RFILE__TSEL_REG__TSEL_OFF_VALUE_DQS__WRITE(src) \
					(((uint32_t)(src)\
					<< 12) & 0x0000f000U)
#define CTB_RFILE__TSEL_REG__TSEL_OFF_VALUE_DQS__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x0000f000U) | (((uint32_t)(src) <<\
					12) & 0x0000f000U)
#define CTB_RFILE__TSEL_REG__TSEL_OFF_VALUE_DQS__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 12) & ~0x0000f000U)))

/* macros for field tsel_rd_value_data */
#define CTB_RFILE__TSEL_REG__TSEL_RD_VALUE_DATA__SHIFT                       16
#define CTB_RFILE__TSEL_REG__TSEL_RD_VALUE_DATA__WIDTH                        4
#define CTB_RFILE__TSEL_REG__TSEL_RD_VALUE_DATA__MASK               0x000f0000U
#define CTB_RFILE__TSEL_REG__TSEL_RD_VALUE_DATA__READ(src) \
					(((uint32_t)(src)\
					& 0x000f0000U) >> 16)
#define CTB_RFILE__TSEL_REG__TSEL_RD_VALUE_DATA__WRITE(src) \
					(((uint32_t)(src)\
					<< 16) & 0x000f0000U)
#define CTB_RFILE__TSEL_REG__TSEL_RD_VALUE_DATA__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x000f0000U) | (((uint32_t)(src) <<\
					16) & 0x000f0000U)
#define CTB_RFILE__TSEL_REG__TSEL_RD_VALUE_DATA__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 16) & ~0x000f0000U)))

/* macros for field tsel_off_value_data */
#define CTB_RFILE__TSEL_REG__TSEL_OFF_VALUE_DATA__SHIFT                      20
#define CTB_RFILE__TSEL_REG__TSEL_OFF_VALUE_DATA__WIDTH                       4
#define CTB_RFILE__TSEL_REG__TSEL_OFF_VALUE_DATA__MASK              0x00f00000U
#define CTB_RFILE__TSEL_REG__TSEL_OFF_VALUE_DATA__READ(src) \
					(((uint32_t)(src)\
					& 0x00f00000U) >> 20)
#define CTB_RFILE__TSEL_REG__TSEL_OFF_VALUE_DATA__WRITE(src) \
					(((uint32_t)(src)\
					<< 20) & 0x00f00000U)
#define CTB_RFILE__TSEL_REG__TSEL_OFF_VALUE_DATA__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00f00000U) | (((uint32_t)(src) <<\
					20) & 0x00f00000U)
#define CTB_RFILE__TSEL_REG__TSEL_OFF_VALUE_DATA__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 20) & ~0x00f00000U)))
#define CTB_RFILE__TSEL_REG__TYPE                                      uint32_t
#define CTB_RFILE__TSEL_REG__READ                                   0x00ffff00U
#define CTB_RFILE__TSEL_REG__WRITE                                  0x00ffff00U

#endif /* __CTB_RFILE__TSEL_REG_MACRO__ */


/* macros for phy_ctb.phy_tsel_reg */
#define INST_PHY_CTB__PHY_TSEL_REG__NUM                                       1

/* macros for BlueprintGlobalNameSpace::ctb_Rfile::gp_reg_0 */
#ifndef __CTB_RFILE__GP_REG_0_MACRO__
#define __CTB_RFILE__GP_REG_0_MACRO__

/* macros for field phy_gpio_ctrl_value */
#define CTB_RFILE__GP_REG_0__PHY_GPIO_CTRL_VALUE__SHIFT                       0
#define CTB_RFILE__GP_REG_0__PHY_GPIO_CTRL_VALUE__WIDTH                      32
#define CTB_RFILE__GP_REG_0__PHY_GPIO_CTRL_VALUE__MASK              0xffffffffU
#define CTB_RFILE__GP_REG_0__PHY_GPIO_CTRL_VALUE__READ(src) \
					((uint32_t)(src)\
					& 0xffffffffU)
#define CTB_RFILE__GP_REG_0__PHY_GPIO_CTRL_VALUE__WRITE(src) \
					((uint32_t)(src)\
					& 0xffffffffU)
#define CTB_RFILE__GP_REG_0__PHY_GPIO_CTRL_VALUE__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0xffffffffU) | ((uint32_t)(src) &\
					0xffffffffU)
#define CTB_RFILE__GP_REG_0__PHY_GPIO_CTRL_VALUE__VERIFY(src) \
					(!(((uint32_t)(src)\
					& ~0xffffffffU)))
#define CTB_RFILE__GP_REG_0__TYPE                                      uint32_t
#define CTB_RFILE__GP_REG_0__READ                                   0xffffffffU
#define CTB_RFILE__GP_REG_0__WRITE                                  0xffffffffU

#endif /* __CTB_RFILE__GP_REG_0_MACRO__ */


/* macros for phy_ctb.phy_gpio_ctrl */
#define INST_PHY_CTB__PHY_GPIO_CTRL__NUM                                      1

/* macros for BlueprintGlobalNameSpace::ctb_Rfile::gp_reg_1 */
#ifndef __CTB_RFILE__GP_REG_1_MACRO__
#define __CTB_RFILE__GP_REG_1_MACRO__

/* macros for field phy_gpio_status_value */
#define CTB_RFILE__GP_REG_1__PHY_GPIO_STATUS_VALUE__SHIFT                     0
#define CTB_RFILE__GP_REG_1__PHY_GPIO_STATUS_VALUE__WIDTH                    32
#define CTB_RFILE__GP_REG_1__PHY_GPIO_STATUS_VALUE__MASK            0xffffffffU
#define CTB_RFILE__GP_REG_1__PHY_GPIO_STATUS_VALUE__READ(src) \
					((uint32_t)(src)\
					& 0xffffffffU)
#define CTB_RFILE__GP_REG_1__TYPE                                      uint32_t
#define CTB_RFILE__GP_REG_1__READ                                   0xffffffffU

#endif /* __CTB_RFILE__GP_REG_1_MACRO__ */


/* macros for phy_ctb.phy_gpio_status */
#define INST_PHY_CTB__PHY_GPIO_STATUS__NUM                                    1
#define RFILE_INST_PHY_DATASLICE__NUM                                         1
#define RFILE_INST_PHY_CTB__NUM                                               1

#endif /* __REG_DLL_PHY_ADDR_MAP_MACRO_H__ */
