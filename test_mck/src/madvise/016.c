/* 016.c COPYRIGHT FUJITSU LIMITED 2016 */
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

static char* dont_fork;

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int result = 0;
	pid_t pid;
	int status;
	struct rusage rusage;
	struct memory_info info;

	dont_fork = mmap(NULL, PAGE_SIZE, PROT_WRITE|PROT_READ, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	tp_assert(dont_fork != MAP_FAILED, "mmap error.");
	memset(dont_fork, 'z', PAGE_SIZE);

	status = get_memory_info_self((unsigned long)dont_fork, &info);
	tp_assert(status == 0, "get memory info error.");
	tp_assert(info.phys != 0, "memory allocation error.");

	status = madvise(dont_fork, PAGE_SIZE, MADV_DONTFORK);
	tp_assert(status == 0, "madvise error.");

	pid = fork();
	tp_assert(pid != -1, "fork error");

	switch (pid) {
	case 0:
	{
#define __local_tp_assert(exp, msg, exit) do{ if(!(exp)) { printf("%s", msg); _exit(exit); } } while(0)
		/* child process */
		status = get_memory_info_self((unsigned long)dont_fork, &info);
		__local_tp_assert(status == 0, "child: get memory info error.\n", -1);
	        __local_tp_assert(info.phys == 0, "dont fork error.\n", -1);
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
