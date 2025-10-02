/* Copyright (C) 2025 Luca Mazza, Roeld Hoxha, Vasco Silva Pereira
 *
 */

#ifndef _BTHREADTYPES_H
#define _BTHREADTYPES_H

#include <setjmp.h>
#include <stdint.h>

typedef unsigned long int bthread_t;

typedef enum {
    __BTHREAD_READY = 0,
    __BTHREAD_ZOMBIE,
    __BTHREAD_BLOCKED,
    __BTHREAD_SLEEPING,
} bthread_state;

typedef struct {
} bthread_attr_t;

typedef void *(*bthread_routine)(void *);

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

#endif
