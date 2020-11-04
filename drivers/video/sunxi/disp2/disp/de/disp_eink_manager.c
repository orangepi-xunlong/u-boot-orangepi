/*
 * Copyright (C) 2015 Allwinnertech, z.q <zengqi@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include "disp_eink.h"

#ifdef SUPPORT_EINK

/*#define __EINK_DEBUG__*/
/*#define __EINK_TEST__*/
#define MAX_EINK_ENGINE 1

#ifdef __EINK_DEBUG__
#define USED 1
#define LBL 4
#define LEL 44
#define LSL 10
#define FBL 4
#define FEL 12
#define FSL 4
#define WIDTH 800
#define HEIGHT 600
#endif

#ifdef EINK_FLUSH_TIME_TEST
struct timeval  wb_end_timer, flush_start_timer, open_tcon_timer, flush_end_timer, test_start, test_end,
en_lcd, dis_lcd, start_decode_timer, index_hard_timer;
struct timeval en_decode[3];
struct timeval fi_decode[3];
unsigned int t3_f3[3] = {0, 0, 0};
static int decode_task_t;
static int decode_t;

extern unsigned int lcd_t1, lcd_t2, lcd_t3, lcd_t4, lcd_t5, lcd_pin, lcd_po, lcd_tcon;
extern struct timeval ioctrl_start_timer;
unsigned int t1 = 0, t2 = 0, t3 = 0, t4 = 0, t3_1 = 0, t3_2 = 0, t3_3 = 0, t2_1 = 0, t2_2 = 0;
#endif

#ifdef __EINK_TEST__
static int decount;
static int wacount;
static int emcount;
static int qcount;
#endif

__attribute__((section(".data")))
static struct disp_eink_manager *eink_manager;
__attribute__((section(".data")))
static struct eink_private *eink_private_data;
static volatile int suspend;
static unsigned char index_finish_flag;
static volatile int diplay_pre_finish_flag;
volatile int diplay_finish_flag;

/*extern s32 tcon0_simple_close(u32 sel);*/
extern s32 tcon0_simple_open(u32 sel);

/*static int __write_edma_first(struct disp_eink_manager *manager);*/
/*static int __write_edma_second(struct disp_eink_manager *manager);*/
static int __write_edma(struct disp_eink_manager *manager);

struct disp_eink_manager *disp_get_eink_manager(unsigned int disp)
{
	return &eink_manager[disp];
}

unsigned int get_temperature(struct disp_eink_manager *manager)
{
	unsigned int temp = 28;

	if (manager) {
		temp = manager->get_temperature(manager);
	} else {
		printf("eink manager is null\n");
	}

	return temp;
}

struct eink_private *eink_get_priv(struct disp_eink_manager *manager)
{
	return (manager != NULL) ? (&eink_private_data[manager->disp]) : NULL;
}

int __eink_clk_init(struct disp_eink_manager *manager)
{
	return 0;
}

int __eink_clk_disable(struct disp_eink_manager *manager)
{
	int ret = 0;

	if (manager->private_data->eink_clk)
		clk_disable(manager->private_data->eink_clk);

	if (manager->private_data->edma_clk)
		clk_disable(manager->private_data->edma_clk);

	return ret;
}

int __eink_clk_enable(struct disp_eink_manager *manager)
{
	int ret = 0;

	if (manager->private_data->eink_clk)
		ret = clk_prepare_enable(manager->private_data->eink_clk);

	if (manager->private_data->edma_clk)
		ret = clk_prepare_enable(manager->private_data->edma_clk);

	return ret;
}

void __clear_wavedata_buffer(struct wavedata_queue *queue)
{
	int i = 0;
	unsigned long flags = 0;

	spin_lock_irqsave(&queue->slock, flags);

	queue->head = 0;
	queue->tail = 0;
	for (i = 0; i < WAVE_DATA_BUF_NUM; i++)
		queue->wavedata_used[i] = false;

	spin_unlock_irqrestore(&queue->slock, flags);
}

#ifdef __EINK_TEST__
void __eink_clear_wave_data(struct disp_eink_manager *manager, int overlap)
{
	__clear_wavedata_buffer(&(manager->private_data->wavedata_ring_buffer));
}

int __eink_debug_decode(struct disp_eink_manager *manager, int enable)
{
	int ret = 0;

	#ifdef EINK_FLUSH_TIME_TEST
	do_gettimeofday(&test_start);
	msleep(300);
	do_gettimeofday(&test_end);
	#endif

	return ret;
}

/*
static bool __is_wavedata_buffer_full(struct wavedata_queue *queue)
{
	bool ret;
	unsigned long flags = 0;
	spin_lock_irqsave(&queue->slock, flags);

	ret = ((queue->head + 1)%WAVE_DATA_BUF_NUM == queue->tail) ? true : false;

	spin_unlock_irqrestore(&queue->slock, flags);

	return ret;

}

static bool __is_wavedata_buffer_empty(struct wavedata_queue *queue)
{
	bool ret;
	unsigned long flags = 0;
	spin_lock_irqsave(&queue->slock, flags);

	ret = (queue->head == queue->tail) ? true : false;

	spin_unlock_irqrestore(&queue->slock, flags);

	return ret;

}
*/
#endif/*__EINK_TEST__*/

