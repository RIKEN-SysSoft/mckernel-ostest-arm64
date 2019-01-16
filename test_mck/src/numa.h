/* numa.h COPYRIGHT FUJITSU LIMITED 2017 */
#ifndef __NUMA_H_
#define __NUMA_H_

struct mbind_args {
	int mode;
	unsigned long node_mask;
	unsigned int npoff;		// npages offset
	unsigned long len;
	void *addr;
};

struct mbind_expect {
	int mode;
	unsigned long node_mask;
};

extern unsigned long arg_analyze(int, char**);
extern void usage_print(void);
extern void* create_node_mask(unsigned long, unsigned long*);
extern void destroy_node_mask(unsigned long*);
extern int node_mask_check(unsigned long*, unsigned long, unsigned long);
extern int mbind_check(const struct mbind_expect*, unsigned long*,unsigned long, char*, unsigned long);

#define BITS_PER_BYTE 8
#define BITS_PER_LONG (sizeof(long) * BITS_PER_BYTE)
#define MPOL_F_STATIC_NODES (1 << 15)
#define MPOL_F_RELATIVE_NODES (1 << 14)
#define NUM_REGION 7

#endif /*__NUMA_H_*/
