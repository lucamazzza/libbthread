#include <stdio.h>
#include "bthread.h"
#include "tmutex.h"
#include "tsemaphore.h"
#include "tcondition.h"
#include "tbarrier.h"

void *worker_thread(void *arg) {
    int id = *(int *)arg;
    printf("Thread %d: Starting\n", id);
    bthread_yield();
    printf("Thread %d: Finishing\n", id);
    return arg;
}

int main() {
    printf("=== LibBThread Demo ===\n\n");
    
    bthread_t thread1, thread2;
    bthread_attr_t attr = {};
    int arg1 = 1, arg2 = 2;
    void *retval;
    
    printf("Creating threads...\n");
    bthread_create(&thread1, &attr, worker_thread, &arg1);
    bthread_create(&thread2, &attr, worker_thread, &arg2);
    
    printf("Joining thread 1...\n");
    bthread_join(thread1, &retval);
    printf("Thread 1 completed with return value: %d\n", *(int *)retval);
    
    printf("Joining thread 2...\n");
    bthread_join(thread2, &retval);
    printf("Thread 2 completed with return value: %d\n", *(int *)retval);
    
    printf("\nDemo completed successfully!\n");
    
    return 0;
}
