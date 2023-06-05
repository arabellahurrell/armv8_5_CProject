//
// Created by Arabella Hurrell on 30/05/2023.
//

#include "data_processing.h"
#include <string.h>
#include "utility.c"
#include "shifts.c"



char* shiftBits (char* shiftType) {
    char* shifts[2];
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
    return shifts;
}

char* arithmetics(char* opcode, char* rd, char* rn, char* op2, char* shiftType, char* shiftAmount) {

    return strcat(strcat(sf(rn), strcat(opcode, "100010")), strcat(master(convert(op2,18), shiftType, shiftAmount) + 10, convert(rd, 5)));
}

char* registerArithmetic(char* opcode, char* rd, char* rn, char*rm, char* op2, char* shiftType, char* shiftValue) {
    return strcat(strcat(strcat(strcat(sf(rd), opcode), "01011"), strcat(shiftBits(shiftType), "0")), strcat(strcat(rm, master(op2, shiftType, shiftValue)), strcat(convert(rn,5), convert(rd,5))));
}

char* moveWides(char* opcode, char* rd, char* imm, char* sh, char* shiftType, char* shiftAmount) {
    return strcat(strcat(sf(rd), strcat(opcode, "101")), strcat(strcat(master(convert(imm, 16), shiftType, shiftAmount), sh), convert(rd, 16)));
}

char* logicalBitwise (char* mnemonic, char* rd, char* rn, char* rm, char* op2, char* shiftType) {
    char* shifts = shiftBits(shiftType);
    if (strcmp(mnemonic, "and") ==0 || strcmp(mnemonic, "orr") ==0 || strcmp(mnemonic, "eon") ==0 || strcmp(mnemonic, "ands") ==0) {
        shifts[2] = (char) "0";
    } else {
        shifts[2] = (char) "1";
    }
    char* result = strcat(strcat(sf(rd), strcat(mnemonic, strcat(strcat("01010", shifts), convert(rm, 5)))), strcat(convert(op2, 6), strcat(convert(rn, 5), convert(rd, 5))));
    return result;
}

char* multiply (char* mnemonic, char* rd, char* rn, char* rm, char* ra) {
    char* negate;
    if (strcmp(mnemonic, "madd") == 0) {
        negate = "0";
    } else {
        negate = "1";
    }
    char* result = strcat(strcat(sf(rd), strcat("0011011000", convert(rm, 5))), strcat(negate, strcat(convert(ra,5), strcat(convert(rn, 5), convert(rd, 5)))));
    return result;
}




