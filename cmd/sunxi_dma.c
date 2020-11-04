
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
	sunxi_dma_set dma_set;
	uint hdma, st=0;
	ulong timeout;

	if(argc < 3) {
		printf("parameters error\n");
		return -1;
	}
	else {
		/* use argument only*/
		src_addr = simple_strtoul(argv[1], NULL, 16);
		dst_addr = simple_strtoul(argv[2], NULL, 16);
		if(argc == 4)
			len = simple_strtoul(argv[3], NULL, 16);
	}
	len = ALIGN(len, 4);
	printf("sunxi dma: 0x%08x ====> 0x%08x, len %d \n", src_addr, dst_addr, len);

	/* dma */
	dma_set.loop_mode = 0;
	dma_set.wait_cyc  = 8;
	dma_set.data_block_size = 1 * 32/8;
	/* channal config (from dram to dram)*/
	dma_set.channal_cfg.src_drq_type     = DMAC_CFG_TYPE_DRAM ;  //dram
	dma_set.channal_cfg.src_addr_mode    = DMAC_CFG_DEST_ADDR_TYPE_LINEAR_MODE;
	dma_set.channal_cfg.src_burst_length = DMAC_CFG_SRC_1_BURST;
	dma_set.channal_cfg.src_data_width   = DMAC_CFG_SRC_DATA_WIDTH_32BIT;
	dma_set.channal_cfg.reserved0        = 0;

	dma_set.channal_cfg.dst_drq_type     = DMAC_CFG_TYPE_DRAM;  //dram
	dma_set.channal_cfg.dst_burst_length = DMAC_CFG_DEST_1_BURST;
	dma_set.channal_cfg.dst_addr_mode    = DMAC_CFG_DEST_ADDR_TYPE_LINEAR_MODE;
	dma_set.channal_cfg.dst_data_width   = DMAC_CFG_DEST_DATA_WIDTH_32BIT;
	dma_set.channal_cfg.reserved1        = 0;

	flush_cache(src_addr, len);
	flush_cache(dst_addr, len);

	sunxi_dma_init();
	hdma =  sunxi_dma_request(0);
	if(!hdma)
		printf("can't request dma\n");

	sunxi_dma_install_int(hdma, sunxi_dma_isr, NULL);
	sunxi_dma_enable_int(hdma);
	sunxi_dma_setting(hdma, &dma_set);
	sunxi_dma_start(hdma, src_addr, dst_addr, len);

	/* timeout : 1000 ms */
	timeout = get_timer(0);
	st = sunxi_dma_querystatus(hdma);

	while((get_timer(timeout) < 1000) && st)
		st = sunxi_dma_querystatus(hdma);
	if (st) {
		printf("wait dma timeout!\n");
	}

	sunxi_dma_stop(hdma);
	sunxi_dma_release(hdma);

	return 0;
}


U_BOOT_CMD(
	sunxi_dma,	3,	1,	do_sunxi_dma_test,
	"do dma test",
	"sunxi_dma src_addr dst_addr"
);
