#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>

SYSCALL_DEFINE0(tarungetpid)
{
	return task_tgid_vnr(current);
}
