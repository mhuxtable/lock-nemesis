
#include <stat.h>
#include <test.h>

void ln_stats_print_header(void)
{
	printk(KERN_ALERT
		"Name\t"
		"Time\t"
		"Threads\t"
		"Reads\t"
		"Writes\t"
		"TotalOps\t"
		"VerMiss\t"
		"VerCorr\t"
		"OpsPerSec\n"
	);
	return;
}

void thread_print_stats(ln_test_t *test, ln_test_stats_t *stats, int num_threads)
{
	int ops_total = stats->reads + stats->writes;
	u64 runtime   = (stats->endtime - stats->starttime) / HZ;

	printk(KERN_ALERT "%s\t%llu\t%d\t%d\t%d\t%d\t%d\t%d\t%llu\n",
		test->shortname,
		runtime,
		num_threads,
		stats->reads, stats->writes, ops_total,
		stats->verify_miss,
		stats->verify_corrupt,
		(ops_total / runtime)
	);
}

void ln_stats_collate_threads(ln_thread_t *threads, int num_threads,
	ln_test_stats_t *result)
{
	int i;
	for (i = 0; i < num_threads; i++)
	{
		ln_thread_t *t = &threads[i];
		result->reads  += t->stats.reads;
		result->writes += t->stats.writes;
		result->verify_miss    += t->stats.verify_miss;
		result->verify_corrupt += t->stats.verify_corrupt;
	}
	return;
}
