//SPDX-License-Identifier:	GPL-2.0+

#ifndef __NAND_OSAL_H__
#define __NAND_OSAL_H__

//#include <common.h>

#ifndef __s8
typedef signed char __s8;
#endif
#ifndef _s8
typedef signed char _s8;
#endif
#ifndef s8
typedef signed char s8;
#endif
#ifndef S8
typedef signed char S8;
#endif
#ifndef sint8
typedef signed char sint8;
#endif
#ifndef SINT8
typedef signed char SINT8;
#endif
#ifndef int8
typedef signed char int8;
#endif
#ifndef INT8
typedef signed char INT8;
#endif
#ifndef __u8
typedef unsigned char __u8;
#endif
#ifndef _u8
typedef unsigned char _u8;
#endif
#ifndef u8
typedef unsigned char u8;
#endif
#ifndef U8
typedef unsigned char U8;
#endif
#ifndef uint8
typedef unsigned char uint8;
#endif
#ifndef UINT8
typedef unsigned char UINT8;
#endif
#ifndef uchar
typedef unsigned char uchar;
#endif
#ifndef UCHAR
typedef unsigned char UCHAR;
#endif
#ifndef __s16
typedef signed short __s16;
#endif
#ifndef _s16
typedef signed short _s16;
#endif
#ifndef s16
typedef signed short s16;
#endif
#ifndef S16
typedef signed short S16;
#endif
#ifndef sint16
typedef signed short sint16;
#endif
#ifndef int16
typedef signed short int16;
#endif
#ifndef INT16
typedef signed short INT16;
#endif
#ifndef __u16
typedef unsigned short __u16;
#endif
#ifndef _u16
typedef unsigned short _u16;
#endif
#ifndef u16
typedef unsigned short u16;
#endif
#ifndef U16
typedef unsigned short U16;
#endif
#ifndef uint16
typedef unsigned short uint16;
#endif
#ifndef UINT16
typedef unsigned short UINT16;
#endif
#ifndef __s32
typedef signed int __s32;
#endif
#ifndef _s32
typedef signed int _s32;
#endif
#ifndef s32
typedef signed int s32;
#endif
#ifndef S32
typedef signed int S32;
#endif
#ifndef int32
typedef signed int int32;
#endif
#ifndef INT32
typedef signed int INT32;
#endif
#ifndef sint32
typedef signed int sint32;
#endif
#ifndef SINT32
typedef signed int SINT32;
#endif
#ifndef __u32
typedef unsigned int __u32;
#endif
#ifndef _u32
typedef unsigned int _u32;
#endif
#ifndef u32
typedef unsigned int u32;
#endif
#ifndef U32
typedef unsigned int U32;
#endif
#ifndef uint32
typedef unsigned int uint32;
#endif
#ifndef UINT32
typedef unsigned int UINT32;
#endif
#ifndef __s64
typedef signed long long __s64;
#endif
#ifndef _s64
typedef signed long long _s64;
#endif
#ifndef s64
typedef signed long long s64;
#endif
#ifndef S64
typedef signed long long S64;
#endif
#ifndef sint64
typedef signed long long sint64;
#endif
#ifndef SINT64
typedef signed long long SINT64;
#endif
#ifndef int64
typedef signed long long int64;
#endif
#ifndef INT64
typedef signed long long INT64;
#endif
#ifndef __u64
typedef unsigned long long __u64;
#endif
#ifndef _u64
typedef unsigned long long _u64;
#endif
#ifndef u64
typedef unsigned long long u64;
#endif
#ifndef U64
typedef unsigned long long U64;
#endif
#ifndef uint64
typedef unsigned long long uint64;
#endif
#ifndef UINT64
typedef unsigned long long UINT64;
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

extern void *NAND_IORemap(void *base_addr, unsigned int size);

