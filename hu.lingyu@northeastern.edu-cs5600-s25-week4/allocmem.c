#include <stdio.h>
#include <stdlib.h>

#define HEAP_SIZE (10 * 1024 * 1024)  // 10 MB
#define STATIC_SIZE (5 * 1024 * 1024) // 5 MB
#define STACK_SIZE (1 * 1024 * 1024)  // 1 MB

// Static memory allocation (5 MB)
static char static_memory[STATIC_SIZE];

void allocate_stack_memory(size_t size) {
    // Allocating memory from the stack by declaring a large array.
    char stack_memory[size];

    // Simulate using the stack memory
    for (size_t i = 0; i < size; i++) {
        stack_memory[i] = 'A';
    }

    printf("Stack memory of %zu bytes allocated and initialized.\n", size);
    // Stack memory is automatically deallocated when the function exits.
}

int main() {
    printf("Memory allocation program started.\n");

    // Heap memory allocation (10 MB)
    char *heap_memory = (char *)malloc(HEAP_SIZE);
    if (heap_memory == NULL) {
        fprintf(stderr, "Heap memory allocation failed.\n");
        return 1;
    }

    // Simulate using the heap memory
    for (size_t i = 0; i < HEAP_SIZE; i++) {
        heap_memory[i] = 'B';
    }
    printf("Heap memory of %d bytes allocated and initialized.\n", HEAP_SIZE);

    // Static memory usage (already allocated)
    for (size_t i = 0; i < STATIC_SIZE; i++) {
        static_memory[i] = 'C';
    }
    printf("Static memory of %d bytes initialized.\n", STATIC_SIZE);

    // Allocate memory from the stack (1 MB)
    allocate_stack_memory(STACK_SIZE);

    // Release the heap memory back to the operating system 
    free(heap_memory);
    printf("Heap memory deallocated.\n");

    printf("Program exiting. Static and stack memory will be reclaimed automatically.\n");
    return 0;
}