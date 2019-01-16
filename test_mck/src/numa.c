/* numa.c COPYRIGHT FUJITSU LIMITED 2017 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <numaif.h>

#include "numa.h"
#include "test_mck.h"

/* arg_analyze */
unsigned long arg_analyze(int tc_argc, char** tc_argv)
{
	int opt;
	char *ends;
	unsigned long max_node = 0;

	while ((opt = getopt(tc_argc, tc_argv, "n:")) != -1) {
		switch (opt) {
		case 'n':
			max_node = strtoul(optarg, &ends, 0);
			break;
		default:
			break;
		}
	}
	return max_node;
}

/* usage_print */
void usage_print(void) {
	printf("Usage: ./test_mck -s test_suite -n test_number [-h] -- -n max_node\n");
	printf("\n  -n max_node\tnumber of actually mounted nodes\n");
}

/* create node_mask */
void* create_node_mask(unsigned long max_node, unsigned long *p_nr_node_mask)
{
	unsigned long *node_mask = NULL;

	/* number of array of aligned node_mask */
	*p_nr_node_mask = (max_node + BITS_PER_LONG - 1) / BITS_PER_LONG;
	if (*p_nr_node_mask == 0) {
		*p_nr_node_mask = 1;
	}
	node_mask = (unsigned long *)malloc(*p_nr_node_mask * sizeof(unsigned long));
	if (node_mask == NULL) {
		return NULL;
	}
	memset(node_mask, 0, *p_nr_node_mask * sizeof(unsigned long));

	/* return node_mask */
	return node_mask;
}

/* free node_mask */
void destroy_node_mask(unsigned long *node_mask)
{
 	free(node_mask);
 	return;
}

/* node_mask_check */
int node_mask_check(unsigned long *node_mask, unsigned long nr_node_mask, unsigned long num_bits)
{
        int i;

        /* node_mask_check */
        for (i = nr_node_mask-1; i>=0; i--) {
                /* the last element */
                if (i == nr_node_mask-1) {
                        /* the node_mask[i] is not 1 to the (max_node-1) bit */
                        if ( node_mask[i] != ((1 << (num_bits % BITS_PER_LONG)) -1)) {
                                return -1;
                        }
                /* other than the last element */
                } else {
                        /* node_mask[i] is not all 1 */
                        if (~(node_mask[i]) != 0) {
                                return -1;
                        }
                }
        }

        /* result OK */
        return 0;
}

/* mbind check for each region */
int mbind_check(const struct mbind_expect *m_expect, unsigned long *node_mask, unsigned long max_node,
						char *ptr, unsigned long flags) {
	int i_ret, mode, i;

	/* check for each region */
	for ( i = 0; i < NUM_REGION; i++ ) {
		printf("region %d\n", i);
		i_ret = -1;
		mode = -1;
		memset(node_mask, 0, sizeof(*node_mask));
		i_ret = get_mempolicy(&mode, node_mask, max_node, ptr+(PAGE_SIZE*i), flags);
		if (i_ret == -1) {
			printf("mbind error\n");
			return -1;
		}

		printf("get      : mode = %d, node_mask = %lu\n", mode, node_mask[0]);
		printf("m_expect : mode = %d, node_mask = %lu\n\n", 
					m_expect[i].mode, m_expect[i].node_mask);

		if (mode != m_expect[i].mode) {
			printf("m_expect mode error\n");
			return -1;
		} else if (node_mask[0] != m_expect[i].node_mask) {
			printf("m_expect node_mask error\n");
			return -1;
		}
	}

	/* result OK */
	return 0;
}
