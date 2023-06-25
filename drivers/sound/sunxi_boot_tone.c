/*
 * (C) Copyright 2014-2019
 * allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * caiyongheng <caiyongheng@allwinnertech.com>
 *
 * some simple description for this code
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 */

#include <common.h>
#include <command.h>
#include <linux/ctype.h>
#include <linux/types.h>
#include <asm/global_data.h>
#include <fdt_support.h>
#include <fdtdec.h>
#include <console.h>
#include <malloc.h>

#include <asm/arch/dma.h>
#include <sunxi_board.h>
#include <sunxi-codec.h>

#if 0
#define tone_dbg(fmt, arg...)	printf("[%s:%d] "fmt"\n", __FUNCTION__, __LINE__, ##arg)
#else
#define tone_dbg(fmt, arg...)
#endif

DECLARE_GLOBAL_DATA_PTR;

void *g_tone_buffer;
static sunxi_dma_set *g_codec_tx_buf;
static ulong g_codec_dma_handle;

typedef struct wav_header {
	char            riffType[4];            //4byte,资源交换文件标志:RIFF
	unsigned int    riffSize;               //4byte,从下个地址到文件结尾的总字节数
	char            waveType[4];            //4byte,wav文件标志:WAVE
	char            formatType[4];          //4byte,波形文件标志:FMT(最后一位空格符)最后一位空格符
	unsigned int    formatSize;             //4byte,音频属性(compressionCode,numChannels,sampleRate,bytesPerSecond,blockAlign,bitsPerSample)所占字节数
	unsigned short  compressionCode;        //2byte,格式种类(1-线性pcm-WAVE_FORMAT_PCM,WAVEFORMAT_ADPCM)
	unsigned short  numChannels;            //2byte,通道数
	unsigned int    sampleRate;             //4byte,采样率
	unsigned int    bytesPerSecond;         //4byte,传输速率
	unsigned short  blockAlign;             //2byte,数据块的对齐，即DATA数据块长度
	unsigned short  bitsPerSample;          //2byte,采样精度-PCM位宽
	char            dataType[4];            //4byte,数据标志:data
	unsigned int    dataSize;               //4byte,从下个地址到文件结尾的总字节数，即除了wav header以外的pcm data length
} wav_header_t;

static int sunxi_boot_tone_dma_init(void)
{
	int ret;
	/* send pcm data with DMA */
	/* g_codec_tx_buf = malloc_noncache(sizeof(sunxi_dma_set)); */
	g_codec_tx_buf = malloc(sizeof(sunxi_dma_set));
	memset(g_codec_tx_buf, 0, sizeof(sunxi_dma_set));

	sunxi_dma_init();
	/*g_codec_dma_handle = sunxi_dma_request(DMAC_DMATYPE_NORMAL);*/
	/* requeset from the last channel */
	g_codec_dma_handle = sunxi_dma_request_from_last(DMAC_DMATYPE_NORMAL);
	if (g_codec_dma_handle == 0) {
		printf("g_codec_dma_handle request dma failed\n");
		return -1;
	}

	g_codec_tx_buf->channal_cfg.src_drq_type	= DMAC_CFG_TYPE_DRAM;
	g_codec_tx_buf->channal_cfg.src_addr_mode	= DMAC_CFG_SRC_ADDR_TYPE_LINEAR_MODE;
	g_codec_tx_buf->channal_cfg.src_burst_length	= DMAC_CFG_SRC_1_BURST;
	g_codec_tx_buf->channal_cfg.src_data_width	= DMAC_CFG_SRC_DATA_WIDTH_16BIT;
	g_codec_tx_buf->channal_cfg.reserved0		= 0;

#ifdef CONFIG_MACH_SUN50IW10
	g_codec_tx_buf->channal_cfg.dst_drq_type	= 0x07;
#else
	g_codec_tx_buf->channal_cfg.dst_drq_type	= 0x06;
#endif
	g_codec_tx_buf->channal_cfg.dst_addr_mode	= DMAC_CFG_DEST_ADDR_TYPE_IO_MODE;
	g_codec_tx_buf->channal_cfg.dst_burst_length	= DMAC_CFG_DEST_4_BURST;
	g_codec_tx_buf->channal_cfg.dst_data_width	= DMAC_CFG_DEST_DATA_WIDTH_16BIT;
	g_codec_tx_buf->channal_cfg.reserved1		= 0;
	tone_dbg("config:0x%x...\n", g_codec_tx_buf->channal_cfg);

	/* g_codec_tx_buf->channal_cfg.wait_state		= 4; */
	/* g_codec_tx_buf->channal_cfg.continuous_mode	= 0; */

	g_codec_tx_buf->wait_cyc		= 8;
	g_codec_tx_buf->loop_mode		= 0;

	ret = sunxi_dma_enable_int(g_codec_dma_handle);
	if (ret < 0)
		printf("sunxi_dma_enable_int failed, error=%d\n", ret);

	ret = sunxi_dma_setting(g_codec_dma_handle, g_codec_tx_buf);
	if (ret < 0)
		printf("sunxi_dma_setting failed, error=%d\n", ret);

	return 0;
}

static void dump_wavheader(wav_header_t *wav)
{
	tone_dbg("wav header:\n");
	tone_dbg("channel: %u\n", wav->numChannels);
	tone_dbg("sample rate: %u\n", wav->sampleRate);
	tone_dbg("bytes per sec: %u\n", wav->bytesPerSecond);
	tone_dbg("sample resolution: %u\n", wav->bitsPerSample);
	tone_dbg("data size: %u\n", wav->dataSize);
}

