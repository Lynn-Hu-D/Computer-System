#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_SIZE 256 // Pre-defined size for the integer array

// Define the structure using typedef
typedef struct {
    int *data;       // Pointer to dynamically allocated array
    size_t size;     // Size of the array
} dyn_block;

// Function to allocate a dyn_block instance and its data array
dyn_block* alloc_dyn_block(size_t size) {
    // Allocate memory for the structure
    dyn_block *block = (dyn_block *)malloc(sizeof(dyn_block));
    if (block == NULL) {
        fprintf(stderr, "Failed to allocate memory for dyn_block.\n");
        return NULL;
    }

    // Allocate memory for the data array inside the structure
    block->data = (int *)malloc(size * sizeof(int));
    if (block->data == NULL) {
        fprintf(stderr, "Failed to allocate memory for data array.\n");
        free(block);
        return NULL;
    }

    block->size = size;
    return block;
}

// Function to store an array of integers in the dynamic block
void store_mem_blk(dyn_block *block, int *values, size_t size) {
    if (block == NULL || block->data == NULL) {
        fprintf(stderr, "Invalid dyn_block provided.\n");
        return;
    }

    if (size > block->size) {
        fprintf(stderr, "Size exceeds allocated block size.\n");
        return;
    }

    // Copy values into the block's data array
    for (size_t i = 0; i < size; i++) {
        block->data[i] = values[i];
    }

    printf("Stored %zu integers in dyn_block.\n", size);
}

// Function to read integers from a line and store them in a dyn_block
dyn_block* parse_line_to_block(const char *line) {
    size_t size = 0;
    int *values = NULL;

    // Tokenize the line and count integers
    char *token = strtok(strdup(line), " ");
    while (token != NULL) {
        size++;
        values = realloc(values, size * sizeof(int));
        if (values == NULL) {
            fprintf(stderr, "Failed to allocate memory for values.\n");
            free(values);
            return NULL;
        }
        values[size - 1] = atoi(token); // Convert token to integer
        token = strtok(NULL, " ");
    }

    // Allocate and populate dyn_block
    dyn_block *block = alloc_dyn_block(size);
    if (block != NULL) {
        memcpy(block->data, values, size * sizeof(int));
    }

    free(values); // Temporary array no longer needed
    return block;
}

// Function to read the file and create dynamic blocks for each line
dyn_block** read_file_to_blocks(const char *filename, size_t *block_count) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
        return NULL;
    }

    dyn_block **blocks = NULL;
    char line[MAX_LINE_SIZE];
    *block_count = 0;

    // Read lines from the file
    while (fgets(line, sizeof(line), file)) {
        (*block_count)++;
        blocks = realloc(blocks, (*block_count) * sizeof(dyn_block *));
        if (blocks == NULL) {
            fprintf(stderr, "Failed to allocate memory for blocks.\n");
            fclose(file);
            return NULL;
        }

        // Parse line into a dyn_block
        blocks[*block_count - 1] = parse_line_to_block(line);
    }

    fclose(file);
    return blocks;
}

// Function to display the content of dynamic blocks
void display_blocks(dyn_block **blocks, size_t block_count) {
    for (size_t i = 0; i < block_count; i++) {
        printf("Block %zu: ", i + 1);
        for (size_t j = 0; j < blocks[i]->size; j++) {
            printf("%d ", blocks[i]->data[j]);
        }
        printf("\n");
    }
}

// Function to free a dynamic block
void free_dyn_block(dyn_block *block) {
    if (block) {
        free(block->data);
        free(block);
    }
}

// Free all dynamic blocks
void free_all_blocks(dyn_block **blocks, size_t block_count) {
    for (size_t i = 0; i < block_count; i++) {
        free_dyn_block(blocks[i]);
    }
    free(blocks);
}

int main() {
    // File containing the data
    const char *filename = "blocks.data";
    size_t block_count = 0;

    // Read the file and create dynamic blocks
    dyn_block **blocks = read_file_to_blocks(filename, &block_count);
    if (blocks == NULL) {
        return 1;
    }

    // Display the content of the blocks
    printf("Contents of the dynamic blocks:\n");
    display_blocks(blocks, block_count);

    // Free all allocated memory
    free_all_blocks(blocks, block_count);

    return 0;
}
