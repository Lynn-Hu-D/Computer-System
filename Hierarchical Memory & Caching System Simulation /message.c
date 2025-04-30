#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "message.h"

/**
 * @brief Creates a new message with the given parameters and allocates memory dynamically.
 * 
 * This function initializes a new message object, sets its fields, and returns a pointer to it.
 * The caller is responsible for freeing the allocated memory using `free_msg()`.
 * 
 * @param id Unique identifier for the message.
 * @param sender Sender of the message.
 * @param receiver Receiver of the message.
 * @param content Content of the message.
 * @return Message* Pointer to the newly created message, or NULL if memory allocation fails.
 */
Message* create_msg(int id, const char* sender, const char* receiver, const char* content) {
    // Allocate memory for the message
    Message* msg = (Message*)malloc(sizeof(Message));
    if (!msg) {
        perror("malloc failed");
        return NULL; // Memory allocation failed
    }

    // Initialize fields
    msg->id = id;
    msg->timestamp = time(NULL); // Assign current timestamp
    msg->delivered = false; // Default to undelivered

    // Copy sender, receiver, and content safely
    snprintf(msg->sender, MAX_TEXT_LENGTH, "%s", sender);
    snprintf(msg->receiver, MAX_TEXT_LENGTH, "%s", receiver);
    snprintf(msg->content, MAX_TEXT_LENGTH, "%s", content);
    
    return msg;
}

/**
 * @brief Stores a message to disk by writing it to a file.
 * 
 * This function writes the binary representation of the message into a file located in 
 * the `messages/` directory. The filename is generated using the message ID.
 * 
 * @param msg Pointer to the message to store.
 * @return true if the message was stored successfully, false otherwise.
 */
bool store_msg(const Message *msg) {
    if (!msg) {
        fprintf(stderr, "Invalid message: NULL pointer\n");
        return false;
    }

    // Ensure the 'messages' directory exists
    struct stat st = {0};
    if (stat(MESSAGE_FLODER, &st) == -1) { // Check if directory exists
        if (mkdir(MESSAGE_FLODER, 0700) == -1) { // Create directory if it doesn't exist
            perror("Error creating directory");
            return false;
        }
    }

    // Generate filename based on message ID
    char filename[64];
    snprintf(filename, sizeof(filename), MESSAGE_FLODER "/messages_%d.txt", msg->id);

    // Open file for binary writing
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Error opening file for writing");
        return false;
    }

    // Write the message to the file
    if (fwrite(msg, sizeof(Message), 1, file) != 1) {
        perror("Error writing to file");
        fclose(file);
        return false;
    }

    fclose(file); // Close the file after successful write
    return true;
}

/**
 * @brief Retrieves a message from disk using the given message ID.
 * 
 * This function searches for a file with the corresponding message ID in the `messages/` directory.
 * If found, it reads the message into a dynamically allocated structure.
 * The caller is responsible for freeing the allocated memory using `free_msg()`.
 * 
 * @param id Unique identifier of the message to retrieve.
 * @return Message* Pointer to the retrieved message, or NULL if retrieval fails.
 */
Message* retrieve_msg(const int id) {
    // Ensure the "messages" directory exists
    struct stat st = {0};
    if (stat(MESSAGE_FLODER, &st) == -1) {
        fprintf(stderr, "Messages directory does not exist\n");
        return NULL;
    }

    // Generate filename based on message ID
    char filename[64];
    snprintf(filename, sizeof(filename), MESSAGE_FLODER "/messages_%d.txt", id);

    // Open file for binary reading
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    // Allocate memory for the message
    Message *msg = (Message*)malloc(sizeof(Message));
    if (!msg) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    // Read message data from file
    if (fread(msg, sizeof(Message), 1, file) != 1) {
        perror("Error reading file");
        free_msg(msg);
        fclose(file);
        return NULL;
    }

    fclose(file); // Close the file after successful read
    return msg;
}

/**
 * @brief Frees the memory allocated for a message.
 * 
 * This function should be used to properly deallocate messages that were retrieved or created.
 * 
 * @param msg Pointer to the message to free.
 */
void free_msg(Message *msg) {
    if (msg != NULL) {
        free(msg);
    }
}
