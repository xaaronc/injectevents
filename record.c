#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <poll.h>
#include <linux/input.h>
#include "uinput.h"
#include "common.h"

#define EVENTFILE_PERMS (S_IRUSR|S_IWUSR|S_IWGRP|S_IRGRP|S_IROTH|S_IWOTH) /* 666 */

struct pollfd in_fds[NUM_DEVICES];
int out_fd;

static int
init(const char *outfn)
{
	char buffer[256];
	int i;

	out_fd = open(outfn, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR|S_IWUSR|S_IWGRP|S_IRGRP|S_IROTH);
	if (out_fd < 0) {
		perror("failed to open output file");
		return 1;
	}

	for (i = 0; i < NUM_DEVICES; i++) {
		sprintf(buffer, "%s%s", EV_PREFIX, ev_devices[i]);
		in_fds[i].events = POLLIN;
		in_fds[i].fd = open(buffer, O_RDONLY | O_NDELAY);
		if (in_fds[i].fd < 0) {
			fprintf(stderr, "failed to open input device `%s': ", ev_devices[i]);
			perror(NULL);
			return 2;
		}
	}

	return 0;
}

static int
record(void)
{
	int i, numread;
	struct input_event event;

	while (1) {
		if (poll(in_fds, NUM_DEVICES, -1) < 0) {
			perror("poll error");
			return 1;
		}

		for (i = 0; i < NUM_DEVICES; i++) {
			if (in_fds[i].revents & POLLIN) {
				/* Data available */
				numread = read(in_fds[i].fd, &event, sizeof(event));
				if (numread != sizeof(event)) {
					perror("read error");
					return 2;
				}
				if (write(out_fd, &i, sizeof(i)) != sizeof(i)) {
					perror("device write error");
					return 3;
				}
				if (write(out_fd, &event, sizeof(event)) != sizeof(event)) {
					perror("event write error");
					return 4;
				}

//				printf("input %d, time %ld.%06ld, type %d, code %d, value %d\n", i,
//						event.time.tv_sec, event.time.tv_usec, event.type, event.code, event.value);
			}
		}
	}
}

int
main(int argc, char *argv[])
{
	const char *outfn = DEF_FN;

	if (argc > 1) {
		outfn = argv[1];
	} else {
		fprintf(stderr, "using default output file (%s)\n", DEF_FN);
	}

	if (init(outfn) != 0) {
		fprintf(stderr, "init failed\n");
		return 1;
	}

	record();

	return 0;
}

