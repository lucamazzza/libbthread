/*
 * File:        tqueue.h
 * Author:      Luca Mazza
 * Description: Thread queue implementation for the basic user-level threading library.
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

#ifndef __TQUEUE_H__
#define __TQUEUE_H__

/* Opaque structure representing a single queue node */
struct tqueue_node;
/* A queue is represented as the pointer to the first node */
typedef struct tqueue_node* tqueue_t;

/* Adds a new element at the end of the queue. Returns its position.*/
unsigned long int   tqueue_enqueue(tqueue_t* q, void* data);
/* Removes the first element of the queue and returns its data. Returns NULL if the queue is empty.*/
void               *tqueue_pop(tqueue_t* q);
/* Returns the number of elements in the queue.*/
unsigned long int   tqueue_size(tqueue_t q);
/* Frees the queue and all its elements. */
tqueue_t            tqueue_at_offset(tqueue_t q, unsigned long int offset);
/* Returns the data of the element at the front of the queue without removing it. Returns NULL if the queue is empty.*/
void               *tqueue_get_data(tqueue_t q);  

#endif // __TQUEUE_H__
