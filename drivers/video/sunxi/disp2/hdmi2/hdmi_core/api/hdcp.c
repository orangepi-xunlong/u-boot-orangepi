/*
 * Allwinner SoCs hdmi2.0 driver.
 *
 * Copyright (C) 2016 Allwinner.
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */



#include "hdcp.h"
#include "log.h"

void hdcp_rxdetect(hdmi_tx_dev_t *dev, u8 enable)
{
	LOG_TRACE1(enable);
	dev_write_mask(dev, A_HDCPCFG0, A_HDCPCFG0_RXDETECT_MASK, enable);
}

static void _DataEnablePolarity(hdmi_tx_dev_t *dev, u8 bit)
{
	LOG_TRACE1(bit);
	dev_write_mask(dev, A_VIDPOLCFG, A_VIDPOLCFG_DATAENPOL_MASK, bit);
}

void _DisableEncryption(hdmi_tx_dev_t *dev, u8 bit)
{
	LOG_TRACE1(bit);
	dev_write_mask(dev, A_HDCPCFG1, A_HDCPCFG1_ENCRYPTIONDISABLE_MASK, bit);
}

static void _BypassEncryption(hdmi_tx_dev_t *dev, u8 bit)
{
	LOG_TRACE1(bit);
	dev_write_mask(dev, A_HDCPCFG0, A_HDCPCFG0_BYPENCRYPTION_MASK, bit);
}

/*static void _HSyncPolarity(hdmi_tx_dev_t *dev, u8 bit)
{
	LOG_TRACE1(bit);
	dev_write_mask(dev, A_VIDPOLCFG, A_VIDPOLCFG_HSYNCPOL_MASK, bit);
}


static void _VSyncPolarity(hdmi_tx_dev_t *dev, u8 bit)
{
	LOG_TRACE1(bit);
	dev_write_mask(dev, A_VIDPOLCFG, A_VIDPOLCFG_VSYNCPOL_MASK, bit);
}*/

/*static void hdcp22_ovr_val_1p4(hdmi_tx_dev_t *dev)
{
	dev_write_mask(dev, (HDCP22REG_CTRL),
		HDCP22REG_CTRL_HDCP22_OVR_VAL_MASK, 0);
	dev_write_mask(dev, (HDCP22REG_CTRL),
		HDCP22REG_CTRL_HDCP22_OVR_EN_MASK, 1);
}*/

void _setDeviceMode(hdmi_tx_dev_t *dev, video_mode_t mode)
{
	u8 set_mode;

	LOG_TRACE1(mode);

	set_mode = (mode == HDMI ? 1 : 0);  /* 1 - HDMI : 0 - DVI */
	dev_write_mask(dev, A_HDCPCFG0, A_HDCPCFG0_HDMIDVI_MASK, set_mode);
}

/*static void _EnableFeature11(hdmi_tx_dev_t *dev, u8 bit)
{
	LOG_TRACE1(bit);
	dev_write_mask(dev, A_HDCPCFG0, A_HDCPCFG0_EN11FEATURE_MASK, bit);
}

static void _RiCheck(hdmi_tx_dev_t *dev, u8 bit)
{
	LOG_TRACE1(bit);
	dev_write_mask(dev, A_HDCPCFG0, A_HDCPCFG0_SYNCRICHECK_MASK, bit);
}*/

/*static void _EnableI2cFastMode(hdmi_tx_dev_t *dev, u8 bit)
{
	LOG_TRACE1(bit);
	dev_write_mask(dev, A_HDCPCFG0, A_HDCPCFG0_I2CFASTMODE_MASK, bit);
}

static void _EnhancedLinkVerification(hdmi_tx_dev_t *dev, u8 bit)
{
	LOG_TRACE1(bit);
	dev_write_mask(dev, A_HDCPCFG0, A_HDCPCFG0_ELVENA_MASK, bit);
}*/

static void _EnableAvmute(hdmi_tx_dev_t *dev, u8 bit)
{
	LOG_TRACE1(bit);
	dev_write_mask(dev, A_HDCPCFG0, A_HDCPCFG0_AVMUTE_MASK, bit);
}

/*static void _UnencryptedVideoColor(hdmi_tx_dev_t *dev, u8 value)
{
	LOG_TRACE1(value);
	dev_write_mask(dev, A_VIDPOLCFG, A_VIDPOLCFG_UNENCRYPTCONF_MASK, value);
}*/

static void _OessWindowSize(hdmi_tx_dev_t *dev, u8 value)
{
	LOG_TRACE1(value);
	dev_write(dev, (A_OESSWCFG), value);
}

/*static void _EncodingPacketHeader(hdmi_tx_dev_t *dev, u8 bit)
{
	LOG_TRACE1(bit);
	dev_write_mask(dev, A_HDCPCFG1, A_HDCPCFG1_PH2UPSHFTENC_MASK, bit);
}*/

int hdcp_initialize(hdmi_tx_dev_t *dev)
{
	LOG_TRACE();
	i2cddc_fast_mode(dev, 0);
	i2cddc_clk_config(dev, I2C_SFR_CLK,
				I2C_MIN_SS_SCL_LOW_TIME,
				I2C_MIN_SS_SCL_HIGH_TIME,
				I2C_MIN_FS_SCL_LOW_TIME,
				I2C_MIN_FS_SCL_HIGH_TIME);
	_OessWindowSize(dev, 64);
	fc_video_hdcp_keepout(dev, true);
	dev_write_mask(dev, A_HDCPCFG0, A_HDCPCFG0_HDMIDVI_MASK,
				(dev->snps_hdmi_ctrl.hdmi_on == HDMI) ? 1 : 0);
	/*_HSyncPolarity(dev, (dev->snps_hdmi_ctrl.hspol > 0) ? 1 : 0);
	_VSyncPolarity(dev, (dev->snps_hdmi_ctrl.vspol > 0) ? 1 : 0);*/
	_BypassEncryption(dev, TRUE);
	hdcp_rxdetect(dev, 0);
	_DataEnablePolarity(dev, 0);
	_DisableEncryption(dev, 1);

	return TRUE;
}

#if defined(__LINUX_PLAT__)
#define KSV_LEN  5 /* KSV value size */

/* HDCP Interrupt fields */
#define INT_KSV_ACCESS    (A_APIINTSTAT_KSVACCESSINT_MASK)
#define INT_KSV_SHA1      (A_APIINTSTAT_KSVSHA1CALCINT_MASK)
#define INT_KSV_SHA1_DONE (A_APIINTSTAT_KSVSHA1CALCDONEINT_MASK)
#define INT_HDCP_FAIL     (A_APIINTSTAT_HDCP_FAILED_MASK)
#define INT_HDCP_ENGAGED  (A_APIINTSTAT_HDCP_ENGAGED_MASK)

#define SIZE	(160/8)
#define KSIZE	(1024/8)

static void sha_process_block(hdmi_tx_dev_t *dev, sha_t *sha);
static void sha_pad_message(hdmi_tx_dev_t *dev, sha_t *sha);
static int hdcp_array_add(hdmi_tx_dev_t *dev, u8 *r, const u8 *a,
						const u8 *b, size_t n);
