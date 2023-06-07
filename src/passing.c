//
// Created by Arabella Hurrell on 02/06/2023.
//

#include <string.h>
#include "utility.c"
#include "data_processing.c"
#include "branch.c"
#include "single_data_transfer.c"

void writeStringToFile(const char* fileName, const char* str) {
    FILE* file = fopen(fileName, "ab");
    if (file == NULL) {
        printf("Unable to open the file.\n");
        return;
    }
    size_t length = strlen(str);
    fwrite(str, sizeof(char), length, file);
    fclose(file);

    printf("String written to the file successfully.\n");
}

struct passOne {
    char* label;
    char* address;
};


bool isLabel (char* instruction) {
    return instruction[strlen(instruction) - 1] == ':';
}

bool isDirective (char* instruction) {
    return instruction[0] == ':';
}


char* functionSelector(char* mnemonic, char* arguments, char* address)
{
    char** mnemonics = {"add", "adds", "sub", "subs", "cmp", "cmn", "neg", "negs", "and", "ands", "bic", "bics", "eor",
                        "eon", "orr", "orn", "tst", "mvn", "mov", "movn", "movk", "movz", "madd", "msub", "mul", "mneg",
                        "b", "br", "b.eq", "b.ne", "b.ge", "b.lt", "b.gt", "b.le", "b.al", "ldr", "str", "nop"};
    // fun_ptr_arr is an array of function pointers
    char* (*fun_ptr_arr[])(char*, char*) = {add, adds, sub, subs, cmp, cmn, neg,
                                            negs, and, ands, bic , bics, eor, eon,
                                            orr, orn, tst, mvn, mov, movn, movk,
                                            movz, madd, msub, mul, mneg, b ,br,
                                            beq, bne, bge, blt, bgt, ble, bal,
                                            ldr, str, nop};

    for (int i = 0; i < getStringArrayLength(mnemonics); i++) {
        if (strcmp(mnemonics[i], mnemonic) == 0) {
             return (*fun_ptr_arr[i])(arguments, address);
        }
    }


//    unsigned int ch, a = 15, b = 10;
//
//    printf("Enter Choice: 0 for add, 1 for subtract and 2 "
//           "for multiply\n");
//    scanf("%d", &ch);
//
//    if (ch > 2) return 0;
//
//    (*fun_ptr_arr[ch])(a, b);
//
//    return 0;
}


void one_pass(char** instruction, char* name) {

    int capacity = 2;
    int num = 0;
    struct passOne *passone = (struct passOne *) malloc(capacity * sizeof(struct passOne));
    for (int i = 0; i < sizeof(instruction) / sizeof(instruction[0]); i++) {
        if (isLabel(instruction[i])) {
            char **x = strtok(instruction[i], ":");
            struct passOne pass;
            pass.label = x[0];
            pass.address = strcat("#", decimalToHexadecimal(i + 1));
            passone[num] = pass;
            num++;
            if (num == capacity) {
                capacity *= 2;
                struct passOne *newpassone = (struct passOne *) realloc(passone, capacity * sizeof(struct passOne));
                passone = newpassone;
            }
        }
    }
//    int capacity2 = 2;
//    int num2 = 0;

    for (int i = 0; i < getStringArrayLength(instruction); i++) {
        if (isLabel(instruction[i])) {
        }
        else if (isDirective(instruction[i])) {
            char** splitted = splitStringOnWhitespace(instruction[i]);
            writeStringToFile(hexToBinary(splitted[1]));
        }
        else {
            for (int j = 0; j < num; j++) {
                if (isSubstringInString(instruction[i], passone[j].label)) {
                    replaceSubstring(instruction[i], passone[j].label, passone[j].address);
                }
            }
            char** split = splitStringOnFirstSpace(instruction[i]);
            char* result = functionSelector(split[0], split[1], (char*) i);
            writeStringToFile(name, result);
        }
    }
}
