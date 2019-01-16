/* common.c COPYRIGHT FUJITSU LIMITED 2017 */
#include "common.h"

/* config types */
const char *config_str[] = {
/* 0x0000 */	"SW_INCR",
/* 0x0001 */	"L1I_CACHE_REFILL",
/* 0x0002 */	"L1I_TLB_REFILL",
/* 0x0003 */	"L1D_CACHE_REFILL",
/* 0x0004 */	"L1D_CACHE",
/* 0x0005 */	"L1D_TLB_REFILL",
/* 0x0006 */	"LD_RETIRED",
/* 0x0007 */	"ST_RETIRED",
/* 0x0008 */	"INST_RETIRED",
/* 0x0009 */	"EXC_TAKEN",
/* 0x000A */	"EXC_RETURN",
/* 0x000B */	"CID_WRITE_RETIRED",
/* 0x000C */	"PC_WRITE_RETIRED",
/* 0x000D */	"BR_IMMED_RETIRED",
/* 0x000E */	"BR_RETURN_RETIRED",
/* 0x000F */	"UNALIGNED_LDST_RETIRED",
/* 0x0010 */	"BR_MIS_PRED",
/* 0x0011 */	"CPU_CYCLES",
/* 0x0012 */	"BR_PRED",
/* 0x0013 */	"MEM_ACCESS",
/* 0x0014 */	"L1I_CACHE",
/* 0x0015 */	"L1D_CACHE_WB",
/* 0x0016 */	"L2D_CACHE",
/* 0x0017 */	"L2D_CACHE_REFILL",
/* 0x0018 */	"L2D_CACHE_WB",
/* 0x0019 */	"BUS_ACCESS",
/* 0x001A */	"MEMORY_ERROR",
/* 0x001B */	"INST_SPEC",
/* 0x001C */	"TTBR_WRITE_RETIRED",
/* 0x001D */	"BUS_CYCLES",
/* 0x001E */	"CHAIN",
/* 0x001F */	"L1D_CACHE_ALLOCATE",
/* 0x0020 */	"L2D_CACHE_ALLOCATE",
/* 0x0021 */	"BR_RETIRED",
/* 0x0022 */	"BR_MIS_PRED_RETIRED",
/* 0x0023 */	"STALL_FRONTEND",
/* 0x0024 */	"STALL_BACKEND",
/* 0x0025 */	"L1D_TLB",
/* 0x0026 */	"L1I_TLB",
/* 0x0027 */	"L2I_CACHE",
/* 0x0028 */	"L2I_CACHE_REFILL",
/* 0x0029 */	"L3D_CACHE_ALLOCATE",
/* 0x002A */	"L3D_CACHE_REFILL",
/* 0x002B */	"L3D_CACHE",
/* 0x002C */	"L3D_CACHE_WB",
/* 0x002D */	"L2D_TLB_REFILL",
/* 0x002E */	"L2I_TLB_REFILL",
/* 0x002F */	"L2D_TLB",
/* 0x0030 */	"L2I_TLB",
/* 0x0031 */	"REMOTE_ACCESS",
/* 0x0032 */	"LL_CACHE",
/* 0x0033 */	"LL_CACHE_MISS",
/* 0x0034 */	"DTLB_WALK",
/* 0x0035 */	"ITLB_WALK",
/* 0x0036 */	"LL_CACHE_RD",
/* 0x0037 */	"LL_CACHE_MISS_RD",
/* 0x0038 */	"REMOTE_ACCESS_RD",
};

int get_config_num(void)
{
	return (sizeof(config_str) / sizeof(config_str[0]));
}

void print_perf_config(unsigned long config)
{
	if (config < (sizeof(config_str) / sizeof(config_str[0]))) {
		printf("  event#0x%02lx (%s)\n", config, config_str[config]);
	}
}

void print_test_overview(int tp_num)
{
	printf("# %2d : %s\n", tp_num, usage_messages[tp_num]);
}

int perfevent_exec(int os_num, struct ihk_perf_event_attr *attr, const unsigned int n)
{
	int ret = -1, i = 0, get_count = 0;
	unsigned long *countval = NULL;

	/* setting perf event */
	ret = ihk_os_setperfevent(os_num, attr, n);
	if (ret < 0) {
		perror("  ihk_os_setperfevent");
		goto err;
	}
	printf("  set event %d / %d registered.\n", ret, n);

	if (ret == 0) {
		printf("  no registered perfevent.\n");
		goto err;
	}
	get_count = ret;

	/* enable perf event */
	ret = ihk_os_perfctl(os_num, PERF_EVENT_ENABLE);
	if (ret < 0) {
		perror("  ihk_os_perfctl(PERF_EVENT_ENABLE)");
		goto err;
	}

	/* get register value area allocate */
	countval = calloc(sizeof(unsigned long), get_count);
	if (countval == NULL) {
		printf("  memory allocate failed. (countval)\n");
		goto err;
	}

	/* get perf counter value */
	ret = ihk_os_getperfevent(os_num, countval, get_count);
	if (ret < 0) {
		perror("  ihk_os_getperfevent");
		goto err;
	}

	for (i = 0; i < get_count; i++) {
		printf("  countval[%d] = 0x%lx\n", i, countval[i]);
	}

	/* disable perf event */
	ret = ihk_os_perfctl(os_num, PERF_EVENT_DISABLE);
	if (ret < 0) {
		perror("  ihk_os_perfctl(PERF_EVENT_DISABLE)");
		goto err;
	}

	/* destroy perf event */
	ret = ihk_os_perfctl(os_num, PERF_EVENT_DESTROY);
	if (ret < 0) {
		perror("  ihk_os_perfctl(PERF_EVENT_DESTROY)");
		goto err;
	}

	/* success */
	ret = 0;

err:
	if (countval) {
		free(countval);
	}
	return ret;
}

