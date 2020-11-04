/*
 * drivers/video/sunxi/disp2/hdmi/hdmi_edid.c
 *
 * Copyright (c) 2007-2019 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <zhengxiaobin@allwinnertech.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include "hdmi_core.h"

static s32 is_hdmi;
static s32 is_yuv;
u8 Device_Support_VIC[512];
static u8 EDID_Buf[HDMI_EDID_LEN];

u32 is_exp = 0;
u32 rgb_only = 0;
#if defined(__LINUX_PLAT__)

static u8 exp0[16] =
{
	0x36,0x74,0x4d,0x53,0x74,0x61,0x72,0x20,0x44,0x65,0x6d,0x6f,0x0a,0x20,0x20,0x38
};

static u8 exp1[16] =
{
	0x2d,0xee,0x4b,0x4f,0x4e,0x41,0x4b,0x20,0x54,0x56,0x0a,0x20,0x20,0x20,0x20,0xa5
};

static void ddc_init(void)
{

}

static void edid_read_data(u8 block,u8 *buf)
{
	u8 i;
	u8 * pbuf = buf + 128*block;
	u8 offset = (block&0x01)? 128:0;

	bsp_hdmi_ddc_read(Explicit_Offset_Address_E_DDC_Read,block>>1,offset,128,(char*)pbuf);

	////////////////////////////////////////////////////////////////////////////
	__inf("Sink : EDID bank %d:\n",block);

	__inf(" 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F\n");
	__inf(" ===============================================================================================\n");

	for (i = 0; i < 8; i++) {
		__inf(" %2.2x  %2.2x  %2.2x  %2.2x  %2.2x  %2.2x  %2.2x  %2.2x  %2.2x  %2.2x  %2.2x  %2.2x  %2.2x  %2.2x  %2.2x  %2.2x\n",
			pbuf[i*16 + 0 ],pbuf[i*16 + 1 ],pbuf[i*16 + 2 ],pbuf[i*16 + 3 ],
			pbuf[i*16 + 4 ],pbuf[i*16 + 5 ],pbuf[i*16 + 6 ],pbuf[i*16 + 7 ],
			pbuf[i*16 + 8 ],pbuf[i*16 + 9 ],pbuf[i*16 + 10],pbuf[i*16 + 11],
			pbuf[i*16 + 12],pbuf[i*16 + 13],pbuf[i*16 + 14],pbuf[i*16 + 15]
		);
	}
	__inf(" ===============================================================================================\n");

	return;
}

/////////////////////////////////////////////////////////////////////
// hdmi_edid_parse()
// Check EDID check sum and EDID 1.3 extended segment.
/////////////////////////////////////////////////////////////////////
static s32 edid_check_sum(u8 block,u8 *buf)
{
	s32 i = 0, CheckSum = 0;
	u8 *pbuf = buf + 128*block;

	for ( i = 0, CheckSum = 0 ; i < 128 ; i++ ) {
		CheckSum += pbuf[i];
		CheckSum &= 0xFF ;
	}
	if ( CheckSum != 0 ) {
		__inf("EDID block %d checksum error\n",block);
		return -1 ;
	}

	return 0;
}
static s32 edid_check_header(u8 *pbuf)
{
	if ( pbuf[0] != 0x00 ||
		pbuf[1] != 0xFF ||
		pbuf[2] != 0xFF ||
		pbuf[3] != 0xFF ||
		pbuf[4] != 0xFF ||
		pbuf[5] != 0xFF ||
		pbuf[6] != 0xFF ||
		pbuf[7] != 0x00)
	{
		__inf("EDID block0 header error\n");
		return -1 ;
	}

	return 0;
}

static s32 edid_check_version(u8 *pbuf)
{
	__inf("EDID version: %d.%d ",pbuf[0x12],pbuf[0x13]) ;
	if ( (pbuf[0x12]!= 0x01) || (pbuf[0x13]!=0x03)) {
		__inf("Unsupport EDID format,EDID parsing exit\n");
		return -1;
	}

	return 0;
}

static s32 edid_parse_dtd_block(u8 *pbuf)
{
	u32 	pclk,sizex,Hblanking,sizey,Vblanking,/*Hsync_offset,Hsync_plus,
	Vsync_offset,Vsync_plus,H_image_size,V_image_size,H_Border,
	V_Border,*/pixels_total,frame_rate;
	pclk 		= ( (u32)pbuf[1]	<< 8) + pbuf[0];
	sizex 		= (((u32)pbuf[4] 	<< 4) & 0x0f00) + pbuf[2];
	Hblanking 	= (((u32)pbuf[4] 	<< 8) & 0x0f00) + pbuf[3];
	sizey 		= (((u32)pbuf[7] 	<< 4) & 0x0f00) + pbuf[5];
	Vblanking 	= (((u32)pbuf[7] 	<< 8) & 0x0f00) + pbuf[6];
