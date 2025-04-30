/*
 * server.c -- Multithreaded TCP File Server
 * 
 * This server accepts TCP connections and supports commands:
 * WRITE <path> <size> - Uploads a file to the server
 * GET <path>          - Downloads a file from the server
 * RM <path>           - Removes a file or directory from the server
 *
 * Each client is handled in a separate thread.
 * 
 * adapted from: 
 *   https://www.educative.io/answers/how-to-implement-tcp-sockets-in-c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>

// Define server port, folder, and buffer limits
#define PORT 2000
#define ROOT_FOLDER "server_root"
#define BUFFER_SIZE 8192
#define MAX_FILES 100

// Structure representing a lock for a specific file path
struct FileLock {
    char path[1024];          // Path to the file
    pthread_mutex_t mutex;    // Mutex to lock access to the file
};

struct FileLock file_locks[MAX_FILES];  // Array of file locks
int lock_count = 0;                     // How many file locks are used
pthread_mutex_t lock_table_mutex = PTHREAD_MUTEX_INITIALIZER;  // Global lock for lock table

// Struct for passing arguments to each client thread
typedef struct {
    int client_sock;
    int thread_num;
} ThreadArgs;

int thread_counter = 0;                      // Counter for assigning thread IDs
pthread_mutex_t thread_counter_mutex = PTHREAD_MUTEX_INITIALIZER; // Lock for thread counter

// Function declarations
void handle_client(int client_sock);
void *client_thread(void *arg);
void receive_file(int client_sock, char *remote_path, int file_size);
void send_file(int client_sock, const char *remote_path);
void remove_file_or_dir(int client_sock, const char *remote_path);
pthread_mutex_t* get_file_mutex(const char *path);

/**
 * @brief Entry point of the server program. Initializes the server, listens for incoming client
 *        connections, and spawns threads to handle each client. Each client is assigned to a new
 *        thread to allow concurrent handling of multiple clients.
 * 
 *        The server sets up a TCP socket, binds it to the specified port, and listens for incoming
 *        connections. Upon accepting a connection, it allocates memory for the thread arguments,
 *        creates a new thread to handle the client, and increments the thread counter. The server
 *        continuously loops to accept new connections, spawning new threads for each.
 * 
 * @return int Exit status of the program (0 for successful termination, non-zero for failure).
 */
int main() {
    int server_fd, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Create TCP socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket failed");
        return 1;
    }

    // Set up server address struct
    server_addr.sin_family = AF_INET;             // IPv4
    server_addr.sin_port = htons(PORT);           // Set port
    server_addr.sin_addr.s_addr = INADDR_ANY;     // Accept connections on any local address


    // Bind socket to address and port
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    // Start listening for incoming client connections
    listen(server_fd, 5);
    printf("Server listening on port %d...\n", PORT);

    // Ensure the root folder exists
    mkdir(ROOT_FOLDER, 0777);

    // Main loop to accept clients
    while (1) {
        client_sock = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_sock < 0) {
            perror("Accept failed");
            continue;
        }

        // Allocate memory for thread arguments
        ThreadArgs *args = malloc(sizeof(ThreadArgs));
        if (args == NULL) {
            perror("Failed to allocate memory for thread args");
            close(client_sock); // Close the accepted socket
            continue; // Skip this client
        }

        // Assign the client socket and a unique thread number
        args->client_sock = client_sock;
        pthread_mutex_lock(&thread_counter_mutex);
        args->thread_num = thread_counter++;
        pthread_mutex_unlock(&thread_counter_mutex);

        // Launch thread to handle the client
        // Each client connection is handled in a separate thread using pthread_create()
        pthread_t tid;
        if (pthread_create(&tid, NULL, client_thread, args) != 0) {
            perror("Failed to create thread");
            free(args); // Free allocated memory if thread creation fails
            close(client_sock);
        } else {
             pthread_detach(tid); // Auto-release thread resources
        }
    }
    return 0;
}

/**
 * @brief Thread function to handle an individual client connection.
 * 
 *        This function is executed by a newly created thread for each client.
 *        It extracts the socket and thread number from the passed arguments,
 *        handles communication with the client, and then closes the connection.
 * 
 * @param arg Pointer to ThreadArgs struct containing client socket and thread number.
 * @return void* Always returns NULL.
 */
