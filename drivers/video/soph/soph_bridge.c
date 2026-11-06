#include "soph_bridge.h"

void soph_bridge_init(struct soph_bridge *bridge,
			  struct soph_connector *conn,
			  struct display_state *state)
{
	if (!bridge)
		return;

	bridge->conn = conn;
	bridge->state = state;

	if (bridge->next_bridge)
		soph_bridge_init(bridge->next_bridge, conn, state);
}

void soph_bridge_pre_enable(struct soph_bridge *bridge)
{
	if (!bridge)
		return;

	if (bridge->funcs && bridge->funcs->pre_enable)
		bridge->funcs->pre_enable(bridge);

	if (bridge->next_bridge)
		soph_bridge_pre_enable(bridge->next_bridge);
}

void soph_bridge_post_disable(struct soph_bridge *bridge)
{
	if (!bridge)
		return;

	if (bridge->next_bridge)
		soph_bridge_post_disable(bridge->next_bridge);

	if (bridge->funcs && bridge->funcs->post_disable)
		bridge->funcs->post_disable(bridge);
}

void soph_bridge_enable(struct soph_bridge *bridge)
{
	if (!bridge)
		return;

	if (bridge->funcs && bridge->funcs->enable)
		bridge->funcs->enable(bridge);

	if (bridge->next_bridge)
		soph_bridge_enable(bridge->next_bridge);
}

void soph_bridge_disable(struct soph_bridge *bridge)
{
	if (!bridge)
		return;

	if (bridge->next_bridge)
		soph_bridge_disable(bridge->next_bridge);

	if (bridge->funcs && bridge->funcs->disable)
		bridge->funcs->disable(bridge);
}

void soph_bridge_mode_set(struct soph_bridge *bridge,
			      const struct drm_display_mode *mode)
{
	if (!bridge || !mode)
		return;

	if (bridge->funcs && bridge->funcs->mode_set)
		bridge->funcs->mode_set(bridge, mode);

	if (bridge->next_bridge)
		soph_bridge_mode_set(bridge->next_bridge, mode);
}

bool soph_bridge_detect(struct soph_bridge *bridge)
{
	if (bridge->funcs && bridge->funcs->detect)
		if (!bridge->funcs->detect(bridge))
			return false;

	if (bridge->next_bridge)
		return soph_bridge_detect(bridge->next_bridge);

	return true;
}