static void __eink_get_sys_config(u32 disp, struct eink_init_param *eink_param)
{

	int  value = 1;
	char primary_key[20];
	int  ret;

	sprintf(primary_key, "lcd%d", disp);

	ret = disp_sys_script_get_item(primary_key, "lcd_used", &value, 1);
    if (ret == 1)
		eink_param->used = value;

	if (eink_param->used == 0)
		return;

	ret = disp_sys_script_get_item(primary_key, "eink_bits", &value, 1);
	if (ret == 1)
		eink_param->eink_bits = value;

	ret = disp_sys_script_get_item(primary_key, "eink_mode", &value, 1);
	if (ret == 1)
		eink_param->eink_mode = value;

	ret = disp_sys_script_get_item(primary_key, "eink_lbl", &value, 1);
	if (ret == 1)
		eink_param->timing.lbl = value;

	ret = disp_sys_script_get_item(primary_key, "eink_lel", &value, 1);
	if (ret == 1)
		eink_param->timing.lel = value;

	ret = disp_sys_script_get_item(primary_key, "eink_lsl", &value, 1);
	if (ret == 1)
		eink_param->timing.lsl = value;

	ret = disp_sys_script_get_item(primary_key, "eink_fbl", &value, 1);
	if (ret == 1)
		eink_param->timing.fbl = value;

	ret = disp_sys_script_get_item(primary_key, "eink_fel", &value, 1);
	if (ret == 1)
		eink_param->timing.fel = value;

	ret = disp_sys_script_get_item(primary_key, "eink_fsl", &value, 1);
	if (ret == 1)
		eink_param->timing.fsl = value;

	ret = disp_sys_script_get_item(primary_key, "eink_width", &value, 1);
	if (ret == 1)
		eink_param->timing.width = value;

	ret = disp_sys_script_get_item(primary_key, "eink_height", &value, 1);
	if (ret == 1)
		eink_param->timing.height = value;

	strcpy(eink_param->wavefile_path, "wavefile\\default.awf");
}
void eink_decode_task(void);

static int __eink_interrupt_proc(int irq, void *parg)
{
/*	u32 reg_irq;
	u32 reg_ctrl;
	reg_irq = readl(0x01400004);
	reg_ctrl = readl(0x01400150);
	tick_printf("%x,%x\n", reg_irq, reg_ctrl);
*/
	struct disp_eink_manager *manager;
	int ret = -1;

	manager = (struct disp_eink_manager *)parg;
	if (!manager)
		return DISP_IRQ_RETURN;
	ret = disp_al_eink_irq_query(manager->disp);
	/* query irq, 0 is decode, 1 is calculate index. */
	if (ret == 1) {
		index_finish_flag = 1;
		goto out;
	} else if (ret == 0) {
		eink_decode_task();
/*		reg_irq = readl(0x01400004);
		reg_ctrl = readl(0x01400150);
		tick_printf("out %x,%x\n", reg_irq, reg_ctrl);
*/
		goto out;
	}

out:

	return DISP_IRQ_RETURN;
}

/* return a physic address for tcon
 * used to display wavedata,then
 * dequeue wavedata buffer.
*/
static void *__request_buffer_for_display(struct wavedata_queue *queue)
{
	void *ret;
	unsigned long flags = 0;
	bool is_wavedata_buf_empty;
	spin_lock_irqsave(&queue->slock, flags);

	is_wavedata_buf_empty = (queue->head == queue->tail) ? true : false;
	if (is_wavedata_buf_empty) {
		ret =  NULL;
		goto out;
	}

/*	reg_val_irq = readl(0x01400004);
	reg_ctrl = readl(0x01400150);
	tick_printf("%x,%x,%d\n", reg_val_irq, reg_ctrl, decode_cnt);
*/
	ret = (void *)queue->wavedata_paddr[queue->tail];

out:

	spin_unlock_irqrestore(&queue->slock, flags);

	return ret;
}

/* return a physic address for eink
*  engine used to decode one frame,
*  then queue wavedata buffer.
*/
static void *__request_buffer_for_decode(struct wavedata_queue *queue)
{
	void *ret;
	unsigned long flags = 0;
	bool is_wavedata_buf_full, is_used;

	spin_lock_irqsave(&queue->slock, flags);

	is_wavedata_buf_full = ((queue->head + 1)%WAVE_DATA_BUF_NUM == queue->tail) ? true : false;
	is_used = queue->wavedata_used[queue->head];
	if (is_wavedata_buf_full || is_used) {
		ret =  NULL;
		goto out;
	}

	ret = (void *)queue->wavedata_paddr[queue->head];

out:

	spin_unlock_irqrestore(&queue->slock, flags);

	return ret;
}

