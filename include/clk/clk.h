#ifndef __CLK__H
#define __CLK__H
#include<clk/clk_plat.h>

void clk_put(struct clk *clk);
int clk_set_rate(struct clk *clk, unsigned long rate);
unsigned long clk_get_rate(struct clk *clk);
struct clk *clk_get(void *dev, const char *con_id);
struct clk *clk_register(struct clk_hw *hw);
int clk_set_parent(struct clk *clk, struct clk *parent);
struct clk *clk_get_parent(struct clk *clk);
void  clk_init(void);
int clk_prepare_enable(struct clk *clk);
int clk_disable(struct clk *clk);
int of_periph_clk_config_setup(int node_offset);
struct clk* of_clk_get(int node_offset, int index);
long clk_round_rate(struct clk *clk, unsigned long rate);

#endif
