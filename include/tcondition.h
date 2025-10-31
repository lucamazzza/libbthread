#ifndef __TCONDITION_H__
#define __TCONDITION_H__

#include "tqueue.h"
#include "tmutex.h"

#define bthread_cond_notify(s)      bthread_cond_signal(s)
#define bthread_cond_notify_all(s)  bthread_cond_broadcast(s)

typedef struct {
    tqueue_t waiting_queue;
} bthread_cond_t;

typedef struct {} bthread_condattr_t;

/* Initialize a condition variable */
int bthread_cond_init(bthread_cond_t *cond, const bthread_condattr_t *attr);
/* Destroy a condition variable */
int bthread_cond_destroy(bthread_cond_t *cond);
/* Wait on a condition variable */
int bthread_cond_wait(bthread_cond_t *cond, bthread_mutex_t *mutex);
/* Signal a condition variable */
int bthread_cond_signal(bthread_cond_t *cond);
/* Broadcast to all waiting threads on a condition variable */
int bthread_cond_broadcast(bthread_cond_t *cond);

#endif // __TCONDITION_H__
