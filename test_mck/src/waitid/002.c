/* 002.c COPYRIGHT FUJITSU LIMITED 2015-2018 */
#include "test_mck.h"
#include "testsuite.h"
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	static struct waitid_args args;
	int opt;

	memset(&args, 0 ,sizeof(args));

	while ((opt = getopt(tc_argc, tc_argv, "p:")) != -1) {
		switch (opt) {
		case 'p':
			args.proc_num = atoi(optarg);
			break;
		default:
			break;
		}
	}
	return &args;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int result = 0;
	int i = 0;
	pid_t pid;
	siginfo_t info;
	struct waitid_args *args = (struct waitid_args*)tc_arg;
	sem_t *wait = NULL;

	tp_assert(args != NULL, "-p <child proc num>");
	tp_assert(0 < args->proc_num, "-p <child proc num> invalid argument.");

	wait = (sem_t *)mmap(NULL, sizeof(sem_t),
			     PROT_READ | PROT_WRITE,
			     MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	tp_assert(wait != MAP_FAILED, "semaphore mmap failed.");

	sem_init(wait, 1, 0);

	for (i = 0; i < args->proc_num; i++) {
		pid = fork();
		switch(pid){
		case -1:
			break;
		case 0:
			/* child process */
			sem_wait(wait);
			munmap(wait, sizeof(sem_t));
			usleep(100000 + 100000 * i);
			printf("[child:%5d] sleep 100+%dmsec done.\n", getpid(), 100 * i);
			_exit(0);
			break;
		default:
			/* parent process */
			break;
		}
	}

	/* parent process */
	pid = getpid();
	for (i = 0; i < args->proc_num; i++) {
		sem_post(wait);
	}
	munmap(wait, sizeof(sem_t));

	for (i = 0; i < args->proc_num; i++) {
		printf("[parent:%5d] waitid(P_ALL) called.\n", pid);
		result = waitid(P_ALL, 0, &info, WEXITED);
		printf("[parent:%5d] waitid(P_ALL) returned. EXTED pid = %d\n", pid, info.si_pid);

		printf("[parent:%5d] waitid()=%d, errno=%d\n", pid, result, errno);
		tp_assert(result != -1, "waitid failed.");
	}
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
