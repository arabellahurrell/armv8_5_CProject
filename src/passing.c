//
// Created by Arabella Hurrell on 02/06/2023.
//

#include "passing.h"
#include "symbol_table.c"
#include <string.h>

char** one_pass(char** instruction) {
    HashMap symbol_table;
    initializeHashMap(&symbol_table);
    char* label;
    // first pass
    for (int i = 0; i < sizeof(instruction)/sizeof(instruction[0]); i++) {
        if (instruction[i][strlen(instruction[i]) - 1] == ":") {
            //insert(symbol_table, strncpy(), i + 1)
        }
    }
}