//
// This file contains the passing algorithms for the program (two pass) as well as our write to file function
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
    int x = strtoul(str, NULL, 2);
    printf("decimal result: %d\n", x);
    fwrite(&x, sizeof(x), 1, file);
    fclose(file);

}

struct passOne {
    char *label;
    char *address;
};


bool isLabel(char *instruction) {
    return instruction[strlen(instruction) - 1] == ':';
}

bool isDirective(const char *instruction) {
    return instruction[0] == '.';
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
}


void pass(char **instruction, char *name) {
    int capacity = 2;
    int num = 0;
    struct passOne *passone = (struct passOne *) malloc(
            capacity * sizeof(struct passOne));
    int line_counter = 0;
    for (int i = 0; i < getStringArrayLength(instruction); i++) {
        if (isLabel(instruction[i])) {
            struct passOne pass;
            char* label_name = instruction[i];
            label_name[strlen(label_name) - 1] = '\0'; // get rid of : from label
            pass.label = label_name;
            char *res = decimalToHexadecimal(4 * line_counter);
            int length = strlen(res);
            char *address = malloc((length + 3) * sizeof(char));
            address[0] = '\0';
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
        } else {
            line_counter += 1;
        }
    }

    line_counter = 0;
    for (int i = 0; i < getStringArrayLength(instruction); i++) {
        if (isLabel(instruction[i])) {
            instruction[i][strlen(instruction[i]) - 1] = '\0';
        } else if (isDirective(instruction[i])) {
            char **splitted = splitStringOnWhitespace(instruction[i]);
            writeStringToFile(name, immHexToBinary(splitted[1],32));
            line_counter += 1;
        } else {
            for (int j = 0; j < num; j++) {
                instruction[i] = replaceWord(instruction[i], passone[j].label, passone[j].address);
            }
            char **split = splitStringOnFirstSpace(instruction[i]);

            if (strcmp(" ", split[0]) == 0 || split[0][0] == '0') {

            } else {
                printf("instruction: %s\n", instruction[i]);
                char *result = functionSelector(split[0], split[1],
                                                decimalToHexadecimal(4 * (line_counter)));
                printf("result: %s\n", result);
                writeStringToFile(name, result);
                line_counter += 1;
            }
           // line_counter += 1;
        }

    }
}
