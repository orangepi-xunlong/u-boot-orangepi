
#include <common.h>
#include <command.h>
#include <malloc.h>
#include <asm/arch/dma.h>


static void  sunxi_dma_isr(void *p_arg)
{
	printf("dma int occur\n");
}

static int do_sunxi_dma_test(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	unsigned int src_addr = 0, dst_addr= 0, len = 512;
	sunxi_dma_setting_t cfg;
	uint hdma;

	printf("make sure dma contorller has inited\n");
	//dma
	cfg.loop_mode = 0;
	cfg.wait_cyc  = 8;
	cfg.data_block_size = 1 * 32/8;
	//config recv(from dram to dram)
	cfg.cfg.src_drq_type     = DMAC_CFG_SRC_TYPE_DRAM;  //dram
	cfg.cfg.src_addr_mode    = DMAC_CFG_DEST_ADDR_TYPE_LINEAR_MODE;
	cfg.cfg.src_burst_length = DMAC_CFG_SRC_1_BURST;
	cfg.cfg.src_data_width   = DMAC_CFG_SRC_DATA_WIDTH_32BIT;
	cfg.cfg.reserved0        = 0;

	cfg.cfg.dst_drq_type     = DMAC_CFG_DEST_TYPE_DRAM;  //DRAM
	cfg.cfg.dst_addr_mode    = DMAC_CFG_DEST_ADDR_TYPE_LINEAR_MODE;
	cfg.cfg.dst_burst_length = DMAC_CFG_DEST_1_BURST;
	cfg.cfg.dst_data_width   = DMAC_CFG_DEST_DATA_WIDTH_32BIT;
	cfg.cfg.reserved1        = 0;
	
	if(argc < 3)
	{
		printf("parameters error\n");
		return -1;
	}
	else
	{
		/* use argument only*/
		src_addr = simple_strtoul(argv[1], NULL, 16);
		dst_addr = simple_strtoul(argv[2], NULL, 16);
		if(argc == 4)
			len = simple_strtoul(argv[3], NULL, 16);
	}
	len = ALIGN(len, 4);
	printf("sunxi dma: 0x%08x ====> 0x%08x, len %d \n", src_addr, dst_addr, len);
	hdma =  sunxi_dma_request(0);

	sunxi_dma_install_int( hdma, sunxi_dma_isr, NULL);
	sunxi_dma_enable_int(hdma);

	sunxi_dma_setting(hdma,&cfg);
	sunxi_dma_start(hdma,src_addr,dst_addr,len);

	while(sunxi_dma_request()){
	
	}
	return 0;
}


U_BOOT_CMD(
	sunxi_dma,	3,	1,	do_sunxi_dma_test,
	"do dma test",
	"sunxi_dma src_addr dst_addr"
);
