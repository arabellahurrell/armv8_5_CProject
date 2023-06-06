//
// Created by Arabella Hurrell on 02/06/2023.
//

#include "passing.h"
#include "symbol_table.c"
#include <string.h>
#include "utility.c"

struct passOne {
    char* label;
    char* address;
};

struct mnemonic {
    char* mnemonic;
    char* arguments;
};


char** one_pass(char** instruction) {

    int capacity = 2;
    int num = 0;
    struct passOne *passone = (struct passOne *) malloc(capacity * sizeof(struct passOne));
    for (int i = 0; i < sizeof(instruction) / sizeof(instruction[0]); i++) {
        if (instruction[i][strlen(instruction[i]) - 1] == ":") {
            char **x = strtok(instruction[i], ":");
            struct passOne pass;
            pass.label = x[0];
            pass.address = strcat("#", (char *) ((char *) (i + 1))));
            passone[num] = pass;
            num++;
            if (num == capacity) {
                capacity *= 2;
                struct passOne *newpassone = (struct passOne *) realloc(passone, capacity * sizeof(struct passOne));
                passone = newpassone;
            }
        }
    }
    int capacity2 = 2;
    int num2 = 0;
    for (int i = 0; i < sizeof(instruction) / sizeof(instruction[0]); i++) {
        for (int j = 0; j < num; j++) {
            if (isSubstringInString(instruction[i], passone[j].label)) {
                replaceSubstring(instruction[i], passone[j].label, passone[j].address);
            }
        }
        char *firstPart[100];
        char *secondPart[100];
        char *whitespace = strchr(instruction[i], ' ');
        if (whitespace != NULL) {
            strncpy(firstPart, instruction[i], whitespace - instruction[i]);
            firstPart[whitespace - instruction[i]] = '\0';
            strcpy(secondPart, whitespace + 1);
        } else {
            strcpy(firstPart, instruction[i]);
            strcpy(secondPart, "");
        }
        struct mnemonic *mnemonics = (struct passOne *) malloc(capacity2 * sizeof(struct mnemonic));
        struct mnemonic m;
        m.mnemonic = firstPart;
        m.arguments = secondPart;
        mnemonics[num2] = m;
        num2++;
        if (capacity2 == num2) {
            capacity2 *=2;
            struct mnemonic *newmnemonic = (struct mnemonic *) realloc(mnemonics, capacity2 * sizeof(struct mnemonic));
            passone = newmnemonic;
        }

    }
}
//char** two_pass()

//    HashMap symbol_table;
//    initializeHashMap(&symbol_table);
//    char* label;
//    // first pass
//    for (int i = 0; i < sizeof(instruction)/sizeof(instruction[0]); i++) {
//        if (instruction[i][strlen(instruction[i]) - 1] == ":") {
//            char** x = strtok(instruction[i],":");
//            insert(&symbol_table, x[0] , i + 1);
//        }
//    }