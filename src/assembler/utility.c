//
// Created by Arabella Hurrell on 05/06/2023.

// Converts a character to a string

#define MAX_LINE_LENGTH 256

char *truncateString(const char *str, int length) {
    int strLength = strlen(str);
    if (strLength > length) {
        // Remove extra leading zeros if the original string is larger
        // than the desired size
        int leadingZeros = 0;
        int i = 0;
        while (str[i] == '0' && i < strLength) {
            leadingZeros++;
            i++;
        }

        char *truncatedStr = malloc((length + 1) * sizeof(char));

        // if (leadingZeros > 1) {
        //     leadingZeros = 1;
        // }
        leadingZeros--;

        strncpy(truncatedStr, str + leadingZeros, length);
        truncatedStr[length] = '\0';
        return truncatedStr;
    } else {
        // Add leading zeros
        int leadingZeros = length - strLength;
        int newLength = strLength + leadingZeros;

        char *truncatedStr = malloc((newLength + 1) * sizeof(char));

        for (int i = 0; i < leadingZeros; i++) {
            truncatedStr[i] = '0';
        }

        strncpy(truncatedStr + leadingZeros, str, strLength);
        truncatedStr[newLength] = '\0';
        return truncatedStr;
    }
}

char *charToString(char c) {
    char *str = malloc(2 * sizeof(char));
    str[0] = c;
    str[1] = '\0';
    return str;
}

// Duplicates a string
char *duplicateString(const char *originalString) {
    // Include space for the null terminator
    size_t length = strlen(originalString) + 1;
    // Allocate memory for the duplicated string
    char *duplicatedString = malloc(length);
    if (duplicatedString != NULL) {
        // Copy the original string into the new memory
        strcpy(duplicatedString, originalString);
    }
    return duplicatedString;
}

// Converts an integer to a string
char *intToString(int num) {
    // Allocate memory for the string
    char *str = malloc(12 * sizeof(char));
    // Convert the integer to a string
    sprintf(str, "%d", num);
    return str;
}

