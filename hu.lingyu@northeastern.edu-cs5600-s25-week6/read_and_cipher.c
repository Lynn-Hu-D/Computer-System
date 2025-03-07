#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "queue.h"

// Function to read the file into the queue
void read_file_into_queue(const char *filename, queue_t *queue) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Unable to open file");
        exit(1);
    }

    char str[100];
    while (fgets(str, sizeof(str), file)) {
        str[strcspn(str, "\n")] = '\0';  // Remove the newline character
        push_queue(queue, str, sizeof(str));
    }

    fclose(file);
}

// Function to print the first n elements in the queue
void print_first_n_elements(queue_t *queue, int n) {
    node_t *current = queue->head;
    int count = 0;
    
    // Iterate through the queue, printing each element
    while (current != NULL && count < n) {
        printf("%s\n", (char *)current->data);  // Assuming the data is a string
        current = current->next;
        count++;
    }

    // If less than n elements, print a message
    if (count == 0) {
        printf("Queue is empty.\n");
    } else if (count < n) {
        printf("Queue has less than n elements. Printed %d elements.\n", count);
    }
}

// Function to check if the cipher program exists
int is_program_available(const char *program) {
    return access(program, X_OK) == 0;  // Check if the file exists and is executable
}


int main() {
	queue_t* queue = create_queue();
	read_file_into_queue("random_strings.txt", queue);

	printf("Queue (size %d):\n", get_queue_size(queue));

    if (get_queue_size(queue) == 0) {
        fprintf(stderr, "Error: random_strings.txt does not contain sentences.\n");
        return 1;  // Exit the program if random_strings.txt is empty
    }

    // Check if the cipher program exists
    const char *cipher_program = "./cipher";
    if (!is_program_available(cipher_program)) {
        fprintf(stderr, "Error: The cipher program '%s' does not exist or is not executable.\n", cipher_program);
        return 1;  // Exit the program if cipher does not exist
    }

    // print first 10 elements for queue for checking correctness
	print_first_n_elements(queue, 10);

    int num_strings_per_batch = 100;
    int batch_count = 0;
    while (get_queue_size(queue) > 0) {
        char *batch[num_strings_per_batch+3];
        batch[0] = "./cipher";
        batch[1] = "-e";
        int j = 0;
        // pop out 100 strings into a batch
        while (get_queue_size(queue) > 0 && j < 100) {
            batch[j+2] = pop_queue(queue);
            j++;
        }
        batch[j + 2] = NULL;  // Ensure the argument list is NULL-terminated

        pid_t pid = fork();  // Fork a child process
        if (pid < 0) {
            perror("Fork failed");
            exit(1);
        }

        if (pid == 0) { // Child process
            printf("Child PID: %d PPID: %d\n", getpid(), getppid());
            
            char filename[256];
            sprintf(filename, "output_cipher_%d.txt", batch_count);

             // Open the file for writing
            FILE *output_file = fopen(filename, "w");
            if (output_file == NULL) {
                perror("Error opening output file");
                exit(1);
            }

            // Use dup2() to redirect stdout to the file
            int fd = fileno(output_file);
            if (dup2(fd, STDOUT_FILENO) == -1) {
                perror("Failed to redirect stdout to file");
                exit(1);
            }

            // encrypt all strings in a batch at a time
            execvp("./cipher", batch);
            perror("execvp failed");  // If execvp fails

            return 0;
        }
        batch_count++;
    }
    printf("Parent PID: %d PPID: %d\n", getpid(), getppid() );

    // Wait for the child process to finish
    wait(NULL);

	return 0;
}