//	Hsync_offset= (((u32)pbuf[11] << 2) & 0x0300) + pbuf[8];
//	Hsync_plus 	= (((u32)pbuf[11] << 4) & 0x0300) + pbuf[9];
//	Vsync_offset= (((u32)pbuf[11] << 2) & 0x0030) + (pbuf[10] >> 4);
//	Vsync_plus 	= (((u32)pbuf[11] << 4) & 0x0030) + (pbuf[8] & 0x0f);
//	H_image_size= (((u32)pbuf[14] << 4) & 0x0f00) + pbuf[12];
//	V_image_size= (((u32)pbuf[14] << 8) & 0x0f00) + pbuf[13];
//	H_Border 	=  pbuf[15];
//	V_Border 	=  pbuf[16];

	pixels_total = (sizex + Hblanking) * (sizey + Vblanking);

	if ( (pbuf[0] == 0) && (pbuf[1] == 0) && (pbuf[2] == 0)) {
		return 0;
	}

	if (pixels_total == 0) {
		return 0;
	} else {
		frame_rate = (pclk * 10000) /pixels_total;
	}

	if ((frame_rate == 59) || (frame_rate == 60))	{
		if ((sizex== 720) && (sizey == 240)) {
			Device_Support_VIC[HDMI1440_480I] = 1;
		}
		if ((sizex== 720) && (sizey == 480)) {
			//Device_Support_VIC[HDMI480P] = 1;
		}
		if ((sizex== 1280) && (sizey == 720)) {
			Device_Support_VIC[HDMI720P_60] = 1;
		}
		if ((sizex== 1920) && (sizey == 540)) {
			Device_Support_VIC[HDMI1080I_60] = 1;
		}
		if ((sizex== 1920) && (sizey == 1080)) {
			Device_Support_VIC[HDMI1080P_60] = 1;
		}
	}
	else if ((frame_rate == 49) || (frame_rate == 50)) {
		if ((sizex== 720) && (sizey == 288)) {
			Device_Support_VIC[HDMI1440_576I] = 1;
		}
		if ((sizex== 720) && (sizey == 576)) {
			Device_Support_VIC[HDMI576P] = 1;
		}
		if ((sizex== 1280) && (sizey == 720)) {
			Device_Support_VIC[HDMI720P_50] = 1;
		}
		if ((sizex== 1920) && (sizey == 540)) {
			Device_Support_VIC[HDMI1080I_50] = 1;
		}
		if ((sizex== 1920) && (sizey == 1080)) {
			Device_Support_VIC[HDMI1080P_50] = 1;
		}
	}
	else if ((frame_rate == 23) || (frame_rate == 24)) {
		if ((sizex== 1920) && (sizey == 1080)) {
			Device_Support_VIC[HDMI1080P_24] = 1;
		}
	}
	__inf("PCLK=%d\tXsize=%d\tYsize=%d\tFrame_rate=%d\n",
	pclk*10000,sizex,sizey,frame_rate);

	return 0;
}

static s32 edid_parse_videodata_block(u8 *pbuf,u8 size)
{
	int i=0;
	while (i<size) {
		Device_Support_VIC[pbuf[i] &0x7f] = 1;
		if (pbuf[i] &0x80)	{
			__inf("edid_parse_videodata_block: VIC %d(native) support\n", pbuf[i]&0x7f);
		}
		else {
			__inf("edid_parse_videodata_block: VIC %d support\n", pbuf[i]);
		}
		i++;
	}

	return 0;
}

