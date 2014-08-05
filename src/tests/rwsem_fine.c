
#include <linux/rwsem.h>
#include <linux/vmalloc.h>

#include <test.h>

static struct rw_semaphore *locks = NULL;

static void ln_rwsem_setup(unsigned buckets)
{
	int lock;
	locks = (struct rw_semaphore *) vmalloc(sizeof(struct rw_semaphore) * buckets);
	
	for (lock = 0; lock < buckets; lock++)
		init_rwsem(&locks[lock]);

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

static void ln_rwsem_rlock(unsigned bucket, void *lockdata)
{
	down_read(&locks[bucket]);
	return;
}

static void ln_rwsem_runlock(unsigned bucket, void *data)
{
	up_read(&locks[bucket]);
	return;
}

static void ln_rwsem_wlock(unsigned bucket, void *lockdata)
{
	down_write(&locks[bucket]);
	return;
}

static void ln_rwsem_wunlock(unsigned bucket, void *data)
{
	up_write(&locks[bucket]);
	return;
}

static void ln_rwsem_teardown(unsigned buckets)
{
	vfree(locks);
	locks = NULL;
	
	return;
}

ln_test_t test_rwsem_fine = {
	.name = "Read-write semaphore, fine-grained",
	.shortname = "rwsem-fine",
	.min_threads = 1,
	.max_threads = 12,
	.ops.setup = ln_rwsem_setup,
	.ops.threadsetup = no_thread_setup,
	.ops.threadteardown = no_thread_teardown,
	.ops.rlock  = ln_rwsem_rlock,
	.ops.runlock = ln_rwsem_runlock,
	.ops.wlock = ln_rwsem_wlock,
	.ops.wunlock = ln_rwsem_wunlock,
	.ops.teardown = ln_rwsem_teardown,
	.stats.reads = 0,
	.stats.writes = 0,
};
