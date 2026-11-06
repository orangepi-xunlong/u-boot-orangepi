#ifndef __REG_NFC9_MINICTRL_REGS_MACRO_H__
#define __REG_NFC9_MINICTRL_REGS_MACRO_H__


/* macros for BlueprintGlobalNameSpace::rf_minictrl_regs::rt_wp_settings */
#ifndef __RF_MINICTRL_REGS__RT_WP_SETTINGS_MACRO__
#define __RF_MINICTRL_REGS__RT_WP_SETTINGS_MACRO__

/* macros for field WP */
#define RF_MINICTRL_REGS__RT_WP_SETTINGS__WP__SHIFT                           0
#define RF_MINICTRL_REGS__RT_WP_SETTINGS__WP__WIDTH                           1
#define RF_MINICTRL_REGS__RT_WP_SETTINGS__WP__MASK                  0x00000001U
#define RF_MINICTRL_REGS__RT_WP_SETTINGS__WP__READ(src) \
					((uint32_t)(src)\
					& 0x00000001U)
#define RF_MINICTRL_REGS__RT_WP_SETTINGS__WP__WRITE(src) \
					((uint32_t)(src)\
					& 0x00000001U)
#define RF_MINICTRL_REGS__RT_WP_SETTINGS__WP__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00000001U) | ((uint32_t)(src) &\
					0x00000001U)
#define RF_MINICTRL_REGS__RT_WP_SETTINGS__WP__VERIFY(src) \
					(!(((uint32_t)(src)\
					& ~0x00000001U)))
#define RF_MINICTRL_REGS__RT_WP_SETTINGS__WP__SET(dst) \
					(dst) = ((dst) &\
					~0x00000001U) | (uint32_t)(1)
#define RF_MINICTRL_REGS__RT_WP_SETTINGS__WP__CLR(dst) \
					(dst) = ((dst) &\
					~0x00000001U) | (uint32_t)(0)
#define RF_MINICTRL_REGS__RT_WP_SETTINGS__TYPE                         uint32_t
#define RF_MINICTRL_REGS__RT_WP_SETTINGS__READ                      0x00000001U
#define RF_MINICTRL_REGS__RT_WP_SETTINGS__WRITE                     0x00000001U

#endif /* __RF_MINICTRL_REGS__RT_WP_SETTINGS_MACRO__ */


/* macros for minictrl_settings.wp_settings */
#define INST_MINICTRL_SETTINGS__WP_SETTINGS__NUM                              1

/* macros for BlueprintGlobalNameSpace::rf_minictrl_regs::rt_rbn_settings */
#ifndef __RF_MINICTRL_REGS__RT_RBN_SETTINGS_MACRO__
#define __RF_MINICTRL_REGS__RT_RBN_SETTINGS_MACRO__

/* macros for field Rbn */
#define RF_MINICTRL_REGS__RT_RBN_SETTINGS__RBN__SHIFT                         0
#define RF_MINICTRL_REGS__RT_RBN_SETTINGS__RBN__WIDTH                         8
#define RF_MINICTRL_REGS__RT_RBN_SETTINGS__RBN__MASK                0x000000ffU
#define RF_MINICTRL_REGS__RT_RBN_SETTINGS__RBN__READ(src) \
					((uint32_t)(src)\
					& 0x000000ffU)
#define RF_MINICTRL_REGS__RT_RBN_SETTINGS__TYPE                        uint32_t
#define RF_MINICTRL_REGS__RT_RBN_SETTINGS__READ                     0x000000ffU

#endif /* __RF_MINICTRL_REGS__RT_RBN_SETTINGS_MACRO__ */


/* macros for minictrl_settings.rbn_settings */
#define INST_MINICTRL_SETTINGS__RBN_SETTINGS__NUM                             1

/* macros for BlueprintGlobalNameSpace::rf_minictrl_regs::rt_common_settings */
#ifndef __RF_MINICTRL_REGS__RT_COMMON_SETTINGS_MACRO__
#define __RF_MINICTRL_REGS__RT_COMMON_SETTINGS_MACRO__

/* macros for field opr_mode */
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__OPR_MODE__SHIFT                 0
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__OPR_MODE__WIDTH                 2
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__OPR_MODE__MASK        0x00000003U
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__OPR_MODE__READ(src) \
					((uint32_t)(src)\
					& 0x00000003U)
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__OPR_MODE__WRITE(src) \
					((uint32_t)(src)\
					& 0x00000003U)
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__OPR_MODE__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00000003U) | ((uint32_t)(src) &\
					0x00000003U)
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__OPR_MODE__VERIFY(src) \
					(!(((uint32_t)(src)\
					& ~0x00000003U)))

/* macros for field device_16bit */
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__DEVICE_16BIT__SHIFT             8
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__DEVICE_16BIT__WIDTH             1
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__DEVICE_16BIT__MASK    0x00000100U
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__DEVICE_16BIT__READ(src) \
					(((uint32_t)(src)\
					& 0x00000100U) >> 8)
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__DEVICE_16BIT__WRITE(src) \
					(((uint32_t)(src)\
					<< 8) & 0x00000100U)
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__DEVICE_16BIT__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00000100U) | (((uint32_t)(src) <<\
					8) & 0x00000100U)
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__DEVICE_16BIT__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 8) & ~0x00000100U)))
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__DEVICE_16BIT__SET(dst) \
					(dst) = ((dst) &\
					~0x00000100U) | ((uint32_t)(1) << 8)
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__DEVICE_16BIT__CLR(dst) \
					(dst) = ((dst) &\
					~0x00000100U) | ((uint32_t)(0) << 8)

/* macros for field rd_warmup */
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__RD_WARMUP__SHIFT               16
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__RD_WARMUP__WIDTH                4
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__RD_WARMUP__MASK       0x000f0000U
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__RD_WARMUP__READ(src) \
					(((uint32_t)(src)\
					& 0x000f0000U) >> 16)
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__RD_WARMUP__WRITE(src) \
					(((uint32_t)(src)\
					<< 16) & 0x000f0000U)
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__RD_WARMUP__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x000f0000U) | (((uint32_t)(src) <<\
					16) & 0x000f0000U)
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__RD_WARMUP__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 16) & ~0x000f0000U)))

