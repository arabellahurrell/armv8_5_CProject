
#include "assemble.h"

int main(int argc, char **argv) {
    //char** read = splitStringOnWhitespace(argv);
    const char* filename = "add02.s";
    char** lines = readLinesFromFile(filename);
    one_pass(lines, "output.bin");
}
