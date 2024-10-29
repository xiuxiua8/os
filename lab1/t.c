#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int shared_var = 0;
pthread_mutex_t lock;
pthread_cond_t cond;
int turn = 0;  //0 increment 1 decrement 

void *increment(void *param) {
    for (int i = 0; i < 5000; i++) {
        pthread_mutex_lock(&lock);

        while (turn != 0) {  
            pthread_cond_wait(&cond, &lock);
        }

        shared_var++;
        printf("Increment: shared_var = %d\n", shared_var);

        turn = 1;  
        pthread_cond_signal(&cond);  

        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

void *decrement(void *param) {
    for (int i = 0; i < 5000; i++) {
        pthread_mutex_lock(&lock);

        while (turn != 1) { 
            pthread_cond_wait(&cond, &lock);
        }

        shared_var--;
        printf("Decrement: shared_var = %d\n", shared_var);

        turn = 0; 
        pthread_cond_signal(&cond);  

        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main() {
    pthread_t tid1, tid2;

    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_create(&tid1, NULL, increment, NULL);
    pthread_create(&tid2, NULL, decrement, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    printf("Final value of shared_var = %d\n", shared_var);

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);

    return 0;
}
