#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

// Original syscall 78 function: gettimeofday
// New syscall 78 function: print "No 78 syscall has changed to hello" 
// and return a + b
#define SYS_NO 78
unsigned long old_sys_call_func;
unsigned long p_sys_call_table = 0xc0361860; // Find in /boot/System.map --'uname -r'

asmlinkage int hello(int a, int b) // New function
{
    printk("No 78 syscall has changed to hello");
    return a + b;
}

void modify_syscall(void)
{
    unsigned long *sys_call_addr;
    sys_call_addr = (unsigned long *)(p_sys_call_table + SYS_NO * 4);
    old_sys_call_func = *sys_call_addr;
    *sys_call_addr = (unsigned long)&hello; // Point to new function
}

void restore_syscall(void)
{
    unsigned long *sys_call_addr;
    sys_call_addr = (unsigned long *)(p_sys_call_table + SYS_NO * 4);
    *sys_call_addr = old_sys_call_func; // Point to original function
}

static int mymodule_init(void)
{
    modify_syscall();
    return 0;
}

static void mymodule_exit(void)
{
    restore_syscall();
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_LICENSE("GPL");
