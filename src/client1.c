#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <ulog.h>

static volatile int stop = 0;

static void int_handler(int sig)
{
	printf("client1: interrupted\n");
	stop = 1;
}

static int get_random_level()
{
	return (rand() % 4) + 1;
}

static char *get_random_string(char *str, int size)
{
	const char charset[] = "abcdefghijklmnopqrstuvwxyz";
	int n, key;

	if (size > 0) {
		--size;
		for (n = 0; n < size; n++) {
			key = rand() % (int) (sizeof charset - 1);
			str[n] = charset[key];
		}
		str[size] = '\0';
	}
	return str;
}

int main(int argc, char **argv)
{
	char msg[ULOG_MSG_MAX] = {0};
	int ret;

	printf("client1 started...\n");

	if (ulog_init("client1")) {
		fprintf(stderr, "failed to init ulog");
		exit(EXIT_FAILURE);
	}

	signal(SIGINT, int_handler);
	while (!stop) {
		ret = ulog(get_random_level(),
			get_random_string(msg, ULOG_MSG_MAX / 4));
		if (ret)
			fprintf(stderr, "failed to log a message");

		sleep(3);
	}

	ulog_destroy();
	exit(EXIT_SUCCESS);
}
