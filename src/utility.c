//
// Created by Arabella Hurrell on 05/06/2023.
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

char* charToString (char c) {
    char* str = malloc(2 * sizeof(char));
    str[0] = c;
    str[1] = '\0';
    return str;
}

char* duplicateString(const char* originalString) {
    size_t length = strlen(originalString) + 1; // Include space for the null terminator

    char* duplicatedString = malloc(length); // Allocate memory for the duplicated string

    if (duplicatedString != NULL) {
        strcpy(duplicatedString, originalString); // Copy the original string into the new memory
    }

    return duplicatedString;
}

char* intToString(int num) {
    char* str = malloc(12 * sizeof(char)); // Allocate memory for the string
    sprintf(str, "%d", num); // Convert the integer to a string
    return str;
}

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
        for (int k = 0; k < 4; k++) {
            int bit = (value >> (3 - k)) & 1;
            binary[(i - 2) * 4 + k] = bit + '0';
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

char* truncateString(char* str, int length) {
    int strLength = strlen(str);

    if (strLength > length) {
        str[length] = '\0';
    }
    return str;
}

char * sf (char* reg) {
    char* x;
    if (reg[0] == 'x') {
        x = "1";
    } else {
        x = "0";
    }
    return x;
}


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

bool removeNonAlphaNumeric(char c) {
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || (c == '-') || (c == '!') || (c == '.')) {
            return true;
        }
        return false;
    }


char** splitStringOnWhitespace(char* str) {
    int count = 0;
    const char* delimiter = " ";
    char* strCopy = duplicateString(str);
    char* word = strtok(strCopy, delimiter);

    while (word != NULL) {
        count++;
        word = strtok(NULL, delimiter);
    }
    char** words = (char**)malloc((count + 1) * sizeof(char*));
    strCopy = duplicateString(str);
    word = strtok(strCopy, delimiter);
    int index = 0;

    while (word != NULL) {
        int len = strlen(word);
        char* cleanWord = (char*)malloc((len + 1) * sizeof(char));
        int cleanIndex = 0;

        for (int i = 0; i < len; i++) {
            if (removeNonAlphaNumeric(word[i])) {
                cleanWord[cleanIndex++] = word[i];
            }
        }

        cleanWord[cleanIndex] = '\0';

        words[index] = cleanWord;
        index++;
        word = strtok(NULL, delimiter);
    }

    words[index] = NULL;

    free(strCopy);
    return words;
}

size_t getStringArrayLength(char** array) {
    size_t length = 0;

    if (array != NULL) {
        while (array[length] != NULL) {
            length++;
        }
    }

    return length;
}

char** splitStringOnFirstSpace(const char* input) {
    char** output = malloc(2 * sizeof(char*));
    output[0] = NULL;
    output[1] = NULL;

    int length = strlen(input);
    int splitIndex = -1;

    for (int i = 0; i < length; i++) {
        if (input[i] == ' ') {
            splitIndex = i;
            break;
        }
    }
    if (splitIndex != -1) {
        output[0] = malloc((splitIndex + 1) * sizeof(char));
        strncpy(output[0], input, splitIndex);
        output[0][splitIndex] = '\0';

        int remainingLength = length - splitIndex - 1;
        output[1] = malloc((remainingLength + 1) * sizeof(char));
        strcpy(output[1], input + splitIndex + 1);
    } else {
        output[0] = malloc((length + 1) * sizeof(char));
        strcpy(output[0], input);
    }

    return output;
}
