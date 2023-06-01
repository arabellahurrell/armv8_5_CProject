//
// Created by Arabella Hurrell on 01/06/2023.
//

#include "shifts.h"
#include <string.h>
#include <stdlib.h>
char* master(char* value, char* type, char* amount) {
    uint8_t new;
    u_int8_t val = atoi(value);
    u_int8_t am = atoi(amount);
    if(strcmp(type, "lsl") == 0) {
        new = val << am;
    } else if (strcmp(type, "lsr") == 0 || strcmp(type, "asr") == 0) {
        new = val >> am;
    }
    char * result = (char*) &new;
    return result + 2;
}