// Converts a binary string to a decimal integer
int binaryToDecimal(const char *binary) {
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

// Converts a hexadecimal string to a binary string
char *hexToBinary(char *hex) {
    int hexLength = strlen(hex);
    int resultSize = (hexLength - 2) * 4;
    char *binary = (char *) malloc((resultSize + 1) * sizeof(char));
    for (int i = 2; i < hexLength; i++) {
        char c = hex[i];
        int value;
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

char *convert(char *denary, int numBits) {
    const char *numberStr;
    if (denary[0] != 'x' && denary[0] != 'w') {
        numberStr = denary;
    } else {
        numberStr = denary + 1;
    }
    int decimalValue = atoi(numberStr);
    int binaryLength = 0;
    int temp = decimalValue;

    // Count the number of bits required
    while (temp > 0) {
        binaryLength++;
        temp >>= 1;
    }

    // Determine the effective number of bits to consider
    int effectiveBits = (binaryLength > numBits) ? binaryLength : numBits;

    // Allocate memory for the binary string
    char *binaryString = (char *) malloc((effectiveBits + 1) * sizeof(char));
    if (!binaryString) {
        printf("malloc error\n");
        return NULL;
    }

    binaryString[effectiveBits] = '\0';

    // Convert decimal to binary
    for (int i = effectiveBits - 1; i >= 0; i--) {
        binaryString[i] = (decimalValue & 1) ? '1' : '0';
        decimalValue >>= 1;
    }

    // Allocate memory for the binary array
    char *binary = (char *) malloc((numBits + 1) * sizeof(char));
    if (!binary) {
        printf("malloc failed\n");
        free(binaryString);
        return NULL;
    }

    binary = truncateString(binaryString, numBits);

    // Copy binaryString to binary array, adding leading zeros if necessary
//    for (int i = 0; i < numBits; i++) {
//
//        fflush(stdout);
//        if (i < effectiveBits) {
//            binary[i] = binaryString[i];
//        } else {
//            binary[i] = '0';
//        }
//    }

    binary[numBits] = '\0';
    free(binaryString); // Free the memory allocated for binaryString
    return binary;
}


//char* truncateString(char* str, int length) {
//    int strLength = strlen(str);
//    if (strLength > length) {
//        str[length] = '\0';
//        return str;
//    }
//    else if (length > strLength) {
//        char* res = malloc((length + 1) * sizeof(char));
//        int temp = length - strLength;
//        for(int i = 0; i < temp; i++) {
//            res[i] = '0';
//        }
//        res[temp] = '\0';
//        strcat(res, str);
//        return res;
//    }
//    return str;
//}




char *sf(char *reg) {
    char *x;
    if (tolower(reg[0]) == 'x') {
        x = "1";
    } else {
        x = "0";
    }
    return x;
}


int immOrHex(char *str) {
    if (strlen(str) <= 1) {
        return 0;
    }
    if (str[1] == 'x') {
        return binaryToDecimal(hexToBinary(str));
    } else {
//        char* res = malloc(strlen(str) - 1);
//        for (int i = 0; i < strlen(str) - 1; i++) {
//            res[i] = str[i+1];
//        }
        return atoi(str);
    }
}

char *immConverter(char *str) {
    if (strlen(str) <= 1) {
        return 0;
    }
    if (str[1] == 'x') {
        return hexToBinary(str);
    } else {
        //char* res = malloc(17 * sizeof(char));
        char *res = convert(str, 16);
        return res;
    }
}

char *replaceSubstring(const char *str, const char *oldSubstr,
                       const char *newSubstr) {
    size_t strLength = strlen(str);
    size_t oldSubstrLength = strlen(oldSubstr);
    size_t newSubstrLength = strlen(newSubstr);
    size_t count = 0;
    const char *ptr = str;
    while ((ptr = strstr(ptr, oldSubstr)) != NULL) {
        count++;
        ptr += oldSubstrLength;
    }
    size_t newLength = strLength + count * (newSubstrLength - oldSubstrLength);
    char *result = (char *) malloc((newLength + 1) * sizeof(char));
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

bool isSubstringInString(const char *str, const char *substr) {
    return strstr(str, substr) != NULL;
}

char *decimalToHexadecimal(int decimal) {
    // Maximum 8 digits for hexadecimal representation + 2 characters for
    // "0x" + 1 character for null terminator
    char *hexString = malloc(11 * sizeof(char));

    if (hexString == NULL) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    sprintf(hexString, "0x%X", decimal);
    return hexString;
}


bool removeNonAlphaNumeric(char c) {
//    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
//        (c >= '0' && c <= '9') || (c == '-') || (c == '!') || (c == '.') ||
//        (c == ':') || (c == ']') || (c == '*') || (c == '/')) {
//        return true;
//    }
//    return false;
    return !((c == '[') || (c == '#'));

}


char **splitStringOnWhitespace(char *str) {
    int count = 0;
    const char *delimiter = " ,";
    char *strCopy = duplicateString(str);
    char *word = strtok(strCopy, delimiter);

    while (word != NULL) {
        count++;
        word = strtok(NULL, delimiter);
    }
    char **words = (char **) malloc((count + 1) * sizeof(char *));
    strCopy = duplicateString(str);
    word = strtok(strCopy, delimiter);
    int index = 0;

    while (word != NULL) {
        int len = strlen(word);
        char *cleanWord = (char *) malloc((len + 1) * sizeof(char));
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

size_t getStringArrayLength(char **array) {
    size_t length = 0;

    if (array != NULL) {
        while (array[length] != NULL) {
            length++;
        }
    }

    return length;
}

char **splitStringOnFirstSpace(const char *input) {
    char **output = malloc(2 * sizeof(char *));
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

char *registerConvert(char *r) {
    if (strcmp(r, "xzr") == 0 || strcmp(r, "wzr") == 0) {
        return "11111";
    } else {
        return convert(r, 5);
    }
}

char *immHexToBinary(char *value, int x) {
    char *v = malloc((x + 1) * sizeof(char));
    if (value[1] == 'x') {
        v = convert(intToString(strtol(value, NULL, 16)), x);
        //v = truncateString(hexToBinary(value), x);
    } else {
        v = convert(value, x);
    }

    return v;
}

int calculateOffset(char *label_address, char *branch_address) {
    int x;
    if (label_address[1] == 'x') {
        x = strtol(label_address, NULL, 16);
    } else {
        x = atoi(label_address);
    }
    int y;
    if (branch_address[1] == 'x') {
        y = strtol(branch_address, NULL, 16);
    } else {
        y = atoi(branch_address);
    }
    int offset =  (x - y)/4;
    return offset;
}

int countLinesInFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return -1;  // Return -1 to indicate an error
    }

    int count = 0;
    int lastNonEmptyLine = 0;  // Index of the last non-empty line
    char buffer[256];  // Buffer to store each line

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        if (buffer[0] != '\n') {
            count++;  // Increment count for non-empty lines
            lastNonEmptyLine = count;  // Update the last non-empty line index
        } else {
            //count++;  // Increment count for empty lines
        }
    }

    fclose(file);
    return lastNonEmptyLine;
}

char* replaceWord(const char* sentence, const char* wordToReplace, const char* newWord) {
    const char* delimiter = " ,";
    char* result = NULL;
    char* token;
    char* rest = strdup(sentence);

    while ((token = strtok_r(rest, delimiter, &rest))) {
        if (strcmp(token, wordToReplace) == 0) {
            if (result == NULL) {
                result = strdup(newWord);
            } else {
                size_t currentLen = strlen(result);
                size_t newLen = currentLen + strlen(newWord) + 1;
                result = realloc(result, newLen);
                strcat(result, " ");
                strcat(result, newWord);
            }
        } else {
            if (result == NULL) {
                result = strdup(token);
            } else {
                size_t currentLen = strlen(result);
                size_t tokenLen = strlen(token);
                size_t newLen = currentLen + tokenLen + 1;
                result = realloc(result, newLen);
                strcat(result, " ");
                strcat(result, token);
            }
        }
    }

    return result;
}

void removeComments(char* line) {
    bool inComment = false;
    char* commentStart = strstr(line, "//");

    if (commentStart != NULL) {
        *commentStart = '\0';  // Truncate the line at the comment start
    }

    if (inComment) {
        char* lineEnd = line + strlen(line) - 1;
        while (lineEnd >= line && (*lineEnd == ' ' || *lineEnd == '\t' || *lineEnd == '\n')) {
            *lineEnd = '\0';  // Trim any trailing whitespace characters
            lineEnd--;
        }
    }
}

void removeMultilineComments(char** lines, int numLines) {
    int i;
    int inComment = 0;

    for (i = 0; i < numLines; i++) {
        char *line = lines[i];
        int lineLength = strlen(line);

        if (!inComment) {
            char *startComment = strstr(line, "/*");
            char *endComment = strstr(line, "*/");

            if (startComment != NULL && endComment != NULL) {
                // Remove entire comment on a single line
                memmove(startComment, endComment + 2, line + lineLength - endComment - 1);
                lineLength -= (endComment - startComment + 2);
            } else if (startComment != NULL) {
                // Remove start of multiline comment
                memmove(startComment, startComment + 2, line + lineLength - startComment - 1);
                inComment = 1;
                lineLength -= 2;
            }
        }

        if (inComment) {
            char *endComment = strstr(line, "*/");

            if (endComment != NULL) {
                // Remove end of multiline comment
                memmove(line, endComment + 2, line + lineLength - endComment - 1);
                lineLength -= (endComment - line + 2);
                inComment = 0;

                // Remove any remaining characters after the end of the comment
                line[lineLength] = '\0';
            } else {
                // Remove entire line inside the multiline comment
                line[0] = '\0';
                lineLength = 0;
            }
        }

        // Trim any trailing whitespace or newline characters
        while (lineLength > 0 && (line[lineLength - 1] == ' ' || line[lineLength - 1] == '\t' || line[lineLength - 1] == '\n')) {
            line[--lineLength] = '\0';
        }
    }
}

void removeBlankLines(char *strings[], int length) {
    int i, j;
    bool blank;

    for (i = 0; i < length; i++) {
        blank = true;

        // Check if the current string is blank
        for (j = 0; j < strlen(strings[i]); j++) {
            if (strings[i][j] != ' ' && strings[i][j] != '\t' && strings[i][j] != '\n') {
                blank = false;
                break;
            }
        }

        // Shift non-blank lines up to replace the blank line
        if (blank) {
            for (j = i; j < length - 1; j++) {
                strings[j] = strings[j + 1];
            }
            length--; // Reduce the length of the array
            i--; // Recheck the same index in the next iteration
        }
    }
}