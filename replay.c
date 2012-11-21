#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h> 
#include <unistd.h>
#include <linux/input.h>
#include "uinput.h"
#include "common.h"

static int out_fds[NUM_DEVICES];
static int num_events;
static int in_fd;

static int
init(const char *in_fn)
{
	char buf[256];
	unsigned i;
	struct stat statinfo;

	for (i = 0; i < NUM_DEVICES; i++) {
		sprintf(buf, "%s%s", EV_PREFIX, ev_devices[i]);
		out_fds[i] = open(buf, O_WRONLY | O_NDELAY);
		if (out_fds[i] < 0) {
			perror("failed to open output device");
			return 1;
		}
	}

	if (stat(in_fn, &statinfo) == -1) {
		perror("failed to stat input file");
		return 2;
	}

	num_events = statinfo.st_size / (sizeof(struct input_event) + sizeof(int));

	if ((in_fd = open(in_fn, O_RDONLY)) < 0) {
		perror("failed to open input file");
		return 3;
	}

	// Hacky ioctl init
	ioctl (out_fds[3], UI_SET_EVBIT, EV_KEY);
	ioctl (out_fds[3], UI_SET_EVBIT, EV_REP);
	ioctl (out_fds[1], UI_SET_EVBIT, EV_ABS);

	return 0;
}

static int
replay(void)
{
	struct timeval tdiff;
	struct input_event event;
	int i, outputdev;

	timerclear(&tdiff);

	for (i = 0; i < num_events; i++) {
		struct timeval now, tevent, tsleep;

		if (read(in_fd, &outputdev, sizeof(outputdev)) != sizeof(outputdev)) {
			perror("device read error");
			return 1;
		}

		if (read(in_fd, &event, sizeof(event)) != sizeof(event)) {
			perror("event read error");
			return 2;
		}

		gettimeofday(&now, NULL);
		if (!timerisset(&tdiff)) {
			timersub(&now, &event.time, &tdiff);
		}

		timeradd(&event.time, &tdiff, &tevent);
		timersub(&tevent, &now, &tsleep);
		if (tsleep.tv_sec > 0 || tsleep.tv_usec > 100)
			select(0, NULL, NULL, NULL, &tsleep);

		event.time = tevent;

		if (write(out_fds[outputdev], &event, sizeof(event)) != sizeof(event)) {
			perror("output write error");
			return 2;
		}

//		printf("input %d, time %ld.%06ld, type %d, code %d, value %d\n", outputdev,
//				event.time.tv_sec, event.time.tv_usec, event.type, event.code, event.value);
	}

	return 0;
}

int
main(int argc, char *argv[])
{
	const char *infn = DEF_FN;

	if (argc > 1) {
		infn = argv[1];
	} else {
		fprintf(stderr, "using default input file (%s)\n", DEF_FN);
	}

	if (init(infn) != 0) {
		fprintf(stderr, "init failed\n");
		return 1;
	}

	if (replay() != 0) {
		fprintf(stderr, "replay failed\n");
		return 2;
	}

	return 0;
}

