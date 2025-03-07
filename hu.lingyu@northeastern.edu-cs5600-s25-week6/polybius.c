#include "polybius.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Initialize the Polybius cipher table
void pbInitCipher(PolybiusCipher_t *cipher) {
    // This is a standard Polybius table (A-Z, treating J as I)
    char alphabet[25] = "ABCDEFGHIKLMNOPQRSTUVWXYZ";

    // Fill the table with the alphabet
    size_t index = 0;
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 5; col++) {
            cipher->table[row][col] = alphabet[index++];
        }
    }
}

// Function to encode plaintext using the Polybius cipher
void pbEncode(const char *plaintext, const PolybiusCipher_t *cipher, char *encoded) {
    int index = 0;
    for (size_t i = 0; plaintext[i] != '\0'; i++) {
        char c = toupper(plaintext[i]);

        // Ignore non-alphabetic characters
        if (c < 'A' || c > 'Z') {
            continue;
        }

        // Treat J as I
        if (c == 'J') {
            c = 'I';
        }

        // Find character in table
        for (int row = 0; row < 5; row++) {
            for (int col = 0; col < 5; col++) {
                if (cipher->table[row][col] == c) {
                    encoded[index++] = '1' + row; // Row as number
                    encoded[index++] = '1' + col; // Column as number
                    break;
                }
            }
        }
    }

    encoded[index] = '\0'; // Null-terminate the encoded string
}

// Function to decode ciphertext using the Polybius cipher
void pbDecode(const char *ciphertext, const PolybiusCipher_t *cipher, char *decoded) {
    int index = 0; // Index for the decoded buffer

    // Iterate through the ciphertext in steps of 2
    for (size_t i = 0; ciphertext[i] != '\0'; i += 2) {
        // Skip if the current or next character is not a digit
        if (!isdigit(ciphertext[i]) || !isdigit(ciphertext[i + 1])) {
            fprintf(stderr, "Warning: Skipping invalid character pair at index %zu\n", i);
            continue;
        }

        // Convert characters to row and column indices (0-based)
        int row = ciphertext[i] - '1';
        int col = ciphertext[i + 1] - '1';

        // Check if indices are within bounds
        if (row < 0 || row >= 5 || col < 0 || col >= 5) {
            fprintf(stderr, "Warning: Invalid indices (%d, %d) at index %zu\n", row, col, i);
            continue;
        }

        // Append the decoded character to the buffer
        decoded[index++] = cipher->table[row][col];
    }

    decoded[index] = '\0'; // Null-terminate the decoded string
}
