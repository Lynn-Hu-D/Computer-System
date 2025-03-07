#include "polybius.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <-e|-d> <text>\n", argv[0]);
        return 1;
    }

    PolybiusCipher_t cipher;
    pbInitCipher(&cipher);  // Initialize cipher with standard table

    char mode = argv[1][1];
    const char *input = argv[2];
    char output[1024];

    if (mode == 'e') {
        pbEncode(input, &cipher, output);
        printf("Encoded: %s\n", output);
    } else if (mode == 'd') {
        pbDecode(input, &cipher, output);
        printf("Decoded: %s\n", output);
    } else {
        fprintf(stderr, "Invalid mode. Use -e for encode or -d for decode.\n");
        return 1;
    }

    return 0;
}


