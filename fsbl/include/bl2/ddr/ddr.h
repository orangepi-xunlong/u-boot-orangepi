#ifndef __DDR_H__
#define __DDR_H__

extern uint32_t usys0_cap_in_mbyte, usys1_cap_in_mbyte;

struct ddr_param {
	uint8_t data[1024 * 16];
};

int ddr_init(const struct ddr_param *ddr_param);


#endif /* __DDR_H__ */
