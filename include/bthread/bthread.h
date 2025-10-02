#ifndef __BTHREAD_H__
#define __BTHREAD_H__

#include "bthreadtypes.h"

#define save_context(CONTEXT) sigsetjmp(CONTEXT, 1)
#define restore_context(CONTEXT) siglongjmp(CONTEXT, 1)

static int           bthread_is_thread_zombie_state(bthread_t thread, void **retval);
static tqueue_t      bthread_get_queue_starting_at_thread(bthread_t thread);

__bthread_scheduler *bthread_get_scheduler();
int                  bthread_create(bthread_t *thread, const bthread_attr_t *attr, void *(*start_routine)(void *), void *arg);
int                  bthread_join(bthread_t thread, void **retval);
void                 bthread_yield();
void                 bthread_exit(void *retval);

#endif
