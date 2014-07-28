
#include <test.h>

/* No locks */
static void no_setup(unsigned buckets)
{
	return;
}

static void no_read_lock(unsigned bucket)
{
	return;
}

static void no_read_unlock(unsigned bucket)
{
	return;
}

static void no_write_lock(unsigned bucket)
{
	return;
}

static void no_write_unlock(unsigned bucket)
{
	return;
}

static void no_teardown(unsigned buckets)
{
	return;
}

ln_test_t test_none = {
	.name = "No locks, singly threaded",
	.shortname = "nolock",
	.min_threads = 1,
	.max_threads = 1,
	.ops.setup = no_setup,
	.ops.rlock  = no_read_lock,
	.ops.runlock = no_read_unlock,
	.ops.wlock  = no_write_lock,
	.ops.wunlock = no_write_unlock,
	.ops.teardown = no_teardown,
	.stats.reads = 0,
	.stats.writes = 0,
};

