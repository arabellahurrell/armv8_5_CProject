
#include "assemble.h"

int main(int argc, char **argv) {
    char** read = splitStringOnWhitespace(argv);
    const char* filename = read[1];
    int lineCount = 0;
    char** lines = readLinesFromFile(filename, &lineCount);
    FILE* file = fopen(read[2], "wb");
    one_pass(lines, read[2]);
    fclose(file);
}
