/* 002.c COPYRIGHT FUJITSU LIMITED 2017 */
/* set/enable/get/disable/destroy check disabled flags on. */
#include "common.h"

TEST_FUNC(TEST_NUMBER, os_num)
{
	struct ihk_perf_event_attr attr[] = {
		{
			.config = ARMV8_PMUV3_PERFCTR_CPU_CYCLES,
			.disabled = 1,
		},
		{
			.config = ARMV8_PMUV3_PERFCTR_INST_RETIRED,
			.disabled = 1,
		},
	};

	int i = 0;
	int ret = 0;

	print_test_overview(tp_num);

	for (i = 0; i < sizeof(attr) / sizeof(attr[0]); i++) {
		printf("##### start event#0x%02lx #####\n", attr[i].config);
		print_perf_config(attr[i].config);
		ret = perfevent_exec(os_num, &attr[i], 1);
		if (ret) {
			printf("  failed  to event#0x%lx\n", attr[i].config);
		} else {
			printf("  success to event#0x%lx\n", attr[i].config);
		}
		printf("##### end              #####\n\n");

		if (ret) {
			break;
		}
	}
	return ret;
}
