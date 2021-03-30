/*
 * Copyright (C)  All rights reserved.
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Author: z.q <zengqi@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */

/*
 * This file contains clk functions, adapt for clk operation in boot.
 */

#include<clk/clk.h>
#include <fdt_support.h>

__attribute__((section(".data")))
static HLIST_HEAD(clk_list);

struct clk *__clk_lookup(const char *name)
{
	struct clk *root_clk;
	struct hlist_node *tmp;
	int test_count = 0;
	if (!name)
		return NULL;

	/* search the 'proper' clk tree first */
	hlist_for_each_entry(root_clk, tmp, &clk_list, child_node) {
		test_count++;
		if (!strcmp(root_clk->name, name))
			return root_clk;
	}

	return NULL;
}

static void __clk_disable(struct clk *clk)
{
	if (!clk)
		return;

	if (clk->enable_count == 0)
		return;

	if (--clk->enable_count > 0)
		return;

	if (clk->ops->disable)
		clk->ops->disable(clk->hw);

	__clk_disable(clk->parent);
}

unsigned long __clk_get_rate(struct clk *clk)
{
	unsigned long ret;

	if (!clk) {
		ret = 0;
		goto out;
	}
	ret = clk->rate;

out:
	return ret;
}

static int __clk_enable(struct clk *clk)
{
	int ret = 0;

	if (!clk){

		printf("%s: clk is null.\n",__func__);
		return -1;
	}
	if (clk->flags & CLK_IS_ROOT) {
		//printf("%s: clk is root.\n",__func__);
		return 0;
	}

	if (clk->enable_count == 0) {
		ret = __clk_enable(clk->parent);
		if (ret)
			return ret;

		if (clk->ops->enable) {
			ret = clk->ops->enable(clk->hw);
			if (ret) {
				__clk_disable(clk->parent);
				return ret;
			}
		}
	}

	clk->enable_count++;
	return 0;
}

static int __clk_set_parent(struct clk *clk, struct clk *parent)
{
	struct clk *old_parent;
	int ret = -1;
	u8 i;

	old_parent = clk->parent;

	if (!clk->parents) {
		clk->parents = malloc(sizeof(struct clk*) * clk->num_parents);
		if(clk->parents)
			memset(clk->parents, 0, sizeof(struct clk*) * clk->num_parents);
	}

	/*
	 * find index of new parent clock using cached parent ptrs,
	 * or if not yet cached, use string name comparison and cache
	 * them now to avoid future calls to __clk_lookup.
	 */
	for (i = 0; i < clk->num_parents; i++) {
		if (clk->parents && clk->parents[i] == parent) {
			break;
		}
		else if (!strcmp(clk->parent_names[i], parent->name)) {
			if (clk->parents)
				clk->parents[i] = __clk_lookup(parent->name);
			break;
		}
	}

	if (i == clk->num_parents) {
		printf("%s: clock %s is not a possible parent of clock %s\n",
				__func__, parent->name, clk->name);
		goto out;
	}

	/* FIXME replace with clk_is_enabled(clk) someday */
	if (clk->enable_count)
		__clk_enable(parent);

	/* change clock input source */
	ret = clk->ops->set_parent(clk->hw, i);
	clk->parent = clk->parents[i];
	/* clean up old prepare and enable */
	if (clk->enable_count)
		__clk_disable(old_parent);

out:
	return ret;
}

static void __clk_recalc_rates(struct clk *clk)
{
	unsigned long parent_rate = 0;

	if (clk->parent)
		parent_rate = clk->parent->rate;

	if (clk->ops->recalc_rate)
		clk->rate = clk->ops->recalc_rate(clk->hw, parent_rate);
	else
		clk->rate = parent_rate;
}

static struct clk *__clk_init_parent(struct clk *clk)
{
	struct clk *ret = NULL;
	u8 index;

	/* handle the trivial cases */
	if (!clk->num_parents)
		goto out;
	
	if (clk->num_parents == 1) {
		if (!clk->parent)
			ret = clk->parent = __clk_lookup(clk->parent_names[0]);
		ret = clk->parent;
		goto out;
	}

	if (!clk->ops->get_parent) {
		printf("%s: multi-parent clocks must implement .get_parent\n",
			__func__);
		goto out;
	};

	/*
	 * Do our best to cache parent clocks in clk->parents.  This prevents
	 * unnecessary and expensive calls to __clk_lookup.  We don't set
	 * clk->parent here; that is done by the calling function
	 */

	index = clk->ops->get_parent(clk->hw);

