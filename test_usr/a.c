/* a.c COPYRIGHT FUJITSU LIMITED 2015 */
#include <stdio.h>

#define isb() asm volatile("isb" : : : "memory")

static unsigned long
arch_counter_get_cntvct(void)
{
	unsigned long cval = 0;

	isb();
	asm volatile("mrs %0, cntvct_el0" : "=r" (cval));

	return cval;
}

/*
static unsigned long
arch_counter_get_cntpct(void)
{
	unsigned long cval = 0;

	isb();
	asm volatile("mrs %0, cntpct_el0" : "=r" (cval));

	return cval;
}
*/

static unsigned int
arch_timer_get_cntfrq(void)
{
	unsigned int val = 0;
	asm volatile("mrs %0, cntfrq_el0" : "=r" (val));
	return val;
}

int
main(int argc, char** argv)
{
	printf("cntvct=%#lx\n", arch_counter_get_cntvct());
	//printf("cntpct=%#lx\n", arch_counter_get_cntpct());
	printf("cntfrq=%#x\n", arch_timer_get_cntfrq());
	return 0;
}
