/*
**********************************************************************************************************************
*                                                   eGon
*                                  the Embedded GO-ON Bootloader System
*                                          eGON arm boot sub-system
*
*                         Copyright(C), 2006-2010, SoftWinners Microelectronic Co., Ltd.
*                                           All Rights Reserved
*
* File    : nand_osal_boot0.c
*
* By      : Jerry
*
* Version : V2.00
*
* Date    :
*
* Descript:
**********************************************************************************************************************
*/
#include "../../nand_common.h"

int NAND_Print(const char * str, ...);

#define get_wvalue(addr)    (*((volatile unsigned long  *)(addr)))
#define put_wvalue(addr, v) (*((volatile unsigned long  *)(addr)) = (unsigned long)(v))

u32 _get_pll4_periph1_clk(void)
{
    return 24000000;
}

__u32 _Getpll6Clk(void)
{
    return 600;
}

int NAND_ClkRequest(__u32 nand_index)
{
    __u32 cfg;
    __u32 m, n;
    __u32 clk_src;

    clk_src = 0;
    if(nand_index == 0)
    {
        //10M
        if(clk_src == 0)
        {
            m = 0;
            n = 0;
        }
        else
        {
            m = 14;
            n = 1;
        }


        /*set nand clock gate on*/
        cfg = 0;

        /*gate on nand clock*/
        cfg |= (1U << 31);
        /*take pll6 as nand src block*/
        cfg |=  ((clk_src&0x3) << 24);

        //set divn = 0
        cfg |= (n&0x3)<<16;
        cfg |= (m&0xf)<<0;

        *(volatile __u32 *)(0x01c20000 + 0x80) = cfg;

        //open ahb
        cfg = *(volatile __u32 *)(0x01c20000 + 0x60);
        cfg |= (0x1<<13);
        *(volatile __u32 *)(0x01c20000 + 0x60) = cfg;

        //reset
        cfg = *(volatile __u32 *)(0x01c20000 + 0x2c0);
        cfg &= (~(0x1<<13));
        *(volatile __u32 *)(0x01c20000 + 0x2c0) = cfg;

        cfg = *(volatile __u32 *)(0x01c20000 + 0x2c0);
        cfg |= (0x1<<13);
        *(volatile __u32 *)(0x01c20000 + 0x2c0) = cfg;

        cfg = *(volatile __u32 *)(0x01c20000 + 0x2c0);
        cfg &= (~(0x1<<13));
        *(volatile __u32 *)(0x01c20000 + 0x2c0) = cfg;

        cfg = *(volatile __u32 *)(0x01c20000 + 0x2c0);
        cfg |= (0x1<<13);
        *(volatile __u32 *)(0x01c20000 + 0x2c0) = cfg;

        NAND_Print("NAND_ClkRequest, nand_index: 0x%x\n", nand_index);
        NAND_Print("Reg 0x01c20080: 0x%x\n", *(volatile __u32 *)(0x01c20080));
        NAND_Print("Reg 0x01c20060: 0x%x\n", *(volatile __u32 *)(0x01c20060));
        NAND_Print("Reg 0x01c202c0: 0x%x\n", *(volatile __u32 *)(0x01c202c0));
    }
    else
    {
        //10M
        if(clk_src == 0)
        {
            m = 0;
            n = 0;
        }
        else
        {
            m = 14;
            n = 1;
        }

        /*set nand clock gate on*/
        cfg = 0;

        /*gate on nand clock*/
        cfg |= (1U << 31);
        /*take pll6 as nand src block*/
        cfg |=  ((clk_src&0x3) << 24);
        //set divn = 0
        cfg |= (n&0x3)<<16;
        cfg |= (m&0xf)<<0;

        *(volatile __u32 *)(0x01c20000 + 0x84) = cfg;

        //open ahb
        cfg = *(volatile __u32 *)(0x01c20000 + 0x60);
        cfg |= (0x1<<12);
        *(volatile __u32 *)(0x01c20000 + 0x60) = cfg;

        //open reset
        cfg = *(volatile __u32 *)(0x01c20000 + 0x2c0);
        cfg &= (~(0x1<<12));
        *(volatile __u32 *)(0x01c20000 + 0x2c0) = cfg;

        cfg = *(volatile __u32 *)(0x01c20000 + 0x2c0);
        cfg |= (0x1<<12);
        *(volatile __u32 *)(0x01c20000 + 0x2c0) = cfg;

        cfg = *(volatile __u32 *)(0x01c20000 + 0x2c0);
        cfg &= (~(0x1<<12));
        *(volatile __u32 *)(0x01c20000 + 0x2c0) = cfg;

        cfg = *(volatile __u32 *)(0x01c20000 + 0x2c0);
        cfg |= (0x1<<12);
        *(volatile __u32 *)(0x01c20000 + 0x2c0) = cfg;

        NAND_Print("NAND_ClkRequest, nand_index: 0x%x\n", nand_index);
        NAND_Print("Reg 0x01c20084: 0x%x\n", *(volatile __u32 *)(0x01c20084));
        NAND_Print("Reg 0x01c20060: 0x%x\n", *(volatile __u32 *)(0x01c20060));
        NAND_Print("Reg 0x01c202c0: 0x%x\n", *(volatile __u32 *)(0x01c202c0));
    }


    return 0;
}

void NAND_ClkRelease(__u32 nand_index)
{
    return ;
}


