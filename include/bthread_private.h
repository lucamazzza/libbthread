/*
 * File:        bthread_private.h
 * Author:      Luca Mazza
 * Description: Private definitions for the basic user-level threading library.
 * License:     MIT
 *
 * Copyright (c) 2025 Luca Mazza
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef __BTHREAD_PRIVATE_H__
#define __BTHREAD_PRIVATE_H__

#include <setjmp.h>
#include <stdint.h>

#include "bthread.h"
#include "tqueue.h"
#include "common.h"

#define STACK_SIZE               (64 * 1024)
#define QUANTUM_USEC             50000  // 50 milliseconds
#define save_context(CONTEXT)    sigsetjmp(CONTEXT, 1)
#define restore_context(CONTEXT) siglongjmp(CONTEXT, 1)

/* Contains all the information regarding the thread:
 *  - an identifier
 *  - the body routine and its arguments
 *  - the execution state
 *  - attributes
 *  - stack content
 *  - return value
 *  - cancellation request flag */
typedef struct {
    bthread_t       tid;
    bthread_routine body;
    void           *arg;
    bthread_state   state;
    bthread_attr_t  attr;
    uintptr_t       stack;
    sigjmp_buf      context;
    void           *retval;
    double          wake_up_time;
    int             cancel_req;
} __bthread_private;
/* Responsible for initializing and scheduling threads, according to some scheduling policy.
 * The list of threads is stored in a `tqueue_t`, `cur` refers to the currently executing thread.
 */
typedef struct {
    tqueue_t        queue;
    tqueue_t        cur;
    sigjmp_buf      context;
    bthread_t       cur_tid;
} __bthread_scheduler;

/* Checks wether the thread reference in `thread` has reached `__BTHREAD_ZOMBIE` state.
 * If not (i.e. the thread is still running) the function returns `0`.
 * Otherwise the following steps are performed:
 *  - if `retval` is not `NULL` the exit status of the thread is copied in its pointed location
 *  - the thread's stack is freed and the thread removed from the scheduler
 *  - the function returns `1`. 
 */
static UNREF int        bthread_is_thread_zombie_state(bthread_t thread, void **retval);
/* Returns a "view" on the queue beginning at the node containing the data for the thread identified by `thread`
 * If the queue is empty or doesn't contain the data, the function returns `NULL`. */
static UNREF tqueue_t   bthread_get_queue_starting_at_thread(bthread_t thread);
/* Sets up the timer to deliver `SIGALRM` signals at regular intervals defined by `QUANTUM_USEC`,
 * to allow preemptive scheduling of threads. */
static UNREF void       bthread_setup_timer(void);
/* Creates, mantains and returns a static pointer to a singleton instance of `__bthread_scheduler_private`. */
__bthread_scheduler    *bthread_get_scheduler();
/* Sets up the stack and context for the thread identified by `thread`, so that when scheduled it starts executing
 * its body routine with the provided argument. */
void                    bthread_begin_atomic_execution(void);
/* Cleans up after the atomic execution of a thread's body routine has ended. */
void                    bthread_end_atomic_execution(void);
/* A printf-like function that outputs to stdout, used for debugging purposes inside the bthread library. */
void                    bthread_printf(const char *format, ...);

#endif // __BTHREAD_PRIVATE_H__
