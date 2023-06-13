
#include "assemble.h"

int main(int argc, char **argv) {
    //char** read = splitStringOnWhitespace(argv);
    printf("Program started\n");
    const char* filename = argv[1];
    char** lines = readLinesFromFile(filename);
    printf("File Read\n");
    fopen(argv[2], "w");
    one_pass(lines, argv[2]);
    printf("Passed\n");
}
