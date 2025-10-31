#ifndef __TBARRIER_H__
#define __TBARRIER_H__

#include "tqueue.h"

typedef struct {
    unsigned int count;
    unsigned int size;
    tqueue_t waiting_queue;
} bthread_barrier_t;

typedef struct {} bthread_barrierattr_t;

/* Initialize a barrier */
int bthread_barrier_init(bthread_barrier_t *bar, const bthread_barrierattr_t *attr, unsigned count);
/* Destroy a barrier */
int bthread_barrier_destroy(bthread_barrier_t *bar);
/* Wait at the barrier */
int bthread_barrier_wait(bthread_barrier_t *bar);

#endif // __TBARRIER_H__