static int hdcp_array_cmp(hdmi_tx_dev_t *dev, const u8 *a,
						const u8 *b, size_t n);
static int hdcp_array_mac(hdmi_tx_dev_t *dev, u8 *r, const u8 *M,
						const u8 m, size_t n);
static int hdcp_array_mul(hdmi_tx_dev_t *dev, u8 *r, const u8 *M,
						const u8 *m, size_t n);
static void hdcp_array_set(hdmi_tx_dev_t *dev, u8 *dst,
						const u8 src, size_t n);
static int hdcp_array_usb(hdmi_tx_dev_t *dev, u8 *r, const u8 *a,
						const u8 *b, size_t n);
static void hdcp_array_swp(hdmi_tx_dev_t *dev, u8 *r, size_t n);
static int hdcp_array_tst(hdmi_tx_dev_t *dev, const u8 *a,
						const u8 b, size_t n);
static int hdcp_compute_exp(hdmi_tx_dev_t *dev, u8 *c, const u8 *M,
				const u8 *e, const u8 *p, size_t n, size_t nE);
static int hdcp_compute_inv(hdmi_tx_dev_t *dev, u8 *out, const u8 *z,
						const u8 *a, size_t n);
static int hdcp_compute_mod(hdmi_tx_dev_t *dev, u8 *dst,
					const u8 *src, const u8 *p, size_t n);
static int hdcp_compute_mul(hdmi_tx_dev_t *dev, u8 *p,
			const u8 *a, const u8 *b, const u8 *m, size_t n);
static int hdcp_verify_ksv(hdmi_tx_dev_t *dev, const u8 *data, size_t size);
static void hdcp_array_cpy(hdmi_tx_dev_t *dev, u8 *dst,
						const u8 *src, size_t n);
static int hdcp_array_div(hdmi_tx_dev_t *dev, u8 *r, const u8 *D,
						const u8 *d, size_t n);


void hdcp_sw_reset(hdmi_tx_dev_t *dev)
{
	LOG_TRACE();
	/* Software reset signal, active by writing a zero
	and auto cleared to 1 in the following cycle */
	dev_write_mask(dev, A_HDCPCFG1, A_HDCPCFG1_SWRESET_MASK, 0);
}

static void _InterruptClear(hdmi_tx_dev_t *dev, u8 value)
{
	dev_write(dev, (A_APIINTCLR), value);
}

static u8 _InterruptStatus(hdmi_tx_dev_t *dev)
{
	return dev_read(dev, A_APIINTSTAT);
}

static void _InterruptMask(hdmi_tx_dev_t *dev, u8 value)
{
	dev_write(dev, (A_APIINTMSK), value);
}

static u8 _InterruptMaskStatus(hdmi_tx_dev_t *dev)
{
	return dev_read(dev, A_APIINTMSK);
}

static void _MemoryAccessRequest(hdmi_tx_dev_t *dev, u8 bit)
{
	LOG_TRACE();
	dev_write_mask(dev, A_KSVMEMCTRL, A_KSVMEMCTRL_KSVMEMREQUEST_MASK, bit);
}

static u8 _MemoryAccessGranted(hdmi_tx_dev_t *dev)
{
	LOG_TRACE();
	return (u8)((dev_read(dev, A_KSVMEMCTRL)
		& A_KSVMEMCTRL_KSVMEMACCESS_MASK) >> 1);
}

static void _UpdateKsvListState(hdmi_tx_dev_t *dev, u8 bit)
{
	LOG_TRACE1(bit);
	dev_write_mask(dev, A_KSVMEMCTRL, A_KSVMEMCTRL_SHA1FAIL_MASK, bit);
	dev_write_mask(dev, A_KSVMEMCTRL, A_KSVMEMCTRL_KSVCTRLUPD_MASK, 1);
	dev_write_mask(dev, A_KSVMEMCTRL, A_KSVMEMCTRL_KSVCTRLUPD_MASK, 0);
}

u8 _ksv_sha1_status(hdmi_tx_dev_t *dev)
{
	LOG_TRACE();
	return (u8)((dev_read(dev, A_KSVMEMCTRL)
		& A_KSVMEMCTRL_KSVSHA1STATUS_MASK) >> 4);
}

static u16 _BStatusRead(hdmi_tx_dev_t *dev)
{
	u16 bstatus = 0;

	bstatus	= dev_read(dev, HDCP_BSTATUS);
	bstatus	|= dev_read(dev, HDCP_BSTATUS + ADDR_JUMP) << 8;
	return bstatus;
}

void _M0Read(hdmi_tx_dev_t *dev, u8 *data)
{
	u8 i = 0;

	for (i = 0; i < HDCP_M0_SIZE; i++)
		data[i] = dev_read(dev, HDCP_M0 + (i * ADDR_JUMP));
}
#if 0
int _KsvListRead(hdmi_tx_dev_t *dev, u16 size, u8 *data)
{
	u8 i = 0;

	if (size > HDCP_KSV_SIZE) {
		/* LOGGER(SNPS_ERROR,"Invalid number of devices"); */
		return -1;
	}

	for (i = 0; i < size; i++)
		data[i] = dev_read(dev, HDCP_KSV + (i * ADDR_JUMP));

	return 0;
}
#endif

static u8 _hdcp_2p2_version(hdmi_tx_dev_t *dev)
{
	HDMI_INFO_MSG("TBI\n");
	/* 1 - Configure the RX FIFO to read HDCP 2.2 version: */
	/* a. Write 8'h50 to h22s_rxmsg_addr.rxmsg_addr. */
	/* b. Write 8'h01 to h22s_rxmsg_nbyteslow.rxmsg_nbytes_lo. */
	/* c. Write 2'b00 to h22s_rxmsg_nbyteshigh.rxmsg_nbytes_hi. */
	/* d. Write 1'b0 to h22s_rxmsg_cfg.rxmsg_rcv_autostart. */
	/* e. Write 1'b1 to h22s_rxmsg_ctrl.rxmsg_rcv_start. */

	/* 2 - Wait for the interrupt hdcp2version_chg to be asserted. */

	/* 3 - The HDCP 2.2 version can be read by the software
		using two different procedures: */
	/* a. Read h22s_hdcp2version_sts.hdcp2version_sts. */
	/* Or */
	/* b. Read from h22s_rxmsg_byte.rxmsg_byte. */
	/* Write 1'b1 to h22s_rxmsg_ff_ctrl.rxmsgfifo_pop */
	return 0x0;
}

static u8 _hdcp_2p2_reset_engine(hdmi_tx_dev_t *dev)
{
	HDMI_INFO_MSG("TBI\n");
	/* Reset the HDCP 2.2 engine,
	write “1” in the h22s_ctrl.swrstreq bit field register. */
	return TRUE;
}

