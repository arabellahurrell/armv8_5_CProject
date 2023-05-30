//
// Created by Arabella Hurrell on 30/05/2023.
//

#include "function_decoder.h"
#include <stdio.h>
#include <string.h>

void function_decoder(char** arguments) {
    char* mnemonic = arguments[0];
    if (strcmp(mnemonic, "add") == 0) {

    } else if (strcmp(mnemonic, "sub") == 0) {

    } else if (strcmp(mnemonic, "cmp") == 0) {

    } else if (strcmp(mnemonic, "cmn") == 0) {

    } else if (strcmp(mnemonic, "neg") == 0) {

    } else if (strcmp(mnemonic, "and") == 0) {

    } else if (strcmp(mnemonic, "bic") == 0) {

    } else if (strcmp(mnemonic, "eor") == 0) {

    } else if (strcmp(mnemonic, "orr") == 0) {

    } else if (strcmp(mnemonic, "eon") == 0) {

    } else if (strcmp(mnemonic, "orn") == 0) {

    } else if (strcmp(mnemonic, "tst") == 0) {

    } else if (strcmp(mnemonic, "mov") == 0) {

    } else if (strcmp(mnemonic, "mvn") == 0) {

    } else if (strcmp(mnemonic, "mul") == 0) {

    } else if (strcmp(mnemonic, "adds") == 0) {

    } else if (strcmp(mnemonic, "subs") == 0) {

    } else if (strcmp(mnemonic, "negs") == 0) {

    } else if (strcmp(mnemonic, "ands") == 0) {

    } else if (strcmp(mnemonic, "bics") == 0) {

    } else if (strcmp(mnemonic, "movk") == 0) {

    } else if (strcmp(mnemonic, "movn") == 0) {

    } else if (strcmp(mnemonic, "movz") == 0) {

    } else if (strcmp(mnemonic, "madd") == 0) {

    } else if (strcmp(mnemonic, "msub") == 0) {

    } else if (strcmp(mnemonic, "mneg") == 0) {

    } else if (strcmp(mnemonic, "str") == 0) {

    } else if (strcmp(mnemonic, "ldr") == 0) {

    } else if (strcmp(mnemonic, "b") == 0) {

    } else if (strcmp(mnemonic, "br") == 0) {

    } else if (strcmp(mnemonic, "beq") == 0) {

    } else if (strcmp(mnemonic, "bne") == 0) {

    } else if (strcmp(mnemonic, "bge") == 0) {

    } else if (strcmp(mnemonic, "blt") == 0) {

    } else if (strcmp(mnemonic, "bgt") == 0) {

    } else if (strcmp(mnemonic, "ble") == 0) {

    } else if (strcmp(mnemonic, "bal") == 0) {

    } else {
        printf("Error no such instruction");
    }

}