static s32 __queue_wavedata_buffer(struct wavedata_queue *queue)
{
	int ret = 0;
	unsigned long flags = 0;
	bool is_wavedata_buf_full;

	spin_lock_irqsave(&queue->slock, flags);

	is_wavedata_buf_full = ((queue->head + 1)%WAVE_DATA_BUF_NUM == queue->tail) ? true : false;

	if (is_wavedata_buf_full) {
		ret =  -1;
		goto out;
	}
	/* set used status true */
	queue->wavedata_used[queue->head] = true;

	queue->head = (queue->head + 1) % WAVE_DATA_BUF_NUM;

out:

#ifdef __EINK_TEST__
		qcount++;
#endif

	spin_unlock_irqrestore(&queue->slock, flags);

	return ret;
}

static s32 __dequeue_wavedata_buffer(struct wavedata_queue *queue)
{
	int ret = 0;
	unsigned long flags = 0;
	bool is_wavedata_buf_empty;

	spin_lock_irqsave(&queue->slock, flags);

	is_wavedata_buf_empty = (queue->head == queue->tail) ? true : false;

	if (is_wavedata_buf_empty) {
		ret =  -1;
		goto out;
	}
	queue->tail = (queue->tail + 1) % WAVE_DATA_BUF_NUM;

out:

#ifdef __EINK_TEST__
		decount++;
#endif

	spin_unlock_irqrestore(&queue->slock, flags);

	return ret;
}

static s32 __clean_used_wavedata_buffer(struct wavedata_queue *queue)
{
	int ret = 0;
	unsigned long flags = 0;

	spin_lock_irqsave(&queue->slock, flags);

	if (queue->tail >= 2) {
		queue->wavedata_used[queue->tail - 2] = false;
	} else {
		queue->wavedata_used[queue->tail + WAVE_DATA_BUF_NUM - 2] = false;
	}

	spin_unlock_irqrestore(&queue->slock, flags);

	return ret;
}

static int index_err;
static s32 eink_calculate_index_data(struct disp_eink_manager *manager,
					struct eink_8bpp_image *last_image,
					struct eink_8bpp_image *current_image)

{
	unsigned long flags = 0;
	unsigned long old_index_data_paddr = 0;
	unsigned long new_index_data_paddr = 0;
	unsigned int new_index = 0, old_index = 0;
	unsigned int t_new_index = 0, t_old_index = 0;
	int count = 0;

	if (!last_image || !current_image) {
		__wrn("image paddr is NULL!\n");
		return -EINVAL;
	}

	spin_lock_irqsave(&manager->private_data->slock, flags);

	t_new_index = new_index = manager->private_data->new_index;
	t_old_index = old_index = manager->private_data->old_index;

	if (new_index > 1 || old_index > 1 || new_index != old_index) {
		__wrn("index larger then 1,new_index=%d,old_index=%d\n",
			new_index, old_index);
		spin_unlock_irqrestore(&manager->private_data->slock, flags);
		return -EINVAL;
	}

	if (old_index == new_index)
		manager->private_data->new_index = 1 - manager->private_data->old_index;

	old_index = manager->private_data->old_index;
	new_index = manager->private_data->new_index;

	old_index_data_paddr = (unsigned long)manager->private_data->index_paddr[old_index];
	new_index_data_paddr = (unsigned long)manager->private_data->index_paddr[new_index];

	/*printf("new index=%d,old_index=%d, old_index_data_paddr=%p, new_index_data_paddr=%p\n",
		manager->private_data->new_index,
		manager->private_data->old_index,
		(void *)old_index_data_paddr,
		(void *)new_index_data_paddr);*/

	spin_unlock_irqrestore(&manager->private_data->slock, flags);

	disp_al_eink_start_calculate_index(manager->disp,
					old_index_data_paddr,
					new_index_data_paddr,
					last_image,
					current_image);

	/* check hardware status,if calculate over, then continue,
	*  otherwise wait for status,if timeout, throw warning and quit.
	*/

	while ((index_finish_flag != 1) && (count < 200)) {
		count++;

		/* it may fix by different param by hardware.
		 * if too less,the first frame index calc err.
		 * at this time, no use msleep.
		*/
		udelay(300);
	}
	if ((count >= 200) && (index_finish_flag != 1)) {
		__wrn("calculate index data is wrong!\n");

		spin_lock_irqsave(&manager->private_data->slock, flags);

		manager->private_data->new_index = t_new_index;
		manager->private_data->old_index = t_old_index;
		spin_unlock_irqrestore(&manager->private_data->slock, flags);
		eink_irq_query_index();
		index_finish_flag = 0;
		index_err = 1;
		return -1;
	}
	index_err = 0;
	index_finish_flag = 0;
	if (current_image->window_calc_enable)
		disp_al_get_update_area(manager->disp, &current_image->update_area);

#ifdef __EINK_TEST__
	printf("calc en=%d, flash mode = %d\n", current_image->window_calc_enable, current_image->flash_mode);
	printf("xtop=%d,ytop=%d,xbot=%d,ybot=%d\n",
		current_image->update_area.x_top, current_image->update_area.y_top,
		current_image->update_area.x_bottom, current_image->update_area.y_bottom);
#endif

