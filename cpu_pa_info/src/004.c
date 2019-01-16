/* 004.c COPYRIGHT FUJITSU LIMITED 2017 */
/* set/enable/get/disable/destroy check exclude flags on. */
#include "common.h"

TEST_FUNC(TEST_NUMBER, os_num)
{
	struct ihk_perf_event_attr attr[] = {
		{ .exclude_user = 1, .exclude_kernel = 0, .exclude_hv = 0, .exclude_idle = 0, },
		{ .exclude_user = 0, .exclude_kernel = 1, .exclude_hv = 0, .exclude_idle = 0, },
		{ .exclude_user = 1, .exclude_kernel = 1, .exclude_hv = 0, .exclude_idle = 0, },
		{ .exclude_user = 0, .exclude_kernel = 0, .exclude_hv = 1, .exclude_idle = 0, },
		{ .exclude_user = 1, .exclude_kernel = 0, .exclude_hv = 1, .exclude_idle = 0, },
		{ .exclude_user = 0, .exclude_kernel = 1, .exclude_hv = 1, .exclude_idle = 0, },
		{ .exclude_user = 1, .exclude_kernel = 1, .exclude_hv = 1, .exclude_idle = 0, },
		{ .exclude_user = 0, .exclude_kernel = 0, .exclude_hv = 0, .exclude_idle = 1, },
		{ .exclude_user = 1, .exclude_kernel = 0, .exclude_hv = 0, .exclude_idle = 1, },
		{ .exclude_user = 0, .exclude_kernel = 1, .exclude_hv = 0, .exclude_idle = 1, },
		{ .exclude_user = 1, .exclude_kernel = 1, .exclude_hv = 0, .exclude_idle = 1, },
		{ .exclude_user = 0, .exclude_kernel = 0, .exclude_hv = 1, .exclude_idle = 1, },
		{ .exclude_user = 1, .exclude_kernel = 0, .exclude_hv = 1, .exclude_idle = 1, },
		{ .exclude_user = 0, .exclude_kernel = 1, .exclude_hv = 1, .exclude_idle = 1, },
		{ .exclude_user = 1, .exclude_kernel = 1, .exclude_hv = 1, .exclude_idle = 1, },
	};

	unsigned long config[] = { ARMV8_PMUV3_PERFCTR_CPU_CYCLES, ARMV8_PMUV3_PERFCTR_INST_RETIRED };
	int i = 0, j = 0;
	int ret = 0;

	print_test_overview(tp_num);

	for (i = 0; i < sizeof(attr) / sizeof(attr[0]); i++) {
		for (j = 0; j < sizeof(config) / sizeof(config[0]); j++) {
			attr[i].config = config[j];

			printf("##### start event#0x%02lx #####\n", attr[i].config);
			printf("##### exclude_user:%d, exclude_kernel:%d, exclude_hv:%d, exclude_idle:%d #####\n",
				attr[i].exclude_user, attr[i].exclude_kernel, attr[i].exclude_hv, attr[i].exclude_idle);
			print_perf_config(attr[i].config);
			ret = perfevent_exec(os_num, &attr[i], 1);
			if (ret) {
				printf("  failed  to event#0x%lx\n", attr[i].config);
			} else {
				printf("  success to event#0x%lx\n", attr[i].config);
			}
			printf("##### end              #####\n\n");

			if (ret) {
				goto out;
			}
		}
	}
out:
	return ret;
}