static wav_header_t *wav_file_parser(wav_header_t *wav)
{
	if (strncmp("WAVE", wav->waveType, 4) == 0) {
		dump_wavheader(wav);
		return wav;
	}
	return NULL;
}

static int boot_tone_is_enable(u32 *limit)
{
	int node;
	char *dev_name = "boottone";
	const void *blob = gd->fdt_blob;

	/*node = fdt_node_offset_by_compatible(blob, 0, "boottone");*/
	node = fdt_node_offset_by_prop_value(blob, -1, "device_type", dev_name, strlen(dev_name) + 1);
	if (node < 0) {
		printf("unable to find boottone node in device tree.\n");
		return 0;
	}
	if (!fdtdec_get_is_enabled(blob, node)) {
		printf("boottone disabled in device tree\n");
		return 0;
	}
	if (fdt_getprop_u32(blob, node, "len_limit", limit) < 0)
		*limit = 0;

	return 1;
}

int sunxi_boot_tone_play(void)
{
	int ret;
	u32 buf_size;
	u32 *buf_start;
	wav_header_t *wav_header;
	u32 len_limit = 0;
	int workmode = get_boot_work_mode();

	/*printf("[%s] line:%d start\n", __func__, __LINE__);*/
	if (workmode != WORK_MODE_BOOT)
		return 0;
	if (!boot_tone_is_enable(&len_limit))
		return 0;

	g_tone_buffer = (void *)simple_strtoul(env_get("uboot_tone_addr"), NULL, 16);
	if (!g_tone_buffer)
		return 0;
	ret = run_command(env_get("load_boottone"), 0);
	if (ret == 0)
		printf("load boottone into %p success.\n", g_tone_buffer);
	else
		return 0;

	/* parser wav file */
	wav_header = wav_file_parser((wav_header_t *)g_tone_buffer);
	if (!wav_header) {
		printf("unknown wav header.\n");
		return 0;
	}

	sunxi_codec_probe();
	sunxi_codec_hw_params(wav_header->bitsPerSample, wav_header->sampleRate, wav_header->numChannels);
	buf_size = wav_header->dataSize;

	sunxi_boot_tone_dma_init();

	buf_start = (u32 *)(g_tone_buffer + sizeof(wav_header_t));
	tone_dbg("buffer start:%p, buffer size:%u\n", buf_start, buf_size);

	if (len_limit > 0 && len_limit < buf_size)
		buf_size = len_limit;

	sunxi_codec_playback_prepare();
	ret = sunxi_codec_playback_start(g_codec_dma_handle, buf_start, ALIGN(buf_size, 64));
	if (ret < 0)
		printf("sunxi_codec_dma_start failed, error=%d\n", ret);

	/*printf("[%s] line:%d end\n", __func__, __LINE__);*/
	return 0;
}

/*#define CPU_MODE 1*/
static int sunxi_boot_tone_test(void)
{
	int ret;
	u32 buf_size, st;
	u32 *buf_start;
	ulong timeout;
	wav_header_t *wav_header;

	g_tone_buffer = (void *)simple_strtoul(env_get("uboot_tone_addr"), NULL, 16);
	if (!g_tone_buffer)
		return 0;
	ret = run_command(env_get("load_boottone"), 0);
	if (ret == 0)
		printf("load boottone into %p success.\n", g_tone_buffer);
	else
		return 0;

	/* parser wav file */
	wav_header = wav_file_parser((wav_header_t *)g_tone_buffer);
	if (!wav_header) {
		printf("unknown wav header.\n");
		return 0;
	}

	sunxi_codec_probe();

	sunxi_codec_hw_params(wav_header->bitsPerSample, wav_header->sampleRate, wav_header->numChannels);
	buf_size = wav_header->dataSize;
#ifdef CPU_MODE
	/* Only for test */
	u16 *ptr = g_tone_buffer + sizeof(wav_header_t);
	u32 len = 0;

	sunxi_codec_playback_prepare();
	while (len < buf_size) {
		u32 data = (u32)(*ptr);
		sunxi_codec_fill_txfifo(&data);
		len += 2;
		ptr++;
		udelay(62);
	}

#else
	sunxi_boot_tone_dma_init();

	buf_start = g_tone_buffer + sizeof(wav_header_t);
	tone_dbg("buffer start:%p, buffer size:%u\n", buf_start, buf_size);

	sunxi_codec_playback_prepare();
	ret = sunxi_codec_playback_start(g_codec_dma_handle, buf_start, ALIGN(buf_size, 64));
	if (ret < 0)
		printf("sunxi_codec_dma_start failed, error=%d\n", ret);

#if 1
	timeout = get_timer(0);
	st = sunxi_dma_querystatus(g_codec_dma_handle);
	tone_dbg("dma st=0x%x\n", st);
#define BOOTTONE_TIMEOUT_LIMIT (10000)
	/* limit 5000ms, if music larger than 5s, will stop it */
	while ((get_timer(timeout) < BOOTTONE_TIMEOUT_LIMIT) && st)
		st = sunxi_dma_querystatus(g_codec_dma_handle);
	if (st)
		printf("wait dma timeout(%dms)!, status:0x%x\n", BOOTTONE_TIMEOUT_LIMIT, st);

	tone_dbg("dma st=0x%x\n", st);

	sunxi_dma_stop(g_codec_dma_handle);
	sunxi_dma_release(g_codec_dma_handle);

	free(g_codec_tx_buf);
	g_codec_tx_buf = NULL;
	g_codec_dma_handle = 0;
#endif

#endif //def CPU_MODE
	return 0;
}

int do_play_boot_tone(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	return  sunxi_boot_tone_test();
}

U_BOOT_CMD(boottone, 1, 1, do_play_boot_tone,
	   "play boot tone", "");
