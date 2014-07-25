
#ifndef LN_TEST_H
#define LN_TEST_H

#include <linux/sched.h>
#include <linux/seqlock.h>

// Indirection on test operations for setup, run and teardown
typedef struct ln_test_ops
{
	// set up the test; returns pointer to locks (array), or equivalent
	void  (*setup)(unsigned);
	// lock the data structure with lock number passed in
	void  (*lock)(unsigned);
	// unlock the data structure with lock number passed in
	void  (*unlock)(unsigned);
	// teardown afterwards
	void (*teardown)(void);
} ln_test_ops_t;

typedef struct ln_test_stats
{
	int reads;
	int writes;
	// data which are simply not present in the hash table
	int verify_miss;
	// data which are present, but with the wrong value
	int verify_corrupt;
	u64 starttime;
	u64 endtime;
} ln_test_stats_t;

typedef struct ln_test
{
	char           *name;
	// The bounded number of threads this locking scheme should be tested with
	unsigned        min_threads;
	unsigned        max_threads;
	ln_test_ops_t   ops;
	ln_test_stats_t stats;
} ln_test_t;

/********************************************************/
// register a test
int ln_test_register(ln_test_t *);

#endif /* LN_TEST_H */
