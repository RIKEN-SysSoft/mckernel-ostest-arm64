/* 005.c COPYRIGHT FUJITSU LIMITED 2016 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include "test_mck.h"
#include "testsuite.h"

static int shmid = -1;
static char *shmaddr = NULL;


SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	unsigned long hugepagesize;
	unsigned long length;
	unsigned long pgsize_log = 0;
	int large_page_flag;
	int i;
	FILE *fp;
  
	fp = popen("grep Hugepagesize /proc/meminfo", "r");
	tp_assert(fp, "opening /proc/meminfo failed.");
	fscanf(fp, "Hugepagesize:       %ld kB", &hugepagesize);
	hugepagesize *= 1024;
	fclose(fp);
	length = hugepagesize * 2;

	shmid = shmget(IPC_PRIVATE, length, pgsize_log | SHM_HUGETLB | IPC_CREAT | SHM_R | SHM_W);
	tp_assert(0 <= shmid, "shmget error.");
	printf("shmid: 0x%x\n", shmid);

	shmaddr = shmat(shmid, ADDR, SHMAT_FLAGS);
	tp_assert(shmaddr != (char*)-1, "shmgat error.");
	printf("shmaddr: %p\n", shmaddr);

	for (i = 0; i < length; i += PAGE_SIZE) {
		shmaddr[i] = (char)(i);
	}

        large_page_flag = check_page_size((unsigned long)shmaddr, hugepagesize);
	// 注意：物理メモリのフラグメントが酷い場合にはノーマルページになるが、その場合は正常。(ページサイズは自動的に決まる)
	tp_assert(large_page_flag != 0, "invalid page size?");
	return NULL;
}

TEARDOWN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	if (shmaddr != NULL) {
		shmdt(shmaddr);
	}
	if (0 <= shmid) {
		shmctl(shmid, IPC_RMID, NULL);
	}
}