/* macros for field wr_warmup */
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__WR_WARMUP__SHIFT               20
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__WR_WARMUP__WIDTH                4
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__WR_WARMUP__MASK       0x00f00000U
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__WR_WARMUP__READ(src) \
					(((uint32_t)(src)\
					& 0x00f00000U) >> 20)
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__WR_WARMUP__WRITE(src) \
					(((uint32_t)(src)\
					<< 20) & 0x00f00000U)
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__WR_WARMUP__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00f00000U) | (((uint32_t)(src) <<\
					20) & 0x00f00000U)
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__WR_WARMUP__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 20) & ~0x00f00000U)))
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__TYPE                     uint32_t
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__READ                  0x00ff0103U
#define RF_MINICTRL_REGS__RT_COMMON_SETTINGS__WRITE                 0x00ff0103U

#endif /* __RF_MINICTRL_REGS__RT_COMMON_SETTINGS_MACRO__ */


/* macros for minictrl_settings.common_settings */
#define INST_MINICTRL_SETTINGS__COMMON_SETTINGS__NUM                          1

/* macros for BlueprintGlobalNameSpace::rf_minictrl_regs::rt_skip_bytes_conf */
#ifndef __RF_MINICTRL_REGS__RT_SKIP_BYTES_CONF_MACRO__
#define __RF_MINICTRL_REGS__RT_SKIP_BYTES_CONF_MACRO__

/* macros for field skip_bytes */
#define RF_MINICTRL_REGS__RT_SKIP_BYTES_CONF__SKIP_BYTES__SHIFT               0
#define RF_MINICTRL_REGS__RT_SKIP_BYTES_CONF__SKIP_BYTES__WIDTH               8
#define RF_MINICTRL_REGS__RT_SKIP_BYTES_CONF__SKIP_BYTES__MASK      0x000000ffU
#define RF_MINICTRL_REGS__RT_SKIP_BYTES_CONF__SKIP_BYTES__READ(src) \
					((uint32_t)(src)\
					& 0x000000ffU)
#define RF_MINICTRL_REGS__RT_SKIP_BYTES_CONF__SKIP_BYTES__WRITE(src) \
					((uint32_t)(src)\
					& 0x000000ffU)
#define RF_MINICTRL_REGS__RT_SKIP_BYTES_CONF__SKIP_BYTES__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x000000ffU) | ((uint32_t)(src) &\
					0x000000ffU)
#define RF_MINICTRL_REGS__RT_SKIP_BYTES_CONF__SKIP_BYTES__VERIFY(src) \
					(!(((uint32_t)(src)\
					& ~0x000000ffU)))

/* macros for field marker */
#define RF_MINICTRL_REGS__RT_SKIP_BYTES_CONF__MARKER__SHIFT                  16
#define RF_MINICTRL_REGS__RT_SKIP_BYTES_CONF__MARKER__WIDTH                  16
#define RF_MINICTRL_REGS__RT_SKIP_BYTES_CONF__MARKER__MASK          0xffff0000U
#define RF_MINICTRL_REGS__RT_SKIP_BYTES_CONF__MARKER__READ(src) \
					(((uint32_t)(src)\
					& 0xffff0000U) >> 16)
#define RF_MINICTRL_REGS__RT_SKIP_BYTES_CONF__MARKER__WRITE(src) \
					(((uint32_t)(src)\
					<< 16) & 0xffff0000U)
#define RF_MINICTRL_REGS__RT_SKIP_BYTES_CONF__MARKER__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0xffff0000U) | (((uint32_t)(src) <<\
					16) & 0xffff0000U)
#define RF_MINICTRL_REGS__RT_SKIP_BYTES_CONF__MARKER__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 16) & ~0xffff0000U)))
#define RF_MINICTRL_REGS__RT_SKIP_BYTES_CONF__TYPE                     uint32_t
#define RF_MINICTRL_REGS__RT_SKIP_BYTES_CONF__READ                  0xffff00ffU
#define RF_MINICTRL_REGS__RT_SKIP_BYTES_CONF__WRITE                 0xffff00ffU

#endif /* __RF_MINICTRL_REGS__RT_SKIP_BYTES_CONF_MACRO__ */


/* macros for minictrl_settings.skip_bytes_conf */
#define INST_MINICTRL_SETTINGS__SKIP_BYTES_CONF__NUM                          1

/* macros for BlueprintGlobalNameSpace::rf_minictrl_regs::rt_skip_bytes_offset */
#ifndef __RF_MINICTRL_REGS__RT_SKIP_BYTES_OFFSET_MACRO__
#define __RF_MINICTRL_REGS__RT_SKIP_BYTES_OFFSET_MACRO__

/* macros for field skip_bytes_offset */
#define RF_MINICTRL_REGS__RT_SKIP_BYTES_OFFSET__SKIP_BYTES_OFFSET__SHIFT      0
#define RF_MINICTRL_REGS__RT_SKIP_BYTES_OFFSET__SKIP_BYTES_OFFSET__WIDTH     24
#define RF_MINICTRL_REGS__RT_SKIP_BYTES_OFFSET__SKIP_BYTES_OFFSET__MASK \
					0x00ffffffU
#define RF_MINICTRL_REGS__RT_SKIP_BYTES_OFFSET__SKIP_BYTES_OFFSET__READ(src) \
					((uint32_t)(src)\
					& 0x00ffffffU)
#define RF_MINICTRL_REGS__RT_SKIP_BYTES_OFFSET__SKIP_BYTES_OFFSET__WRITE(src) \
					((uint32_t)(src)\
					& 0x00ffffffU)
#define RF_MINICTRL_REGS__RT_SKIP_BYTES_OFFSET__SKIP_BYTES_OFFSET__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00ffffffU) | ((uint32_t)(src) &\
					0x00ffffffU)
#define RF_MINICTRL_REGS__RT_SKIP_BYTES_OFFSET__SKIP_BYTES_OFFSET__VERIFY(src) \
					(!(((uint32_t)(src)\
					& ~0x00ffffffU)))