static s32 edid_parse_audiodata_block(u8 *pbuf,u8 size)
{
	u8 sum = 0;

	while (sum < size) {
		if ( (pbuf[sum]&0xf8) == 0x08) {
			__inf("edid_parse_audiodata_block: max channel=%d\n",(pbuf[sum]&0x7)+1);
			__inf("edid_parse_audiodata_block: SampleRate code=%x\n",pbuf[sum+1]);
			__inf("edid_parse_audiodata_block: WordLen code=%x\n",pbuf[sum+2]);
		}
		sum += 3;
	}
	return 0;
}

static s32 edid_parse_vsdb(u8 * pbuf,u8 size)
{
	u8 index = 8;
	u8 vic_len = 0;
	u8 i;

	/* check if it's HDMI VSDB */
	if ((pbuf[0] ==0x03) &&	(pbuf[1] ==0x0c) &&	(pbuf[2] ==0x00)) {
		is_hdmi = 1;
		__inf("Find HDMI Vendor Specific DataBlock\n");
	} else {
		is_hdmi = 0;
		is_yuv = 0;
		return 0;
	}

	if (size <=8)
		return 0;

	if ((pbuf[7]&0x20) == 0 )
		return 0;
	if ((pbuf[7]&0x40) == 0x40 )
		index = index +2;
	if ((pbuf[7]&0x80) == 0x80 )
		index = index +2;

	/* mandatary format support */
	if (pbuf[index]&0x80) {
		Device_Support_VIC[HDMI1080P_24_3D_FP] = 1;
		Device_Support_VIC[HDMI720P_50_3D_FP] = 1;
		Device_Support_VIC[HDMI720P_60_3D_FP] = 1;
		__inf("3D_present\n");
	}

	if ( ((pbuf[index]&0x60) ==1) || ((pbuf[index]&0x60) ==2) )
		__inf("3D_multi_present\n");

	vic_len = pbuf[index+1]>>5;
	for (i=0; i<vic_len; i++) {
		/* HDMI_VIC for extended resolution transmission */
		Device_Support_VIC[pbuf[index+1+1+i] + 0x100] = 1;
		__inf("edid_parse_vsdb: VIC %d support\n", pbuf[index+1+1+i]);
	}

	index += (pbuf[index+1]&0xe0) + 2;
	if (index > (size+1) )
	    return 0;

	__inf("3D_multi_present byte(%2.2x,%2.2x)\n",pbuf[index],pbuf[index+1]);

	return 0;
}

static s32 edid_check_special(u8 *buf_src, u8*buf_dst)
{
	u32 i;

	for (i = 0; i < 2; i++)
	{
		if (buf_dst[i] != buf_src[8+i])
			return -1;
	}
	for (i = 0; i < 13; i++)
	{
		if (buf_dst[2+i] != buf_src[0x5f+i])
			return -1;
	}
	if (buf_dst[15] != buf_src[0x7f])
		return -1;

	return 0;
}

