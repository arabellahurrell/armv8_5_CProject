//
// Created by Arabella Hurrell on 02/06/2023.
//
//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>

void writeStringToFile(char *fileName, const char *str) {
    FILE *file = fopen(fileName, "ab");
    if (file == NULL) {
        printf("Unable to open the file.\n");
        return;
    }
    printf("FINAL RESULT: %s\n", str);
    int x = strtoul(str, NULL, 2);
    fwrite(&x, sizeof(x), 1, file);
    fclose(file);

}

struct passOne {
    char *label;
    char *address;
};


bool isLabel(char *instruction) {
    printf("instruction: %c\n", instruction[strlen(instruction) - 1]);
    return instruction[strlen(instruction) - 1] == ':';
}

bool isDirective(const char *instruction) {
    return instruction[0] == ':';
}


char *functionSelector(char *mnemonic, char *arguments, char *address) {
    char *mnemonics[38] = {"add", "adds", "sub", "subs", "cmp", "cmn", "neg",
                           "negs", "and", "ands", "bic", "bics", "eor",
                           "eon", "orr", "orn", "tst", "mvn", "mov", "movn",
                           "movk", "movz", "madd", "msub", "mul", "mneg",
                           "b", "br", "b.eq", "b.ne", "b.ge", "b.lt", "b.gt",
                           "b.le", "b.al", "ldr", "str", "nop"};
    // fun_ptr_arr is an array of function pointers
    char *
    (*fun_ptr_arr[])(char *, char *) = {add, adds, sub, subs, cmp, cmn, neg,
                                        negs, and, ands, bic, bics, eor, eon,
                                        orr, orn, tst, mvn, mov, movn, movk,
                                        movz, madd, msub, mul, mneg, b, br,
                                        beq, bne, bge, blt, bgt, ble, bal,
                                        ldr, str, nop};

    for (int i = 0; i < getStringArrayLength(mnemonics); i++) {
        if (strcmp(mnemonics[i], mnemonic) == 0) {
            return (*fun_ptr_arr[i])(arguments, address);
        }
    }
    return NULL;
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


void one_pass(char **instruction, char *name) {
//    FILE* file = fopen(name, "wb");
    int capacity = 2;
    int num = 0;
    struct passOne *passone = (struct passOne *) malloc(
            capacity * sizeof(struct passOne));
    for (int i = 0; i < getStringArrayLength(instruction); i++) {
        if (isLabel(instruction[i])) {
            //instruction[i][strlen(instruction[i]) -1] = '\0';
            //char** x = strtok(instruction[i], ":");
            struct passOne pass;
            char* label_name = instruction[i];
            label_name[strlen(label_name) - 1] = '\0'; // get rid of : from label
            pass.label = label_name;
            char *res = decimalToHexadecimal(i);
            printf("res: %s\n", res);
            int length = strlen(res);
            char *address = malloc((length + 3) * sizeof(char));
            address[0] = '\0';
            //strcat(address, "0x");
            pass.address = strcat(address, res);
            passone[num] = pass;
            num++;
            if (num == capacity) {
                capacity *= 2;
                struct passOne *newpassone
                        = (struct passOne *) realloc(passone,
                                                     capacity *
                                                     sizeof(struct passOne));
                passone = newpassone;
            }
        }
    }

    for (int i = 0; i < getStringArrayLength(instruction); i++) {
        if (isLabel(instruction[i])) {
            instruction[i][strlen(instruction[i]) - 1] = '\0';
        } else if (isDirective(instruction[i])) {
            char **splitted = splitStringOnWhitespace(instruction[i]);
            writeStringToFile(name, hexToBinary(splitted[1]));
        } else {
            for (int j = 0; j < num; j++) {
                if (isSubstringInString(instruction[i], passone[j].label)) {
                    instruction[i] = replaceSubstring(instruction[i],
                                                      passone[j].label,
                                                      passone[j].address);
                }
            }
            char **split = splitStringOnFirstSpace(instruction[i]);
            if (strcmp(" ", split[0]) == 0 || split[0][0] == '0') {
            } else {
                char *result = functionSelector(split[0], split[1],
                                                decimalToHexadecimal(i));
                writeStringToFile(name, result);
            }

        }

    }
}
