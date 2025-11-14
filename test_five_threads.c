#include <stdio.h>
#include <stdint.h>
#include "bthread.h"

void* simple_thread(void* arg) {
    int id = (int)(intptr_t)arg;
    printf("Thread %d starting\n", id);
    for (int i = 0; i < 3; i++) {
        bthread_yield();
    }
    printf("Thread %d done\n", id);
    return NULL;
}

int main() {
    printf("Creating 5 threads\n");
    bthread_t threads[5];
    
    for (int i = 0; i < 5; i++) {
        printf("Creating thread %d\n", i);
        bthread_create(&threads[i], NULL, simple_thread, (void*)(intptr_t)i);
    }
    
    printf("Joining threads\n");
    for (int i = 0; i < 5; i++) {
        printf("Joining thread %d\n", i);
        bthread_join(threads[i], NULL);
    }
    
    printf("All done\n");
    return 0;
}
