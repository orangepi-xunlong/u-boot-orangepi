//*****************************************************************************
//  Allwinner Technology, All Right Reserved. 2006-2010 Copyright (c)
//
//  File:               chip_id.c
//
//  Description:  This file implements basic functions for AW1673 DRAM controller
//
//  History:
//              2015/03/23      YSZ         0.10    Initial version
//*****************************************************************************
#include "chip_id_sd.h"

extern unsigned int id_judge_fun(unsigned short disturb_code);


//***********************************************************************************************
//  unsigned short crc_16_sd(unsigned short reg, unsigned char data_crc)
//
//  Description:    CRC16 coding
//
//  Arguments:      16bit disturb_code, 8bit id_num.
//                              id_num is the label of the bonding_id,
//                              but the value of id_num and bonding_id may be different
//
//  Return Value:   CRC16 result
//***********************************************************************************************
unsigned short crc_16_sd(unsigned short reg, unsigned char data_crc)
//reg is crc register,data_crc is 8bit data stream
{
    unsigned short msb;
    unsigned short data;
    unsigned short gx = 0x8005, i = 0;

    data = (unsigned short)data_crc;
    data = data << 8;
    reg = reg ^ data;

    do
    {
        msb = reg & 0x8000;
        reg = reg << 1;
        if(msb == 0x8000)
        {
            reg = reg ^ gx;
        }
        i++;
    }
    while(i < 8);

    return (reg);
}

//***********************************************************************************************
//    unsigned int disturb_coding_sd(unsigned short disturb_code,unsigned char id_num)
//
//    Description:      generate a scrambling code depend on the data input
//
//    Arguments:        16bit disturb_code, 8bit id_num.
//                      id_num is the label of the bonding_id, but the value of id_num and bonding_id may be different
//
//    Return Value:     scrambling code
//***********************************************************************************************
unsigned int disturb_coding_sd(unsigned short disturb_code,unsigned char id_num)//
{
    unsigned int crc_result = 0;
    unsigned int disturb_result = 0;

    crc_result = crc_16_sd(disturb_code,id_num);

    disturb_result = ( (crc_result << 16) | (disturb_code + id_num) );

    return disturb_result;
}

//***********************************************************************************************
//  unsigned int bond_id_check(void)
//
//  Description:    called by init_DRAM function, check whether the ID judgment is OK
//
//  Arguments:      None
//
//  Return Value:   check result
//***********************************************************************************************
unsigned int bond_id_check(void)
{
    unsigned char i=0;
    unsigned int id_judge_ret=0;
    unsigned char legal_id_num=0;
    unsigned int reg_val=0;

    id_judge_ret = id_judge_fun(0);
    //check the ID number of the platform: ID number is not always the bonding_id
    for(i=1;i<17;i++)
    {
        reg_val = disturb_coding_sd(0,i);
        if(reg_val == id_judge_ret)
        {
            legal_id_num = i;
            break;
        }
    }

    if(i>=17)
        return 0;

    //after bonding_id found, do more check about the ID
    for(i=1;i<5;i++)
    {
        id_judge_ret = id_judge_fun(i);
        reg_val = disturb_coding_sd(i,legal_id_num);
        if(id_judge_ret != reg_val)
        {
            return 0;
        }
    }

    return 1;   //id check OK
}
