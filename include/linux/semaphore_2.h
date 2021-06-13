typedef struct semaphore_s
{
	raw_spinlock_t lock;
	unsigned int count;
	struct list_head wait_list;
} semaphore;

struct semaphore_waiter
{
	struct list_head list;
	struct task_struct *task;
	bool up;
};

#define _TABLE_SIZE 20