static u8 _hdcp_2p2_authentication(hdmi_tx_dev_t *dev)
{
	HDMI_INFO_MSG("TBI\n");
	/* Perform the HDCP 2.2 authentication as described in the
	“HDCP 2.2 Authentication” on page 228. */
	/* Skip the remaining steps as they are related to HDMI 1.4. */
	return TRUE;
}

void hdcp_params_reset(hdmi_tx_dev_t *dev, hdcpParams_t *params)
{
	params->mEnable11Feature = 0;
	params->mRiCheck = 1;
	params->mI2cFastMode = 0;
	params->mEnhancedLinkVerification = 0;
	params->maxDevices = 0;

	if (params->mKsvListBuffer != NULL)
		kfree(params->mKsvListBuffer);
	params->mKsvListBuffer = NULL;
}

static void sha_reset(hdmi_tx_dev_t *dev, sha_t *sha)
{
	size_t i = 0;

	sha->mIndex = 0;
	sha->mComputed = FALSE;
	sha->mCorrupted = FALSE;
	for (i = 0; i < sizeof(sha->mLength); i++)
		sha->mLength[i] = 0;

	sha->mDigest[0] = 0x67452301;
	sha->mDigest[1] = 0xEFCDAB89;
	sha->mDigest[2] = 0x98BADCFE;
	sha->mDigest[3] = 0x10325476;
	sha->mDigest[4] = 0xC3D2E1F0;
}

static int sha_result(hdmi_tx_dev_t *dev, sha_t *sha)
{
	if (sha->mCorrupted == TRUE)
		return FALSE;

	if (sha->mComputed == FALSE) {
		sha_pad_message(dev, sha);
		sha->mComputed = TRUE;
	}
	return TRUE;
}

static void sha_input(hdmi_tx_dev_t *dev, sha_t *sha,
				const u8 *data, size_t size)
{
	int i = 0;
	unsigned j = 0;
	int rc = TRUE;

	if (data == 0 || size == 0) {
		HDMI_ERROR_MSG("invalid input data\n");
		return;
	}
	if (sha->mComputed == TRUE || sha->mCorrupted == TRUE) {
		sha->mCorrupted = TRUE;
		return;
	}
	while (size-- && sha->mCorrupted == FALSE) {
		sha->mBlock[sha->mIndex++] = *data;

		for (i = 0; i < 8; i++) {
			rc = TRUE;
			for (j = 0; j < sizeof(sha->mLength); j++) {
				sha->mLength[j]++;
				if (sha->mLength[j] != 0) {
					rc = FALSE;
					break;
				}
			}
			sha->mCorrupted = (sha->mCorrupted == TRUE
					   || rc == TRUE) ? TRUE : FALSE;
		}
		/* if corrupted then message is too long */
		if (sha->mIndex == 64)
			sha_process_block(dev, sha);

		data++;
	}
}

static void sha_process_block(hdmi_tx_dev_t *dev, sha_t *sha)
{
	#define shaCircularShift(bits, word) ((((word) << (bits)) & 0xFFFFFFFF) | ((word) >> (32-(bits))))
	const unsigned K[] = {	/* constants defined in SHA-1 */
		0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6
	};
	unsigned W[80];		/* word sequence */
	unsigned A, B, C, D, E;	/* word buffers */
	unsigned temp = 0;
	int t = 0;

	/* Initialize the first 16 words in the array W */
	for (t = 0; t < 80; t++) {
		if (t < 16) {
			W[t] = ((unsigned)sha->mBlock[t * 4 + 0]) << 24;
			W[t] |= ((unsigned)sha->mBlock[t * 4 + 1]) << 16;
			W[t] |= ((unsigned)sha->mBlock[t * 4 + 2]) << 8;
			W[t] |= ((unsigned)sha->mBlock[t * 4 + 3]) << 0;
		} else {
			W[t] =
			    shaCircularShift(1,
					     W[t - 3] ^ W[t - 8] ^ W[t -
							14] ^ W[t - 16]);
		}
	}

	A = sha->mDigest[0];
	B = sha->mDigest[1];
	C = sha->mDigest[2];
	D = sha->mDigest[3];
	E = sha->mDigest[4];

	for (t = 0; t < 80; t++) {
		temp = shaCircularShift(5, A);
		if (t < 20)
			temp += ((B & C) | ((~B) & D)) + E + W[t] + K[0];
		else if (t < 40)
			temp += (B ^ C ^ D) + E + W[t] + K[1];
		else if (t < 60)
			temp += ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];
		else
			temp += (B ^ C ^ D) + E + W[t] + K[3];

		E = D;
		D = C;
		C = shaCircularShift(30, B);
		B = A;
		A = (temp & 0xFFFFFFFF);
	}

	sha->mDigest[0] = (sha->mDigest[0] + A) & 0xFFFFFFFF;
	sha->mDigest[1] = (sha->mDigest[1] + B) & 0xFFFFFFFF;
	sha->mDigest[2] = (sha->mDigest[2] + C) & 0xFFFFFFFF;
	sha->mDigest[3] = (sha->mDigest[3] + D) & 0xFFFFFFFF;
	sha->mDigest[4] = (sha->mDigest[4] + E) & 0xFFFFFFFF;

	sha->mIndex = 0;
}

static void sha_pad_message(hdmi_tx_dev_t *dev, sha_t *sha)
{
	/*
	 *  Check to see if the current message block is too small to hold
	 *  the initial padding bits and length.  If so, we will pad the
	 *  block, process it, and then continue padding into a second
	 *  block.
	 */
	if (sha->mIndex > 55) {
		sha->mBlock[sha->mIndex++] = 0x80;
		while (sha->mIndex < 64)
			sha->mBlock[sha->mIndex++] = 0;

		sha_process_block(dev, sha);
		while (sha->mIndex < 56)
			sha->mBlock[sha->mIndex++] = 0;

	} else {
		sha->mBlock[sha->mIndex++] = 0x80;
		while (sha->mIndex < 56)
			sha->mBlock[sha->mIndex++] = 0;
	}

	/* Store the message length as the last 8 octets */
	sha->mBlock[56] = sha->mLength[7];
	sha->mBlock[57] = sha->mLength[6];
	sha->mBlock[58] = sha->mLength[5];
	sha->mBlock[59] = sha->mLength[4];
	sha->mBlock[60] = sha->mLength[3];
	sha->mBlock[61] = sha->mLength[2];
	sha->mBlock[62] = sha->mLength[1];
	sha->mBlock[63] = sha->mLength[0];

	sha_process_block(dev, sha);
}

static int hdcp_array_add(hdmi_tx_dev_t *dev, u8 *r,
			const u8 *a, const u8 *b, size_t n)
{
	u8 c = 0;
	size_t i = 0;

	for (i = 0; i < n; i++) {
		u16 s = a[i] + b[i] + c;

		c = (u8) (s >> 8);
		r[i] = (u8) s;
	}
	return c;
}

static int hdcp_array_cmp(hdmi_tx_dev_t *dev, const u8 *a,
						const u8 *b, size_t n)
{
	int i = 0;

	for (i = n; i > 0; i--) {
		if (a[i - 1] > b[i - 1])
			return 1;
		else if (a[i - 1] < b[i - 1])
			return -1;
	}
	return 0;
}

