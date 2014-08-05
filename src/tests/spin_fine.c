
#include <linux/spinlock.h>
#include <linux/vmalloc.h>

#include <test.h>

static spinlock_t *locks = NULL;

static void ln_spin_setup(unsigned buckets)
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

static void ln_spin_lock(unsigned bucket, void *lockdata)
{
	spin_lock(&locks[bucket]);
	return NULL;
}

static void ln_spin_unlock(unsigned bucket, void *data)
{
	spin_unlock(&locks[bucket]);
	return;
}

static void ln_spin_teardown(unsigned buckets)
{
	vfree(locks);
	locks = NULL;
	
	return;
}

ln_test_t test_spin_fine = {
	.name = "Spinlock, fine-grained",
	.shortname = "spin-fine",
	.min_threads = 1,
	.max_threads = 12,
	.ops.setup = ln_spin_setup,
	.ops.threadsetup = no_thread_setup,
	.ops.threadteardown = no_thread_teardown,
	.ops.rlock  = ln_spin_lock,
	.ops.runlock = ln_spin_unlock,
	.ops.wlock = ln_spin_lock,
	.ops.wunlock = ln_spin_unlock,
	.ops.teardown = ln_spin_teardown,
	.stats.reads = 0,
	.stats.writes = 0,
};
