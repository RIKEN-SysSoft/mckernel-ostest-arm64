/* 008.c COPYRIGHT FUJITSU LIMITED 2016 */
#include <sys/mman.h>
#include "test_mck.h"
#include "testsuite.h"

static char* addr = (void*)-1;
static size_t hugepagesize;
static size_t length;
static const int prot = PROT_READ | PROT_WRITE;
static const int flag = MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB;
static const int fd = -1;
static const off_t offset = 0;

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
    char* start = NULL;
    char* end = NULL;
    int large_page_flag = -1;
    FILE *fp;

    fp = popen("grep Hugepagesize /proc/meminfo", "r");
    tp_assert(fp, "opening /proc/meminfo failed.");
    fscanf(fp, "Hugepagesize:       %ld kB", &hugepagesize);
    hugepagesize *= 1024;
    fclose(fp);
    length = hugepagesize * 2;

    /* mmap */
    addr = mmap(NULL, length, prot, flag, fd, offset);
    tp_assert(addr != MAP_FAILED, "mmap error.");
    printf("mmap=%p\n", addr);
		
    /* access */
    start = (void*)align_up((unsigned long)addr, hugepagesize);
    end = (void*)((unsigned long)start + length / 2);
    tp_assert(start < end, "address overflow.");

    while (start < end) {
        unsigned long tte;
	unsigned long phys;
	/* store */
        *start = 'Z';

#if 0 /* for fx10 */
	/* check tte */
	tte = mck_va2pte((unsigned long)start);
	tp_assert(mck_pte_is_valid(tte) != 0, "tte valid.");
	/* check page size */
	phys = tte & MCK_PAGE_PADDR;
	if (is_large_page_alien(start) && is_large_page_alien(phys)) {
		if (large_page_flag == -1) {
			unsigned long pgsz = mck_pte_get_page_size(tte);
			if (pgsz == LARGE_PAGE_SIZE) {
				large_page_flag = 1;
			}
			else {
				large_page_flag = 0;
			}
		}
	}
#else /* for fx100 */
	UNUSED_VARIABLE(tte);
	UNUSED_VARIABLE(phys);
        large_page_flag = check_page_size((unsigned long)start, hugepagesize);
        if (!large_page_flag) {
            break;
        }
#endif
	/* next */
        start += PAGE_SIZE;
    }
    // 注意：物理メモリのフラグメントが酷い場合にはノーマルページになるが、その場合は正常。(ページサイズは自動的に決まる)
    tp_assert(large_page_flag != 0, "invalid page size?");
    return NULL;
}

TEARDOWN_FUNC(TEST_SUITE, TEST_NUMBER)
{
    if (addr != (void*)-1) {
        munmap(addr, length);
    }
}
