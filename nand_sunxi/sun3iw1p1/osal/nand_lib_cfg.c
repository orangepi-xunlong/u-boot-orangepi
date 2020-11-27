

#define   PHY_ONLY_TOG_AND_SDR                           1
#define   PHY_WAIT_RB_BEFORE                             0
#define   PHY_WAIT_RB_INTERRRUPT                         0
#define   PHY_WAIT_DMA_INTERRRUPT                        0



#define   PHY_SUPPORT_TWO_PLANE                          1
#define   PHY_SUPPORT_VERTICAL_INTERLEAVE                1
#define   PHY_SUPPORT_DUAL_CHANNEL                       1


int nand_cfg_interface(void)
{
    return PHY_ONLY_TOG_AND_SDR ? 1 : 0;
}

int nand_wait_rb_before(void)
{
    return PHY_WAIT_RB_BEFORE ? 1 : 0;
}

int nand_wait_rb_mode(void)
{
    return PHY_WAIT_RB_INTERRRUPT ? 1 : 0;
}

int nand_wait_dma_mode(void)
{
    return PHY_WAIT_DMA_INTERRRUPT ? 1 : 0;
}

int nand_support_two_plane(void)
{
    return PHY_SUPPORT_TWO_PLANE ? 1 : 0;
}

int nand_support_vertical_interleave(void)
{
    return PHY_SUPPORT_VERTICAL_INTERLEAVE ? 1 : 0;
}

int nand_support_dual_channel(void)
{
    return PHY_SUPPORT_DUAL_CHANNEL ? 1 : 0;
}
