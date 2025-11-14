#include <stdio.h>
#include <stdint.h>
#include "bthread.h"

void* cancellable_thread(void* arg) {
    int* counter = (int*)arg;
    for (int i = 0; i < 100; i++) {
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
    bthread_cancel(t);
    
    void* retval;
    bthread_join(t, &retval);
    
    printf("Counter: %d\n", counter);
    printf("Return value: %ld\n", (intptr_t)retval);
    
    if ((intptr_t)retval == -1) {
        printf("SUCCESS\n");
        return 0;
    } else {
        printf("FAILED\n");
        return 1;
    }
}
