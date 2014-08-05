
#include <linux/rcupdate.h>
#include <linux/spinlock.h>
#include <linux/vmalloc.h>

#include <test.h>

/* RCU locks still have to protect writes from being invoked concurrently.
   For this purpose, spinlocks are used in this test. */
static spinlock_t *locks = NULL;

static void ln_rcu_setup(unsigned buckets)
{
	int lock;
	locks = (spinlock_t *) vmalloc(sizeof(spinlock_t) * buckets);

	for (lock = 0; lock < buckets; lock++)
		spin_lock_init(&locks[lock]);

	return;
}

static void *no_thread_setup(unsigned buckets)
{
	return NULL;
}

static void no_thread_teardown(void *data)
{
	return;
}

static void ln_rcu_rlock(unsigned bucket, void *lockdata)
{
	rcu_read_lock();
	return NULL;
}

static void ln_rcu_runlock(unsigned bucket, void *data)
{
	rcu_read_unlock();
	return;
}

static void ln_rcu_wlock(unsigned bucket, void *lockdata)
{
	spin_lock(&locks[bucket]);
	return NULL;
}

static void ln_rcu_wunlock(unsigned bucket, void *data)
{
	spin_unlock(&locks[bucket]);
	return;
}

static void ln_rcu_teardown(unsigned buckets)
{
	vfree(locks);
	locks = NULL;
	
	return;
}

ln_test_t test_rcu_finewrite = {
	.name = "RCU, fine-grained writes",
	.shortname = "RCU",
	.min_threads = 1,
	.max_threads = 12,
	.ops.setup = ln_rcu_setup,
	.ops.threadsetup = no_thread_setup,
	.ops.threadteardown = no_thread_teardown,
	.ops.rlock  = ln_rcu_rlock,
	.ops.runlock = ln_rcu_runlock,
	.ops.wlock = ln_rcu_wlock,
	.ops.wunlock = ln_rcu_wunlock,
	.ops.teardown = ln_rcu_teardown,
	.stats.reads = 0,
	.stats.writes = 0,
};
