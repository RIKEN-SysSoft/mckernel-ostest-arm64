/* 007.c COPYRIGHT FUJITSU LIMITED 2015-2018 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include "test_mck.h"
#include "testsuite.h"

#define PROCFILE_LEN 256
#define BUF_LEN PAGE_SIZE
#define DUMMY_MMAP_COUNT 512

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)
TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	FILE *fp = NULL;
	char pfname[PROCFILE_LEN] = "";
	char buf[BUF_LEN];
	pid_t pid = getpid();
	char *map[DUMMY_MMAP_COUNT];
	int i = 0;

	/* pfname set */
	snprintf(pfname, sizeof(pfname), "/proc/%d/maps", pid);

	/* fopen */
	if ((fp = fopen(pfname, "r")) == NULL) {
		printf("fopen() failed. %d\n", errno);
		goto err;
	}

	/* dummy mmap */
	for (i = 0; i < DUMMY_MMAP_COUNT; i++) {
		int prot = PROT_READ | PROT_WRITE;

		if (i % 2 == 0) {
			prot |= PROT_EXEC;
		}
		map[i] = (char *)mmap(NULL, PAGE_SIZE, prot,
					MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		*map[i] = 'c';
		printf("range: %lx-%lx rw%cp\n",
		       (unsigned long)map[i],
		       (unsigned long)map[i] + PAGE_SIZE,
		       i % 2 == 0 ? 'x' : '-');
	}

	/* dump */
	printf("output: %s\n", pfname);
	while(fgets(buf, sizeof(buf), fp) != NULL) {
		printf("%s", buf);
	}

	/* dummy munmap */
	for (i = 0; i < DUMMY_MMAP_COUNT; i++) {
		munmap(map[i], PAGE_SIZE);
	}

	/* fclose */
	if (fclose(fp)) {
		printf("fclose() failed. %d\n", errno);
		goto err;
	}
	tp_assert(0, "you need check McKernel Log & Dump MAPS.");

err:
	/* tp error */
	tp_assert(0, "TP failed.");
}
