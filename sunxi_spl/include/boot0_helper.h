
#ifndef _BOOT0_HELPER_H_
#define _BOOT0_HELPER_H_

extern __s32 boot_set_gpio(void  *user_gpio_list, __u32 group_count_max, __s32 set_gpio);
extern void mmu_setup(u32 dram_size);
extern void  mmu_turn_off( void );
extern int load_boot1(void);
extern void set_dram_para(void *dram_addr , __u32 dram_size, __u32 boot_cpu);
extern void boot0_jump(unsigned int addr);
extern void boot0_jmp_boot1(unsigned int addr);
extern void boot0_jmp_other(unsigned int addr);
extern void boot0_jmp_monitor(void);
extern void reset_pll( void );
extern int load_fip(int *use_monitor);
extern void set_debugmode_flag(void);
extern void set_pll( void );
extern void update_flash_para(void);


extern int printf(const char *fmt, ...);
extern void * memcpy(void * dest,const void *src,size_t count);
extern void * memset(void * s, int c, size_t count);
extern void * memcpy_align16(void * dest,const void *src,size_t count);
extern int strncmp(const char * cs,const char * ct,size_t count);

#endif
