/* 017.c COPYRIGHT FUJITSU LIMITED 2016 */
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include "test_mck.h"
#include "testsuite.h"

static char* do_fork;

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int result = 0;
	pid_t pid;
	int status;
	struct rusage rusage;
	struct memory_info info;

	do_fork = mmap(NULL, PAGE_SIZE, PROT_WRITE|PROT_READ, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	tp_assert(do_fork != MAP_FAILED, "mmap error.");
	memset(do_fork, 'z', PAGE_SIZE);

	status = get_memory_info_self((unsigned long)do_fork, &info);
	tp_assert(status == 0, "get memory info error.");
	tp_assert(info.phys != 0, "memory allocation error.");

	status = madvise(do_fork, PAGE_SIZE, MADV_DONTFORK);
	tp_assert(status == 0, "madvise(MADV_DONTFORK) error.");

	status = madvise(do_fork, PAGE_SIZE, MADV_DOFORK);
	tp_assert(status == 0, "madvise(MADV_DOFORK) error.");

	pid = fork();
	tp_assert(pid != -1, "fork error");

	switch (pid) {
	case 0:
	{
#define __local_tp_assert(exp, msg, exit) do{ if(!(exp)) { printf("%s", msg); _exit(exit); } } while(0)
		/* child process */
		int i;
		struct memory_info childinfo;
		status = get_memory_info_self((unsigned long)do_fork, &childinfo);
		__local_tp_assert(status == 0, "child: get memory info error.\n", -1);
		__local_tp_assert(childinfo.phys != 0, "memory is not present.", -1);
		for (i = 0; i + 10 < PAGE_SIZE; i += 10) {
			__local_tp_assert(do_fork[i + 0] == 'z', "invalid vaule", 1);
			__local_tp_assert(do_fork[i + 1] == 'z', "invalid vaule", 2);
			__local_tp_assert(do_fork[i + 2] == 'z', "invalid vaule", 3);
			__local_tp_assert(do_fork[i + 3] == 'z', "invalid vaule", 4);
			__local_tp_assert(do_fork[i + 4] == 'z', "invalid vaule", 5);
			__local_tp_assert(do_fork[i + 5] == 'z', "invalid vaule", 6);
			__local_tp_assert(do_fork[i + 6] == 'z', "invalid vaule", 7);
			__local_tp_assert(do_fork[i + 7] == 'z', "invalid vaule", 8);
			__local_tp_assert(do_fork[i + 8] == 'z', "invalid vaule", 9);
			__local_tp_assert(do_fork[i + 9] == 'z', "invalid vaule", 10);
		}
		for (; i < PAGE_SIZE; i++) {
			__local_tp_assert(do_fork[i] == 'z', "invalid vaule", 11);
		}
		_exit(0);
	}
	default:
	{
		int exit_status;
		/* parent process */
		result = wait4(pid, &status, 0, &rusage);
		tp_assert(result != -1, "wait4 error");
		tp_assert(WIFEXITED(status), "WIFEXITED error.");
		exit_status = WEXITSTATUS(status);
		tp_assert(exit_status == 0, "WEXITSTATUS error.");
	}
		break;
	}//switch
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
