
#include "assemble.h"

int main(int argc, char **argv) {
    //char** read = splitStringOnWhitespace(argv);
    const char* filename = argv[1];//"add02.s";
    char** lines = readLinesFromFile(filename);
    //FILE* file = fopen("output.bin", "w");
    one_pass(lines, argv[2]);//"output.bin");
}
