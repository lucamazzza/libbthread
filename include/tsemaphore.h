#ifndef __TSEMAPHORE_H__
#define __TSEMAPHORE_H__

#include "tqueue.h"

#define bthread_sem_up(s)   bthread_sem_post(s)
#define bthread_sem_down(s) bthread_sem_wait(s)

typedef struct {
    int value;
    tqueue_t waiting_queue;
} bthread_sem_t;

/* Initialize a semaphore */
int bthread_sem_init(bthread_sem_t *sem, int pshared, unsigned int value);
/**/
int bthread_sem_destroy(bthread_sem_t *sem);
/**/
int bthread_sem_wait(bthread_sem_t *sem);
/**/
int bthread_sem_post(bthread_sem_t *sem);

#endif // __TSEMAPHORE_H__
