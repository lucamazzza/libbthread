#include "tqueue.h"
#include <stdlib.h>

typedef struct tqueue_node {
    struct  tqueue_node *next;
    void   *data;
} tqueue_node_t;

unsigned long int tqueue_enqueue(tqueue_t *q, void *data) {
    tqueue_node_t *ne = (tqueue_node_t *)malloc(sizeof(tqueue_node_t));
    ne->data = data;
    unsigned long int id = 0;
    if (*q == NULL) {
        *q = ne;
        ne->next = *q;
    } else {
        tqueue_node_t *head = *q;
        id = 1;
        while (head->next != *q) {
            head = head->next;
            id++;
        }
        head->next = ne;
        ne->next = *q;
    }
    return id;
}

void *tqueue_pop(tqueue_t *q) {
    if (*q == NULL) return NULL;
    else {
        tqueue_node_t *old_head = *q;
        void* ret_val = old_head->data;
        if (old_head->next == old_head) *q = NULL;
        else {
            *q = old_head->next;
            tqueue_t head = *q;
            while (head->next != old_head) head = head->next;
            head->next = *q;
        }
        free(old_head);
        return ret_val;
    }
}   

unsigned long int tqueue_size(tqueue_t q) {
    unsigned long int size = 1;
    if (q == NULL) return 0;
    tqueue_t head = q;
    while (head->next != q) {
        head = head->next;
        size++;
    }
    return size;
}

tqueue_t tqueue_at_offset(tqueue_t q, unsigned long int offset) {
    if (offset < 0 || q == NULL) return NULL;
    unsigned long int id;
    tqueue_t head = q;
    for (id = 0; id < offset; id++) {
        head = head->next;
        // if (head == q) return NULL;
    }
    return head;
}

void *tqueue_get_data(tqueue_t q) {
    return (q == NULL) ? NULL : q->data; 
}
