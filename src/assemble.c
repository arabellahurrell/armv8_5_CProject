
#include "assemble.h"

int main(int argc, char **argv) {
    //char** read = splitStringOnWhitespace(argv);
    const char* filename = "add02.s";//"add02.s";
    char** lines = readLinesFromFile(filename);
    //FILE* file = fopen("output.bin", "w");
    one_pass(lines, "output.bin");//"output.bin");
}