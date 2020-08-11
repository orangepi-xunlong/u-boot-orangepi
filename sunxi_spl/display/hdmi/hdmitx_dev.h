

#ifndef _HDMITX_DEV_H_
#define _HDMITX_DEV_H_
#include <common.h>


#define HDMI_ERROR_MSG printf
#define	HDMI_INFO_MSG printf
#define LOG_TRACE()
#define LOG_TRACE1(a)
#define LOG_TRACE2(a, b)
#define LOG_TRACE3(a, b, c)
#define BIT(x)			(1 << (x))

#define FALSE false
#define TRUE true
typedef enum {
	PHY_ACCESS_UNDEFINED = 0,
	PHY_I2C = 1,
	PHY_JTAG
} phy_access_t;

struct hdmi_tx_ctrl {
	u8 data_enable_polarity;
	u32  pixel_clock;
	u8 pixel_repetition;
	u32  tmds_clk;
	u8 color_resolution;
	u8 csc_on;
	u8 audio_on;
	u8 esm_enable;
	u8 hdmi_on;
	u8 hdcp_on;
	u8 cec_on;
	phy_access_t phy_access;
};

typedef struct hdmi_tx_dev_api {
	struct hdmi_tx_ctrl	snps_hdmi_ctrl;
} hdmi_tx_dev_t;

/* *************************************************************************
 * Data Manipulation and Access
 * *************************************************************************/
/**
 * Find first (least significant) bit set
 * @param[in] data word to search
 * @return bit position or 32 if none is set
 */
static inline unsigned first_bit_set(uint32_t data)
{
	unsigned n = 0;

	if (data != 0) {
		for (n = 0; (data & 1) == 0; n++)
			data >>= 1;
	}
	return n;
}


/**
 * Set bit field
 * @param[in] data raw data
 * @param[in] mask bit field mask
 * @param[in] value new value
 * @return new raw data
 */
static inline uint32_t set(uint32_t data, uint32_t mask, uint32_t value)
{
	return ((value << first_bit_set(mask)) & mask) | (data & ~mask);
}


#endif
