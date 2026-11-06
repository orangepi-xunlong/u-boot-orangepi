/**********************************************************************
 * wrapper.h
 *
 * HPNFC wrapper
 ***********************************************************************/

#ifndef WRAPPER_H
#define WRAPPER_H

/* functions put controller to reset
 * next assign boot_en signal to 1 and releases reset
 * wait until boot is complete (boot_comp is 1)*/
#define ResetControllerBootEn()// do{} while(0)


#endif
