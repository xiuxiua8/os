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

## 实验内容
### 1 编译、安装与卸载动态模块

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


### 2 实现系统调用的篡改

- 编译模块 加载模块
make
sudo insmod modify_syscall.ko
- 执行用户程序
./modify_old_syscall
./modify_new_syscall
- 卸除模块
rmmod modify_syscall

我执行了
sudo insmod modify_syscall.ko
vscode 的ssh连接就卡掉了

这可能是系统调用表中 syscall 78 的行为被错误地修改，影响了系统的正常运行，甚至可能导致 系统内核陷入不稳定状态。
SSH 服务可能依赖于 syscall 78（如 gettimeofday 或其他时间相关功能）来处理网络通信、超时计算等。如果系统调用被替换为不兼容的逻辑，SSH 服务会异常。


从 `/boot/System.map` 文件中手动查找地址。

怎么做？

bash-5.0$ ls /boot/System.map-$(uname -r)
/boot/System.map-4.19.90-2110.8.0.0119.oe1.aarch64
bash-5.0$ grep sys_call_table /boot/System.map-$(uname -r)
ffff000008af0698 R sys_call_table
ffff000008af3a98 R a32_sys_call_table

内核崩溃是因为你的模块试图对只读区域进行写操作吧

系统调用篡改那里需要先关闭对应位置的写保护

### 3 编写一个简单的字符设备驱动程序，以内核空间模拟字符设备，完成对该设备的打开、读写和释放操作

### 4 编写聊天程序实现不同用户通过该设备的一对一、一对多、多对多聊天

bash-5.0$ lsmod
Module                  Size  Used by
aes_ce_blk            262144  0
crypto_simd           262144  1 aes_ce_blk
cryptd                262144  1 crypto_simd
aes_ce_cipher         262144  1 aes_ce_blk
ghash_ce              262144  0
sha2_ce               262144  0
sha256_arm64          262144  1 sha2_ce

#### insert module globalvar
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


#### insert module modify_syscall ssh中断了


[zilong@ecs-zilong ~]$ ls
bin  coding  dotfiles  share
[zilong@ecs-zilong ~]$ lsmod
Module                  Size  Used by
globalvar             262144  0
aes_ce_blk            262144  0
crypto_simd           262144  1 aes_ce_blk
cryptd                262144  1 crypto_simd
aes_ce_cipher         262144  1 aes_ce_blk

[zilong@ecs-zilong ~]$ Connection to 121.36.88.76 closed by remote host.
Connection to 121.36.88.76 closed.
❯ ssh hua
bind [127.0.0.1]:9999: Address already in use
channel_setup_fwd_listener_tcpip: cannot listen to port: 9999
Could not request local forwarding.

	Welcome to Huawei Cloud Service

Last login: Wed Dec 11 00:53:49 2024 from 1.85.33.91


Welcome to 4.19.90-2110.8.0.0119.oe1.aarch64

System information as of time: 	2024年 12月 11日 星期三 00:58:57 CST

System load: 	2.54
Processes: 	160
Memory used: 	21.1%
Swap used: 	0.0%
Usage On: 	14%
IP address: 	192.168.1.54
Users online: 	1



[zilong@ecs-zilong ~]$ lsmod
Module                  Size  Used by
aes_ce_blk            262144  0
crypto_simd           262144  1 aes_ce_blk
cryptd                262144  1 crypto_simd
aes_ce_cipher         262144  1 aes_ce_blk
ghash_ce              262144  0

注意到globalvar也不见了

