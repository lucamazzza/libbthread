#ifndef __BTHREAD_PRIVATE_H__
#define __BTHREAD_PRIVATE_H__

#include <setjmp.h>
#include <stdint.h>

#include "bthread.h"
#include "tqueue.h"

#define save_context(CONTEXT)    sigsetjmp(CONTEXT, 1)
#define restore_context(CONTEXT) siglongjmp(CONTEXT, 1)

/* Contains all the information regarding the thread:
 *  - an identifier
 *  - the body routine and its arguments
 *  - the execution state
 *  - attributes
 *  - stack content
 *  - return value */
typedef struct {
    bthread_t       tid;
    bthread_routine body;
    void           *arg;
    bthread_state   state;
    bthread_attr_t  attr;
    uintptr_t       stack;
    jmp_buf         context;
    void           *retval;
} __bthread_private;
/* Responsible for initializing and scheduling threads, according to some scheduling policy.
 * The list of threads is stored in a `tqueue_t`, `cur` refers to the currently executing thread.
 */
typedef struct {
    tqueue_t        queue;
    tqueue_t        cur;
    jmp_buf         context;
    bthread_t       cur_tid;
} __bthread_scheduler;
typedef struct {
    void           *owner;
    tqueue_t        queue;
} bthread_mutex_t;

/* Creates, mantains and returns a static pointer to a singleton instance of `__bthread_scheduler_private`. */
__bthread_scheduler *bthread_get_scheduler();
/* Checks wether the thread reference in `thread` has reached `__BTHREAD_ZOMBIE` state.
 * If not (i.e. the thread is still running) the function returns `0`.
 * Otherwise the following steps are performed:
 *  - if `retval` is not `NULL` the exit status of the thread is copied in its pointed location
 *  - the thread's stack is freed and the thread removed from the scheduler
 *  - the function returns `1`. */
static int           bthread_is_thread_zombie_state(bthread_t thread, void **retval);
/* Returns a "view" on the queue beginning at the node containing the data for the thread identified by `thread`
 * If the queue is empty or doesn't contain the data, the function returns `NULL`. */
static tqueue_t      bthread_get_queue_starting_at_thread(bthread_t thread);
void                 bthread_begin_atomic_execution(void);
void                 bthread_end_atomic_execution(void);
void                 bthread_setup_timer(void);

#endif // __BTHREAD_PRIVATE_H__
