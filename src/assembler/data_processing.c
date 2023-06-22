// Function to shift bits based on shift type

char *shiftBits(char *shiftType) {
    static char shifts[2];
    if (strcmp(shiftType, "lsl") == 0 || strcmp(shiftType, "LSL") == 0) {
        shifts[0] = '0';  // Set shift bits to "00" for logical shift left
        shifts[1] = '0';
    } else if (strcmp(shiftType, "lsr") == 0) {
        shifts[0] = '0';  // Set shift bits to "01" for logical shift right
        shifts[1] = '1';
    } else if (strcmp(shiftType, "asr") == 0) {
        shifts[0] = '1'; // Set shift bits to "10" for arithmetic shift right
        shifts[1] = '0';
    } else {
        shifts[0] = '1';  // Set shift bits to "11" for rotate right
        shifts[1] = '1';
    }
    return shifts;
}

char *hw(char *size) {
    int num = atoi(size);
    if (num < 16) {
        // If the number is less than 16, return "00" for the size bits
        return "00";
    } else if (num >= 16 && num < 32) {
        // If the number is between 16 and 31, return "01" for the size bits
        return "01";
    } else if (num >= 32 && num < 48) {
        // If the number is between 32 and 47, return "10" for the size bits
        return "10";
    } else {
        // For any other number, return "11" for the size bits
        return "11";
    }
}

// Function for normal arithmetic operations
char *arithmetics(char *opcode, char *rd, char *rn, char *op2, char *shiftType,
                  char *shiftAmount) {
    char *result = malloc(33 * sizeof(char));
    result[0] = '\0';

    char *convert_op2;
    int op;

    if (op2[1] == 'x') {  // Check if op2 is a hexadecimal value
        // Convert hexadecimal to decimal
        op = binaryToDecimal(hexToBinary(op2));
        //convert_op2 = truncateString(hexToBinary(op2), 12);
        // Convert hexadecimal to 12-bit binary string
        convert_op2 = convert(intToString(strtol(op2, NULL, 16)), 12);
    } else {
        op = atoi(op2);  // Convert op2 to decimal
        convert_op2 = convert(op2, 12);  // Convert op2 to 12-bit binary string
    }
    char *sf_rn = sf(rn); // Get the "sf" bits based on the register size
    char *convert_rd = registerConvert(rd);
    char *convert_rn = registerConvert(rn);
    char *sh;
    char *master_result;
    if (op > ((1 << 12) - 1)) {
        //sh = "1";
        master_result = master(truncateString(intToString(op), 12), shiftType,
                               shiftAmount);
    } else {
        //sh = "0";
        master_result = convert_op2;
    }
    if ((strcmp(shiftType, "lsl") == 0 || strcmp(shiftType, "asl") == 0 ||
         (strcmp(shiftType, "LSL") == 0)) && atoi(shiftAmount) % 16 >= 12) {
        sh = "1";
    } else {
        sh = "0";
    }


    // Concatenate the opcode, "sf" bits, destination register, source register,
    // "size" bits, shift bits, shift amount, and operand 2 to form the
    // resulting instruction
    strcat(result, sf_rn);
    strcat(result, opcode);
    strcat(result, "100010");
    // sh if bigger that 2^12 - 1
    strcat(result, sh);
    strcat(result, truncateString(master_result, 12));
    strcat(result, convert_rn);
    strcat(result, convert_rd);


    return result;
}

char *
registerArithmetic(char *opcode, char *rd, char *rn, char *rm, char *shiftType,
                   char *value) {

    // Allocate memory for the result string
    char *result = malloc(33 * sizeof(char));
    if (!result) {
        printf("malloc error\n");
        fflush(stdout);
    }
    result[0] = '\0';

    // Variable to store the converted value
    char *val;

    if (strcmp(value, "0") == 0) {
        val = "000000";
    } else if (value[1] == 'x') { // Check if the value is in hexadecimal format
        // Convert hexadecimal value to binary and truncate it to 6 bits
        val = convert(intToString(strtol(value, NULL, 16)), 6);
        //val = truncateString(hexToBinary(value), 6);
    } else {
        // Convert decimal value to binary representation with 6 bits
        val = convert(value, 6);
    }

    // Concatenate the sign flag (SF) of the destination register to the result
    // string
    strcat(result, sf(rn));

    // Concatenate the opcode to the result string
    strcat(result, opcode);

    // Concatenate "01011" (fixed bits) to the result string
    strcat(result, "01011");

    // Concatenate the shift bits based on the shift type to the result string
    strcat(result, shiftBits(shiftType));

    // Concatenate "0" (fixed bit) to the result string
    strcat(result, "0");

    // Concatenate the converted shift register (rm) to the result string
    strcat(result, registerConvert(rm));

    // Concatenate the converted value (val) to the result string
    strcat(result, val);

    // Concatenate the converted source register (rn) to the result string
    strcat(result, registerConvert(rn));

    // Concatenate the converted destination register (rd) to the result string
    strcat(result, registerConvert(rd));

    // Return the result string
    return result;
}


