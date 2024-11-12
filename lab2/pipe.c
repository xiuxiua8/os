#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int pid1, pid2; // 定义两个进程变量

int main() {
    int fd[2];
    char InPipe[5000]; 
    char c1 = '1', c2 = '2';
    pipe(fd); 
    
    while ((pid1 = fork()) == -1); 
    if (pid1 == 0) { //子进程1
        lockf(fd[1], 1, 0);
        for (int i=0; i < 2000; i++) {
            write(fd[1], &c1, 1);
        }
        sleep(5); 
        lockf(fd[1], 0, 0);
        exit(0); 
    } else {
        while ((pid2 = fork()) == -1);
        if (pid2 == 0) { //子进程2
            lockf(fd[1], 1, 0);
            for (int i=0; i < 2000; i++) {
                write(fd[1], &c2, 1);
            }
            sleep(5);
            lockf(fd[1], 0, 0);
            exit(0);
        } else {   // 父进程
            wait(0);
            wait(0);
            int end = read(fd[0], &InPipe, 4000);
            InPipe[end] = '\0';
            printf("%s\n", InPipe); 
            exit(0); 
        }
    }
}
