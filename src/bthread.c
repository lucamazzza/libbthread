#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "bthread.h"
#include "bthread_private.h"
#include "common.h"
#include "tqueue.h"

static double get_current_time_millis();

static UNREF int bthread_is_thread_zombie_state(bthread_t thread, void **retval) {
    int done = 0;
    bthread_begin_atomic_execution();
    __bthread_scheduler *s = bthread_get_scheduler();
    tqueue_t start = bthread_get_queue_starting_at_thread(thread);
    if (LIKELY(start)) {
        __bthread_private *tp = (__bthread_private *)tqueue_get_data(start);
        if (tp->tid == thread && tp->state == __BTHREAD_ZOMBIE) {
            if (retval) {
                *retval = tp->retval;
            }
            if (tp->stack) {
                free((void *)tp->stack);
                tp->stack = 0;
            }
            s->queue = start;
            if (s->cur == start) {
                s->cur = tqueue_at_offset(start, 1);
            }
            tqueue_pop(&s->queue);
            done = 1;
        }
    }
    bthread_end_atomic_execution();
    return done;
}

static UNREF tqueue_t bthread_get_queue_starting_at_thread(bthread_t thread) {
    __bthread_scheduler *s = bthread_get_scheduler();
    tqueue_t cur = s->queue;
    while (cur) {
        __bthread_private *tp = (__bthread_private *)tqueue_get_data(cur);
        if (tp->tid == thread) {
            return cur;
        }
        cur = tqueue_at_offset(cur, 1);
    }
    return 0;
}

static UNREF void bthread_setup_timer() {
    static bool initialized = false;
    if (UNLIKELY(!initialized)) {
        signal(SIGVTALRM, (void (*)(int))bthread_yield);
        struct itimerval time;
        time.it_interval.tv_sec = 0;
        time.it_interval.tv_usec = QUANTUM_USEC;
        time.it_value.tv_sec = 0;
        time.it_value.tv_usec = QUANTUM_USEC;
        initialized = true;
        setitimer(ITIMER_VIRTUAL, &time, NULL);
    }
}

__bthread_scheduler *bthread_get_scheduler(void) {
    static __bthread_scheduler s = {0};
    return &s;
}

void bthread_begin_atomic_execution() {
    bthread_setup_timer();
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGVTALRM);
    sigprocmask(SIG_BLOCK, &sigset, NULL);
}

void bthread_end_atomic_execution() {
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGVTALRM);
    sigprocmask(SIG_UNBLOCK, &sigset, NULL);
}

void bthread_printf(const char *format, ...) {
    bthread_begin_atomic_execution();
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    bthread_end_atomic_execution();
}

int bthread_create(bthread_t *thread, const bthread_attr_t *attr, void *(*start_routine)(void *), void *arg) {
    __bthread_scheduler *s = bthread_get_scheduler();
    __bthread_private *t = malloc(sizeof(__bthread_private));
    t->tid = ++s->cur_tid;
    t->body = start_routine;
    t->arg = arg;
    t->attr = *attr;
    t->state = __BTHREAD_READY;
    t->stack = 0;
    tqueue_enqueue(&s->queue, t);
    if (LIKELY(thread))
        *thread = t->tid;
    return 0;
}

int bthread_join(bthread_t thread, void **retval) {
    volatile __bthread_scheduler *s = bthread_get_scheduler();
    s->cur = s->queue;
    save_context(s->context);
    if (bthread_is_thread_zombie_state(thread, retval))
        return 0;
    __bthread_private *tp;
    do {
        s->cur = tqueue_at_offset(s->cur, 1);
        tp = (__bthread_private *)tqueue_get_data(s->cur);
        if (tp->state == __BTHREAD_SLEEPING) {
            if (get_current_time_millis() >= tp->wake_up_time) {
                tp->state = __BTHREAD_READY;
            }
        }
    } while (tp->state != __BTHREAD_READY);
    if (LIKELY(tp->stack))
        restore_context(tp->context);
    else {
        tp->stack = (uintptr_t)malloc(sizeof(char) * STACK_SIZE);
        uintptr_t target = tp->stack + STACK_SIZE - 1;
        target -= (target % 16);
#if __aarch64__
        asm __volatile__("mov x1, %0" ::"r"(tp) : "x1");
        asm __volatile__("mov sp, %0" ::"r"((uintptr_t)target) : "x1");
        asm __volatile__("mov %0, x1" : "=r"(tp));
#elif __arm__
        asm __volatile__("mov %%sp, %0" ::"r"((uintptr_t)target));
#elif __x86_64__
        asm __volatile__("movq %0, %%rsp" ::"r"((uintptr_t)target));
#else
        asm __volatile__("movl %0, %%esp" ::"r"((uintptr_t)target));
#endif
        bthread_exit(tp->body(tp->arg));
    }
    return 0;
}

void bthread_yield() {
    bthread_begin_atomic_execution();
    __bthread_scheduler *s = bthread_get_scheduler();
    __bthread_private *tp = (__bthread_private *)tqueue_get_data(s->cur);
    if (tp && save_context(tp->context) != 0) {
        bthread_end_atomic_execution();
        return;
    }
    restore_context(s->context);
    bthread_end_atomic_execution();
}

void bthread_exit(void *retval) {
    __bthread_scheduler *s = bthread_get_scheduler();
    __bthread_private *tp = (__bthread_private *)tqueue_get_data(s->cur);
    tp->retval = retval;
    tp->state = __BTHREAD_ZOMBIE;
    bthread_yield();
}

static double get_current_time_millis() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
}

void bthread_sleep(double ms) {
    bthread_begin_atomic_execution();
    __bthread_scheduler *s = bthread_get_scheduler();
    __bthread_private *tp = (__bthread_private *)tqueue_get_data(s->cur);
    tp->state = __BTHREAD_SLEEPING;
    tp->wake_up_time = get_current_time_millis() + ms;
    bthread_end_atomic_execution();
    bthread_yield();
}

int bthread_cancel(bthread_t thread) {
    bthread_begin_atomic_execution();
    tqueue_t start = bthread_get_queue_starting_at_thread(thread);
    if (LIKELY(start)) {
        __bthread_private *tp = (__bthread_private *)tqueue_get_data(start);
        if (tp->tid == thread) {
            tp->state = __BTHREAD_ZOMBIE;
            bthread_end_atomic_execution();
            return 0;
        }
    }
    bthread_end_atomic_execution();
    return -1;
}

void bthread_testcancel(void) {
    bthread_begin_atomic_execution();
    __bthread_scheduler *s = bthread_get_scheduler();
    __bthread_private *tp = (__bthread_private *)tqueue_get_data(s->cur);
    if (tp->state == __BTHREAD_ZOMBIE) {
        bthread_end_atomic_execution();
        bthread_yield();
    }
    bthread_end_atomic_execution();
}
