/* testsuite.h COPYRIGHT FUJITSU LIMITED 2017-2018 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__

#define __NR_profile			701

#define PROF_JOB			0x40000000
#define PROF_PROC			0x80000000
#define PROF_CLEAR			0x01
#define PROF_ON				0x02
#define PROF_OFF			0x04
#define PROF_PRINT			0x08

int profile_proc_clear(void);
int profile_proc_on(void);
int profile_proc_off(void);
int profile_proc_print(void);

#endif /*__TEST_SUITE_H__*/
