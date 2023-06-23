//
// this contains our code for parsing branches
//


char *branching(char *mnemonic, char *value, int offset) {
    char *encoding;
    if (strcmp(mnemonic, "b") == 0) {
        //char* res = convert(intToString(binaryToDecimal(hexToBinary(value))
        //                                + offset), 26);
        char *res = convert(intToString(offset), 26);
        char *result = malloc(33 * sizeof(char));
        result[0] = '\0';

        // Append the fixed bits for the branch instruction
        strcat(result, "000101");
        // Append the converted branch target address
        strcat(result, res);

        return result;
    } else if (strcmp(mnemonic, "br") == 0) {
        char *result = malloc(33 * sizeof(char));
        result[0] = '\0';
        strcat(result, "1101011000011111000000");
        strcat(result, registerConvert(value));
        strcat(result, "00000");
        return result;
    } else {
        char *result = malloc(33 * sizeof(char));
        result[0] = '\0';

        char *res = convert(intToString(offset), 19);

        if (strcmp(mnemonic, "b.eq") == 0) {
            encoding = "0000";
        } else if (strcmp(mnemonic, "b.ne") == 0) {
            encoding = "0001";
        } else if (strcmp(mnemonic, "b.ge") == 0) {
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
        // Append the fixed bits for the conditional branch instruction
        strcat(result, "01010100");
        // Append the converted branch target address
        strcat(result, res);
        // Append the fixed bit
        strcat(result, "0");
        // Append the branch condition encoding
        strcat(result, encoding);

        return result;
    }
}

char *b(char *arguments, char *address) {
    char **splitted = splitStringOnWhitespace(arguments);
    int offset = calculateOffset(splitted[0], address);
    return branching("b", splitted[0], offset);
}

char *beq(char *arguments, char *address) {
    char **splitted = splitStringOnWhitespace(arguments);
    int offset = calculateOffset(splitted[0], address);
    return branching("b.eq", splitted[0], offset);

}

char *bne(char *arguments, char *address) {
    char **splitted = splitStringOnWhitespace(arguments);
    int offset = calculateOffset(splitted[0], address);
    return branching("b.ne", splitted[0], offset);

}

char *bge(char *arguments, char *address) {
    char **splitted = splitStringOnWhitespace(arguments);
    int offset = calculateOffset(splitted[0], address);
    return branching("b.ge", splitted[0], offset);

}

char *blt(char *arguments, char *address) {
    char **splitted = splitStringOnWhitespace(arguments);
    int offset = calculateOffset(splitted[0], address);
    return branching("b.lt", splitted[0], offset);

}

char *bgt(char *arguments, char *address) {
    char **splitted = splitStringOnWhitespace(arguments);
    int offset = calculateOffset(splitted[0], address);
    return branching("b.gt", splitted[0], offset);

}

char *ble(char *arguments, char *address) {
    char **splitted = splitStringOnWhitespace(arguments);
    int offset = calculateOffset(splitted[0], address);
    return branching("b.le", splitted[0], offset);

}

char *bal(char *arguments, char *address) {
    char **splitted = splitStringOnWhitespace(arguments);
    int offset = calculateOffset(splitted[0], address);
    return branching("b.al", splitted[0], offset);

}

char *br(char *arguments, char *address) {
    char **splitted = splitStringOnWhitespace(arguments);
    return branching("br", splitted[0], 0);
}
