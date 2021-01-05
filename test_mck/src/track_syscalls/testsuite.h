/* testsuite.h COPYRIGHT FUJITSU LIMITED 2017-2018 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__

#include <stdint.h>

/* import from mckernel/kernel/include/profile.h begins */
#define PROFILE_SYSCALL_MAX                          2000
#define PROFILE_OFFLOAD_MAX   (PROFILE_SYSCALL_MAX << 1)
#define PROFILE_EVENT_MIN            PROFILE_OFFLOAD_MAX

#define PROF_JOB                       0x40000000
#define PROF_PROC                      0x80000000
#define PROF_CLEAR                           0x01
#define PROF_ON                              0x02
#define PROF_OFF                             0x04
#define PROF_PRINT                           0x08

struct profile_event {
	uint32_t cnt;
	uint64_t tsc;
};

/*
 * The layout of profile events is as follows:
 * [0,PROFILE_SYSCALL_MAX) - syscalls
 * [PROFILE_SYSCALL_MAX,PROFILE_OFFLOAD_MAX) - syscall offloads
 * [PROFILE_OFFLOAD_MAX,PROFILE_EVENT_MAX) - general events
 *
 * XXX: Make sure to fill in prof_event_names in profile.c
 * for each added profiled event.
 */
enum profile_event_type {
	PROFILE_tlb_invalidate = PROFILE_EVENT_MIN,
	PROFILE_page_fault,
	PROFILE_page_fault_anon_clr,
	PROFILE_page_fault_file,
	PROFILE_page_fault_dev_file,
	PROFILE_page_fault_file_clr,
	PROFILE_remote_page_fault,
	PROFILE_mpol_alloc_missed,
	PROFILE_mmap_anon_contig_phys,
	PROFILE_mmap_anon_straight,
	PROFILE_mmap_anon_not_straight,
	PROFILE_mmap_anon_no_contig_phys,
	PROFILE_mmap_regular_file,
	PROFILE_mmap_device_file,
	PROFILE_tofu_stag_alloc,
	PROFILE_tofu_stag_alloc_new_steering,
	PROFILE_tofu_stag_alloc_new_steering_alloc_mbpt,
	PROFILE_tofu_stag_alloc_new_steering_update_mbpt,
	PROFILE_tofu_stag_free_stags,
	PROFILE_tofu_stag_free_stag,
	PROFILE_tofu_stag_free_stag_pre,
	PROFILE_tofu_stag_free_stag_cqflush,
	PROFILE_tofu_stag_free_stag_dealloc,
	PROFILE_tofu_stag_free_stag_dealloc_free_pages,
	PROFILE_EVENT_MAX	/* Should be the last event type */
};

#define __NR_profile                   PROFILE_EVENT_MAX
/* import from mckernel/kernel/include/profile.h ends */

int profile_proc_clear(void);
int profile_proc_on(void);
int profile_proc_off(void);
int profile_proc_print(void);

#endif /*__TEST_SUITE_H__*/
