#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 
 * process_t: Represents a process.
 * Contains:
 *   - id: an integer identifier.
 *   - name: a string (dynamically allocated) holding the process name.
 *   - runtime: cumulative run-time.
 *   - priority: an integer priority.
 */
typedef struct {
    int id;
    char *name;
    long runtime;
    int priority;
} process_t;

/*
 * node_t: A node in the doubly-linked list.
 * Holds a generic data pointer and pointers to the next and previous nodes.
 */
typedef struct node {
    void *data;
    struct node *next;
    struct node *prev;
} node_t;

/*
 * queue_t: Represents a generic queue.
 * The queue is implemented as a doubly-linked list with pointers to the
 * head (front) and tail (end) of the queue as well as a size counter.
 */
typedef struct {
    node_t *head;
    node_t *tail;
    int size;
} queue_t;

/* Function Prototypes */

/* Create and return a new, empty queue. */
queue_t* create_queue();

/* Push an element onto the tail of the queue. */
void push_queue(queue_t* queue, void* element, size_t data_size);

/* Pop the element at the front of the queue and return its data pointer.
   Returns NULL if the queue is empty. */
void* pop_queue(queue_t* queue);

/* Remove and return the process_t element with the highest priority.
   (Here, a higher integer value of 'priority' indicates higher priority.)
   Returns NULL if the queue is empty. */
process_t* remove_process(queue_t* queue);

/* Return the number of elements in the queue (0 if empty). */
int get_queue_size(queue_t* queue);

/* Free all nodes in the queue. 
   The free_data callback (if provided) is used to free the element data. */
void free_queue(queue_t* queue, void (*free_data)(void*));

#endif // QUEUE_H