void *client_thread(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    int client_sock = args->client_sock;
    int thread_num = args->thread_num;
    free(arg);

    printf("[Thread #%d] Connected to client\n", thread_num);
    handle_client(client_sock);
    close(client_sock);
    printf("[Thread #%d] Connection closed\n", thread_num);
    return NULL;
}

/**
 * @brief Handles a single client's command request.
 * 
 *        Reads a command line from the client socket, parses the command type,
 *        and dispatches to the appropriate handler function (WRITE, GET, or RM).
 *        Sends response messages back to the client based on the outcome.
 * 
 * @param client_sock Socket file descriptor for the connected client.
 */
void handle_client(int client_sock) {
    char command_buf[1024] = {0};
    char ch;
    int i = 0;

    // Read commands (ends with newline)
    while (recv(client_sock, &ch, 1, 0) > 0 && ch != '\n' && i < sizeof(command_buf) - 1) {
        command_buf[i++] = ch;
    }
    command_buf[i] = '\0';

    // Extract command keyword
    char command[16];
    sscanf(command_buf, "%s", command);

    if (strcmp(command, "WRITE") == 0) {
        char remote_path[1024];
        int file_size;
        if (sscanf(command_buf, "%*s %s %d", remote_path, &file_size) != 2) {
            send(client_sock, "ERROR: Invalid WRITE format\n", 29, 0);
            return;
        }
        printf("Received WRITE %s %d\n", remote_path, file_size);
        receive_file(client_sock, remote_path, file_size);
        send(client_sock, "OK\n", 3, 0);
    } else if (strcmp(command, "GET") == 0) {
        char remote_path[1024];
        if (sscanf(command_buf, "%*s %s", remote_path) != 1) {
            send(client_sock, "ERROR: Invalid GET format\n", 27, 0);
            return;
        }
        printf("Received GET %s\n", remote_path);
        send_file(client_sock, remote_path);
    } else if (strcmp(command, "RM") == 0) {
        char remote_path[1024];
        if (sscanf(command_buf, "%*s %s", remote_path) != 1) {
            send(client_sock, "ERROR: Invalid RM format\n", 25, 0);
            return;
        }
        printf("Received RM %s\n", remote_path);
        remove_file_or_dir(client_sock, remote_path);
    } else {
        send(client_sock, "ERROR: Unknown command\n", 25, 0);
    }
}

/**
 * @brief Receives a file from the client and saves it to the server's file system.
 * 
 *        Acquires a file-specific mutex lock to ensure thread-safe writing.
 *        Automatically creates intermediate directories if they do not exist.
 *        Reads the incoming file data from the socket and writes it to disk.
 * 
 * @param client_sock Socket file descriptor for the connected client.
 * @param remote_path Path (relative to ROOT_FOLDER) where the file should be saved.
 * @param file_size   Expected size of the incoming file in bytes.
 */
void receive_file(int client_sock, char *remote_path, int file_size) {
    char full_path[2048];
    snprintf(full_path, sizeof(full_path), "%s/%s", ROOT_FOLDER, remote_path);

    // Acquire file-specific lock
    pthread_mutex_t *mutex = get_file_mutex(remote_path);
    if (mutex) pthread_mutex_lock(mutex);

    // Create intermediate directories if needed
    for (char *p = full_path + strlen(ROOT_FOLDER) + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            mkdir(full_path, 0777);
            *p = '/';
        }
    }

    FILE *fp = fopen(full_path, "wb");
    if (!fp) {
        perror("File open failed");
        return;
    }

    // Read file content from socket
    int total_received = 0;
    char buffer[BUFFER_SIZE];
    while (total_received < file_size) {
        int bytes = recv(client_sock, buffer, sizeof(buffer), 0);
        if (bytes <= 0) break;
        fwrite(buffer, 1, bytes, fp);
        total_received += bytes;
    }
    fclose(fp);
    printf("File %s written (%d bytes)\n", remote_path, total_received);

    // Unlock file
    if (mutex) pthread_mutex_unlock(mutex);
}

