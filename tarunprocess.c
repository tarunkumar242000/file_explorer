#include<linux/kernel.h>
#include<linux/sched.h>

asmlinkage long tarunprocess(void)
{
	printk("Parent Process ID : %d \n",current->parent->pid);
	printk("Current Process ID : %d \n",current->pid);
	return 0;
}
