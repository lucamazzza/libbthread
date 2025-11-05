/*
 * File:        tsemaphore.h
 * Author:      Luca Mazza
 * Description: Thread semaphore implementation for the basic user-level threading library.
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

#ifndef __TSEMAPHORE_H__
#define __TSEMAPHORE_H__

#include "tqueue.h"

#define bthread_sem_up(s)   bthread_sem_post(s)
#define bthread_sem_down(s) bthread_sem_wait(s)

/* Defines semaphore structure 
 * A semaphore has a value and a waiting queue. */
typedef struct {
    int value;
    tqueue_t waiting_queue;
} bthread_sem_t;

/* Initialize a semaphore */
int bthread_sem_init(bthread_sem_t *sem, int pshared, unsigned int value);
/* Destroy a semaphore */
int bthread_sem_destroy(bthread_sem_t *sem);
/* Wait (decrement) a semaphore */
int bthread_sem_wait(bthread_sem_t *sem);
/* Post (increment) a semaphore */
int bthread_sem_post(bthread_sem_t *sem);

#endif // __TSEMAPHORE_H__
