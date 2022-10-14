#include <stdio.h> 
#include <stdlib.h>
#include <getopt.h> //cli argument parsing
#include <string.h>
#include <ulog.h> //<> reiskia kad header turi buti patalpintas i sistemini headeriu folderi pvz /usr/include "" reiskia kad header yra toje pat direktorijoje kaio ir c failas is kurio jis includinamas

struct option opts[] = { //initialise array
	{ "help", no_argument, NULL, 'h' }, //
	{ "program", required_argument, NULL, 'p' },
	{ "level", required_argument, NULL, 'l' },
	{ NULL, 0, NULL, 0 }
};

static void usage(const char *exec)
{
	fprintf(stderr, "usage: %s [-h] [-p program] [-l level]\n"
		"\t-h --help\t show this message\n"
		"\t-p --program\t specify program\n"
		"\t-l --level\t specify log level\n", exec);
}

int main(int argc, char **argv)
{
	char value[ULOG_NAME_MAX] = {0};
	char *program = NULL;

	int level = ULOG_ALL;
	int id, opt;

	while (1) {
		opt = getopt_long(argc, argv, "hp:l:", opts, &id);
		if (opt == -1)
			break;

		switch (opt)
		{
			case 'p':
				strncpy(value, optarg, ULOG_NAME_MAX);
				program = value;
				break;
			case 'l':
				level = atoi(optarg);
				break;
			default:
				usage(basename(argv[0]));
				exit(EXIT_FAILURE);
		}
	}

	if (ulog_init(NULL)) {
		fprintf(stderr, "failed to init ulog");
		exit(EXIT_FAILURE);
	}
	ulog_print(program, level);
	ulog_destroy();

	exit(EXIT_SUCCESS);
}
