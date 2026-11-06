/**********************************************************************
 * request.h
 *
 * HPNFC request assemble and execute
 ***********************************************************************/

#ifndef REQUEST_H
#define REQUEST_H


#include <hpnfc_if.h>

#define PTR_INC(base, offset) (void *)((uint8_t *)(base) + (offset))
#define PTR_DEC(base, offset) (void *)((uint8_t *)(base) - (offset))
#define ROUND_UP(divident, divisor) ((((divident) + (divisor) - 1) / (divisor)) * (divisor))
#define ROUND_DOWN(divident, divisor) (((divident) / (divisor)) * (divisor))

#define ROUND_UP_DIV(a, b) (((a) + (b)-1) / (b))

void CdmaPrepareCmdReq(void *privateData, HPNFC_CdmaCmd *cdmaCmd,
					   HPNFC_CmdReq *cmdReq, uint8_t thread);

int CdmaExecCmdReq(void *privateData, HPNFC_CdmaCmd *cdmaCmd,
				   HPNFC_CmdReq *cmdReq, uint8_t thread);

int CdmaExecReadPage(void *privateData, uint8_t bank, uint32_t rowAddr,
					 uint32_t pageCnt,  void *buffer, uint8_t thread);

int CdmaExecProgramPage(void *privateData, uint8_t bank, uint32_t rowAddr,
						uint32_t pageCnt, void *buffer, uint8_t thread);

int CdmaExecEraseBlock(void *privateData, uint8_t bank, uint32_t rowAddr,
					   uint32_t blkCnt);

int GenExecEraseBlock(void *privateData, uint8_t chipSelect,
					  uint8_t rowAddr);

int GenExecProgramPage(void *privateData, uint8_t bank, uint32_t rowAddr,
					   void *buffer, uint32_t buffSize);

int GenExecReadPage(void *privateData, uint8_t bank, uint32_t rowAddr,
					void *buffer, uint32_t buffSize);

int GenExecProgramPageSplit(void *privateData, uint8_t bank, uint32_t rowAddr,
							  void *buffer, uint32_t buffSize);

int GenExecReadPageSplit(void *privateData, uint8_t bank, uint32_t rowAddr,
						   void *buffer, uint32_t buffSize);

int PioExecEraseBlock(void *privateData,
					  uint8_t bank, uint8_t rowAddr, uint32_t count);

int PioExecProgramPage(void* privateData, uint8_t bank,
					   uint32_t rowAddr, uint32_t count, void *buffer,
					   HPNFC_DmaMode dmaMode);

int PioExecReadPage(void* privateData, uint8_t bank,
					uint32_t rowAddr, uint32_t count, void *buffer,
					HPNFC_DmaMode dmaMode);

int GenUnlockCmd(void *privateData, uint8_t chipSelect, uint8_t rowAddrUp,
				 uint8_t rowAddrLo);

int GenExecReadID(void *privateData, uint8_t bank,
				  void *buffer, uint32_t buffSize);

int PioExecSetFeatures(void *privateData, uint8_t featAddr,
					   uint32_t featVal, uint8_t bank);
#endif
