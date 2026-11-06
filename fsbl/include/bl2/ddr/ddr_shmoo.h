#include <ddr_sys.h>

#ifdef DDR_SHMOO_LOG_RX
void cvx32_rdlvl_req_shmoo_real(int sys_id, int mode, int sso_period, int rank, int vref_start, int vref_end, int vref_step, int dcode_start, int dcode_end, int dcode_step);
#endif
#ifdef DDR_SHMOO_LOG_TX
void	cvx32_wdqlvl_req_shmoo_real(int sys_id, int mode, int sso_period, int rank, int vref_start, int vref_end, int vref_step,int  dline_start, int  dline_end, int dline_step, int en_dmdbi);
#endif
