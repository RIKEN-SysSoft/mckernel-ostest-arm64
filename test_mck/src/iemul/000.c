/* 000.c COPYRIGHT FUJITSU LIMITED 2017 */
#include "test_mck.h"
#include "testsuite.h"
#include <asm/hwcap.h>
#include <sys/auxv.h>

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	unsigned long hwcap = getauxval(AT_HWCAP);
	tp_assert(hwcap & HWCAP_CPUID, "CPUID registers unavailable");
	return print_cpu_ftr(tc_num);
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
