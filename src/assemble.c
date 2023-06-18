
#include "assemble.h"

int main(int argc, char **argv) {
    const char* filename = argv[1];
    char** lines = readLinesFromFile(filename);
    one_pass(lines, argv[2]);
}