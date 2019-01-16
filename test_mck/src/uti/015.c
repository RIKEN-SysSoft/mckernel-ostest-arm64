/* 015.c COPYRIGHT FUJITSU LIMITED 2018 */
/* after migrate, send system-calls (mmap,mprotect,munmap,brk,futex,getpid,gettid) */
#include <semaphore.h>
#include <sys/types.h> 
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <linux/futex.h>
#include <sys/time.h>
#include "test_mck.h"
#include "testsuite.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)
TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)

static int futex_flag_1 = 1;
static int futex_flag_2 = 1;

static void *child_thread(void *arg)
{
	int ret = 0;

	ret = futex(&futex_flag_1, FUTEX_WAIT, 1, NULL, NULL, 0);
	if (ret != 0) {
		printf("  [NG] child futex wait failed.\n");
	}

	usleep(100000);

	ret = futex(&futex_flag_2, FUTEX_WAKE, 1, NULL, NULL, 0);
	if (ret != 1) {
		printf("  [NG] child futex wake failed.\n");
	}
	return NULL;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int ret = 0;
	uti_attr_t attr;
	pid_t lpid, ltid, rpid, rtid;
	char *buf = NULL;
	const size_t len = PAGE_SIZE;
	pthread_t thread;

	lpid = getpid();
	ltid = gettid();

	memset(&thread, 0, sizeof(thread));
	ret = pthread_create(&thread, NULL, child_thread, NULL);
	tp_assert(ret == 0, "pthread_create error.");

	memset(&attr, 0, sizeof(attr));
	attr.flags |= UTI_FLAG_SAME_NUMA_DOMAIN;
	ret = util_migrate_inter_kernel(&attr);
	if (ret != 0) {
		perror("[NG] util_migrate_inter_kernel:");
		goto join_out;
	}

	ret = get_system();
	if (ret == 0) {
		printf("[NG] this process running on McKernel.\n");
		goto join_out;
	} else if (ret != -1) {
		printf("[NG] get_system illegal return value(%d)\n", ret);
		goto join_out;
	}

	/* check getpid / gettid */
	printf("[send getpid / gettid]\n");
	rpid = getpid();
	rtid = gettid();

	if (lpid != rpid) {
		printf("  [NG] pid compare failed. (lpid=%d, rpid=%d)\n", lpid, rpid);
	} else {
		printf("  [OK] getpid\n");
	}

	if (ltid != rtid) {
		printf("  [NG] tid compare failed. (ltid=%d, rtid=%d)\n", ltid, rtid);
	} else {
		printf("  [OK] gettid\n");
	}

	/* check mmap / mprotect / munmap */
	printf("[send mmap / mprotect / munmap]\n");

	buf = (char *)mmap(NULL, len, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (buf == NULL) {
		printf("  [NG] mmap failed.(%d)\n", errno);
		goto check_brk;
	}
	printf("  [OK] mmap\n");

	if (mprotect(buf, len, PROT_READ | PROT_WRITE)) {
		printf("  [NG] mprotect failed.(%d)\n", errno);
	} else {
		int i = 0;

		for (i = 0; i < len; i++) {
			buf[i] = -1;
		}

		if ((buf[0] == (char)-1) && (buf[len - 1] == (char)-1)) {
			printf("  [OK] mprotect\n");
		} else {
			printf("  [NG] mprotect, value write failed.\n");
		}
	}

	if (munmap(buf, len)) {
		printf("  [NG] munmap failed.(%d)\n", errno);
	} else {
		printf("  [OK] munmap\n");
	}
	buf = NULL;

check_brk:
	/* check malloc */
	printf("[send brk(sbrk)]\n");

	buf = sbrk(0);
	sbrk(PAGE_SIZE);
	if (buf == (void *)-1) {
		printf("  [NG] brk(sbrk) failed.\n");
	} else {
		int i = 0;

		for (i = 0; i < len; i++) {
			buf[i] = -1;
		}

		if ((buf[0] == (char)-1) && (buf[len - 1] == (char)-1)) {
			printf("  [OK] brk(sbrk)\n");
		} else {
			printf("  [NG] brk(sbrk), value write failed.\n");
		}
	}

	/* check futex */
	printf("[send futex]\n");

	ret = futex(&futex_flag_1, FUTEX_WAKE, 1, NULL, NULL, 0);
	if (ret == -1 || ret != 1) {
		printf("  [NG] parent futex wake failed.\n");
	}

	ret = futex(&futex_flag_2, FUTEX_WAIT, 1, NULL, NULL, 0);
	if (ret == -1) {
		printf("  [NG] parent futex wait failed.\n");
	} else {
		printf("  [OK] futex\n");
	}

join_out:
	ret = pthread_join(thread, NULL);
	tp_assert(ret == 0, "pthread_join failed.");
	tp_assert(0, "please check logs. [OK] or [NG]");
	return NULL;
}
