#include<linux/linkage.h>
#include<linux/module.h>
#include<linux/list.h>
#include<linux/printk.h>
#include<linux/semaphore_2.h>
#include <linux/slab.h>


semaphore * semaphore_table[_TABLE_SIZE];
unsigned int semaphore_idx = 0;

EXPORT_SYMBOL(semaphore_table);

asmlinkage long sys_init_semaphore(unsigned int initial_value)
{
	printk("antes INIT_SEMAPHORE ~ %d %d", initial_value);

	size_t semaphore_size = sizeof(struct semaphore_s);
	semaphore *sem = (semaphore *) kmalloc(semaphore_size, GFP_KERNEL);

	*sem = (semaphore) {
		.lock = __RAW_SPIN_LOCK_UNLOCKED(sem->lock),
	    .count = initial_value,
	    .wait_list = LIST_HEAD_INIT(sem->wait_list),
	};

	semaphore_table[semaphore_idx] = sem;
	semaphore_idx++;

	printk("depois INIT_SEMAPHORE ~ %d %d %d\n", sem->count, semaphore_idx, semaphore_table[semaphore_idx-1]);

	return semaphore_idx - 1;
}
