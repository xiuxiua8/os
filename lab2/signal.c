#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>

int flag = 0;

void inter_handler(int sig) {
    if (sig == SIGINT || sig == SIGQUIT || sig == SIGALRM) {
        flag = sig;
    }
    if (sig == 16) {
        printf("\n%d stop test\n",sig);
        printf("\nChild process1 is killed by parent!!\n");
        exit(0);
    } else if (sig == 17) {
        printf("\n%d stop test\n",sig);
        printf("\nChild process2 is killed by parent!!\n");
        exit(0);
    }

}

void waiting() {
    pause(); // 等待信号
}

int main() {
    signal(SIGINT, inter_handler);
    signal(SIGQUIT, inter_handler);
    signal(SIGALRM, inter_handler);
    alarm(5);
    pid_t pid1 = -1, pid2 = -1;

    while (pid1 == -1) pid1 = fork();
    if (pid1 > 0) { // Parent process
        while (pid2 == -1) pid2 = fork();

        if (pid2 > 0) { // Parent process
            waiting();
            printf("\n%d stop test\n",flag);
            kill(pid1, 16);
            kill(pid2, 17);

            wait(NULL);
            wait(NULL);

            printf("\nParent process is killed!!\n");
        } else { // Child process 2

            signal(16, inter_handler);
            signal(17, inter_handler);
            pause();
        }
    } else { // Child process 1

        signal(16, inter_handler);
        signal(17, inter_handler);
        pause();    
    }

    return 0;
}
