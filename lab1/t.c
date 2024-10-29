#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int shared_var = 0;
pthread_mutex_t lock;

void *increment(void *param) {
    for (int i = 0; i < 5000; i++) {
        pthread_mutex_lock(&lock);
        shared_var++;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

void *decrement(void *param) {
    printf("shared_var before decrement begins = %d\n", shared_var);
    for (int i = 0; i < 5000; i++) {
        pthread_mutex_lock(&lock);
        shared_var--;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main() {
    pthread_t tid1, tid2;

    pthread_mutex_init(&lock, NULL);

    pthread_create(&tid1, NULL, increment, NULL);
    pthread_create(&tid2, NULL, decrement, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    printf("Final value of shared_var = %d\n", shared_var);

    pthread_mutex_destroy(&lock);

    return 0;
}
