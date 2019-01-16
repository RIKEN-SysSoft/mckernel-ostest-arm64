/* testsuite.h COPYRIGHT FUJITSU LIMITED 2015-2016 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__

#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <errno.h>

struct waitid_args {
	int proc_num;
};

#endif /*__TEST_SUITE_H__*/