#define RF_MINICTRL_REGS__RT_SKIP_BYTES_OFFSET__TYPE                   uint32_t
#define RF_MINICTRL_REGS__RT_SKIP_BYTES_OFFSET__READ                0x00ffffffU
#define RF_MINICTRL_REGS__RT_SKIP_BYTES_OFFSET__WRITE               0x00ffffffU

#endif /* __RF_MINICTRL_REGS__RT_SKIP_BYTES_OFFSET_MACRO__ */


/* macros for minictrl_settings.skip_bytes_offset */
#define INST_MINICTRL_SETTINGS__SKIP_BYTES_OFFSET__NUM                        1

/* macros for BlueprintGlobalNameSpace::rf_minictrl_regs::rt_toggle_timings_0 */
#ifndef __RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0_MACRO__
#define __RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0_MACRO__

/* macros for field tPSTH */
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0__TPSTH__SHIFT                   0
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0__TPSTH__WIDTH                   6
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0__TPSTH__MASK          0x0000003fU
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0__TPSTH__READ(src) \
					((uint32_t)(src)\
					& 0x0000003fU)
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0__TPSTH__WRITE(src) \
					((uint32_t)(src)\
					& 0x0000003fU)
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0__TPSTH__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x0000003fU) | ((uint32_t)(src) &\
					0x0000003fU)
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0__TPSTH__VERIFY(src) \
					(!(((uint32_t)(src)\
					& ~0x0000003fU)))

/* macros for field tCDQSS */
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0__TCDQSS__SHIFT                  8
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0__TCDQSS__WIDTH                  6
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0__TCDQSS__MASK         0x00003f00U
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0__TCDQSS__READ(src) \
					(((uint32_t)(src)\
					& 0x00003f00U) >> 8)
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0__TCDQSS__WRITE(src) \
					(((uint32_t)(src)\
					<< 8) & 0x00003f00U)
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0__TCDQSS__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00003f00U) | (((uint32_t)(src) <<\
					8) & 0x00003f00U)
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0__TCDQSS__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 8) & ~0x00003f00U)))

/* macros for field tPRE */
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0__TPRE__SHIFT                   16
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0__TPRE__WIDTH                    6
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0__TPRE__MASK           0x003f0000U
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0__TPRE__READ(src) \
					(((uint32_t)(src)\
					& 0x003f0000U) >> 16)
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0__TPRE__WRITE(src) \
					(((uint32_t)(src)\
					<< 16) & 0x003f0000U)
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0__TPRE__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x003f0000U) | (((uint32_t)(src) <<\
					16) & 0x003f0000U)
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0__TPRE__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 16) & ~0x003f0000U)))

/* macros for field tCR */
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0__TCR__SHIFT                    24
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0__TCR__WIDTH                     6
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0__TCR__MASK            0x3f000000U
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0__TCR__READ(src) \
					(((uint32_t)(src)\
					& 0x3f000000U) >> 24)
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0__TCR__WRITE(src) \
					(((uint32_t)(src)\
					<< 24) & 0x3f000000U)
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0__TCR__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x3f000000U) | (((uint32_t)(src) <<\
					24) & 0x3f000000U)
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0__TCR__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 24) & ~0x3f000000U)))
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0__TYPE                    uint32_t
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0__READ                 0x3f3f3f3fU
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0__WRITE                0x3f3f3f3fU

#endif /* __RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_0_MACRO__ */


/* macros for minictrl_settings.toggle_timings_0 */
#define INST_MINICTRL_SETTINGS__TOGGLE_TIMINGS_0__NUM                         1

/* macros for BlueprintGlobalNameSpace::rf_minictrl_regs::rt_toggle_timings_1 */
#ifndef __RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1_MACRO__
#define __RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1_MACRO__

/* macros for field tWPST */
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1__TWPST__SHIFT                   0
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1__TWPST__WIDTH                   6
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1__TWPST__MASK          0x0000003fU
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1__TWPST__READ(src) \
					((uint32_t)(src)\
					& 0x0000003fU)
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1__TWPST__WRITE(src) \
					((uint32_t)(src)\
					& 0x0000003fU)
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1__TWPST__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x0000003fU) | ((uint32_t)(src) &\
					0x0000003fU)
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1__TWPST__VERIFY(src) \
					(!(((uint32_t)(src)\
					& ~0x0000003fU)))

/* macros for field tRPST */
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1__TRPST__SHIFT                   8
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1__TRPST__WIDTH                   6
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1__TRPST__MASK          0x00003f00U
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1__TRPST__READ(src) \
					(((uint32_t)(src)\
					& 0x00003f00U) >> 8)
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1__TRPST__WRITE(src) \
					(((uint32_t)(src)\
					<< 8) & 0x00003f00U)
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1__TRPST__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00003f00U) | (((uint32_t)(src) <<\
					8) & 0x00003f00U)
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1__TRPST__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 8) & ~0x00003f00U)))

/* macros for field tCRES */
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1__TCRES__SHIFT                  16
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1__TCRES__WIDTH                   6
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1__TCRES__MASK          0x003f0000U
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1__TCRES__READ(src) \
					(((uint32_t)(src)\
					& 0x003f0000U) >> 16)
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1__TCRES__WRITE(src) \
					(((uint32_t)(src)\
					<< 16) & 0x003f0000U)
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1__TCRES__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x003f0000U) | (((uint32_t)(src) <<\
					16) & 0x003f0000U)
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1__TCRES__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 16) & ~0x003f0000U)))

/* macros for field tCDQSH */
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1__TCDQSH__SHIFT                 24
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1__TCDQSH__WIDTH                  6
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1__TCDQSH__MASK         0x3f000000U
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1__TCDQSH__READ(src) \
					(((uint32_t)(src)\
					& 0x3f000000U) >> 24)
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1__TCDQSH__WRITE(src) \
					(((uint32_t)(src)\
					<< 24) & 0x3f000000U)
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1__TCDQSH__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x3f000000U) | (((uint32_t)(src) <<\
					24) & 0x3f000000U)
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1__TCDQSH__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 24) & ~0x3f000000U)))
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1__TYPE                    uint32_t
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1__READ                 0x3f3f3f3fU
#define RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1__WRITE                0x3f3f3f3fU

