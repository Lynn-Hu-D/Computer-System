#include "polybius.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <-e|-d> <text> [...<text>]\n", argv[0]);
        return 1;
    }

    PolybiusCipher_t cipher;
    pbInitCipher(&cipher);  // Initialize cipher with standard table

    char mode = argv[1][1];  // 'e' for encode or 'd' for decode

    // Iterate over the arguments starting from the second argument
    for (int i = 2; i < argc; i++) {
        const char *input = argv[i];
        char output[1024];  // Ensure the output buffer is large enough to hold the encoded/decoded string
        
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
    }

    return 0;
}


