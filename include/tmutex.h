/*
 * File:        tmutex.h
 * Author:      Luca Mazza
 * Description: Thread mutex implementation for the basic user-level threading library.
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

#ifndef __TMUTEX_H__
#define __TMUTEX_H__

#include "tqueue.h"

/* Defines mutex structure 
 * A mutex has an owner and a waiting queue.*/
typedef struct {
    void *owner;
    tqueue_t waiting_queue;
} bthread_mutex_t;
/* Defines mutex attributes structure. Currently not used, only for PThread compatibility */
typedef struct {
    char nop;
} bthread_mutexattr_t;

/* Initialize the mutex and its attributes.
 * The attributes parameter is currently unused, and kept for compatibility with PThread.*/
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
