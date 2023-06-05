//
// Created by Arabella Hurrell on 05/06/2023.
//

#include "hex_to_binary.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>


void hexToBinary(const char* hex) {
    int hexLength = strlen(hex);

    if (hexLength < 3 || hex[0] != '0' || hex[1] != 'x') {
        printf("Invalid hexadecimal format. Please provide a number in the format '0x<HexNumber>'.\n");
        return;
    }

    int j = hexLength - 1;


    for (int i = 2; i <= j; i++) {
        char c = hex[i];
        int value = 0;

        if (c >= '0' && c <= '9') {
            value = c - '0';
        } else if (c >= 'A' && c <= 'F') {
            value = c - 'A' + 10;
        } else if (c >= 'a' && c <= 'f') {
            value = c - 'a' + 10;
        } else {
            printf("Invalid hexadecimal digit: %c\n", c);
            return;
        }


        for (int j = 3; j >= 0; j--) {
            int bit = (value >> j) & 1;
            printf("%d", bit);
        }
    }

    printf("\n");
}