	if (!clk->parents) {
		clk->parents =
			malloc(sizeof(struct clk*) * clk->num_parents);
		if(clk->parents)
			memset(clk->parents, 0, sizeof(struct clk*) * clk->num_parents);
	}
	if (!clk->parents)
		ret = __clk_lookup(clk->parent_names[index]);
	else if (!clk->parents[index])
		ret = clk->parents[index] =
			__clk_lookup(clk->parent_names[index]);
	else
		ret = clk->parents[index];

out:
	return ret;
}

int __clk_init(struct clk *clk)
{
	int i, ret = 0;
	/* struct clk *orphan; */
	/* struct hlist_node *tmp, *tmp2; */

	if (!clk) {
		printf("%s: this is a null clk!\n",__func__);
		return -1;
	}
	/* check to see if a clock with this name is already registered */
	if (__clk_lookup(clk->name)) {
		printf("%s: clk %s already initialized\n",
				__func__, clk->name);
		ret = -1;
		goto out;
	}
	/* check that clk_ops are sane.  See Documentation/clk.txt */
	if (clk->ops->set_rate &&
			!(clk->ops->round_rate && clk->ops->recalc_rate)) {
		printf("%s: %s must implement .round_rate & .recalc_rate\n",
				__func__, clk->name);
		ret = -1;
		goto out;
	}

	if (clk->ops->set_parent && !clk->ops->get_parent) {
		printf("%s: %s must implement .get_parent & .set_parent\n",
				__func__, clk->name);
		ret = -1;
		goto out;
	}

	/* throw a WARN if any entries in parent_names are NULL */
	for (i = 0; i < clk->num_parents; i++)
		if(!clk->parent_names[i])
		printf("%s: invalid NULL in %s's .parent_names\n",
				__func__, clk->name);
	/*
	 * Allocate an array of struct clk *'s to avoid unnecessary string
	 * look-ups of clk's possible parents.  This can fail for clocks passed
	 * in to clk_init during early boot; thus any access to clk->parents[]
	 * must always check for a NULL pointer and try to populate it if
	 * necessary.
	 *
	 * If clk->parents is not NULL we skip this entire block.  This allows
	 * for clock drivers to statically initialize clk->parents.
	 */

	if (clk->num_parents > 1 && !clk->parents) {
		clk->parents = malloc((sizeof(struct clk*) * clk->num_parents));
		if(clk->parents)
			memset(clk->parents, 0, sizeof(struct clk*) * clk->num_parents);
		/*
		 * __clk_lookup returns NULL for parents that have not been
		 * clk_init'd; thus any access to clk->parents[] must check
		 * for a NULL pointer.  We can always perform lazy lookups for
		 * missing parents later on.
		 */
		if (clk->parents)
			for (i = 0; i < clk->num_parents; i++)
				clk->parents[i] =
					__clk_lookup(clk->parent_names[i]);
	}
	clk->parent = __clk_init_parent(clk);

	/*
	 * optional platform-specific magic
	 *
	 * The .init callback is not used by any of the basic clock types, but
	 * exists for weird hardware that must perform initialization magic.
	 * Please consider other ways of solving initialization problems before
	 * using this callback, as it's use is discouraged.
	 */


	hlist_add_head(&clk->child_node,
				&clk_list);

	if (clk->ops->recalc_rate)
		clk->rate = clk->ops->recalc_rate(clk->hw,
				__clk_get_rate(clk->parent));
	else if (clk->parent)
		clk->rate = clk->parent->rate;
	else
		clk->rate = 0;
	
	if (clk->ops->init)
		clk->ops->init(clk->hw);
out:

	return ret;
}


int clk_disable(struct clk *clk)
{
	if (!clk)
		return -1;

	 __clk_disable(clk);

	 return 0;
}


int clk_prepare_enable(struct clk *clk)
{
	int ret;
	if (!clk)
		return -1;

	ret = __clk_enable(clk);

	return ret;
}

/**
 * clk_round_rate - round the given rate for a clk
 * @clk: the clk for which we are rounding a rate
 * @rate: the rate which is to be rounded
 *
 * Takes in a rate as input and rounds it to a rate that the clk can actually
 * use which is then returned.  If clk doesn't support round_rate operation
 * then the parent rate is returned.
 */
long clk_round_rate(struct clk *clk, unsigned long rate)
{
	unsigned long parent_rate = 0;

	if (!clk)
		return 0;

	if (!clk->ops->round_rate) {
		return clk->rate;
	}

	if (clk->parent)
		parent_rate = clk->parent->rate;

	return clk->ops->round_rate(clk->hw, rate, &parent_rate);

}

