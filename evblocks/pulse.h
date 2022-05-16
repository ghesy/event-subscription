#include <pulse/pulseaudio.h>

void pulse_info_init(void);
void pulse_info_get(char *info);
int pulse_ev_listen(void (*callback)(void));
