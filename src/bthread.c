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
    bthread_begin_atomic_execution();
    __bthread_scheduler *s = bthread_get_scheduler();
    if (UNLIKELY(!s->queue)) {
        bthread_end_atomic_execution();
        return 1;
    }
    tqueue_t start = bthread_get_queue_starting_at_thread(thread);
    if (UNLIKELY(!start)) {
        bthread_end_atomic_execution();
        return 1;
    }
    __bthread_private *tp = (__bthread_private *)tqueue_get_data(start);
    if (tp->tid == thread && tp->state == __BTHREAD_ZOMBIE) {
        if (retval != NULL) {
            *retval = tp->retval;
        }
        tqueue_t head = bthread_get_queue_starting_at_thread(thread);
        free((void *)tp->stack);
        free(tp);
        s->queue = s->cur = head;
        bthread_end_atomic_execution();
        return 1;
    }
    bthread_end_atomic_execution();
    return 0;
}

static UNREF tqueue_t bthread_get_queue_starting_at_thread(bthread_t thread) {
    volatile __bthread_scheduler *s = bthread_get_scheduler();
    volatile tqueue_t cur = s->queue;
    if (UNLIKELY(cur == NULL)) return NULL;
    while (cur) {
        __bthread_private *tp = (__bthread_private *)tqueue_get_data(cur);
        if (tp->tid == thread) {
            return cur;
        }
        cur = tqueue_at_offset(cur, 1);
    }
    return NULL;
}

static UNREF void bthread_setup_timer() {
    static bool initialized = false;
    if (UNLIKELY(!initialized)) {
        signal(SIGVTALRM, (void (*)())bthread_yield);
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
    static __bthread_scheduler* s;
    if (UNLIKELY(s == NULL)) {
        s = (__bthread_scheduler*)malloc(sizeof(__bthread_scheduler));
        sigemptyset(&s->timer_sig_mask);
        sigaddset(&s->timer_sig_mask, SIGVTALRM);
        s->queue = NULL;
        s->scheduling_routine = NULL;
        s->cur = NULL;
        s->cur_tid = 0;
    }
    return s;
}

void bthread_begin_atomic_execution() {
    __bthread_scheduler *s = bthread_get_scheduler();
    sigprocmask(SIG_BLOCK, &s->timer_sig_mask, NULL);
}

void bthread_end_atomic_execution() {
    __bthread_scheduler *s = bthread_get_scheduler();
    sigprocmask(SIG_UNBLOCK, &s->timer_sig_mask, NULL);
}

void bthread_set_scheduling_routine(bthread_scheduling_routine routine) {
    __bthread_scheduler *s = bthread_get_scheduler();
    s->scheduling_routine = routine;
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
    bthread_begin_atomic_execution();
    volatile __bthread_scheduler* scheduler = bthread_get_scheduler();
    volatile __bthread_private* bthread_priv = (__bthread_private*) malloc(sizeof(__bthread_private));
    bthread_priv->body = start_routine;
    bthread_priv->arg = arg;
    if (attr != NULL) bthread_priv->attr = *attr;
    bthread_priv->stack = (uintptr_t) NULL;
    bthread_priv->cancel_req = 0;
    bthread_priv->wake_up_time = 0;
    bthread_priv->tid = ++scheduler->cur_tid;
    tqueue_enqueue(&scheduler->queue, bthread_priv);
    bthread_priv->state = __BTHREAD_READY;
    *thread = bthread_priv->tid;
    return 0;
}

int bthread_join(bthread_t thread, void **retval) {
 bthread_begin_atomic_execution();
    volatile __bthread_scheduler* scheduler = bthread_get_scheduler();
    scheduler->cur = scheduler->queue;
    save_context(scheduler->context);
    if (bthread_is_thread_zombie_state(thread, retval)) return 0;
    bthread_setup_timer();
    __bthread_private* tp;
    do {
        if (scheduler->scheduling_routine != NULL) scheduler->scheduling_routine();
        else scheduler->cur = tqueue_at_offset(scheduler->cur, 1);
        tp = (__bthread_private*) tqueue_get_data(scheduler->cur);
        double tms = get_current_time_millis();
        if (tp->state == __BTHREAD_SLEEPING) {
            if (tp->wake_up_time < tms) tp->state = __BTHREAD_READY;
        }
    } while (tp->state != __BTHREAD_READY);
    if (tp->stack) {
        restore_context(tp->context);
    } else {
        tp->stack = (uintptr_t) malloc(sizeof(char) * STACK_SIZE);
        uintptr_t target = tp->stack + STACK_SIZE - 1;
        target -= (target % 16);
#if __aarch64__ 
        asm __volatile__("mov %%sp, %0" :: "r"((uintptr_t) target));    
#elif __x86_64__
        asm __volatile__("movq %0, %%rsp" :: "r"((uintptr_t) target));
#else
        asm __volatile__("movl %0, %%esp" :: "r"((uintptr_t) target));
#endif
        bthread_end_atomic_execution();
        bthread_exit(tp->body(tp->arg));
        return -1; 
    }

}

void bthread_yield() {
    bthread_begin_atomic_execution();
    volatile __bthread_scheduler *s = bthread_get_scheduler();
    volatile __bthread_private *tp = (__bthread_private *)tqueue_get_data(s->cur);
    if (tp && save_context(tp->context) == 0) {
        restore_context(s->context);
    }
    bthread_end_atomic_execution();
}

void bthread_exit(void *retval) {
    bthread_begin_atomic_execution();
    volatile __bthread_scheduler *s = bthread_get_scheduler();
    volatile __bthread_private *tp = (__bthread_private *)tqueue_get_data(s->cur);
    tp->retval = retval;
    tp->state = __BTHREAD_ZOMBIE;
    bthread_yield();
}

static double get_current_time_millis() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000.0;
}

void bthread_sleep(double ms) {
    bthread_begin_atomic_execution();
    __bthread_scheduler *s = bthread_get_scheduler();
    __bthread_private *tp = (__bthread_private *)tqueue_get_data(s->cur);
    tp->state = __BTHREAD_SLEEPING;
    tp->wake_up_time = get_current_time_millis() + ms;
    bthread_end_atomic_execution();
    while (tp->state != __BTHREAD_READY)
        bthread_yield();
}

int bthread_cancel(bthread_t thread) {
    tqueue_t q = bthread_get_queue_starting_at_thread(thread);
    if (q != NULL) {
        ((__bthread_private*) tqueue_get_data(q))->cancel_req = 1;
        return 0;
    } else return -1;
}

void bthread_testcancel(void) {
    volatile __bthread_scheduler* scheduler = bthread_get_scheduler();
    volatile __bthread_private* thread = (__bthread_private*) tqueue_get_data(scheduler->cur);
    if (thread->cancel_req) {
        bthread_exit((void*) -1);
    }
}
