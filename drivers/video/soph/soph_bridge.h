#ifndef __SOPH_BRIDGE_H__
#define __SOPH_BRIDGE_H__

#include <config.h>
#include <common.h>
#include <dm/device.h>
#include <errno.h>

struct display_state;
struct soph_bridge;
struct drm_display_mode;
struct soph_connector;

struct soph_bridge_funcs {
	void (*enable)(struct soph_bridge *bridge);
	void (*disable)(struct soph_bridge *bridge);
	void (*pre_enable)(struct soph_bridge *bridge);
	void (*post_disable)(struct soph_bridge *bridge);
	void (*mode_set)(struct soph_bridge *bridge,
			 const struct drm_display_mode *mode);
	bool (*detect)(struct soph_bridge *bridge);
};

struct soph_bridge {
	struct udevice *dev;
	const struct soph_bridge_funcs *funcs;
	struct soph_bridge *next_bridge;
	struct soph_connector *conn;
	struct display_state *state;
};

void soph_bridge_init(struct soph_bridge *bridge,
			  struct soph_connector *conn,
			  struct display_state *state);
void soph_bridge_enable(struct soph_bridge *bridge);
void soph_bridge_disable(struct soph_bridge *bridge);
void soph_bridge_pre_enable(struct soph_bridge *bridge);
void soph_bridge_post_disable(struct soph_bridge *bridge);
void soph_bridge_mode_set(struct soph_bridge *bridge,
			      const struct drm_display_mode *mode);
bool soph_bridge_detect(struct soph_bridge *bridge);

#endif /* __SOPH_BRIDGE_H__ */
