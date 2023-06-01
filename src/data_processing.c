//
// Created by Arabella Hurrell on 30/05/2023.
//

#include "data_processing.h"
#include <string.h>
#include "denary_to_binary.c"

char* arithmetics(char* opcode, char* rd, char* rn, char* op2, char* shift) {
    char* x;
    if (strcmp((const char *) rn[0], "x") == 0) {
        x = "1";
    } else {
        x = "0";
    }
    char* result = strcat(strcat(x, strcat(opcode, "100010")), strcat(convert(op2, 16), convert(rd, 4)));
    return result;
}

char* moveWides(char* opcode, char* rd, char* imm, char* sh) {
    char* x;
    if (strcmp(rd, "xd")) {
        x = "1";
    } else {
        x = "0";
    }
    char* result = strcat(strcat(x, strcat(opcode, "101")), strcat(strcat(imm, sh), rd));
    return result;
}

char* logicalBitwise (char* opcode, char* rd, char* rn, char* op2) {
    char* x;
    if (strcmp(rd, "xd")) {
        x = "1";
    } else {
        x = "0";
    }
    char* result = strcat(strcat(x, strcat(opcode, "01010")), strcat(op2, strcat(rn, rd)));
    return result;
}





