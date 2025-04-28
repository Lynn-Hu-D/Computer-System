#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdbool.h>
#include <time.h>

#define MAX_TEXT_LENGTH 256
#define MESSAGE_FLODER "messages"

// Structure to represent a message
typedef struct {
    int id;                          // Unique identifier for the message
    time_t timestamp;                // Time the message was sent (e.g., Unix timestamp)
    char sender[MAX_TEXT_LENGTH];    // Sender of the message
    char receiver[MAX_TEXT_LENGTH];  // Receiver of the message
    char content[MAX_TEXT_LENGTH];   // Content of the message
    bool delivered;                  // Flag indicating if the message was delivered
} Message;

// Function to create a message
Message* create_msg(int id, const char* sender, const char* receiver, const char* content);

// Function to store a message to disk
bool store_msg(const Message* msg);

// Function to retrieve a message from disk
Message* retrieve_msg(const int id);

// Function to free the memory allocated for a message
void free_msg(Message *msg);

#endif // MESSAGE_H
