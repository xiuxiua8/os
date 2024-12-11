3.1 文件系统实验
3.1.1实验目的
通过一个简单文件系统的设计，理解文件系统的内部结构、基本功能及实现。
3.1.2实验思想
在分析 Linux 的文件系统的基础上，基于Ext2的思想和算法，设计一个类Ext2的虚拟多级文件系统，实现Ext2文件系统的一个功能子集。并且用现有操作系统上的文件来代替硬盘进行硬件模拟。设计文件系统应该考虑的几个层次：①介质的物理结构；②物理操作——设备驱动程序完成；③文件系统的组织结构（逻辑组织结构）；④对组织结构其上的操作；⑤为用户使用文件系统提供的接口。本实验只涉及后三个层次的内容。
3.1.3实验内容
（1）分析EXT2文件系统的结构；
（2）基于Ext2的思想和算法，设计一个类Ext2的多级文件系统，实现Ext2文件系统的一个功能子集；
（3）用现有操作系统上的文件来代替硬盘进行硬件模拟。
3.1.4实验报告内容要求
实验报告中，要给出所设计的文件系统架构图、主要数据结构的定义、主要函数的设计思路。

3.2 Linux的动态模块与设备驱动
3.2.1实验目的
(1)学习内核通过动态模块来动态加载内核新功能的机制；
(2)理解LINUX字符设备驱动程序的基本原理；
(3)掌握字符设备的驱动运作机制；
(4)学会编写字符设备驱动程序；
(5)学会编写用户程序通过对字符设备的读写完成不同用户间的通信。
3.2.2实验内容
(1)编译、安装与卸载动态模块；
(2)实现系统调用的篡改；
(3)编写一个简单的字符设备驱动程序，以内核空间模拟字符设备，完成对该设备的打开、读写和释放操作；
(4)编写聊天程序实现不同用户通过该设备的一对一、一对多、多对多聊天。
3.2.3实验报告内容要求
报告中要说明驱动程序的实现；用户程序的实现；如何实现读写的同步与互斥；一对一、一对多、多对多的聊天分别是如何实现的。
3.3 其他可选题目
(1)设计实现一个最小操作系统内核
开发PC机或笔记本的基于USB盘的引导程序，设计内核任务及其系统调用。
(2)为Linux设计实现一个新的调度程序
在分析Linux内核调度程序的基础上，将其进程调度算法改为随机调度或其他调度算法。
(3) 模拟实现虚拟存储
分析研究虚拟存储的思想，模拟实现虚拟存储（要包括数据缓冲、交换文件的设计）。

# 实验内容
## 1 编译、安装与卸载动态模块

make
ls
sudo insmod mymodules.ko
lsmod 
rmmod mymodules


- 诊断信息 diagnostic
sudo dmesg -T

- 查询内核版本
uname -r
cat /proc/version
hostnamectl
dmesg | grep "Linux version"


- 是否包含 mymodules
lsmod | grep mymodules
ls -l /dev/ | grep mymodules
- 删除
sudo rmmod mymodules


## 2 实现系统调用的篡改

- 编译模块 加载模块
make
sudo insmod modify_syscall.ko
- 执行用户程序
./modify_old_syscall
./modify_new_syscall
- 卸除模块
rmmod modify_syscall

从 `/boot/System.map` 文件中手动查找地址。

怎么做？

bash-5.0$ ls /boot/System.map-$(uname -r)
/boot/System.map-4.19.90-2110.8.0.0119.oe1.aarch64
bash-5.0$ grep sys_call_table /boot/System.map-$(uname -r)
ffff000008af0698 R sys_call_table
ffff000008af3a98 R a32_sys_call_table

我执行了
sudo insmod modify_syscall.ko
vscode 的ssh连接就卡掉了

bash-5.0$ lsmod
Module                  Size  Used by
aes_ce_blk            262144  0
crypto_simd           262144  1 aes_ce_blk
cryptd                262144  1 crypto_simd
aes_ce_cipher         262144  1 aes_ce_blk
ghash_ce              262144  0
sha2_ce               262144  0
sha256_arm64          262144  1 sha2_ce

### insert module globalvar
bash-5.0$ sudo insmod globalvar.ko
[sudo] password for zilong: 
bash-5.0$ lsmod
Module                  Size  Used by
globalvar             262144  0
aes_ce_blk            262144  0
crypto_simd           262144  1 aes_ce_blk
cryptd                262144  1 crypto_simd
aes_ce_cipher         262144  1 aes_ce_blk
ghash_ce              262144  0


