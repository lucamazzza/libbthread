#include <stdio.h>
#include <stdint.h>
#include "bthread.h"

void* cancellable_thread(void* arg) {
    int* counter = (int*)arg;
    for (int i = 0; i < 1000; i++) {
        (*counter)++;
        bthread_testcancel();
        bthread_yield();
    }
    return (void*)(intptr_t)999;
}

int main() {
    printf("Testing cancellation\n");
    bthread_t t;
    int counter = 0;
    
    bthread_create(&t, NULL, cancellable_thread, &counter);
    
    // Let it run a bit
    bthread_sleep(100);
    
    printf("Counter before cancel: %d\n", counter);
    
    // Cancel it
    bthread_cancel(t);
    
    void* retval;
    bthread_join(t, &retval);
    
    printf("Counter after cancel: %d\n", counter);
    printf("Return value: %ld (should be -1)\n", (intptr_t)retval);
    
    if ((intptr_t)retval == -1 && counter < 1000) {
        printf("SUCCESS: Thread was cancelled\n");
        return 0;
    } else {
        printf("FAILED: Thread was not cancelled properly\n");
        return 1;
    }
}
