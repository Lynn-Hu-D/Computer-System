#include "queue.h"

/* Create and initialize a new, empty queue */
queue_t* create_queue() {
    queue_t* queue = (queue_t*) malloc(sizeof(queue_t));
    if (!queue) {
        fprintf(stderr, "Error: Could not allocate memory for queue\n");
        exit(EXIT_FAILURE);
    }
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
    return queue;
}

/* Push an element to the end (tail) of the queue */
void push_queue(queue_t* queue, void* element, size_t data_size) {
    if (!queue) return;
    node_t* new_node = (node_t*) malloc(sizeof(node_t));
    if (!new_node) {
        fprintf(stderr, "Error: Could not allocate memory for new node\n");
        exit(EXIT_FAILURE);
    }
    new_node->data = malloc(data_size);
    memcpy(new_node->data, element, data_size);
    new_node->next = NULL;
    new_node->prev = queue->tail;

    if (queue->tail) {
        queue->tail->next = new_node;
    } else {
        /* Queue was empty so set head as well */
        queue->head = new_node;
    }
    queue->tail = new_node;
    queue->size++;
}

/* Pop and return the data from the front (head) of the queue */
void* pop_queue(queue_t* queue) {
    if (!queue || !queue->head) {
        return NULL;
    }
    node_t* front = queue->head;
    void* data = front->data;
    
    queue->head = front->next;
    if (queue->head) {
        queue->head->prev = NULL;
    } else {
        /* Queue is now empty */
        queue->tail = NULL;
    }
    free(front);
    queue->size--;
    return data;
}

/* Remove and return the process_t element with the highest priority.
   (Higher integer value indicates a higher priority.)
   The search scans the entire queue and removes the matching node. */
process_t* remove_process(queue_t* queue) {
    if (!queue || !queue->head) {
        return NULL;
    }

    node_t* current = queue->head;
    node_t* highest_node = current;
    process_t* highest_proc = (process_t*) current->data;

    while (current) {
        process_t* proc = (process_t*) current->data;
        if (proc->priority > highest_proc->priority) {
            highest_proc = proc;
            highest_node = current;
        }
        current = current->next;
    }

    /* Remove highest_node from the linked list */
    if (highest_node->prev) {
        highest_node->prev->next = highest_node->next;
    } else {
        /* Removing head */
        queue->head = highest_node->next;
    }
    if (highest_node->next) {
        highest_node->next->prev = highest_node->prev;
    } else {
        /* Removing tail */
        queue->tail = highest_node->prev;
    }

    void* data = highest_node->data;
    free(highest_node);
    queue->size--;
    return (process_t*) data;
}

/* Return the number of elements in the queue */
int get_queue_size(queue_t* queue) {
    if (!queue) return 0;
    return queue->size;
}

/* Free all nodes in the queue.
   If free_data is not NULL, it is used to free each element's data. */
void free_queue(queue_t* queue, void (*free_data)(void*)) {
    if (!queue) return;
    node_t* current = queue->head;
    while (current) {
        node_t* next = current->next;
        if (free_data) {
            free_data(current->data);
        }
        free(current);
        current = next;
    }
    free(queue);
}