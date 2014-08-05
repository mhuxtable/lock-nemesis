
#include <linux/percpu-rwsem.h>
#include <linux/vmalloc.h>

#include <test.h>

static struct percpu_rw_semaphore *locks = NULL;

static void ln_rwsem_setup(unsigned buckets)
{
  int lock;
  locks = (struct percpu_rw_semaphore *) vmalloc(sizeof(struct percpu_rw_semaphore) * buckets);

  for (lock = 0; lock < buckets; lock++)
    percpu_init_rwsem(&locks[lock]);

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
	percpu_down_read(&locks[bucket]);
	return NULL;
}

static void ln_rwsem_runlock(unsigned bucket, void *data)
{
	percpu_up_read(&locks[bucket]);
	return;
}

static void ln_rwsem_wlock(unsigned bucket, void *lockdata)
{
	percpu_down_write(&locks[bucket]);
	return NULL;
}

static void ln_rwsem_wunlock(unsigned bucket, void *data)
{
	percpu_up_write(&locks[bucket]);
	return;
}

static void ln_rwsem_teardown(unsigned buckets)
{
  int lock;

  for (lock = 0; lock < buckets; lock++)
    percpu_free_rwsem(&locks[lock]);
	locks = NULL;

	return;
}

ln_test_t test_percpu_rwsem_fine = {
	.name = "Per-CPU read-write semaphore, fine-grained",
	.shortname = "percpurwsem-fine",
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