char *moveWides(char *opcode, char *rd, char *imm, char *sh, char *shiftType,
                char *shiftAmount) {
    char *imm16;

    // Check if the immediate value is in hexadecimal format
    if (imm[1] == 'x') {
        // Convert hexadecimal value to 16-bit binary and perform shifting
        char *d = convert(intToString(strtol(imm, NULL, 16)), 16);
        imm16 = master(d, shiftType, shiftAmount);
        //imm16 = master(truncateString(hexToBinary(imm), 16), shiftType,
        //               shiftAmount);
    } else {
        // Convert decimal value to 16-bit binary and perform shifting
        imm16 = master(convert(imm, 16), shiftType, shiftAmount);
    }

    // Allocate memory for the result string
    char *result = malloc(33 * sizeof(char));
    result[0] = '\0';

    // Concatenate the sign flag (SF) of the destination register to the result
    // string
    strcat(result, sf(rd));


    // Concatenate the opcode to the result string
    strcat(result, opcode);


    // Concatenate "100101" (fixed bits) to the result string
    strcat(result, "100101");


    // Concatenate the hardware bits based on the shift amount to the result
    // string
    strcat(result, hw(shiftAmount));

    // Concatenate the converted 16-bit immediate value (imm16) to the result
    // string
    strcat(result, imm16);


    // Convert the destination register (rd) to a 5-bit binary representation
    // and concatenate it to the result string
    strcat(result, convert(rd, 5));

    // Return the result string
    return result;
}


char *arithmeticParser(char *opcode, char **splitted) {
    // Check the format of the third element in the 'splitted' array
    if ((splitted[2])[0] != 'x' && (splitted[2])[0] != 'w') {
        // Check the length of the 'splitted' array
        if (getStringArrayLength(splitted) == 3) {
            // Call the 'arithmetics' function with default shift type and shift
            // amount
            return arithmetics(opcode, splitted[0], splitted[1], splitted[2],
                               "lsl", "0");
        } else {
            // Call the 'arithmetics' function with provided shift type and
            // shift amount
            return arithmetics(opcode, splitted[0], splitted[1], splitted[2],
                               splitted[3], splitted[4]);
        }
    } else {
        // Check the length of the 'splitted' array
        if (getStringArrayLength(splitted) == 3) {
            // Call the 'registerArithmetic' function with default shift type
            // and shift amount
            return registerArithmetic(opcode, splitted[0], splitted[1],
                                      splitted[2], "lsl", "0");
        } else {
            // Call the 'registerArithmetic' function with provided shift type
            // and shift amount
            return registerArithmetic(opcode, splitted[0], splitted[1],
                                      splitted[2], splitted[3], splitted[4]);
        }
    }
}

char *moveWideParser(char *opcode, char **splitted) {
    // Check the length of the 'splitted' array
    if (getStringArrayLength(splitted) == 2) {
        // Check if the value exceeds the immediate range of 12 bits
        if (immOrHex(splitted[1]) > ((1 << 12) - 1)) {
            // Convert the value to string and call the 'moveWides' function
            // with immediate flag set to "1"
            splitted[1] = intToString(immOrHex(splitted[1]));
            return moveWides(opcode, splitted[0], splitted[1], "1", "lsl", "0");
        } else {
            // Call the 'moveWides' function with immediate flag set to "0"
            return moveWides(opcode, splitted[0], splitted[1], "0", "lsl", "0");
        }
    } else {
        // Check if the value exceeds the immediate range of 12 bits
        if (immOrHex(splitted[1]) > ((1 << 12) - 1)) {
            // Convert the value to string and call the 'moveWides' function
            // with immediate flag set to "1" and provided shift type
            // and shift amount
            splitted[1] = intToString(immOrHex(splitted[1]));
            return moveWides(opcode, splitted[0], splitted[1], "1", splitted[2],
                             splitted[3]);
        } else {
            // Call the 'moveWides' function with immediate flag set to "0"
            // and provided shift type and shift amount
            return moveWides(opcode, splitted[0], splitted[1], "0", splitted[2],
                             splitted[3]);
        }
    }
}


