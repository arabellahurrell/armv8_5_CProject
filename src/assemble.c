
#include "assemble.h"

int main(int argc, char **argv) {
   const char* filename = argv[1];  // Get the filename from command line argument
    //const char* filename = "add02.s";  // Alternatively, set the filename directly
    char** lines = readLinesFromFile(filename);  // Read lines from the specified file
    int length = countLinesInFile(filename);
    lines[length] = '\0';
    //one_pass(lines, "output.bin");  // Perform a one-pass assembly with modified lines
    one_pass(lines, argv[2]);  // Alternatively, use the second command line argument as the output file

    // The rest of the code execution continues...
}