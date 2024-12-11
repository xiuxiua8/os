#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/ptrace.h>
#include <linux/sched.h>

/* 假设78号syscall在内核中的实现函数是 __arm64_sys_gettimeofday(int a, int b)
 * 请根据实际内核确定该函数名称和参数。
 */
static char func_name[] = "__arm64_sys_gettimeofday";

/* 用于在kretprobe的实例数据中记录参数 */
struct my_data {
    int a;
    int b;
};

static struct kprobe kp = {
    .symbol_name = func_name,
};

static struct kretprobe krp;

// pre_handler在目标函数执行前调用，用于获取参数
static int handler_pre(struct kprobe *p, struct pt_regs *regs)
{
    // 对于aarch64，函数参数按顺序存放在x0, x1, x2...中
    // 假设函数签名为: long __arm64_sys_somecall(int a, int b)
    // 则 a = regs->regs[0], b = regs->regs[1]
    // 这里不修改参数，只是记录下来
    struct my_data *data;
    data = (struct my_data *)krp.kp.addr; // 此处不可行，需要使用kretprobe特定的方法存储数据，详见后续说明
    // 由于我们要在post_handler中访问数据，应在kretprobe的entry_handler中保存参数
    
    return 0;
}

// kretprobe的entry_handler在函数即将执行时调用（但在函数本体之前）
static int ret_entry_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{
    struct my_data *data = (struct my_data *)ri->data;

    // 获取参数
    data->a = (int)regs->regs[0];
    data->b = (int)regs->regs[1];

    return 0;
}

// kretprobe的handler在函数执行完毕返回后调用
static int ret_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{
    struct my_data *data = (struct my_data *)ri->data;

    // 打印信息
    pr_info("Intercepted syscall(78): a=%d, b=%d\n", data->a, data->b);

    // 原函数执行完毕后，返回值放在regs->regs[0]中
    // 我们将其强行改为 a+b
    regs->regs[0] = data->a + data->b;

    pr_info("Modified return value to: %ld\n", (long)regs->regs[0]);

    return 0;
}

static int __init mymodule_init(void)
{
    int ret;

    // 设置kprobe。这里虽然定义了kp，但实际hook的主要逻辑会在kretprobe中。
    kp.symbol_name = func_name;
    ret = register_kprobe(&kp);
    if (ret < 0) {
        pr_err("register_kprobe failed, returned %d\n", ret);
        return ret;
    }
    unregister_kprobe(&kp);

    // 使用kretprobe
    krp.kp.symbol_name = func_name;
    krp.entry_handler = ret_entry_handler;
    krp.handler = ret_handler;
    krp.data_size = sizeof(struct my_data);
    krp.maxactive = 20; // 可同时跟踪的并发调用实例数

    ret = register_kretprobe(&krp);
    if (ret < 0) {
        pr_err("register_kretprobe failed, returned %d\n", ret);
        return ret;
    }

    pr_info("kretprobe registered for %s\n", func_name);
    return 0;
}

static void __exit mymodule_exit(void)
{
    unregister_kretprobe(&krp);
    pr_info("kretprobe unregistered\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);
MODULE_LICENSE("GPL");
