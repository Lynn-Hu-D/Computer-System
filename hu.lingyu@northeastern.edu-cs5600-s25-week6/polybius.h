// polybius.h: Header file for Polybius cipher
#ifndef POLYBIUS_H
#define POLYBIUS_H

#include <stddef.h>

// Define a struct to represent the Polybius cipher state
typedef struct {
    char table[5][5];  // 5x5 Polybius cipher table
} PolybiusCipher_t;

// Function to initialize the cipher table with a specific alphabet
void pbInitCipher(PolybiusCipher_t *cipher);

// Function to encode plaintext using the Polybius cipher
void pbEncode(const char *plaintext, const PolybiusCipher_t *cipher, char *encoded);

// Function to decode ciphertext using the Polybius cipher
void pbDecode(const char *ciphertext, const PolybiusCipher_t *cipher, char *decoded);

#endif // POLYBIUS_H
