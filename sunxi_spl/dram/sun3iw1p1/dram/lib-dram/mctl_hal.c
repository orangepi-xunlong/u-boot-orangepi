//*****************************************************************************
//  Allwinner Technology, All Right Reserved. 2006-2010 Copyright (c)
//
//  File:               mctl_hal.c
//
//  Description:  This file implements basic functions for AW1667 DRAM controller
//
//  History:
//              2013/09/18      Golden          0.10    Initial version
//*****************************************************************************

#include <configs/sun3iw1p1.h>
#include <asm/io.h>
#include <asm/arch/dram.h>
#include <asm/arch/timer.h>
#include "mctl_reg.h"



extern int printf(const char *fmt, ...);
extern void * memcpy(void * dest,const void *src,size_t count);
extern  int do_div( int divisor, int by);

void  aw_delay(unsigned n)
{
    __msdelay(5);
}

static int DRAMC_initial(void)
{
    unsigned int time = 0xffffff;

    //initialization
    DRAM_REG_SCTLR = DRAM_REG_SCTLR | 0x1;
    while( (DRAM_REG_SCTLR & 0x1) && time-- )
    {
        if(time == 0)
            return 0;
    }

    return 1;
}


/*
 *************************************************************************************************************
 *                                              DRAM parameter setup
 * Description: setup/update the dram register parameters and initialization
 *
 * Arguments: para: dram configure parameters
 *
 * Returns: EBSP_TRUE
 *
 * Notes: none
 *
 *************************************************************************************************************
 */
static __u32 DRAMC_para_setup(__dram_para_t *para)
{
    __u32   reg_val = 0;

    //setup SCONR register
    reg_val = (para->ddr8_remap)                                   |
        (0x1<<1)                                                   |
        ((para->bank_size>>2)<<3)                                  |
        ((para->cs_num>>1)<<4)                                     |
        ((para->row_width-1)<<5)                                   |
        ((para->col_width-1)<<9)                                   |
        ((para->type? (para->bwidth>>4) : (para->bwidth>>5))<<13)  |
        (para->access_mode<<15)                                    |
        (para->type<<16);
#ifdef DRAM_PAD_MODE0_TEST
    reg_val &= ~0x6;
#endif
#ifdef DRAM_PAD_MODE2_TEST
    reg_val &= ~0x6;
    reg_val |= 0x4;
#endif
    DRAM_REG_SCONR = reg_val;

    //set DRAM control register
    DRAM_REG_SCTLR = DRAM_REG_SCTLR | 0x1<<19;  //enable delay auto refresh mode

    //initialization
    return DRAMC_initial();

}


static int DRAMC_delay_scan(void)
{
    unsigned int time = 0xffffff;
    //check whether read result is right for the readpipe value delay to  auto scan
    DRAM_REG_DDLYR|= 0x1;
    while( (DRAM_REG_DDLYR & 0x1) && time-- )
    {
        if(time == 0)
            return 0;
    }
    return 1;
}



/*
 *************************************************************************************************************
 *                                         DRAM check type
 * Description: check whether the DRAM is DDR or SDR
 *
 * Arguments: para: dram configure parameters
 *
 * Returns: 0: SDR
 *          1: DDR
 * Notes:
 *
 *************************************************************************************************************
 */
__u32 CSP_DRAMC_check_type(__dram_para_t *para)
{
    __u32 reg_val = 0;
    __u32 i;
    __u32 times = 0;

    //check 8 readpipe values
    for(i=0; i<8; i++)
    {
        //readpipe value
        reg_val = DRAM_REG_SCTLR;
        reg_val &= ~(0x7<<6);
        reg_val |= (i<<6);
        DRAM_REG_SCTLR = reg_val;

        //check whether read result is right for the readpipe value
        DRAMC_delay_scan();

        //check whether the DQS flag is success
        if(DRAM_REG_DDLYR & 0x30)
        {
            //fail
            times++;
            //break;
        }
    }
    //all the readpipe values fail
    if(times == 8)
    {
        //SDR
        para->type= DRAM_TYPE_SDR;
        return 0;
    }
    else
    {
        //DDR
        para->type = DRAM_TYPE_DDR;
        return 1;
    }
}


