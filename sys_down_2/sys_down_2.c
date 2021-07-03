#include <linux/linkage.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <linux/semaphore_2.h>
#include <linux/slab.h>

extern semaphore *semaphore_table[_TABLE_SIZE];

asmlinkage long sys_down_2(int sem_id)
{
	printk("[KERNEL] INICIO sys_down_2 ~ sem_id = %d sem_addr = %x \n", sem_id, semaphore_table[sem_id]);
	semaphore *sem = semaphore_table[sem_id];
	unsigned long flags;

	size_t semaphore_waiter_size = sizeof(struct semaphore_waiter);
	struct semaphore_waiter *waiter = kmalloc(semaphore_waiter_size, GFP_KERNEL);

	raw_spin_lock_irqsave(&sem->lock, flags);
	if (sem->count > 0)
	{
		sem->count--;
	}
	else
	{
		if (!waiter)
		{
			return -1;
		}

		list_add_tail(&waiter->list, &sem->wait_list);
		waiter->task = current;
		waiter->up = false;
		current->state = TASK_UNINTERRUPTIBLE;

		printk("[KERNEL] sys_down_2 ~ esse processo vai dormir = %d \n", current->pid);
		schedule();
	}
	raw_spin_unlock_irqrestore(&sem->lock, flags);

	return printk("[KERNEL] FIM sys_down_2 ~ sem_id = %d count = %d \n", sem_id, sem->count);
}