	/* if calculate index sucess, then switch the
	 * index double buffer,set index fresh flag.
	*/
	spin_lock_irqsave(&manager->private_data->slock, flags);

	manager->private_data->index_fresh = true;

	spin_unlock_irqrestore(&manager->private_data->slock, flags);

	return 0;
}

static int start_decode(struct disp_eink_manager *manager, unsigned long wavedata_paddr, unsigned long index_paddr)
{
	struct eink_private *data;
	struct eink_init_param	param;
	int ret = 0;

	data = eink_get_priv(manager);
	memcpy((void *)&param, (void *)&data->param, sizeof(struct eink_init_param));

	ret = disp_al_eink_start_decode(manager->disp, index_paddr, wavedata_paddr, &param);

	return ret;
}

static int current_frame;
void __sync_task(unsigned long disp);
extern void disp_close_eink_panel(struct disp_eink_manager *manager);

int eink_display_one_frame(struct disp_eink_manager *manager)
{
	unsigned long flags = 0;
	int ret = 0;

	spin_lock_irqsave(&manager->private_data->slock, flags);

	manager->private_data->fresh_frame_index++;

	if (((manager->private_data->fresh_frame_index) - 1) == manager->private_data->total_frame) {

		manager->private_data->fresh_frame_index = 0;
		/* manager->private_data->total_frame = 0; */
		__clear_wavedata_buffer(&(manager->private_data->wavedata_ring_buffer));

#ifdef __EINK_TEST__
		/* test frames...*/

		unsigned int head = 0, tail = 0;
		unsigned long flags1 = 0;

		spin_lock_irqsave(&manager->private_data->wavedata_ring_buffer.slock, flags1);
		tail = manager->private_data->wavedata_ring_buffer.tail;
		head = manager->private_data->wavedata_ring_buffer.head;
		spin_unlock_irqrestore(&manager->private_data->wavedata_ring_buffer.slock, flags1);
		printf("<2>fin:tai=%d,hed=%d,idx=%d,dc=%d,qc=%d,ec=%d,tf=%d\n",
			tail, head, index, decount, qcount, emcount,
			manager->private_data->total_frame);
		decount = wacount = emcount = qcount = 0;
#endif
		current_frame = 0;

		/*__eink_clk_disable(manager);*/
		diplay_pre_finish_flag = 1;
		/*plcd = disp_device_find(manager->disp, DISP_OUTPUT_TYPE_LCD);*/
		/* how to close lcd ............*/
		/*schedule_work(&plcd->close_eink_panel_work);*/
		/*tcon0_simple_close(0);*/
		disp_close_eink_panel(manager);
	} else if (manager->private_data->fresh_frame_index <
			(manager->private_data->total_frame)) {

		__sync_task(manager->disp);
	}

	spin_unlock_irqrestore(&manager->private_data->slock, flags);

	return ret;
}

static int eink_decode_finish(struct disp_eink_manager *manager)
{
	unsigned long flags = 0;
	int ret = 0;

	spin_lock_irqsave(&manager->private_data->slock, flags);

	if (manager->private_data->total_frame ==
		manager->private_data->decode_frame_index) {
		/*printf("decode finish!,tot=%d, frame=%d, fresh_frame=%d.\n",
			manager->private_data->total_frame,
			manager->private_data->decode_frame_index,
			manager->private_data->fresh_frame_index);
		*/

		manager->private_data->decode_frame_index = 0;
		ret = 1;
		goto out;
	}

out:
	spin_unlock_irqrestore(&manager->private_data->slock, flags);
/*	printf("frame_index=%d, total_index=%d\n",
		 manager->private_data->decode_frame_index,
		 manager->private_data->total_frame);
*/
	return ret;
}

void __sync_task(unsigned long disp)
{
	struct disp_eink_manager *manager;
	int cur_line = 0;
	static int start_delay;

	start_delay = disp_al_lcd_get_start_delay(disp, NULL);
	manager = disp_get_eink_manager((unsigned int)disp);

	manager->tcon_flag = 0;
	cur_line = disp_al_lcd_get_cur_line(disp, NULL);
	__clean_used_wavedata_buffer(&manager->private_data->wavedata_ring_buffer);

	while (cur_line < start_delay && !diplay_pre_finish_flag)
		cur_line = disp_al_lcd_get_cur_line(disp, NULL);
	__write_edma(manager);
}