#endif /* __RF_MINICTRL_REGS__RT_TOGGLE_TIMINGS_1_MACRO__ */


/* macros for minictrl_settings.toggle_timings_1 */
#define INST_MINICTRL_SETTINGS__TOGGLE_TIMINGS_1__NUM                         1

/* macros for BlueprintGlobalNameSpace::rf_minictrl_regs::rt_async_toggle_timings */
#ifndef __RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS_MACRO__
#define __RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS_MACRO__

/* macros for field tWP */
#define RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS__TWP__SHIFT                 0
#define RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS__TWP__WIDTH                 5
#define RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS__TWP__MASK        0x0000001fU
#define RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS__TWP__READ(src) \
					((uint32_t)(src)\
					& 0x0000001fU)
#define RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS__TWP__WRITE(src) \
					((uint32_t)(src)\
					& 0x0000001fU)
#define RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS__TWP__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x0000001fU) | ((uint32_t)(src) &\
					0x0000001fU)
#define RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS__TWP__VERIFY(src) \
					(!(((uint32_t)(src)\
					& ~0x0000001fU)))

/* macros for field tWH */
#define RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS__TWH__SHIFT                 8
#define RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS__TWH__WIDTH                 5
#define RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS__TWH__MASK        0x00001f00U
#define RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS__TWH__READ(src) \
					(((uint32_t)(src)\
					& 0x00001f00U) >> 8)
#define RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS__TWH__WRITE(src) \
					(((uint32_t)(src)\
					<< 8) & 0x00001f00U)
#define RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS__TWH__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00001f00U) | (((uint32_t)(src) <<\
					8) & 0x00001f00U)
#define RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS__TWH__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 8) & ~0x00001f00U)))

/* macros for field tRP */
#define RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS__TRP__SHIFT                16
#define RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS__TRP__WIDTH                 5
#define RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS__TRP__MASK        0x001f0000U
#define RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS__TRP__READ(src) \
					(((uint32_t)(src)\
					& 0x001f0000U) >> 16)
#define RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS__TRP__WRITE(src) \
					(((uint32_t)(src)\
					<< 16) & 0x001f0000U)
#define RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS__TRP__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x001f0000U) | (((uint32_t)(src) <<\
					16) & 0x001f0000U)
#define RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS__TRP__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 16) & ~0x001f0000U)))

/* macros for field tRH */
#define RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS__TRH__SHIFT                24
#define RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS__TRH__WIDTH                 5
#define RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS__TRH__MASK        0x1f000000U
#define RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS__TRH__READ(src) \
					(((uint32_t)(src)\
					& 0x1f000000U) >> 24)
#define RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS__TRH__WRITE(src) \
					(((uint32_t)(src)\
					<< 24) & 0x1f000000U)
#define RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS__TRH__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x1f000000U) | (((uint32_t)(src) <<\
					24) & 0x1f000000U)
#define RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS__TRH__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 24) & ~0x1f000000U)))
#define RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS__TYPE                uint32_t
#define RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS__READ             0x1f1f1f1fU
#define RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS__WRITE            0x1f1f1f1fU

#endif /* __RF_MINICTRL_REGS__RT_ASYNC_TOGGLE_TIMINGS_MACRO__ */


/* macros for minictrl_settings.async_toggle_timings */
#define INST_MINICTRL_SETTINGS__ASYNC_TOGGLE_TIMINGS__NUM                     1

/* macros for BlueprintGlobalNameSpace::rf_minictrl_regs::rt_sync_timings */
#ifndef __RF_MINICTRL_REGS__RT_SYNC_TIMINGS_MACRO__
#define __RF_MINICTRL_REGS__RT_SYNC_TIMINGS_MACRO__

/* macros for field tCAD */
#define RF_MINICTRL_REGS__RT_SYNC_TIMINGS__TCAD__SHIFT                        0
#define RF_MINICTRL_REGS__RT_SYNC_TIMINGS__TCAD__WIDTH                        6
#define RF_MINICTRL_REGS__RT_SYNC_TIMINGS__TCAD__MASK               0x0000003fU
#define RF_MINICTRL_REGS__RT_SYNC_TIMINGS__TCAD__READ(src) \
					((uint32_t)(src)\
					& 0x0000003fU)
#define RF_MINICTRL_REGS__RT_SYNC_TIMINGS__TCAD__WRITE(src) \
					((uint32_t)(src)\
					& 0x0000003fU)
#define RF_MINICTRL_REGS__RT_SYNC_TIMINGS__TCAD__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x0000003fU) | ((uint32_t)(src) &\
					0x0000003fU)
#define RF_MINICTRL_REGS__RT_SYNC_TIMINGS__TCAD__VERIFY(src) \
					(!(((uint32_t)(src)\
					& ~0x0000003fU)))

/* macros for field tWRCK */
#define RF_MINICTRL_REGS__RT_SYNC_TIMINGS__TWRCK__SHIFT                       8
#define RF_MINICTRL_REGS__RT_SYNC_TIMINGS__TWRCK__WIDTH                       6
#define RF_MINICTRL_REGS__RT_SYNC_TIMINGS__TWRCK__MASK              0x00003f00U
#define RF_MINICTRL_REGS__RT_SYNC_TIMINGS__TWRCK__READ(src) \
					(((uint32_t)(src)\
					& 0x00003f00U) >> 8)
#define RF_MINICTRL_REGS__RT_SYNC_TIMINGS__TWRCK__WRITE(src) \
					(((uint32_t)(src)\
					<< 8) & 0x00003f00U)
#define RF_MINICTRL_REGS__RT_SYNC_TIMINGS__TWRCK__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00003f00U) | (((uint32_t)(src) <<\
					8) & 0x00003f00U)
#define RF_MINICTRL_REGS__RT_SYNC_TIMINGS__TWRCK__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 8) & ~0x00003f00U)))

