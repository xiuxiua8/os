#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
int shared_var = 0;
pthread_mutex_t lock;

void *increment(void *param) {
    for (int i = 0; i < 5000; i++) {
        pthread_mutex_lock(&lock);
        shared_var++;
        pthread_mutex_unlock(&lock);
    }
    pid_t pid = getpid();               
    pid_t tid = syscall(SYS_gettid);     

    printf("thread itself PID: %d, TID: %d\n", pid, tid);

    printf("increment calling system\n");
    system("./call2");  // 使用 system 调用自定义程序
    return NULL;
}

void *decrement(void *param) {
    for (int i = 0; i < 5000; i++) {
        pthread_mutex_lock(&lock);
        shared_var--;
        pthread_mutex_unlock(&lock);
    }
    pid_t pid = getpid();               
    pid_t tid = syscall(SYS_gettid);     

    printf("thread itself PID: %d, TID: %d\n", pid, tid);

    printf("decrement calling system\n");
    system("./call2");  // 使用 system 调用自定义程序
    
    printf("decrement calling exec\n");
    execl("./call2", "call2", NULL);  // 使用 exec 调用自定义程序
    perror("exec failed");
    return NULL;
}

int main() {
    pthread_t tid1, tid2;
    pid_t pid = getpid();               
    pid_t tid = syscall(SYS_gettid);     

    printf("main   itself PID: %d, TID: %d\n", pid, tid);

    pthread_mutex_init(&lock, NULL);

    pthread_create(&tid1, NULL, increment, NULL);
    pthread_create(&tid2, NULL, decrement, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    printf("Final value of shared_var = %d\n", shared_var);

    pthread_mutex_destroy(&lock);

    return 0;
}
