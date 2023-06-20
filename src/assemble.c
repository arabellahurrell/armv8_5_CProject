
#include "assemble.h"

int main(int argc, char **argv) {
    // Get the filename from command line argument
    const char *filename = argv[1];
    // Alternatively, set the filename directly
//    const char* filename = "add02.s";

    // Read lines from the specified file
    char **lines = readLinesFromFile(filename);

    for (int i = 0; i < getStringArrayLength(lines); i++) {
        printf("line = %s\n", lines[i]);  // Print the current line

        if (strcmp(lines[i], "and x0, x0, x0") == 0) {
            // Set the next line to null to effectively remove it
            lines[i + 1] = '\0';
            // Exit the loop since the desired condition is met
            break;
        }
    }

    // Perform a one-pass assembly with modified lines
    one_pass(lines, "output.bin");
    // Alternatively, use the second command line argument as the output file
//    one_pass(lines, argv[2]);

    // The rest of the code execution continues...
}