void CSP_DRAMC_set_autofresh_cycle(__u32 clk)
{
    __u32 reg_val = 0;
    __u32 row = 0;
    __u32 temp = 0;

    row = DRAM_REG_SCONR;
    row &= 0x1E0;
    row >>= 0x5;

    //64 ms/8192 rows = 7.8us per refresh cycle
    if(row == 0xC)
    {
        if(clk >= 1000000)
        {
            //HZ
            temp = clk + (clk >> 3) + (clk >> 4) + (clk >> 5);
            while(temp >= (10000000 >> 6))
            {
                temp -= (10000000 >> 6);
                reg_val++;
            }
        }
        else
        {
            //MHZ
            reg_val = (clk*499)>>6;
        }
    }
    else if(row == 0xB) //64ms/4096 rows = 15.6us per refresh cycle
    {
        if(clk >= 1000000)
        {
            temp = clk + (clk >> 3) + (clk >> 4) + (clk >> 5);
            while(temp >= (10000000 >> 7))
            {
                temp -= (10000000 >> 7);
                reg_val++;
            }
        }
        else
        {
            reg_val = (clk*499)>>5;
        }
    }
    DRAM_REG_SREFR = reg_val;
}

/*

 *************************************************************************************************************
 *                                              CHECK DRAM delay success value
 * Description: counting the DQS report success value
 *
 *    Arguments: bwidth:  DRAM data width
 *
 * Returns: DQS report success value
 *
 * Notes: none
 *
 *************************************************************************************************************
 */
static __u32 DRAMC_check_delay(__u32 bwidth)
{
    __u32   dsize;
    __u32   i,j;
    __u32   num = 0;
    __u32   dflag = 0;

    dsize = ((bwidth == 16) ? 4 : 2);     //data width:  16bits  DQS0~3     8bits  DQS0~1

    for(i=0; i<dsize; i++)
    {
        switch(i)
        {
            case 0: dflag = DRAM_REG_DRPTR0;
                    break;
            case 1: dflag = DRAM_REG_DRPTR1;
                    break;
            case 2: dflag = DRAM_REG_DRPTR2;
                    break;
            case 3: dflag = DRAM_REG_DRPTR3;
                    break;
            default: break;
        }

        for(j=0; j<32; j++)
        {
            if(dflag & 0x1)            //1 success   0 fail
                num++;
            dflag >>= 1;
        }
    }

    return num;
}


/*
 **********************************************************************************************************************
 *                                               DRAM_check_ddr_readpipe
 *
 * Description: check the best dram readpipe value
 *
 * Arguments  : none
 *
 * Returns    : regurn EBSP_TRUE
 *
 * Notes      :
 *
 **********************************************************************************************************************
 */
__u32 sdr_readpipe_scan(void)
{
    __u32 k=0;
    for(k=0;k<32;k++)
    {
        __REG(0x80000000+4*k)=k;
    }
    for(k=0;k<32;k++)
    {
        if( __REG(0x80000000+4*k)!=k )
            return 0;
    }
    return 1;
}


__u32 sdr_readpipe_select(void)
{
    __u32 value=0;
    __u32 i=0;
    for(i=0;i<8;i++)
    {
        DRAM_REG_SCTLR &=(~(0x7<<6 ));
        DRAM_REG_SCTLR |=(i<<6);
        if(sdr_readpipe_scan())
        {
            value=i;
            return value;
        }
    }
    return value;
}


__u32 CSP_DRAMC_scan_readpipe(__dram_para_t *para)
{
    __u32   i, rp_best=0,rp_val=0 ;
    __u32   reg_val = 0;
    __u32   readpipe[8];

    if(para->type == DRAM_TYPE_DDR)
    {
        //DDR type
        for(i=0; i<8; i++)
        {
            //readpipe value fill
            reg_val = DRAM_REG_SCTLR;
            reg_val &= ~(0x7<<6);
            reg_val |= (i<<6);
            DRAM_REG_SCTLR = reg_val;

            //initialization
            //DRAMC_initial();

            //check whether read result is right for the readpipe value
            DRAMC_delay_scan();

            readpipe[i] = 0;
            if((((DRAM_REG_DDLYR>>4) & 0x3) == 0x0) && (((DRAM_REG_DDLYR>>4) & 0x1) == 0x0))
            {
                readpipe[i] = DRAMC_check_delay(para->bwidth);
            }

            //get the best readpipe value
            if(rp_val < readpipe[i])
            {
                rp_val = readpipe[i];
                rp_best = i;
            }
        }

        //set the best readpipe and check it
        reg_val = DRAM_REG_SCTLR;
        reg_val &= ~(0x7<<6);
        reg_val |= (rp_best<<6);
        DRAM_REG_SCTLR = reg_val;

        //check whether read result is right for the readpipe value
        DRAMC_delay_scan();
    }
    else
    {
        //SDR type
        //set up the sdr parameters
        reg_val = DRAM_REG_SCONR;
        reg_val &= (~(0x1<<16));
        reg_val &= (~(0x3<<13));
        DRAM_REG_SCONR = reg_val;

        //initialization
        DRAMC_initial();

        rp_best=sdr_readpipe_select();//auto select the best readpipe

        //set the best readpipe and check it
        reg_val = DRAM_REG_SCTLR;
        reg_val &= ~(0x7<<6);
        reg_val |= (rp_best<<6);
        DRAM_REG_SCTLR = reg_val;
    }

    return 0;
}