static void hdcp_array_cpy(hdmi_tx_dev_t *dev, u8 *dst, const u8 *src, size_t n)
{
	size_t i = 0;

	for (i = 0; i < n; i++)
		dst[i] = src[i];
}

static int hdcp_array_mac(hdmi_tx_dev_t *dev, u8 *r, const u8 *M,
							const u8 m, size_t n)
{
	u16 c = 0;
	size_t i = 0;

	for (i = 0; i < n; i++) {
		u16 p = (M[i] * m) + c + r[i];

		c = p >> 8;
		r[i] = (u8) p;
	}
	return (u8) c;
}

static int hdcp_array_mul(hdmi_tx_dev_t *dev, u8 *r,
				const u8 *M, const u8 *m, size_t n)
{
	size_t i = 0;

	if (r == M || r == m) {
		HDMI_ERROR_MSG("invalid input data\n");
		return FALSE;
	}
	hdcp_array_set(dev, r, 0, n);
	for (i = 0; i < n; i++) {
		if (m[i] == 0)
			continue;
		else if (m[i] == 1)
			hdcp_array_add(dev, &r[i], &r[i], M, n - i);
		else
			hdcp_array_mac(dev, &r[i], M, m[i], n - i);
	}
	return TRUE;
}

static void hdcp_array_set(hdmi_tx_dev_t *dev, u8 *dst, const u8 src, size_t n)
{
	size_t i = 0;

	for (i = 0; i < n; i++)
		dst[i] = src;
}

static int hdcp_array_usb(hdmi_tx_dev_t *dev, u8 *r,
				const u8 *a, const u8 *b, size_t n)
{
	u8 c = 1;
	size_t i = 0;

	for (i = 0; i < n; i++) {
		u16 s = ((u8) a[i] + (u8) (~b[i])) + c;

		c = (u8) (s >> 8);
		r[i] = (u8) s;
	}
	return c;
}

static void hdcp_array_swp(hdmi_tx_dev_t *dev, u8 *r, size_t n)
{
	size_t i = 0;

	for (i = 0; i < (n / 2); i++) {
		u8 tmp = r[i];

		r[i] = r[n - 1 - i];
		r[n - 1 - i] = tmp;
	}
}

static int hdcp_array_tst(hdmi_tx_dev_t *dev, const u8 *a, const u8 b, size_t n)
{
	size_t i = 0;

	for (i = 0; i < n; i++) {
		if (a[i] != b)
			return FALSE;
	}
	return TRUE;
}

static int hdcp_array_div(hdmi_tx_dev_t *dev, u8 *r,
				const u8 *D, const u8 *d, size_t n)
{
	int i = 0;

	if (r == D || r == d || !hdcp_array_tst(dev, d, 0, n) == TRUE) {
		HDMI_ERROR_MSG("invalid input data\n");
		return FALSE;
	}
	hdcp_array_set(dev, &r[n], 0, n);
	hdcp_array_cpy(dev, r, D, n);
	for (i = n; i > 0; i--) {
		r[i - 1 + n] = 0;
		while (hdcp_array_cmp(dev, &r[i - 1], d, n) >= 0) {
			hdcp_array_usb(dev, &r[i - 1], &r[i - 1], d, n);
			r[i - 1 + n] += 1;
		}
	}
	return TRUE;
}

static int hdcp_compute_exp(hdmi_tx_dev_t *dev, u8 *c, const u8 *M,
						const u8 *e, const u8 *p,
			  size_t n, size_t nE)
{
	int i = 8 * nE - 1;
	int rc = TRUE;

	/* LR Binary Method */
	if ((e[i / 8] & (1 << (i % 8))) != 0) {
		hdcp_array_cpy(dev, c, M, n);
	} else {
		hdcp_array_set(dev, c, 0, n);
		c[0] = 1;
	}
	for (i -= 1; i >= 0; i--) {
		rc |= hdcp_compute_mul(dev, c, c, c, p, n);
		if ((e[i / 8] & (1 << (i % 8))) != 0)
			rc &= hdcp_compute_mul(dev, c, c, M, p, n);

	}
	return rc;
}

static int hdcp_compute_inv(hdmi_tx_dev_t *dev, u8 *out,
				const u8 *z, const u8 *a, size_t n)
{
	u8 w[2][SIZE];
	u8 x[2][SIZE];
	u8 y[2][SIZE];
	u8 r[2 * SIZE];
	u8 *i, *j, *q, *t;
	u8 *x1, *x2;
	u8 *y1, *y2;

	if ((n > SIZE) || (hdcp_array_tst(dev, z, 0, n) == TRUE)
	    || (hdcp_array_tst(dev, a, 0, n) == TRUE)
	    || (hdcp_array_cmp(dev, z, a, n) >= 0)) {
		HDMI_ERROR_MSG("invalid input data\n");
		return FALSE;
	}

	hdcp_array_cpy(dev, w[0], a, n);
	hdcp_array_cpy(dev, w[1], z, n);
	i = w[0];
	j = w[1];

	hdcp_array_set(dev, x[1], 0, n);
	x[1][0] = 1;
	hdcp_array_set(dev, x[0], 0, n);
	x2 = x[1];
	x1 = x[0];

	hdcp_array_set(dev, y[1], 0, n);
	hdcp_array_set(dev, y[0], 0, n);
	y[0][0] = 1;
	y2 = y[1];
	y1 = y[0];

	do {
		hdcp_array_div(dev, r, i, j, n);
		hdcp_array_cpy(dev, i, r, n);
		q = &r[n];
		t = i;		/* swap i <-> j */
		i = j;
		j = t;

		hdcp_array_mul(dev, r, x1, q, n);
		hdcp_array_usb(dev, x2, x2, r, n);
		t = x2;		/* swap x1 <-> x2 */
		x2 = x1;
		x1 = t;

		hdcp_array_mul(dev, r, y1, q, n);
		hdcp_array_usb(dev, y2, y2, r, n);
		t = y2;		/* swap y1 <-> y2 */
		y2 = y1;
		y1 = t;

	} while (hdcp_array_tst(dev, j, 0, n) == FALSE);

	j[0] = 1;
	if (hdcp_array_cmp(dev, i, j, n) != 0) {
		HDMI_ERROR_MSG("i != 1\n");
		return FALSE;
	}
	hdcp_array_cpy(dev, out, y2, n);
	return TRUE;
}

static int hdcp_compute_mod(hdmi_tx_dev_t *dev, u8 *dst,
				const u8 *src, const u8 *p, size_t n)
{
	u8 aux[KSIZE];
	u8 ext[SIZE + 1];
	u8 tmp[2 * (KSIZE + 1)];
	int i = 0;

	if (n > SIZE) {
		HDMI_ERROR_MSG("invalid input data\n");
		return FALSE;
	}
	hdcp_array_cpy(dev, aux, src, sizeof(aux));
	hdcp_array_cpy(dev, ext, p, n);
	ext[n] = 0;
	for (i = sizeof(aux) - n - 1; i >= 0; i--) {
		hdcp_array_div(dev, tmp, &aux[i], ext, n + 1);
		hdcp_array_cpy(dev, &aux[i], tmp, n + 1);
	}
	hdcp_array_cpy(dev, dst, aux, n);
	return TRUE;
}