void eink_decode_task(void)
{
	struct disp_eink_manager *manager;
	unsigned int temperature;
	int insert = 0;
	unsigned long flags = 0;
	unsigned long index_buf_paddr = 0, wavedata_paddr = 0;
	unsigned int tframes = 0;
	unsigned int new_index;
	int frame = 0;
	int count = 0;
	static int first = 1;

	manager = disp_get_eink_manager(0U);
	temperature = get_temperature(manager);

	__queue_wavedata_buffer(&manager->private_data->wavedata_ring_buffer);

	insert = manager->pipeline_mgr->update_pipeline_list(manager->pipeline_mgr, temperature, &tframes);

	spin_lock_irqsave(&manager->private_data->slock, flags);
	frame = manager->private_data->decode_frame_index++;

	spin_unlock_irqrestore(&manager->private_data->slock, flags);

	if (frame == 2) {
		struct disp_device *plcd = NULL;
		plcd = disp_device_find(manager->disp, DISP_OUTPUT_TYPE_LCD);

		if (first) {
			diplay_pre_finish_flag = 0;
			plcd->enable(plcd);
			first = 0;
		} else {
			tcon0_simple_open(0);
		}
		disp_al_edma_config(manager->disp, wavedata_paddr, &manager->private_data->param);
		disp_al_edma_write(manager->disp, 1);
		__write_edma(manager);
	}

	if (eink_decode_finish(manager)) {
		spin_lock_irqsave(&manager->private_data->slock, flags);
		manager->private_data->index_fresh = false;
		spin_unlock_irqrestore(&manager->private_data->slock, flags);

			/* disable eink engine. */
			manager->disable(manager);

	} else {
		spin_lock_irqsave(&manager->private_data->slock, flags);

		/* no need inset pipeline */

		new_index = manager->private_data->new_index;

		index_buf_paddr = (unsigned long)manager->private_data->index_paddr[new_index];

		spin_unlock_irqrestore(&manager->private_data->slock, flags);

		wavedata_paddr = (unsigned long)__request_buffer_for_decode(&manager->private_data->wavedata_ring_buffer);

		while ((!wavedata_paddr) && count < 100) {
			/* msleep(1); */
			/* modify, no sleep, it needs wavedata ring buffer more larger */
			/*usleep_range(500, 2000);*/
			wavedata_paddr = (unsigned long)__request_buffer_for_decode(&manager->private_data->wavedata_ring_buffer);
			count++;
		}
		if (count > 100) {
			printf("wavedata_ring_buffer was full, stop decoding.\n");
			return;
		}
		__usdelay(1);
		start_decode(manager, wavedata_paddr, index_buf_paddr);

		spin_lock_irqsave(&manager->private_data->slock, flags);

		if (insert == 1)
			manager->private_data->index_fresh = false;

		spin_unlock_irqrestore(&manager->private_data->slock, flags);
	}
}

static struct eink_8bpp_image last_image;
static inline void *malloc_aligned(u32 size, u32 alignment)
{
	void *ptr = (void *)malloc(size + alignment);
	if (ptr) {
		void *aligned = (void *)(((long)ptr + alignment) & (~(alignment - 1)));

		/* Store the original pointer just before aligned pointer*/
		((void **) aligned)[-1] = ptr;
		return aligned;
	}

	return NULL;
}

static inline void free_aligned(void *aligned_ptr)
{
	if (aligned_ptr)
		free(((void **) aligned_ptr)[-1]);
}

s32 eink_update_image(struct disp_eink_manager *manager, struct eink_8bpp_image *cimage)
{
	int ret = 0;
	unsigned int tframes = 0;
	unsigned int temperature = 0;
	unsigned long wavedata_paddr, index_paddr;

	char *last_bmp = NULL;
	u32 height = cimage->size.height;
	u32 width = cimage->size.width;
	u32 last_buf_size = (height*width) << 2;
	last_bmp = (char *)malloc_aligned(last_buf_size, ARCH_DMA_MINALIGN);
	diplay_finish_flag = 0;
	/*char primary_key[25];
	s32 value = 0;
	u32 disp = 0;
	sprintf(primary_key, "lcd%d", disp);
	ret = disp_sys_script_get_item(primary_key, "eink_width", &value, 1);
	if (ret == 1)
	{
			width = value;
	}
	ret = disp_sys_script_get_item(primary_key, "eink_height", &value, 1);
	if (ret == 1)
	{
		height = value;
	}*/

	last_image.paddr = last_bmp;
	last_image.vaddr = last_bmp;
	last_image.size.width = width;
	last_image.size.height = height;

	if (index_err) {
		printf("%s: index err.\n", __func__);
		return -1;
	}

	manager->enable(manager);
	ret = eink_calculate_index_data(manager, &last_image, cimage);

	temperature = get_temperature(manager);

	manager->private_data->decode_frame_index = 0;
	index_paddr =  (unsigned long)manager->private_data->index_paddr[manager->private_data->new_index];

	manager->pipeline_mgr->config_and_enable_one_pipeline(manager->pipeline_mgr,
							cimage->update_area,
							cimage->update_mode,
							temperature,
							&tframes);

	manager->private_data->total_frame = tframes;
	wavedata_paddr = (unsigned long)__request_buffer_for_decode(&manager->private_data->wavedata_ring_buffer);
	while (!wavedata_paddr)
		wavedata_paddr = (unsigned long)__request_buffer_for_decode(&manager->private_data->wavedata_ring_buffer);

	start_decode(manager, wavedata_paddr, index_paddr);
	/*memcpy(&last_image, cimage, sizeof(struct eink_8bpp_image));*/
	 while (!diplay_finish_flag) {
		/* while finish displaying one image, then return */
		udelay(1000);
	 }

	 if (diplay_finish_flag) {
		manager->private_data->old_index = manager->private_data->new_index;
		if (last_bmp) {
			free_aligned(last_bmp);
			last_bmp = NULL;
		}
		return ret;
	}
	return -1;
}

