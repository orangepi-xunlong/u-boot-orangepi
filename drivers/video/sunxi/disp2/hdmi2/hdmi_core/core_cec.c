/*
 * Allwinner SoCs hdmi2.0 driver.
 *
 * Copyright (C) 2016 Allwinner.
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */
#include "hdmi_core.h"
#include "api/api.h"
#include "api/cec.h"
#if defined(__LINUX_PLAT__)

#include <linux/clk.h>
#include <linux/mutex.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/kthread.h>

#define WAKEUP_TV_MSG			(1 << 0)
#define STANDBY_TV_MSG			(1 << 1)

DEFINE_MUTEX(thread_lock);
static int cec_send_request;
static int cec_active;
static struct task_struct *cec_task;
static s32 hdmi_cec_get_simple_msg(unsigned char *msg, unsigned size);
static void hdmi_cec_image_view_on(void);

void hdmi_cec_wakup_request(void)
{
	mutex_lock(&thread_lock);
	cec_send_request |= WAKEUP_TV_MSG;
	mutex_unlock(&thread_lock);
}

int cec_thread_check_request(void)
{
	int request = 0;
	mutex_lock(&thread_lock);
	request = cec_send_request;
	cec_send_request = 0;
	mutex_unlock(&thread_lock);
	return request;
}

static void cec_msg_sent(struct device *dev, char *buf)
{
	char *envp[2];

	envp[0] = buf;
	envp[1] = NULL;
	kobject_uevent_env(&dev->kobj, KOBJ_CHANGE, envp);
}

static void dump_cec_msg(unsigned char *msg, int size)
{
	if (hdmi_printf)
		print_hex_dump(KERN_DEBUG, "CEC MSG: ",
			DUMP_PREFIX_OFFSET, 16, 1, msg, size, 0);
}

#define CEC_BUF_SIZE 32
static int cec_thread(void *param)
{
	int ret = 0;
	int request;
	char buf[CEC_BUF_SIZE];
	unsigned char msg[16];
	struct device *pdev = (struct device *)param;

	while (1) {
		if (kthread_should_stop()) {
			break;
		}

		request = cec_thread_check_request();
		mutex_lock(&thread_lock);
		if (cec_active) {
			ret = hdmi_cec_get_simple_msg(msg, sizeof(msg));
			if (ret > 0) {
				memset(buf, 0, CEC_BUF_SIZE);
				snprintf(buf, sizeof(buf), "CEC_MSG=0x%x", msg[1]);
				cec_msg_sent(pdev, buf);
				dump_cec_msg(msg, ret);
			}
			if (request & WAKEUP_TV_MSG)
				hdmi_cec_image_view_on();
		}
		mutex_unlock(&thread_lock);
		msleep(10);
	}

	return 0;
}

extern void mc_cec_clock_enable(hdmi_tx_dev_t *dev, u8 bit);
int cec_thread_init(void *param)
{
	cec_task = kthread_create(cec_thread, (void *)param, "cec thread");
	if (IS_ERR(cec_task)) {
		printk("Unable to start kernel thread %s.\n\n", "cec thread");
		cec_task = NULL;
		return PTR_ERR(cec_task);
	}
	wake_up_process(cec_task);
	return 0;
}

void cec_thread_exit(void)
{
	if (cec_task) {
		kthread_stop(cec_task);
		cec_task = NULL;
	}
}

s32 hdmi_cec_enable(int enable)
{
	struct hdmi_tx_core *core = get_platform();

	LOG_TRACE();
	HDMI_INFO_MSG("cec enable %d\n", enable);

	mutex_lock(&thread_lock);
	if (enable) {
		/*
		 * Enable cec module and set logic address to playback device 1.
		 * TODO: ping other devices and allocate a idle logic address.
		 */
		core->hdmi_tx.snps_hdmi_ctrl.cec_on = true;
		mc_cec_clock_enable(&core->hdmi_tx, 0);
		udelay(200);
		cec_Init(&core->hdmi_tx);
		cec_CfgLogicAddr(&core->hdmi_tx, 4, 1);
		cec_active = 1;
	} else {
		cec_active = 0;
		cec_Disable(&core->hdmi_tx, 0);
		mc_cec_clock_enable(&core->hdmi_tx, 1);
		core->hdmi_tx.snps_hdmi_ctrl.cec_on = false;
	}
	mutex_unlock(&thread_lock);
	return 0;
}

s32 hdmi_cec_send(const char *buf, unsigned size, unsigned dst)
{
	unsigned src = 0x04;
	int retval;
	struct hdmi_tx_core *core = get_platform();
	if (!cec_active) {
		HDMI_ERROR_MSG("cec NOT enable now!\n");
		retval = -1;
		goto _cec_send_out;
	}
	retval = cec_ctrlSendFrame(&core->hdmi_tx, buf, size, src, dst);

_cec_send_out:
	return retval;
}

static s32 hdmi_cec_get_simple_msg(unsigned char *msg, unsigned size)
{
	struct hdmi_tx_core *core = get_platform();
	return cec_ctrlReceiveFrame(&core->hdmi_tx, msg, size);
}

static void hdmi_cec_image_view_on(void)
{
	const char image_view_on[2] = {0x04, 0};
	hdmi_cec_send(image_view_on, 1, 0x0);
}
#endif
