//
// Created by Arabella Hurrell on 02/06/2023.
//

#include "branch.h"
#include <string.h>
#include "denary_to_binary.c"

char* branching (char* mnemonic, char* value) {
    char* encoding = "";
    if (strcmp(mnemonic, "b") == 0) {
        return strcat("000101", value);
    } else if (strcmp(mnemonic, "br") == 0) {
        return strcat("1101011000011111000000", convert(value, 26));
    } else {
        if (strcmp(mnemonic, "b.eq") == 0) {
            encoding = "0000";
        } else if (strcmp(mnemonic, "b.ne") == 0) {
            encoding = "0001";
        } else if (strcmp(mnemonic, "b.ge") == 0 ){
            encoding = "1010";
        } else if (strcmp(mnemonic, "b.lt") == 0) {
            encoding = "1011";
        } else if (strcmp(mnemonic, "b.gt") == 0) {
            encoding = "1100";
        } else if (strcmp(mnemonic, "b.le") == 0) {
            encoding = "1101";
        } else {
            encoding = "1110";
        }
        return strcat(strcat("01010100", value), strcat("0", encoding));
    }
}