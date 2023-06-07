//
// Created by Arabella Hurrell on 30/05/2023.
//

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

char* indexedOffset(char* rt, char* xn, char* value, char* l, char* i) {
    return strcat(strcat(strcat("1", sf(rt)), strcat("1110000", l)), strcat(strcat(truncateString(value, 9), i), strcat(strcat("1", convert(xn, 5)), convert(rt, 5))));
}

char* unsignedOffset (char* rt, char* xn, char* value, char* l) {
    return strcat(strcat("0", strcat(sf(rt), "1110010")), strcat(strcat(l, truncateString(value, 12)), strcat(convert(xn, 5), convert(rt, 5))));
}

char* loadLiteral(char* rt, char* value) {
    return strcat(strcat("0", sf(rt)), strcat("011000", strcat(truncateString(value, 19), convert(rt, 5))));
}

char* dataTransferParser (char** splitted, char* l) {
    int length = getStringArrayLength(splitted);
    if( length == 5) {
        return registerOffset(splitted[0], splitted[1], splitted[2], splitted[4], "0", l);
    } else if (length == 3) {
        if ((splitted[2])[strlen(splitted[2]) - 1] == '!' && (splitted[2])[0] == '-') {
            return indexedOffset(splitted[0], splitted[1], splitted[2], l, "1");
        } else if ((splitted[2])[0] != '-') {
            return unsignedOffset(splitted[0], splitted[1], splitted[2], l);
        } else {
            return indexedOffset(splitted[0], splitted[1], splitted[2], l, "0");
        }
    } else {
        return loadLiteral(splitted[0], splitted[1]);
    }
}

char* str (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    return dataTransferParser(splitted, "0");
}

char* ldr (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    return dataTransferParser(splitted, "1");
}