/* 016.c COPYRIGHT FUJITSU LIMITED 2018 */
/* after migrate, catch signal and signal-handler running */
#include <sys/mman.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include "test_mck.h"
#include "testsuite.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)
TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)

#define FLAG_INIT		0
#define FLAG_CHILD_STANDBY	1
#define FLAG_EXECUTED_SIGNAL	2

static int *check_flag = NULL;

static void signal_handler(int sig)
{
	*check_flag = FLAG_EXECUTED_SIGNAL;
}

static void child_proc_remote(void)
{
	int ret;

	ret = get_system();
	if (ret == 0) {
		printf("[NG] this process running on McKernel.\n");
		return;
	} else if (ret == -1) {
		printf("this process running on HOST-Linux.\n");
	} else {
		printf("[NG] get_system illegal return value(%d)\n", ret);
		return;
	}

	*check_flag = FLAG_CHILD_STANDBY;
	while (*check_flag != FLAG_EXECUTED_SIGNAL) {
		cpu_pause();
	}
	printf("[OK] signal-handler was executed.\n");
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int ret = 0;
	pid_t pid = 0;
	uti_attr_t attr;
	struct sigaction act;

	check_flag = (int *)mmap(NULL, sizeof(int),
				 PROT_READ | PROT_WRITE,
				 MAP_SHARED | MAP_ANONYMOUS, -1 , 0);
	tp_assert(check_flag != NULL, "mmap failed.");

	*check_flag = FLAG_INIT;

	memset(&act, 0, sizeof(act));
	act.sa_handler = signal_handler;
	sigaction(SIGUSR1, &act, NULL);

	/* for remote spawn */
	memset(&attr, 0, sizeof(attr));
	attr.flags |= UTI_FLAG_SAME_NUMA_DOMAIN;
	ret = util_indicate_clone(SPAWN_TO_REMOTE, &attr);
	if (ret != 0) {
		perror("util_indicate_clone:");
		munmap(check_flag, sizeof(int));
		tp_assert(0, "util_indicate_clone error.");
	}

	pid = fork();
	switch (pid) {
	case -1:
		munmap(check_flag, sizeof(int));
		tp_assert(0, "fork failed.");
		break;
	case 0:
		child_proc_remote();
		exit(0);
		break;
	default:
		break;
	}

	while (*check_flag != FLAG_CHILD_STANDBY) {
		cpu_pause();
	}
	kill(pid, SIGUSR1);
	waitpid(pid, NULL, 0);

	munmap(check_flag, sizeof(int));
	tp_assert(0, "please check logs. [OK] or [NG]");
	return NULL;
}