extern int sunxi_bmp_display(char *name);


static int first_enable = 1;
s32 eink_enable(struct disp_eink_manager *manager)
{
	unsigned long flags = 0;
	int ret = 0;
	struct eink_init_param param;
	/*struct eink_init_param *eink_param;*/

	suspend = 0;
	spin_lock_irqsave(&manager->private_data->slock, flags);

	if (manager->private_data->enable_flag) {
		spin_unlock_irqrestore(&manager->private_data->slock, flags);
		return ret;
	}

	spin_unlock_irqrestore(&manager->private_data->slock, flags);

	/* enable eink clk */
	if (first_enable)
		ret = __eink_clk_enable(manager);

	/* init eink and edma*/
	memcpy((void *)&param, (void *)&manager->private_data->param, sizeof(struct eink_init_param));
	ret = disp_al_eink_config(manager->disp, &param);
	ret = disp_al_edma_init(manager->disp, &param);
	/*load waveform data,do only once.*/
	if (first_enable) {
		/* register eink irq */
		int i;
		ret = disp_al_init_waveform(param.wavefile_path);
		if (ret) {
			__wrn("malloc and save waveform memory fail!\n");
			disp_al_free_waveform();
		}

		for (i = 0; i < WAVE_DATA_BUF_NUM; i++) {
			ret = disp_al_init_eink_ctrl_data_8(manager->disp,
					((unsigned long)(manager->private_data->wavedata_ring_buffer.wavedata_vaddr[i])),
					&param.timing, 0);
		}

		disp_sys_register_irq(manager->private_data->irq_no, 0,
				__eink_interrupt_proc, (void *)manager, 0, 0);

		disp_sys_enable_irq(manager->private_data->irq_no);
		/*
		   manager->detect_fresh_task = kthread_create(
		   eink_detect_fresh_thread,
		   (void *)manager,
		   "eink fresh proc");


		   if (IS_ERR_OR_NULL(manager->detect_fresh_task)) {
		   __wrn("create eink detect fresh thread fail!\n");
		   ret = PTR_ERR(manager->detect_fresh_task);
		   return ret;
		   }
		   ret = wake_up_process(manager->detect_fresh_task);\
		   */
		first_enable = 0;
	}

	disp_al_eink_irq_enable(manager->disp);

	spin_lock_irqsave(&manager->private_data->slock, flags);

	manager->private_data->enable_flag = true;

	spin_unlock_irqrestore(&manager->private_data->slock, flags);

	return ret;
}

s32 eink_disable(struct disp_eink_manager *manager)
{
	unsigned long flags = 0;
	int ret = 0;

	spin_lock_irqsave(&manager->private_data->slock, flags);

	manager->private_data->enable_flag = false;

	spin_unlock_irqrestore(&manager->private_data->slock, flags);

	ret = disp_al_eink_irq_disable(manager->disp);

	/* disable de. */
/*	if (manager->convert_mgr)
		ret = manager->convert_mgr->disable(manager->convert_mgr->disp);
	else
		__wrn("convert mgr is null.\n");
*/
	/* disable eink engine. */
	ret = disp_al_eink_disable(manager->disp);

	/* disable clk move to when display finish. */
	/* ret = __eink_clk_disable(manager); */

	return ret;
}

int eink_set_temperature(struct disp_eink_manager *manager, unsigned int temp)
{
	s32 ret = -1;

	if (manager) {
		manager->eink_panel_temperature = temp;
		ret = 0;
	} else
		__wrn("eink manager is null\n");

	return ret;
}

unsigned int  eink_get_temperature(struct disp_eink_manager *manager)
{
	s32 temp = 28;

	if (manager) {
		temp = manager->eink_panel_temperature;
	} else
		__wrn("eink manager is null\n");

	return temp;
}

s32 eink_resume(struct disp_eink_manager *manager)
{

	int ret = 0;

	return ret;
}

s32 eink_suspend(struct disp_eink_manager *manager)
{
	int ret = 0;

	return ret;
}

static int __write_edma(struct disp_eink_manager *manager)
{
	int ret = 0;
	unsigned long wavedata_paddr = 0;

	current_frame++;
	wavedata_paddr = (unsigned long)__request_buffer_for_display(&manager->private_data->wavedata_ring_buffer);
	if (!wavedata_paddr) {
		return -1;
	}

	ret = disp_al_eink_edma_cfg_addr(manager->disp, wavedata_paddr);

	ret = disp_al_dbuf_rdy();
	ret = __dequeue_wavedata_buffer(&manager->private_data->wavedata_ring_buffer);

	return ret;
}

