#include <stdlib.h>
#include "reading_source_file.c"
#include "utility.c"
#include "passing.c"

int main(int argc, char **argv) {
    char** read = splitStringOnWhitespace(argv);
    const char* filename = read[1];
    int lineCount = 0;
    char** lines = readLinesFromFile(filename, &lineCount);
    one_pass(lines, read[2]);
}
