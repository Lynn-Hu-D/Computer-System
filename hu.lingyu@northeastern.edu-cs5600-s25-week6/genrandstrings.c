#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_STRINGS 10000
#define MAX_STRING_LENGTH 10
#define MIN_STRING_LENGTH 5

// Variables for the Middle Square Weyl Sequence generator
static unsigned long long state = 0;  // Seed state
static unsigned long long weyl = 0xb5ad4eceda1ce2a9ULL; // Weyl constant

// Middle Square Weyl Sequence (MSWS) RNG function
int genRand(int min, int max) {
    state *= state;                         // Square the state
    state += (weyl += 0xb5ad4eceda1ce2a9ULL); // Add Weyl constant
    state = (state >> 32) | (state << 32);  // Scramble bits


    // Handle negative ranges
    int range = max - min + 1;
    int randomValue = min + (int)(state % range); // Scale to range
    return randomValue;
}

// Function to set the seed for MSWS
void setSeed(unsigned long long newSeed) {
    state = newSeed;
}

// Function to generate a random character A-Z
char generateRandomChar() {
    return 'A' + genRand(0, 25);
}

// Function to generate a random string of given length
void generateRandomString(char *str, int length) {
    for (int i = 0; i < length; i++) {
        str[i] = generateRandomChar();
    }
    str[length] = '\0';
}

// Function to write random strings to a file
void generateRandomStringsToFile(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Unable to open file");
        exit(1);
    }

    // Set the seed for reproducibility
    setSeed((unsigned long long)time(NULL));

    char str[MAX_STRING_LENGTH + 1];  // +1 for the null terminator
    for (int i = 0; i < NUM_STRINGS; i++) {
        // string length is between 5 to 10.
        int string_length = genRand(MIN_STRING_LENGTH, MAX_STRING_LENGTH);
        generateRandomString(str, string_length);
        fprintf(file, "%s\n", str);
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    generateRandomStringsToFile("random_strings.txt");

    return 0;
}
