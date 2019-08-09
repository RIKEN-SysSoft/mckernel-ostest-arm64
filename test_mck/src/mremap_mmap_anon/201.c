/* 201.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"
#include <unistd.h>

SETUP_ALIAS(TEST_SUITE, TEST_NUMBER, 0)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct mremap_mmap_anon_param params[] = {
		{.size = MREMAP_RANGE_LARGE_SIZE, .nopage = 0, .prot = PROT_READ|PROT_WRITE}, // old > new
		{.size = MREMAP_RANGE_LARGE_SIZE, .nopage = 0, .prot = PROT_NONE},            // use   use
	};
	size_t pcount = sizeof(params)/sizeof(params[0]);
	long page_size = sysconf(_SC_PAGESIZE);
	size_t old_size;
	size_t new_size;
	char* remap;

	old_size = (size_t)(page_size * -2);
	new_size = old_size;

	{//init
		size_t maps;
		maps = mremap_mmap_anon(params, pcount);
		tp_assert(maps == pcount, "mmap error.");
	}

	{//remap
		char* addr = params[0].addr;
		errno = 0;
		remap = mremap(addr, old_size, new_size, 0);
#if defined(__sparc)
		tp_assert(remap == MAP_FAILED, "mremap error.");
		tp_assert(errno == EINVAL, "invalid errno.");
		return NULL;
#endif /*defined(__sparc)*/
		tp_assert(remap != MAP_FAILED, "mremap error.");
		tp_assert(errno == 0, "invalid errno.");
	}

	{//verify
		const char* msg;
		msg = mremap_mmap_anon_data_check(remap, params[0].size, '0');
		tp_assert(msg == NULL, msg);
	}

	{//access
		const char* msg;
		msg = mremap_mmap_anon_access_check(remap, params[0].size);
		tp_assert(msg == NULL, msg);
	}
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
