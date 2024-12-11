#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/wait.h>
#include <linux/semaphore.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/device.h>

#define MAXNUM 100
#define DEFAULT_MAJOR_NUM 456

struct Scull_Dev {
    struct cdev devm;
    struct semaphore sem;
    wait_queue_head_t outq;
    int flag;
    char buffer[MAXNUM + 1];
    char *rd, *wr, *end;
};

static struct Scull_Dev globalvar;
static struct class *my_class;
static int major = DEFAULT_MAJOR_NUM;

static ssize_t globalvar_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t globalvar_write(struct file *, const char __user *, size_t, loff_t *);
static int globalvar_open(struct inode *, struct file *);
static int globalvar_release(struct inode *, struct file *);

struct file_operations globalvar_fops = {
    .read = globalvar_read,
    .write = globalvar_write,
    .open = globalvar_open,
    .release = globalvar_release,
};

static int __init globalvar_init(void) {
    int result;
    dev_t dev;

    if (major) {
        dev = MKDEV(major, 0);
        result = register_chrdev_region(dev, 1, "charmem");
    } else {
        result = alloc_chrdev_region(&dev, 0, 1, "charmem");
        major = MAJOR(dev);
    }

    if (result < 0) {
        pr_err("Failed to register char device region\n");
        return result;
    }

    cdev_init(&globalvar.devm, &globalvar_fops);
    globalvar.devm.owner = THIS_MODULE;
    result = cdev_add(&globalvar.devm, dev, 1);
    if (result) {
        pr_err("Failed to add char device: %d\n", result);
        unregister_chrdev_region(dev, 1);
        return result;
    }

    sema_init(&globalvar.sem, 1);
    init_waitqueue_head(&globalvar.outq);
    globalvar.rd = globalvar.buffer;
    globalvar.wr = globalvar.buffer;
    globalvar.end = globalvar.buffer + MAXNUM;
    globalvar.flag = 0;

    my_class = class_create(THIS_MODULE, "ch_device");
    if (IS_ERR(my_class)) {
        pr_err("Failed to create device class\n");
        cdev_del(&globalvar.devm);
        unregister_chrdev_region(dev, 1);
        return PTR_ERR(my_class);
    }

    if (!device_create(my_class, NULL, dev, NULL, "ch_device")) {
        pr_err("Failed to create device\n");
        class_destroy(my_class);
        cdev_del(&globalvar.devm);
        unregister_chrdev_region(dev, 1);
        return -ENOMEM;
    }

    pr_info("Char device registered successfully with major number %d\n", major);
    return 0;
}

static void __exit globalvar_exit(void) {
    device_destroy(my_class, MKDEV(major, 0));
    class_destroy(my_class);
    cdev_del(&globalvar.devm);
    unregister_chrdev_region(MKDEV(major, 0), 1);
    pr_info("Char device unregistered\n");
}

static int globalvar_open(struct inode *inode, struct file *filp) {
    try_module_get(THIS_MODULE);
    pr_info("Device opened\n");
    return 0;
}

static int globalvar_release(struct inode *inode, struct file *filp) {
    module_put(THIS_MODULE);
    pr_info("Device closed\n");
    return 0;
}

static ssize_t globalvar_read(struct file *filp, char __user *buf, size_t len, loff_t *off) {
    if (wait_event_interruptible(globalvar.outq, globalvar.flag != 0)) {
        return -ERESTARTSYS;
    }

    if (down_interruptible(&globalvar.sem)) {
        return -ERESTARTSYS;
    }

    globalvar.flag = 0;

    len = (globalvar.rd < globalvar.wr) ?
          min(len, (size_t)(globalvar.wr - globalvar.rd)) :
          min(len, (size_t)(globalvar.end - globalvar.rd));

    if (copy_to_user(buf, globalvar.rd, len)) {
        up(&globalvar.sem);
        return -EFAULT;
    }

    globalvar.rd += len;
    if (globalvar.rd == globalvar.end) {
        globalvar.rd = globalvar.buffer;
    }

    up(&globalvar.sem);
    pr_info("Read %zu bytes\n", len);
    return len;
}

static ssize_t globalvar_write(struct file *filp, const char __user *buf, size_t len, loff_t *off) {
    if (down_interruptible(&globalvar.sem)) {
        return -ERESTARTSYS;
    }

    len = (globalvar.rd <= globalvar.wr) ?
          min(len, (size_t)(globalvar.end - globalvar.wr)) :
          min(len, (size_t)(globalvar.rd - globalvar.wr - 1));

    if (copy_from_user(globalvar.wr, buf, len)) {
        up(&globalvar.sem);
        return -EFAULT;
    }

    globalvar.wr += len;
    if (globalvar.wr == globalvar.end) {
        globalvar.wr = globalvar.buffer;
    }

    up(&globalvar.sem);
    globalvar.flag = 1;
    wake_up_interruptible(&globalvar.outq);
    pr_info("Wrote %zu bytes\n", len);
    return len;
}

module_init(globalvar_init);
module_exit(globalvar_exit);
MODULE_LICENSE("GPL");
