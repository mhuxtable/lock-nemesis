
#include <linux/spinlock.h>
#include <linux/vmalloc.h>

#include <test.h>

static DEFINE_SPINLOCK(spinlock_lock);

static void ln_spin_setup(unsigned buckets)
{
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
	spin_lock(&spinlock_lock);
	return NULL;
}

static void ln_spin_unlock(unsigned bucket, void *data)
{
	spin_unlock(&spinlock_lock);
	return;
}

static void ln_spin_teardown(unsigned buckets)
{
	return;
}

ln_test_t test_spin_coarse = {
	.name = "Spinlock, coarse-grained",
	.shortname = "spin-coarse",
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
