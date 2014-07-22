
#include <stat.h>

void thread_print_stats(ln_thread_t *thread)
{
	int ops_total = thread->stats.reads + thread->stats.writes;
	u64 runtime   = (thread->stats.endtime - thread->stats.starttime) / HZ;

	printk("[Scaling Locks] [%s] Statistics of run:\n"
	       "                     * Runtime :  %llu\n"
	       "                     * Op Count:  %d reads, %d writes, %d total\n"
	       "                     * Ops/sec :  %llu\n"
		   "                     ***************** VERIFICATION *****************\n"
		   "                     * Missing :  %d objects \n"
		   "                     * Corrupt :  %d objects \n",
		   thread->threadname,
		   runtime,
		   thread->stats.reads, thread->stats.writes, ops_total,
		   ops_total / runtime,
		   thread->stats.verify_miss,
		   thread->stats.verify_corrupt
	);
}

