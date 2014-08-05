
#ifndef LN_TEST_H
#define LN_TEST_H

#include <linux/sched.h>
#include <linux/seqlock.h>

extern int write_fraction;

// Indirection on test operations for setup, run and teardown
typedef struct ln_test_ops
{
	// set up the test; returns pointer to locks (array), or equivalent
	void  (*setup)(unsigned);
	/* The void * pointer passed to locking and unlock
	   operations (corresponding to the void * argument) are for
	   passing arbitrary data to and from the lock/unlock operations.
	   This was added specially for the MCS locks,
	   and in most instances where an explicit exchange of data is not
	   necessary, it is quite in order to set the pointer to NULL in lock
	   and pass NULL back to the unlock function.

	   External consumers of this interface should not do any operation
	   on the pointer other than store it and pass it back for unlock.
	   This is because they cannot possibly know what the pointer
	   actually is, as it is related to the internals of the locktest
	   specification which is hidden. 
	   
	   The data itself are set by the threadsetup and threadteardown
	   methods, allowing for per-thread state. */
	// set up an individual thread
	void *(*threadsetup)(unsigned);
	// lock the data structure with lock number passed in (reading)
	void  (*rlock)(unsigned, void *);
	// unlock the data structure with lock number passed in (reading)
	void  (*runlock)(unsigned, void *);
	// lock the data structure with lock number passed in (writing)
	void  (*wlock)(unsigned, void *);
	// unlock the data structure with lock number passed in (writing)
	void  (*wunlock)(unsigned, void *);
	// teardown an individual thread
	void (*threadteardown)(void *);
	// teardown afterwards
	void (*teardown)(unsigned);
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
	char           *shortname;
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
