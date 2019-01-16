/* 001.c COPYRIGHT FUJITSU LIMITED 2018 */
/* event check IHK_OS_EVENTFD_TYPE_STATUS. (LWK state panic or hung-up) */
#include "common.h"

TEST_FUNC(TEST_NUMBER, os_num)
{
	print_test_overview(tp_num);
	return event_polling(os_num, IHK_OS_EVENTFD_TYPE_STATUS);
}
