#include <linux/linkage.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/printk.h>
#include <linux/semaphore_2.h>
#include <linux/slab.h>

semaphore *semaphore_table[_TABLE_SIZE];
unsigned int semaphore_idx = 0;

EXPORT_SYMBOL(semaphore_table);

asmlinkage long sys_init_semaphore(unsigned int initial_value)
{
	printk("[KERNEL] INICIO DO INIT_SEMAPHORE ~ initial_value = %d sem_idx = %d\n", initial_value, semaphore_idx);

	size_t semaphore_size = sizeof(struct semaphore_s);
	semaphore *sem = (semaphore *)kmalloc(semaphore_size, GFP_KERNEL);

	if (!sem || semaphore_idx >= _TABLE_SIZE)
	{
		return -1;
	}

	*sem = (semaphore){
		.lock = __RAW_SPIN_LOCK_UNLOCKED(sem->lock),
		.count = initial_value,
		.wait_list = LIST_HEAD_INIT(sem->wait_list),
	};

	semaphore_table[semaphore_idx] = sem;
	semaphore_idx++;

	printk("[KERNEL] FIM DO INIT_SEMAPHORE ~ count = %d sem_id = %d sem_addr = %x\n", sem->count, semaphore_idx, semaphore_table[semaphore_idx - 1]);

	return semaphore_idx - 1;
}