/* macros for field tCKWR */
#define RF_MINICTRL_REGS__RT_SYNC_TIMINGS__TCKWR__SHIFT                      16
#define RF_MINICTRL_REGS__RT_SYNC_TIMINGS__TCKWR__WIDTH                       6
#define RF_MINICTRL_REGS__RT_SYNC_TIMINGS__TCKWR__MASK              0x003f0000U
#define RF_MINICTRL_REGS__RT_SYNC_TIMINGS__TCKWR__READ(src) \
					(((uint32_t)(src)\
					& 0x003f0000U) >> 16)
#define RF_MINICTRL_REGS__RT_SYNC_TIMINGS__TCKWR__WRITE(src) \
					(((uint32_t)(src)\
					<< 16) & 0x003f0000U)
#define RF_MINICTRL_REGS__RT_SYNC_TIMINGS__TCKWR__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x003f0000U) | (((uint32_t)(src) <<\
					16) & 0x003f0000U)
#define RF_MINICTRL_REGS__RT_SYNC_TIMINGS__TCKWR__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 16) & ~0x003f0000U)))
#define RF_MINICTRL_REGS__RT_SYNC_TIMINGS__TYPE                        uint32_t
#define RF_MINICTRL_REGS__RT_SYNC_TIMINGS__READ                     0x003f3f3fU
#define RF_MINICTRL_REGS__RT_SYNC_TIMINGS__WRITE                    0x003f3f3fU

#endif /* __RF_MINICTRL_REGS__RT_SYNC_TIMINGS_MACRO__ */


/* macros for minictrl_settings.sync_timings */
#define INST_MINICTRL_SETTINGS__SYNC_TIMINGS__NUM                             1

/* macros for BlueprintGlobalNameSpace::rf_minictrl_regs::rt_timings0 */
#ifndef __RF_MINICTRL_REGS__RT_TIMINGS0_MACRO__
#define __RF_MINICTRL_REGS__RT_TIMINGS0_MACRO__

/* macros for field tRHW */
#define RF_MINICTRL_REGS__RT_TIMINGS0__TRHW__SHIFT                            0
#define RF_MINICTRL_REGS__RT_TIMINGS0__TRHW__WIDTH                            8
#define RF_MINICTRL_REGS__RT_TIMINGS0__TRHW__MASK                   0x000000ffU
#define RF_MINICTRL_REGS__RT_TIMINGS0__TRHW__READ(src) \
					((uint32_t)(src)\
					& 0x000000ffU)
#define RF_MINICTRL_REGS__RT_TIMINGS0__TRHW__WRITE(src) \
					((uint32_t)(src)\
					& 0x000000ffU)
#define RF_MINICTRL_REGS__RT_TIMINGS0__TRHW__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x000000ffU) | ((uint32_t)(src) &\
					0x000000ffU)
#define RF_MINICTRL_REGS__RT_TIMINGS0__TRHW__VERIFY(src) \
					(!(((uint32_t)(src)\
					& ~0x000000ffU)))

/* macros for field tWHR */
#define RF_MINICTRL_REGS__RT_TIMINGS0__TWHR__SHIFT                            8
#define RF_MINICTRL_REGS__RT_TIMINGS0__TWHR__WIDTH                            8
#define RF_MINICTRL_REGS__RT_TIMINGS0__TWHR__MASK                   0x0000ff00U
#define RF_MINICTRL_REGS__RT_TIMINGS0__TWHR__READ(src) \
					(((uint32_t)(src)\
					& 0x0000ff00U) >> 8)
#define RF_MINICTRL_REGS__RT_TIMINGS0__TWHR__WRITE(src) \
					(((uint32_t)(src)\
					<< 8) & 0x0000ff00U)
#define RF_MINICTRL_REGS__RT_TIMINGS0__TWHR__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x0000ff00U) | (((uint32_t)(src) <<\
					8) & 0x0000ff00U)
#define RF_MINICTRL_REGS__RT_TIMINGS0__TWHR__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 8) & ~0x0000ff00U)))

/* macros for field tCCS */
#define RF_MINICTRL_REGS__RT_TIMINGS0__TCCS__SHIFT                           16
#define RF_MINICTRL_REGS__RT_TIMINGS0__TCCS__WIDTH                            8
#define RF_MINICTRL_REGS__RT_TIMINGS0__TCCS__MASK                   0x00ff0000U
#define RF_MINICTRL_REGS__RT_TIMINGS0__TCCS__READ(src) \
					(((uint32_t)(src)\
					& 0x00ff0000U) >> 16)
#define RF_MINICTRL_REGS__RT_TIMINGS0__TCCS__WRITE(src) \
					(((uint32_t)(src)\
					<< 16) & 0x00ff0000U)
#define RF_MINICTRL_REGS__RT_TIMINGS0__TCCS__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00ff0000U) | (((uint32_t)(src) <<\
					16) & 0x00ff0000U)
#define RF_MINICTRL_REGS__RT_TIMINGS0__TCCS__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 16) & ~0x00ff0000U)))

/* macros for field tADL */
#define RF_MINICTRL_REGS__RT_TIMINGS0__TADL__SHIFT                           24
#define RF_MINICTRL_REGS__RT_TIMINGS0__TADL__WIDTH                            8
#define RF_MINICTRL_REGS__RT_TIMINGS0__TADL__MASK                   0xff000000U
#define RF_MINICTRL_REGS__RT_TIMINGS0__TADL__READ(src) \
					(((uint32_t)(src)\
					& 0xff000000U) >> 24)
#define RF_MINICTRL_REGS__RT_TIMINGS0__TADL__WRITE(src) \
					(((uint32_t)(src)\
					<< 24) & 0xff000000U)
#define RF_MINICTRL_REGS__RT_TIMINGS0__TADL__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0xff000000U) | (((uint32_t)(src) <<\
					24) & 0xff000000U)
#define RF_MINICTRL_REGS__RT_TIMINGS0__TADL__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 24) & ~0xff000000U)))
#define RF_MINICTRL_REGS__RT_TIMINGS0__TYPE                            uint32_t
#define RF_MINICTRL_REGS__RT_TIMINGS0__READ                         0xffffffffU
#define RF_MINICTRL_REGS__RT_TIMINGS0__WRITE                        0xffffffffU

