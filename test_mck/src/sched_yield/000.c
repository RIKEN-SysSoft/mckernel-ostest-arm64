/* 000.c COPYRIGHT FUJITSU LIMITED 2016 */
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/mman.h>

#include "test_mck.h"

static sem_t *sync_sem1 = MAP_FAILED;
static sem_t *sync_sem2 = MAP_FAILED;
static int *parent_core = MAP_FAILED;
static int *order_flag = MAP_FAILED;

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	/* get shared memory */
	sync_sem1 = (sem_t *)mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	sync_sem2 = (sem_t *)mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	parent_core = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	order_flag = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	return NULL;
}

TEARDOWN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	/* unmap semaphore memory */
	if (sync_sem1 != MAP_FAILED) {
		munmap(sync_sem1, sizeof(sem_t));
	}

	if (sync_sem2 != MAP_FAILED) {
		munmap(sync_sem2, sizeof(sem_t));
	}

	if (parent_core != MAP_FAILED) {
		munmap(parent_core, sizeof(int));
	}
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	pid_t pid = -1;
	pid_t ret_pid = -1;
	int status = 0;

	/* mmap check */
	tp_assert(sync_sem1 != MAP_FAILED, "mmap() Failed.");
	tp_assert(sync_sem2 != MAP_FAILED, "mmap() Failed.");
	tp_assert(parent_core != MAP_FAILED, "mmap() Failed.");

	/* semaphore init */
	sem_init(sync_sem1, 1, 0);
	sem_init(sync_sem2, 1, 0);

	/* create child process */
	pid = fork();

	switch(pid) {
	case -1:
		/* error */
		tp_assert(0, "fork() Failed." );
		break;

	case 0: {
		/* child */
		/* before migrate, get cpunum */
		int old_mycore = sched_getcpu();
		int new_mycore = -1;

		printf("[child] running core %d\n", old_mycore);
		fflush(stdout);

		/* sync parent */
		sem_post(sync_sem1);

		/* sync parent */
		sem_wait(sync_sem2);

		*order_flag = 1;
		/* after migrate, get cpunum */
		new_mycore = sched_getcpu();

		/* corenum check. */
		if (*parent_core != old_mycore) {
			printf("[child] before migrate prevcore %d, nowcore %d\n", old_mycore, new_mycore);
			fflush(stdout);

			if (old_mycore == new_mycore) {
				printf("TP failed, not migrate child process.\n");
				fflush(stdout);
				_exit(-1);
			}
		} else {
			printf("[child] migrate not required.\n");
			fflush(stdout);
		}
		printf("[child] End process.\n");
		fflush(stdout);
		_exit(0);
		break;
	}

	default: {
		/* parent */
		cpu_set_t cpuset;
		int result = -1;

		/* sync child */
		sem_wait(sync_sem1);

		/* parent corenum get */
		*parent_core = sched_getcpu();

		/* child process to migrate parent core */
		printf("[parent] running core %d\n", *parent_core);
		printf("[parent] child process migrate/bind to core %d\n", *parent_core);
		fflush(stdout);

		CPU_ZERO(&cpuset);
		CPU_SET(*parent_core, &cpuset);

		result = sched_setaffinity(pid, sizeof(cpuset), &cpuset);
		if (result == -1) {
			printf("errno = %d\n", errno);
			fflush(stdout);
			tp_assert(0, "child migrate/bind sched_setaffinity failed.");
		}

		/* parent core bind */
		printf("[parent] parent process bind to core %d\n", *parent_core);
		fflush(stdout);
		result = sched_setaffinity(0, sizeof(cpuset), &cpuset);
		if (result == -1) {
			printf("errno = %d\n", errno);
			fflush(stdout);
			tp_assert(0, "parent bind sched_setaffinity failed.");
		}

		/* sched_setaffinity interval */
		usleep(10000);

		/* sync child, switch to child process */
		printf("[parent] send sched_yield.\n");
		fflush(stdout);
		*order_flag = 0;
		sem_post(sync_sem2);
		result = sched_yield();
		if (result == -1) {
			tp_assert(0, "sched_yield failed.");
		}
		tp_assert(*order_flag == 1, "sched_yield don't schedule to child");

		printf("[parent] End process.\n");
		fflush(stdout);
		break;
	}
	}

	/* child process status check. */
	ret_pid = wait(&status);
	if (ret_pid == pid) {
		if (WIFEXITED(status)) {
			if (WEXITSTATUS(status)) {
				tp_assert(0, "TP failed, child migrate fail.");
			}
		}
	} else {
		tp_assert(0, "TP failed, child process wait() fail.");
	}
	return NULL;
}