static int hdcp_compute_mul(hdmi_tx_dev_t *dev, u8 *p,
			const u8 *a, const u8 *b, const u8 *m, size_t n)
{
	u8 aux[2 * KSIZE + 1];
	u8 ext[KSIZE + 1];
	u8 tmp[2 * (KSIZE + 1)];
	size_t i = 0;
	int j = 0;

	if (n > KSIZE) {
		HDMI_ERROR_MSG("invalid input data\n");
		return FALSE;
	}
	hdcp_array_set(dev, aux, 0, sizeof(aux));
	for (i = 0; i < n; i++)
		aux[n + i] = hdcp_array_mac(dev, &aux[i], a, b[i], n);

	hdcp_array_cpy(dev, ext, m, n);
	ext[n] = 0;
	for (j = n; j >= 0; j--) {
		hdcp_array_div(dev, tmp, &aux[j], ext, n + 1);
		hdcp_array_cpy(dev, &aux[j], tmp, n + 1);
	}
	hdcp_array_cpy(dev, p, aux, n);
	return TRUE;
}

static int hdcp_verify_ksv(hdmi_tx_dev_t *dev, const u8 *data, size_t size)
{
	size_t i = 0;
	sha_t sha;

	LOG_TRACE1((int)size);

	if (data == 0 || size < (HEADER + SHAMAX)) {
		HDMI_ERROR_MSG("invalid input data\n");
		return FALSE;
	}
	sha_reset(dev, &sha);
	sha_input(dev, &sha, data, size - SHAMAX);

	if (sha_result(dev, &sha) == FALSE) {
		HDMI_ERROR_MSG("cannot process SHA digest\n");
		return FALSE;
	}

	for (i = 0; i < SHAMAX; i++) {
		if (data[size - SHAMAX + i] !=
				(u8) (sha.mDigest[i / 4] >> ((i % 4) * 8))) {
			HDMI_ERROR_MSG("SHA digest does not match\n");
			return FALSE;
		}
	}
	return TRUE;
}

int hdcp_verify_srm(hdmi_tx_dev_t *dev, const u8 *data, size_t size)
{
	LOG_TRACE1((int)size);
	if (data == 0 || size < (VRL_HEADER + VRL_NUMBER + 2 * DSAMAX)) {
		HDMI_ERROR_MSG("invalid input data\n");
		return FALSE;
	}
	/* M, n, r, s */
	return hdcp_verify_dsa(dev, data, size - 2 * DSAMAX,
			&data[(int)size - 2 * DSAMAX], &data[size - DSAMAX]);
}

