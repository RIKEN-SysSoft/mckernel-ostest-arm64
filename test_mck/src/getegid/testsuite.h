/* testsuite.h COPYRIGHT FUJITSU LIMITED 2015 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__
#include <sys/types.h> 
#include <unistd.h>
#include <errno.h>

/* getegid.c */
struct getegid_args {
	gid_t egid;
	char **argv;
	int argc;
};

void *getegid_parse_args(int argc, char **argv);

#endif /*__TEST_SUITE_H__*/
