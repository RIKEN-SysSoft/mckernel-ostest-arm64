/* 002.c COPYRIGHT FUJITSU LIMITED 2018 */
/* LWK state check. */
#include "common.h"

char *status_strings[] = {
	"INACTIVE",
	"BOOTING",
	"RUNNING",
	"SHUTDOWN",
	"PANIC",
	"HUNGUP",
	"FREEZING",
	"FROZEN",
};

TEST_FUNC(TEST_NUMBER, os_num)
{
	int ret = -1;
	int status = 0;

	print_test_overview(tp_num);

	status = get_status(os_num);
	if (status < 0) {
		goto out;
	}

	if (status < (sizeof(status_strings) / sizeof(status_strings[0]))) {
		printf("status:%s detected.\n", status_strings[status]);
	}
	else {
		printf("unknown status (%d)\n", status);
		goto out;
	}

	/* success */
	ret = 0;
out:
	return ret;
}
