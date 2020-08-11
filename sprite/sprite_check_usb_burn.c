 #include <common.h>
 #include <sys_config_old.h>
 #include <sunxi_board.h>
 #include "../usb_sunxi/usb_module.h"
 //#include "../usb_sunxi/"
 
 #ifdef CONFIG_AUTO_BURN
 #define  SUNXI_USB_AUTO_BURN           2 
 #define SUNXI_HANDSHAKE_BUSY           1
 #define SUNXI_HANDSHAKE_SUCCESS        2
 #endif
 
 extern int sunxi_usb_dev_register(uint dev_name);
 extern  int sunxi_usb_init(int delaytime);
 extern  int sunxi_usb_exit(void);
 extern  int sunxi_usb_burn_loop(void);
 extern sunxi_usb_setup_req_t     *sunxi_udev_active;
 
 
 volatile int sunxi_usb_auto_burn_from_boot_handshake=0;
 volatile int sunxi_usb_auto_burn_from_boot_init;
 volatile int sunxi_usb_auto_probe_update_action;
 static  int sunxi_usb_status = 1;
 
 
 void set_usb_auto_burn_boot_init_flag(int flag )
 {
    sunxi_usb_auto_burn_from_boot_init = flag;
 }
 
 
 int sunxi_handshake_main(void  *buffer)
 {
 
    sunxi_ubuf_t *sunxi_ubuf = (sunxi_ubuf_t *)buffer;
    
    switch(sunxi_usb_status)
    {
        case SUNXI_HANDSHAKE_BUSY:
            if(sunxi_ubuf->rx_ready_for_data == SUNXI_HANDSHAKE_BUSY)
            {
                sunxi_usb_status = SUNXI_HANDSHAKE_SUCCESS;
            }
            break;
 
        case SUNXI_HANDSHAKE_SUCCESS:
            puts("Handshake successful enter EFL mode \n");
            sunxi_usb_auto_burn_from_boot_handshake = 1;
            sunxi_usb_auto_probe_update_action = 1;
            break;
 
    }
    return 0;
 }
 
 
 
 int try_handshake_usb(void)
 {
    ulong begin_time= 0, over_time = 0;
    
    sunxi_usb_module_reg(SUNXI_USB_AUTO_BURN);
 
    if(sunxi_usb_init(0))
    {
        printf("%s usb init fail\n", __func__);
        sunxi_usb_exit();
        return 0;
    }
 
    puts("usb prepare ok\n");
    begin_time = get_timer(0);
    over_time = 800; /* 800ms */
    while(1)
    {
        if(sunxi_usb_auto_burn_from_boot_init)
        {
            printf("usb sof ok\n");
            break;
        }
        if(get_timer(begin_time) > over_time)
        {
            puts("overtime\n");
            sunxi_usb_exit();
            printf("%s usb : no usb exist\n", __func__);
 
            return 0;
        }
    }
    puts("usb probe ok\n");
    puts("usb setup ok\n");
 
    begin_time = get_timer(0);
    over_time = 1000; /* 1000ms */
    puts("*********Try to shake hands with PhoenixSuit********\n");
    while(1)
    {
         sunxi_usb_burn_loop();
 
        if(sunxi_usb_auto_burn_from_boot_handshake) //当握手成功，停止检查定时器
        {
             if(sunxi_usb_auto_probe_update_action == 1)
             {
                     puts("**********ready to run efex************ \n");
                     sunxi_board_run_fel();
             }
        }
        if(!sunxi_usb_auto_burn_from_boot_handshake)
        {
            if(get_timer(begin_time) > over_time)
            {
                sunxi_usb_exit();
                printf("%s usb : have no handshake\n", __func__);
                return 0;
            }
        }
    }
    sunxi_usb_exit();
 
    return 0;
 }
 
 
 int sunxi_auto_force_burn_by_usb(void)
 {
    __maybe_unused int  ret = 0;
    int workmode = uboot_spare_head.boot_data.work_mode;
    int if_need_auto_force_burn = 0;
    ret = script_parser_fetch("target", "auto_force_burn", &if_need_auto_force_burn, 1);
    if (workmode == WORK_MODE_BOOT && if_need_auto_force_burn == 1)
    {
        try_handshake_usb();
        puts("****************Handshake Failed***************\n");
    }
    return 0;
 }