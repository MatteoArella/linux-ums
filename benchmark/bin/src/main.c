#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>

#include "ums/benchmark/suite.h"
#include "ums/model.h"
#include "pthread/model.h"

enum task_type_e {
	PRIME
};

static char *prog_name;

static const struct option long_options[] = {
	{ "task", required_argument, 0, 't' },
	{ "workers", required_argument, 0, 'w' },
	{ "help", no_argument, 0, 'h' },
	{ 0, 0, 0, 0 }
};

static inline void print_usage(FILE *file)
{
	FILE *f = stdout;

	if (file)
		f = file;

	fprintf(f,
		"Usage: %s [OPTION]... <ums|pthread>\n"
		"\n"
		"Options:\n"
		"  -t, --task=prime             Choose benchmark task type\n"
		"  -w, --workers=NUM            Choose the number of workers"
						" to be run\n"
		"  -h, --help                   Show this help and exit\n"
		"\n",
		prog_name);

	fflush(f);
}

int main(int argc, char **argv)
{
	benchmark_suite_t	*suite;
	benchmark_suite_props_t props;
	benchmark_suite_ums_props_t ums_props = {
		.complist = &comp_list,
		.scheduler_props = {
			.sched_proc = sched_entry_proc
		}
	};
	benchmark_suite_pthread_props_t pthread_props;
	int c, retval;
	long i, n_workers = 0L;
	struct task_s **tasks;
	struct timer *timer;
	enum task_type_e task_type = PRIME;
	unsigned long task_input;

	prog_name = argv[0];

	for (;;) {
		int options_index = 0;

		c = getopt_long(argc, argv, "t:w:h",
				long_options, &options_index);
		if (c == -1)
			break;

		switch (c) {
		case 't':
			if (!strcmp(optarg, "prime")) {
				task_type = PRIME;
			} else {
				fprintf(stderr, "Type '%s' not valid\n",
					optarg);
				print_usage(stderr);
				return -1;
			}
			break;
		case 'w':
			n_workers = strtol(optarg, NULL, 10);
			break;
		case 'h':
			print_usage(NULL);
			return 0;
		case '?':
			fprintf(stderr, "Unknown option '-%c'.\n", optopt);
			print_usage(stderr);
			return -1;
		default:
			return -1;
		}
	}

	if (optind >= argc) {
		print_usage(stderr);
		return -1;
	}

	switch (task_type) {
	case PRIME:
		task_input = 0xE423UL; // 16bit prime number
		ums_props.worker_props.work_proc =
					primality_test_ums_worker_proc;
		pthread_props.work_proc =
					primality_test_pthread_worker_proc;
		break;
	default:
		break;
	}

	if (!strcmp(argv[optind], "ums")) {
		props.type = BENCHMARK_SUITE_UMS;
		props.ums_props = ums_props;
	} else if (!strcmp(argv[optind], "pthread")) {
		props.type = BENCHMARK_SUITE_PTHREAD;
		props.pthread_props = pthread_props;
	} else {
		fprintf(stderr,
			"Benchmark type '%s' not valid\n",
			argv[optind]);
		return -1;
	}

	if (!n_workers) {
		print_usage(stderr);
		return -1;
	}

	tasks = malloc(n_workers * sizeof(*tasks));
	if (!tasks) {
		perror("malloc tasks");
		return -1;
	}

	for (i = 0L; i < n_workers; i++) {
		tasks[i] = malloc(sizeof(*tasks[i]));
		if (!tasks[i]) {
			perror("malloc task");
			retval = -1;
			goto out;
		}

		tasks[i]->input = task_input;
	}

	props.n_tasks = n_workers;
	props.tasks = tasks;

	suite = benchmark_suite_create(&props);
	if (!suite) {
		perror("benchmark_suite_create");
		retval = -1;
		goto out;
	}

	retval = benchmark_suite_run(suite);
	if (retval) {
		perror("benchmark_suite_run");
		retval = -1;
		goto out;
	}

	// get elapsed time
	timer = benchmark_suite_elapsed_time(suite);

	printf("{\n"
		"  \"num_workers\": %ld,\n"
		"  \"elapsed_time\": {\n"
		"    \"nanoseconds\": %lu\n"
		"  }\n"
		"}\n",
		n_workers,
		get_nanoseconds(timer));

	retval = benchmark_suite_destroy(suite);

out:
	for (; i > 0L; i--)
		free(tasks[i - 1]);
	free(tasks);

	return retval;
}