s32 hdmi_edid_parse(void)
{
	//collect the EDID ucdata of segment 0
	u8 BlockCount ;
	u32 i,offset ;

	__inf("hdmi_edid_parse\n");

	memset(Device_Support_VIC,0,sizeof(Device_Support_VIC));
	memset(EDID_Buf,0,sizeof(EDID_Buf));
	is_hdmi = 1;
	is_yuv = 1;
	is_exp = 0;
	ddc_init();

	edid_read_data(0, EDID_Buf);

	if (edid_check_sum(0, EDID_Buf) != 0)
		return 0;

	if (edid_check_header(EDID_Buf)!= 0)
		return 0;

	if (edid_check_version(EDID_Buf)!= 0)
		return 0;

	edid_parse_dtd_block(EDID_Buf + 0x36);

	edid_parse_dtd_block(EDID_Buf + 0x48);

	BlockCount = EDID_Buf[0x7E];

	if ((edid_check_special(EDID_Buf,exp0) == 0)||
		(edid_check_special(EDID_Buf,exp1) == 0))
	{
		__wrn("*****************is_exp*****************\n");
		is_exp = 1;
	}

	if (BlockCount == 0) {
		is_hdmi = 0;
		is_yuv = 0;
		return 0;
	}

	if ( BlockCount > 0 ) {
		if ( BlockCount > 4 )
			BlockCount = 4 ;

		for ( i = 1 ; i <= BlockCount ; i++ ) {
			edid_read_data(i, EDID_Buf) ;
			if (edid_check_sum(i, EDID_Buf)!= 0)
				return 0;

			if ((EDID_Buf[0x80*i+0]==2)/*&&(EDID_Buf[0x80*i+1]==1)*/)
			{
				if ( (EDID_Buf[0x80*i+1]>=1)) {
					if (EDID_Buf[0x80*i+3]&0x20)
					{
						is_yuv = 1;
						__inf("device support YCbCr44 output\n");
						if (rgb_only == 1) {
							__inf("rgb only test!\n");
							is_yuv = 0;
						}
					} else
						is_yuv = 0;
				}

				offset = EDID_Buf[0x80*i+2];
				/* deal with reserved data block */
				if (offset > 4) {
					u8 bsum = 4;
					while (bsum < offset)
					{
						u8 tag = EDID_Buf[0x80*i+bsum]>>5;
						u8 len = EDID_Buf[0x80*i+bsum]&0x1f;
						if ( (len >0) && ((bsum + len + 1) > offset) ) {
							__inf("len or bsum size error\n");
							return 0;
						} else {
							if ( tag == 1) {
								/* ADB */
								edid_parse_audiodata_block(EDID_Buf+0x80*i+bsum+1,len);
							}	else if ( tag == 2) {
								/* VDB */
								edid_parse_videodata_block(EDID_Buf+0x80*i+bsum+1,len);
							}	else if ( tag == 3) {
								/* vendor specific */
								edid_parse_vsdb(EDID_Buf+0x80*i+bsum+1,len);
							}
						}

						bsum += (len +1);
					}
				} else {
					__inf("no data in reserved block%d\n",i);
				}

				/* deal with 18-byte timing block */
				if (offset >= 4)	{
					while (offset < (0x80-18)) {
						edid_parse_dtd_block(EDID_Buf + 0x80*i + offset);
						offset += 18;
					}
				} else {
					__inf("no datail timing in block%d\n",i);
				}
			}
		}
	}

	return 0 ;
}
#endif

static u32 hdmi_tx_support_vic[] = {
HDMI1440_480I,     HDMI1440_576I,      HDMI480P,
HDMI576P,          HDMI720P_50,        HDMI720P_60,
HDMI1080I_50,      HDMI1080I_60,       HDMI1080P_24,
HDMI1080P_50,      HDMI1080P_60,       HDMI1080P_25,
HDMI1080P_30,      HDMI1080P_24_3D_FP, HDMI720P_50_3D_FP,
HDMI720P_60_3D_FP, HDMI3840_2160P_30,  HDMI3840_2160P_25,
HDMI3840_2160P_24, HDMI4096_2160P_24,  HDMI1280_1024,
HDMI1024_768,      HDMI900_540,        HDMI1920_720};

s32 hdmi_edid_parse(void)
{
	int ret = -1;
	char buf = 0x0;

	ret = bsp_hdmi_ddc_read(Explicit_Offset_Address_E_DDC_Read, 0, 0x7e, 1, &buf);
	if (ret < 0) {
		is_yuv = 1;
	} else if (buf == 0) {
		is_yuv = 0;
	} else {

		ret = bsp_hdmi_ddc_read(Explicit_Offset_Address_E_DDC_Read, 0, 0x83, 1, &buf);
		if (ret < 0)
			is_yuv = 1;
		else if (buf & 0x20)
			is_yuv = 1;
		else
			is_yuv = 0;
	}

	return 0;
}