int hdcp_verify_dsa(hdmi_tx_dev_t *dev, const u8 *M, size_t n,
					const u8 *r, const u8 *s)
{
	int i = 0;
	sha_t sha;
	int ret;
	static const u8 q[] = {
		0xE7, 0x08, 0xC7, 0xF9, 0x4D, 0x3F, 0xEF, 0x97, 0xE2, 0x14, 0x6D,
		0xCD, 0x6A, 0xB5, 0x6D, 0x5E, 0xCE, 0xF2, 0x8A, 0xEE
	};
	static const u8 p[] = {
		0x27, 0x75, 0x28, 0xF3, 0x2B, 0x80, 0x59, 0x8C, 0x11, 0xC2, 0xED,
		0x46, 0x1C, 0x95, 0x39, 0x2A, 0x54, 0x19, 0x89, 0x96, 0xFD, 0x49,
		0x8A, 0x02, 0x3B, 0x73, 0x75, 0x32, 0x14, 0x9C, 0x7B, 0x5C, 0x49,
		0x20, 0x98, 0xB9, 0x07, 0x32, 0x3F, 0xA7, 0x30, 0x15, 0x72, 0xB3,
		0x09, 0x55, 0x71, 0x10, 0x3A, 0x4C, 0x97, 0xD1, 0xBC, 0xA0, 0x04,
		0xF4, 0x35, 0xCF, 0x47, 0x54, 0x0E, 0xA7, 0x2B, 0xE5, 0x83, 0xB9,
		0xC6, 0xD4, 0x47, 0xC7, 0x44, 0xB8, 0x67, 0x76, 0x7C, 0xAE, 0x0C,
		0xDC, 0x34, 0x4F, 0x4B, 0x9E, 0x96, 0x1D, 0x82, 0x84, 0xD2, 0xA0,
		0xDC, 0xE0, 0x00, 0xF5, 0x64, 0xA1, 0x7F, 0x8E, 0xFF, 0x58, 0x70,
		0x6A, 0xC3, 0x4F, 0xA2, 0xA1, 0xB8, 0xC7, 0x52, 0x5A, 0x35, 0x5B,
		0x39, 0x17, 0x6B, 0x78, 0x43, 0x93, 0xF7, 0x75, 0x8D, 0x01, 0xB7,
		0x61, 0x17, 0xFD, 0xB2, 0xF5, 0xC3, 0xD3
	};
	static const u8 g[] = {
		0xD9, 0x0B, 0xBA, 0xC2, 0x42, 0x24, 0x46, 0x69, 0x5B, 0x40, 0x67,
		0x2F, 0x5B, 0x18, 0x3F, 0xB9, 0xE8, 0x6F, 0x21, 0x29, 0xAC, 0x7D,
		0xFA, 0x51, 0xC2, 0x9D, 0x4A, 0xAB, 0x8A, 0x9B, 0x8E, 0xC9, 0x42,
		0x42, 0xA5, 0x1D, 0xB2, 0x69, 0xAB, 0xC8, 0xE3, 0xA5, 0xC8, 0x81,
		0xBE, 0xB6, 0xA0, 0xB1, 0x7F, 0xBA, 0x21, 0x2C, 0x64, 0x35, 0xC8,
		0xF7, 0x5F, 0x58, 0x78, 0xF7, 0x45, 0x29, 0xDD, 0x92, 0x9E, 0x79,
		0x3D, 0xA0, 0x0C, 0xCD, 0x29, 0x0E, 0xA9, 0xE1, 0x37, 0xEB, 0xBF,
		0xC6, 0xED, 0x8E, 0xA8, 0xFF, 0x3E, 0xA8, 0x7D, 0x97, 0x62, 0x51,
		0xD2, 0xA9, 0xEC, 0xBD, 0x4A, 0xB1, 0x5D, 0x8F, 0x11, 0x86, 0x27,
		0xCD, 0x66, 0xD7, 0x56, 0x5D, 0x31, 0xD7, 0xBE, 0xA9, 0xAC, 0xDE,
		0xAF, 0x02, 0xB5, 0x1A, 0xDE, 0x45, 0x24, 0x3E, 0xE4, 0x1A, 0x13,
		0x52, 0x4D, 0x6A, 0x1B, 0x5D, 0xF8, 0x92
	};
#ifndef FACSIMILE
	static const u8 y[] = {
		0x99, 0x37, 0xE5, 0x36, 0xFA, 0xF7, 0xA9, 0x62, 0x83, 0xFB, 0xB3,
		0xE9, 0xF7, 0x9D, 0x8F, 0xD8, 0xCB, 0x62, 0xF6, 0x66, 0x8D, 0xDC,
		0xC8, 0x95, 0x10, 0x24, 0x6C, 0x88, 0xBD, 0xFF, 0xB7, 0x7B, 0xE2,
		0x06, 0x52, 0xFD, 0xF7, 0x5F, 0x43, 0x62, 0xE6, 0x53, 0x65, 0xB1,
		0x38, 0x90, 0x25, 0x87, 0x8D, 0xA4, 0x9E, 0xFE, 0x56, 0x08, 0xA7,
		0xA2, 0x0D, 0x4E, 0xD8, 0x43, 0x3C, 0x97, 0xBA, 0x27, 0x6C, 0x56,
		0xC4, 0x17, 0xA4, 0xB2, 0x5C, 0x8D, 0xDB, 0x04, 0x17, 0x03, 0x4F,
		0xE1, 0x22, 0xDB, 0x74, 0x18, 0x54, 0x1B, 0xDE, 0x04, 0x68, 0xE1,
		0xBD, 0x0B, 0x4F, 0x65, 0x48, 0x0E, 0x95, 0x56, 0x8D, 0xA7, 0x5B,
		0xF1, 0x55, 0x47, 0x65, 0xE7, 0xA8, 0x54, 0x17, 0x8A, 0x65, 0x76,
		0x0D, 0x4F, 0x0D, 0xFF, 0xAC, 0xA3, 0xE0, 0xFB, 0x80, 0x3A, 0x86,
		0xB0, 0xA0, 0x6B, 0x52, 0x00, 0x06, 0xC7
	};
#else
	static const u8 y[] = {
		0x46, 0xB9, 0xC2, 0xE5, 0xBE, 0x57, 0x3B, 0xA6,
		0x22, 0x7B, 0xAA, 0x83, 0x81, 0xA9, 0xD2, 0x0F,
		0x03, 0x2E, 0x0B, 0x70, 0xAC, 0x96, 0x42, 0x85,
		0x4E, 0x78, 0x8A, 0xDF, 0x65, 0x35, 0x97, 0x6D,
		0xE1, 0x8D, 0xD1, 0x7E, 0xA3, 0x83, 0xCA, 0x0F,
		0xB5, 0x8E, 0xA4, 0x11, 0xFA, 0x14, 0x6D, 0xB1,
		0x0A, 0xCC, 0x5D, 0xFF, 0xC0, 0x8C, 0xD8, 0xB1,
		0xE6, 0x95, 0x72, 0x2E, 0xBD, 0x7C, 0x85, 0xDE,
		0xE8, 0x52, 0x69, 0x92, 0xA0, 0x22, 0xF7, 0x01,
		0xCD, 0x79, 0xAF, 0x94, 0x83, 0x2E, 0x01, 0x1C,
		0xD7, 0xEF, 0x86, 0x97, 0xA3, 0xBB, 0xCB, 0x64,
		0xA6, 0xC7, 0x08, 0x5E, 0x8E, 0x5F, 0x11, 0x0B,
		0xC0, 0xE8, 0xD8, 0xDE, 0x47, 0x2E, 0x75, 0xC7,
		0xAA, 0x8C, 0xDC, 0xB7, 0x02, 0xC4, 0xDF, 0x95,
		0x31, 0x74, 0xB0, 0x3E, 0xEB, 0x95, 0xDB, 0xB0,
		0xCE, 0x11, 0x0E, 0x34, 0x9F, 0xE1, 0x13, 0x8D
	};
#endif

	u8 w[SIZE];
	u8 z[SIZE];
	u8 u1[SIZE];
	u8 u2[SIZE];
	u8 gu1[KSIZE];
	u8 yu2[KSIZE];
	u8 pro[KSIZE];
	u8 v[SIZE];

	/* adapt to the expected format by arithmetic functions */
	u8 r1[SIZE];
	u8 s1[SIZE];

	sha_reset(dev, &sha);
	hdcp_array_cpy(dev, r1, r, sizeof(r1));
	hdcp_array_cpy(dev, s1, s, sizeof(s1));
	hdcp_array_swp(dev, r1, sizeof(r1));
	hdcp_array_swp(dev, s1, sizeof(s1));

	hdcp_compute_inv(dev, w, s1, q, sizeof(w));
	sha_input(dev, &sha, M, n);
	if (sha_result(dev, &sha) == TRUE) {
		for (i = 0; i < 5; i++) {
			z[i * 4 + 0] = sha.mDigest[i] >> 24;
			z[i * 4 + 1] = sha.mDigest[i] >> 16;
			z[i * 4 + 2] = sha.mDigest[i] >> 8;
			z[i * 4 + 3] = sha.mDigest[i] >> 0;
		}
		hdcp_array_swp(dev, z, sizeof(z));
	} else {
		HDMI_ERROR_MSG("cannot digest message\n");
		return FALSE;
	}
	if (hdcp_compute_mul(dev, u1, z, w, q, sizeof(u1)) == FALSE)
		return FALSE;

	if (hdcp_compute_mul(dev, u2, r1, w, q, sizeof(u2)) == FALSE)
		return FALSE;

	if (hdcp_compute_exp(dev, gu1, g, u1, p, sizeof(gu1), sizeof(u1)) ==
	    FALSE)
		return FALSE;

	if (hdcp_compute_exp(dev, yu2, y, u2, p, sizeof(yu2), sizeof(u2)) ==
	    FALSE)
		return FALSE;

	if (hdcp_compute_mul(dev, pro, gu1, yu2, p, sizeof(pro)) == FALSE)
		return FALSE;

	if (hdcp_compute_mod(dev, v, pro, q, sizeof(v)) == FALSE)
		return FALSE;
	ret = (hdcp_array_cmp(dev, v, r1, sizeof(v)) == 0);
	return ret;
}