typedef int (*NAND_ClkRequest_t)(__u32 nand_index);
typedef void (*NAND_ClkRelease_t)(__u32 nand_index);
typedef int (*NAND_SetClk_t)(__u32 nand_index, __u32 nand_clk0,
			     __u32 nand_clk1);
typedef int (*NAND_GetClk_t)(__u32 nand_index, __u32 *pnand_clk0,
			     __u32 *pnand_clk1);
//USE_SYS_CLK
extern NAND_ClkRequest_t NAND_ClkRequest;
extern NAND_ClkRelease_t NAND_ClkRelease;
extern NAND_SetClk_t NAND_SetClk;
extern NAND_GetClk_t NAND_GetClk;

extern __s32 NAND_CleanFlushDCacheRegion(void *buff_addr, __u32 len);
extern __s32 NAND_InvaildDCacheRegion(__u32 rw, __u32 buff_addr, __u32 len);
extern void *NAND_DMASingleMap(__u32 rw, void *buff_addr, __u32 len);
extern void *NAND_DMASingleUnmap(__u32 rw, void *buff_addr, __u32 len);

extern int nand_dma_config_start(__u32 rw, int addr, __u32 length);
extern int nand_request_dma(void);
extern void *NAND_VA_TO_PA(void *buff_addr);

extern __s32 NAND_PIORequest(__u32 nand_index);
extern void NAND_PIORelease(__u32 nand_index);
extern __u32 NAND_GetNandExtPara(__u32 para_num);

extern void nand_cond_resched(void);

extern __u32 NAND_GetNandIDNumCtrl(void);

extern void NAND_Memset(void *pAddr, unsigned char value, unsigned int len);
extern void NAND_Memcpy(void *pAddr_dst, void *pAddr_src, unsigned int len);
extern int NAND_Memcmp(void *pAddr_dst, void *pAddr_src, unsigned int len);
extern void *NAND_Malloc(unsigned int Size);
extern void NAND_Free(void *pAddr, unsigned int Size);
extern int NAND_Print(const char *fmt, ...);
extern int NAND_Print_DBG(const char *fmt, ...);

extern void *NAND_GetIOBaseAddrCH0(void);
extern void *NAND_GetIOBaseAddrCH1(void);
extern __u32 NAND_GetNdfcVersion(void);
extern __u32 NAND_GetNdfcDmaMode(void);
extern __u32 NAND_GetMaxChannelCnt(void);

extern int NAND_PhysicLockInit(void);
extern int NAND_PhysicLock(void);
extern int NAND_PhysicUnLock(void);
extern int NAND_PhysicLockExit(void);

#define NAND_IO_BASE_ADDR0 (NAND_GetIOBaseAddrCH0())
#define NAND_IO_BASE_ADDR1 (NAND_GetIOBaseAddrCH1())
extern __s32 nand_rb_wait_time_out(__u32 no, __u32 *flag);
extern __s32 nand_rb_wake_up(__u32 no);

extern __s32 nand_dma_wait_time_out(__u32 no, __u32 *flag);
extern __s32 nand_dma_wake_up(__u32 no);

extern int NAND_IS_Secure_sys(void);
extern void NAND_Print_Version(void);

//define the memory set interface
#define MEMSET(x, y, z) NAND_Memset((x), (y), (z))

//define the memory copy interface
#define MEMCPY(x, y, z) NAND_Memcpy((x), (y), (z))

#define MEMCMP(x, y, z) NAND_Memcmp((x), (y), (z))

//define the memory alocate interface
#define MALLOC(x) NAND_Malloc((x))

//define the memory release interface
#define FREE(x, size) NAND_Free((x), (size))

//define the message print interface
#define PRINT(...) NAND_Print(__VA_ARGS__)
#define PRINT_DBG(...) NAND_Print_DBG(__VA_ARGS__)
#define NAND_ERR(...) NAND_Print(__VA_ARGS__)
#define NAND_DBG(...) NAND_Print_DBG(__VA_ARGS__)

#endif //__NAND_OSAL_H__
