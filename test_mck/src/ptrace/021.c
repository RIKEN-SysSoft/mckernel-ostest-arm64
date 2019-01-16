/* 021.c COPYRIGHT FUJITSU LIMITED 2016 */
/* ptrace(PTRACE_POKEDATA) testcase */
#include "test_mck.h"
#include "arch_test_ptrace.h"
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>

static char addr[32];
static const size_t addr_size = sizeof(addr)/sizeof(addr[0]);
static unsigned long inst_addr = 0;

static const char* child_func(int writefd)
{
	int devfd;
	int state;
	long lstate;
	unsigned long* devmap;

	/* open */
	devfd = open("/dev/test_mck/mmap_dev2", O_RDWR);
	tp_assert(devfd != -1, "open mmap_dev2 error.");

	/* ioctl */
	state = ioctl(devfd, TEST_MCK_MMAP_DEV2_BUF_RESET);
	tp_assert(state != -1, "ioctl error.");

	/* mmap */
	devmap = mmap((caddr_t)0, PAGE_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_POPULATE, devfd, 0);
	tp_assert(devmap != MAP_FAILED, "mmap error");
	memset(devmap, 0, PAGE_SIZE);

	/* write */
	state = snprintf(addr, addr_size, "0x%lx", (unsigned long)devmap);
	tp_assert(0 <= state, "snprintf error");
	addr[addr_size-1] = '\0';
	state = write(writefd, addr, addr_size);
	tp_assert(state != -1, "write error");

	/* send PTRACE_TRACEME */
	lstate = ptrace(PTRACE_TRACEME, 0, NULL, NULL);
	tp_assert(lstate != -1L, "ptrace(PTRACE_TRACEME)");

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

	/* check buf (write by parent) */
	tp_assert(*devmap != 0UL, "PTRACE_POKEDATA failed");

	/* cleanup */
	munmap(devmap, PAGE_SIZE);
	close(devfd);

	/* success */
	return NULL;
}

static const char* parent_func(pid_t cpid, int readfd)
{
	const char* msg = NULL;
	pid_t pid = 0;
	int state = 0;
	long lstate = 0;

	unsigned long devmap;
	char *endptr;

	/* read */
	state = read(readfd, addr, addr_size);
	if (state == -1) {
		msg =  "read error";
		goto out;
	}
	devmap = strtoul(addr, &endptr, 0);

	/* wait child stop */
	pid = wait(&state);
	if (pid != cpid) {
		msg = "wait error";
		goto cont;
	}
	if (!WIFSTOPPED(state)) {
		msg = "child is not stopped.";
		goto cont;
	}

	/* rewrite child brk instruction */
	if (rewrite_brk_inst(cpid, &inst_addr)) {
		goto cont;
	}

	/* write child buf */
	lstate = ptrace(PTRACE_POKEDATA, cpid, (void*)devmap, 0xFFFFFFFF);
	if (lstate == -1L) {
		msg = "ptrace(PTRACE_POKEDATA)";
		goto cont;
	}
cont:
	/* child continue */
	lstate = ptrace(PTRACE_CONT, cpid, NULL, NULL);
	if (lstate == -1L) {
		msg = "ptrace(PTRACE_CONT)";
	}
out:
	return msg;
}

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	pid_t pid = 0;
	const char* msg = NULL;

	int state;

	int pipefd[2];

	/* pipe */
	state = pipe(pipefd);
	tp_assert(state == 0, "pipe error.");

	/* create child process */
	pid = fork();
	tp_assert(pid != -1, "fork error");

	switch (pid) {
	case 0:
		/* child process */
		close(pipefd[0]);
		msg = child_func(pipefd[1]);
		close(pipefd[1]);
		if (msg != NULL) {
			printf("%s\n", msg);
			exit(-1);
		}
		exit(0);
	default:
		/* parent process */
		close(pipefd[1]);
		msg = parent_func(pid, pipefd[0]);
		close(pipefd[0]);
		tp_assert(msg == NULL, msg);

		/* wait child */
		pid = wait(&state);
		tp_assert(pid != -1, "wait error");
		tp_assert(WIFEXITED(state), "WIFEXITED error.");
		tp_assert(WEXITSTATUS(state) == 0, "WEXITSTATUS error");
		break;
	}

	/* sccess. */
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