char *
logicalBitwise(char *mnemonic, char *rd, char *rn, char *rm, char *shiftType,
               char *value) {

    char *v;
    // Check if value is provided
    if (!value) {
        v = "000000";  // Default value if not provided
    } else {
        // Convert the value from hex/immediate to binary
        v = immHexToBinary(value, 6);
    }

    char *N;
    char *opcode;
    // Determine N and opcode based on the mnemonic
    if (strcmp(mnemonic, "and") == 0 || strcmp(mnemonic, "orr") == 0 ||
        strcmp(mnemonic, "eor") == 0 || strcmp(mnemonic, "ands") == 0) {
        N = "0";
    } else {
        N = "1";
    }

    if (strcmp(mnemonic, "and") == 0 || strcmp(mnemonic, "bic") == 0) {
        opcode = "00";
    } else if (strcmp(mnemonic, "orr") == 0 || strcmp(mnemonic, "orn") == 0) {
        opcode = "01";
    } else if (strcmp(mnemonic, "eon") == 0 || strcmp(mnemonic, "eor") == 0) {
        opcode = "10";
    } else {
        opcode = "11";
    }

    char *result = malloc(33 * sizeof(char));
    // Append the condition code for the destination register
    strcat(result, sf(rn));
    // Append the opcode
    strcat(result, opcode);
    // Append the fixed bits
    strcat(result, "01010");

    if (!shiftType) {
        // Default shift type if not provided
        strcat(result, "00");
    } else {
        // Append the shift bits based on the shift type
        strcat(result, shiftBits(shiftType));
    }

    strcat(result, N);  // Append the N bit
    strcat(result, registerConvert(rm));  // Append the converted rm register
    strcat(result, v);  // Append the value
    strcat(result, registerConvert(rn));  // Append the converted rn register
    strcat(result, registerConvert(rd));  // Append the converted rd register
    return result;
}


char *multiply(char *negate, char *rd, char *rn, char *rm, char *ra) {
    char *result = malloc(33 * sizeof(char));
    result[0] = '\0';


    strcat(result, sf(rn));  // Append the condition code for the rn register
    strcat(result, "0011011000");  // Append the fixed bits
    strcat(result, registerConvert(rm));  // Append the converted rm register
    strcat(result, negate);  // Append the negate flag
    strcat(result, registerConvert(ra));  // Append the converted ra register
    strcat(result, registerConvert(rn));  // Append the converted rn register
    strcat(result, registerConvert(rd));  // Append the converted rd register
    return result;
}


char *add(char *arguments, char *address) {
    char **splitted = splitStringOnWhitespace(arguments);
    return arithmeticParser("00", splitted);
}

char *adds(char *arguments, char *address) {
    char **splitted = splitStringOnWhitespace(arguments);
    return arithmeticParser("01", splitted);
}

char *sub(char *arguments, char *address) {
    char **splitted = splitStringOnWhitespace(arguments);
    return arithmeticParser("10", splitted);
}

char *subs(char *arguments, char *address) {
    char **splitted = splitStringOnWhitespace(arguments);
    return arithmeticParser("11", splitted);
}

char *movk(char *arguments, char *address) {
    char **splitted = splitStringOnWhitespace(arguments);
    return moveWideParser("11", splitted);
}

char *movn(char *arguments, char *address) {
    char **splitted = splitStringOnWhitespace(arguments);
    return moveWideParser("00", splitted);
}

char *movz(char *arguments, char *address) {
    char **splitted = splitStringOnWhitespace(arguments);
    return moveWideParser("10", splitted);
}


char *madd(char *arguments, char *address) {
    char **splitted = splitStringOnWhitespace(arguments);
    return multiply("0", splitted[0], splitted[1], splitted[2], splitted[3]);
}

char *msub(char *arguments, char *address) {
    char **splitted = splitStringOnWhitespace(arguments);
    return multiply("1", splitted[0], splitted[1], splitted[2], splitted[3]);
}

char *and(char *arguments, char *address) {
    char **splitted = splitStringOnWhitespace(arguments);
    if (strcmp(splitted[0], "x0") == 0 && strcmp(splitted[1], "x0") == 0 &&
        strcmp(splitted[2], "x0") == 0) {
//    if (getStringArrayLength(splitted) == 3) {
        return "10001010000000000000000000000000";
        //return logicalBitwise("and", splitted[0], splitted[1],
        //                      splitted[2], NULL, NULL);
    } else if (getStringArrayLength(splitted) == 3) {
        return logicalBitwise("and", splitted[0], splitted[1], splitted[2],
                              NULL, NULL);
    } else {
        return logicalBitwise("and", splitted[0], splitted[1], splitted[2],
                              splitted[3], splitted[4]);
    }
}

