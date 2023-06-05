//
// Created by Arabella Hurrell on 05/06/2023.
//

#include "truncate.h"
#include <string.h>
void truncateString(char* str, int length) {
    int strLength = strlen(str);

    if (strLength > length) {
        str[length] = '\0';
    }
}