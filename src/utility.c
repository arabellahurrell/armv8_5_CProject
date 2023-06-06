//
// Created by Arabella Hurrell on 05/06/2023.
//

#include "utility.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
int binaryToDecimal(const char* binary) {
    int binaryLength = strlen(binary);
    int decimal = 0;
    int power = 0;

    // Iterate over the binary digits in reverse order
    for (int i = binaryLength - 1; i >= 0; i--) {
        if (binary[i] == '1') {
            decimal += pow(2, power);
        }
        power++;
    }

    return decimal;
}

char* hexToBinary(const char* hex) {
    int hexLength = strlen(hex);
    int j = hexLength - 1;
    int resultSize = (hexLength - 2) * 4;
    char* binary = (char*) malloc((resultSize + 1) * sizeof(char));
    binary[resultSize] = '\0';
    for (int i = 2 ; i <= j; i++) {
        char c = hex[i];
        int value = 0;

        if (c >= '0' && c <= '9') {
            value = c - '0';
        } else if (c >= 'A' && c <= 'F') {
            value = c - 'A' + 10;
        } else if (c >= 'a' && c <= 'f') {
            value = c - 'a' + 10;
        }
        for (int j = 0; j < 4; j++) {
            int bit = (value >> (3 - j)) & 1;
            binary[(i - 2) * 4 + j] = bit + '0';
        }
    }

    return binary;
}

char* convert(char* denary, int numBits) {
    const char* numberStr = denary + 1;
    int decimal = atoi(numberStr);
    char* binary = (char*)malloc(sizeof(char) * (numBits + 1));
    for (int i = numBits - 1; i >= 0; i--) {
        binary[i] = (decimal & 1) ? '1' : '0';
        decimal >>= 1;
    }
    binary[numBits] = '\0';

    return binary;
}

const char * truncateString(char* str, int length) {
    int strLength = strlen(str);

    if (strLength > length) {
        str[length] = '\0';
    }
}

char * sf (char* reg) {
    char* x;
    if (strcmp((const char *) reg[0], "x") == 0) {
        x = "1";
    } else {
        x = "0";
    }
    return x;
}

#include <stdbool.h>
#include <stdio.h>

bool isSecondCharacterX(char* str) {
    if (str[1] == 'x') {
        return true;
    } else {
        return false;
    }
}

char* replaceSubstring(const char* str, const char* oldSubstr, const char* newSubstr) {
    size_t strLength = strlen(str);
    size_t oldSubstrLength = strlen(oldSubstr);
    size_t newSubstrLength = strlen(newSubstr);
    size_t count = 0;
    const char* ptr = str;
    while ((ptr = strstr(ptr, oldSubstr)) != NULL) {
        count++;
        ptr += oldSubstrLength;
    }
    size_t newLength = strLength + count * (newSubstrLength - oldSubstrLength);
    char* result = (char*) malloc((newLength + 1) * sizeof(char));
    size_t resultIndex = 0;
    ptr = str;
    while (count > 0 && (ptr = strstr(ptr, oldSubstr)) != NULL) {
        size_t partLength = ptr - str;
        strncpy(result + resultIndex, str, partLength);
        resultIndex += partLength;
        strncpy(result + resultIndex, newSubstr, newSubstrLength);
        resultIndex += newSubstrLength;
        ptr += oldSubstrLength;
        count--;
    }
    strncpy(result + resultIndex, ptr, strLength - (ptr - str));
    result[newLength] = '\0';

    return result;
}

bool isSubstringInString(const char* str, const char* substr) {
    return strstr(str, substr) != NULL;
}

char* decimalToHexadecimal(int decimal) {
    char* hexadecimal = (char*)malloc(9 * sizeof(char));
    sprintf(hexadecimal, "%X", decimal);
    return hexadecimal;
}

