//
// Created by Arabella Hurrell on 30/05/2023.
//

#include "reading_source_file.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int isLabel (const char* line) {
    if (line[strlen(line) - 1] == ':') {
        return 1;
    }
    return 0;
}

int isDirective (const char* line) {
    if (line[0] == '.') {
        return 1;
    }
    return 0;
}

#define MAX_LINE_LENGTH 256
#define INITIAL_ARRAY_SIZE 10

char** readLinesFromFile(const char* filename, int* lineCount) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        return NULL;
    }

    int arraySize = INITIAL_ARRAY_SIZE;
    char** lines = malloc(arraySize * sizeof(char*));
    if (lines == NULL) {
        printf("Memory allocation failed.\n");
        fclose(file);
        return NULL;
    }

    char line[MAX_LINE_LENGTH];
    int count = 0;

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        // Remove trailing newline character
        line[strcspn(line, "\n")] = '\0';

        // Allocate memory for the line
        lines[count] = malloc((strlen(line) + 1) * sizeof(char));
        if (lines[count] == NULL) {
            printf("Memory allocation failed.\n");
            fclose(file);
            free(lines);
            return NULL;
        }

        // Copy the line to the array
        strcpy(lines[count], line);

        count++;

        // Resize the array if necessary
        if (count >= arraySize) {
            arraySize *= 2;
            char** resizedLines = realloc(lines, arraySize * sizeof(char*));
            if (resizedLines == NULL) {
                printf("Memory reallocation failed.\n");
                fclose(file);
                free(lines);
                return NULL;
            }
            lines = resizedLines;
        }
    }

    fclose(file);

    *lineCount = count;

    return lines;
}

void freeLinesArray(char** lines, int lineCount) {
    for (int i = 0; i < lineCount; i++) {
        free(lines[i]);
    }
    free(lines);
}

int main() {
    const char* filename = "example.s";
    int lineCount = 0;
    char** lines = readLinesFromFile(filename, &lineCount);

    if (lines != NULL) {
        printf("Lines read from file:\n");
        for (int i = 0; i < lineCount; i++) {
            printf("%s\n", lines[i]);
        }
        // Free the memory allocated for lines
        freeLinesArray(lines, lineCount);
    }



    return 0;
}