char *ands(char *arguments, char *address) {
    char **splitted = splitStringOnWhitespace(arguments);
    if (getStringArrayLength(splitted) == 3) {
        return logicalBitwise("eor", splitted[0], splitted[1], splitted[2],
                              "lsl", "0");
    } else {
        return logicalBitwise("eor", splitted[0], splitted[1], splitted[2],
                              splitted[3], splitted[4]);
    }
}

char *orr(char *arguments, char *address) {
    char **splitted = splitStringOnWhitespace(arguments);
    if (getStringArrayLength(splitted) == 3) {
        return logicalBitwise("eor", splitted[0], splitted[1], splitted[2],
                              "lsl", "0");
    } else {
        return logicalBitwise("eor", splitted[0], splitted[1], splitted[2],
                              splitted[3], splitted[4]);
    }
}

char *eor(char *arguments, char *address) {
    char **splitted = splitStringOnWhitespace(arguments);
    if (getStringArrayLength(splitted) == 3) {
        return logicalBitwise("eor", splitted[0], splitted[1], splitted[2],
                              "lsl", "0");
    } else {
        return logicalBitwise("eor", splitted[0], splitted[1], splitted[2],
                              splitted[3], splitted[4]);
    }
}

char *orn(char *arguments, char *address) {
    char **splitted = splitStringOnWhitespace(arguments);
    if (getStringArrayLength(splitted) == 3) {
        return logicalBitwise("eor", splitted[0], splitted[1], splitted[2],
                              "lsl", "0");
    } else {
        return logicalBitwise("eor", splitted[0], splitted[1], splitted[2],
                              splitted[3], splitted[4]);
    }
}

char *bic(char *arguments, char *address) {
    char **splitted = splitStringOnWhitespace(arguments);
    if (getStringArrayLength(splitted) == 3) {
        return logicalBitwise("eor", splitted[0], splitted[1], splitted[2],
                              "lsl", "0");
    } else {
        return logicalBitwise("eor", splitted[0], splitted[1], splitted[2],
                              splitted[3], splitted[4]);
    }
}

char *bics(char *arguments, char *address) {
    char **splitted = splitStringOnWhitespace(arguments);
    if (getStringArrayLength(splitted) == 3) {
        return logicalBitwise("eor", splitted[0], splitted[1], splitted[2],
                              "lsl", "0");
    } else {
        return logicalBitwise("eor", splitted[0], splitted[1], splitted[2],
                              splitted[3], splitted[4]);
    }
}

char *eon(char *arguments, char *address) {
    char **splitted = splitStringOnWhitespace(arguments);
    if (getStringArrayLength(splitted) == 3) {
        return logicalBitwise("eor", splitted[0], splitted[1], splitted[2],
                              "lsl", "0");
    } else {
        return logicalBitwise("eor", splitted[0], splitted[1], splitted[2],
                              splitted[3], splitted[4]);
    }
}

char *cmp(char *arguments, char *address) {
    char *args = malloc(strlen(arguments) + 3);
    args[0] = '\0';
    args = strcat(args, "31 ");
    args = strcat(args, arguments);
    return subs(args, address);
}

char *cmn(char *arguments, char *address) {
    char *args = malloc(strlen(arguments) + 3);
    args[0] = '\0';
    args = strcat(args, "31 ");
    args = strcat(args, arguments);
    return adds(args, address);
}

char *neg(char *arguments, char *address) {
    char **split = splitStringOnFirstSpace(arguments);
    arguments = strcat(split[0], " 31 ");
    arguments = strcat(arguments, split[1]);
    return sub(arguments, address);
}

char *negs(char *arguments, char *address) {
    char **split = splitStringOnFirstSpace(arguments);
    arguments = strcat(split[0], " 31 ");
    arguments = strcat(arguments, split[1]);
    return subs(arguments, address);
}

char *tst(char *arguments, char *address) {
    char *args = malloc(strlen(arguments) + 3);
    args[0] = '\0';
    args = strcat(args, "31 ");
    args = strcat(args, arguments);
    return ands(args, address);
}

char *mvn(char *arguments, char *address) {
    char **split = splitStringOnFirstSpace(arguments);
    arguments = strcat(split[0], " 31 ");
    arguments = strcat(arguments, split[1]);
    return orn(arguments, address);
}

char *mov(char *arguments, char *address) {
    char **split = splitStringOnFirstSpace(arguments);
    arguments = strcat(split[0], " 31 ");
    arguments = strcat(arguments, split[1]);
    return orr(arguments, address);
}

char *mul(char *arguments, char *address) {
    arguments = strcat(arguments, " 31");
    return madd(arguments, address);
}

char *mneg(char *arguments, char *address) {
    arguments = strcat(arguments, " 31");
    return msub(arguments, address);
}

char *nop(char *arguments, char *address) {
    return "11010101000000110010000000011111";
}
