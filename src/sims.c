
#include <test.h>

/* No locks */
static void * no_setup(unsigned buckets)
{
	return NULL;
}

static void no_lock(unsigned bucket)
{
	return;
}

static void no_unlock(unsigned bucket)
{
	return;
}

static void no_teardown(void * locks, unsigned buckets)
{
	return;
}

static ln_test_t test_none = {
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

void ln_sims_register_all(void)
{
	ln_test_register(&test_none);
	return;
}
