
#include "assemble.h"

int main(int argc, char **argv) {
    //char** read = splitStringOnWhitespace(argv);
    const char* filename = "add02.s";
    int lineCount = 0;
    char** lines = readLinesFromFile(filename, &lineCount);
    one_pass(lines, "output.bin");
}
