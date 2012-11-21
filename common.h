const char *EV_PREFIX  = "/dev/input/";
const char *DEF_FN = "/sdcard/events";

char *ev_devices[] = {
	"event0", "event1", "event2", "event3", "event4", "event5",
	"event6", "event7", "event8", "event9", "event10",
};

#define NUM_DEVICES (sizeof(ev_devices) / sizeof(char *))

