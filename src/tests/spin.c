
#include <linux/slab.h>
#include <linux/spinlock.h>

#include <test.h>

static spinlock_t *locks = NULL;

static void ln_spin_setup(unsigned buckets)
{
	int lock;
	locks = (spinlock_t *) kmalloc(sizeof(spinlock_t) * buckets, GFP_KERNEL);
	
	if (IS_ERR_OR_NULL(locks))
		BUG();
	
	for (lock = 0; lock < buckets; lock++)
		spin_lock_init(&locks[lock]);

	return;
}

static void ln_spin_lock(unsigned bucket)
{
	if (unlikely(!locks))
		BUG();
	
	spin_lock(&locks[bucket]);
	return;
}

static void ln_spin_unlock(unsigned bucket)
{
	if (unlikely(!locks))
		BUG();
	
	spin_unlock(&locks[bucket]);
	return;
}

static void ln_spin_teardown(void)
{
	if (unlikely(!locks))
		BUG();
	
	kfree(locks);
	locks = NULL;
	
	return;
}

ln_test_t test_spin = {
	.name = "Spinlock, singly threaded",
	.min_threads = 1,
	.max_threads = 1,
	.ops.setup = ln_spin_setup,
	.ops.lock  = ln_spin_lock,
	.ops.unlock = ln_spin_unlock,
	.ops.teardown = ln_spin_teardown,
	.stats.reads = 0,
	.stats.writes = 0,
};
