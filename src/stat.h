#ifndef LN_STAT_H
#define LN_STAT_H

#include <test.h>
#include <thread.h>

void thread_print_stats(ln_test_t *test, ln_test_stats_t *stats, int num_threads);
void ln_stats_collate_threads(ln_thread_t *threads, int num_threads,
	ln_test_stats_t *result);

#endif // LN_STAT_H
