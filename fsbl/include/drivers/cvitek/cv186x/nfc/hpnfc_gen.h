 /*
 * Copyright (c) 2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef HPNFC_GEN_H
#define HPNFC_GEN_H

#include <hpnfc_if.h>



#define  GEN_SEQ_CMD_WRITE(cmd)                 ((uint64_t)(cmd & 0xFFFF) << 16)

#define  GEN_SEQ_BYTES_COUNT_WRITE(bytes)       ((bytes & 0x7) << 11)

#define  GEN_SEQ_ADDR0_SHIFT                    16
#define  GEN_SEQ_ADDR0_MASK                     (0xFFuLL << 16)
#define  GEN_SEQ_ADDR1_SHIFT                    24
#define  GEN_SEQ_ADDR1_MASK                     (0xFFuLL << 24)
#define  GEN_SEQ_ADDR2_SHIFT                    32
#define  GEN_SEQ_ADDR2_MASK                     (0xFFuLL << 32)
#define  GEN_SEQ_ADDR3_SHIFT                    40
#define  GEN_SEQ_ADDR3_MASK                     (0xFFuLL << 40)
#define  GEN_SEQ_ADDR4_SHIFT                    48
#define  GEN_SEQ_ADDR4_MASK                     (0xFFuLL << 48)
#define  GEN_SEQ_ADDR5_SHIFT                    56
#define  GEN_SEQ_ADDR5_MASK                     (0xFFuLL << 56)
#define  GEN_SEQ_ADDR_WRITE(addr, bytes)        (((uint64_t)addr & (0x0000FFFFFFFFFFFFuLL >> ((6 - bytes) * 8))) << 16)


#define  GEN_SEQ_DIR_READ_MASK                  (0x0uLL << 11)
#define  GEN_SEQ_DIR_WRITE_MASK                 (0x1uLL << 11)
#define  GEN_SEQ_ECC_ENABLE_MASK                (0x1uLL << 12)
#define  GEN_SEQ_ECC_SCR_ENABLE_MASK            (0x1uLL << 13)
#define  GEN_SEQ_ERASE_DETECT_ENABLE_MASK       (0x1uLL << 14)
#define  GEN_SEQ_SECT_SIZE_WRITE(sectSize)      ((uint64_t)(sectSize & 0xFFFF) << 16)
#define  GEN_SEQ_SECT_COUNT_WRITE(sectCount)    ((uint64_t)(sectCount & 0xFF) << 32)
#define  GEN_SEQ_LAST_SECT_SIZE_WRITE(lSecSize) ((uint64_t)(lSecSize & 0xFFFF) << 40)
#define  GEN_SEQ_CORR_CAP_WRITE(corrCap)        ((uint64_t)(corrCap & 0x7) << 56)

#define  GEN_SEQ_DATA_WRITE(data)               ((uint64_t)(data & 0xFFFFFFFF) << 32)

#define  GEN_SEQ_STATUS_F2_ENABLE_WRITE(f2En)   ((uint64_t)(f2En & 0x1) << 15)

#define  GEN_SEQ_SEED_VALUE_WRITE(val)          ((uint64_t)(val & 0xFFFFFF) << 32)



#define HPNFC_GEN_INSTR_CMD_SEQ                 0
#define HPNFC_GEN_INSTR_ADDR_SEQ                1
#define HPNFC_GEN_INSTR_DATA_SEQ                2
#define HPNFC_GEN_INSTR_READ                    3
#define HPNFC_GEN_INSTR_WRITE                   4
#define HPNFC_GEN_INSTR_RESET                   5
#define HPNFC_GEN_INSTR_ERASE                   6
#define HPNFC_GEN_INSTR_STATUS                  7
#define HPNFC_GEN_INSTR_ENHANCED_STATUS         8
#define HPNFC_GEN_INSTR_READ_CACHE              9
#define HPNFC_GEN_INSTR_COPYBACK_READ           10
#define HPNFC_GEN_INSTR_COPYBACK_WRITE          11
#define HPNFC_GEN_INSTR_READ_COLUMN             12
#define HPNFC_GEN_INSTR_READ_COLUMN_ENHANCED    13
#define HPNFC_GEN_INSTR_READ_COLUMN_JEDEC       14
#define HPNFC_GEN_INSTR_MULTIPLANE_READ         15
#define HPNFC_GEN_INSTR_MULTIPLANE_ERASE        16
#define HPNFC_GEN_INSTR_MULTIPLANE_ERASE2       17
#define HPNFC_GEN_INSTR_WRITE_COLUMN            18
#define HPNFC_GEN_INSTR_SMALL_DATA_MOVE         19
#define HPNFC_GEN_INSTR_SYNCHRONOUS_RESET       20
#define HPNFC_GEN_INSTR_VOLUME_SELECT           21
#define HPNFC_GEN_INSTR_ODT_CONFIGURE           22
#define HPNFC_GEN_INSTR_SET_FEATURES            23
#define HPNFC_GEN_INSTR_GET_FEATURES            24
#define HPNFC_GEN_INSTR_LUN_GET_FEATURES        25
#define HPNFC_GEN_INSTR_LUN_SET_FEATURES        26
#define HPNFC_GEN_INSTR_READ_ID                 27
#define HPNFC_GEN_INSTR_READ_PARAM_PAGE         28
#define HPNFC_GEN_INSTR_ZQ_CALIB_SHORT          29
#define HPNFC_GEN_INSTR_ZQ_CALIB_LONG           30
#define HPNFC_GEN_INSTR_LUN_REST                31
#define HPNFC_GEN_INSTR_SEED_INITIAL            35

#define GEN_SEQ_CHIP_SELECT_WRITE(chipNr)       ((uint64_t)(chipNr & 0x3) << 8)
#define GEN_SEQ_JEDEC_SUPP_MASK                 (1uLL << 7)
#define GEN_SEQ_TWB_ACTIVE_MASK                 (1uLL << 6)
#define GEN_SEQ_INSTR_TYPE_WRITE(instr)         ((uint64_t)(instr & 0x3F))


#define HPNFC_GEN_CMD_REG0_THREAD_WRITE         HPNFC_CMD_REG0_THREAD_WRITE
#define HPNFC_GEN_CMD_REG0_INT_MASK             HPNFC_CMD_REG0_INT_MASK

#endif




