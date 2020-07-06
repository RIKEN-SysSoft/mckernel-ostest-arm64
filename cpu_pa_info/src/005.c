/* 005.c COPYRIGHT FUJITSU LIMITED 2018 */
/* set/enable/get/disable/destroy check config(0x00-0x07). (8 counter at once) */
#include "common.h"

TEST_FUNC(TEST_NUMBER, os_num)
{
	const int count = 6/*8*/; /* ThunderX2 has 6 counters */
	struct ihk_perf_event_attr attr[count];
	int i = 0;
	int ret = 0;

	print_test_overview(tp_num);

	memset(&attr, 0, sizeof(attr));
	for (i = 0; i < count; i++) {
		attr[i].config = i;
	}

	printf("##### start event#0x%02x-0x%02x #####\n", 0, count - 1);
	ret = perfevent_exec(os_num, attr, count);
	if (ret) {
		printf("  failed.\n");
	} else {
		printf("  success.\n");
	}
	printf("##### end                   #####\n\n");

	return ret;
}
