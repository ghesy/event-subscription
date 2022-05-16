all: pulse networkmanager

pulse: pulse.c
	$(CC) $< -o $@ $(shell pkg-config --cflags --libs libpulse)

networkmanager: networkmanager.c
	$(CC) $< -o $@ $(shell pkg-config --cflags --libs libnm)

clean:
	rm -f pulse networkmanager

.PHONY: all clean