struct clk *clk_get_parent(struct clk *clk)
{
	return !clk ? NULL : clk->parent;
}

int clk_set_parent(struct clk *clk, struct clk *parent)
{
	int ret = 0;

	if (!clk || !clk->ops)
		return -1;

	if (!clk->ops->set_parent)
		return -1;

	/* prevent racing with updates to the clock topology */

	if (clk->parent == parent)
		goto out;

	/* only re-parent if the clock is not in use */
	if ((clk->flags & CLK_SET_PARENT_GATE) && clk->prepare_count) {

		printf("%s: clk->enable_count = %d\n",__func__, clk->enable_count);
		ret = -1;
	}
	else
		ret = __clk_set_parent(clk, parent);

	/* propagate ABORT_RATE_CHANGE if .set_parent failed */
	if (ret) {
		__clk_recalc_rates(clk);
		goto out;
	}

out:

	return ret;
}

unsigned long clk_get_rate(struct clk *clk)
{
	unsigned long rate;

	rate = __clk_get_rate(clk);

	return rate;
}

int clk_set_rate(struct clk *clk, unsigned long rate)
{
	unsigned long best_parent_rate = 0;
	int ret = 0;
	unsigned long new_rate;

	/* bail early if nothing to do */
	if (rate == clk->rate && !(clk->flags & CLK_GET_RATE_NOCACHE)) {
		goto out;
	}

	/* calculate new rates and get the topmost changed clock */
	if (clk->parent)
		best_parent_rate = clk->parent->rate;

	if (!clk->parent) {
		printf("%s: %s has NULL parent\n", __func__, clk->name);
		ret = -1;

		goto out;
	}

	if (!clk->ops->round_rate)
		new_rate = clk->parent->new_rate;
	else {
		new_rate = clk->ops->round_rate(clk->hw, rate, &best_parent_rate);
	}
	clk->new_rate = new_rate;

	/* change the rates */
	if (clk->ops->set_rate)
		clk->ops->set_rate(clk->hw, clk->new_rate, best_parent_rate);

	if (clk->ops->recalc_rate)
		clk->rate = clk->ops->recalc_rate(clk->hw, best_parent_rate);
	else
		clk->rate = best_parent_rate;

	return 0;
out:

	return ret;
}



struct clk *clk_get(void *dev, const char *con_id)
{
	struct clk *clk;
	clk = __clk_lookup(con_id);

	return clk ? clk : NULL;
}

void clk_put(struct clk *clk)
{
}

void  clk_init(void)
{
	init_clocks();
}

struct clk *clk_register(struct clk_hw *hw)
{
	int ret;
	struct clk *clk;

	clk = malloc(sizeof(*clk));
	if(clk)
		memset(clk, 0, sizeof(*clk));
	if (!clk) {
		printf("%s: could not allocate clk\n", __func__);
		ret = -1;
		goto fail_out;
	}
	clk->name = hw->init->name;
	clk->ops = hw->init->ops;
	clk->hw = hw;
	clk->flags = hw->init->flags;
	clk->parent_names = hw->init->parent_names;
	clk->num_parents = hw->init->num_parents;
	hw->clk = clk;

	ret = __clk_init(clk);
	if (!ret)
        return clk;

	free(clk);
fail_out:
	return NULL;
}

int of_pll_clk_config_setup(struct clk *child_clk, u32 parent_handle)
{
	int node_offset = 0;
	int ret = 0;
	u32 rate = 0;
	char *clk_name = NULL;
	struct clk *clk;

	if (child_clk == NULL || parent_handle == 0)
		printf("%s: error:child_clk: 0x%x or parent_handle:0x%x is error\n",
		     __func__, (u32) child_clk, (u32) parent_handle);

	node_offset = fdt_node_offset_by_phandle(working_fdt, parent_handle);
	if (node_offset < 0) {
		printf("%s: error:get property by handle error\n", __func__);
		return -1;
	}

	ret =
	    fdt_getprop_string(working_fdt, node_offset, "clock-output-names",
			       &clk_name);
	if (ret < 0) {
		printf("%s: clock-output-names is null\n", __func__);
		return -1;
	}

	clk = clk_get(NULL, clk_name);
	if (!clk) {
		printf("%s: get clk is null\n", __func__);
		return -1;
	}

	ret = clk_set_parent(child_clk, clk);
	if (ret) {
		printf("%s: error:set parent is error. ret = %d\n", __func__,
		       ret);
		return -1;
	}

	ret =
	    fdt_getprop_u32(working_fdt, node_offset, "assigned-clock-parents",
			    &parent_handle);
	if (ret >= 0) {
		ret = of_pll_clk_config_setup(clk, parent_handle);
		if (ret) {
			printf("%s: of_pll_clk_config_setup error.\n",
			       __func__);
			return -1;
		}
	}

	ret =
	    fdt_getprop_u32(working_fdt, node_offset, "assigned-clock-rates",
			    &rate);
	if (ret >= 0) {
		ret = clk_set_rate(clk, rate);
		if (ret) {
			printf("%s: clk_set_rate is error. ret = %d\n",
			       __func__, ret);
			return -1;
		}
	}

	return 0;
}

