#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>

int flag = 0;

void inter_handler(int sig) {
    // TODO
}

void waiting() {
    // TODO
}

int main() {
    // TODO: 五秒之后或接收到两个信号
    pid_t pid1 = -1, pid2 = -1;
    while (pid1 == -1) pid1 = fork();

    if (pid1 > 0) { // Parent process
        while (pid2 == -1) pid2 = fork();

        if (pid2 > 0) { // Parent process
            // TODO: 父进程
            printf("\nParent process is killed!!\n");
        } else { // Child process 2
            // TODO: 子进程2
            printf("\nChild process2 is killed by parent!!\n");
            return 0;
        }
    } else { // Child process 1
        // TODO: 子进程1
        printf("\nChild process1 is killed by parent!!\n");
        return 0;
    }

    return 0;
}
