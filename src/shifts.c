//
// Created by Arabella Hurrell on 01/06/2023.
//
//#include <string.h>
//#include <stdlib.h>

char* master(char* value, char* type, char* amount) {
    if (strcmp(amount, "0") == 0) {
        return value;
    }
    int length = strlen(value);
    int am = atoi(amount);
    char* shifted = malloc((length + 1) * sizeof(char));
    if(strcmp(type, "lsl") == 0) {
        am %= length; // Ensure the amount is within the string length

        // Copy the characters from the original string to the shifted string
        strncpy(shifted, value + am, length - am);
        strncpy(shifted + length - am, value, am);

        shifted[length] = '\0';
    } else if (strcmp(type, "lsr") == 0 || strcmp(type, "asr") == 0) {
        int num = strtol(value, NULL, 2);
        int shiftedNum = num >> am;
        for (int i = length - 1; i >= 0; i--) {
            shifted[i] = (shiftedNum & 1) + '0';
            shiftedNum >>= 1;
        }
        shifted[length] = '\0';
    } else {
        am %= length;
        strncpy(shifted, value + length - am, am);
        strncpy(shifted + am, value, length - am);
        shifted[length] = '\0';
    }
    return shifted;
}