/* #define LARGE_MEM_TEST */
#ifdef LARGE_MEM_TEST
static void *malloc_wavedata_buffer(u32 mem_len, void *phy_address)
{
	u32 temp_size = PAGE_ALIGN(mem_len);
	struct page *page = NULL;
	void *tmp_virt_address = NULL;
	unsigned long tmp_phy_address = 0;

	page = alloc_pages(GFP_KERNEL, get_order(temp_size));
	if (page != NULL) {
		tmp_virt_address = page_address(page);
		if (tmp_virt_address == NULL)	{
			free_pages((unsigned long)(page), get_order(temp_size));
			__wrn("page_address fail!\n");
			return NULL;
		}

		tmp_phy_address = virt_to_phys(tmp_virt_address);
		*((unsigned long *)phy_address) = tmp_phy_address;

		__inf("pa=0x%p, va=0x%p, size=0x%x, len=0x%x\n",
			(void *)tmp_phy_address, tmp_virt_address,
			mem_len, temp_size);
	}

	return tmp_virt_address;
}

static void  free_wavedata_buffer(void *virt_addr, void *phys_addr, u32 num_bytes)
{
	unsigned int map_size = PAGE_ALIGN(num_bytes);

	free_pages((unsigned long)virt_addr, get_order(map_size));
	virt_addr = NULL;
	phys_addr = NULL;
}
#endif/*LARGE_MEM_TEST*/

