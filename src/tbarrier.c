#include <assert.h>
#include <stdlib.h>

#include "tbarrier.h"
#include "common.h"
#include "bthread_private.h"

int bthread_barrier_init(bthread_barrier_t *bar, const bthread_barrierattr_t *attr, unsigned count) {
    UNUSED(attr);
    assert(bar != NULL);
    bar->count = count;
    bar->size = 0;
    bar->waiting_queue = NULL;
    return 0;
}

int bthread_barrier_destroy(bthread_barrier_t *bar) {
    assert(bar->size == 0);
    assert(tqueue_size(bar->waiting_queue) == 0);
    return 0;
}

int bthread_barrier_wait(bthread_barrier_t *bar) {
    bthread_begin_atomic_execution();
    __bthread_scheduler *s = bthread_get_scheduler();
    volatile __bthread_private *tp = (__bthread_private *)tqueue_get_data(s->cur);
    bar->size++;
    if (bar->size < bar->count) {
        tp->state = __BTHREAD_BLOCKED;
        tqueue_enqueue(&bar->waiting_queue, (void *)tp);
        while (tp->state != __BTHREAD_READY)
            bthread_yield();
    } else {
        __bthread_private *wtp;
        while ((wtp = (__bthread_private *)tqueue_pop(&bar->waiting_queue)) != NULL) {
            wtp->state = __BTHREAD_READY;
        }
        bar->size = 0;
        bthread_yield();
    }
    bthread_end_atomic_execution();
    return 0;
}
