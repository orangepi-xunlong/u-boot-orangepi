/*
 * Copyright (C) 2019 Allwinnertech, <liuli@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include "include/eink_driver.h"
#include "include/eink_sys_source.h"
#include "lowlevel/eink_reg.h"

static struct pipe_manager *g_pipe_mgr;

inline bool is_invalid_pipe_id(struct pipe_manager *mgr, int id)
{
	return ((id < 0) || (id >= mgr->max_pipe_cnt));
}

struct pipe_manager *get_pipeline_manager(void)
{
	return g_pipe_mgr;
}

static int request_pipe(struct pipe_manager *mgr)
{
	u32 pipe_id = -1;
	unsigned long flags = 0;
	struct pipe_info_node *pipe = NULL, *temp = NULL;

#ifdef PIPELINE_DEBUG
	EINK_INFO_MSG("Before Request Pipe\n");
/*	print_free_pipe_list(mgr); */
	print_used_pipe_list(mgr);
#endif
	spin_lock_irqsave(&mgr->list_lock, flags);

	if (list_empty(&mgr->pipe_free_list)) {
		pr_err("There is no free pipe!\n");
		spin_unlock_irqrestore(&mgr->list_lock, flags);
		return -1;
	}

	list_for_each_entry_safe(pipe, temp, &mgr->pipe_free_list, node) {
		pipe_id = pipe->pipe_id;

		list_move_tail(&pipe->node, &mgr->pipe_used_list);
		break;
	}

	spin_unlock_irqrestore(&mgr->list_lock, flags);

#ifdef PIPELINE_DEBUG
	EINK_INFO_MSG("After Request Pipe\n");
	/*print_free_pipe_list(mgr);*/
	print_used_pipe_list(mgr);
#endif
	EINK_INFO_MSG("request pipe_id = %d\n", pipe_id);
	return pipe_id;
}

static int config_pipe(struct pipe_manager *mgr, struct pipe_info_node info)
{
	int ret = 0;
	unsigned long flags = 0;
	struct pipe_info_node *pipe = NULL, *temp = NULL;
	u32 pipe_id = 0;

	pipe_id = info.pipe_id;

	if (pipe_id >= mgr->max_pipe_cnt) {
		pr_err("%s:pipe is invalied!\n", __func__);
		return -EINVAL;
	}

	spin_lock_irqsave(&mgr->list_lock, flags);
	list_for_each_entry_safe(pipe, temp, &mgr->pipe_used_list, node) {
		if (pipe->pipe_id != pipe_id) {
			continue;
		}
		memcpy(&pipe->upd_win, &info.upd_win, sizeof(struct upd_win));
		pipe->img = info.img;
		pipe->wav_paddr = info.wav_paddr;
		pipe->wav_vaddr = info.wav_vaddr;
		pipe->total_frames = info.total_frames;
		pipe->upd_mode = info.upd_mode;
		pipe->fresh_frame_cnt = 0;
		pipe->dec_frame_cnt = 0;

		if (info.img->upd_all_en == true)
			eink_set_upd_all_en(1);
		else
			eink_set_upd_all_en(0);
		eink_pipe_config(pipe);
		eink_pipe_config_wavefile(pipe->wav_paddr, pipe->pipe_id);

		EINK_INFO_MSG("config Pipe_id=%d, UPD:(%d,%d)~(%d,%d), wav addr = 0x%x, total frames=%d, upd_all_en = %d\n",
			      pipe->pipe_id,
				pipe->upd_win.left, pipe->upd_win.top,
				pipe->upd_win.right, pipe->upd_win.bottom,
				(unsigned int)pipe->wav_paddr, pipe->total_frames,
				info.img->upd_all_en);
		break;
	}

	spin_unlock_irqrestore(&mgr->list_lock, flags);

	return ret;
}

static int active_pipe(struct pipe_manager *mgr, u32 pipe_no)
{
	struct pipe_info_node *pipe = NULL, *tmp_pipe = NULL;
	unsigned long flags = 0;
	unsigned long tframes = 0;
	int ret = -1;

	if (pipe_no >= mgr->max_pipe_cnt) {
		pr_err("%s:pipe is invalied!\n", __func__);
		return -EINVAL;
	}
	mgr->current_pipe_finish = false;

	spin_lock_irqsave(&mgr->list_lock, flags);
	list_for_each_entry_safe(pipe, tmp_pipe, &mgr->pipe_used_list, node) {
		if (pipe->pipe_id != pipe_no)
			continue;
		if (pipe->active_flag == true) {
			pr_warn("%s:The pipe has already active!\n", __func__);
			spin_unlock_irqrestore(&mgr->list_lock, flags);
			return 0;
		} else {
			eink_pipe_enable(pipe->pipe_id);
			pipe->active_flag = true;
			tframes = pipe->total_frames;
			EINK_INFO_MSG("Enable an new pipe id = %d, pipe total frames=%lu\n", pipe->pipe_id, tframes);
			ret = 0;
			break;
		}
	}

	if (tframes == 0) {
		pr_err("%s:no pipe or total_frame of pipe %d is 0!\n", __func__, pipe_no);
		ret = -1;
		spin_unlock_irqrestore(&mgr->list_lock, flags);
		return ret;
	}

	spin_unlock_irqrestore(&mgr->list_lock, flags);
	return ret;
}

