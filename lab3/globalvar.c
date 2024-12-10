#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");

#define MAJOR_NUM 290

static ssize_t globalvar_read(struct file *filp, char *buf, size_t len, loff_t *off);
static ssize_t globalvar_write(struct file *filp, const char *buf, size_t len, loff_t *off);

struct file_operations globalvar_fops = {
    .read = globalvar_read,
    .write = globalvar_write,
};

static int global_var = 0;

static int __init init_mymodule(void)
{
    int ret;
    ret = register_chrdev(MAJOR_NUM, "globalvar", &globalvar_fops);
    if (ret) {
        printk("globalvar register failure\n");
    } else {
        printk("globalvar register success\n");
    }
    return ret;
}

static void __exit cleanup_mymodule(void)
{
    unregister_chrdev(MAJOR_NUM, "globalvar");
}

static ssize_t globalvar_read(struct file *filp, char *buf, size_t len, loff_t *off)
{
    if (copy_to_user(buf, &global_var, sizeof(int))) {
        return -EFAULT;
    }
    return sizeof(int);
}

static ssize_t globalvar_write(struct file *filp, const char *buf, size_t len, loff_t *off)
{
    if (copy_from_user(&global_var, buf, sizeof(int))) {
        return -EFAULT;
    }
    return sizeof(int);
}

module_init(init_mymodule);
module_exit(cleanup_mymodule);