int NAND_SetClk(__u32 nand_index, __u32 nand_clock)
{
    __u32 edo_clk;
    __u32 cfg;
    __u32 m = 0, n = 0;
    __u32 clk_src;


    clk_src = 0;

    if(clk_src == 0)
    {
    }
    else
    {
        edo_clk = nand_clock * 2;
        if(edo_clk <= 20)  //10M
        {
            n =  1;
            m = 14;
        }
        else if((edo_clk >20)&&(edo_clk <= 40))  //20M
        {
            n =  0;
            m = 14;
        }
        else if((edo_clk >40)&&(edo_clk <= 50))  //25M
        {
            n =  0;
            m = 11;
        }
        else if((edo_clk >50)&&(edo_clk <= 60))  //30M
        {
            n = 0;
            m = 9;
        }
        else //40M
        {
            n = 0;
            m = 7;
        }
    }


    if(nand_index == 0)
    {
        /*set nand clock*/
        /*set nand clock gate on*/
        cfg = *(volatile __u32 *)(0x01c20000 + 0x80);
        cfg &= (~(0x03 << 16));
        cfg &= (~(0xf));
        cfg |= ((n&0x3)<<16);
        cfg |= ((m&0xf));

        *(volatile __u32 *)(0x01c20000 + 0x80) = cfg;
        NAND_Print("NAND_SetClk, nand_index: 0x%x\n", nand_index);
        NAND_Print("Reg 0x01c20080: 0x%x\n", *(volatile __u32 *)(0x01c20080));

    }
    else
    {
        /*set nand clock*/
        /*set nand clock gate on*/
        cfg = *(volatile __u32 *)(0x01c20000 + 0x84);
        cfg &= (~(0x03 << 16));
        cfg &= (~(0xf));
        cfg |= ((n&0x3)<<16);
        cfg |= ((m&0xf));

        *(volatile __u32 *)(0x01c20000 + 0x84) = cfg;
        NAND_Print("NAND_SetClk, nand_index: 0x%x\n", nand_index);
        NAND_Print("Reg 0x01c20084: 0x%x\n", *(volatile __u32 *)(0x01c20084));

    }

    return 0;
}

int NAND_GetClk(__u32 nand_index)
{
    __u32 pll6_clk;
    __u32 cfg;
    __u32 nand_max_clock;
    __u32 m, n;

    if(nand_index == 0)
    {
        /*set nand clock*/
        pll6_clk = _Getpll6Clk();

        /*set nand clock gate on*/
        cfg = *(volatile __u32 *)(0x01c20000 + 0x80);
        m = ((cfg)&0xf) +1;
        n = ((cfg>>16)&0x3);
        nand_max_clock = pll6_clk/(2*(1<<n)*(m+1));
        NAND_Print("NAND_GetClk, nand_index: 0x%x, nand_clk: %dM\n", nand_index, nand_max_clock);
        NAND_Print("Reg 0x01c20080: 0x%x\n", *(volatile __u32 *)(0x01c20080));


    }
    else
    {
        /*set nand clock*/
        pll6_clk = _Getpll6Clk();

        /*set nand clock gate on*/
        cfg = *(volatile __u32 *)(0x01c20000 + 0x84);
        m = ((cfg)&0xf) +1;
        n = ((cfg>>16)&0x3);
        nand_max_clock = pll6_clk/(2*(1<<n)*(m+1));
        NAND_Print("NAND_GetClk, nand_index: 0x%x, nand_clk: %dM\n", nand_index, nand_max_clock);
        NAND_Print("Reg 0x01c20084: 0x%x\n", *(volatile __u32 *)(0x01c20084));
    }

    return nand_max_clock;
}

void NAND_PIORequest(__u32 nand_index)
{
    __u32 cfg;

    if(nand_index == 0)
    {
        *(volatile __u32 *)(0x01c20800 + 0x48) = 0x22222222;
        *(volatile __u32 *)(0x01c20800 + 0x4c) = 0x22222222;
        *(volatile __u32 *)(0x01c20800 + 0x50) = 0x72222222;
        *(volatile __u32 *)(0x01c20800 + 0x54) = 0x2;
        *(volatile __u32 *)(0x01c20800 + 0x5c) = 0x55555555;
        *(volatile __u32 *)(0x01c20800 + 0x60) = 0x15555;
        *(volatile __u32 *)(0x01c20800 + 0x64) = 0x00005140;
        *(volatile __u32 *)(0x01c20800 + 0x68) = 0x00001555;
    }
    else if(nand_index == 1)
    {
        *(volatile __u32 *)(0x01c20800 + 0x50) = 0x33333333;
        *(volatile __u32 *)(0x01c20800 + 0xfc) = 0x22222222;
        cfg = *(volatile __u32 *)(0x01c20800 + 0x100);
        cfg &= (~0xf);
        cfg |= (0x2);
        *(volatile __u32 *)(0x01c20800 + 0x100) = cfg;
        cfg = *(volatile __u32 *)(0x01c20800 + 0x108);
        cfg &= (~0xfU<<20);
        cfg |= (0x22<<20);
        *(volatile __u32 *)(0x01c20800 + 0x108) = cfg;
    }

}

void NAND_PIORelease(__u32 nand_index)
{
    return;
}

void NAND_EnRbInt(void)
{
    return ;
}


void NAND_ClearRbInt(void)
{
    return ;
}

int NAND_WaitRbReady(void)
{
    return 0;
}

int NAND_WaitDmaFinish(void)
{
    return 0;
}

void NAND_RbInterrupt(void)
{
    return ;
}

void* NAND_Malloc(unsigned int Size)
{
    return (void *)CONFIG_SYS_SDRAM_BASE;
}

void NAND_Free(void *pAddr, unsigned int Size)
{
    return ;
}

void *NAND_IORemap(unsigned int base_addr, unsigned int size)
{
    return (void *)base_addr;
}

__s32 NAND_Print(const char * str, ...)
{
    printf(str);
    return 0;
}
