#include<linux/linkage.h>
#include<linux/module.h>
#include<linux/list.h>
#include<linux/printk.h>
#include <linux/sched.h>
#include<linux/semaphore_2.h>

extern semaphore* semaphore_table[_TABLE_SIZE];

asmlinkage long sys_up_2(int sem_id)
{
	printk("antes ~ init_semaphore ~ %d %d \n", sem_id, semaphore_table[sem_id]);
	semaphore *sem = semaphore_table[sem_id];

	unsigned long flags;

	raw_spin_lock_irqsave(&sem->lock, flags);
	if (list_empty(&sem->wait_list)){
		sem->count++;
	} else {
		struct semaphore_waiter *waiter = list_first_entry(&sem->wait_list,  struct semaphore_waiter, list);
		list_del(&waiter->list);
		waiter->up = true;
		wake_up_process(waiter->task);
	}
	raw_spin_unlock_irqrestore(&sem->lock, flags);	
	
	return printk("depois ~ init_semaphore ~ %d \n", sem->count);
}

