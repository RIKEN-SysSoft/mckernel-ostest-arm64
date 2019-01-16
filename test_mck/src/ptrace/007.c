/* 007.c COPYRIGHT FUJITSU LIMITED 2016 */
/* ptrace(PTRACE_POKEUSER) testcase */
#include "test_mck.h"
#include "arch_test_ptrace.h"
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

static unsigned long inst_addr = 0;

static int child_func(void)
{
	int ret = -1;

	/* send PTRACE_TRACEME */
	if (ptrace(PTRACE_TRACEME, 0, NULL, NULL)) {
		perror("ptrace(PTRACE_TRACEME)");
		goto out;
	}

	/* stop mine, brk instruction */
	/* rewrite nop from parent process */
	asm volatile(
		"adr x10, 1f\n"
		"str x10, [%0]\n"
		"nop\n"
		"nop\n"
		"1:\n"
		"brk #0\n"
		"nop\n"
		: /* nothing */
		: "r"(&inst_addr)
		: "x10"
	);

	/* success */
	ret = 0;
out:
	return ret;
}

static int parent_func(pid_t cpid)
{
	pid_t pid = 0;
	int status = 0;
	int ret = -1;
	long read_val = 0;

	/* wait child stop */
	pid = wait(&status);
	if (pid == cpid) {
		if (!WIFSTOPPED(status)) {
			printf("child is not stopped.\n");
			goto out;
		}
	} else {
		perror("wait()");
		goto out;
	}

	/* rewrite child brk instruction */
	if (rewrite_brk_inst(cpid, &inst_addr)) {
		goto cont;
	}

	/* write child buf */
	read_val = ptrace(PTRACE_POKEUSER, cpid, 0, 0);
	if ((read_val != -1) && (!errno)) {
		printf("ptrace(PTRACE_POKEUSER) why successed ???\n");
		goto cont;
	}

	if (errno == EIO) {
		printf("ptrace(PTRACE_POKEUSER) expected errno EIO(%d) catch\n", EIO);
	} else {
		printf("ptrace(PTRACE_POKEUSER) unexpected errno(%d)\n", errno);
		printf("expected errno = EIO(%d)\n", EIO);
		goto cont;
	}

	/* success */
	ret = 0;
cont:
	/* child continue */
	if (ptrace(PTRACE_CONT, cpid, NULL, NULL)) {
		perror("ptrace(PTRACE_CONT)");
		ret = -1;
	}
out:
	return ret;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	pid_t pid = 0;
	int func_ret = 0;
	int status = 0;

	/* create child process */
	pid = fork();

	switch (pid) {
	case -1:
		/* fork() error. */
		perror("fork()");
		return "fork() failed.";
		break;
	case 0:
		/* child process */
		func_ret = child_func();

		/* child exit */
		exit(func_ret);
		break;
	default:
		/* parent process */
		func_ret = parent_func(pid);

		/* wait child */
		pid = wait(&status);
		if (pid != -1) {
			if (WEXITSTATUS(status)) {
				return "TP failed.";
			}
		} else {
			perror("wait()");
			return "wait() failed.";
		}

		/* parent_func check */
		if (func_ret) {
			return "TP failed.";
		}
		break;
	}

	/* sccess. */
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
