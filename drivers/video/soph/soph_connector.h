#ifndef __SOPH_CONNECTOR_H__
#define __SOPH_CONNECTOR_H__

#include "soph_bridge.h"
// #include "soph_panel.h"

struct soph_connector {
	struct udevice *dev;
	struct soph_bridge *bridge;
	struct soph_panel *panel;
	struct soph_phy *phy;
	struct list_head head;
	int id;
	int type;

	const struct soph_connector_funcs *funcs;
	void *data;
};

struct soph_connector_funcs {
	/*
	 * pre init connector, prepare some parameter out_if, this will be
	 * used by soph_display.c and vop
	 */
	int (*pre_init)(struct soph_connector *connector, struct display_state *state);

	/*
	 * init connector, prepare resource to ensure
	 * detect and get_timing can works
	 */
	int (*init)(struct soph_connector *connector, struct display_state *state);

	void (*deinit)(struct soph_connector *connector, struct display_state *state);
	/*
	 * Optional, if connector not support hotplug,
	 * Returns:
	 *   0 means disconnected, else means connected
	 */
	int (*detect)(struct soph_connector *connector, struct display_state *state);
	/*
	 * Optional, if implement it, need fill the timing data:
	 *     state->conn_state->mode
	 * you can refer to the soph_display: display_get_timing(),
	 * Returns:
	 *   0 means success, else means failed
	 */
	int (*get_timing)(struct soph_connector *connector, struct display_state *state);
	/*
	 * Optional, if implement it, need fill the edid data:
	 *     state->conn_state->edid
	 * Returns:
	 *   0 means success, else means failed
	 */
	int (*get_edid)(struct soph_connector *connector, struct display_state *state);
	/*
	 * call before crtc enable.
	 */
	int (*prepare)(struct soph_connector *connector, struct display_state *state);
	/*
	 * call after crtc enable
	 */
	int (*enable)(struct soph_connector *connector, struct display_state *state);
	int (*disable)(struct soph_connector *connector, struct display_state *state);
	void (*unprepare)(struct soph_connector *connector, struct display_state *state);

	int (*check)(struct soph_connector *connector, struct display_state *state);
	int (*mode_valid)(struct soph_connector *connector, struct display_state *state);
};

const struct soph_connector *
soph_get_connector(const void *blob, int connector_node);
int soph_connector_bind(struct soph_connector *connector, struct udevice *dev, int id,
			    const struct soph_connector_funcs *funcs, void *data, int type);
struct soph_connector *get_soph_connector_by_device(struct udevice *dev);
int soph_connector_pre_init(struct display_state *state);
int soph_connector_init(struct display_state *state);
int soph_connector_deinit(struct display_state *state);
bool soph_connector_detect(struct display_state *state);
int soph_connector_get_timing(struct display_state *state);
int soph_connector_get_edid(struct display_state *state);
int soph_connector_pre_enable(struct display_state *state);
int soph_connector_enable(struct display_state *state);
int soph_connector_disable(struct display_state *state);
int soph_connector_post_disable(struct display_state *state);

#endif /* __SOPH_CONNECTOR_H__ */
