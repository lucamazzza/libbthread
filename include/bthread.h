/*
 * File:        bthread.h
 * Author:      Luca Mazza
 * Description: Basic user-level threading library interface.
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

#ifndef __BTHREAD_H__
#define __BTHREAD_H__

/* A thread, represented by its thread identifier */
typedef unsigned long int bthread_t;
/* Thread states options */
typedef enum {
  __BTHREAD_READY = 0,
  __BTHREAD_ZOMBIE,
  __BTHREAD_BLOCKED,
  __BTHREAD_SLEEPING,
} bthread_state;
/* Attributes passed to the thread function upon execution. */
typedef struct {
} bthread_attr_t;
typedef void *(*bthread_routine)(void *);

/* Creates a new thread structure and puts it at the end off the queue. 
 * The Thread Identifier corresponds to the position in the queue. 
 *
 * The thread is not started once this is called.
 * Attributes passsed through the `attr` argument are ignored (`NULL` pointer possible)
 * The stack pointer for the thread's stack is set to `NULL` */
int bthread_create(bthread_t *thread, const bthread_attr_t *attr, void *(*start_routine)(void *), void *arg);
/* Waits for a specific thread to terminate (i.e. `__BTHREAD_ZOMBIE` state), by scheduling all the threads. */
int bthread_join(bthread_t thread, void **retval);
/* Saves the context and restores the scheduler context */
void bthread_yield();
/* Terminates the calling thread and returns a value via `retval`, that will be available to another thread 
 * in the same process, that calls `bthread_join`, then yields to the scheduler. */
void bthread_exit(void *retval);
/* Puts the thread in `__BTHREAD_SLEEPING` status for a determined amount of milliseconds. */
void bthread_sleep(double ms);
/* Requests the cancellation of a thread during its execution time.
 * The cancellation happens as soon as the thread receiving the request calls `bthread_testcancel()`. */
int bthread_cancel(bthread_t thread);
/* Cancellation point marker. When called the executing thread calls upon cancellation (if requested). */
void bthread_testcancel(void);

#endif // __BTHREAD_H__
