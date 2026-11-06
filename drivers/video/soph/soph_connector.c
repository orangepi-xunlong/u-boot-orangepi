#include <dm/device.h>
#include <dm/read.h>
#include <linux/compat.h>
#include <linux/list.h>

#include "soph_crtc.h"
#include "soph_connector.h"


static LIST_HEAD(soph_connector_list);

int soph_connector_bind(struct soph_connector *conn, struct udevice *dev, int id,
			    const struct soph_connector_funcs *funcs, void *data, int type)
{
	conn->id = id;
	conn->dev = dev;
	conn->funcs = funcs;
	conn->data = data;
	conn->type = type;
	list_add_tail(&conn->head, &soph_connector_list);

	return 0;
}

struct soph_connector *get_soph_connector_by_device(struct udevice *dev)
{
	struct soph_connector *conn;

	list_for_each_entry(conn, &soph_connector_list, head) {
		if (conn->dev == dev)
			return conn;
	}

	return NULL;
}

int soph_connector_pre_init(struct display_state *state)
{
	int ret = 0;
	struct soph_connector *conn;

	conn = state->conn_state.connector;
	if (conn->funcs->pre_init) {
		ret = conn->funcs->pre_init(conn, state);
		if (ret)
			return ret;
		if (state->conn_state.secondary) {
			conn = state->conn_state.connector;
			ret = conn->funcs->pre_init(conn, state);
			if (ret)
				return ret;
		}
	}

	return ret;
}

static int soph_connector_path_init(struct soph_connector *conn,
					struct display_state *state)
{
	int ret = 0;

	// if (conn->panel)
	//	soph_panel_init(conn->panel, conn, state);

	if (conn->bridge)
		soph_bridge_init(conn->bridge, conn, state);

	if (conn->funcs->init) {
		ret = conn->funcs->init(conn, state);
		if (ret)
			return ret;
	}


	return ret;
}

int soph_connector_init(struct display_state *state)
{
	int ret = 0;
	struct soph_connector *conn;

	conn = state->conn_state.connector;
	ret = soph_connector_path_init(conn, state);
	if (ret)
		return ret;
	if (state->conn_state.secondary) {
		conn = state->conn_state.secondary;
		ret = soph_connector_path_init(conn, state);
		if (ret)
			return ret;
	}

	return ret;
}

int soph_connector_deinit(struct display_state *state)
{
	struct soph_connector *conn;

	conn = state->conn_state.connector;
	if (conn->funcs->deinit) {
		conn->funcs->deinit(conn, state);
		if (state->conn_state.secondary) {
			conn = state->conn_state.secondary;
			conn->funcs->deinit(conn, state);
		}
	}

	return 0;
}

static bool soph_connector_path_detect(struct soph_connector *conn,
					   struct display_state *state)
{
	int ret;

	if (conn->funcs->detect) {
		ret = conn->funcs->detect(conn, state);
		if (!ret) {
			printf("%s disconnected\n", conn->dev->name);
			return false;
		}
	}

	if (conn->bridge) {
		ret = soph_bridge_detect(conn->bridge);
		if (!ret) {
			printf("%s disconnected\n",
			       dev_np(conn->bridge->dev)->full_name);
			return false;
		}
	}

	return true;
}

bool soph_connector_detect(struct display_state *state)
{
	bool ret;
	struct soph_connector *conn;

	conn = state->conn_state.connector;
	ret = soph_connector_path_detect(conn, state);
	if (!ret)
		return false;
	if (state->conn_state.secondary) {
		conn = state->conn_state.secondary;
		ret = soph_connector_path_detect(conn, state);
		if (!ret)
			return false;
	}

	return true;
}

int soph_connector_get_timing(struct display_state *state)
{
	int ret = 0;
	struct soph_connector *conn;

	conn = state->conn_state.connector;
	if (conn->funcs->get_timing) {
		ret = conn->funcs->get_timing(conn, state);
		if (ret)
			return ret;
		if (state->conn_state.secondary) {
			conn = state->conn_state.secondary;
			ret = conn->funcs->get_timing(conn, state);
			if (ret)
				return ret;
		}
	}

	return ret;
}

int soph_connector_get_edid(struct display_state *state)
{
	int ret = 0;
	struct soph_connector *conn;

	conn = state->conn_state.connector;
	if (conn->funcs->get_edid) {
		ret = conn->funcs->get_edid(conn, state);
		if (ret)
			return ret;
		if (state->conn_state.secondary) {
			conn = state->conn_state.secondary;
			ret = conn->funcs->get_edid(conn, state);
			if (ret)
				return ret;
		}
	}

	return ret;
}

static int soph_connector_path_pre_enable(struct soph_connector *conn,
					      struct display_state *state)
{
	if (conn->funcs->prepare)
		conn->funcs->prepare(conn, state);

	if (conn->bridge)
		soph_bridge_pre_enable(conn->bridge);

	// if (conn->panel)
	//	soph_panel_prepare(conn->panel);

	return 0;
}

int soph_connector_pre_enable(struct display_state *state)
{
	struct soph_connector *conn;

	conn = state->conn_state.connector;
	soph_connector_path_pre_enable(conn, state);
	if (state->conn_state.secondary) {
		conn = state->conn_state.secondary;
		soph_connector_path_pre_enable(conn, state);
	}

	return 0;
}

static int soph_connector_path_enable(struct soph_connector *conn,
					  struct display_state *state)
{
	if (conn->funcs->enable)
		conn->funcs->enable(conn, state);

	if (conn->bridge)
		soph_bridge_enable(conn->bridge);

	// if (conn->panel)
	//	soph_panel_enable(conn->panel);

	return 0;
}

int soph_connector_enable(struct display_state *state)
{
	struct soph_connector *conn;

	conn = state->conn_state.connector;
	soph_connector_path_enable(conn, state);
	if (state->conn_state.secondary) {
		conn = state->conn_state.secondary;
		soph_connector_path_enable(conn, state);
	}

	return 0;
}

static int soph_connector_path_disable(struct soph_connector *conn,
					   struct display_state *state)
{
	// if (conn->panel)
	//	soph_panel_disable(conn->panel);

	if (conn->bridge)
		soph_bridge_disable(conn->bridge);

	if (conn->funcs->disable)
		conn->funcs->disable(conn, state);

	return 0;
}

int soph_connector_disable(struct display_state *state)
{
	struct soph_connector *conn;

	conn = state->conn_state.connector;
	soph_connector_path_disable(conn, state);
	if (state->conn_state.secondary) {
		conn = state->conn_state.secondary;
		soph_connector_path_disable(conn, state);
	}

	return 0;
}

static int soph_connector_path_post_disable(struct soph_connector *conn,
						struct display_state *state)
{
	// if (conn->panel)
	//	soph_panel_unprepare(conn->panel);

	if (conn->bridge)
		soph_bridge_post_disable(conn->bridge);

	if (conn->funcs->unprepare)
		conn->funcs->unprepare(conn, state);

	return 0;
}

int soph_connector_post_disable(struct display_state *state)
{
	struct soph_connector *conn;

	conn = state->conn_state.connector;
	soph_connector_path_post_disable(conn, state);
	if (state->conn_state.secondary) {
		conn = state->conn_state.secondary;
		soph_connector_path_post_disable(conn, state);
	}

	return 0;
}
