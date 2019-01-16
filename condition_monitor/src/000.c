/* 000.c COPYRIGHT FUJITSU LIMITED 2018 */
/* event check IHK_OS_EVENTFD_TYPE_OOM. (physical memory used exceeds the limit) */
#include "common.h"

TEST_FUNC(TEST_NUMBER, os_num)
{
	print_test_overview(tp_num);
	return event_polling(os_num, IHK_OS_EVENTFD_TYPE_OOM);
}