void hdcp_1p4_configure(hdmi_tx_dev_t *dev, hdcpParams_t *hdcp)
{
	LOG_TRACE();
	/* HDCP only */
	_EnableFeature11(dev, (hdcp->mEnable11Feature > 0) ? 1 : 0);
	_RiCheck(dev, (hdcp->mRiCheck > 0) ? 1 : 0);
	_EnableI2cFastMode(dev,	(hdcp->mI2cFastMode > 0) ? 1 : 0);
	_EnhancedLinkVerification(dev,
			(hdcp->mEnhancedLinkVerification > 0) ? 1 : 0);

	/* fixed */
	_EnableAvmute(dev, FALSE);
	_UnencryptedVideoColor(dev, 0x02);
	_EncodingPacketHeader(dev, TRUE);

	/* 9 - Set encryption */
	_OessWindowSize(dev, 64);
	_BypassEncryption(dev, FALSE);
	/* _DisableEncryption(dev, FALSE); */

	/* 10 - Reset the HDCP 1.4 engine */
	hdcp_sw_reset(dev);

	/* 12 - Enable encryption */
	hdcp_rxdetect(dev, 1);

	HDMI_INFO_MSG("HDCP enable interrupts\n");

	_InterruptClear(dev, A_APIINTCLR_KSVACCESSINT_MASK |
					A_APIINTCLR_KSVSHA1CALCINT_MASK |
					A_APIINTCLR_KEEPOUTERRORINT_MASK |
					A_APIINTCLR_LOSTARBITRATION_MASK |
					A_APIINTCLR_I2CNACK_MASK |
					A_APIINTCLR_HDCP_FAILED_MASK |
					A_APIINTCLR_HDCP_ENGAGED_MASK);

	_InterruptMask(dev, (~(A_APIINTMSK_KSVACCESSINT_MASK |
					     A_APIINTMSK_KSVSHA1CALCINT_MASK |
					     A_APIINTMSK_KEEPOUTERRORINT_MASK |
					     A_APIINTMSK_LOSTARBITRATION_MASK |
					     A_APIINTMSK_I2CNACK_MASK |
					     A_APIINTMSK_SPARE_MASK |
					     A_APIINTMSK_HDCP_FAILED_MASK |
					     A_APIINTMSK_HDCP_ENGAGED_MASK)) &
					     _InterruptMaskStatus(dev));

	/* _DisableEncryption(dev, FALSE); */
}

int hdcp_configure(hdmi_tx_dev_t *dev, hdcpParams_t *hdcp, videoParams_t *video)
{
	video_mode_t mode = dev->snps_hdmi_ctrl.hdmi_on;
	u8 hsPol = video->mDtd.mHSyncPolarity;
	u8 vsPol = video->mDtd.mVSyncPolarity;
	static int hdcp_2p2;
	u8 version = 0;

	if (dev->snps_hdmi_ctrl.hdcp_on == 0) {
		HDMI_INFO_MSG("HDCP is not active\n");
		return TRUE;
	}

	/* Before configure HDCP we should configure the internal parameters */
	hdcp->maxDevices = 128;
	hdcp->mI2cFastMode = 0;
	if (hdcp->mKsvListBuffer == NULL)
		hdcp->mKsvListBuffer = kmalloc(sizeof(u8) * 670, GFP_KERNEL);
	memcpy(&dev->hdcp, hdcp, sizeof(hdcpParams_t));

	/* 1 - To determine if the controller supports HDCP */
	if (id_product_type(dev) != 0xC1) {
		HDMI_ERROR_MSG("Controller does not supports HDCP\n");
		return FALSE;
	}

	/* 2 - To determine the HDCP version of the transmitter */
	if (id_hdcp22_support(dev) == FALSE) {
		HDMI_INFO_MSG("HDCP 2.2 SNPS is not present (HDCP 1.4 support only)");
		hdcp_2p2 = 0;
	} else {
		HDMI_INFO_MSG("HDCP 2.2 SNPS is present i (both HDCP 1.4 and HDCP 2.2 versions supported)");
		hdcp_2p2 = 1;
	}

	/* 3 - Select DVI or HDMI mode */
	HDMI_INFO_MSG("Set HDCP %s\n", mode == HDMI ? "HDMI" : "DVI");
	dev_write_mask(dev, A_HDCPCFG0, A_HDCPCFG0_HDMIDVI_MASK,
						(mode == HDMI) ? 1 : 0);

	/* 4 - Set the Data enable, Hsync, and VSync polarity */
	_HSyncPolarity(dev, (hsPol > 0) ? 1 : 0);
	_VSyncPolarity(dev, (vsPol > 0) ? 1 : 0);
	_DataEnablePolarity(dev,
		(dev->snps_hdmi_ctrl.data_enable_polarity > 0) ? 1 : 0);

	/* 5 - If hdcp22_snps read in Step 2 is 0
	(Synopsys HDCP 2.2 not supported), skip to Step 9. */
	if (hdcp_2p2 == 0) {
		HDMI_INFO_MSG("HDCP 2.2 not supported - configuring HDCP 1.4");
		hdcp_1p4_configure(dev, hdcp);
		return TRUE;
	} else {
		HDMI_INFO_MSG("Configuring HDCP 2.2");
		/* 6 - If hdcp22_snps read in Step 2 is 1
		(Synopsys HDCP 2.2 is supported): */
		/* Read HDCP2Version through the HDCP 2.2
		Synopsys RX Message FIFO and select HDCP version in */
		/* the Transmitter (refer to “Reading HDCP 2.2 Version
		(HDCP Port 0x50)” on page 83). */
		version = _hdcp_2p2_version(dev);

		if (version == 0x00) {
			/* a. If HDCP2Version is 0x00,
			write “0” to mc_opctrl.h22s_ovr_val
			(this selects HDCP 1.4) */
			dev_write_mask(dev, 0x1000C, 1 << 5, 0x0);
			hdcp22_ovr_val_1p4(dev);
			HDMI_INFO_MSG("HDCP 2.2 not supported - configuring HDCP 1.4");
			hdcp_1p4_configure(dev, hdcp);
			return TRUE;
		} else if (version == 0x04) {
			/* b. If HDCP2Version is 0x04,
			write “1” to mc_opctrl.h22s_ovr_val
			(this selects HDCP 2.2).*/
			dev_write_mask(dev, 0x1000C, 1 << 5, 0x1);
			/* c. Write “1” to mc_opctrl.h22s_switch_lck. */
			dev_write_mask(dev, 0x1000C, 1 << 4, 0x1);
		}
	}

	/* 7 - Reset the HDCP 2.2 engine */
	_hdcp_2p2_reset_engine(dev);

	/* 8 - Perform the HDCP 2.2 authentication */
	_hdcp_2p2_authentication(dev);

	return TRUE;
}

void hdcp22_configure(hdmi_tx_dev_t *dev, hdcpParams_t *hdcp, videoParams_t *video)
{

}

u8 read_sink_hdcp_type(void)
{
	return 0;
}

void hdcp_configure_new(hdmi_tx_dev_t *dev, hdcpParams_t *hdcp, videoParams_t *video)
{
	u8 hdcp_type = 0;
	int success = 0;

	if (dev->snps_hdmi_ctrl.hdcp_encrypt_on)
		return;

	hdcp_type = read_sink_hdcp_type();
	if (hdcp_type == 1) {
		hdcp22_configure(dev, hdcp, video);
	} else {
		success = hdcp_configure(dev, hdcp, video);
		if (success == false)
			pr_info("ERROR:Could not configure HDCP\n");
	}

	dev->snps_hdmi_ctrl.hdcp_encrypt_on = 1;
}

void hdcp_disconfigure_new(hdmi_tx_dev_t *dev)
{

}

int get_hdcp_status(void)
{
	return 0;
}