/*
 *************************************************************************************************************
 *                                         DRAM check size
 * Description: check DRAM size :16MB/32MB/64MB
 *
 * Arguments: para: dram configure parameters
 *
 * Returns: 0: find the match size
 *          1: can't find the match size
 * Notes:
 *
 *************************************************************************************************************
 */
__u32 CSP_DRAMC_get_dram_size(__dram_para_t *para)
{
    __u32   colflag = 10, rowflag = 13;
    __u32   i = 0;
    __u32   val1=0;
    __u32   count = 0;
    __u32   addr1, addr2;

    //--------------------column test begin---------------------------------
    para->col_width = colflag;
    para->row_width = rowflag;

    //fill the parameter and initial
    DRAMC_para_setup(para);

    //scan the best readpipe
    CSP_DRAMC_scan_readpipe(para);

    //bank0 10 column or 9 column test
    for(i=0;i<32;i++)
    {
        *( (__u32 *) (0x80000200+i) ) = 0x11111111;
        *( (__u32 *) (0x80000600+i) ) = 0x22222222;
    }
    for(i=0;i<32;i++)
    {
        val1 = *( (__u32 *) (0x80000200+i) );
        if(val1 == 0x22222222)
            count++;
    }

    //not 10 column
    if(count == 32)
    {
        colflag = 9;
    }
    else
    {
        colflag = 10;
    }

    //--------------------row test begin-------------------------------
    count = 0;
    para->col_width = colflag;
    para->row_width = rowflag;

    //fill the parameter and initial
    DRAMC_para_setup(para);

    //scan the best readpipe
    //CSP_DRAMC_scan_readpipe();

    if(colflag == 10)
    {
        addr1 = 0x80400000;
        addr2 = 0x80c00000;
    }
    else
    {
        addr1 = 0x80200000;
        addr2 = 0x80600000;
    }
    //bank0 13 row or 12 row test
    for(i=0;i<32;i++)
    {
        *( (__u32 *) (addr1+i) ) = 0x33333333;
        *( (__u32 *) (addr2+i) ) = 0x44444444;
    }
    for(i=0;i<32;i++)
    {
        val1 = *( (__u32 *) (addr1+i) );
        if(val1 == 0x44444444)
        {
            count++;
        }
    }

    if(count == 32)  //not 13 row
    {
        rowflag = 12;
    }
    else
        rowflag = 13;

    para->col_width = colflag;
    para->row_width = rowflag;

    //return size type
    (para->row_width != 13) ? (para->size = 16) : ( (para->col_width == 10) ? (para->size = 64) : (para->size = 32) );
    if(para->row_width != 13)
    {
        para->size = 16;
    }
    else if(para->col_width == 10)
    {
        para->size = 64;
    }
    else
    {
        para->size = 32;
    }

    //set DRAM refresh interval register
    CSP_DRAMC_set_autofresh_cycle(para->clk);

    //fill the parameter and initial
    para->access_mode = 0;
    DRAMC_para_setup(para);

    //scan the best readpipe
    //CSP_DRAMC_scan_readpipe();

    return 0;
}


