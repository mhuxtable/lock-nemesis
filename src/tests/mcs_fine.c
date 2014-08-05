
#include <linux/mcs_spinlock.h>
#include <linux/vmalloc.h>

#include <test.h>

static struct mcs_spinlock *locks = NULL;

static void ln_mcs_setup(unsigned buckets)
{
	locks = (struct mcs_spinlock *) vzalloc(sizeof(struct mcs_spinlock) * buckets);
	
	return;
}

static void *ln_mcs_lock(unsigned bucket)
{
	struct mcs_spinlock *node = (struct mcs_spinlock *) 
		vmalloc(sizeof(struct mcs_spinlock));
	mcs_spin_lock(&locks[bucket].next, node);
	return node;
}

static void ln_mcs_unlock(unsigned bucket, void *data)
{
	struct mcs_spinlock *node = (struct mcs_spinlock *) data;
	mcs_spin_unlock(&locks[bucket].next, node);
	vfree(data);
	return;
}

static void ln_mcs_teardown(unsigned buckets)
{
	vfree(locks);
	locks = NULL;
	
	return;
}

ln_test_t test_mcs_fine = {
	.name = "MCS, fine-grained",
	.shortname = "mcs-fine",
	.min_threads = 1,
	.max_threads = 12,
	.ops.setup = ln_mcs_setup,
	.ops.rlock  = ln_mcs_lock,
	.ops.runlock = ln_mcs_unlock,
	.ops.wlock = ln_mcs_lock,
	.ops.wunlock = ln_mcs_unlock,
	.ops.teardown = ln_mcs_teardown,
	.stats.reads = 0,
	.stats.writes = 0,
};
