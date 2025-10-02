#include "../include/bthread/bthreadqueue.h"

typedef struct bthread_queue_node {
    struct bthread_queue_node* next;
    void *data;
} bthread_queue_node_t;