#endif /* __RF_MINICTRL_REGS__RT_TIMINGS0_MACRO__ */


/* macros for minictrl_settings.timings0 */
#define INST_MINICTRL_SETTINGS__TIMINGS0__NUM                                 1

/* macros for BlueprintGlobalNameSpace::rf_minictrl_regs::rt_timings1 */
#ifndef __RF_MINICTRL_REGS__RT_TIMINGS1_MACRO__
#define __RF_MINICTRL_REGS__RT_TIMINGS1_MACRO__

/* macros for field tVDLY */
#define RF_MINICTRL_REGS__RT_TIMINGS1__TVDLY__SHIFT                           0
#define RF_MINICTRL_REGS__RT_TIMINGS1__TVDLY__WIDTH                           8
#define RF_MINICTRL_REGS__RT_TIMINGS1__TVDLY__MASK                  0x000000ffU
#define RF_MINICTRL_REGS__RT_TIMINGS1__TVDLY__READ(src) \
					((uint32_t)(src)\
					& 0x000000ffU)
#define RF_MINICTRL_REGS__RT_TIMINGS1__TVDLY__WRITE(src) \
					((uint32_t)(src)\
					& 0x000000ffU)
#define RF_MINICTRL_REGS__RT_TIMINGS1__TVDLY__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x000000ffU) | ((uint32_t)(src) &\
					0x000000ffU)
#define RF_MINICTRL_REGS__RT_TIMINGS1__TVDLY__VERIFY(src) \
					(!(((uint32_t)(src)\
					& ~0x000000ffU)))

/* macros for field tCWAW */
#define RF_MINICTRL_REGS__RT_TIMINGS1__TCWAW__SHIFT                           8
#define RF_MINICTRL_REGS__RT_TIMINGS1__TCWAW__WIDTH                           8
#define RF_MINICTRL_REGS__RT_TIMINGS1__TCWAW__MASK                  0x0000ff00U
#define RF_MINICTRL_REGS__RT_TIMINGS1__TCWAW__READ(src) \
					(((uint32_t)(src)\
					& 0x0000ff00U) >> 8)
#define RF_MINICTRL_REGS__RT_TIMINGS1__TCWAW__WRITE(src) \
					(((uint32_t)(src)\
					<< 8) & 0x0000ff00U)
#define RF_MINICTRL_REGS__RT_TIMINGS1__TCWAW__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x0000ff00U) | (((uint32_t)(src) <<\
					8) & 0x0000ff00U)
#define RF_MINICTRL_REGS__RT_TIMINGS1__TCWAW__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 8) & ~0x0000ff00U)))

/* macros for field tWB */
#define RF_MINICTRL_REGS__RT_TIMINGS1__TWB__SHIFT                            16
#define RF_MINICTRL_REGS__RT_TIMINGS1__TWB__WIDTH                             8
#define RF_MINICTRL_REGS__RT_TIMINGS1__TWB__MASK                    0x00ff0000U
#define RF_MINICTRL_REGS__RT_TIMINGS1__TWB__READ(src) \
					(((uint32_t)(src)\
					& 0x00ff0000U) >> 16)
#define RF_MINICTRL_REGS__RT_TIMINGS1__TWB__WRITE(src) \
					(((uint32_t)(src)\
					<< 16) & 0x00ff0000U)
#define RF_MINICTRL_REGS__RT_TIMINGS1__TWB__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00ff0000U) | (((uint32_t)(src) <<\
					16) & 0x00ff0000U)
#define RF_MINICTRL_REGS__RT_TIMINGS1__TWB__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 16) & ~0x00ff0000U)))

/* macros for field tRHZ */
#define RF_MINICTRL_REGS__RT_TIMINGS1__TRHZ__SHIFT                           24
#define RF_MINICTRL_REGS__RT_TIMINGS1__TRHZ__WIDTH                            8
#define RF_MINICTRL_REGS__RT_TIMINGS1__TRHZ__MASK                   0xff000000U
#define RF_MINICTRL_REGS__RT_TIMINGS1__TRHZ__READ(src) \
					(((uint32_t)(src)\
					& 0xff000000U) >> 24)
#define RF_MINICTRL_REGS__RT_TIMINGS1__TRHZ__WRITE(src) \
					(((uint32_t)(src)\
					<< 24) & 0xff000000U)
#define RF_MINICTRL_REGS__RT_TIMINGS1__TRHZ__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0xff000000U) | (((uint32_t)(src) <<\
					24) & 0xff000000U)
#define RF_MINICTRL_REGS__RT_TIMINGS1__TRHZ__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 24) & ~0xff000000U)))
#define RF_MINICTRL_REGS__RT_TIMINGS1__TYPE                            uint32_t
#define RF_MINICTRL_REGS__RT_TIMINGS1__READ                         0xffffffffU
#define RF_MINICTRL_REGS__RT_TIMINGS1__WRITE                        0xffffffffU

#endif /* __RF_MINICTRL_REGS__RT_TIMINGS1_MACRO__ */


/* macros for minictrl_settings.timings1 */
#define INST_MINICTRL_SETTINGS__TIMINGS1__NUM                                 1

/* macros for BlueprintGlobalNameSpace::rf_minictrl_regs::rt_timings2 */
#ifndef __RF_MINICTRL_REGS__RT_TIMINGS2_MACRO__
#define __RF_MINICTRL_REGS__RT_TIMINGS2_MACRO__

/* macros for field CS_setup_time */
#define RF_MINICTRL_REGS__RT_TIMINGS2__CS_SETUP_TIME__SHIFT                   0
#define RF_MINICTRL_REGS__RT_TIMINGS2__CS_SETUP_TIME__WIDTH                   6
#define RF_MINICTRL_REGS__RT_TIMINGS2__CS_SETUP_TIME__MASK          0x0000003fU
#define RF_MINICTRL_REGS__RT_TIMINGS2__CS_SETUP_TIME__READ(src) \
					((uint32_t)(src)\
					& 0x0000003fU)