int disp_init_eink(disp_bsp_init_para *para)
{
	int ret = 0;
	int i = 0;
	unsigned int  disp;
	unsigned long image_buf_size;
	unsigned int wavedata_buf_size, hsync, vsync;
	unsigned long indexdata_buf_size = 0;
	struct eink_init_param eink_param;
	struct disp_eink_manager *manager = NULL;
	struct eink_private *data = NULL;
	int wd_buf_id = 0;

	/*
	* request eink manager and its private data, then initial this two structure
	*/

	eink_manager = (struct disp_eink_manager *)malloc(sizeof(struct disp_eink_manager) * MAX_EINK_ENGINE);
	if (eink_manager == NULL) {
		__wrn("malloc eink manager memory fail!\n");
		ret =  -1;
		goto manager_malloc_err;
	}

	memset(eink_manager, 0, sizeof(struct disp_eink_manager) * MAX_EINK_ENGINE);

	eink_private_data = (struct eink_private *)malloc(sizeof(struct eink_private) * MAX_EINK_ENGINE);
	if (eink_private_data == NULL) {
		__wrn("malloc private memory fail!\n");
		ret = -1;
		goto private_data_malloc_err;
	}
	memset((void *)eink_private_data, 0, sizeof(struct eink_private) * MAX_EINK_ENGINE);

	for (disp = 0; disp < MAX_EINK_ENGINE; disp++) {
		/* get sysconfig and config eink_param */
		__eink_get_sys_config(disp, &eink_param);

		hsync = eink_param.timing.lbl + eink_param.timing.lel + eink_param.timing.lsl;
		vsync = eink_param.timing.fbl + eink_param.timing.fel + eink_param.timing.fsl;
		image_buf_size = eink_param.timing.width * eink_param.timing.height;
		if (eink_param.eink_mode)
			/* mode 1, 16 data */
			wavedata_buf_size = 4 * (eink_param.timing.width/8 + hsync) * (eink_param.timing.height + vsync);
		else
			/* mode 0, 8 data */
			wavedata_buf_size = 2 * (eink_param.timing.width/4 + hsync) * (eink_param.timing.height + vsync);
		/*fix it when 5bits*/
		if (eink_param.eink_bits < EINK_BIT_5) {
			/* 3bits or 4bits */
			indexdata_buf_size = image_buf_size;
		} else {
			/* 5bits */
			indexdata_buf_size = image_buf_size<<1;
		}

		manager = &eink_manager[disp];
		data = &eink_private_data[disp];

		manager->disp = disp;
		manager->private_data = data;
		manager->mgr = disp_get_layer_manager(disp);
		manager->eink_update = eink_update_image;
		manager->enable = eink_enable;
		manager->disable = eink_disable;
		manager->resume = eink_resume;
		manager->suspend = eink_suspend;
		manager->set_temperature = eink_set_temperature;
		manager->get_temperature = eink_get_temperature;
		manager->eink_panel_temperature = 28;
		/* functions for debug */

		data->enable_flag = false;

		data->eink_clk = para->mclk[DISP_MOD_EINK];
		data->edma_clk = para->mclk[DISP_MOD_EDMA];
		data->eink_base_addr = (unsigned long)para->reg_base[DISP_MOD_EINK];
		data->irq_no = para->irq_no[DISP_MOD_EINK];

		memcpy((void *)&data->param, (void *)&eink_param, sizeof(struct eink_init_param));

		printf("eink_clk: 0x%p; edma_clk: 0x%p; base_addr: 0x%p; irq_no: 0x%x\n",
					data->eink_clk, data->edma_clk, (void *)data->eink_base_addr, data->irq_no);

		disp_al_set_eink_base(disp, data->eink_base_addr);

		/* init index buffer, it includes old and new index buffer */
		data->index_fresh = false;
		data->new_index = 0;
		data->old_index = 0;
		data->index_vaddr[0] = (void *)malloc(indexdata_buf_size * INDEX_BUFFER_NUM);
		data->index_paddr[0] = data->index_vaddr[0];
		if (data->index_vaddr[i] == NULL) {
			__wrn("malloc old index data memory fail!\n");
			ret =  -1;
		}

		memset(data->index_vaddr[0], 0, image_buf_size * INDEX_BUFFER_NUM);
		for (i = 0; i < INDEX_BUFFER_NUM; i++) {
			data->index_paddr[i] = data->index_paddr[0] + indexdata_buf_size * i;
			data->index_vaddr[i] = data->index_vaddr[0] + indexdata_buf_size * i;
			printf("index_paddr%d:0x%p\n", i, data->index_paddr[i]);
		}

		memset(&data->wavedata_ring_buffer, 0, sizeof(struct wavedata_queue));

		data->wavedata_ring_buffer.head = 0;
		data->wavedata_ring_buffer.tail = 0;
		data->wavedata_ring_buffer.size.width = eink_param.timing.width;
		data->wavedata_ring_buffer.size.height = eink_param.timing.height;
		/* align param need match with drawer's pitch */
		data->wavedata_ring_buffer.size.align = 4;

		/*init wave data, if need lager memory,open the LARGE_MEM_TEST.*/
#ifdef LARGE_MEM_TEST
		for (wd_buf_id = 0; wd_buf_id < WAVE_DATA_BUF_NUM; wd_buf_id++) {
			data->wavedata_ring_buffer.wavedata_vaddr[wd_buf_id] =
				malloc_wavedata_buffer(wavedata_buf_size,
					&data->wavedata_ring_buffer.wavedata_paddr[wd_buf_id]);
			if (data->wavedata_ring_buffer.wavedata_vaddr[wd_buf_id] == NULL) {
				__wrn("malloc eink wavedata memory fail, size=%d, id=%d\n",
						wavedata_buf_size, wd_buf_id);
				ret =  -ENOMEM;
			}
			memset((void *)data->wavedata_ring_buffer.wavedata_vaddr[wd_buf_id], 0, wavedata_buf_size);

			printf("wavedata id=%d, virt-addr=0x%p, phy-addr=0x%p\n", \
					wd_buf_id, (data->wavedata_ring_buffer.wavedata_vaddr[wd_buf_id]),
					data->wavedata_ring_buffer.wavedata_paddr[wd_buf_id]);
		}
#else
		for (wd_buf_id = 0; wd_buf_id < WAVE_DATA_BUF_NUM; wd_buf_id++) {
			data->wavedata_ring_buffer.wavedata_vaddr[wd_buf_id] = (void *)malloc(wavedata_buf_size);
			data->wavedata_ring_buffer.wavedata_paddr[wd_buf_id] = data->wavedata_ring_buffer.wavedata_vaddr[wd_buf_id];

			if (data->wavedata_ring_buffer.wavedata_vaddr[wd_buf_id] == NULL) {
				__wrn("malloc eink wavedata memory fail, size=%d, id=%d\n", wavedata_buf_size, wd_buf_id);
				ret =  -1;
			}
			memset((void *)data->wavedata_ring_buffer.wavedata_vaddr[wd_buf_id], 0, wavedata_buf_size);

/*			printf("wavedata id=%d, virt-addr=%p, phy-addr=%p,wavedata_buf_size=%x\n",
			wd_buf_id,
			data->wavedata_ring_buffer.wavedata_vaddr[wd_buf_id],
			data->wavedata_ring_buffer.wavedata_paddr[wd_buf_id],
			wavedata_buf_size);
*/
/*			printf("wavedata id=%d, virt-addr value =%d, phy-addr value=%d\n",
			wd_buf_id,
			*((int *)data->wavedata_ring_buffer.wavedata_vaddr[wd_buf_id]),
			*((int *)data->wavedata_ring_buffer.wavedata_paddr[wd_buf_id]));
*/
		}
#endif
		pipeline_manager_init(manager);
/*		printf("fresh_frame_index=%p, total_frame=%p, decode_frame_index= %p, diplay_finish_flag=%p\n",
					&manager->private_data->fresh_frame_index,
					&manager->private_data->total_frame,
					&manager->private_data->decode_frame_index,
					&diplay_finish_flag);
*/

		return 0;
/*waveform_err:
	disp_al_free_waveform();
*/
private_data_malloc_err:
	free(eink_private_data);

manager_malloc_err:
	free(eink_manager);

	}
	return ret;
}
#endif