/**
* parse this edid vdb and check if this vdb support the init_vic
* params:
* 	addr: the address of this vdb    init_vic: the vic that will be checked
*       len: vdb payload length
*	*vdb_vic: return VICs that this vdb has
*return:
*	0: this vdb support the init vic
*      -1: this vdb do NOT support the init vic
*/
static s32 hdmi_edid_parse_vdb_and_check_vic_support(unsigned char addr,
						     int len,
						     int init_vic,
						     char *vdb_vic)
{
	u8 i = 0;
	char buf = 0x0;
	char vic_data[len];

	for (i = 0; i < len; i++) {
		/*Read VIC*/
		bsp_hdmi_ddc_read(Explicit_Offset_Address_E_DDC_Read, 0,
					addr + i, 1, &buf);
		vic_data[i] = buf & 0x7f;
		if (init_vic == vic_data[i])
			break;
	}

	if (i < len) {
		return 0;
	} else {
		/*conpy the vics to the vdb_vic[]*/
		memcpy(vdb_vic, vic_data, len);
		return -1;
	}
}

/*check if HDMI TX support a vic*/
static bool hdmi_check_vic_tx_support(int vic)
{
	unsigned int tx_vic_cn = 0, j;

	tx_vic_cn = sizeof(hdmi_tx_support_vic) / sizeof(u32);
	for (j = 0; j < tx_vic_cn; j++)
		if (vic == hdmi_tx_support_vic[j])
			return true;
	return false;
}

/**
* check if this edid vdb support the init vic and return the support vic
* params:
*	addr: the address of this vdb    init_vic: the vic that will be checked
*       len: vdb payload length
* return:
*	>0: return the vic that this vdb support and HDMI TX also support
*	-1: there is no vic that HDMI TX support in this vdb
*       -2: read edid failed
*/
static s32 hdmi_edid_get_vdb_support_vic(unsigned char addr, int len,
						int init_vic)
{
	int ret = 0, i;
	char vdb_vic[50] = {0x0};

	/*To check if VDB support init_vic*/
	ret = hdmi_edid_parse_vdb_and_check_vic_support(addr, len,
							init_vic, vdb_vic);
	if (ret == 0) {/*init vic is supported by VDB*/
		return init_vic;
	} else if (ret == -1) {/*init vic is NOT supported by VDB*/
		/*find a VDB VIC that HDMI TX also support*/
		for (i = 0; (i < 50) && vdb_vic[i]; i++)
			if (hdmi_check_vic_tx_support(vdb_vic[i]))
				return vdb_vic[i];
		printf ("WARN: No vdb vic that TX support\n");
		return -1;
	} else {/*read edid failed*/
		return -2;
	}

}

/**
* parse vdb's vics one by one until to get the one that hdmi_tx support
* return value:
*	>0: return the vic that hdmi TX support in vsdb
*       -1: there is NO vic that hdmi TX support in vsdb
*	-2: read edid failed
*/
static s32 hdmi_edid_parse_vdb_and_get_tx_support_vic(void)
{
	int ret = 0;
	u8 i = 0;
	char buf = 0x0, len = 0;
	unsigned char addr = 0x84;

	/*read the data block tag and length*/
	ret = bsp_hdmi_ddc_read(Explicit_Offset_Address_E_DDC_Read, 0, addr, 1, &buf);
	if (ret >= 0) {
		if (buf && (((buf & 0xe0) >> 5) == 0x2)) {/*it is VDB*/
			len = buf & 0x1f;
			for (i = 0; i < len; i++) {
				/*read vic*/
				bsp_hdmi_ddc_read(Explicit_Offset_Address_E_DDC_Read, 0,
						addr + 1 + i, 1, &buf);
				/*check if TX support this vic*/
				if (hdmi_check_vic_tx_support(buf))
					return (s32)buf;
			}

			if (i >= len) /*No vic that TX support*/
				return -1;
		} else if (buf && (((buf & 0xe0) >> 5) != 0x2)) {/*it is not VDB*/
			addr = addr + 1 + (buf & 0x1f);
			/*read the next data block*/
			bsp_hdmi_ddc_read(Explicit_Offset_Address_E_DDC_Read, 0,
						addr, 1, &buf);
			if (((buf & 0xe0) >> 5) != 0x2) {/*The next data block is also not VDB*/
				printf("Do not find the VDB, buf:0x%02x\n", buf);
				return -1;
			} else {/*The next data block is VDB*/
				for (i = 0; i < len; i++) {
					/*read vic*/
					bsp_hdmi_ddc_read(Explicit_Offset_Address_E_DDC_Read, 0,
						addr + 1 + i, 1, &buf);
					/*check if TX support this vic*/
					if (hdmi_check_vic_tx_support(buf))
						return (s32)buf;
				}
				if (i >= len)/*No vic that TX support*/
					return -1;

			}
		} else {
			printf("Error: it is NOT data block\n");
			return -1;
		}
	} else {
		printf("WARN:Read flag of edid cea_extension data block failed-3\n");
		return -2;
	}

	return -1;
}

