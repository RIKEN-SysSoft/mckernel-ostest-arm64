/* 000.c COPYRIGHT FUJITSU LIMITED 2017 */
/* set/enable/get/disable/destroy check config(0x00-0x3f). (for each) */
#include "common.h"

TEST_FUNC(TEST_NUMBER, os_num)
{
	struct ihk_perf_event_attr attr;
	int i = 0;
	int ret = 0;
	int count = 0;

	print_test_overview(tp_num);

	count = get_config_num();
	for (i = 0; i < count; i++) {
		memset(&attr, 0, sizeof(attr));
		attr.config = i;

		printf("##### start event#0x%02x #####\n", i);
		print_perf_config(attr.config);
		ret = perfevent_exec(os_num, &attr, 1);
		if (ret) {
			printf("  failed  to event#0x%x\n", i);
		} else {
			printf("  success to event#0x%x\n", i);
		}
		printf("##### end              #####\n\n");

		if (ret) {
			break;
		}
	}
	return ret;
}
