
#include "assemble.h"

int main(int argc, char **argv) {
   const char* filename = argv[1];  // Get the filename from command line argument
    //const char* filename = "add02.s";  // Alternatively, set the filename directly

    char** lines = readLinesFromFile(filename);  // Read lines from the specified file
    int length = countLinesInFile(filename);
    printf("LENGTH OF FILE %d\n", length);
    lines[length] = '\0';
//    for (int i = 0; i < getStringArrayLength(lines); i++) {
//        if (strcmp(lines[i], "and x0, x0, x0") == 0) {
//            printf("%d\n", i);
//            printf("%d\n", length);
//            fflush(stdout);
//            lines[i + 1] = '\0';  // Set the next line to null to effectively remove it
//            break;  // Exit the loop since the desired condition is met
//        }
//    }


    //one_pass(lines, "output.bin");  // Perform a one-pass assembly with modified lines
    one_pass(lines, argv[2]);  // Alternatively, use the second command line argument as the output file

    // The rest of the code execution continues...
}