/**
* check if hdmi edid cea_extension vdb block support the init vic, if support
* then return the init_vic, if not:
* 1.return a vic that vdb has and HDMI TX also support;
* 2.return -1 if there is no vic that HDMI TX support in edid vdb
*
* return -2 if edid read failed;
*/
static s32 hdmi_check_vic_edid_vdb_support_and_get_support_vic(int init_vic)
{
	int ret = 0;
	char buf = 0x0;
	unsigned char addr = 0x84;

	/*read data block tag and length*/
	ret = bsp_hdmi_ddc_read(Explicit_Offset_Address_E_DDC_Read, 0, addr, 1, &buf);
	if (ret >= 0) {
		if (buf && (((buf & 0xe0) >> 5) == 0x2)) { /*it is VDB*/
			/*return the vic that VDB support*/
			return hdmi_edid_get_vdb_support_vic(addr + 1, buf & 0x1f,
								init_vic);
		} else if (buf && (((buf & 0xe0) >> 5) != 0x2)) {/*it is NOT VDB*/
			addr = addr + 1 + (buf & 0x1f);
			/*read the Next data block*/
			ret = bsp_hdmi_ddc_read(Explicit_Offset_Address_E_DDC_Read,
						0, addr, 1, &buf);
			if (ret >= 0) {
				if (((buf & 0xe0) >> 5) != 0x2) {
					printf("Do not find VSDB-1\n");
					return -1;
				}

				return hdmi_edid_get_vdb_support_vic(addr + 1,
								     buf & 0x1f,
								     init_vic);
			} else {
				printf("WARN: Read flag of edid cea_extension data block failed-2");
				return -2;
			}
		} else {
			printf("Error: it is not data block-1\n");
			return -1;
		}
	} else {
		printf("WARN:Read flag of edid cea_extension data block failed-1\n");
		return -2;
	}

}

/**
* check if edid cea_extension vsdb support 4k@24/25/30 and 3d
* params:
*	init_vic: the VIRTUAL vic that will be check if it is supported by vsdb
*	NOTE: This VIC is not true vic, it is just be defined as the virtual VIC
*	to represent the 4k and 3d
* return value:
*	 0: indicate the vertual vic is supported by vsdb
*	-1: the vsdb do not support 4k or 3d
*       -2: read edid failed
*/
static s32 hdmi_check_vic_edid_vsdb_support(int init_vic)
{
	int ret = 0;
	char buf;

	ret = bsp_hdmi_ddc_read(Explicit_Offset_Address_E_DDC_Read, 0, 0xbc, 1, &buf);
	if (ret >= 0) {
		if (((init_vic == HDMI1080P_24_3D_FP)
			|| (init_vic == HDMI720P_50_3D_FP)
			|| (init_vic == HDMI720P_60_3D_FP))) {
			if (buf & 0x1f)
				return 0;
			else
				return -1;
		} else if ((init_vic == HDMI3840_2160P_30)
			  || (init_vic == HDMI3840_2160P_25)
			  || (init_vic == HDMI3840_2160P_24)
			  || (init_vic == HDMI4096_2160P_24)) {
			if (buf & 0xe0)
				return 0;
			else
				return -1;
		}
	} else {
		printf("Warn: HDMI read EDID VSDB failed\n");
		return -2;
	}

	return -1;
}