### insert module modify_syscall ssh中断了


内核崩溃是因为模块试图对只读区域进行写操作

系统调用篡改那里需要先关闭对应位置的写保护

在现代 Linux 内核中想从模块层面关闭对内核关键数据结构（比如 `sys_call_table` 所在内存区域）的写保护几乎是不可能的。内核已经从底层架构上进行了一系列安全措施使这种行为变得异常困难甚至无法实现。以下是一些常见但不再适用或不推荐的方式及原因：

1. **x86 架构上的老方法：**  
   在很老的 x86 内核中，可以通过清除 `CR0` 寄存器中的 `WP`（Write Protect）位来临时关闭写保护：
   ```c
   write_cr0(read_cr0() & ~X86_CR0_WP);
   // ...对内存进行修改...
   write_cr0(read_cr0() | X86_CR0_WP);
   ```
   但是，这种方法在现代内核中通常已不可行，且会触发内核保护机制，使模块无法正常工作或导致内核拒绝加载模块。

2. **aarch64 架构上的问题：**  
   你所使用的内核为 `4.19.90-2110.8.0.0119.oe1.aarch64`，在 aarch64 架构下并没有类似 x86 的简单 `CR0.WP` 位可以通过模块级别的代码轻松清除。内核会对 `.rodata` 段（包括 `sys_call_table`）进行严格保护，以防止运行时修改。

3. **`set_memory_rw()` 不适用：**  
   `set_memory_rw()` 和 `set_memory_ro()` 等函数通常只适用于模块自身分配的内存页（如 `vmalloc` 的内存），不适用于内核核心只读数据段。在对内核核心内存段调用这类函数时，往往无效，并且会触发安全机制。

4. **内核安全性增强措施：**  
   现代内核在编译和运行时启用多种安全增强特性（如内核地址空间布局随机化(KASLR)、只读数据段保护、不可修改的 `sys_call_table` 等）。这些特性旨在防止内核被恶意代码篡改。

**可行的替代方式：**

- 使用 **kprobe/kretprobe** 或 **ftrace** 来 hook 内核函数：  
  虽然不能直接修改 `sys_call_table`，但可以通过 kprobe 或 ftrace 对内核函数进行动态追踪和重定向，以实现类似拦截系统调用的效果（实际上是拦截 `do_syscall_*` 或相关路径）。
  
- 使用 **eBPF**：  
  eBPF 是一种安全且受内核限制的方式，用于过滤和拦截系统调用。这不会直接修改 `sys_call_table`，但可以在系统调用进入内核时对参数和行为进行控制。

- 修改内核源码并重新编译：  
  如果目标是研究性或特定环境使用，可以在编译内核时关闭 `rodata` 保护(例如内核启动参数 `rodata=off`)，但这会大幅降低内核的安全性，不适合生产环境。

**总结：**  
在现代内核中，没有简单的方法从模块中关闭内核的写保护来篡改 `sys_call_table`。内核的安全策略使这种行为非常困难和不推荐。您应该采用更符合当下内核设计原则的替代方案来实现您的需求。

## 3 编写一个简单的字符设备驱动程序，以内核空间模拟字符设备，完成对该设备的打开、读写和释放操作


设备文件的 VFS中，每个文件都有一个 inode 与其对应，内核的 inode 结构中的 i_fop
成员，类型为 file_operations,file_operations 定义了文件的各种操作,用户对文件操作是通过调用 file_operations 来实现的，或者说内核使用file_operations 来访问设备驱动程序中的函数,为了使用户对设备文件的操作
能够转换成对设备的操作， VFS 必须在设备文件打开时，改变其 inode 结构中i_fop 成员的默认值，将其替换为与设备相关的具体函数操作。

在这里我们使用的是

```c
struct file_operations globalvar_fops = {
    .read = globalvar_read,
    .write = globalvar_write,
    .open = globalvar_open,
    .release = globalvar_release,
};
```
将该文件的读写打开释放操作变成自定义的，从而完成了字符设备驱动程序的编写。

利用flag和PV操作实现了写读相关的互斥。 
这里使用down/up_interruptible函数，它们是是 Linux 内核中用于进行 PV 操作的函数，是信号可中断的版本。
对于字符设备等场景，通常使用这个函数来进行信号量的 PV 操作。使用其他的 PV 操作函数，可以考虑down/up()函数，其信号不可中断，不响应中断信号


