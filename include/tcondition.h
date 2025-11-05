/*
 * File:        tcondition.h
 * Author:      Luca Mazza
 * Description: Thread condition variable implementation for the basic user-level threading library.
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

#ifndef __TCONDITION_H__
#define __TCONDITION_H__

#include "tqueue.h"
#include "tmutex.h"

#define bthread_cond_notify(s)      bthread_cond_signal(s)
#define bthread_cond_notify_all(s)  bthread_cond_broadcast(s)

/* Defines condition variable structure 
 * A condition variable has a waiting queue. */
typedef struct {
    tqueue_t waiting_queue;
} bthread_cond_t;
/* Defines condition variable attributes structure. Currently not used, only for PThread compatibility */
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
