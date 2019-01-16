/* 001.c COPYRIGHT FUJITSU LIMITED 2016-2017 */
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <sys/stat.h>
#include <pthread.h>

#include "test_mck.h"

SETUP_ALIAS(TEST_SUITE, TEST_NUMBER, 0)

void *tkill001_child_func(void *arg)
{
	pid_t *c_tid = (pid_t *)arg;

	/* send thread-id */
	*c_tid = syscall(__NR_gettid);

	printf("[child]pid=%d, tid=%d\n", getpid(), *c_tid);
	
	while(1) {
		cpu_pause();
	}
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	pthread_t child_th;
	pid_t child_tid = 0;

	int status = pthread_create(&child_th, NULL, tkill001_child_func, &child_tid);
	tp_assert(status == 0, "failed to create thread");

	printf("[parent]sleep a few sec...\n");
	sleep(1);
	printf("[parent]wake up\n");

	/* send signal to child thread. */
	tp_assert(syscall(__NR_tkill, child_tid, SIGUSR1) == 0, "tkill failed");

	while(1) {
		cpu_pause();
	}

	/* テスト成功 */
	return NULL;
}

TEARDOWN_ALIAS(TEST_SUITE, TEST_NUMBER, 0)

