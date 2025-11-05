/*
 * File:        tbarrier.h
 * Author:      Luca Mazza
 * Description: Thread barrier implementation for the basic user-level threading library.
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

#ifndef __TBARRIER_H__
#define __TBARRIER_H__

#include "tqueue.h"

/* Defines barrier structure 
 * A barrier has a count, size and a waiting queue. */
typedef struct {
    unsigned int count;
    unsigned int size;
    tqueue_t waiting_queue;
} bthread_barrier_t;
/* Defines barrier attributes structure. Currently not used, only for PThread compatibility */
typedef struct {} bthread_barrierattr_t;

/* Initialize a barrier */
int bthread_barrier_init(bthread_barrier_t *bar, const bthread_barrierattr_t *attr, unsigned count);
/* Destroy a barrier */
int bthread_barrier_destroy(bthread_barrier_t *bar);
/* Wait at the barrier */
int bthread_barrier_wait(bthread_barrier_t *bar);

#endif // __TBARRIER_H__
