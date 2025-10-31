#include <assert.h>
#include <stdlib.h>

#include "tcondition.h"
#include "common.h"
#include "bthread_private.h"

int bthread_cond_init(bthread_cond_t *cond, const bthread_condattr_t *attr) {
    UNUSED(attr);
    assert(cond != NULL);
    cond->waiting_queue = NULL;
    return 0;
}

int bthread_cond_destroy(bthread_cond_t *cond) {
    assert(tqueue_size(cond->waiting_queue) == 0);
    return 0;
}

int bthread_cond_wait(bthread_cond_t *cond, bthread_mutex_t *mutex) {
    bthread_begin_atomic_execution();
    __bthread_scheduler *s = bthread_get_scheduler();
    volatile __bthread_private *tp = (__bthread_private *)tqueue_get_data(s->cur);
    tp->state = __BTHREAD_BLOCKED;
    tqueue_enqueue(&cond->waiting_queue, (void *)tp);
    bthread_mutex_unlock(mutex);
    while (tp->state != __BTHREAD_READY)
        bthread_yield();
    bthread_mutex_lock(mutex);
    bthread_end_atomic_execution();
    return 0;
}

int bthread_cond_signal(bthread_cond_t *cond) {
    bthread_begin_atomic_execution();
    __bthread_private *tp = (__bthread_private *)tqueue_pop(&cond->waiting_queue);
    if (LIKELY(tp != NULL)) {
        tp->state = __BTHREAD_READY;
        bthread_yield();
    }
    bthread_end_atomic_execution();
    return 0;
}

int bthread_cond_broadcast(bthread_cond_t *cond) {
    bthread_begin_atomic_execution();
    __bthread_private *tp;
    while ((tp = (__bthread_private *)tqueue_pop(&cond->waiting_queue)) != NULL) {
        tp->state = __BTHREAD_READY;
    }
    bthread_yield();
    bthread_end_atomic_execution();
    return 0;
}
