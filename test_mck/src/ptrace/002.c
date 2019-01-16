/* 002.c COPYRIGHT FUJITSU LIMITED 2016 */
/* ptrace(PTRACE_PEEKDATA) testcase */
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

static unsigned long buf = 0;
static unsigned long inst_addr = 0;
#define CHILD_MAGIC  0x1234567890abcdef
#define PARENT_MAGIC 0xffaaffaaffaaffaa

static int child_func(void)
{
	int ret = -1;

	/* send PTRACE_TRACEME */
	if (ptrace(PTRACE_TRACEME, 0, NULL, NULL)) {
		perror("ptrace(PTRACE_TRACEME)");
		goto out;
	}

	/* write buffer to magic */
	buf = CHILD_MAGIC;

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

	/* child buf check */
	if (buf != CHILD_MAGIC) {
		printf("why child process buf rewrite ??? (0x%lx)\n", buf);
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
	long read_val = 0;

	/* write buffer to magic */
	buf = PARENT_MAGIC;

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

	/* read child buf */
	read_val = ptrace(PTRACE_PEEKDATA, cpid, &buf, NULL);
	if ((read_val == -1) && errno) {
		perror("ptrace(PTRACE_PEEKDATA)");
		goto cont;
	}

	/* parent buf check */
	if (buf != PARENT_MAGIC) {
		printf("why parent process buf rewrite ??? (0x%lx)\n", buf);
		goto cont;
	}

	/* check read value */
	if (read_val == CHILD_MAGIC) {
		printf("PTRACE_PEEKDATA read_val(0x%lx) == 0x%lx\n", read_val, CHILD_MAGIC);
	} else {
		printf("PTRACE_PEEKDATA read_val(0x%lx) != 0x%lx\n", read_val, CHILD_MAGIC);
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
