
#ifndef _CORE_CEC_H_
#define _CORE_CEC_H_

int cec_thread_init(void *param);
void cec_thread_exit(void);
s32 hdmi_cec_enable(int enable);
void hdmi_cec_wakup_request(void);

#endif
