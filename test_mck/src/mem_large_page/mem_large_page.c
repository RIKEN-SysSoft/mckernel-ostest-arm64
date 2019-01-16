/* mem_large_page.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "test_mck.h"
#include "testsuite.h"

int check_page_size(unsigned long va, unsigned long pagesize)
{
	struct memory_info info;
	int stat;

	stat = get_memory_info_self(va, &info);
	if (stat != 0) {
		printf("get memory info failed.\n");
		return 0;
	}
	if (info.pgsize != pagesize) {
		printf("pagesize = 0x%lx, Not as expected.\n", info.pgsize);
		return 0;
	}
	return 1;
}
