#include "nm-ev.h"

static void
nm_ev_callback(GDBusProxy *proxy, char *sendername, char *signalname,
	GVariant *parameters, void (*callback)(void))
{
	callback();
}

int
nm_ev_listen(void (*callback)(void))
{
	GMainLoop *loop;
	GDBusProxy *proxy;
	GDBusProxyFlags flags;
	GError *error = NULL;

	flags = G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES | G_DBUS_PROXY_FLAGS_DO_NOT_AUTO_START;
	proxy = g_dbus_proxy_new_for_bus_sync(
		G_BUS_TYPE_SYSTEM,
		flags,
		NULL, /* GDBusInterfaceInfo */
		"org.freedesktop.NetworkManager",
		"/org/freedesktop/NetworkManager",
		"org.freedesktop.NetworkManager",
		NULL, /* GCancellable */
		&error
	);

	if (proxy == NULL) {
		g_dbus_error_strip_remote_error(error);
		g_printerr("Error creating D-Bus proxy: %s\n", error->message);
		g_error_free(error);
		return 0;
	}

	/* connect to g-signal to receive signals from proxy (remote object) */
	g_signal_connect(proxy, "g-signal", G_CALLBACK(nm_ev_callback), callback);

	/* run main loop */
	loop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(loop);

	g_object_unref(proxy);

	return 1;
}

/* vim:noexpandtab
*/
