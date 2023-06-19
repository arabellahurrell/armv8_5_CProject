//
// Created by Arabella Hurrell on 02/06/2023.
//

//#include "utility.c"

char* branching (char* mnemonic, char* value, int offset) {
    char* encoding;
    if (strcmp(mnemonic, "b") == 0) {
        char* res = convert(intToString(binaryToDecimal(hexToBinary(value)) + offset) , 26);
        char* result = malloc(33 * sizeof(char));
        result[0] = '\0';

        strcat(result, "000101");
        strcat(result, res);

        printf("%s\n", result);
        fflush(stdout);

        return result;
        //return strcat("000101", res);
    } else if (strcmp(mnemonic, "br") == 0) {
        return strcat("1101011000011111000000", convert(value, 26));
    } else {
        char* result = malloc(33 * sizeof(char));
        result[0] = '\0';
        char* res = convert(intToString(binaryToDecimal(value) + offset) , 19);
        if (strcmp(mnemonic, "b.eq") == 0) {
            encoding = "0000";
        } else if (strcmp(mnemonic, "b.ne") == 0) {
            encoding = "0001";
        } else if (strcmp(mnemonic, "b.ge") == 0 ){
            encoding = "1010";
        } else if (strcmp(mnemonic, "b.lt") == 0) {
            encoding = "1011";
        } else if (strcmp(mnemonic, "b.gt") == 0) {
            encoding = "1100";
        } else if (strcmp(mnemonic, "b.le") == 0) {
            encoding = "1101";
        } else {
            encoding = "1110";
        }
        strcat(result, "01010100");
        strcat(result, res);
        strcat(result, "0");
        strcat(result, encoding);

        printf("%s\n", result);
        fflush(stdout);

        return result;
        //return strcat(strcat("01010100", res), strcat("0", encoding));
    }
}

char* b (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    int offset = binaryToDecimal(hexToBinary(address)) - binaryToDecimal(hexToBinary(splitted[0]));
    return branching("b", splitted[0], offset);

}

char* beq (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    int offset = binaryToDecimal(hexToBinary(address)) - binaryToDecimal(hexToBinary(splitted[0]));
    return branching("b.eq", splitted[0], offset);

}

char* bne (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    int offset = binaryToDecimal(hexToBinary(address)) - binaryToDecimal(hexToBinary(splitted[0]));
    return branching("b.ne", splitted[0], offset);

}
char* bge (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    int offset = binaryToDecimal(hexToBinary(address)) - binaryToDecimal(hexToBinary(splitted[0]));
    return branching("b.ge", splitted[0], offset);

}
char* blt (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    int offset = binaryToDecimal(hexToBinary(address)) - binaryToDecimal(hexToBinary(splitted[0]));
    return branching("b.lt", splitted[0], offset);

}
char* bgt (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    int offset = binaryToDecimal(hexToBinary(address)) - binaryToDecimal(hexToBinary(splitted[0]));
    return branching("b.gt", splitted[0], offset);

}
char* ble (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    int offset = binaryToDecimal(hexToBinary(address)) - binaryToDecimal(hexToBinary(splitted[0]));
    return branching("b.le", splitted[0], offset);

}
char* bal (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    int offset = binaryToDecimal(hexToBinary(address)) - binaryToDecimal(hexToBinary(splitted[0]));
    return branching("b.al", splitted[0], offset);

}

char* br (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    return branching("br", splitted[0], 0);
}
