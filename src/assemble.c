
#include "assemble.h"

int main(int argc, char **argv) {
    const char* filename = argv[1];
    int lineCount = 0;
    char** lines = readLinesFromFile(filename, &lineCount);
    one_pass(lines, argv[2]);
}

//./run -Apf -a ../armv8_5/assemble
