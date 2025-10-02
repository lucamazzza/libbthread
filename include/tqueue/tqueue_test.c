#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#include "tqueue.h"

int main(void) {
    tqueue_t q = NULL;
    void *t1, *t2, *t3, *t4;
    t1 = (void *)1;
    t2 = (void *)2;
    t3 = (void *)3;
    t4 = (void *)4;

    assert(tqueue_size(q) == 0);
    assert(tqueue_enqueue(&q, t1) == 0);
    assert(tqueue_size(q) == 1);
    assert(tqueue_enqueue(&q, t2) == 1);
    assert(tqueue_size(q) == 2);
    assert(tqueue_enqueue(&q, t3) == 2);
    assert(tqueue_size(q) == 3);
    assert(tqueue_enqueue(&q, t4) == 3);
    assert(tqueue_size(q) == 4);

    for (int i = 0; i < 4; i++) {
        assert(tqueue_get_data(tqueue_at_offset(q, i)) == (void *) (intptr_t) (i + 1));
    }

    for (int i = 1; i < 5; i++) {
        assert(tqueue_pop(&q) == (void *) (intptr_t) i);
        assert(tqueue_size(q) == 4 - i);
    }

    assert(tqueue_pop(&q) == NULL);

    assert(tqueue_size(q) == 0);
    assert(tqueue_enqueue(&q, t1) == 0);
    assert(tqueue_size(q) == 1);
    assert(tqueue_enqueue(&q, t2) == 1);
    assert(tqueue_size(q) == 2);

    assert(tqueue_pop(&q) == (void *)1);
    assert(tqueue_size(q) == 1);

    assert(tqueue_enqueue(&q, t1) == 1);
    assert(tqueue_size(q) == 2);

    assert(tqueue_pop(&q) == (void *)2);
    assert(tqueue_size(q) == 1);

    assert(tqueue_at_offset(q, 0) != NULL);
    assert(tqueue_at_offset(q, 1) == q);

    assert(tqueue_pop(&q) == (void *)1);
    assert(tqueue_size(q) == 0);

    printf("All tests passed!\n");

    return 0;
}
