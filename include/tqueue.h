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
