//
// Created by Arabella Hurrell on 01/06/2023.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* convert(const char* denary, int numBits) {
    const char* numberStr = denary + 1;
    int decimal = atoi(numberStr);
    char* binary = (char*)malloc(sizeof(char) * (numBits + 1));
    if (binary == NULL) {
        printf("Memory allocation failed.\n");
        return NULL;
    }
    for (int i = numBits - 1; i >= 0; i--) {
        binary[i] = (decimal & 1) ? '1' : '0';
        decimal >>= 1;
    }
    binary[numBits] = '\0';

    return binary;
}

