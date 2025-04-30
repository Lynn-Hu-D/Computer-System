/*
 * client.c -- TCP Socket Client
 *
 * This program is a TCP client that supports three file operations with a server:
 *  - WRITE <local-file> <remote-file>: Uploads a file from client to server
 *  - GET <remote-file> <local-file>: Downloads a file from server to client
 *  - RM <remote-file>: Deletes a file on the server
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
#include <sys/stat.h>

#define PORT 2000
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 8192

// Function declarations
int send_write_command(const char *local_path, const char *remote_path);
int send_get_command(const char *remote_path, const char *local_path);
int send_rm_command(const char *remote_path);


/**
 * @brief Entry point of the client program. Parses command-line arguments and 
 *        dispatches to the corresponding command function.
 * 
 * @param argc Number of command-line arguments
 * @param argv Array of command-line arguments
 * @return int Exit status
 */
int main(int argc, char *argv[]) {
    // Invalid usage with insufficient arguments
    if (argc < 2) {
        printf("Usage:\n");
        printf("  %s WRITE <local> <remote>\n", argv[0]);
        printf("  %s GET <remote> <local>\n", argv[0]);
        printf("  %s RM <remote>\n", argv[0]);
        return 1;
    }

    // Handle WRITE command
    if (strcmp(argv[1], "WRITE") == 0) {
        if (argc != 4) {
            printf("Usage: %s WRITE <local-file-path> <remote-file-path>\n", argv[0]);
            return 1;
        }
        return send_write_command(argv[2], argv[3]);

    // Handle GET command
    } else if (strcmp(argv[1], "GET") == 0) {
        if (argc != 4) {
            printf("Usage: %s GET <remote-file-path> <local-file-path>\n", argv[0]);
            return 1;
        }
        return send_get_command(argv[2], argv[3]);

    // Handle RM command
    } else if (strcmp(argv[1], "RM") == 0) {
        if (argc != 3) {
            printf("Usage: %s RM <remote-file-path>\n", argv[0]);
            return 1;
        }
        return send_rm_command(argv[2]);

    // Unknown command
    } else {
        printf("Unknown command: %s\n", argv[1]);
        return 1;
    }
}

/**
 * @brief Sends a WRITE command to the server, transferring the specified local file.
 * 
 * @param local_path Path to the local file on client
 * @param remote_path Destination path on the server
 * @return int Exit status
 */
int send_write_command(const char *local_path, const char *remote_path) {
    // Open the local file for reading in binary mode
    FILE *fp = fopen(local_path, "rb");
    if (!fp) {
        perror("Failed to open local file");
        return 1;
    }

    // Determine file size
    fseek(fp, 0, SEEK_END);
    int file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Create socket and connect to server
    int sock = connect_to_server();
    if (sock < 0) {
        return 1;
    }

    // Send WRITE header to server
    char header[1024];
    snprintf(header, sizeof(header), "WRITE %s %d\n", remote_path, file_size);
    send(sock, header, strlen(header), 0);

    // Send file content
    char buffer[BUFFER_SIZE];
    int bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        send(sock, buffer, bytes, 0);
    }

    // Wait for server response
    memset(buffer, 0, sizeof(buffer));
    recv(sock, buffer, sizeof(buffer) - 1, 0);
    printf("Server response: %s", buffer);

    // Close the connection and return the exist status
    fclose(fp);
    close(sock);
    return 0;
}

/**
 * @brief Sends a GET command to retrieve a file from the server and save it locally. 
 *         If the file does not exist locally, create a path and file in local.
 *         If the file already exists locally, replace the existing file with downloaded file.
 * 
 * @param remote_path Path to the file on the server
 * @param local_path Path to store the file locally
 * @return int Exit status
 */
int send_get_command(const char *remote_path, const char *local_path) {
    // Create socket and connect to server
    int sock = connect_to_server();
    if (sock < 0) {
        return 1;
    }

    // Send GET request
    char request[1024];
    snprintf(request, sizeof(request), "GET %s\n", remote_path);
    send(sock, request, strlen(request), 0);

    // Receive SIZE header from server
    char header[128] = {0};
    char ch;
    int i = 0;
    while (recv(sock, &ch, 1, 0) > 0 && ch != '\n' && i < sizeof(header) - 1) {
        header[i++] = ch;
    }
    header[i] = '\0';

    // Check for error or size info
    if (strncmp(header, "SIZE", 4) != 0) {
        printf("Server response: %s\n", header);
        close(sock);
        return 1;
    }

    int file_size;
    sscanf(header, "SIZE %d", &file_size);

    // Create directories if needed
    char path_copy[1024];
    strncpy(path_copy, local_path, sizeof(path_copy));
    char *p = strrchr(path_copy, '/');
    if (p) {
        *p = '\0';
        char mkdir_cmd[1050];
        snprintf(mkdir_cmd, sizeof(mkdir_cmd), "mkdir -p %s", path_copy);
        system(mkdir_cmd);
    }

    // Open local file for writing
    FILE *fp = fopen(local_path, "wb");
    if (!fp) {
        perror("Failed to open local file");
        close(sock);
        return 1;
    }

    // Receive and write file data
    int total_received = 0;
    char buffer[BUFFER_SIZE];
    while (total_received < file_size) {
        int bytes = recv(sock, buffer, sizeof(buffer), 0);
        if (bytes <= 0) break;
        fwrite(buffer, 1, bytes, fp);
        total_received += bytes;
    }

    fclose(fp);
    close(sock);
    printf("Downloaded file to %s (%d bytes)\n", local_path, total_received);
    return 0;
}

/**
 * @brief Sends a RM (remove) command to the server to delete a file.
 * 
 * @param remote_path Path of the file to remove on the server
 * @return int Exit status
 */
int send_rm_command(const char *remote_path) {
    // Create socket and connect to server
    int sock = connect_to_server();
    if (sock < 0) {
        return 1;
    }

    // Send RM request
    char request[1024];
    snprintf(request, sizeof(request), "RM %s\n", remote_path);
    send(sock, request, strlen(request), 0);

    // Receive and print response
    char response[1024] = {0};
    recv(sock, response, sizeof(response) - 1, 0);
    printf("Server response: %s", response);

    close(sock);
    return 0;
}

/**
 * @brief Establishes a TCP connection to the server.
 * 
 * This function creates a socket, sets up the server address structure, and attempts
 * to connect to the server using the provided IP and port. If the connection is 
 * successful, it returns the socket file descriptor. Otherwise, it prints an error 
 * message and returns -1.
 * 
 * @return int Socket file descriptor on success, -1 on failure.
 */
static int connect_to_server() {
    // Create TCP socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Set up server address struct
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connect failed");
        close(sock);
        return -1;
    }

    return sock;
}

