//
// This file holds the code responsible for reading the source file into an array
//

int isAlphaInLine(char* line) {
    for (int i = 0; i < strlen(line); i++) {
        if (isalpha(line[i]) || line[i] == '.' || line[i] == '*' || line[i] == '/') return i;
    }
    return -1;
}

char** readLinesFromFile(const char* filename) {
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

    while (fgets(line, MAX_LINE_LENGTH, file)) {
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

        if(!isalpha(line[0]) && line[0] != '.' && line[0] != '*' && line[0] != '/') {
            int indexOfChar = isAlphaInLine(line);
            if (indexOfChar != -1) {
                const char *line1;
                line1 = line + indexOfChar;
                strcpy(lines[count], line1);
                count++;
            }
        } else {
            // Copy the line to the array
            strcpy(lines[count], line);
            count++;
        }

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

    return lines;
}
