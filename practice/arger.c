#include <stdio.h>

void to_uppercase(char *str);
void to_lowercase(char *str);
void capitalize(char *str);

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Error: Insufficient arguments.\n");
        printf("Usage: arger [-u | -l | -cap] text...\n");
        return -1;
    }

    // Get the first option of the argument
    char *option = argv[1]; // -u -l -cap

    // cannot use strio.h
    // if (strcmp(option, "-u") == 0) {

    // } else if (strcmp(option, "-l") == 0) {

    // } else if (strcmp(option, "-cap") == 0) {

    // } else {

    //     return -1;
    // }

    // " help is on the WAY"
    for (int i = 2; i < argc; i++) {
        if (i > 2) {
            putchar(' ');
        }

        if (option[0] == '-' && option[1] == 'u') {
            to_uppercase(argv[i]);
        } else if (option[0] == '-' && option[1] == 'l') {
            to_lowercase(argv[i]);
        } else if (option[0] == '-' && option[1] == 'c' && option[2] == 'a' && option[3] == 'p') {
            capitalize(argv[i]);
        } else {
            printf("Error: Invalid flag '%s'.\n", option);
            return -1;
        }
    }

    return 0;
}

// Convert a string to uppercase
void to_uppercase(char *str) {
    while (*str) {
        if (*str >= 'a' && *str <= 'z') {
            putchar(*str - 32); // Convert to uppercase
        } else {
            putchar(*str); // Leave other characters unchanged
        }
        str++;
    }
}

// Convert a string to lowercase
void to_lowercase(char *str) {
    while (*str) {
        if (*str >= 'A' && *str <= 'Z') {
            putchar(*str + 32); // Convert to lowercase
        } else {
            putchar(*str); // Leave other characters unchanged
        }
        str++;
    }
}

// Capitalize the first letter of each word, make others lowercase
void capitalize(char *str) {
    int is_start_of_word = 1;

    while (*str) {
        if (*str == ' ') {
            putchar(*str); // Keep spaces
            is_start_of_word = 1; // Next character is the start of a new word
        } else if (is_start_of_word && *str >= 'a' && *str <= 'z') {
            putchar(*str - 32); // Capitalize the first letter
            is_start_of_word = 0;
        } else if (!is_start_of_word && *str >= 'A' && *str <= 'Z') {
            putchar(*str + 32); // Convert other letters to lowercase
        } else {
            putchar(*str); // Leave other characters unchanged
            is_start_of_word = 0;
        }
        str++;
    }
}
