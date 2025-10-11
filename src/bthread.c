#include <stdint.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>

#include "bthread.h"
#include "bthread_private.h"
#include "tqueue.h"

__bthread_scheduler *bthread_get_scheduler(void) {
    static __bthread_scheduler s = {0};
    return &s;
}

int bthread_is_thread_zombie_state(bthread_t thread, void **retval) {
}

int bthread_create(bthread_t *thread, const bthread_attr_t *attr, void *(*start_routine)(void *), void *arg) {
    __bthread_scheduler *s = bthread_get_scheduler();
    __bthread_private *t = malloc(sizeof(__bthread_private));
    t->tid = ++s->cur_tid;
    t->body = start_routine;
    t->arg = arg;
    t->state = __BTHREAD_READY;
    t->stack = 0;
    tqueue_enqueue(&s->queue, t);
    if (thread) *thread = t->tid;
    return 0;
}

int bthread_join(bthread_t thread, void **retval) {
    volatile __bthread_scheduler *s = bthread_get_scheduler();
    s->cur = s->queue;
    save_context(s->context);
    if (bthread_is_thread_zombie_state(thread, retval)) return 0;
    __bthread_private *tp;
    do {
        s->cur = tqueue_at_offset(s->cur, 1);
        tp = (__bthread_private*) tqueue_get_data(s->cur);
    } while (tp->state != __BTHREAD_READY);
    if (tp->stack) restore_context(tp->context);
    else {
        tp->stack = (uintptr_t) malloc(sizeof(char) * STACK_SIZE);
        uintptr_t target = tp->stack + STACK_SIZE - 1;
        target -= (target % 16);
#if __aarch64__
        asm __volatile__("mov x1, %0" :: "r"(tp) : "x1");
        asm __volatile__("mov sp, %0" :: "r"((uintptr_t) target) : "x1");
        asm __volatile__("mov %0, x1" : "=r"(tp));
#elif __arm__
        asm __volatile__("mov %%sp, %0" :: "r"((uintptr_t) target));
#elif __x86_64__
        asm __volatile__("movq %0, %%rsp" :: "r"((uintptr_t) target));
#else
        asm __volatile__("movl %0, %%esp" :: "r"((uintptr_t) target));
#endif
        bthread_exit(tp->body(tp->arg));
    }
}

