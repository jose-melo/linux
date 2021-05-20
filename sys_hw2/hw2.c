#include<linux/linkage.h>
#include<linux/module.h>

asmlinkage long sys_hw2(void)
{
	return pr_info("hello world 2\n");
}
