#ifndef LN_THREAD_H
#define LN_THREAD_H

#include <test.h>

typedef struct ln_thread
{
	struct task_struct *thread;
	char               *threadname;
	ln_test_stats_t     stats;
	ln_test_ops_t      *ops;
	void               *locks;
} ln_thread_t;

#endif // LN_THREAD_H
