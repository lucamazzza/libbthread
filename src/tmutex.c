#include <assert.h>
#include <string.h>

#include "tmutex.h"
#include "bthread_private.h"
#include "common.h"
#include "tqueue.h"

int bthread_mutex_init(bthread_mutex_t *m, const bthread_mutexattr_t *attr) {
    UNUSED(attr);
    assert(m != NULL);
    m->owner = NULL;
    m->waiting_queue = NULL;
    return 0;
}

int bthread_mutex_destroy(bthread_mutex_t *m) {
    assert(m->owner == NULL);
    assert(tqueue_size(m->waiting_queue) == 0);
    return 0;
}

int bthread_mutex_lock(bthread_mutex_t *m) {
    bthread_begin_atomic_execution();
    __bthread_scheduler *s = bthread_get_scheduler();
    volatile __bthread_private *tp = (__bthread_private*) tqueue_get_data(s->cur);
    if (LIKELY(m->owner == NULL)) {
        m->owner = (void*) tp;
    } else {
        tp->state = __BTHREAD_BLOCKED;
        tqueue_enqueue(&m->waiting_queue, (void*) tp);
        while(tp->state != __BTHREAD_READY) bthread_yield();
    }
    bthread_end_atomic_execution();
    return 0;
}

int bthread_mutex_trylock(bthread_mutex_t *m) {
    bthread_begin_atomic_execution();
    __bthread_scheduler *s = bthread_get_scheduler();
    volatile __bthread_private *tp = (__bthread_private*) tqueue_get_data(s->cur);
    if (LIKELY(m->owner == NULL)) {
        m->owner = (void*) tp;
        bthread_end_atomic_execution();
        return 0;
    } else {
        bthread_end_atomic_execution();
        return -1; 
    }
}

int bthread_mutex_unlock(bthread_mutex_t *m) {
    bthread_begin_atomic_execution();
    assert(m->owner != NULL);
    assert(m->owner == tqueue_get_data(bthread_get_scheduler()->cur));
    __bthread_private *tp = (__bthread_private*) tqueue_pop(&m->waiting_queue);
    if (LIKELY(tp != NULL)) {
        m->owner = (void*) tp;
        tp->state = __BTHREAD_READY;
        bthread_yield();
    } else {
        m->owner = NULL;
    }
    bthread_end_atomic_execution();
    return 0;
}
