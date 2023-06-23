//
// This file contains the functions and parsers for load and store code
//



char *registerOffset(char *rt, char *xn, char *xm, char *l) {
    char *result = malloc(33 * sizeof(char));
    result[0] = '\0';

    // Concatenate the binary representation for the register offset instruction
    strcat(result, "1");                   // Bit 31: Always 1
    strcat(result, sf(rt));                // Bit 30: Set flags
    strcat(result, "1110000");             // Bits 29-23: Opcode specific bits
    strcat(result, l);                     // Bit 22: Load/store bit
    strcat(result, "1");                   // Bit 21: Register offset bit
    strcat(result, registerConvert(xm));   // Bits 20-16: Register Xm
    strcat(result, "011010");              // Bits 15-10: Opcode specific bits
    strcat(result, registerConvert(xn));   // Bits 9-5: Register Xn
    strcat(result, registerConvert(rt));   // Bits 4-0: Register Rt

    return result;
}

char *indexedOffset(char *rt, char *xn, char *value, char *l, char *i) {
    char *result = malloc(33 * sizeof(char));
    result[0] = '\0';
    int length = strlen(value);
    if (value[length -1] == ']') {
        value[length-1] = '\0';
    }
    char *v = immHexToBinary(value, 9);

    // Concatenate the binary representation for the indexed offset instruction
    strcat(result, "1");                   // Bit 31: Always 1
    strcat(result, sf(rt));                // Bit 30: Set flags
    strcat(result, "1110000");             // Bits 29-23: Opcode specific bits
    strcat(result, l);                     // Bit 22: Load/store bit
    strcat(result, "0");                   // Bit 21: Immediate offset bit
    strcat(result, v);                     // Bits 20-12: Immediate value
    strcat(result, i);                     // Bit 11: Unsigned/signed bit
    strcat(result, "1");                   // Bit 10: Immediate offset bit
    strcat(result, registerConvert(xn));   // Bits 9-5: Register Xn
    strcat(result, registerConvert(rt));   // Bits 4-0: Register Rt

    return result;
}

char *unsignedOffset(char *rt, char *xn, char *value, char *l) {
    char *result = malloc(33 * sizeof(char));
    int length = strlen(value);
    if (value[length -1 ] == ']') {
        value[length-1] = '\0';
    }
    if (rt[0] == 'x') {
        value = intToString(binaryToDecimal(immHexToBinary(value, 12))/8);
    } else {
        value = intToString(binaryToDecimal(immHexToBinary(value, 12))/4);
    }

    result[0] = '\0';
    char *val = immHexToBinary(value, 12);
    // Concatenate the binary representation for the unsigned offset instruction
    strcat(result, "1");               // Bit 31: Always 1
    strcat(result, sf(rt));            // Bit 30: Set flags
    strcat(result, "1110010");         // Bits 29-23: Opcode specific bits
    strcat(result, l);                 // Bit 22: Load/store bit
    strcat(result, val);               // Bits 21-10: Unsigned offset value
    strcat(result, convert(xn, 5));    // Bits 9-5: Register Xn
    strcat(result, convert(rt, 5));    // Bits 4-0: Register Rt

    return result;
}

char *loadLiteral(char *rt, char *value) {
    char *result = malloc(33 * sizeof(char));
    result[0] = '\0';
    // Concatenate the binary representation for the load literal instruction
    strcat(result, "0");                   // Bit 31: Always 0
    strcat(result, sf(rt));                // Bit 30: Set flags
    strcat(result, "011000");              // Bits 29-24: Opcode specific bits
    strcat(result, value);                   // Bits 23-5: Literal value
    strcat(result, registerConvert(rt));   // Bits 4-0: Register Rt

    return result;
}

char *dataTransferParser(char **splitted, char *l) {
    int length = getStringArrayLength(splitted);
    if (length == 3) {
        if ((splitted[2])[strlen(splitted[2]) - 1] == '!') {
            // Handle indexed offset with pre-indexing
            splitted[2][strlen(splitted[2]) - 1] = '\0';
            return indexedOffset(splitted[0], splitted[1], splitted[2], l, "1");
        } else if (splitted[2][0] == 'w' || splitted[2][0] == 'x') {
            // Handle register offset
            return registerOffset(splitted[0], splitted[1], splitted[2], l);
        } else if ((splitted[2])[strlen(splitted[2]) - 1] == ']') {
            // Handle unsigned offset
            return unsignedOffset(splitted[0], splitted[1], splitted[2], l);
        } else {
            // Handle indexed offset without pre-indexing
            return indexedOffset(splitted[0], splitted[1], splitted[2], l, "0");
        }
    } else {
        if ((splitted[1])[strlen(splitted[1]) - 1] == ']') {
            // Handle unsigned offset with zero offset
            return unsignedOffset(splitted[0], splitted[1], "0", l);
        } else {
            // Handle load literal
            return loadLiteral(splitted[0], splitted[1]);
        }
    }
}

char *str(char *arguments, char *address) {
    char **splitted = splitStringOnWhitespace(arguments);
    // Call the data transfer parser with load/store bit set to 0
    return dataTransferParser(splitted, "0");
}

char *ldr(char *arguments, char *address) {
    //address = decimalToHexadecimal(binaryToDecimal(hexToBinary(address)) - 4);
    address = decimalToHexadecimal(binaryToDecimal(hexToBinary(address)));

    char **splitted = splitStringOnWhitespace(arguments);
    if (getStringArrayLength(splitted) == 2 && (splitted[1])[strlen(splitted[1]) - 1] != ']'){
        int offset = calculateOffset(splitted[1], address);
        splitted[1] = convert(intToString(offset), 19);
    }
    // Call the data transfer parser with load/store bit set to 1
    return dataTransferParser(splitted, "1");
}