//
// Created by Arabella Hurrell on 30/05/2023.
//

#include "single_data_transfer.h"
#include <string.h>
char* singleData (char* rt, char* xn, char* val, char* shiftAmount) {
    char* x;
    if (strcmp((const char *) rt[0], "x") == 0) {
        x = "1";
    } else {
        x = "0";
    }
    int sizeOfValue = sizeof(val)/(sizeof(val[0]));
    if (val[sizeOfValue -1] == "!") {

    }
}

char* registerOffset ()