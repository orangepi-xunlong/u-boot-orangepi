 /*
 * Copyright (c) 2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _HAVE_DBG_LOG_

#define _HAVE_DBG_LOG_ 1

#include <stdio.h>
#include <assert.h>
#include "cdn_inttypes.h"

#ifdef DEBUG
 #if DEBUG
  //#define CFP_DBG_MSG 1
 #endif
#endif

/**
 * Modules definitions
 */
#define CLIENT_MSG         0x01000000

#define DBG_GEN_MSG        0xFFFFFFFF

/**
 * Log level:
 * DBG_CRIT - critical
 * DBG_WARN - warning
 * DBG_FYI - fyi
 * DBG_HIVERB - highly verbose
 * DBG_INFLOOP - infinite loop debug
 */
#define DBG_CRIT 0
#define DBG_WARN 5
#define DBG_FYI 10
#define DBG_HIVERB 100
#define DBG_INFLOOP 200

#define PTR_INC(base, offset) (void *)((uint8_t *)(base) + (offset))
#define PTR_DEC(base, offset) (void *)((uint8_t *)(base) - (offset))

/* module mask: */
#ifdef _HAVE_DBG_LOG_INT_
uint32_t g_dbg_enable_log  = 0;
#else
extern uint32_t g_dbg_enable_log;
#endif

/* level, counter, state: */
#ifdef _HAVE_DBG_LOG_INT_
uint32_t g_dbg_log_lvl = DBG_CRIT;
uint32_t g_dbg_log_cnt = 0;
uint32_t g_dbg_state = 0;
#else
extern uint32_t g_dbg_log_lvl;
extern uint32_t g_dbg_log_cnt;
extern uint32_t g_dbg_state;
#endif

#if 1
#define cDbgMsg( _t, _x, ...) ( printf( __VA_ARGS__))
#else
#define cDbgMsg( _t, _x, ...) ( ((_x)==  0) || \
								(((_t) & g_dbg_enable_log) && ((_x) <= g_dbg_log_lvl)) ? \
								printf( __VA_ARGS__): 0 )
#endif


#ifdef CFP_DBG_MSG
zxcv
#define DbgMsg( t, x, ...)  cDbgMsg( (t), (x), __VA_ARGS__ )
#else
#define DbgMsg( t, x, ...)
#endif

#ifdef _HAVE_DBG_LOG_INT_
#define vDbgMsg( l, m, n, ...) DbgMsg( (l), (m), "[%-20.20s %4d %4"PRId32"]-" n, __func__,\
												   __LINE__, g_dbg_log_cnt++, __VA_ARGS__)
#define cvDbgMsg( l, m, n, ...) cDbgMsg( (l), (m), "[%-20.20s %4d %4"PRId32"]-" n, __func__,\
												   __LINE__, g_dbg_log_cnt++, __VA_ARGS__)
#define evDbgMsg( l, m, n, ...) { cDbgMsg( (l), (m), "[%-20.20s %4d %4"PRId32"]-" n, __func__,         \
												   __LINE__, g_dbg_log_cnt++, __VA_ARGS__); \
								  assert(0); }
#endif

#define DbgMsgSetLvl( x ) (g_dbg_log_lvl = (x))
#define DbgMsgEnableModule( x ) (g_dbg_enable_log |= (x) )
#define DbgMsgDisableModule( x ) (g_dbg_enable_log &= ~( (uint32_t) (x) ))
#define DbgMsgClearAll( _x ) ( g_dbg_enable_log = (_x) )

#define SetDbgState( _x ) (g_dbg_state = (_x) )
#define GetDbgState       (g_dbg_state)

#endif
