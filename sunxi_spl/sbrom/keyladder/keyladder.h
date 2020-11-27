/*
**********************************************************************************************************************
*
*						           the Embedded Secure Bootloader System
*
*
*						       Copyright(C), 2006-2014, Allwinnertech Co., Ltd.
*                                           All Rights Reserved
*
* File    :
*
* By      :
*
* Version : V2.00
*
* Date	  :
*
* Descript:
**********************************************************************************************************************
*/
#ifndef  __KEYLADDER_H__
#define  __KEYLADDER_H__

extern void key_ladder_init(toc0_private_head_t *toc0);
extern int sunxi_key_ladder_verify(sunxi_key_t  *pubkey);


#endif

