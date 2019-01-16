/* 010.c COPYRIGHT FUJITSU LIMITED 2016 */
/* ptrace(PTRACE_SETREGSET) testcase */
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/ptrace.h>
#include <asm/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include "test_mck.h"
#include "arch_test_ptrace.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

static unsigned long regno[2] = { 0 };
static unsigned long inst_addr = 0;

static inline void register_read(unsigned long *buf)
{
	asm volatile (
		"stp x0, x1, [%0, #16 * 0]\n"
		"stp x2, x3, [%0, #16 * 1]\n"
		"stp x4, x5, [%0, #16 * 2]\n"
		"stp x6, x7, [%0, #16 * 3]\n"
		"stp x8, x9, [%0, #16 * 4]\n"
		"stp x10, x11, [%0, #16 * 5]\n"
		"stp x12, x13, [%0, #16 * 6]\n"
		"stp x14, x15, [%0, #16 * 7]\n"
		"stp x16, x17, [%0, #16 * 8]\n"
		"stp x18, x19, [%0, #16 * 9]\n"
		"stp x20, x21, [%0, #16 * 10]\n"
		"stp x22, x23, [%0, #16 * 11]\n"
		"stp x24, x25, [%0, #16 * 12]\n"
		"stp x26, x27, [%0, #16 * 13]\n"
		"stp x28, x29, [%0, #16 * 14]\n"
		: "=r"(buf)
		: /* nothing */
		: "memory"
	);
}

static int child_func(void)
{
	int ret = -1;
	struct user_pt_regs gregs;

	memset(&gregs, 0, sizeof(gregs));

	/* send PTRACE_TRACEME */
	if (ptrace(PTRACE_TRACEME, 0, NULL, NULL)) {
		perror("ptrace(PTRACE_TRACEME)");
		goto out;
	}

	/* pre write register */
	pre_getregset();

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

	register_read((unsigned long *)gregs.regs);

	if (setregset_child_check(regno[0], regno[1], &gregs)) {
		printf("setregset_child_check failed.\n");
		goto out;
	}

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

	if (setregset_check(cpid, &regno[0], &regno[1])) {
		printf("setregset_check failed.\n");
		goto cont;
	}

	/* write child regno */
	if (ptrace(PTRACE_POKEDATA, cpid, &regno[0], regno[0])) {
		perror("ptrace(PTRACE_POKEDATA)");
		goto cont;
	}

	if (ptrace(PTRACE_POKEDATA, cpid, &regno[1], regno[1])) {
		perror("ptrace(PTRACE_POKEDATA)");
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
