//
// Created by Arabella Hurrell on 30/05/2023.
//

#include "data_processing.h"
#include <string.h>
#include "denary_to_binary.c"
#include "shifts.c"
char* arithmetics(char* opcode, char* rd, char* rn, char* op2, char* shiftType, char* shiftAmount) {
    char* x;
    if (strcmp((const char *) rn[0], "x") == 0) {
        x = "1";
    } else {
        x = "0";
    }
    char* result = strcat(strcat(x, strcat(opcode, "100010")), strcat(master(convert(op2,18), shiftType, shiftAmount) + 10, convert(rd, 5)));
    return result;
}

char* moveWides(char* opcode, char* rd, char* imm, char* sh, char* shiftType, char* shiftAmount) {
    char* x;
    if (strcmp((const char *) rd[0], "x") == 0) {
        x = "1";
    } else {
        x = "0";
    }
    char* result = strcat(strcat(x, strcat(opcode, "101")), strcat(strcat(master(convert(imm, 16), shiftType, shiftAmount), sh), convert(rd, 16)));
    return result;
}

char* logicalBitwise (char* mnemonic, char* rd, char* rn, char* rm, char* op2, char* shiftType) {
    char* x;
    if (strcmp((const char *) rd[0], "x") == 0) {
        x = "1";
    } else {
        x = "0";
    }
    char* shifts[3];
    if (strcmp(shiftType, "lsl") == 0) {
        shifts[0] = "0";
        shifts[1] = "0";
    } else if (strcmp(shiftType, "lsr") == 0) {
        shifts[0] = "0";
        shifts[1] = "1";
    } else if (strcmp(shiftType, "asr") == 0) {
        shifts[0] = "1";
        shifts[1] = "0";
    } else {
        shifts[0] = "1";
        shifts[1] = "1";
    }
    if (strcmp(mnemonic, "and") ==0 || strcmp(mnemonic, "orr") ==0 || strcmp(mnemonic, "eon") ==0 || strcmp(mnemonic, "ands") ==0) {
        shifts[2] = "0";
    } else {
        shifts[2] = "1";
    }
    char* result = strcat(strcat(x, strcat(mnemonic, strcat(strcat("01010", shifts), convert(rm, 5)))), strcat(convert(op2, 6), strcat(convert(rn, 5), convert(rd, 5))));
    return result;
}

char* multiply (char* mnemonic, char* rd, char* rn, char* rm, char* ra) {
    char* x;
    if (strcmp((const char *) rd[0], "x") == 0) {
        x = "1";
    } else {
        x = "0";
    }
    char* negate;
    if (strcmp(mnemonic, "madd") == 0) {
        negate = "0";
    } else {
        negate = "1";
    }
    char* result = strcat(strcat(x, strcat("0011011000", convert(rm, 5))), strcat(negate, strcat(convert(ra,5), strcat(convert(rn, 5), convert(rd, 5)))));
    return result;
}




