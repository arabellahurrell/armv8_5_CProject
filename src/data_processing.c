//
// Created by Arabella Hurrell on 30/05/2023.
//

#include "data_processing.h"
#include <string.h>

char* arithmetics(char* opcode, char* rd, char* rn, char* op2) {
    char* x;
    if (strcmp(rn, "xn") == 0) {
        x = "1";
    } else {
        x = "0";
    }
    char* result = strcat(strcat(x, strcat(opcode, "100010")), strcat(op2, rd));
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





