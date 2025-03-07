#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Usage: %s <number_of_integers> <file_name> [-a]\n", argv[0]);
        return 1;
    }

    int numIntegers = atoi(argv[1]);
    if (numIntegers <= 0) {
        fprintf(stderr, "Error: Number of integers must be greater than 0.\n");
        return 1;
    }

    const char *fileName = argv[2];
    int appendMode = (argc == 4 && strcmp(argv[3], "-a") == 0);

    // Open the file in the appropriate mode
    FILE *file = fopen(fileName, appendMode ? "a" : "w");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    // Set the seed for reproducibility
    setSeed((unsigned long long)time(NULL));

    // Generate and write random numbers to the file
    for (int i = 0; i < numIntegers; i++) {
        int randomNumber = genRand(-100, 100); // Random numbers between MIN and Max
        fprintf(file, "%d\n", randomNumber);
    }

    fclose(file);
    printf("Successfully wrote %d random integers to %s\n", numIntegers, fileName);

    return 0;
}