可用以下命令查看使用模块情况  
sudo lsof /dev/chardev0
可用以下命令杀死所有使用该模块的进程  
sudo fuser -k /dev/chardev0

下面是代码各部分的详细功能说明和实现逻辑：

### 1. 头文件导入
导入了字符设备驱动所需的内核头文件，比如 `fs.h`（文件操作）、`cdev.h`（字符设备支持）
`uaccess.h`（用户-内核空间数据交换）、`semaphore.h`（信号量）等

---

### 2. 全局变量和数据结构定义
```c
struct Scull_Dev {
    struct cdev devm;
    struct semaphore sem;
    wait_queue_head_t outq;
    int flag;
    char buffer[MAXNUM + 1];
    char *rd, *wr, *end;
};
```
- **`struct Scull_Dev`** 定义了一个字符设备的结构，包含：
  - **`cdev devm`**：字符设备对象，管理设备文件和操作方法的绑定。
  - **`semaphore sem`**：信号量，保证读写互斥操作。
  - **`wait_queue_head_t outq`**：等待队列，用于阻塞读操作。
  - **`flag`**：标志变量，用于唤醒阻塞的进程。
  - **`buffer`**：存储数据的循环缓冲区。
  - **`rd`, `wr`, `end`**：分别是缓冲区的读指针、写指针和尾指针。


### 3. 设备操作方法

struct file_operations 定义了设备操作接口，绑定了 `read`, `write`, `open`, `release` 函数。


### 4. 模块初始化和注销
#### 初始化函数
```c
static int __init globalvar_init(void);
```
- 分配设备号：
  - 如果 `major` 有值，则使用静态设备号注册。
  - 如果 `major` 为 0，则动态分配设备号并记录主设备号。
- 初始化字符设备并绑定操作函数。
- 初始化信号量、等待队列和缓冲区指针。
- 创建 `/dev/chardev0` 设备节点，供用户程序访问。

#### 注销函数
```c
static void __exit globalvar_exit(void);
```
- 销毁设备节点，释放设备号和字符设备对象。


### 5. 打开和关闭设备
#### 打开设备
- 调用 `try_module_get` 增加模块引用计数，防止模块被卸载。
- 调用 `module_put` 减少模块引用计数。

### 6. 读操作
```c
static ssize_t globalvar_read(struct file *filp, char __user *buf, size_t len, loff_t *off);
```
- **等待可读条件**：
  - 调用 `wait_event_interruptible`，如果 `flag == 0`，阻塞进程直到 `flag` 被置 1。
- **互斥访问**：
  - 调用 `down_interruptible` 获取信号量，确保缓冲区不被其他进程访问。
- **数据读取**：
  - 根据读写指针计算实际可读取的字节数。
  - 使用 `copy_to_user` 将数据从内核空间拷贝到用户空间。
  - 更新读指针，如果到达缓冲区尾部则循环到起始位置。
- **释放信号量**：
  - 调用 `up` 释放信号量，允许其他进程访问缓冲区。

---

### 7. 写操作
```c
static ssize_t globalvar_write(struct file *filp, const char __user *buf, size_t len, loff_t *off);
```
- **互斥访问**：
  - 获取信号量，确保缓冲区不被其他进程修改。
- **数据写入**：
  - 根据读写指针计算实际可写入的字节数。
  - 使用 `copy_from_user` 将数据从用户空间拷贝到内核空间。
  - 更新写指针，如果到达缓冲区尾部则循环到起始位置。
- **唤醒阻塞进程**：
  - 设置 `flag` 为 1，表示数据可读。
  - 调用 `wake_up_interruptible` 唤醒阻塞的读进程。


### 8. 内核日志
使用 `pr_info` 输出调试信息，帮助观察模块运行状态。
在初始化失败、读写错误时输出详细日志。

### 9. 同步与阻塞机制
保证读写操作的互斥访问。
当缓冲区中无数据时，读操作会阻塞，直到有数据写入。

### 10. 循环缓冲区
缓冲区采用循环机制

1 to 1 实现 write 不断向缓冲区写， read 阅后即焚

### 4 编写聊天程序实现不同用户通过该设备的一对一、一对多、多对多聊天



