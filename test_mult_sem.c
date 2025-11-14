#include <stdio.h>
#include "bthread.h"
#include "tsemaphore.h"
#include "tmutex.h"

#define N 5

int main() {
    printf("Init %d semaphores\n", N);
    bthread_sem_t sem[N];
    for (int i = 0; i < N; i++) {
        printf("  Init sem %d\n", i);
        bthread_sem_init(&sem[i], 0, 0);
    }
    
    printf("Init mutex\n");
    bthread_mutex_t mutex;
    bthread_mutex_init(&mutex, NULL);
    
    printf("Cleanup\n");
    for (int i = 0; i < N; i++) {
        bthread_sem_destroy(&sem[i]);
    }
    bthread_mutex_destroy(&mutex);
    
    printf("Done\n");
    return 0;
}