/* SHA-1 calculation by Software */
u8 _read_ksv_list(hdmi_tx_dev_t *dev, int *param)
{
	int timeout = 1000;
	u16 bstatus = 0;
	u16 deviceCount = 0;
	int valid = HDCP_IDLE;
	int size = 0;
	int i = 0;

	u8 *hdcp_ksv_list_buffer = dev->hdcp.mKsvListBuffer;

	/* 1 - Wait for an interrupt to be triggered
		(a_apiintstat.KSVSha1calcint) */
	/* This is called from the INT_KSV_SHA1 irq
		so nothing is required for this step */

	/* 2 - Request access to KSV memory through
		setting a_ksvmemctrl.KSVMEMrequest to 1'b1 and */
	/* pool a_ksvmemctrl.KSVMEMaccess until
		this value is 1'b1 (access granted). */
	_MemoryAccessRequest(dev, TRUE);
	while (_MemoryAccessGranted(dev) == 0 && timeout--)
		asm volatile ("nop");

	if (_MemoryAccessGranted(dev) == 0) {
		_MemoryAccessRequest(dev, FALSE);
		HDMI_INFO_MSG("KSV List memory access denied");
		*param = 0;
		return HDCP_KSV_LIST_ERR_MEM_ACCESS;
	}

	/* 3 - Read VH', M0, Bstatus, and the KSV FIFO.
	The data is stored in the revocation memory, as */
	/* provided in the "Address Mapping for Maximum Memory Allocation"
	table in the databook. */
	bstatus = _BStatusRead(dev);
	deviceCount = bstatus & BSTATUS_DEVICE_COUNT_MASK;

	if (deviceCount > dev->hdcp.maxDevices) {
		*param = 0;
		HDMI_INFO_MSG("depth exceeds KSV List memory");
		return HDCP_KSV_LIST_ERR_DEPTH_EXCEEDED;
	}

	size = deviceCount * KSV_LEN + HEADER + SHAMAX;

	for (i = 0; i < size; i++) {
		if (i < HEADER) { /* BSTATUS & M0 */
			hdcp_ksv_list_buffer[(deviceCount * KSV_LEN) + i] =
			(u8)dev_read(dev, HDCP_BSTATUS + (i * ADDR_JUMP));
		} else if (i < (HEADER + (deviceCount * KSV_LEN))) { /* KSV list */
			hdcp_ksv_list_buffer[i - HEADER] =
			(u8)dev_read(dev, HDCP_BSTATUS + (i * ADDR_JUMP));
		} else { /* SHA */
			hdcp_ksv_list_buffer[i] = (u8)dev_read(dev,
				HDCP_BSTATUS + (i * ADDR_JUMP));
		}
	}

	/* 4 - Calculate the SHA-1 checksum (VH) over M0,
		Bstatus, and the KSV FIFO. */
	if (hdcp_verify_ksv(dev, hdcp_ksv_list_buffer, size) == TRUE) {
		valid = HDCP_KSV_LIST_READY;
		HDMI_INFO_MSG("HDCP_KSV_LIST_READY");
	} else{
		valid = HDCP_ERR_KSV_LIST_NOT_VALID;
		HDMI_INFO_MSG("HDCP_ERR_KSV_LIST_NOT_VALID");
	}

	/* 5 - If the calculated VH equals the VH',
	set a_ksvmemctrl.SHA1fail to 0 and set */
	/* a_ksvmemctrl.KSVCTRLupd to 1.
	If the calculated VH is different from VH' then set */
	/* a_ksvmemctrl.SHA1fail to 1 and set a_ksvmemctrl.KSVCTRLupd to 1,
	forcing the controller */
	/* to re-authenticate from the beginning. */
	_MemoryAccessRequest(dev, 0);
	_UpdateKsvListState(dev, (valid == HDCP_KSV_LIST_READY) ? 0 : 1);

	return valid;
}

u8 hdcp_event_handler(hdmi_tx_dev_t *dev, int *param, u32 irq_stat)
{
	u8 interrupt_status = 0;
	int valid = HDCP_IDLE;

	LOG_TRACE();
	interrupt_status = irq_stat;

	HDMI_INFO_MSG("hdcp_interrupt_status 0x%x\n", interrupt_status);

	if (interrupt_status == 0) {
		HDMI_INFO_MSG("HDCP_IDLE\n");
		return HDCP_IDLE;
	}

	if (interrupt_status & INT_KSV_SHA1) {
		HDMI_INFO_MSG("INT_KSV_SHA1\n");
		return _read_ksv_list(dev, param);
	}

	if ((interrupt_status & INT_HDCP_FAIL) != 0) {
		*param = 0;
		HDMI_INFO_MSG("HDCP_FAILED\n");
		return HDCP_FAILED;
	}

	if ((interrupt_status & INT_HDCP_ENGAGED) != 0) {
		*param = 1;
		HDMI_INFO_MSG("HDCP_ENGAGED\n");

		_DisableEncryption(dev, false);
		return HDCP_ENGAGED;
	}

#if 1
	if ((interrupt_status & A_APIINTSTAT_KEEPOUTERRORINT_MASK) != 0) {
		HDMI_INFO_MSG("keep out error interrupt\n");
		return HDCP_IDLE;
	}

	if ((interrupt_status & A_APIINTSTAT_LOSTARBITRATION_MASK) != 0) {
		HDMI_INFO_MSG("lost arbitration error interrupt\n");
		return HDCP_IDLE;
	}

	if ((interrupt_status & A_APIINTSTAT_I2CNACK_MASK) != 0) {
		HDMI_INFO_MSG("i2c nack error interrupt\n");
		return HDCP_IDLE;
	}

	if (interrupt_status & INT_KSV_ACCESS) {
		HDMI_INFO_MSG("INT_KSV_ACCESS\n");
		return HDCP_IDLE;
	}

	if (interrupt_status & INT_KSV_SHA1_DONE) {
		HDMI_INFO_MSG("INT_KSV_SHA1_DONE\n");
		if (!_ksv_sha1_status(dev))
			hdcp_sw_reset(dev);

		return HDCP_IDLE;
	}
#endif

	return valid;
}

void hdcp_disable_encryption(hdmi_tx_dev_t *dev, int disable)
{
	LOG_TRACE1(disable);
	_DisableEncryption(dev,	(disable == TRUE) ? 1 : 0);
}

u8 hdcp_interrupt_status(hdmi_tx_dev_t *dev)
{
	return _InterruptStatus(dev);
}

int hdcp_interrupt_clear(hdmi_tx_dev_t *dev, u8 value)
{
	_InterruptClear(dev, value);
	return TRUE;
}
#endif

void hdcp_av_mute(hdmi_tx_dev_t *dev, int enable)
{
	LOG_TRACE1(enable);
	_EnableAvmute(dev,
			(enable == TRUE) ? 1 : 0);
}

u8 hdcp_av_mute_state(hdmi_tx_dev_t *dev)
{
	LOG_TRACE();
	return dev_read_mask(dev, A_HDCPCFG0, A_HDCPCFG0_AVMUTE_MASK);

}