/**
* check if edid block0 support Detailed Timing Descriptor #0/#1/#2/#3
* return value:
*	>0: return one of Detailed Timing Descriptor that edid block0 support
*	    NOTE: HDMI_DT0/1/2/3 is not true VICs, They are just virtual VICS
*	    defined to represent Detailed Timing Descriptor #0/#1/#2/#3
*	-1: edid block0 do NOT support Detailed Timing Descriptors
*	    or support Detailed Timing Descriptors, but in which HDMI TX do not
*	    support the resolution
*	-2: read edid failed
*/
static s32 hdmi_edid_get_dt_support(void)
{
	char buf[2];
	unsigned int i = 0, dt_addr;
	int ret = -1;
	u16 pixel_clk = 0;

	for (i = 0; i < 4; i++) {
		dt_addr = 54 + i * 18;
		ret = bsp_hdmi_ddc_read(Explicit_Offset_Address_E_DDC_Read, 0,
							dt_addr, 2, buf);
		if (ret >= 0) {
			pixel_clk = (buf[1] << 8) + buf[0];
			if (pixel_clk <= 29700)
				return HDMI_DT0 + i;
		} else {
			printf("WARN: read edid dt block failed\n");
			return -2;
		}
	}

	return -1;
}

/**
* check if edid block0 support Detailed Timing Descriptor init_vic
* params:
*	init_vic: the VIRTUAL vic that will be check if it is supported by vsdb
*	NOTE: This VIC is not true vic, it is just be defined as the virtual VIC
*	to represent the Detailed Timing Descriptor #0/#1/#2/#3
* return value:
*	 0: indicate the vertual vic is supported by edid block0
*	-1: edid block0 do NOT support Detailed Timing Descriptors
*	    or support Detailed Timing Descriptors, but in which HDMI TX do not
*	    support
*       -2: read edid failed
*/
static s32 hdmi_check_vic_edid_dt_support(int init_vic)
{
	unsigned int dt_addr = 0, dt_num = 0;
	u16 pixel_clk = 0;
	char buf[2];
	int ret = 0;

	if ((init_vic < HDMI_DT0) && (init_vic < HDMI_DT3)) {
		printf("ERROR: error dt init_vic\n");
		return -1;
	}

	dt_num = init_vic - HDMI_DT0;
	dt_addr = 54 + dt_num * 18;
	ret = bsp_hdmi_ddc_read(Explicit_Offset_Address_E_DDC_Read, 0, dt_addr, 2, buf);
	if (ret >= 0) {
		pixel_clk = (buf[1] << 8) + buf[0];
		if (pixel_clk) {
			if (pixel_clk > 29700) {
				printf("WARN:DT%d pixel_clk:%d is too big\n",
					dt_num, pixel_clk);
				return -1;
			} else {
				return 0;
			}
		} else {
			return -1;
		}
	} else {
		printf("Warn: HDMI read detailed descriptor %d failed\n", dt_num);
		return init_vic;
	}
}

static s32 hdmi_check_other_vic(int init_vic)
{
	if (hdmi_check_vic_tx_support(init_vic))
		return 0;
	else
		return -1;
}

/**
* check if Rx's edid support the init_vic and get the vic
* that Rx's edid and HDMI TX both supported
* params:
*	init_vic: the vic that will be checked
* return value:
*	the vic that Rx's edid and HDMI TX both supported
*/
s32 hdmi_edid_check_init_vic_and_get_supported_vic(int init_vic)
{
	int ret = 0, ret1 = 0, ret2 = 0;

	if ((0 < init_vic) && (init_vic < 0x80)) {
		ret = hdmi_check_vic_edid_vdb_support_and_get_support_vic(init_vic);
		if (ret > 0) {
			return ret;
		} else if (ret == -1) {
			ret1 = hdmi_edid_get_dt_support();
			if (ret1 > 0)
				return ret;
			else
				return init_vic;
		} else {
			return init_vic;
		}
	} else if ((init_vic == HDMI1080P_24_3D_FP)
		|| (init_vic == HDMI720P_50_3D_FP)
		|| (init_vic == HDMI720P_60_3D_FP)
		|| (init_vic == HDMI3840_2160P_30)
		|| (init_vic == HDMI3840_2160P_25)
		|| (init_vic == HDMI3840_2160P_24)
		|| (init_vic == HDMI4096_2160P_24)) {
		ret = hdmi_check_vic_edid_vsdb_support(init_vic);
		if (ret == 0) {
			return init_vic;
		} else if (ret == -1) {
			ret1 = hdmi_edid_parse_vdb_and_get_tx_support_vic();
			if (ret1 > 0) {
				return ret1;
			} else if (ret1 == -1) {
				ret2 = hdmi_edid_get_dt_support();
				if (ret2 > 0)
					return ret2;
				else
					return init_vic;
			} else {
				return init_vic;
			}
		} else {
			return init_vic;
		}

	} else if ((init_vic >= HDMI_DT0) && (init_vic <= HDMI_DT3)) {
		ret = hdmi_check_vic_edid_dt_support(init_vic);
		if (ret == 0) {
			return init_vic;
		} else if (ret == -1) {
			ret1 = hdmi_edid_parse_vdb_and_get_tx_support_vic();
			if (ret1 > 0) {
				return ret1;
			} else if (ret1 == -1) {
				ret2 = hdmi_edid_get_dt_support();
				if (ret2 > 0)
					return ret2;
				else
					return HDMI480P;
			} else {
				return HDMI480P;
			}
		}
	} else {
		if (hdmi_check_other_vic(init_vic)) {
			return init_vic;
		} else {
			printf("Error: unsupport vic\n");
			return -1;
		}
	}

	return -1;
}

