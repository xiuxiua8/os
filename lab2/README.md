### signal.c

多次运行所写程序，比较5s内按下Ctrl+\或Ctrl+Delete发送中断，
或5s内不进行任何操作发送中断，分别会出现什么结果？分析原因。


#### 5s内按下Ctrl+\或Ctrl+Delete发送中断

```
bash-5.0$ make; ./signal
make: Nothing to be done for 'all'.
^\
3 stop test

17 stop test

Child process2 is killed by parent!!

16 stop test

Child process1 is killed by parent!!

Parent process is killed!!
```
父进程收到 SIGQUIT 信号并按照预期发送了信号 16 和 17 以终止子进程，程序正常完成了任务。

```
bash-5.0$ make; ./signal
make: Nothing to be done for 'all'.
^C
2 stop test

17 stop test

Child process2 is killed by parent!!

Parent process is killed!!
```
子进程 2 捕获信号 17，输出 17 stop test 和 Child process2 is killed by parent!! 并退出。
子进程 1 没有及时处理信号 16 或未被调度到，导致只输出了 Child process2 的信息。
父进程等待子进程结束后，输出 Parent process is killed!!。

#### 5s内不进行任何操作发送中断

```
bash-5.0$ make; ./signal
make: Nothing to be done for 'all'.

14 stop test

16 stop test

Child process1 is killed by parent!!

17 stop test

Child process2 is killed by parent!!

Parent process is killed!!
```

14 stop test：父进程收到 SIGALRM 信号并按照预期发送了信号 16 和 17 以终止子进程，程序正常完成了任务。