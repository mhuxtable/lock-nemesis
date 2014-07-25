
#include <test.h>

/* No locks */
static void no_setup(unsigned buckets)
{
	return;
}

static void no_lock(unsigned bucket)
{
	return;
}

static void no_unlock(unsigned bucket)
{
	return;
}

static void no_teardown(void)
{
	return;
}

ln_test_t test_none = {
	.name = "No locks, singly threaded",
	.min_threads = 1,
	.max_threads = 1,
	.ops.setup = no_setup,
	.ops.lock  = no_lock,
	.ops.unlock = no_unlock,
	.ops.teardown = no_teardown,
	.stats.reads = 0,
	.stats.writes = 0,
};