/**
 * @brief Sends a file from the server to the client.
 * 
 *        Constructs the full file path, opens the file in binary read mode,
 *        and sends a header containing the file size. Then transmits the file
 *        contents in chunks over the socket. If the file does not exist, an
 *        error message is sent instead.
 * 
 * @param client_sock Socket file descriptor for the connected client.
 * @param remote_path Path (relative to ROOT_FOLDER) of the file to send.
 */
void send_file(int client_sock, const char *remote_path) {
    char full_path[2048];
    snprintf(full_path, sizeof(full_path), "%s/%s", ROOT_FOLDER, remote_path);

    FILE *fp = fopen(full_path, "rb");
    if (!fp) {
        send(client_sock, "ERROR: File not found\n", 23, 0);
        return;
    }

    // Acquire file-specific lock
    pthread_mutex_t *mutex = get_file_mutex(remote_path);
    if (mutex) pthread_mutex_lock(mutex);

    // Get file size
    fseek(fp, 0, SEEK_END);
    int file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Send file size as header
    char header[128];
    snprintf(header, sizeof(header), "SIZE %d\n", file_size);
    send(client_sock, header, strlen(header), 0);

    // Send file content
    char buffer[BUFFER_SIZE];
    int bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        send(client_sock, buffer, bytes, 0);
    }

    fclose(fp);
    printf("Sent file %s (%d bytes)\n", remote_path, file_size);

    // Unlock file
    if (mutex) pthread_mutex_unlock(mutex);
}

/**
 * @brief Removes a file or directory from the server.
 * 
 *        Constructs the full path of the file or directory, checks if it exists,
 *        and then removes it. If the path refers to a directory, it is removed
 *        using `rmdir`, otherwise, a regular file is removed with `remove`. The
 *        client is notified of the operation result.
 * 
 * @param client_sock Socket file descriptor for the connected client.
 * @param remote_path Path (relative to ROOT_FOLDER) of the file or directory to remove.
 */
void remove_file_or_dir(int client_sock, const char *remote_path) {
    char full_path[2048];
    snprintf(full_path, sizeof(full_path), "%s/%s", ROOT_FOLDER, remote_path);

    // Acquire file-specific lock
    pthread_mutex_t *mutex = get_file_mutex(remote_path);
    if (mutex) pthread_mutex_lock(mutex);

    struct stat st;
    if (stat(full_path, &st) != 0) {
        send(client_sock, "ERROR: File not found\n", 23, 0);
        return;
    }

    int result = -1;
    if (S_ISDIR(st.st_mode)) {
        result = rmdir(full_path);
    } else {
        result = remove(full_path);
    }

    if (result == 0) {
        send(client_sock, "OK\n", 3, 0);
        printf("Deleted: %s\n", full_path);
    } else {
        perror("Remove failed");
        send(client_sock, "ERROR: Unable to delete\n", 25, 0);
    }

    // Unlock file
    if (mutex) pthread_mutex_unlock(mutex);
}

/**
 * @brief Retrieves a mutex for a specific file path to ensure mutual exclusion
 *        when accessing the file. If no mutex exists for the path, a new one is created.
 * 
 *        This function ensures that only one thread can access a particular file at a time
 *        by using a mutex. If the file already has an associated mutex, it is returned.
 *        If the mutex does not exist and space is available, a new mutex is initialized
 *        and added to the lock table.
 * 
 * @param path The file path for which to retrieve or create a mutex.
 * @return pthread_mutex_t* Pointer to the mutex associated with the given file path, or NULL
 *                          if no mutex could be created due to space constraints.
 */
pthread_mutex_t* get_file_mutex(const char *path) {
    pthread_mutex_lock(&lock_table_mutex);

    // Check if mutex for path already exists
    for (int i = 0; i < lock_count; i++) {
        if (strcmp(file_locks[i].path, path) == 0) {
            pthread_mutex_unlock(&lock_table_mutex);
            return &file_locks[i].mutex;
        }
    }

    // If not found and space available, create new lock
    if (lock_count < MAX_FILES) {
        strcpy(file_locks[lock_count].path, path);
        pthread_mutex_init(&file_locks[lock_count].mutex, NULL);
        pthread_mutex_t *m = &file_locks[lock_count].mutex;
        lock_count++;
        pthread_mutex_unlock(&lock_table_mutex);
        return m;
    }

    pthread_mutex_unlock(&lock_table_mutex);
    return NULL;  // No lock available
}
