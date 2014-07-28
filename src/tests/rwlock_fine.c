
#include <linux/spinlock.h>
#include <linux/vmalloc.h>

#include <test.h>

static rwlock_t *locks = NULL;

static void ln_spin_setup(unsigned buckets)
{
	int lock;
	locks = (rwlock_t *) vmalloc(sizeof(rwlock_t) * buckets);
	
	for (lock = 0; lock < buckets; lock++)
		rwlock_init(&locks[lock]);

	return;
}

static void ln_read_lock(unsigned bucket)
{
	read_lock(&locks[bucket]);
	return;
}

static void ln_read_unlock(unsigned bucket)
{
	read_unlock(&locks[bucket]);
	return;
}

static void ln_write_lock(unsigned bucket)
{
	write_lock(&locks[bucket]);
	return;
}

static void ln_write_unlock(unsigned bucket)
{
	write_unlock(&locks[bucket]);
	return;
}

static void ln_spin_teardown(void)
{
	vfree(locks);
	locks = NULL;
	
	return;
}

ln_test_t test_rwlock_fine = {
	.name = "RW-spinlock, fine-grained",
	.min_threads = 1,
	.max_threads = 8,
	.ops.setup = ln_spin_setup,
	.ops.rlock  = ln_read_lock,
	.ops.runlock = ln_read_unlock,
	.ops.wlock = ln_write_lock,
	.ops.wunlock = ln_write_unlock,
	.ops.teardown = ln_spin_teardown,
	.stats.reads = 0,
	.stats.writes = 0,
};
