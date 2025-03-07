#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

/* Helper function: Print the details of a process_t */
void print_process(process_t* proc) {
    if (!proc)
        return;
    printf("  Process ID: %d, Name: %s, Runtime: %ld, Priority: %d\n",
           proc->id, proc->name, proc->runtime, proc->priority);
}

/* Helper function: Print the entire queue contents.
   (This function assumes the queue stores process_t* elements.) */
void print_queue(queue_t* queue) {
    if (!queue) return;
    printf("Queue (size %d):\n", get_queue_size(queue));
    node_t* current = queue->head;
    while (current) {
        print_process((process_t*) current->data);
        current = current->next;
    }
    printf("\n");
}

int main() {
    /* Create an empty queue */
    queue_t* queue = create_queue();
    printf("Initial state: Queue is empty.\n");
    print_queue(queue);

    /* Create several process_t elements */
    process_t* p1 = malloc(sizeof(process_t));
    p1->id = 1;
    p1->name = strdup("Process1");
    p1->runtime = 100;
    p1->priority = 3;

    process_t* p2 = malloc(sizeof(process_t));
    p2->id = 2;
    p2->name = strdup("Process2");
    p2->runtime = 200;
    p2->priority = 5;

    process_t* p3 = malloc(sizeof(process_t));
    p3->id = 3;
    p3->name = strdup("Process3");
    p3->runtime = 150;
    p3->priority = 4;

    process_t* p4 = malloc(sizeof(process_t));
    p4->id = 4;
    p4->name = strdup("Process4");
    p4->runtime = 250;
    p4->priority = 2;

    /* Push processes into the queue */
    printf("Pushing p1 into queue.\n");
    push_queue(queue, p1);
    print_queue(queue);

    printf("Pushing p2 into queue.\n");
    push_queue(queue, p2);
    print_queue(queue);

    printf("Pushing p3 into queue.\n");
    push_queue(queue, p3);
    print_queue(queue);

    printf("Pushing p4 into queue.\n");
    push_queue(queue, p4);
    print_queue(queue);

    /* Pop an element from the front of the queue */
    printf("Popping the front element from the queue.\n");
    process_t* popped = (process_t*) pop_queue(queue);
    printf("Popped process:\n");
    print_process(popped);
    /* Free the popped process */
    free(popped->name);
    free(popped);
    print_queue(queue);

    /* Remove the process with the highest priority */
    printf("Removing process with the highest priority from the queue.\n");
    process_t* removed = remove_process(queue);
    printf("Removed process:\n");
    print_process(removed);
    free(removed->name);
    free(removed);
    print_queue(queue);

    /* Get current queue size */
    printf("Current queue size: %d\n\n", get_queue_size(queue));

    /* Pop all remaining elements and free them */
    printf("Emptying the queue:\n");
    while(get_queue_size(queue) > 0) {
        process_t* proc = (process_t*) pop_queue(queue);
        print_process(proc);
        free(proc->name);
        free(proc);
    }
    print_queue(queue);

    /* Free the queue structure itself */
    free(queue);
    return 0;
}