__s32 CSP_DRAMC_init(__dram_para_t *dram_para, __s32 mode)
{
    uint reg_val = 0;
    uint i=0;
    uint ddl=0;
    //disabled the pin of internal reference voltage
    SDR_VREF|=(0x7)<<12;

    aw_delay(0xffff);

    if(dram_para->cas & (0x1<<3))
    {
        //enable internal reference
        SDR_PAD_PUL_REG |= ((0x1<<23));
        //set the factor of internal reference
        SDR_PAD_PUL_REG |= ((0x20<<17));
    }

    aw_delay(0xffff);


    SDR_PAD_DRV_REG=0xfff;


    aw_delay(0xffff);


    //ccmu configure M | K | N ,PLL_DDR = 24MHZ*N*K/M
    if((dram_para->clk)<=96)
    {
        //M=1,K=1,N=,  enable
        reg_val = (0x0<<0) |  (0x0<<4) | (((dram_para->clk*2)/24 - 1)<<8) | (0x1u<<31);//N=PLL/12---PLL=12*N(N<=32)---DDR_CLK=6*N--M=2K
    }
    else
    {
        //M=2,K=2,N=,  enable
        reg_val = (0x1<<0) |  (0x1<<4) | (((dram_para->clk*2)/24 - 1)<<8) | (0x1u<<31);//N=PLL/24---PLL=24*N(N<=32)---DDR_CLK=12*N--M=K
    }


    if(dram_para->cas & (0x1<<4))
    {
        //Modulation mode enable,FREQ = 31.5KHz, FREQ_MODEL:triangular
        PLL_DDR_PAT_CTRL_REG= 0xD1303333;
    }
    else if(dram_para->cas & (0x1<<5))//----------------------------------------------------------------------------------------
    {
        //Modulation mode enable,FREQ = 31.5KHz,FREQ_MODEL:triangular
        PLL_DDR_PAT_CTRL_REG= 0xcce06666;
    }
    else if(dram_para->cas & (0x1<<6))//---------------------------------------------------------------------------------------
    {
        //Modulation mode enable,FREQ = 31.5KHz,FREQ_MODEL:triangular
        PLL_DDR_PAT_CTRL_REG= 0xc8909999;
    }
    else if(dram_para->cas & (0x1<<7))//----------------------------------------------------------------------------------------
    {
        //Modulation mode enable,FREQ = 31.5KHz,FREQ_MODEL:triangular
        PLL_DDR_PAT_CTRL_REG= 0xc440cccc;
    }
    if( dram_para->cas & (0xf<<4) )//open frequnce extend
    {
        //SIGMA_DELA enable
        reg_val |= 0x1<<24;
    }


    //------------------------------------------------------------------------------------
    PLL_DDR_CTRL_REG = reg_val;

    PLL_DDR_CTRL_REG|=(0x1<<20);//updata
    while((PLL_DDR_CTRL_REG & (1<<28)) == 0);
    aw_delay(0xffff);

    //dram ahb1 gating
    BUS_CLK_GATING_REG0 |= 0x1<<14;

    //dram reset
    BUS_SOFT_RST_REG0 &= ~(0x1<<14);
    for(i=0; i<10; i++)
        continue;
    BUS_SOFT_RST_REG0 |= (0x1<<14);



    //set SDRAM PAD TYPE
    reg_val = SDR_PAD_PUL_REG;
    (dram_para->type == DRAM_TYPE_DDR) ? (reg_val |= (0x1<<16)) : (reg_val &= ~(0x1<<16));
    SDR_PAD_PUL_REG = reg_val;

    //set MCTL Timing 0 Register
    reg_val = (SDR_T_CAS<<0)|(SDR_T_RAS<<3)|(SDR_T_RCD<<7)|(SDR_T_RP<<10)|(SDR_T_WR<<13)|(SDR_T_RFC<<15)|(SDR_T_XSR<<19)|(SDR_T_RC<<28);
    DRAM_REG_STMG0R = reg_val;

    //set MCTL Timing 1 Register
    reg_val = (SDR_T_INIT<<0)|(SDR_T_INIT_REF<<16)|(SDR_T_WTR<<20)|(SDR_T_RRD<<22)|(SDR_T_XP<<25);
    DRAM_REG_STMG1R = reg_val;

    //parameters setup and update
    DRAMC_para_setup(dram_para);

    //check DRAM TYPE
    CSP_DRAMC_check_type(dram_para);

    //update SDRAM PAD TYPE
    reg_val = SDR_PAD_PUL_REG;
    (dram_para->type == DRAM_TYPE_DDR) ? (reg_val |= (0x1<<16)) : (reg_val &= ~(0x1<<16));
    SDR_PAD_PUL_REG = reg_val;

    CSP_DRAMC_set_autofresh_cycle(dram_para->clk);
    CSP_DRAMC_scan_readpipe(dram_para);//readpipe---inserted in read data path(not write) for SDRAM in order to correctly latch data
    //check DRAM SIZE
    CSP_DRAMC_get_dram_size(dram_para);

    if((dram_para->cs_num&0xffff0000)!=0)//add write dqs dq delay
    {
        ddl=((dram_para->cs_num&0xf0000)>>16);//write dq0~dq7 delay
        DRAM_REG_SDLY1|=(ddl|(ddl<<4)|(ddl<<8)|(ddl<<12)|(ddl<<16)|(ddl<<20)|(ddl<<24)|(ddl<<28));

        ddl=((dram_para->cs_num&0xf00000)>>20);//write dq8~dq15 delay
        DRAM_REG_SDLY2|=(ddl|(ddl<<4)|(ddl<<8)|(ddl<<12)|(ddl<<16)|(ddl<<20)|(ddl<<24)|(ddl<<28));

        ddl=((dram_para->cs_num&0xf000000)>>24);//write dqs0/dqm0 delay
        DRAM_REG_SDLY0|=(ddl&0x7);

        ddl=((dram_para->cs_num&0xf0000000)>>28);//write dqs1/dqm1 delay
        DRAM_REG_SDLY0|=((ddl&0x7)<<4);

    }


    //basic rw test
    for(i=0; i<128; i++)
    {
        __REG(dram_para->base + 4*i) = dram_para->base + 4*i;
    }
    for(i=0; i<128; i++)
    {
        if( __REG(dram_para->base + 4*i) != (dram_para->base + 4*i) )
        {
            return 0;
        }
    }

        return dram_para->size;
}


