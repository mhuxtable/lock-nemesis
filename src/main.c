
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/kthread.h>
#include <linux/sched.h>

#include <test.h>

static struct task_struct *thr_test;
extern int ln_test_run_all(void);

extern ln_test_t test_none;
extern ln_test_t test_spin;

// for each batch of 256, on average, by LLN
int    write_fraction = 26;
module_param_named(writefrac, write_fraction, int, 0);

void ln_sims_register_all(void)
{
	ln_test_register(&test_none);
	ln_test_register(&test_spin);
	return;
}

static int __init locking_init(void)
{
	printk(KERN_ALERT "[Scaling Locks] Starting up...\n");

	printk(KERN_ALERT "[Scaling Locks] Write fraction configured as %d in 256\n",
		write_fraction);

	ln_sims_register_all();

	/* spin up the thread for running the tests */
	thr_test = kthread_create((void*)ln_test_run_all, NULL, "scalinglocks");
	
	if (IS_ERR_OR_NULL(thr_test))
	{
		printk(KERN_ALERT "[Scaling Locks] Unable to create management thread. Quitting.\n");
		return -1;
	}

	get_task_struct(thr_test);
	wake_up_process(thr_test);

	return 0;
}

static void __exit locking_quit(void)
{
	printk(KERN_ALERT "[Scaling Locks] Shutting down.\n");
	kthread_stop(thr_test);
	return;
}

module_init(locking_init);
module_exit(locking_quit);
MODULE_LICENSE("BSD");