static void release_pipe(struct pipe_manager *mgr, struct pipe_info_node *pipe)
{
#if 0
#ifdef PIPELINE_DEBUG
	EINK_DEFAULT_MSG("Before Config Pipe\n");
	print_free_pipe_list(mgr);
	print_used_pipe_list(mgr);
#endif
#endif
	/* hardware disable this pipe self */
	/* eink_pipe_disable(pipe->pipe_id);*/

	/* initial and add to free list */
	pipe->active_flag = false;
	pipe->dec_frame_cnt = 0;
	pipe->fresh_frame_cnt = 0;
	pipe->total_frames = 0;

#if 0
#ifdef PIPELINE_DEBUG
	EINK_DEFAULT_MSG("After Config Pipe\n");
	print_free_pipe_list(mgr);
	print_used_pipe_list(mgr);
#endif
#endif
	EINK_INFO_MSG("release pipe id=%d\n", pipe->pipe_id);

	return;
}

u64 get_free_pipe_state(struct pipe_manager *mgr)
{
	struct pipe_info_node *pipe_node = NULL, *tmp_node = NULL;
	u64 state = 0;
	unsigned long flags = 0;

	if (!mgr) {
		pr_err("%s:pipe mgr is NULL!\n", __func__);
		return 0;
	}

	spin_lock_irqsave(&mgr->list_lock, flags);
	list_for_each_entry_safe(pipe_node, tmp_node, &mgr->pipe_free_list, node) {
		state = state | (1ULL << pipe_node->pipe_id);
	}
	spin_unlock_irqrestore(&mgr->list_lock, flags);

	return state;
}

int pipeline_mgr_enable(struct pipe_manager *mgr)
{
	int ret = 0;
	unsigned long flags = 0;

	spin_lock_irqsave(&mgr->list_lock, flags);
	if (mgr->enable_flag == true) {
		EINK_DEFAULT_MSG("pipe mgr already enable!\n");
		spin_unlock_irqrestore(&mgr->list_lock, flags);
		return 0;
	}

	eink_set_out_panel_mode(&mgr->panel_info);
	eink_set_data_reverse();
	eink_irq_enable();

	mgr->enable_flag = true;
	spin_unlock_irqrestore(&mgr->list_lock, flags);

	return ret;
}

static int pipeline_mgr_disable(void)
{
	struct pipe_manager *mgr = g_pipe_mgr;
	unsigned long flags = 0;

	if (!mgr) {
		pr_err("%s:mgr is null!\n", __func__);
		return -EINVAL;
	}
	spin_lock_irqsave(&mgr->list_lock, flags);
	if (mgr->enable_flag == false) {
		spin_unlock_irqrestore(&mgr->list_lock, flags);
		return 0;
	}
	/* ee start clean hw self*/

	mgr->enable_flag = false;
	spin_unlock_irqrestore(&mgr->list_lock, flags);

	return 0;
}

int pipe_mgr_init(struct eink_manager *eink_mgr)
{
	int ret = 0, i = 0;
	struct pipe_manager *pipe_mgr = NULL;
	struct eink_panel_info  *panel_info = NULL;
	struct pipe_info_node **pipe_node = NULL;

	pipe_mgr = (struct pipe_manager *)malloc(sizeof(struct pipe_manager));
	if (!pipe_mgr) {
		pr_err("pipe mgr kmalloc failed!\n");
		ret = -ENOMEM;
		goto pipe_mgr_err;
	}

	memset((void *)pipe_mgr, 0, sizeof(struct pipe_manager));
	pipe_mgr->current_pipe_finish = true;
	panel_info = &eink_mgr->panel_info;

	if (panel_info->bit_num == 4) {
		pipe_mgr->max_pipe_cnt = 64;
	} else if (panel_info->bit_num == 5) {
		pipe_mgr->max_pipe_cnt = 31;
	}

	memcpy(&pipe_mgr->panel_info, &eink_mgr->panel_info, sizeof(struct eink_panel_info));
	spin_lock_init(&pipe_mgr->list_lock);

	INIT_LIST_HEAD(&pipe_mgr->pipe_free_list);
	INIT_LIST_HEAD(&pipe_mgr->pipe_used_list);

	pipe_node =
		(struct pipe_info_node **)malloc(pipe_mgr->max_pipe_cnt * sizeof(struct pipe_info_node *));

	if (!pipe_node) {
		pr_err("pipe node ** malloc failed!\n");
		ret = -ENOMEM;
		goto pipe_node_err;
	}
	memset((void *)pipe_node, 0, pipe_mgr->max_pipe_cnt * sizeof(struct pipe_info_node *));

	for (i = 0; i < pipe_mgr->max_pipe_cnt; i++) {
		pipe_node[i] = (struct pipe_info_node *)malloc(sizeof(struct pipe_info_node));
		if (!pipe_node[i]) {
			pr_err("%s:malloc pipe failed!\n", __func__);
			ret = -ENOMEM;
			goto err_out;
		}
		memset((void *)pipe_node[i], 0, sizeof(struct pipe_info_node));
		pipe_node[i]->pipe_id = i;
		pipe_node[i]->active_flag = false;

		list_add_tail(&pipe_node[i]->node, &pipe_mgr->pipe_free_list);
	}

	pipe_mgr->pipe_mgr_enable = pipeline_mgr_enable;
	pipe_mgr->pipe_mgr_disable = pipeline_mgr_disable;
	pipe_mgr->get_free_pipe_state = get_free_pipe_state;
	pipe_mgr->request_pipe = request_pipe;
	pipe_mgr->config_pipe = config_pipe;
	pipe_mgr->active_pipe = active_pipe;
	pipe_mgr->release_pipe = release_pipe;

	eink_mgr->pipe_mgr = pipe_mgr;
	g_pipe_mgr = pipe_mgr;

	return 0;
err_out:
	for (i = 0; i < pipe_mgr->max_pipe_cnt; i++) {
		free(pipe_node[i]);
	}
pipe_node_err:
	free(pipe_node);
pipe_mgr_err:
	free(pipe_mgr);

	return ret;
}