s32 hdmi_get_edid_dt_timing_info(int dt_mode, struct disp_video_timings *timing)
{
	char buf[18];
	u8 dt_num = 0;
	u32 dt_addr = 0;
	u32 hactive, hblank, vactive, vblank, hsync_offset, hsync;
	u32 vsync_offset, vsync;
	u8 interlace, vpol, hpol;
	int ret = 0;
	int i;

	if ((dt_mode < HDMI_DT0) || (dt_mode > HDMI_DT3)) {
		printf("ERROR HDMI EDID DT Mode\n");
		return -1;
	}

	dt_num = dt_mode - 0x120;
	dt_addr = 54 + dt_num * 18;

	ret = bsp_hdmi_ddc_read(Explicit_Offset_Address_E_DDC_Read, 0, dt_addr, 18, buf);
	if (ret < 0) {
		printf("error: can not read edid,");
		printf("please do not select hdmi_mode = 2 in sys_config\n");
		return -1;
	} else {
		for (i = 0; i < 18; i++)
			printf("EDIDBUF[%d]:0x%02x\n", i, buf[i]);
		hactive = ((buf[4] & 0xf0) << 4) + buf[2];
		hblank = ((buf[4] & 0x0f) << 8) + buf[3];
		vactive = ((buf[7] & 0xf0) << 4) + buf[5];
		vblank = ((buf[7] & 0x0f) << 8) + buf[6];
		hsync_offset = ((buf[11] & 0xc0) << 2) + buf[8];
		hsync = ((buf[11] & 0x30) << 4) + buf[9];
		vsync_offset = ((buf[11] & 0x0c) << 6) + ((buf[10] & 0xf0) >> 4);
		vsync = ((buf[11] & 0x03) << 8) + ((buf[10] & 0x0f) >> 0);
		interlace = (buf[17] & 0x80) ? 1 : 0;
		vpol = (buf[17] & 0x04) ? 1 : 0;
		hpol = (buf[17] & 0x02) ? 1 : 0;

		timing->pixel_clk = 10000 * (buf[0] + (buf[1] << 8));
		timing->pixel_repeat = 0;
		timing->x_res = hactive;
		timing->y_res = vactive * (interlace + 1);
		timing->hor_total_time = hactive + hblank;
		timing->hor_back_porch = hblank - hsync - hsync_offset;
		timing->hor_front_porch = hsync_offset;
		timing->hor_sync_time = hsync;
		timing->ver_total_time = (vactive + vblank) * (interlace + 1);
		timing->ver_back_porch = vblank - vsync - vsync_offset;
		timing->ver_front_porch = vsync_offset;
		timing->ver_sync_time = vsync;
		timing->hor_sync_polarity = hpol;
		timing->ver_sync_polarity = vpol;
		timing->b_interlace = interlace;

		return 0;
	}
}

u32 hdmi_edid_is_hdmi(void)
{
	return is_hdmi;
}

u32 hdmi_edid_is_yuv(void)
{
	return is_yuv;
}

uintptr_t hdmi_edid_get_data(void)
{
	return (uintptr_t)EDID_Buf;
}

