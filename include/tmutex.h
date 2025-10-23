#ifndef __TMUTEX_H__
#define __TMUTEX_H__

#include "tqueue.h"

/* Defines mutex structure 
 * A mutex has an owner and a waiting queue.
 */
typedef struct {
    void *owner;
    tqueue_t waiting_queue;
} bthread_mutex_t;
/* Defines mutex attributes structure. Currently not used, only for PThread compatibility */
typedef struct {
    char nop;
} bthread_mutexattr_t;
/* Initialize the mutex and its attributes.
 * The attributes parameter is currently unused, and kept for compatibility with PThread.
 */
int bthread_mutex_init(bthread_mutex_t *m, const bthread_mutexattr_t *attr);
/* Destroy the mutex, freeing any resources associated with it. */
int bthread_mutex_destroy(bthread_mutex_t *m);
/* Lock the mutex, blocking if necessary until it becomes available. */
int bthread_mutex_lock(bthread_mutex_t *m);
/* Try to lock the mutex, returning immediately if it is not available. */
int bthread_mutex_trylock(bthread_mutex_t *m);
/* Unlock the mutex, releasing it for other threads to acquire. */
int bthread_mutex_unlock(bthread_mutex_t *m);

#endif // __TMUTEX_H__
