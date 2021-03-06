
#include <asm/atomic.h>
#include <linux/hashtable.h>
#include <linux/jiffies.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/wait.h>

#include <rnd.h>
#include <test.h>
#include <stat.h>
#include <thread.h>

#define HASH_TABLE_BITS		24
#define MAX_NUM_OF_TESTS	8
#define TEST_RUNTIME_SECS	30

/* arrays of tests should only be accessed from the test management thread */
static ln_test_t *tests[MAX_NUM_OF_TESTS];
static unsigned   tests_count = 0;

static int ln_test_run(ln_test_t *test, unsigned num_threads);

static DECLARE_HASHTABLE(hashtable, HASH_TABLE_BITS);

int ln_test_register(ln_test_t *test)
{
	int retval;
	if (tests_count >= MAX_NUM_OF_TESTS)
	{
		printk(KERN_ALERT "[Scaling Locks] Unable to register %s due to lack of space.\n"
			"There are currently %d tests registered, of %d total size.\n"
			"Increase the value of MAX_NUM_OF_TESTS and recompile to add more.\n",
			test->name, tests_count, MAX_NUM_OF_TESTS);
		retval = 0;
	}
	else
	{
		tests[tests_count++] = test;
		printk(KERN_ALERT "[Scaling Locks] Registered test %d: %s\n",
			tests_count, test->name);
		retval = 1;
	}
	return retval;
}

static int test_set_up(void)
{
	ln_rnd_seed(LN_RND_SIZE_DEFAULT);
	return 1;
}

int ln_test_run_all(void)
{
	/* For each test, set up a hashtable with a fixed number of buckets,
	 * and spawn threads in accordance with the test specification (up to
	 * the designated maximum) each of which hammers the hash table with 
	 * a specified fraction of READ and WRITE operations.
	 *
	 * There is scope for using alternative data structures (making this 
	 * a data structure + locking prototyping tool) but that is not included
	 * in this release, because this framework is primarily intended for
	 * gathering performance statistics for the in-kernel hash table 
	 * implementation.
	 */
	unsigned i;
	ln_stats_print_header();

	if (unlikely(!test_set_up()))
	{
		printk(KERN_ALERT "[Scaling Locks] Set-up of the test environment failed."
			"Cannot proceed.\n");
		return 0;
	}
	
	for (i = 0; i < tests_count; i++)
	{
		unsigned threads;
		int last_thread_count = 0;
		ln_test_t *test = tests[i];
		for (threads = test->min_threads; threads <= test->max_threads; threads *= 2)
		{
			last_thread_count = threads;
			ln_test_run(test, threads);
		}
		/* make sure we catch the max_threads, if it doesn't happen to be a
		   multiple of 2. Can't reuse the "threads" variable here since the last
		   invocation of the for loop will change its value, so need to track based
		   on the counter within the loop added for this purpose. */
		if (last_thread_count < test->max_threads)
			ln_test_run(test, test->max_threads);

		// TODO: collate statistics here
	}

	return 1;
}

static atomic_t active_threads = ATOMIC_INIT(0);
static DECLARE_WAIT_QUEUE_HEAD(thread_wq);

typedef enum ln_oper
{
	op_read,
	op_write,
} oper_t;

typedef struct ln_hash_entry
{
	u64 key;
	u64 val;
	struct hlist_node hash;
} ln_hash_entry_t;

static inline oper_t get_op(void)
{
	int rnd;
	get_random_bytes_arch(&rnd, sizeof(int));

	if ((rnd & 0xFF) < write_fraction)
		return op_write;
	else
		return op_read;
}

static inline void process_read(ln_thread_t *thread, void *lockdata)
{
	/* Get a random key and look it up. */
	u64 key;
	ln_hash_entry_t *e;
	u64 bucket;

	get_random_bytes_arch(&key, sizeof(u64));
	key = ln_rnd_key_mask(key);
	bucket = hash_min(key, HASH_TABLE_BITS);
	thread->ops->rlock(bucket, lockdata);
	hash_for_each_possible_rcu(hashtable, e, hash, key)
	{
		if (e->key == key)
		{
			if (unlikely(e->val != ln_rnd_key_get_val(key)))
				thread->stats.verify_corrupt++;

			thread->ops->runlock(bucket, lockdata);
			return;
		}
		else
		{
			continue;
		}
	}
	thread->ops->runlock(bucket, lockdata);

	thread->stats.verify_miss++;
	return;
}
static inline void process_write(ln_thread_t *thread, void *lockdata)
{
	u64 bucket;
	ln_hash_entry_t *e = kmalloc(sizeof(ln_hash_entry_t), GFP_KERNEL);

	if (unlikely(IS_ERR_OR_NULL(e)))
	{
		printk("[Scaling Locks] [%s] Unable to obtain memory for ln_hash_entry_t.\n",
			thread->threadname);
		return;
	}

	get_random_bytes_arch(&e->key, sizeof(u64));
	e->key = ln_rnd_key_mask(e->key);
	e->val = ln_rnd_key_get_val(e->key);

	bucket = hash_min(e->key, HASH_TABLE_BITS);
	
	thread->ops->wlock(bucket, lockdata);
	hash_add_rcu(hashtable, &e->hash, e->key);
	thread->ops->wunlock(bucket, lockdata);

	return;
}

