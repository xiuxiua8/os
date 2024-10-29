#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <pthread.h>

int main() {
    pid_t pid = getpid();               
    pid_t tid = syscall(SYS_gettid);     

    printf("call Program PID: %d, TID: %d\n", pid, tid);

    return 0;
}
