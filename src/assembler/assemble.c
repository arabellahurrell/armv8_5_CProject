#include "assemble.h"

int main(int argc, char **argv) {
    // Get the filename from command line argument
    const char *filename = argv[1];
    // Read lines from the specified file
    char **lines = readLinesFromFile(filename);
    removeMultilineComments(lines, getStringArrayLength(lines));
    for (int i = 0; i < getStringArrayLength(lines); i++) {
        removeComments(lines[i]);
    }
    removeBlankLines(lines, getStringArrayLength(lines));
    int length = countLinesInFile(filename);
    lines[length] = NULL;
    // Alternatively, use the second command line argument as the output file
    one_pass(lines, argv[2]);

    // The rest of the code execution continues...
}