static int test_thread(void *data)
{
	ln_thread_t *thread = (ln_thread_t *) data;
	void *lockdata = thread->ops->threadsetup(1 << HASH_TABLE_BITS);
	thread->threadname  = thread->thread->comm;

	/* The thread should run for 30 seconds, i.e. from current jiffies to jiffies
	   plus 30 seconds from now. */
	thread->stats.starttime = get_jiffies_64();

#ifdef DEBUG
	printk(KERN_ALERT "[Scaling Locks] [%s] Embarking on a %u second test.\n",
		thread->threadname, TEST_RUNTIME_SECS);
#endif

	while (!kthread_should_stop())
	{
		/* What type of operation is this?
		 * Get some randomness and decide if it's a read/write depending on how
		 * the random number turns out. */
		oper_t op = get_op();

		switch (op)
		{
			case op_read:
				process_read(thread, lockdata);
				thread->stats.reads++;
				break;
			case op_write:
				process_write(thread, lockdata);
				thread->stats.writes++;
				break;
		}

		/* We're going to occasionally decide to go to sleep, but don't want
		 * this to happen deterministically, i.e. always after a read or a write
		 * operation. I'm piggybacking on the get_op() stuff now, a second time,
		 * (i.e. decoupled from the above) and sleeping whenever the answer is
		 * op_write. I'm also assuming the write_fraction will be quite small,
		 * and hence sleeping is not the common case through this code path.
		 */
		if (unlikely(get_op() == op_write))
			schedule();

		continue;
	}

	thread->stats.endtime = get_jiffies_64();

#ifdef DEBUG
	printk(KERN_ALERT "[Scaling Locks] [%s] 30 seconds elapsed, done.\n", 
		thread->threadname);
#endif

	thread->ops->threadteardown(lockdata);

	atomic_dec(&active_threads);
	wake_up_all(&thread_wq);

	return 0;
	// TODO: add verification code here to verify the data from the hash table are
	//       still consistent after being hammered by the threads.
}

static void free_hash_table(void)
{
	int bkt;
	ln_hash_entry_t *e;
	hash_for_each_rcu(hashtable, bkt, e, hash)
	{
		if (likely(e))
			hash_del(&e->hash);
			kfree(e);
	}
	return;
}

static int ln_test_run(ln_test_t *test, unsigned num_threads)
{
	int i;
	ln_test_stats_t stats;	// place for overall stats to be collated

	ln_thread_t *threads = (ln_thread_t *) 
		kzalloc(sizeof(ln_thread_t) * num_threads, GFP_KERNEL);
	if (unlikely(ksize(threads) < sizeof(ln_thread_t) * num_threads))
	{
		printk(KERN_ALERT "[Scaling Locks] Did not get full allocation for ln_thread_t. "
			"Cannot proceed.\n");
		if (threads)
			kfree(threads);
		return -1;
	}

	memset(&stats, 0, sizeof(ln_test_stats_t));
#ifdef DEBUG
	printk(KERN_ALERT "[Scaling Locks] Setting up for test %s with %u threads.\n",
		test->name, num_threads);
#endif

	test->ops.setup((1 << HASH_TABLE_BITS));

	for (i = 0; i < num_threads; i++)
	{
		char name[20];
		snprintf(name, 20, "scalinglocks%d", (i+1));

		threads[i].ops       = &test->ops;

		atomic_inc(&active_threads);
		threads[i].thread = kthread_create((void *) test_thread, &threads[i], name);
		if (IS_ERR_OR_NULL(threads[i].thread))
		{
			atomic_dec(&active_threads);
			printk(KERN_ALERT "[Scaling Locks] Unable to make worker thread %d\n", i);
			continue;
		}

		get_task_struct(threads[i].thread);
		wake_up_process(threads[i].thread);
	}

	stats.starttime = get_jiffies_64();
	/* Figure out when to stop the threads after 30 seconds have elapsed */
	while (jiffies_to_msecs(get_jiffies_64() - stats.starttime)
		< (TEST_RUNTIME_SECS * 1000))
	{
		/* Go to sleep */
		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(HZ);
		continue;
	}
	stats.endtime = get_jiffies_64();
	/* Signal all threads to quit it */
	for (i = 0; i < num_threads; i++)
		kthread_stop(threads[i].thread);

	/* Now wait for all the threads to actually quit */
	wait_event_interruptible(thread_wq, atomic_read(&active_threads) == 0);

#ifdef DEBUG
	printk(KERN_ALERT "[Scaling Locks] Completed test %s. %u threads terminated.\n",
		test->name, i);
#endif

	/* Free every object we allocated on that run. */
	free_hash_table();

	test->ops.teardown((1 << HASH_TABLE_BITS));
	
	/* Gather statistics and print to console. */
	ln_stats_collate_threads(threads, num_threads, &stats);
	thread_print_stats(test, &stats, num_threads);

	kfree(threads);

	return 0;
}
