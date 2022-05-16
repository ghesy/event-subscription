all: pulse networkmanager

pulse: pulse.c
	$(CC) -o pulse pulse.c $(shell pkg-config --cflags --libs libpulse)

networkmanager: networkmanager.c
	$(CC) -o networkmanager networkmanager.c $(shell pkg-config --cflags --libs libnm)

clean:
	rm -f pulse networkmanager

.PHONY: all clean
