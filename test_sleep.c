#include <stdio.h>
#include "bthread.h"

void *thread_func(void *arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 3; i++) {
        printf("Thread %d: iteration %d\n", id, i);
        bthread_sleep(100);
    }
    return arg;
}

int main() {
    bthread_t t1;
    bthread_attr_t attr = {};
    int arg1 = 1;
    void *retval;
    
    bthread_create(&t1, &attr, thread_func, &arg1);
    bthread_join(t1, &retval);
    
    printf("Thread returned: %d\n", *(int*)retval);
    return 0;
}