#define RF_MINICTRL_REGS__RT_TIMINGS2__CS_SETUP_TIME__WRITE(src) \
					((uint32_t)(src)\
					& 0x0000003fU)
#define RF_MINICTRL_REGS__RT_TIMINGS2__CS_SETUP_TIME__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x0000003fU) | ((uint32_t)(src) &\
					0x0000003fU)
#define RF_MINICTRL_REGS__RT_TIMINGS2__CS_SETUP_TIME__VERIFY(src) \
					(!(((uint32_t)(src)\
					& ~0x0000003fU)))

/* macros for field CS_hold_time */
#define RF_MINICTRL_REGS__RT_TIMINGS2__CS_HOLD_TIME__SHIFT                    8
#define RF_MINICTRL_REGS__RT_TIMINGS2__CS_HOLD_TIME__WIDTH                    6
#define RF_MINICTRL_REGS__RT_TIMINGS2__CS_HOLD_TIME__MASK           0x00003f00U
#define RF_MINICTRL_REGS__RT_TIMINGS2__CS_HOLD_TIME__READ(src) \
					(((uint32_t)(src)\
					& 0x00003f00U) >> 8)
#define RF_MINICTRL_REGS__RT_TIMINGS2__CS_HOLD_TIME__WRITE(src) \
					(((uint32_t)(src)\
					<< 8) & 0x00003f00U)
#define RF_MINICTRL_REGS__RT_TIMINGS2__CS_HOLD_TIME__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00003f00U) | (((uint32_t)(src) <<\
					8) & 0x00003f00U)
#define RF_MINICTRL_REGS__RT_TIMINGS2__CS_HOLD_TIME__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 8) & ~0x00003f00U)))

/* macros for field tFEAT */
#define RF_MINICTRL_REGS__RT_TIMINGS2__TFEAT__SHIFT                          16
#define RF_MINICTRL_REGS__RT_TIMINGS2__TFEAT__WIDTH                          10
#define RF_MINICTRL_REGS__RT_TIMINGS2__TFEAT__MASK                  0x03ff0000U
#define RF_MINICTRL_REGS__RT_TIMINGS2__TFEAT__READ(src) \
					(((uint32_t)(src)\
					& 0x03ff0000U) >> 16)
#define RF_MINICTRL_REGS__RT_TIMINGS2__TFEAT__WRITE(src) \
					(((uint32_t)(src)\
					<< 16) & 0x03ff0000U)
#define RF_MINICTRL_REGS__RT_TIMINGS2__TFEAT__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x03ff0000U) | (((uint32_t)(src) <<\
					16) & 0x03ff0000U)
#define RF_MINICTRL_REGS__RT_TIMINGS2__TFEAT__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 16) & ~0x03ff0000U)))
#define RF_MINICTRL_REGS__RT_TIMINGS2__TYPE                            uint32_t
#define RF_MINICTRL_REGS__RT_TIMINGS2__READ                         0x03ff3f3fU
#define RF_MINICTRL_REGS__RT_TIMINGS2__WRITE                        0x03ff3f3fU

#endif /* __RF_MINICTRL_REGS__RT_TIMINGS2_MACRO__ */


/* macros for minictrl_settings.timings2 */
#define INST_MINICTRL_SETTINGS__TIMINGS2__NUM                                 1

/* macros for BlueprintGlobalNameSpace::rf_minictrl_regs::rt_dll_phy_update_cnt */
#ifndef __RF_MINICTRL_REGS__RT_DLL_PHY_UPDATE_CNT_MACRO__
#define __RF_MINICTRL_REGS__RT_DLL_PHY_UPDATE_CNT_MACRO__

/* macros for field resync_cnt */
#define RF_MINICTRL_REGS__RT_DLL_PHY_UPDATE_CNT__RESYNC_CNT__SHIFT            0
#define RF_MINICTRL_REGS__RT_DLL_PHY_UPDATE_CNT__RESYNC_CNT__WIDTH           32
#define RF_MINICTRL_REGS__RT_DLL_PHY_UPDATE_CNT__RESYNC_CNT__MASK   0xffffffffU
#define RF_MINICTRL_REGS__RT_DLL_PHY_UPDATE_CNT__RESYNC_CNT__READ(src) \
					((uint32_t)(src)\
					& 0xffffffffU)
#define RF_MINICTRL_REGS__RT_DLL_PHY_UPDATE_CNT__RESYNC_CNT__WRITE(src) \
					((uint32_t)(src)\
					& 0xffffffffU)
#define RF_MINICTRL_REGS__RT_DLL_PHY_UPDATE_CNT__RESYNC_CNT__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0xffffffffU) | ((uint32_t)(src) &\
					0xffffffffU)
#define RF_MINICTRL_REGS__RT_DLL_PHY_UPDATE_CNT__RESYNC_CNT__VERIFY(src) \
					(!(((uint32_t)(src)\
					& ~0xffffffffU)))
#define RF_MINICTRL_REGS__RT_DLL_PHY_UPDATE_CNT__TYPE                  uint32_t
#define RF_MINICTRL_REGS__RT_DLL_PHY_UPDATE_CNT__READ               0xffffffffU
#define RF_MINICTRL_REGS__RT_DLL_PHY_UPDATE_CNT__WRITE              0xffffffffU

#endif /* __RF_MINICTRL_REGS__RT_DLL_PHY_UPDATE_CNT_MACRO__ */


/* macros for minictrl_settings.dll_phy_update_cnt */
#define INST_MINICTRL_SETTINGS__DLL_PHY_UPDATE_CNT__NUM                       1

/* macros for BlueprintGlobalNameSpace::rf_minictrl_regs::rt_dll_phy_ctrl */
#ifndef __RF_MINICTRL_REGS__RT_DLL_PHY_CTRL_MACRO__
#define __RF_MINICTRL_REGS__RT_DLL_PHY_CTRL_MACRO__