int of_periph_clk_config_setup(int node_offset)
{
	int handle_num = 0;
	u32 handle[10] = { 0 };
	u32 parent_handle = 0;
	int i = 0;
	int ret = 0;
	char *clk_name = NULL;
	struct clk **clk_list = NULL;

	if (node_offset < 0) {
		printf("error:fdt err returned %s\n",
		       fdt_strerror(node_offset));
		return -1;
	}

	handle_num =
	    fdt_getprop_u32(working_fdt, node_offset, "clocks", handle);
	if (handle_num < 0) {
		printf("%s:%d:error:get property handle %s error:%s\n",
		       __func__, __LINE__, "clocks", fdt_strerror(handle_num));
		return -1;
	}

	clk_list = malloc(handle_num * sizeof(struct clk *));
	if (!clk_list) {
		printf("%s: clk_list malloc error!\n", __func__);
		return -1;
	}
	memset(clk_list, 0, handle_num * sizeof(struct clk *));

	for (i = 0; i < handle_num; i++) {
		u32 rate = 0;

		node_offset =
		    fdt_node_offset_by_phandle(working_fdt, handle[i]);
		if (node_offset < 0) {
			printf("%s:%d: error:get property by handle error\n",
			       __func__, __LINE__);
			goto err;
		}

		ret =
		    fdt_getprop_string(working_fdt, node_offset,
				       "clock-output-names", &clk_name);
		if (ret < 0) {
			printf("%s: clock-output-names is null\n", __func__);
			goto err;
		}

		clk_list[i] = clk_get(NULL, clk_name);
		if (strcmp(clk_list[i]->name, clk_name))
			printf("%s: clk_list[%d]->name = %s\n", __func__, i,
			       clk_list[i]->name);

		ret =
		    fdt_getprop_u32(working_fdt, node_offset,
				    "assigned-clock-parents", &parent_handle);
		if (ret >= 0) {
			ret =
			    of_pll_clk_config_setup(clk_list[i], parent_handle);
			if (ret) {
				printf
				    ("%s: %d: of_pll_clk_config_setup is error.",
				     __func__, __LINE__);
				goto err;
			}
		}

		ret =
		    fdt_getprop_u32(working_fdt, node_offset,
				    "assigned-clock-rates", &rate);
		if (ret >= 0)
			clk_set_rate(clk_list[i], rate);
		clk_put(clk_list[i]);
	}

      err:
	free(clk_list);

	return 0;
}

struct clk *of_clk_get(int node_offset, int index)
{
	int handle_num = 0;
	u32 handle[10] = { 0 };
	int ret;
	char *clk_name = NULL;
	struct clk *clk;

	if (node_offset < 0) {
		printf("%s: error:fdt err returned %s\n", __func__,
		       fdt_strerror(node_offset));
		return NULL;
	}

	handle_num =
	    fdt_getprop_u32(working_fdt, node_offset, "clocks", handle);
	if (handle_num < 0) {
		printf("%s: error:get property handle %s error:%s\n", __func__,
		       "clocks", fdt_strerror(handle_num));
		return NULL;
	}

	if (index >= handle_num) {
		printf("%s: index is error,reset it.\n", __func__);
		return NULL;
	}

	node_offset = fdt_node_offset_by_phandle(working_fdt, handle[index]);
	if (node_offset < 0) {
		printf("%s: error:get property by handle error\n", __func__);
		return NULL;
	}

	ret =
	    fdt_getprop_string(working_fdt, node_offset, "clock-output-names",
			       &clk_name);
	if (ret < 0) {
		printf("%s: get clock name error.\n", __func__);
		return NULL;
	}
	clk = clk_get(NULL, clk_name);

	return clk;
}
