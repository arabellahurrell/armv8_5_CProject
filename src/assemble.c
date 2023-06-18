
#include "assemble.h"

int main(int argc, char **argv) {
//    const char* filename = argv[1];
    const char* filename = "add02.s";
    char** lines = readLinesFromFile(filename);
    for (int i = 0; i < getStringArrayLength(lines); i++) {
        if (strcmp(lines[i], "and x0, x0, x0") == 0) {
            lines[i + 1] = '\0';
            break;
        }
    }
    one_pass(lines, "output.bin");
//    one_pass(lines, argv[2]);
}