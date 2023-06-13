
#include "assemble.h"

int main(int argc, char **argv) {
    //char** read = splitStringOnWhitespace(argv);
    printf("Program started\n");
    const char* filename = "add02.s";
    char** lines = readLinesFromFile(filename);
    printf("File Read\n");
    one_pass(lines, "output.bin");
    printf("Passed\n");
}
