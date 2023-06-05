//
// Created by Arabella Hurrell on 30/05/2023.
//

#include "single_data_transfer.h"
#include <string.h>
#include "utility.c"
#include "shifts.c"

char* singleData (char* rt, char* xn, char* val, char* shiftAmount) {
    char* x = sf(rt);
    int sizeOfValue = strlen(val);
    if (val[sizeOfValue -1] == "!") {

    }
}

char* registerOffset(char* rt, char* xn, char* xm, char* shiftAmount, char* u, char* l) {
    convert((char*) (binaryToDecimal(master(xm, "lsl", shiftAmount)) + binaryToDecimal(xn)), 19);
    return strcat(strcat(strcat("1", sf(rt)), strcat("1110000", l)), strcat(strcat("1", convert(xm, 5)), strcat("011010", strcat(convert(xn, 5), convert(rt, 5)))));
}

char* indexedOffset(char* rt, char* xn, char* value, char* shiftAmount, char* u, char* l, char* i) {
    return strcat(strcat(strcat("1", sf(rt)), strcat("1110000", l)), strcat(strcat(truncateString(value, 9), i), strcat(strcat("1", convert(xn, 5)), convert(rt, 5))));
}

char* unsignedOffset (char* rt, char* value, char* xn, char* shiftAmount, char* u, char* l) {
    return strcat(strcat("0", strcat(sf(rt), "1110010")), strcat(strcat(l, truncateString(value, 12)), strcat(convert(xn, 5), convert(rt, 5))));
}

char* loadLiteral(char* rt, char* value) {
    return strcat(strcat("0", sf(rt)), strcat("011000", strcat(truncateString(value, 19), convert(rt, 5))));
}