/* macros for field resync_idle_cnt */
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__RESYNC_IDLE_CNT__SHIFT             0
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__RESYNC_IDLE_CNT__WIDTH             8
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__RESYNC_IDLE_CNT__MASK    0x000000ffU
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__RESYNC_IDLE_CNT__READ(src) \
					((uint32_t)(src)\
					& 0x000000ffU)
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__RESYNC_IDLE_CNT__WRITE(src) \
					((uint32_t)(src)\
					& 0x000000ffU)
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__RESYNC_IDLE_CNT__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x000000ffU) | ((uint32_t)(src) &\
					0x000000ffU)
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__RESYNC_IDLE_CNT__VERIFY(src) \
					(!(((uint32_t)(src)\
					& ~0x000000ffU)))

/* macros for field resync_high_wait_cnt */
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__RESYNC_HIGH_WAIT_CNT__SHIFT        8
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__RESYNC_HIGH_WAIT_CNT__WIDTH        4
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__RESYNC_HIGH_WAIT_CNT__MASK \
					0x00000f00U
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__RESYNC_HIGH_WAIT_CNT__READ(src) \
					(((uint32_t)(src)\
					& 0x00000f00U) >> 8)
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__RESYNC_HIGH_WAIT_CNT__WRITE(src) \
					(((uint32_t)(src)\
					<< 8) & 0x00000f00U)
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__RESYNC_HIGH_WAIT_CNT__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00000f00U) | (((uint32_t)(src) <<\
					8) & 0x00000f00U)
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__RESYNC_HIGH_WAIT_CNT__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 8) & ~0x00000f00U)))

/* macros for field extended_rd_mode */
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__EXTENDED_RD_MODE__SHIFT           16
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__EXTENDED_RD_MODE__WIDTH            1
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__EXTENDED_RD_MODE__MASK   0x00010000U
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__EXTENDED_RD_MODE__READ(src) \
					(((uint32_t)(src)\
					& 0x00010000U) >> 16)
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__EXTENDED_RD_MODE__WRITE(src) \
					(((uint32_t)(src)\
					<< 16) & 0x00010000U)
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__EXTENDED_RD_MODE__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00010000U) | (((uint32_t)(src) <<\
					16) & 0x00010000U)
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__EXTENDED_RD_MODE__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 16) & ~0x00010000U)))
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__EXTENDED_RD_MODE__SET(dst) \
					(dst) = ((dst) &\
					~0x00010000U) | ((uint32_t)(1) << 16)
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__EXTENDED_RD_MODE__CLR(dst) \
					(dst) = ((dst) &\
					~0x00010000U) | ((uint32_t)(0) << 16)

/* macros for field extended_wr_mode */
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__EXTENDED_WR_MODE__SHIFT           17
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__EXTENDED_WR_MODE__WIDTH            1
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__EXTENDED_WR_MODE__MASK   0x00020000U
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__EXTENDED_WR_MODE__READ(src) \
					(((uint32_t)(src)\
					& 0x00020000U) >> 17)
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__EXTENDED_WR_MODE__WRITE(src) \
					(((uint32_t)(src)\
					<< 17) & 0x00020000U)
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__EXTENDED_WR_MODE__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x00020000U) | (((uint32_t)(src) <<\
					17) & 0x00020000U)
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__EXTENDED_WR_MODE__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 17) & ~0x00020000U)))
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__EXTENDED_WR_MODE__SET(dst) \
					(dst) = ((dst) &\
					~0x00020000U) | ((uint32_t)(1) << 17)
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__EXTENDED_WR_MODE__CLR(dst) \
					(dst) = ((dst) &\
					~0x00020000U) | ((uint32_t)(0) << 17)

/* macros for field dll_rst_n */
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__DLL_RST_N__SHIFT                  24
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__DLL_RST_N__WIDTH                   1
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__DLL_RST_N__MASK          0x01000000U
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__DLL_RST_N__READ(src) \
					(((uint32_t)(src)\
					& 0x01000000U) >> 24)
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__DLL_RST_N__WRITE(src) \
					(((uint32_t)(src)\
					<< 24) & 0x01000000U)
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__DLL_RST_N__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x01000000U) | (((uint32_t)(src) <<\
					24) & 0x01000000U)
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__DLL_RST_N__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 24) & ~0x01000000U)))
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__DLL_RST_N__SET(dst) \
					(dst) = ((dst) &\
					~0x01000000U) | ((uint32_t)(1) << 24)
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__DLL_RST_N__CLR(dst) \
					(dst) = ((dst) &\
					~0x01000000U) | ((uint32_t)(0) << 24)

/* macros for field dfi_ctrlupd_req */
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__DFI_CTRLUPD_REQ__SHIFT            25
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__DFI_CTRLUPD_REQ__WIDTH             1
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__DFI_CTRLUPD_REQ__MASK    0x02000000U
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__DFI_CTRLUPD_REQ__READ(src) \
					(((uint32_t)(src)\
					& 0x02000000U) >> 25)
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__DFI_CTRLUPD_REQ__WRITE(src) \
					(((uint32_t)(src)\
					<< 25) & 0x02000000U)
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__DFI_CTRLUPD_REQ__MODIFY(dst, src) \
					(dst) = ((dst) &\
					~0x02000000U) | (((uint32_t)(src) <<\
					25) & 0x02000000U)
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__DFI_CTRLUPD_REQ__VERIFY(src) \
					(!((((uint32_t)(src)\
					<< 25) & ~0x02000000U)))
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__DFI_CTRLUPD_REQ__SET(dst) \
					(dst) = ((dst) &\
					~0x02000000U) | ((uint32_t)(1) << 25)
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__DFI_CTRLUPD_REQ__CLR(dst) \
					(dst) = ((dst) &\
					~0x02000000U) | ((uint32_t)(0) << 25)
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__TYPE                        uint32_t
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__READ                     0x03030fffU
#define RF_MINICTRL_REGS__RT_DLL_PHY_CTRL__WRITE                    0x03030fffU

#endif /* __RF_MINICTRL_REGS__RT_DLL_PHY_CTRL_MACRO__ */


/* macros for minictrl_settings.dll_phy_ctrl */
#define INST_MINICTRL_SETTINGS__DLL_PHY_CTRL__NUM                             1
#define RFILE_INST_MINICTRL_SETTINGS__NUM                                     1

#endif /* __REG_NFC9_MINICTRL_REGS_MACRO_H__ */