/*
 *********************************************************************************************************
 *                                   DRAM INITIALISE
 *
 * Description: dram initialise;
 *
 * Arguments  : para     dram configure parameters
 *              mode     mode for initialise dram
 *
 * Returns    : EBSP_FALSE: dram parameter is invalid or read_write failed
 *               EBSP_TRUE: initialzation success
 * Note       :
 *********************************************************************************************************
 */

unsigned int mctl_init(void *para)
{
    signed int ret_val=0;
    //DRAM of sun3i is different,so we set the parameters here
    //can we move these parameters to sys_config ?
    __dram_para_t parameters = {
        0x80000000,             //base
        32,                     //size--MBYTE
        PLL_DDR_CLK/1000000,    //clk
        1,                      //access_mode
        1,                      //cs_num
        0,                      //ddr8_remap
        DRAM_TYPE_DDR,          //type
        16,                     //bwidth
        10,                     //col_width
        13,                     //row_width
        4,                      //bank_size
        3                       //cas
    };

    parameters.clk = PLL_DDR_CLK/1000000;

    ret_val = CSP_DRAMC_init(&parameters, 0);
    if(para != NULL)
    {
	//parameters after 12 may be used by other codes, don't cover them
        memcpy(para,&parameters, 12*sizeof(int) );
    }
    return ret_val;
}

#define AW1663_F20A_CHIP_ID     0x00
#define AW1663_C500_CHIP_ID     0x03
#define AW1663_C600_CHIP_ID     0x02
#define SYS_CTRL_BASE_REG       0x01c00000
#define CHIP_ID_REG_OFFSET      0x28
#define READ_UINT32(reg)        (*((volatile unsigned int *)(reg)))

unsigned int get_ic_chip_id(void)
{
    volatile unsigned int chip_id_val = 0;

    chip_id_val = READ_UINT32(SYS_CTRL_BASE_REG + CHIP_ID_REG_OFFSET);

    return chip_id_val;
}

int init_DRAM( int type, __dram_para_t *buff)
{
    volatile unsigned int chip_id_val = 0;

    chip_id_val = get_ic_chip_id();
    if (AW1663_F20A_CHIP_ID != chip_id_val
                    && AW1663_C500_CHIP_ID != chip_id_val
                    && AW1663_C600_CHIP_ID != chip_id_val)
    {
        printf("Chip id is not right, result=%d\n", chip_id_val);
        return 0;
    }

    return mctl_init((void*)buff);
}
