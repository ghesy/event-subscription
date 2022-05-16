/* gcc -o pulse pulse.c $(pkg-config --cflags --libs libpulse) */

#include <pulse/pulseaudio.h>
#include <stdio.h>
#include <assert.h>

void cb_state(pa_context* context, void* mainloop);
void cb_subscribe(pa_context *c, pa_subscription_event_type_t ev, uint32_t idx, void *userdata);
void cb_server_info(pa_context *c, const pa_server_info *i, void *userdata);
void cb_sink_info(pa_context *c, const pa_sink_info *i, int eol, void *userdata);

int
main(void)
{
	pa_threaded_mainloop *ml;
	pa_mainloop_api *mlapi;
	pa_context *c;

	ml = pa_threaded_mainloop_new();
	assert(ml);

	mlapi = pa_threaded_mainloop_get_api(ml);
	c = pa_context_new(mlapi, "PulseAPI.c");
	assert(c);

	/* set a callback so we can wait for the context to be ready */
	pa_context_set_state_callback(c, cb_state, ml);

	/* lock the mainloop so that it does not run and crash before the context is ready */
	pa_threaded_mainloop_lock(ml);

	pa_threaded_mainloop_start(ml);
	assert(pa_context_connect(c, NULL, PA_CONTEXT_NOFLAGS, NULL) == 0);

    while (1) {
		pa_context_state_t s = pa_context_get_state(c);
		assert(PA_CONTEXT_IS_GOOD(s));
		if (s == PA_CONTEXT_READY)
			break;
		else
			pa_threaded_mainloop_wait(ml);
	}

    pa_threaded_mainloop_unlock(ml);

	pa_context_set_subscribe_callback(c, cb_subscribe, NULL);
	pa_context_subscribe(c, PA_SUBSCRIPTION_MASK_CLIENT, NULL, NULL);

	system("sleep inf");
}

void
cb_state(pa_context *c, void *ml)
{
	pa_threaded_mainloop_signal(ml, 0);
}

void
cb_subscribe(pa_context *c, pa_subscription_event_type_t ev,
	uint32_t idx, void *userdata)
{
	if ((ev & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) == PA_SUBSCRIPTION_EVENT_CLIENT
	 && (ev & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_CHANGE) {
		pa_context_get_server_info(c, cb_server_info, NULL);
		/* printf("client changed\n"); */
	}
}

void
cb_server_info(pa_context *c, const pa_server_info *i, void *userdata)
{
	assert(i);
	pa_context_get_sink_info_by_name(c, i->default_sink_name, cb_sink_info, NULL);
	/* printf("default sink: %s\n", i->default_sink_name); */
}

void
cb_sink_info(pa_context *c, const pa_sink_info *i, int eol, void *userdata)
{
	if (i) {
		float vol = (float)pa_cvolume_avg(&(i->volume)) / (float)PA_VOLUME_NORM * 100.0f;
		printf("%.0f%%%s\n", vol, i->mute ? " (muted)" : "");
	}
}

/* vim:noexpandtab
*/
