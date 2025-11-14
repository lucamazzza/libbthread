#include <stdio.h>
#include "bthread.h"
#include "tsemaphore.h"

void* test_thread(void* arg) {
    printf("Thread running\n");
    return NULL;
}

int main() {
    printf("Starting test\n");
    
    bthread_sem_t sem;
    printf("Init semaphore\n");
    bthread_sem_init(&sem, 0, 0);
    
    bthread_t t;
    printf("Create thread\n");
    bthread_create(&t, NULL, test_thread, NULL);
    
    printf("Join thread\n");
    bthread_join(t, NULL);
    
    printf("Destroy semaphore\n");
    bthread_sem_destroy(&sem);
    
    printf("Done\n");
    return 0;
}
