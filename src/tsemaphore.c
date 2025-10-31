#include <assert.h>
#include <stdlib.h>

#include "tsemaphore.h"
#include "bthread_private.h"
#include "common.h"

int bthread_sem_init(bthread_sem_t *sem, int pshared, unsigned int value) {
    UNUSED(pshared);
    assert(sem != NULL);
    sem->value = (int)value;
    sem->waiting_queue = NULL;
    return 0;
}

int bthread_sem_destroy(bthread_sem_t *sem) {
    assert(sem->value >= 0);
    assert(tqueue_size(sem->waiting_queue) == 0);
    return 0;
}

int bthread_sem_wait(bthread_sem_t *sem) {
    bthread_begin_atomic_execution();
    __bthread_scheduler *s = bthread_get_scheduler();
    volatile __bthread_private *tp = (__bthread_private *)tqueue_get_data(s->cur);
    if (LIKELY(sem->value > 0)) {
        sem->value--;
    } else {
        tp->state = __BTHREAD_BLOCKED;
        tqueue_enqueue(&sem->waiting_queue, (void *)tp);
        while (tp->state != __BTHREAD_READY)
            bthread_yield();
    }
    bthread_end_atomic_execution();
    return 0;
}

int bthread_sem_post(bthread_sem_t *sem) {
    bthread_begin_atomic_execution();
    __bthread_private *tp = (__bthread_private *)tqueue_pop(&sem->waiting_queue);
    if (LIKELY(tp != NULL)) {
        tp->state = __BTHREAD_READY;
        bthread_yield();
    } else {
        sem->value++;
    }
    bthread_end_atomic_execution();
    return 0;
}
